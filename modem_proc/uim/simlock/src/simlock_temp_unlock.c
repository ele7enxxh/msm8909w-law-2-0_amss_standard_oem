/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            S I M   L O C K   T E M P   U N L O C K   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the simlock temporary unlock functions.



                        COPYRIGHT INFORMATION

Copyright (c) 2015 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/simlock/src/simlock_temp_unlock.c#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/08/16   bcho    Added support for ICCID simlock
06/17/16   vdc     Added support for SPN based SIMLOCK feature
01/06/16   stv     Remote simlock support
12/31/15   stv     Send events to client from engine msg handling
08/12/15   stv     Clear temp unlock data when temp unlock expires
08/10/15   stv     Remove temporary unlock end condition where an end is not sent
07/06/15   stv     Clear temp unlock data when a permanent unlock/reset simlock req
06/29/15   stv     Start time in temp unlock req as current time
                   Graceful handling of PMIC reset during temp unlock
06/11/15   stv     Success for temp unlock req when no time is available
05/17/15   stv     Fix compiler warnings
05/15/15   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"

#ifdef FEATURE_SIMLOCK
#include "simlock.h"
#include "simlock_timer.h"
#include "simlock_util.h"

#if defined(FEATURE_SIMLOCK_RSU) || defined(FEATURE_SIMLOCK_QC_REMOTE_LOCK)
#include "simlock_file.h"
#include "simlock_efs.h"
#include "simlock_modem.h"
#include "simlock_temp_unlock.h"
#include "simlock_time.h"
#include "simlocklib.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define SIMLOCK_TEMP_UNLOCK_FILE_VERSION                       1
#define SIMLOCK_TEMP_UNLOCK_TIME_FILE_SIZE                    13

#define SIMLOCK_TEMP_UNLOCK_DURATION_IN_MINUTES(x)            (x/60)

/* timer set when the temporary unlock start is at a later time */
static rex_timer_type    * simlock_temp_unlock_pending_timer_ptr  = NULL;

/* prototype for registration with time module */
static void simlock_temp_unlock_process_new_time(simlock_time_info_type  time);

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/

/* ----------------------------------------------------------------------------
   STRUCTURE:      SIMLOCK_TEMP_UNLOCK_TIME_DATA_TYPE

   DESCRIPTION:
     This structure contains the time data related to the last processed
     temporary unlock request. The times are in seconds

     version             : File version
     last_start_time     : Start time of the last processed temporary unlock
     last_duration       : Duration of the last processed temporary unlock
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST {
  uint8     version;
  uint64    last_start_time;
  uint32    last_duration;
} simlock_temp_unlock_time_data_type;

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION SIMLOCK_TEMP_UNLOCK_READ_TIME_FILE

DESCRIPTION
  This function reads the temporary unlock time file and provides the
  last times data

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_temp_unlock_read_time_file
(
  simlock_temp_unlock_time_data_type    * time_data_ptr
)
{
  simlock_result_enum_type    simlock_status     = SIMLOCK_SUCCESS;
  uint32                      time_file_size     = 0;
  uint8                     * time_file_data_ptr = NULL;

  if(time_data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  do
  {
    simlock_status = simlock_file_read(&time_file_data_ptr,
                                       &time_file_size,
                                       SIMLOCK_FILE_TEMP_UNLOCK,
                                       SIMLOCK_SFS_LOCAL);
    if((simlock_status != SIMLOCK_SUCCESS) ||
       (time_file_data_ptr == NULL))
    {
      simlock_status = SIMLOCK_GENERIC_ERROR;
      break;
    }

     /* if the file size is not of the expected size,
       then it is not valid */
    if(time_file_size != SIMLOCK_TEMP_UNLOCK_TIME_FILE_SIZE)
    {
      simlock_status = SIMLOCK_GENERIC_ERROR;
      break;
    }

    simlock_memscpy((uint8 *)time_data_ptr,
                    sizeof(simlock_temp_unlock_time_data_type),
                    time_file_data_ptr,
                    time_file_size);

    /* File version check */
    if(time_data_ptr->version != SIMLOCK_TEMP_UNLOCK_FILE_VERSION)
    {
      simlock_status = SIMLOCK_GENERIC_ERROR;
      break;
    }
  }while (0);

  SIMLOCK_MEM_FREE(time_file_data_ptr);
  return simlock_status;
} /* simlock_temp_unlock_read_time_file */


/*===========================================================================
FUNCTION SIMLOCK_TEMP_UNLOCK_WRITE_TIME_FILE

DESCRIPTION
  This function builds the temporary unlock time data and writes to a file.
  Version is to always written correctly to this file.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_temp_unlock_write_time_file
(
  simlock_temp_unlock_time_data_type  * time_data_ptr
)
{
  if(time_data_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  return simlock_file_write((uint8 *)time_data_ptr,
                            sizeof(simlock_temp_unlock_time_data_type),
                            SIMLOCK_FILE_TEMP_UNLOCK,
                            SIMLOCK_SFS_LOCAL);
} /* simlock_temp_unlock_write_time_file */


/*===========================================================================
FUNCTION SIMLOCK_TEMP_UNLOCK_GET_LOCK_STATUS

DESCRIPTION
  This function gets the status of the locks (enabled or disabled)

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_temp_unlock_get_lock_status
(
  simlock_message_response_data_type  * lock_status_resp_ptr
)
{
  simlock_result_enum_type              simlock_status       = SIMLOCK_SUCCESS;
  simlock_message_request_data_type   * lock_status_req_ptr  = NULL;

  if(lock_status_resp_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(lock_status_req_ptr,
                               sizeof(simlock_message_request_data_type));
  if (lock_status_req_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  lock_status_req_ptr->msg_type  = SIMLOCK_GET_STATUS_MSG;

  /* call the engine with get status request */
  simlock_status = simlock_process_message(lock_status_req_ptr, lock_status_resp_ptr);

  SIMLOCK_MEM_FREE(lock_status_req_ptr);

  return simlock_status;
} /* simlock_temp_unlock_get_lock_status */


/*===========================================================================
FUNCTION SIMLOCK_TEMP_UNLOCK_PENDING_TIMER_EXPIRY

DESCRIPTION
  This function handles the temp unlock pending timer expiry

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
static void simlock_temp_unlock_pending_timer_expiry
(
  unsigned long input
)
{
  simlock_result_enum_type     simlock_status = SIMLOCK_SUCCESS;
  simlock_task_cmd_type      * task_cmd_ptr   = NULL;

  (void)input;

  SIMLOCK_CHECK_AND_MEM_MALLOC(task_cmd_ptr,
                               sizeof(simlock_task_cmd_type));
  if (task_cmd_ptr == NULL)
  {
    return;
  }

  task_cmd_ptr->cmd_type  = SIMLOCK_TEMPORARY_UNLOCK_START_CMD;

  /* Put on SIM Lock command queue */
  simlock_status = simlock_queue_cmd(task_cmd_ptr);
  if (simlock_status != SIMLOCK_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    SIMLOCK_MEM_FREE(task_cmd_ptr);
  }

  SIMLOCK_MSG_MED_1("queue of pending timer expiry command, status 0x%x",
                    simlock_status);
} /* simlock_temp_unlock_pending_timer_expiry */


/*===========================================================================
FUNCTION SIMLOCK_TEMP_UNLOCK_STOP_PENDING_TIMER

DESCRIPTION
  This function stops the timer for a pending temporary unlock request

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static void simlock_temp_unlock_stop_pending_timer
(
  void
)
{
  /* if timer doesn't exist, return error */
  if(simlock_temp_unlock_pending_timer_ptr == NULL)
  {
    return;
  }

  /* stop the timer */
  (void)rex_clr_timer(simlock_temp_unlock_pending_timer_ptr);

  /* deallocate the timer */
  rex_delete_timer_ex(simlock_temp_unlock_pending_timer_ptr);

  /* mark the timer as inactive */
  simlock_temp_unlock_pending_timer_ptr = NULL;
}/* simlock_temp_unlock_stop_pending_timer */


/*===========================================================================
FUNCTION SIMLOCK_TEMP_UNLOCK_START_PENDING_TIMER

DESCRIPTION
  This function starts the timer for a pending temporary unlock request

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_temp_unlock_start_pending_timer
(
  simlock_time_type   pending_time
)
{
  rex_timer_cnt_type     timer_value     = 0;

  /* stop the timer, in case it already exists */
  (void)simlock_temp_unlock_stop_pending_timer();

  simlock_temp_unlock_pending_timer_ptr =
                  rex_create_timer_ex(simlock_temp_unlock_pending_timer_expiry, 0);

  if(simlock_temp_unlock_pending_timer_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  /* convert to milli seconds */
  timer_value = (rex_timer_cnt_type)(pending_time * SIMLOCK_MILLISECONDS_IN_A_SECOND);

  /* start the timer */
  (void)rex_set_timer(simlock_temp_unlock_pending_timer_ptr, timer_value);

  return SIMLOCK_SUCCESS;
} /* simlock_temp_unlock_start_pending_timer */


/*===========================================================================
FUNCTION SIMLOCK_TEMP_UNLOCK_UNSET

DESCRIPTION
  This function sends the request to unset any existing temporary unlocks,
  that are currently active.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_temp_unlock_unset
(
  simlock_slot_enum_type       slot,
  simlock_category_enum_type   category
)
{
  simlock_result_enum_type              simlock_status        = SIMLOCK_SUCCESS;
  simlock_message_request_data_type   * temp_unlock_req_ptr   = NULL;

  SIMLOCK_CHECK_AND_MEM_MALLOC(temp_unlock_req_ptr,
                               sizeof(simlock_message_request_data_type));
  if (temp_unlock_req_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  temp_unlock_req_ptr->msg_type                                            = SIMLOCK_TEMPORARY_UNLOCK_MSG;
  temp_unlock_req_ptr->message.temporary_unlock_msg.temporary_unlock_type  = SIMLOCK_TEMPORARY_UNLOCK_UNSET;
  temp_unlock_req_ptr->message.temporary_unlock_msg.slot                   = slot;
  temp_unlock_req_ptr->message.temporary_unlock_msg.category               = category;
  temp_unlock_req_ptr->message.temporary_unlock_msg.duration               = 0;

  /* call the engine with temporary unlock unset request */
  simlock_status = simlock_process_message(temp_unlock_req_ptr, NULL);

  SIMLOCK_MEM_FREE(temp_unlock_req_ptr);
  return simlock_status;
} /* simlock_temp_unlock_unset */


/*===========================================================================
FUNCTION SIMLOCK_TEMP_UNLOCK_SET

DESCRIPTION
  This function sets the temporary unlock for all the categories that are
  currently locked

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_temp_unlock_set
(
  simlock_slot_enum_type       slot,
  simlock_category_enum_type   category,
  simlock_time_type            duration
)
{
  simlock_result_enum_type              simlock_status        = SIMLOCK_SUCCESS;
  simlock_message_request_data_type   * temp_unlock_req_ptr   = NULL;

  SIMLOCK_CHECK_AND_MEM_MALLOC(temp_unlock_req_ptr,
                               sizeof(simlock_message_request_data_type));
  if (temp_unlock_req_ptr == NULL)
  {
    return SIMLOCK_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  temp_unlock_req_ptr->msg_type                                            = SIMLOCK_TEMPORARY_UNLOCK_MSG;
  temp_unlock_req_ptr->message.temporary_unlock_msg.temporary_unlock_type  = SIMLOCK_TEMPORARY_UNLOCK_SET;
  temp_unlock_req_ptr->message.temporary_unlock_msg.slot                   = slot;
  temp_unlock_req_ptr->message.temporary_unlock_msg.category               = category;
  temp_unlock_req_ptr->message.temporary_unlock_msg.duration               = SIMLOCK_TEMP_UNLOCK_DURATION_IN_MINUTES(duration);

  /* call the engine with temporary unlock set request */
  simlock_status = simlock_process_message(temp_unlock_req_ptr, NULL);

  SIMLOCK_MEM_FREE(temp_unlock_req_ptr);
  return simlock_status;
} /* simlock_temp_unlock_set */


/*===========================================================================
FUNCTION SIMLOCK_TEMP_UNLOCK_COMPARE_TIME_AND_SET

DESCRIPTION
  This function compares the current time and the start time of the temporary
  unlock and sets the appropriate timer

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_temp_unlock_compare_time_and_set
(
  simlock_slot_enum_type       slot,
  simlock_category_enum_type   category,
  simlock_time_type            current_time,
  simlock_time_type            start_time,
  simlock_time_type            duration
)
{
  simlock_result_enum_type     simlock_status  = SIMLOCK_SUCCESS;
  simlock_time_type            pending_time    = 0;

  SIMLOCK_MSG_MED_3("current_time: 0x%x, start_time: 0x%x, duration: 0x%x",
                    current_time, start_time, duration);

  if((start_time <= current_time) &&
     (start_time + duration > current_time))
  {
    duration = (duration - (current_time - start_time));
    simlock_status = simlock_temp_unlock_set(slot, category, duration);
  }
  else if ((start_time < current_time) &&
           (start_time + duration <= current_time))
  {
    simlock_status = simlock_temp_unlock_unset(slot, category);
    /* Check if temporary unlock timers are stopped.
       If yes, clear the temporary unlock details from SFS so that the temporary unlock would not be re-applied
       even on receipt of a previous time from any source */
    simlock_temp_unlock_reset_config(SIMLOCK_TEMP_UNLOCK_RESET_CONFIG_IF_UNLOCK_EXPIRED);
  }
  else if(start_time > current_time)
  {
    pending_time = (start_time - current_time);
    simlock_status = simlock_temp_unlock_start_pending_timer(pending_time);
  }

  return simlock_status;
} /* simlock_temp_unlock_compare_time_and_set */


/*===========================================================================
FUNCTION SIMLOCK_TEMP_UNLOCK_CHECK_LOCK_STATUS_AND_SET

DESCRIPTION
  This function checks the lock status and set the appopriate timer

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
static void simlock_temp_unlock_check_lock_status_and_set
(
  simlock_time_type            current_time,
  simlock_time_type            start_time,
  simlock_time_type            duration
)
{
  simlock_result_enum_type               simlock_status      = SIMLOCK_SUCCESS;
  simlock_slot_enum_type                 slot                = SIMLOCK_SLOT_1;
  simlock_category_enum_type             category            = SIMLOCK_CATEGORY_3GPP_NW;
  uint8                                  category_index      = 0;
  uint8                                  slot_index          = 0;
  simlock_message_response_data_type   * get_status_resp_ptr = NULL;

  SIMLOCK_MSG_MED_3("simlock_temp_unlock_check_lock_status_and_set current time 0x%x start time 0x%x duration 0x%x",
                      current_time, start_time, duration);

  SIMLOCK_CHECK_AND_MEM_MALLOC(get_status_resp_ptr,
                               sizeof(simlock_message_response_data_type));
  if (get_status_resp_ptr == NULL)
  {
    return;
  }

  /* get the lock status */
  simlock_status = simlock_temp_unlock_get_lock_status(get_status_resp_ptr);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MEM_FREE(get_status_resp_ptr);
    return;
  }

  /* loop through all the slot and categories and set
     appropriate timer for the locks that are active.*/
  for(slot = SIMLOCK_SLOT_1; slot < SIMLOCK_SLOT_NUM; slot++)
  {
    simlock_status = simlock_util_get_slot_index(slot, &slot_index);
    if((simlock_status == SIMLOCK_SUCCESS) &&
       (slot_index < SIMLOCK_SLOT_COUNT_MAX))
    {
      for(category = SIMLOCK_CATEGORY_3GPP_NW;
          category <= SIMLOCK_CATEGORY_3GPP2_RUIM;
          category++)
      {
        simlock_status = simlock_util_get_category_index(category, &category_index);
        if((simlock_status == SIMLOCK_SUCCESS) &&
           (category_index < SIMLOCK_CATEGORY_COUNT_MAX))
        {
          /* if the lock is not enabled, then continue */
          if(!get_status_resp_ptr->message.get_status_resp.lock_info[slot_index].category_info[category_index].category_status)
          {
            continue;
          }

          /* based on the current and start times, the appropriate
             timer (temporary unlock or pending) is set */
          (void)simlock_temp_unlock_compare_time_and_set(slot,
                                                         category,
                                                         current_time,
                                                         start_time,
                                                         duration);
        }
      } /* end loop categories */
    }
  } /* end loop for slots */

  SIMLOCK_MEM_FREE(get_status_resp_ptr);
} /* simlock_temp_unlock_check_lock_status_and_set */


/*===========================================================================
FUNCTION SIMLOCK_TEMP_UNLOCK_INIT

DESCRIPTION
  This function is invoked during the simlock initialization

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_temp_unlock_init
(
  void
)
{
  simlock_result_enum_type               simlock_status      = SIMLOCK_SUCCESS;
  simlock_temp_unlock_time_data_type     time_data;
  simlock_time_info_type                 time;

  memset(&time_data, 0x00, sizeof(simlock_temp_unlock_time_data_type));
  memset(&time, 0x00, sizeof(simlock_time_info_type));

  /* register with the time module */
  (void)simlock_time_register_time_event(simlock_temp_unlock_process_new_time);

  /* get the current time */
  simlock_status = simlock_time_get_current_time(&time);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return;
  }

  /* read the temporary unlock time file */
  simlock_status = simlock_temp_unlock_read_time_file(&time_data);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return;
  }

  /* check the status and set the appropriate timer */
  simlock_temp_unlock_check_lock_status_and_set(time.current_time,
                                                time_data.last_start_time,
                                                time_data.last_duration);
} /* simlock_temp_unlock_init */


/*===========================================================================
FUNCTION SIMLOCK_TEMP_UNLOCK_PROCESS_PENDING_TIMER_EXPIRY

DESCRIPTION
  The function processes the temporary unlock pending timer expiry request
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
simlock_result_enum_type simlock_temp_unlock_process_pending_timer_expiry
(
  void
)
{
  simlock_temp_unlock_time_data_type     time_data;
  simlock_time_info_type                 time;

  memset(&time_data, 0x00, sizeof(simlock_temp_unlock_time_data_type));
  memset(&time, 0x00, sizeof(simlock_time_info_type));

  /* stop the pending timer */
  (void)simlock_temp_unlock_stop_pending_timer();

  /* get the current time */
  if(simlock_time_get_current_time(&time) == SIMLOCK_SUCCESS)
  {
    /* read the temporary unlock time file */
    if(simlock_temp_unlock_read_time_file(&time_data) == SIMLOCK_SUCCESS)
    {
      /* check the status and set the appropriate timer */
      simlock_temp_unlock_check_lock_status_and_set(time.current_time,
                                                    time_data.last_start_time,
                                                    time_data.last_duration);
      return SIMLOCK_SUCCESS;
    }
  }

  return SIMLOCK_GENERIC_ERROR;
} /* simlock_temp_unlock_process_pending_timer_expiry */


/*===========================================================================
FUNCTION SIMLOCK_TEMP_UNLOCK_PROCESS_NEW_TIME

DESCRIPTION
  This is the callback function that gets called when the time module
  receives the new time

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
static void simlock_temp_unlock_process_new_time
(
  simlock_time_info_type       time
)
{
  simlock_result_enum_type               simlock_status      = SIMLOCK_SUCCESS;
  simlock_temp_unlock_time_data_type     time_data;

  memset(&time_data, 0x00, sizeof(simlock_temp_unlock_time_data_type));
  SIMLOCK_MSG_MED_2("New time received : 0x%x 0x%x",
                     time.current_time >> 32 & 0xFFFFFFFF,
                     time.current_time & 0xFFFFFFFF);

  simlock_status = simlock_temp_unlock_read_time_file(&time_data);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    /* Nothing to do, if file read fails */
    return;
  }

  /* check the status and set the appropriate timer */
  simlock_temp_unlock_check_lock_status_and_set(time.current_time,
                                                time_data.last_start_time,
                                                time_data.last_duration);
} /* simlock_temp_unlock_process_new_time */


/*===========================================================================
FUNCTION SIMLOCK_TEMP_UNLOCK_PROCESS_REQUEST

DESCRIPTION
  This function processes the temporary unlock request for a category

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_temp_unlock_process_request
(
  simlock_slot_enum_type       slot,
  simlock_category_enum_type   category,
  simlock_time_type            start_time,
  simlock_time_type            duration
)
{
  simlock_result_enum_type               simlock_status  = SIMLOCK_SUCCESS;
  simlock_temp_unlock_time_data_type     time_data;
  simlock_time_info_type                 time;

  memset(&time_data, 0x00, sizeof(simlock_temp_unlock_time_data_type));
  memset(&time, 0x00, sizeof(simlock_time_info_type));

  SIMLOCK_MSG_MED_4("simlock_temp_unlock_process_request, start_time: 0x%x slot 0x%x category 0x%x duration 0x%x",
                    start_time, slot, category, duration);

  /* check if start time is less than base time */
  if(start_time < SIMLOCK_TIME_BASE)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  /* update the file, with start time and duration */
  time_data.version         = SIMLOCK_TEMP_UNLOCK_FILE_VERSION;
  time_data.last_start_time = start_time - SIMLOCK_TIME_BASE;
  time_data.last_duration   = (uint32)duration;

  simlock_status = simlock_temp_unlock_write_time_file(&time_data);
  if(simlock_status != SIMLOCK_SUCCESS)
  {
    return simlock_status;
  }

  /* unset any existing temporary unlocks.
     This would stop the active timers */
  (void)simlock_temp_unlock_unset(slot, category);

  simlock_time_update_send_time(SIMLOCK_TIME_SOURCE_BINARY_BLOB, start_time);

  /* Get the current time, this should be success all the time.
     Hence ignoring the return value */
  (void)simlock_time_get_current_time(&time);

  /* the appropriate timer is started based
     current and start times */
  return simlock_temp_unlock_compare_time_and_set(slot, category, time.current_time, (start_time - SIMLOCK_TIME_BASE), duration);
} /* simlock_temp_unlock_process_request */


/*===========================================================================
FUNCTION SIMLOCK_TEMP_UNLOCK_IS_ACTIVE

DESCRIPTION
  This function returns true if temporary unlock is active on any category.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  boolean

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_temp_unlock_is_active
(
  void
)
{
  simlock_temp_unlock_remaining_time_type  time_remaining;
  boolean is_active = FALSE;

  if(SIMLOCK_SUCCESS == simlock_temp_unlock_get_remaining_time(&time_remaining))
  {
    uint8   i         = 0;
    uint8   j         = 0;

    for(i = 0; i < SIMLOCK_SLOT_COUNT_MAX; i++)
    {
      for(j = 0; j < SIMLOCK_CATEGORY_COUNT_MAX; j++)
      {
        if(time_remaining.duration[i][j] > 0)
        {
          is_active = TRUE;
          break;
        }
      }

      if(is_active)
      {
        break;
      }
    }
  }

  SIMLOCK_MSG_LOW_1("simlock_temp_unlock_is_active 0x%x", is_active);
  return is_active;
} /* simlock_temp_unlock_is_active */


/*===========================================================================
FUNCTION SIMLOCK_TEMP_UNLOCK_RESET_CONFIG

DESCRIPTION
  This function would post an enginee msg, GET_STATUS
  Based on the input parameter clears the temporary unlock
  information from SFS

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_temp_unlock_reset_config(
  simlock_temp_unlock_reset_config_enum_type   reset_config_type
)
{
  simlock_result_enum_type             simlock_status       = SIMLOCK_GENERIC_ERROR;
  simlock_category_enum_type           category             = SIMLOCK_CATEGORY_3GPP_NW;
  uint8                                category_index       = 0;
  boolean                              is_atleast_a_category_enabled   = FALSE;
  boolean                              is_atleast_a_temp_unlock_active = FALSE;
  boolean                              clear_temp_unlock               = FALSE;
  struct
  {
    simlock_task_cmd_type              cmd;
    simlock_message_response_data_type rsp;
  }*cmd_rsp_ptr = NULL;

  SIMLOCK_CHECK_AND_MEM_MALLOC(cmd_rsp_ptr,
                               sizeof(*cmd_rsp_ptr));

  if(cmd_rsp_ptr == NULL)
  {
    return;
  }

  cmd_rsp_ptr->cmd.data.engine.msg_type = SIMLOCK_GET_STATUS_MSG;

  simlock_status = simlock_process_message(&cmd_rsp_ptr->cmd.data.engine,
                                           &cmd_rsp_ptr->rsp);

  if(simlock_status != SIMLOCK_SUCCESS ||
     cmd_rsp_ptr->rsp.msg_type != SIMLOCK_GET_STATUS_MSG)
  {
    SIMLOCK_MEM_FREE(cmd_rsp_ptr);
    return;
  }

  /* Check the categories enabled.
     3GPP_SIM category is normally controlled by SafeSwitch module, while all
     other categories are under the control of operator. We don't want RSU to
     modify 3GPP_SIM */
  for(category = SIMLOCK_CATEGORY_3GPP_NW;
      category < SIMLOCK_CATEGORY_3GPP_SIM;
      category++)
  {
    simlock_status = simlock_util_get_category_index(category, &category_index);
    if((simlock_status == SIMLOCK_SUCCESS) &&
       (category_index < SIMLOCK_CATEGORY_COUNT_MAX))
    {
      /* Check the status corresponding to slot-1 (zero index) only as
         we lock categories on slot-1 */
      if(cmd_rsp_ptr->rsp.message.get_status_resp.lock_info[0].category_info[category_index].category_status)
      {
        is_atleast_a_category_enabled = TRUE;
      }

      if(cmd_rsp_ptr->rsp.message.get_status_resp.lock_info[0].category_info[category_index].category_status &&
         cmd_rsp_ptr->rsp.message.get_status_resp.remaining_time.duration[0][category_index] > 0)
      {
        is_atleast_a_temp_unlock_active = TRUE;
      }
    }
  }

  SIMLOCK_MEM_FREE(cmd_rsp_ptr);

  switch(reset_config_type)
  {
    case SIMLOCK_TEMP_UNLOCK_RESET_CONFIG_IF_UNLOCKED:
      clear_temp_unlock = !is_atleast_a_category_enabled;
      if(clear_temp_unlock)
      {
        /* Stop the pending timer if active */
        simlock_temp_unlock_stop_pending_timer();
      }
      break;

    case SIMLOCK_TEMP_UNLOCK_RESET_CONFIG_IF_UNLOCK_EXPIRED:
      clear_temp_unlock = !is_atleast_a_temp_unlock_active;
      break;

    default:
      break;
  }

  if(clear_temp_unlock)
  {
    simlock_temp_unlock_time_data_type  time_data;

    /* Remove the start time and duration details from SFS so that
       Temp unlock will not be started after this point across boot ups
       This should be done
         a. When temp unlock is to be applied for a future time
         b. When temp unlock is in progress
         c. When temp unlock is expired
       No harm in writing when no temp unlock at all */
    time_data.version         = SIMLOCK_TEMP_UNLOCK_FILE_VERSION;
    time_data.last_start_time = 0;
    time_data.last_duration   = 0;

    simlock_status = simlock_temp_unlock_write_time_file(&time_data);
  }
  SIMLOCK_MSG_MED_3("simlock_temp_unlock_reset_config reset_config_type 0x%x clear_temp_unlock 0x%x status 0x%x",
                     reset_config_type, clear_temp_unlock, simlock_status);
} /* simlock_temp_unlock_reset_config */

#endif /* FEATURE_SIMLOCK_RSU || FEATURE_SIMLOCK_QC_REMOTE_LOCK */


/*===========================================================================
FUNCTION SIMLOCK_TEMP_UNLOCK_GET_REMAINING_TIME

DESCRIPTION
  This function retrieves the amount of time remaining for the temporary
  unlock

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_temp_unlock_get_remaining_time
(
  simlock_temp_unlock_remaining_time_type   * resp_msg_ptr
)
{
  simlock_result_enum_type               simlock_status   = SIMLOCK_SUCCESS;
  simlock_slot_enum_type                 slot             = SIMLOCK_SLOT_1;
  simlock_category_enum_type             category         = SIMLOCK_CATEGORY_3GPP_NW;
  uint32                                 remaining_time   = 0;
  uint8                                  slot_index       = 0;
  uint8                                  category_index   = 0;

  if(resp_msg_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  memset(resp_msg_ptr->duration, 0x00, sizeof(resp_msg_ptr->duration));

  for(slot = SIMLOCK_SLOT_1; slot < SIMLOCK_SLOT_NUM; slot++)
  {
    simlock_status = simlock_util_get_slot_index(slot, &slot_index);
    if((simlock_status == SIMLOCK_SUCCESS) &&
       (slot_index < SIMLOCK_SLOT_COUNT_MAX))
    {
      for(category = SIMLOCK_CATEGORY_3GPP_NW; category <= SIMLOCK_CATEGORY_3GPP_SIM;
          category++)
      {
        simlock_status = simlock_util_get_category_index(category, &category_index);
        if((simlock_status == SIMLOCK_SUCCESS) &&
           (category_index < SIMLOCK_CATEGORY_COUNT_MAX))
        {
          /* if the category is not temporarily unlocked, continue */
          if(!simlock_timer_check_is_temporary_unlock_set(slot, category))
          {
            continue;
          }

          /* retrieve the remaining time (in msecs) */
          remaining_time = simlock_timer_get_remaining_time(slot, category);
          if(remaining_time < SIMLOCK_MILLISECONDS_IN_A_SECOND)
          {
            continue;
          }

          resp_msg_ptr->duration[slot_index][category_index] = (remaining_time / SIMLOCK_MILLISECONDS_IN_A_SECOND);
        }
      } /* end loop categories */
    }
  } /* end loop for slots */

  return SIMLOCK_SUCCESS;
} /* simlock_temp_unlock_get_remaining_time */

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

#endif /* FEATURE_SIMLOCK */
