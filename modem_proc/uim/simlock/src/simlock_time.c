/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G E N E R I C   S I M   L O C K  T I M E  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the simlock time functions.

                        COPYRIGHT INFORMATION

Copyright (c) 2015 - 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/06/16   stv      Remote simlock support
10/12/15   stv      Update back up time(last power cycle source time) correctly
10/06/15   stv      Fix for compilation issue
06/29/15   stv      Start time in temp unlock req as current time
                    Graceful handling of PMIC reset during temp unlock
05/17/15   stv      Fix compiler warnings
04/15/15   stv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"

#if defined(FEATURE_SIMLOCK) && (defined(FEATURE_SIMLOCK_RSU) || defined(FEATURE_SIMLOCK_QC_REMOTE_LOCK))

#include "pm_rtc.h"
#include "rex.h"
#include "simlock_common.h"
#include "simlock_platform.h"
#include "simlock.h"
#include "simlock_file.h"
#include "simlock_time_gps.h"
#include "simlock_time_nitz.h"
#include "simlock_modem.h"


/* Global to store time from all sources which is also written to SFS */
static simlock_time_storage_type    simlock_time_global_time_info;

/* Function ptr to store the function registered by SIMLOCK to send Time events */
static simlock_time_evt_cb_type    *simlock_time_reg_cb = NULL;

/* Timer to tigger the update of the untrusted backup time in case coin
   battery is missing and time from previous run is used. */
static rex_timer_type              *simlock_backup_time_update_timer_ptr = NULL;
/* Indicates whether the coin battery is present in the device.
   Note: This will not be known during the initial power up. */
static boolean                      simlock_coin_battery_present = TRUE;

/* Minimum accuracy time - say 2hrs */
#define SIMLOCK_TIME_MIN_ACCURACY_TIME      (3600 * 2)

/* Max Coherence delta between similar sources - say 2hrs */
#define SIMLOCK_TIME_MAX_COHERENCE_DELTA    (3600 * 2)

/* Number of milli seconds in one hour */
#define SIMLOCK_MILLISECONDS_IN_A_HOUR       3600000

/*===========================================================================
FUNCTION SIMLOCK_TIME_GET_CURRENT_PMIC_TIME

DESCRIPTION
  This function gets the current PMIC time by calling PMIC API
  Time is filled in the out parameter

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type
  If function returns SIMLOCK_SUCCESS   - Time is filled in out parameter

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_time_get_current_pmic_time(
  simlock_time_type *time_ptr
)
{
  pm_err_flag_type       error_flag = PM_ERR_FLAG__INVALID;
  pm_hal_rtc_time_type   rtc_time   = {0};
  uint8                  pmic_chip  = 0;

  if(time_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* Usually the primary PMIC has the RTC for which pmic_chip index 0 can be used */
  error_flag = pm_dev_hal_rtc_get_time(pmic_chip, &rtc_time);

  SIMLOCK_MSG_HIGH_2("PMIC status 0x%x time 0x%x", error_flag, rtc_time.sec);

  if(PM_ERR_FLAG__SUCCESS != error_flag)
  {
    return SIMLOCK_GENERIC_ERROR;
  }

  *time_ptr = rtc_time.sec;
  return SIMLOCK_SUCCESS;
}/* simlock_time_get_current_pmic_time */


/*===========================================================================
FUNCTION SIMLOCK_BACKUP_TIME_UPDATE_TIMER_EXPIRY

DESCRIPTION
  This function handles the trigger of the backup time update timer.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
static void simlock_backup_time_update_timer_expiry
(
  unsigned long unused
)
{
  simlock_result_enum_type     simlock_status = SIMLOCK_SUCCESS;
  simlock_task_cmd_type      * task_cmd_ptr   = NULL;

  (void)unused;

  SIMLOCK_MSG_MED_0("backup time update timer expired");

  SIMLOCK_CHECK_AND_MEM_MALLOC(task_cmd_ptr,
                               sizeof(simlock_task_cmd_type));
  if (task_cmd_ptr == NULL)
  {
    return;
  }

  memset(task_cmd_ptr, 0x00, sizeof(simlock_task_cmd_type));

  task_cmd_ptr->cmd_type = SIMLOCK_TEMPORARY_UNLOCK_BACKUP_TIME_UPDATE_CMD;

  /* Put on SIM Lock command queue */
  simlock_status = simlock_queue_cmd(task_cmd_ptr);
  if (simlock_status != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_LOW_1("failed to build timer expiry request, status: 0x%x",
                      simlock_status);
    /* Free the pointer since the task_cmd_ptr has not been put
       onto the command queue */
    SIMLOCK_MEM_FREE(task_cmd_ptr);
  }
} /* simlock_backup_time_update_timer_expiry */


/*===========================================================================
FUNCTION SIMLOCK_TIME_GET_CURRENT_TIME

DESCRIPTION
  API to get the current time from all sources combined.
  The time returned will be from SIMLOCK_TIME_BASE in seconds.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type
    SIMLOCK_SUCCESS          - if successful
    SIMLOCK_INCORRECT_PARAMS - if NULL call back

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type  simlock_time_get_current_time(
  simlock_time_info_type  *time_ptr
)
{
  simlock_time_type            current_pmic_time      = 0;
  simlock_time_type            source_time            = 0;
  simlock_result_enum_type     pmic_time_status       = SIMLOCK_GENERIC_ERROR;
  boolean                      valid_time_available   = FALSE;
  uint8                        i                      = 0;
  uint8                        j                      = 0;
  uint8                        best_source            = 0;
  uint8                        similar_sources[SIMLOCK_TIME_TRUSTED_SOURCE_COUNT];

  if(NULL == time_ptr)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  /* First get the PMIC time */
  pmic_time_status = simlock_time_get_current_pmic_time(&current_pmic_time);

  if(pmic_time_status != SIMLOCK_SUCCESS)
  {
    return pmic_time_status;
  }

  /* Find how many trusted sources of time are similar for each available source */
  /* The best source will be returned */
  for(i = 0; i < SIMLOCK_TIME_TRUSTED_SOURCE_COUNT; i++)
  {
    similar_sources[i] = 0;

    if(simlock_time_global_time_info.data[i].valid == FALSE)
    {
      continue;
    }

    valid_time_available = TRUE;

    source_time = simlock_time_global_time_info.data[i].offset + current_pmic_time;

    SIMLOCK_MSG_MED_2("Get current time : source_time[0x%x] is 0x%x", i, source_time);

    for(j = 0; j < SIMLOCK_TIME_TRUSTED_SOURCE_COUNT; j++)
    {
      simlock_time_type valid_source_time = 0;

      /* Check if the source is valid, if not continue to next source.*/
      if(simlock_time_global_time_info.data[j].valid == FALSE)
      {
        continue;
      }

      valid_source_time = simlock_time_global_time_info.data[j].offset + current_pmic_time;

      if((source_time > valid_source_time && source_time - valid_source_time < SIMLOCK_TIME_MAX_COHERENCE_DELTA)||
         (source_time <= valid_source_time && valid_source_time - source_time < SIMLOCK_TIME_MAX_COHERENCE_DELTA))
      {
        /* If this is a source with similar time, increment similar source count.
           Note that the self source is also counted here.
           When all the sources are incoherent, first valid time will be given assuming that it is trusted than the rest.*/
        similar_sources[i]++;
      }
    }

    /* Check if this could be the best source */
    if(i != best_source && similar_sources[i] > similar_sources[best_source])
    {
      best_source = i;
    }
  }

  /* Check if trusted source is available first */
  if(valid_time_available)
  {
    /* Pick the time from the best source identified */
    time_ptr->current_time = simlock_time_global_time_info.data[best_source].offset + current_pmic_time;
    return SIMLOCK_SUCCESS;
  }


  /* No valid time from trusted sources */
  if(simlock_time_global_time_info.data[SIMLOCK_TIME_SOURCE_BINARY_BLOB].valid)
  {
    /* If no source is valid then give time from BLOB Source */
    time_ptr->current_time = simlock_time_global_time_info.data[SIMLOCK_TIME_SOURCE_BINARY_BLOB].offset +
                             current_pmic_time;
    SIMLOCK_MSG_MED_1("Time from blob 0x%x", time_ptr->current_time);
    return SIMLOCK_SUCCESS;
  }

  /* None of the trusted sources are available and no time from
     blob also.hence check if last power cycle time is available */
  if (simlock_time_global_time_info.data[SIMLOCK_TIME_SOURCE_LAST_POWER_CYCLE].valid)
  {
    time_ptr->current_time = simlock_time_global_time_info.data[SIMLOCK_TIME_SOURCE_LAST_POWER_CYCLE].offset + current_pmic_time;
    SIMLOCK_MSG_MED_1("Time from last power cycle 0x%x", time_ptr->current_time);

    return SIMLOCK_SUCCESS;
  }

  /* Could not give any time */
  return SIMLOCK_GENERIC_ERROR;
}/* simlock_time_get_current_time */


/*===========================================================================
FUNCTION  SIMLOCK_TIME_UPDATE_TIME

DESCRIPTION
  This function updates the time given by source to global.
  Also writes the information to SFS.
  source_time parameter would the time in seconds from 
  UTC base time(Jan 1 1970 00:00:00).

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_time_update_time(
  simlock_time_source_enum_type    source,
  simlock_time_type                source_time
)
{
  simlock_time_type         pmic_time     = 0;
  simlock_result_enum_type  get_pmic_time = SIMLOCK_GENERIC_ERROR;

  if(source >= SIMLOCK_TIME_SOURCE_COUNT)
  {
    return;
  }

  get_pmic_time = simlock_time_get_current_pmic_time(&pmic_time);

  SIMLOCK_MSG_MED_3("Update time : Time 0x%x 0x%x for source 0x%x",
                     source_time >> 32 & 0xFFFFFFFF,
                     source_time & 0xFFFFFFFF,
                     source);

  /* Source time is from UTC time base to now
     Hence should be greater than SIMLOCK_TIME_BASE
     PMIC time is from System start
     Hence Source time should be always greater */
  if(source_time < SIMLOCK_TIME_BASE  ||
     get_pmic_time != SIMLOCK_SUCCESS ||
     source_time < pmic_time          ||
     source_time < SIMLOCK_TIME_BASE + pmic_time)
  {
    return;
  }

  /* Update the last pmic time */
  simlock_time_global_time_info.last_pmic_time                       = pmic_time;
  simlock_time_global_time_info.data[source].pmic_time_when_acquired = pmic_time;
  simlock_time_global_time_info.data[source].offset                  = source_time - pmic_time - (simlock_time_type)SIMLOCK_TIME_BASE;
  simlock_time_global_time_info.data[source].valid                   = TRUE;

  (void)simlock_file_write((uint8*)&simlock_time_global_time_info,
                           (uint32)sizeof(simlock_time_global_time_info),
                           SIMLOCK_FILE_TIME_INFO,
                           SIMLOCK_SFS_LOCAL);
}/* simlock_time_update_time */


/*===========================================================================
FUNCTION SIMLOCK_TIME_UPDATE_SEND_TIME

DESCRIPTION
  This function
  a. Updates the time from the source to global and writes to SFS
  b. If the best time of all the sources is 2 hrs(MIN ACCURACY) greater than  the last sent time to SIMLOCK,
      SIMLOCK registered call back is called to send the latest time.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_time_update_send_time(
  simlock_time_source_enum_type  source,
  simlock_time_type              source_time
)
{
  simlock_time_info_type   time_info;
  simlock_result_enum_type get_time_status = SIMLOCK_GENERIC_ERROR;

  simlock_time_update_time(source, source_time);

  get_time_status = simlock_time_get_current_time(&time_info);

  if(get_time_status != SIMLOCK_SUCCESS)
  {
    return;
  }

  if(time_info.current_time >= simlock_time_global_time_info.last_sent_time &&
     time_info.current_time - simlock_time_global_time_info.last_sent_time > SIMLOCK_TIME_MIN_ACCURACY_TIME &&
     simlock_time_reg_cb != NULL)
  {
    simlock_time_reg_cb(time_info);
    /* Update the current time as  last sent time to SIMLOCK*/
    simlock_time_global_time_info.last_sent_time = time_info.current_time;

    (void)simlock_file_write((uint8*)&simlock_time_global_time_info,
                             sizeof(simlock_time_global_time_info),
                             SIMLOCK_FILE_TIME_INFO,
                             SIMLOCK_SFS_LOCAL);
  }
}/* simlock_time_update_send_time */


/*===========================================================================
FUNCTION SIMLOCK_TIME_RETRIEVE_FROM_SFS

DESCRIPTION
  This function reads the SIMLOCK_FILE_TIME_INFO from SFS and cache.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
static void simlock_time_retrieve_from_sfs(
  void
)
{
  uint32                    data_len     = 0;
  uint8                    *data_buf_ptr = NULL;
  simlock_time_type         pmic_time    = 0;
  simlock_result_enum_type  status       = SIMLOCK_GENERIC_ERROR;
  uint8                     i            = 0;
  simlock_time_info_type    time;

  data_len = sizeof(simlock_time_global_time_info);

  status = simlock_file_read(&data_buf_ptr,
                             &data_len,
                             SIMLOCK_FILE_TIME_INFO,
                             SIMLOCK_SFS_LOCAL);

  if(SIMLOCK_SUCCESS != status ||
     data_buf_ptr == NULL ||
     data_len < sizeof(simlock_time_storage_type))
  {
    simlock_coin_battery_present = FALSE;

    return;
  }

  simlock_time_global_time_info = *((simlock_time_storage_type *)data_buf_ptr);

  SIMLOCK_FREE(data_buf_ptr);

  status = simlock_time_get_current_pmic_time(&pmic_time);

  if(status != SIMLOCK_SUCCESS)
  {
    /* Unable to get PMIC time - this should not really happen
       and we can only wipe out all previous data */
    SIMLOCK_MSG_ERR_0("Failure from PMIC");

    /* Failed to retrieve the current pmic time so clear all global time and
       source information as they are now potentially invalid. */
    pmic_time = 0;

    memset(&simlock_time_global_time_info, 0x00, sizeof(simlock_time_global_time_info));
  }
  /* PMIC time has reset indicating that coin battery is not present/empty */
  else if (simlock_time_global_time_info.last_pmic_time >= pmic_time)
  {
    simlock_coin_battery_present = FALSE;

    /* The last stored PMIC time is greater than current PMIC time: this means
       that PMIC might have restarted... this can happen if the coin cell battery
       is not present on the device.
       In this case, expectation for RSU is that time when device was off between
       power cycles is not accounted, so we need to calculate the time when information
       was last saved and use that as current time. In order to do it, we update the
       offsets using the new PMIC time */
    for (i = 0; i < SIMLOCK_TIME_SOURCE_COUNT; i++)
    {
      if (simlock_time_global_time_info.data[i].valid)
      {
        simlock_time_global_time_info.data[i].offset =
          simlock_time_global_time_info.last_pmic_time - pmic_time + simlock_time_global_time_info.data[i].offset;
        simlock_time_global_time_info.data[i].pmic_time_when_acquired = pmic_time;
      }
    }

    /* Once the offsets are calculated, retrieve the most trusted current time to be saved
       as the last power cycle backup in SFS. This will provide base protection of the current
       time in the case of an ungraceful shutdown. Since all "trusted" sources are now considered
       invalid and untrustworthy, purge them from the cache and SFS. */
    status = simlock_time_get_current_time(&time);

    memset(&simlock_time_global_time_info, 0x00, sizeof(simlock_time_global_time_info));

    if(status == SIMLOCK_SUCCESS)
    {
      simlock_time_update_time(SIMLOCK_TIME_SOURCE_LAST_POWER_CYCLE, time.current_time + SIMLOCK_TIME_BASE);

      return;
    }
  }

  simlock_time_global_time_info.last_pmic_time = pmic_time;

  /* Update the PMIC time received to SFS */
  (void)simlock_file_write((uint8*)&simlock_time_global_time_info,
                           sizeof(simlock_time_global_time_info),
                           SIMLOCK_FILE_TIME_INFO,
                           SIMLOCK_SFS_LOCAL);
} /* simlock_time_retrieve_from_sfs */


/*===========================================================================
FUNCTION SIMLOCK_TIME_REGISTER_TIME_EVENT

DESCRIPTION
  API to register the simlock call back function with TIME module
  NULL call back is allowed for de-registration

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type
    SIMLOCK_SUCCESS              successful
    SIMLOCK_GENERIC_ERROR        already a callback is registered

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type    simlock_time_register_time_event(
  simlock_time_evt_cb_type  *evt_cb_ptr
)
{
  if(simlock_time_reg_cb != NULL)
  {
    /* A call back is already registered */
    return SIMLOCK_GENERIC_ERROR;
  }

  simlock_time_reg_cb = evt_cb_ptr;

  return SIMLOCK_SUCCESS;
} /* simlock_time_register_time_event */


/*===========================================================================
FUNCTION SIMLOCK_TIME_INIT

DESCRIPTION
  Initialization of SIMLOCK TIME module

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_time_init(
  void
)
{
  memset(&simlock_time_global_time_info, 0, sizeof(simlock_time_global_time_info));

  /* Retrive the last stored time info from global if present */
  simlock_time_retrieve_from_sfs();

#ifdef FEATURE_SIMLOCK_QMI_LOC
  /* Initialize module for GPS time */
  simlock_time_qmi_loc_init();
#endif /* FEATURE_SIMLOCK_QMI_LOC */
} /* simlock_time_init */


/*===========================================================================
FUNCTION SIMLOCK_TIME_HANDLE_SOURCE_TIME_INFO_CMD

DESCRIPTION
  This function handles SIMLOCK_TIME_HANDLE_SOURCE_TIME_CMD

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_time_handle_source_time_info_cmd(
  simlock_time_source_time_msg_req_type *source_time_info_ptr
)
{
  if(source_time_info_ptr == NULL)
  {
    return;
  }

  switch(source_time_info_ptr->source)
  {
    case SIMLOCK_TIME_SOURCE_NITZ_PRI:
    case SIMLOCK_TIME_SOURCE_NITZ_SEC:
      simlock_time_handle_cm_ss_evt(&source_time_info_ptr->src_data.cm_time,
                                    source_time_info_ptr->source);
      break;

#ifdef FEATURE_SIMLOCK_QMI_LOC
    case SIMLOCK_TIME_SOURCE_GPS:
      simlock_time_handle_qmi_loc_ind(&source_time_info_ptr->src_data.gps_time);
      break;
#endif /* FEATURE_SIMLOCK_QMI_LOC */

    default:
      SIMLOCK_MSG_ERR_1("Invalid source in simlock_time_handle_source_time_info_cmd 0x%x",
                         source_time_info_ptr->source);
      break;
  }
}/* simlock_time_handle_source_time_info_cmd */


/*===========================================================================
FUNCTION SIMLOCK_TIME_IS_GPS_TIME_REQUIRED

DESCRIPTION
  This function checks if a new request is required to be posted to
  QMI LOC to get GPS time.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  boolean

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_time_is_gps_time_required(
  void
)
{
#ifdef FEATURE_SIMLOCK_QMI_LOC
  simlock_time_type        gps_max_valid_time = 0;
  simlock_result_enum_type ret_val            = SIMLOCK_GENERIC_ERROR;
  simlock_time_type        pmic_time          = 0;

  /* If valid GPS time is present, check if validity of the time is expired.
     In case the validity is not expired, calculate in how much time a new GPS
     request is to be placed and start a timer for that value.
     On timer expiry a new request will be placed to QMI LOC */
  if(simlock_time_global_time_info.data[SIMLOCK_TIME_SOURCE_GPS].valid == FALSE)
  {
    return TRUE;
  }

  gps_max_valid_time = simlock_time_global_time_info.data[SIMLOCK_TIME_SOURCE_GPS].pmic_time_when_acquired +
                       SIMLOCK_TIME_GPS_MAX_VALIDITY_TIME;

  ret_val = simlock_time_get_current_pmic_time(&pmic_time);

  if((ret_val == SIMLOCK_SUCCESS) &&
     (gps_max_valid_time > pmic_time))
  {
    simlock_time_type timer_val = 0;

    timer_val = gps_max_valid_time - pmic_time;
    SIMLOCK_MSG_MED_1("Already have a valid GPS time. Will get time from GPS after 0x%x seconds", timer_val);
    simlock_time_start_gps_timer(timer_val);
    return FALSE;
  }

  return TRUE;
#else
  return FALSE;
#endif /* FEATURE_SIMLOCK_QMI_LOC */
} /* simlock_time_is_gps_time_required */


/*===========================================================================
FUNCTION SIMLOCK_TIME_DEINIT

DESCRIPTION
  This function deinitalize the simlock time moduel by clearing
  simlock_time_global_time_info global. If temporary unlock is enabled then
  saves the information current time information in the SFS as a backup to keep
  the integrity of the temporary unlock.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_time_deinit(
  boolean    store_current_time
)
{
  /* Store the best possible current time in SFS as a backup in case we are not
     able to retrieve a valid trusted time upon power up and coin battery is not
     available. Save the current time regardless of the simlock_coin_battery_present
     flag in case this is the first power up and the presence of the coin battery
     is still unknown. */

  if(store_current_time)
  {
    simlock_time_info_type   time;

    if(simlock_time_get_current_time(&time) == SIMLOCK_SUCCESS)
    {
      simlock_time_update_time(SIMLOCK_TIME_SOURCE_LAST_POWER_CYCLE, time.current_time + SIMLOCK_TIME_BASE);
    }
  }

  simlock_time_periodic_backup(FALSE);
}/* simlock_time_deinit */


/*===========================================================================
FUNCTION SIMLOCK_TIME_PROCESS_BACKUP_TIME_UPDATE

DESCRIPTION
  This function processes the updating and backup of the current time.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_time_process_backup_time_update
(
  void
)
{
  simlock_time_info_type   time;

  simlock_time_periodic_backup(FALSE);

  if(simlock_time_get_current_time(&time) == SIMLOCK_SUCCESS)
  {
    simlock_time_update_time(SIMLOCK_TIME_SOURCE_LAST_POWER_CYCLE, time.current_time + SIMLOCK_TIME_BASE);
  }

  simlock_time_periodic_backup(TRUE);
} /* simlock_time_process_backup_time_update */


/*===========================================================================
FUNCTION SIMLOCK_TIME_PERIODIC_BACKUP

DESCRIPTION
  Creates/starts or stop/deletes the periodic current time backup timer.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_time_periodic_backup
(
  boolean set
)
{

  SIMLOCK_MSG_HIGH_2("simlock_time_periodic_backup_timer coin_battery 0x%x  set 0x%x", simlock_coin_battery_present, set);

  if(simlock_coin_battery_present)
  {
    return;
  }

  if(set)
  {
    if(simlock_backup_time_update_timer_ptr == NULL)
  {
    /* Start timer to recurringly update the current time */
    simlock_backup_time_update_timer_ptr =
                      rex_create_timer_ex(simlock_backup_time_update_timer_expiry, 0);
    if(simlock_backup_time_update_timer_ptr != NULL)
    {
      (void)rex_set_timer(simlock_backup_time_update_timer_ptr, SIMLOCK_MILLISECONDS_IN_A_HOUR);
    }
  }
  }
  else
  {
    if(simlock_backup_time_update_timer_ptr != NULL)
    {
      (void)rex_clr_timer(simlock_backup_time_update_timer_ptr);

      rex_delete_timer_ex(simlock_backup_time_update_timer_ptr);

      simlock_backup_time_update_timer_ptr = NULL;
    }
  }
} /* simlock_time_periodic_backup */

#endif /* FEATURE_SIMLOCK && (FEATURE_SIMLOCK_RSU || FEATURE_SIMLOCK_QC_REMOTE_LOCK) */



