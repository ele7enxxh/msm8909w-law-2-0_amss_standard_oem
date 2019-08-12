/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G E N E R I C   S I M   L O C K  T I M E  N I T Z  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the simlock time nitz functions.

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
05/17/15   stv      Fix compiler warnings
04/15/15   stv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "customer.h"

#if defined(FEATURE_SIMLOCK) && (defined(FEATURE_SIMLOCK_RSU) || defined(FEATURE_SIMLOCK_QC_REMOTE_LOCK))

#include "simlock_common.h"
#include "simlock_platform.h"
#include "sys.h"
#include "cm.h"
#include "simlock_modem.h"
#include "simlock_time_nitz.h"
#include "time_jul.h"

/* Number of seconds elapsed from 01 Jan 1970 00:00:00 UTC to 06 Jan 1980 00:00:00 UTC*/
#define SIMLOCK_TIME_GPS_EPOCH_SECONDS 315964800

/* SIM LOCK TIME BASE DATE to calculate time for a source */
#define SIMLOCK_TIME_BASE_DATE_YEAR    2014

/* SIM LOCK TIME NITZ BASE Year */
#define SIMLOCK_TIME_BASE_NITZ_YEAR    2000

/* Macros for Month validation */
#define SIMLOCK_TIME_MONTH_MIN         1
#define SIMLOCK_TIME_MONTH_MAX         12

/* Macros for Day validation */
#define SIMLOCK_TIME_DAY_MIN           1
#define SIMLOCK_TIME_DAY_MAX           31

/* Macros for Hour validation */
#define SIMLOCK_TIME_HOUR_MIN          0
#define SIMLOCK_TIME_HOUR_MAX          23

/* Macros for Minute validation */
#define SIMLOCK_TIME_MINUTE_MIN        0
#define SIMLOCK_TIME_MINUTE_MAX        59

/* Macros for Seconds validation */
#define SIMLOCK_TIME_SECOND_MIN        0
#define SIMLOCK_TIME_SECOND_MAX        59

#define SIMLOCK_TIME_3GPP_SYS_MODE(x)                             \
        ((x == SYS_SYS_MODE_GSM) || (x == SYS_SYS_MODE_WCDMA)  || \
         (x == SYS_SYS_MODE_GW)  || (x == SYS_SYS_MODE_LTE)    || \
         (x == SYS_SYS_MODE_GWL) || (x == SYS_SYS_MODE_TDS))

#define SIMLOCK_TIME_3GPP2_SYS_MODE(x)                            \
         ((x == SYS_SYS_MODE_CDMA) || (x == SYS_SYS_MODE_HDR))

/*===========================================================================
FUNCTION SIMLOCK_TIME_NITZ_CONVERT_DATE_TO_SECONDS

DESCRIPTION
  This function converts the data passed to seconds

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
static simlock_result_enum_type simlock_time_nitz_convert_date_to_seconds(
  simlock_time_gw_nitz_type  gw_nitz_info,
  simlock_time_type         *time_ptr
)
{
  simlock_result_enum_type ret         = SIMLOCK_GENERIC_ERROR;
  uint16                   year_abs    = 0;
  uint32                   time_in_sec = 0;
  time_julian_type         julian_time;

  if(time_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  memset(&julian_time, 0, sizeof(julian_time));

  do
  {
    /* Add the NITZ base year and Validate the year received */
    year_abs = SIMLOCK_TIME_BASE_NITZ_YEAR + gw_nitz_info.year;
    if(year_abs < SIMLOCK_TIME_BASE_DATE_YEAR)
    {
      break;
    }

    /* Validate the month received */
    if(gw_nitz_info.month < SIMLOCK_TIME_MONTH_MIN &&
       gw_nitz_info.month > SIMLOCK_TIME_MONTH_MAX)
    {
      break;
    }

    if(gw_nitz_info.day < SIMLOCK_TIME_DAY_MIN &&
       gw_nitz_info.day > SIMLOCK_TIME_DAY_MAX)
    {
      break;
    }

    /* Validate the hours  - Min is not requried as uint8 min is 0 */
    if(gw_nitz_info.hours > SIMLOCK_TIME_HOUR_MAX)
    {
      break;
    }

    /* Validate the minutes */
    if(gw_nitz_info.minutes > SIMLOCK_TIME_MINUTE_MAX)
    {
      break;
    }

    if(gw_nitz_info.seconds > SIMLOCK_TIME_SECOND_MAX)
    {
      /* If number of seconds is greater than 59, ignore seconds -need not error for error in seconds time*/
      gw_nitz_info.seconds = 0;
    }

    julian_time.year   = year_abs;
    julian_time.month  = gw_nitz_info.month;
    julian_time.day    = gw_nitz_info.day;
    julian_time.hour   = gw_nitz_info.hours;
    julian_time.minute = gw_nitz_info.minutes;
    julian_time.second = gw_nitz_info.seconds;

    /* Number of seconds elapsed from 06 January 1980 00:00:00 UTC till the date in julian_time variable */
    time_in_sec = time_jul_to_secs(&julian_time);
    /* Add the time elapsed from 01 January 1970 00:00:00 UTC till 06 January 1980 00:00:00 UTC */
    time_in_sec += SIMLOCK_TIME_GPS_EPOCH_SECONDS;
    *time_ptr = (simlock_time_type) time_in_sec;
    ret = SIMLOCK_SUCCESS;
  }while(0);

  if(ret != SIMLOCK_SUCCESS)
  {
    SIMLOCK_MSG_MED_5("Date in CM SS : Year 0x%x Month 0x%x Day 0x%x Hour 0x%x time_in_sec 0x%x",
                       gw_nitz_info.year,
                       gw_nitz_info.month,
                       gw_nitz_info.day,
                       gw_nitz_info.hours,
                       time_in_sec);
  }

  return ret;
} /* simlock_time_nitz_convert_date_to_seconds */


/*===========================================================================
FUNCTION SIMLOCK_TIME_3GPP_MODE_UPDATE_TIME

DESCRIPTION
  Convert the time received in CM SS event NITZ info and
  update to our globals

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
static void simlock_time_3gpp_mode_update_time(
  simlock_time_source_enum_type    source,
  const simlock_time_gw_nitz_type *gw_nitz_time_ptr
)
{
  simlock_time_type time = 0;
  simlock_result_enum_type get_seconds_status = SIMLOCK_GENERIC_ERROR;

  if( NULL == gw_nitz_time_ptr ||
     (source != SIMLOCK_TIME_SOURCE_NITZ_PRI &&
      source != SIMLOCK_TIME_SOURCE_NITZ_SEC))
  {
    return;
  }

  /* Check if valid time is present in CM SS info */
  if(gw_nitz_time_ptr->is_time_present == FALSE)
  {
    return;
  }

  get_seconds_status = simlock_time_nitz_convert_date_to_seconds(*gw_nitz_time_ptr,
                                                                  &time);

  if(get_seconds_status == SIMLOCK_SUCCESS)
  {
    simlock_time_update_send_time(source, time);
  }
} /* simlock_time_3gpp_mode_update_time */


/*===========================================================================
FUNCTION   SIMLOCK_TIME_CM_SS_EVENT_CB

DESCRIPTION
    Handler for CM_SS_EVENT_SRV_CHANGED. It posts command to SIMLOCK task.
    This command is for NITZ time information used by SIMLOCK TIME.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_time_cm_ss_event_cb(
  cm_ss_event_e_type               ss_event,
  const cm_mm_msim_ss_info_s_type *mm_ss_info_ptr
)
{
  simlock_time_source_time_msg_req_type  cm_ss_evt_info;
  simlock_task_cmd_type                 *task_cmd_ptr   = NULL;
  simlock_result_enum_type               ret_val        = SIMLOCK_SUCCESS;
  boolean                                post_command   = FALSE;

  if(ss_event != CM_SS_EVENT_SRV_CHANGED ||
     mm_ss_info_ptr == NULL)
  {
    return;
  }

  memset(&cm_ss_evt_info, 0, sizeof(cm_ss_evt_info));

  if(mm_ss_info_ptr->asubs_id == SYS_MODEM_AS_ID_1)
  {
    cm_ss_evt_info.source = SIMLOCK_TIME_SOURCE_NITZ_PRI;
  }
  else if(mm_ss_info_ptr->asubs_id == SYS_MODEM_AS_ID_2)
  {
    cm_ss_evt_info.source = SIMLOCK_TIME_SOURCE_NITZ_SEC;
  }
  else
  {
    /* Unhandled AS ID */
    return;
  }

  /* Check if the stack is operational and full service is acquired 
     Check always for first stack */
  if(mm_ss_info_ptr->stack_info[0].is_operational == FALSE ||
     SYS_SRV_STATUS_SRV != mm_ss_info_ptr->stack_info[0].srv_status)
  {
    return;
  }

  if(SIMLOCK_TIME_3GPP_SYS_MODE(mm_ss_info_ptr->stack_info[0].sys_mode))
  {
    cm_ss_evt_info.src_data.cm_time.sys = SIMLOCK_TIME_SYS_3GPP;

    if(mm_ss_info_ptr->stack_info[0].mode_info.gw_info.mm_information.univ_time_and_time_zone_avail)
    {
      post_command = TRUE;
      cm_ss_evt_info.src_data.cm_time.nitz_info.gw_nitz_info.is_time_present = 
        mm_ss_info_ptr->stack_info[0].mode_info.gw_info.mm_information.univ_time_and_time_zone_avail;
      cm_ss_evt_info.src_data.cm_time.nitz_info.gw_nitz_info.year =
        mm_ss_info_ptr->stack_info[0].mode_info.gw_info.mm_information.univ_time_and_time_zone.year;
      cm_ss_evt_info.src_data.cm_time.nitz_info.gw_nitz_info.month =
        mm_ss_info_ptr->stack_info[0].mode_info.gw_info.mm_information.univ_time_and_time_zone.month;
      cm_ss_evt_info.src_data.cm_time.nitz_info.gw_nitz_info.day =
        mm_ss_info_ptr->stack_info[0].mode_info.gw_info.mm_information.univ_time_and_time_zone.day;
      cm_ss_evt_info.src_data.cm_time.nitz_info.gw_nitz_info.hours =
        mm_ss_info_ptr->stack_info[0].mode_info.gw_info.mm_information.univ_time_and_time_zone.hour;
      cm_ss_evt_info.src_data.cm_time.nitz_info.gw_nitz_info.minutes =
        mm_ss_info_ptr->stack_info[0].mode_info.gw_info.mm_information.univ_time_and_time_zone.minute;
      cm_ss_evt_info.src_data.cm_time.nitz_info.gw_nitz_info.seconds =
        mm_ss_info_ptr->stack_info[0].mode_info.gw_info.mm_information.univ_time_and_time_zone.second;
      cm_ss_evt_info.src_data.cm_time.nitz_info.gw_nitz_info.time_zone =
        mm_ss_info_ptr->stack_info[0].mode_info.gw_info.mm_information.univ_time_and_time_zone.time_zone;
    }
  }
  else
  {
    SIMLOCK_MSG_LOW_1("Unhandled sys mode 0x%x of CM SS event", mm_ss_info_ptr->stack_info[0].sys_mode);
  }

  if(!post_command)
  {
    return;
  }

  SIMLOCK_CHECK_AND_MEM_MALLOC(task_cmd_ptr, sizeof(simlock_task_cmd_type));
  if(task_cmd_ptr == NULL)
  {
    return;
  }

  task_cmd_ptr->cmd_type                  = SIMLOCK_TIME_HANDLE_SOURCE_TIME_INFO_CMD;
  task_cmd_ptr->data.source_time_info_msg = cm_ss_evt_info;

  ret_val = simlock_queue_cmd(task_cmd_ptr);

  SIMLOCK_MSG_MED_2("Queue of SIMLOCK_TIME_HANDLE_SOURCE_TIME_INFO_CMD to SIMLOCK, status 0x%x, source 0x%x",
                     ret_val,
                     task_cmd_ptr->data.source_time_info_msg.source);

  if(ret_val != SIMLOCK_SUCCESS)
  {
    SIMLOCK_FREE(task_cmd_ptr);
  }
  /* Task command pointer free is done once the command is handled */
} /* simlock_time_cm_ss_event_cb */


/*===========================================================================
FUNCTION SIMLOCK_TIME_HANDLE_CM_SS_EVENT

DESCRIPTION
  This function handles the CM SS Event posted to SIMLOCK TIME

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_time_handle_cm_ss_evt(
  const simlock_time_cm_ss_info_type *info_ptr,
  simlock_time_source_enum_type       source
)
{
  if(info_ptr == NULL)
  {
    return SIMLOCK_INCORRECT_PARAMS;
  }

  if(info_ptr->sys == SIMLOCK_TIME_SYS_3GPP)
  {
    /* Sys mode is determined to be 3GPP */
    simlock_time_3gpp_mode_update_time(source, &info_ptr->nitz_info.gw_nitz_info);
  }
  else
  {
    /* Unhandled Sys mode */
    return SIMLOCK_GENERIC_ERROR;
  }

  return SIMLOCK_SUCCESS;
} /* simlock_time_handle_cm_ss_evt */

#endif /* FEATURE_SIMLOCK && (FEATURE_SIMLOCK_RSU || FEATURE_SIMLOCK_QC_REMOTE_LOCK) */


