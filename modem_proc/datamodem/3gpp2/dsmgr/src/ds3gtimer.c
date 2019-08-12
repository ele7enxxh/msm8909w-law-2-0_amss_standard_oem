/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               3 G   D A T A   S E R V I C E S   M A N A G E R

                        T I M E R   F U N C T I O N S

GENERAL DESCRIPTION
  This software unit contains wrapper functions for REX timer services.

EXTERNALIZED FUNCTIONS
  ds3gi_timer_init()
    Initializes timers

  ds3gi_process_timer_expired_cmd()
    Called to process the DS_TIMER_EXPIRED_CMD

  ds3gi_start_timer()
    Starts a specified timer

  ds3gi_stop_timer()
    Stops a specified timer

  ds3gi_is_timer_running()
    Queries a specified timer's state
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  ds3gi_timer_init() must be called once, during DS task startup, to
  initialize the timer services.


  Copyright (c) 2001 - 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds3gtimer.c_v   1.1   13 Aug 2002 16:24:52   sramacha  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/ds3gtimer.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/27/14   tk      Fixed issue in RING URC reporting for MT CSD call.
01/02/12   msh     Coan: Feature cleanup
05/26/11   dvk     Global Variables Cleanup
03/23/11   sn      Globals cleanup for free floating of tasks.
09/09/10   op      Migrated to MSG 2.0 macros
22/10/09   ca      Fixed Lint Errors.
05/05/09   ss      Fixed Compiler Warnings.
02/25/09   sn      Fixed off-target lint errors.
07/15/08   ar      Featurized for multiprocessor build.
01/04/08   psng    Fixed lint errors.
02/14/07   vd      Lint error fixes.
07/23/06   ar      Use ds3g_siolib_current_ring_type for ring timer.  Add
                   ds3gi_is_timer_running().
01/19/06   sl      Serializer related changes.
03/01/02   smp     Added DCD wink timer.
12/14/01   smp     Renamed ds3g_ring_serial_port() to ds3g_alert_user().
07/10/01   smp     Initial version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "amssassert.h"
#include "msg.h"
#include "err.h"
#include "rex.h"
#include "dstask.h"
#include "ds3gmgr.h"
#include "ds3gmgrint.h"
#include "ds3gtimer.h"
#include "ds3gsiolib.h"
#include "data_err_suppr.h"
#include "data_msg.h"
#include "ds3gsiolib_ex.h"
#include "ps_sys_conf.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/*------------------------------------------------------------------------- 
  Default value to refresh actual throughput timer value (in msec)
  -------------------------------------------------------------------------*/
#define DS3G_REFRESH_ACTUAL_THROUGHPUT_TIMER_DEF      250

/*---------------------------------------------------------------------------
       TIMERS
---------------------------------------------------------------------------*/
typedef struct
{
  void                  *timer_ptr;  
    /*! timer object>*/
  ds3g_timer_type_e      timer_type; 
    /*! Timer type Rex (or) MCS>*/
  boolean                is_periodic_timer;
    /*!Used for MCS timers only to start a periodic timer>*/
  uint16                 timer_exp_cmd_cnt;
    /*!Number of times the timer expired and waiting to be processed 
      in DS cmd queue */              
}ds3gi_timer_s;

static ds3gi_timer_s ds3gi_timer_tbl[DS3G_TIMER_MAX] ={{NULL}};

/*-------------------------------------------------------------------------
  Global variable for TIMER critical sections.
-------------------------------------------------------------------------*/
rex_crit_sect_type      ds3g_timer_crit_sect = {{0}};

/*---------------------------------------------------------------------------
  Variable to store time interval to send Throughput Info Indication
  periodically
  ---------------------------------------------------------------------------*/
static uint32  ds3g_throughput_info_ind_interval[DS3GSUBSMGR_SUBS_ID_MAX] = {0};
/*--------------------------------------------------------------------------- 
  Variable to store time interval to refresh actual throughput for all
  active calls periodically
  ---------------------------------------------------------------------------*/
static uint32  ds3g_actual_throughput_interval[DS3GSUBSMGR_SUBS_ID_MAX] = {0};

/*--------------------------------------------------------------------------- 
  Variable to store time interval to send indication for downlink estimated
  throughput for all active calls periodically
  ---------------------------------------------------------------------------*/
static uint32  ds3g_estimated_dl_throughput_interval[DS3GSUBSMGR_SUBS_ID_MAX] = {0};

/*--------------------------------------------------------------------------- 
  Timer objects for Throughput Info Indication 
  ---------------------------------------------------------------------------*/
static mcs_timer_type ds3g_throughput_info_timer[DS3GSUBSMGR_SUBS_ID_MAX];

/*--------------------------------------------------------------------------- 
  Timer object to refresh actual throughput
  ---------------------------------------------------------------------------*/
static mcs_timer_type ds3g_refresh_actual_throughput_timer[DS3GSUBSMGR_SUBS_ID_MAX];

/*--------------------------------------------------------------------------- 
  Timer object to estimated downlink throughput timer
  ---------------------------------------------------------------------------*/
static mcs_timer_type ds3g_estimated_dl_throughput_timer[DS3GSUBSMGR_SUBS_ID_MAX];

/*===========================================================================
                    Forward declarations/prototypes
===========================================================================*/
/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3GI_GET_ACTUAL_THROUGHPUT_REFRESH_INTERVAL

DESCRIPTION   For the given subscription ID, this function returns the time 
              interval(in msec), the actual throughput rate gets refreshed
              for every bearer/RLP depending on the current sys mode and
              the number of active calls 

DEPENDENCIES  The throughput Info event timer should be running to refresh 
              actual throughput for all bearers/RLP's  

RETURN VALUE  uint32 - Refresh Interval in (msec)
 
              0       - If the timer is not running (or) if the subscription
                        ID is invalid
 
SIDE EFFECTS  None
===========================================================================*/
uint32 ds3gi_get_actual_throughput_refresh_interval
(
  ds3gsubsmgr_subs_id_e_type ds3g_subs_id
)
{
  uint32          interval = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
  {
    interval = ds3g_actual_throughput_interval[ds3g_subs_id];
  }
  return interval;
} /* ds3gi_get_actual_throughput_refresh_interval */


/*===========================================================================
FUNCTION      DS3GI_GET_ESTIMATED_DL_THROUGHPUT_INTERVAL

DESCRIPTION   For the given subscription ID, this function returns the 
              multiplier for t_acumulate that is used to calculate t_report

DEPENDENCIES  

RETURN VALUE  uint32 - T_accumulate multiplier 
 

 
SIDE EFFECTS  None
===========================================================================*/
uint32 ds3gi_get_estimated_dl_throughput_interval
(
  ds3gsubsmgr_subs_id_e_type ds3g_subs_id
)
{
  uint32          interval = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
  {
    interval = ds3g_estimated_dl_throughput_interval[ds3g_subs_id];
  }
  return interval;
} /* ds3gi_get_estimated_dl_throughput_interval */

/*===========================================================================
FUNCTION      DS3GI_GET_THROUGHPUT_INFO_IND_INTERVAL

DESCRIPTION   This function returns the frequency (in msec) the throughput 
              Information Indication is generated periodically for the
              given subscription ID

DEPENDENCIES  None

RETURN VALUE  uint32 - Refresh Interval in (msec)
 
              0       - If the timer is not running (or) if the subscription
                        ID is invalid
 
SIDE EFFECTS  None
===========================================================================*/
static uint32 ds3gi_get_throughput_info_ind_interval
(
  ds3gsubsmgr_subs_id_e_type ds3g_subs_id
)
{
  uint32          interval = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
{
    interval = ds3g_throughput_info_ind_interval[ds3g_subs_id];
  }

  return interval;
} /* ds3gi_get_throughput_info_ind_interval() */

/*===========================================================================
FUNCTION      DS3GI_SET_THROUGHPUT_INFO_IND_INTERVAL

DESCRIPTION   This function configures the timer interval to generate 
              throughput information Ind periodically. 

DEPENDENCIES  None

RETURN VALUE  TRUE  - If Throughput Info Ind interval has been set for 
                      the Subscription ID
              FALSE - Otherwise
 
SIDE EFFECTS  None
===========================================================================*/
static boolean ds3gi_set_throughput_info_ind_interval
(
  ds3gsubsmgr_subs_id_e_type ds3g_subs_id,
  uint32 interval
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Is the Subs ID Valid?
  -------------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
  {
    return FALSE;
  }
  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,"Subs ID: %d Set Throughput Info Ind Timer"
                                  "to %d msec", ds3g_subs_id, interval);
  ds3g_throughput_info_ind_interval[ds3g_subs_id] = interval;
  return TRUE;
}/* ds3gi_set_throughput_info_ind_interval() */


/*===========================================================================
FUNCTION      DS3GI_SET_STIMATED_DL_THROUGHPUT_INTERVAL

DESCRIPTION   This function sets the 
              multiplier for t_acumulate that is used to calculate t_report

DEPENDENCIES  None

RETURN VALUE  TRUE  - If Throughput Info Ind interval has been set for 
                      the Subscription ID
              FALSE - Otherwise
 
SIDE EFFECTS  None
===========================================================================*/
static boolean ds3gi_set_estimated_dl_throughput_interval
(
  ds3gsubsmgr_subs_id_e_type ds3g_subs_id,
  uint32 interval
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Is the Subs ID Valid?
  -------------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
  {
    return FALSE;
  }
  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,"Subs ID: %d Set Throughput Info Ind Timer"
                                  "to %d msec", ds3g_subs_id, interval);
  ds3g_estimated_dl_throughput_interval[ds3g_subs_id] = interval;
  return TRUE;
}/* ds3gi_set_stimated_dl_throughput_interval() */

/*===========================================================================
FUNCTION      DS3GI_SET_ACTUAL_THROUGHPUT_REFRESH_INTERVAL

DESCRIPTION   For the current subscription ID, this function sets
              sets the time interval to periodically refresh 
              the actual throughput numbers for all bearers/RLP's for all
              active calls

DEPENDENCIES  None

RETURN VALUE  TRUE  - If Actual throughput refresh interval has been set for 
                      the Subscription ID
              FALSE - Otherwise
 
 
SIDE EFFECTS  None
===========================================================================*/
static boolean ds3g_set_actual_throughput_refresh_interval
(
  ds3gsubsmgr_subs_id_e_type ds3g_subs_id,
  uint32 interval
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Is the Subs ID Valid?
  -------------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
  {
    return FALSE;
  }
  DATA_3GMGR_MSG2(MSG_LEGACY_HIGH, "Subs ID: %d Setting Time to refresh actual "
                                  " throughput every %d msec",
                                    ds3g_subs_id,interval);
  ds3g_actual_throughput_interval[ds3g_subs_id] = interval;
  return TRUE;
} /* ds3g_set_actual_throughput_refresh_interval() */




/*==============================================================================
FUNCTION      DS3GI_UPDATE_DOWNLINK_THROUGHPUT_ESTIMATION_IND_FREQUENCY

DESCRIPTION   * Given the current subscription ID, this function updates t_report
              and prepares MH to the reporting
 
SIDE EFFECTS  None
===========================================================================*/
static int ds3gi_update_downlink_throughput_estimation_ind_frequency
(
  ds3gsubsmgr_subs_id_e_type          ds3g_subs_id
)
{
  int                   ret_val                       = -1;
  uint32                dl_estm_throughput_info_ind_freq  =  0;
  ds3g_timer_enum_type  timer_id                      =  DS3G_TIMER_MIN;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Invalid DS3G sub ID %d",ds3g_subs_id);
    return ret_val;
  }

  /*------------------------------------------------------------------------- 
    Calculate the duration to refresh actual throughput based on the
    duration of reporting throughput Info Indication
    -------------------------------------------------------------------------*/
  dl_estm_throughput_info_ind_freq = 
                          ds3gi_get_estimated_dl_throughput_interval(ds3g_subs_id);


  /*------------------------------------------------------------------------- 
    Acquire the timer ID to start the timer based on the subscription ID
    -------------------------------------------------------------------------*/
  switch (ds3g_subs_id)
  {
    case DS3GSUBSMGR_SUBS_ID_1:
      timer_id = DS3G_TIMER_DOWNLINK_THROUGHPUT_ESTIMATION_SUBS_ID_1;
      break;
    case DS3GSUBSMGR_SUBS_ID_2:
      timer_id = DS3G_TIMER_DOWNLINK_THROUGHPUT_ESTIMATION_SUBS_ID_2;
      break;
#if defined(FEATURE_TRIPLE_SIM)
    case DS3GSUBSMGR_SUBS_ID_3:
      timer_id = DS3G_TIMER_DOWNLINK_THROUGHPUT_ESTIMATION_SUBS_ID_3;
      break;
#endif
    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_MED, "Not a valid ds3g subs id %d", ds3g_subs_id);
      break;
  }

  /*------------------------------------------------------------------------- 
    Validate the timer 
    -------------------------------------------------------------------------*/
  if ((timer_id < DS3G_TIMER_DOWNLINK_THROUGHPUT_ESTIMATION_MIN)||
      (timer_id >= DS3G_TIMER_DOWNLINK_THROUGHPUT_ESTIMATION_MAX))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "No timer regd for sub ID %d to refresh"
                                      "actual throughput",ds3g_subs_id);
    return ret_val;
  }

  /*------------------------------------------------------------------------- 
    Stop timer if the duration is 0 else start the timer to refresh
    actual throughput
    -------------------------------------------------------------------------*/
  if (dl_estm_throughput_info_ind_freq > 0)
  {
    /*----------------------------------------------------------------------- 
      Actual throughput timer duration changed. Run the timer with
      the new duration
      -----------------------------------------------------------------------*/

    if (ds3gi_is_timer_running(timer_id)) 
     {
        ds3gi_start_timer(timer_id, dl_estm_throughput_info_ind_freq);
    DATA_3GMGR_MSG2(MSG_LEGACY_HIGH, "Started Periodic timer %d for %d seconds",
                    timer_id,dl_estm_throughput_info_ind_freq);
  }
  }
  ret_val = 1;

  ds3g_prepare_downlink_throughput_estimation(ds3g_subs_id);

  return ret_val;
} /* ds3gi_update_downlink_throughput_estimation_ind_frequency() */


/*==============================================================================
FUNCTION      DS3GI_UPDATE_DOWNLINK_THROUGHPUT_ESTIMATION_INFO_IND_INTERVAL

DESCRIPTION   This is a callback function registered with PS per subscription 
              ID, for t_accumulate multiplier change from the client side
 
DEPENDENCIES  None 
 
RETURN VALUE  None

===========================================================================*/
void ds3gi_update_downlink_throughput_estimation_info_ind_interval
(
   ps_sys_tech_enum_type                          tech_type,
   ps_sys_event_enum_type                         event_name,
   ps_sys_subscription_enum_type                  ps_subs_id,
   void                                          *event_info_ptr,
   void                                          *user_data_ptr
)
{
  uint32                       t_accumuate_frequency = 0;
  uint32                       t_accumulate = 0;
  ds3gsubsmgr_subs_id_e_type   ds3g_subs_id = DS3GSUBSMGR_SUBS_ID_INVALID;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------- 
    Validate Arguments
    -------------------------------------------------------------------------*/
  if (event_info_ptr == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, "NULL Argument. Cannot update"
                    "dl estimation throughput Info Ind Interval");
    return;
  }

  if (event_name != PS_SYS_EVENT_UPDATED_DL_THROUGHPUT_TIMER)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, "Ignoring Event %d. Not updating"
                                    "dl estimationthroughput Info timer");
    return;
  }

  /*------------------------------------------------------------------------- 
    Get DS 3G SUB ID from PS Subscription ID
    -------------------------------------------------------------------------*/
  ds3g_subs_id = ds3gsubsmgr_subs_id_ds_to_ds3g((ds3gsubsmgr_subs_id_e_type)ps_subs_id);
  if (!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Invalid DS3G sub ID %d",ds3g_subs_id);
    return;
  }
  /*-------------------------------------------------------------------------
    Get the timer value to be updated
    -------------------------------------------------------------------------*/
  t_accumuate_frequency = *(uint32 *)event_info_ptr;
  ds3g_get_dl_tput_t_accumuate_value(ds3g_subs_id, &t_accumulate);
  t_accumuate_frequency = t_accumuate_frequency*t_accumulate;

  if (FALSE == ds3gi_set_estimated_dl_throughput_interval(ds3g_subs_id,t_accumuate_frequency))
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_ERROR, "Can't set timer duration %d on ds3g subs"
                                      " id %d",t_accumuate_frequency,ds3g_subs_id);
    return;
  }
  /*------------------------------------------------------------------------- 
    Update the timer to refresh actual throughput timer value, based on the
    new throughput Info Ind timer duration
    -------------------------------------------------------------------------*/
  if (ds3gi_update_downlink_throughput_estimation_ind_frequency(ds3g_subs_id) < 0)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, "Can't update actual tput refresh timer");
  }

  return;
} /* ds3gi_update_downlink_throughput_estimation_info_ind_interval() */



/*==============================================================================
FUNCTION      DS3G_UPDATE_ACTUAL_THROUGHPUT_REFRESH_INTERVAL

DESCRIPTION   * Given the current subscription ID, this utility function 
                estimates the duration to periodically refresh the actual
                throughput rate for all the active data calls for the current
                subscription based on the timer duration of the throughput Info
                Indication timer
              * Starts the periodic refresh actual throughput Timer

DEPENDENCIES  None 
 
RETURN VALUE  1   -   Timer updated and started 
              0   -   Timer stopped
              -1  -   Either timer value not updated (or) Actual throughput
                      Timer start attempt failed
 
SIDE EFFECTS  None
===========================================================================*/
static int ds3gi_update_actual_throughput_refresh_interval
(
  ds3gsubsmgr_subs_id_e_type          ds3g_subs_id
)
{
  int                   ret_val                       = -1;
  uint32                throughput_info_ind_duration  =  0;
  uint32                refresh_interval              =  0;
  ds3g_timer_enum_type  timer_id                      =  DS3G_TIMER_MIN;
  uint8                 expiry_count                  =  0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Invalid DS3G sub ID %d",ds3g_subs_id);
    return ret_val;
  }

  /*------------------------------------------------------------------------- 
    Calculate the duration to refresh actual throughput based on the
    duration of reporting throughput Info Indication
    -------------------------------------------------------------------------*/
  throughput_info_ind_duration = 
                          ds3gi_get_throughput_info_ind_interval(ds3g_subs_id);
  if (throughput_info_ind_duration == 0)
  {
    refresh_interval = 0;
  } 
  else if (throughput_info_ind_duration < DS3G_REFRESH_ACTUAL_THROUGHPUT_TIMER_DEF)
  {
    refresh_interval = throughput_info_ind_duration;
  } 
  else
  {
    refresh_interval = DS3G_REFRESH_ACTUAL_THROUGHPUT_TIMER_DEF;
  }
  /*-------------------------------------------------------------------------- 
    Update the new the actual throughput Timer duration
    --------------------------------------------------------------------------*/
  if (FALSE == ds3g_set_actual_throughput_refresh_interval(ds3g_subs_id,
                                                           refresh_interval))
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_MED, "DS 3G subs id %d Failed setting actual "
                                    "t'put refresh duration",ds3g_subs_id,
                                    refresh_interval);
    return ret_val;
  }
  /*------------------------------------------------------------------------- 
    Acquire the timer ID to start the timer based on the subscription ID
    -------------------------------------------------------------------------*/
  switch (ds3g_subs_id)
  {
    case DS3GSUBSMGR_SUBS_ID_1:
      timer_id = DS3G_TIMER_REFRESH_ACTUAL_THROUGHPUT_SUBS_ID_1;
      break;
    case DS3GSUBSMGR_SUBS_ID_2:
      timer_id = DS3G_TIMER_REFRESH_ACTUAL_THROUGHPUT_SUBS_ID_2;
      break;
#if defined(FEATURE_TRIPLE_SIM)
    case DS3GSUBSMGR_SUBS_ID_3:
      timer_id = DS3G_TIMER_REFRESH_ACTUAL_THROUGHPUT_SUBS_ID_3;
      break;
#endif
    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_MED, "Not a valid ds3g subs id %d", ds3g_subs_id);
      break;
  }

  /*------------------------------------------------------------------------- 
    Validate the timer 
    -------------------------------------------------------------------------*/
  if ((timer_id < DS3G_TIMER_REFRESH_ACTUAL_THROUGHPUT_MIN)||
      (timer_id >= DS3G_TIMER_REFRESH_ACTUAL_THROUGHPUT_MAX))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "No timer regd for sub ID %d to refresh"
                                      "actual throughput",ds3g_subs_id);
    return ret_val;
  }

  /*------------------------------------------------------------------------- 
    Stop timer if the duration is 0 else start the timer to refresh
    actual throughput
    -------------------------------------------------------------------------*/
  if (refresh_interval == 0)
  {
    ds3gi_stop_timer(timer_id);

    DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "Stopping Timer %d",timer_id);

    ret_val = 0;
  }
  else
  {
    /*----------------------------------------------------------------------- 
      Actual throughput timer duration changed. Run the timer with
      the new duration
      -----------------------------------------------------------------------*/
    ds3gi_start_timer(timer_id,refresh_interval);

    DATA_3GMGR_MSG2(MSG_LEGACY_HIGH, "Started Periodic timer %d for %d seconds",
                    timer_id,refresh_interval);
    /*----------------------------------------------------------------------- 
      Set the expiry count to 0. This will initialize the bearer throughput
      stats on the Mode Handlers
      -----------------------------------------------------------------------*/
    ds3g_refresh_actual_throughput(ds3g_subs_id, expiry_count);
    ret_val = 1;
  }

  return ret_val;
} /* ds3gi_update_actual_throughput_refresh_interval() */

/*==============================================================================
FUNCTION      DS3GI_UPDATE_THROUGHPUT_INFO_IND_INTERVAL

DESCRIPTION   This is a callback function registered with PS per subscription 
              ID, that gets triggered, when an active client sets a timer
              value 'x' (msec) to periodically report throughput Info indication
              When
              'x' != 0   the throughput Info indication timer is started to
              periodically expire and generate Indication
              'x'  = 0   The throughput Info Indication timer is stopped, if
              already running ==> Reporting throughput Indication for the
              current subscription ID is stopped
 
DEPENDENCIES  None 
 
RETURN VALUE  None

===========================================================================*/
void ds3gi_update_throughput_info_ind_interval
(
   ps_sys_tech_enum_type                          tech_type,
   ps_sys_event_enum_type                         event_name,
   ps_sys_subscription_enum_type                  ps_subs_id,
   void                                          *event_info_ptr,
   void                                          *user_data_ptr
)
{
  uint32                       timer_val      = 0;
  ds3gsubsmgr_subs_id_e_type   ds3g_subs_id   = DS3GSUBSMGR_SUBS_ID_INVALID;
  ds3g_timer_enum_type         timer_id       = DS3G_TIMER_MIN;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------- 
    Validate Arguments
    -------------------------------------------------------------------------*/
  if (event_info_ptr == NULL)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, "NULL Argument. Cannot update"
                    "throughput Info Ind Interval");
    return;
  }

  if (event_name != PS_SYS_EVENT_UPDATED_THROUGHPUT_TIMER)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, "Ignoring Event %d. Not updating"
                                    "throughput Info timer");
    return;
  }

  /*------------------------------------------------------------------------- 
    Get DS 3G SUB ID from PS Subscription ID
    -------------------------------------------------------------------------*/
  ds3g_subs_id = ds3gsubsmgr_subs_id_ds_to_ds3g((ds3gsubsmgr_subs_id_e_type)ps_subs_id);
  if (!ds3gsubsmgr_is_ds3g_subs_index_valid(ds3g_subs_id))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Invalid DS3G sub ID %d",ds3g_subs_id);
    return;
  }
  /*-------------------------------------------------------------------------
    Get the timer value to be updated
    -------------------------------------------------------------------------*/
  timer_val = *(uint32 *)event_info_ptr;
  if (FALSE == ds3gi_set_throughput_info_ind_interval(ds3g_subs_id,timer_val))
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_ERROR, "Can't set timer duration %d on ds3g subs"
                                      " id %d",timer_val,ds3g_subs_id);
    return;
  }
  /*------------------------------------------------------------------------- 
    Update the timer to refresh actual throughput timer value, based on the
    new throughput Info Ind timer duration
    -------------------------------------------------------------------------*/
  if (ds3gi_update_actual_throughput_refresh_interval(ds3g_subs_id) < 0)
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, "Can't update actual tput refresh timer");
  }

  /*------------------------------------------------------------------------- 
    Acquire the timer ID to start based on the subscription ID
    -------------------------------------------------------------------------*/
  switch (ds3g_subs_id)
  {
    case DS3GSUBSMGR_SUBS_ID_1:
      timer_id = DS3G_TIMER_REPORT_THROUGHPUT_INFO_IND_SUBS_ID_1;
      break;
    case DS3GSUBSMGR_SUBS_ID_2:
      timer_id = DS3G_TIMER_REPORT_THROUGHPUT_INFO_IND_SUBS_ID_2;
      break;
#if defined(FEATURE_TRIPLE_SIM)
    case DS3GSUBSMGR_SUBS_ID_3:
      timer_id = DS3G_TIMER_REPORT_THROUGHPUT_INFO_IND_SUBS_ID_3;
      break;
#endif
    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_MED,"Invalid ds3g subs id %d", ds3g_subs_id);
      break;
  }
  /*------------------------------------------------------------------------- 
    validate the timer ID for the current subscription 
    -------------------------------------------------------------------------*/
  if ((timer_id < DS3G_TIMER_REPORT_THROUGHPUT_INFO_IND_MIN)||
      (timer_id >= DS3G_TIMER_REPORT_THROUGHPUT_INFO_IND_MAX))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,"No timer registered for ds3g sub %d."
                                     "t'put Info Indication", ds3g_subs_id);
    return;
  }
  /*------------------------------------------------------------------------- 
    Decide whether to start (or) stop the timer.
    Start the timer  - (If) the new timer duration received is non zero
    Stop the timer  - Otherwise
    -------------------------------------------------------------------------*/
  if (timer_val == 0)
  {
    ds3gi_stop_timer(timer_id);
  }
  else
  {
    /*-----------------------------------------------------------------------
      Start the timer to report Throughput Info indication
      -----------------------------------------------------------------------*/
    DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,"Starting Timer %d to periodically expire"
                                     "every %d msec",timer_id,timer_val);
    ds3gi_start_timer(timer_id, timer_val);
  }
  return;
} /* ds3gi_update_throughput_info_ind_interval() */

/*===========================================================================
FUNCTION      DS3GI_TIMER_GET_TIMER_EXP_CNT

DESCRIPTION   For a particulat timer ID, this function returns the number of 
              times the timer has expired before being processed
 
DEPENDENCIES  None

RETURN VALUE  uint16 - Timer expiry count

SIDE EFFECTS  None
===========================================================================*/
static uint16 ds3gi_timer_get_timer_exp_cnt
(
  ds3g_timer_enum_type timer_id
)
{
  uint16        ret_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((timer_id <= DS3G_TIMER_MIN) || (timer_id >=DS3G_TIMER_MAX))
  {
    return ret_val;
  }

  rex_enter_crit_sect(&ds3g_timer_crit_sect);
  ret_val = ds3gi_timer_tbl[timer_id].timer_exp_cmd_cnt;
  rex_leave_crit_sect(&ds3g_timer_crit_sect);

  return  ret_val;
} /* ds3gi_timer_get_timer_exp_cnt() */

/*===========================================================================
FUNCTION      DS3GI_TIMER_INCR_TIMER_EXP_CNT

DESCRIPTION   For a particulat timer ID, this function increments the timer 
              expiry count by one
 
DEPENDENCIES  None

RETURN VALUE  TRUE - Timer Expiry count has been incremented successfully

SIDE EFFECTS  None
===========================================================================*/
static boolean ds3gi_timer_inc_timer_exp_cnt(ds3g_timer_enum_type timer_id)
{
  boolean  ret_val = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((timer_id <= DS3G_TIMER_MIN) || (timer_id >=DS3G_TIMER_MAX))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Invalid Timer ID %d",timer_id);
    return FALSE;
  }

  /*------------------------------------------------------------------------- 
    Increment the timer expiry count
    -------------------------------------------------------------------------*/
  rex_enter_crit_sect(&ds3g_timer_crit_sect);
  ds3gi_timer_tbl[timer_id].timer_exp_cmd_cnt++;
  rex_leave_crit_sect(&ds3g_timer_crit_sect);

  return ret_val;
} /* ds3gi_timer_inc_timer_exp_cnt() */

/*===========================================================================
FUNCTION      DS3GI_TIMER_CLR_TIMER_EXP_CNT

DESCRIPTION   For a particulat timer ID, this function resets the timer 
              expiry count. The count will be cleared, when the command posted
              for timer expiry gets processed
 
DEPENDENCIES  None

RETURN VALUE  TRUE - Timer Expiry count has been cleared successfully

SIDE EFFECTS  None
===========================================================================*/
static boolean ds3gi_timer_clr_timer_exp_cnt
(
  ds3g_timer_enum_type timer_id
)
{
  boolean  ret_val = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((timer_id <= DS3G_TIMER_MIN) || (timer_id >=DS3G_TIMER_MAX))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "not a valid Timer ID %d",timer_id);
    return FALSE;
  }
  /*------------------------------------------------------------------------- 
    Increment the timer expiry count
    -------------------------------------------------------------------------*/
  rex_enter_crit_sect(&ds3g_timer_crit_sect);
  ds3gi_timer_tbl[timer_id].timer_exp_cmd_cnt = 0;
  rex_leave_crit_sect(&ds3g_timer_crit_sect);
  return  ret_val;
} /* ds3gi_timer_clr_timer_exp_cnt() */

/*===========================================================================
FUNCTION      DS3GI_TIMER_CB

DESCRIPTION   This is a Callback function registered with REX/MCS timer  to
              notify DS of timer expirations. This function posts a command to
              DS task upon timer expiry with the timer ID for the expired timer

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds3gi_timer_cb
(
  unsigned long  timer_id      /* Used to identify which timer has expired */
)
{
  ds_cmd_type                  *cmd_ptr = NULL;
  ds3g_timer_expired_cmd_type  *timer_expired_cmd_ptr = NULL;
  ds3g_timer_enum_type          timer   = timer_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ((timer <= DS3G_TIMER_MIN)||(timer >= DS3G_TIMER_MAX))
  {
    return;
  }
  DATA_3GMGR_MSG2(MSG_LEGACY_MED,"Timer Id %d expired. Exp cnt %d", timer,
                  ds3gi_timer_get_timer_exp_cnt(timer));

  /*------------------------------------------------------------------------- 
   Check the number of times the timer has expired previously since the last
   time the timer expiry processing was done. If the timer has expired more
   than once, then probably there is already a command waiting in the queue
   to be processed. In that case we do not have to post command to DS task,
   if the command is already in the queue and waiting to be processed
    -------------------------------------------------------------------------*/
  if (ds3gi_timer_get_timer_exp_cnt(timer) > 0)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Timer expiry cmd for timer id %d"
                    "already in cmd queue. Skip posting cmd again",timer);
    ds3gi_timer_inc_timer_exp_cnt(timer);
    return;
  }
  /*-------------------------------------------------------------------------
    Command already not in command queue. Post the command to DS task
  -------------------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds3g_timer_expired_cmd_type));
  if ( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  /*------------------------------------------------------------------------- 
    Increment the timer expiry count before posting
    -------------------------------------------------------------------------*/
  ds3gi_timer_inc_timer_exp_cnt(timer);

  /*------------------------------------------------------------------------- 
    Send a DS_TIMER_EXPIRED_CMD to the DS task, and indicate which timer
    expired.
    -------------------------------------------------------------------------*/
  cmd_ptr->hdr.cmd_id = DS_CMD_TIMER_EXPIRED;
  timer_expired_cmd_ptr = cmd_ptr->cmd_payload_ptr;
  timer_expired_cmd_ptr->timer_id = timer;
  ds_put_cmd( cmd_ptr ); 

  return;
} /* ds3gi_timer_cb() */

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION      DS3GI_CONFIGURE_TIMER_PERIODICITY

DESCRIPTION   This utility can be used to set (or) unset the periodicity of 
              the timer. (one time (or) recurring timer runs)

DEPENDENCIES  Currently used for MCS timer types only

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_configure_timer_periodicity
( 
  ds3g_timer_enum_type  timer_id,
  boolean               is_periodic_tmr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Validate Timer ID
  -------------------------------------------------------------------------*/
  if ((timer_id <= DS3G_TIMER_MIN) ||
      (timer_id >= DS3G_TIMER_MAX))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,"Bad timer id %d to set periodicity",
                    timer_id);
    return;
  }
  /*-------------------------------------------------------------------------
    Validate Timer Type
    -------------------------------------------------------------------------*/
  if ((ds3gi_timer_tbl[timer_id].timer_type <= DS3G_TIMER_TYPE_INVALID)||
      (ds3gi_timer_tbl[timer_id].timer_type >= DS3G_TIMER_TYPE_MAX_TYPE))
  {
    DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,"Invalid tmrtype %d Can't set periodicity"
                    "for timer %d",ds3gi_timer_tbl[timer_id].timer_type,
                    timer_id);
    return;
  }
  /*-------------------------------------------------------------------------
    Validate Timer object
    -------------------------------------------------------------------------*/
  if (ds3gi_timer_tbl[timer_id].timer_ptr == NULL)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Trying to set periodicity for unregd"
                                      "timer %d",timer_id);
    return;
  }
  /*----------------------------------------------------------------------- 
    Set the periodicity flag
    -----------------------------------------------------------------------*/
  DATA_3GMGR_MSG2(MSG_LEGACY_ERROR, "Setting timer %d periodicity %d",
                  timer_id,is_periodic_tmr);
  ds3gi_timer_tbl[timer_id].is_periodic_timer = is_periodic_tmr;

  return;
} /* ds3gi_configure_timer_periodicity() */

/*===========================================================================
FUNCTION      DS3GI_TIMER_INIT

DESCRIPTION   This function initializes the timer services. This function must 
              be called once, at Data Services Task startup.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds3gi_timer_init( void )
{
  unsigned long i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i = 0; i <(unsigned long)DS3G_TIMER_MAX; i++)
  {
    ds3gi_timer_tbl[i].timer_ptr         = NULL;
    ds3gi_timer_tbl[i].timer_type        = DS3G_TIMER_TYPE_INVALID;
    ds3gi_timer_tbl[i].is_periodic_timer = FALSE;
  }
  return;
} /* ds3gi_timer_init() */

/*===========================================================================
FUNCTION      DS3GI_TIMER_PROCESS_INIT_COMPLETE_CMD

DESCRIPTION   This function registers for PS sys events and timer objects

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds3gi_timer_process_init_complete_cmd( void )
{
  int16                   dss_errno                 = 0;
  ds3g_timer_enum_type    throughput_ind_timer      = DS3G_TIMER_MIN;
  ds3g_timer_enum_type    refrsh_act_throughput_tmr = DS3G_TIMER_MIN;
  ds3g_timer_enum_type    dl_tput_est_tmr           = DS3G_TIMER_MIN;
  uint8                   index                     = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*========================================================================= 
    REGISTER FOR ALL TIMERS AND EVENTS PER SUBSCRIPTION ID
    =========================================================================*/
  for (index = 0; index < DS3GSUBSMGR_SUBS_ID_MAX;index++) 
  {
    throughput_ind_timer      = DS3G_TIMER_REPORT_THROUGHPUT_INFO_IND_MIN+index;
    refrsh_act_throughput_tmr = DS3G_TIMER_REFRESH_ACTUAL_THROUGHPUT_MIN+index;
    dl_tput_est_tmr           = DS3G_TIMER_DOWNLINK_THROUGHPUT_ESTIMATION_MIN+index;
    /*------------------------------------------------------------------------- 
      REGISTER THROUGPUT INFORMATION IND TIMER EXPIRY CB
      -------------------------------------------------------------------------*/
    
    memset(&ds3g_throughput_info_timer[index],0,sizeof(mcs_timer_type));
    memset(&ds3g_refresh_actual_throughput_timer[index],0,sizeof(mcs_timer_type));
    memset(&ds3g_estimated_dl_throughput_timer[index],0,sizeof(mcs_timer_type));

    ds3gi_timer_register((void *)&ds3g_throughput_info_timer[index],
                         ds3gi_timer_cb,
                         throughput_ind_timer,
                         DS3G_TIMER_TYPE_MCS_TIMER);

    ds3gi_configure_timer_periodicity(throughput_ind_timer, TRUE);

    /*------------------------------------------------------------------------- 
      REGISTER TIMER TO REFRESH ACTUAL THROUGHPUT TIMER EXPIRY CB
      -------------------------------------------------------------------------*/

    ds3gi_timer_register((void *)&ds3g_refresh_actual_throughput_timer[index],
                         ds3gi_timer_cb,
                         refrsh_act_throughput_tmr,
                         DS3G_TIMER_TYPE_MCS_TIMER);

    ds3gi_configure_timer_periodicity(refrsh_act_throughput_tmr, TRUE);

    /*------------------------------------------------------------------------- 
      REGISTER TIMER TO SEND ESTIMATED DL THROUGHPUT TIMER EXPIRY CB
      -------------------------------------------------------------------------*/

    ds3gi_timer_register((void *)&ds3g_estimated_dl_throughput_timer[index],
                         ds3gi_timer_cb,
                         dl_tput_est_tmr,
                         DS3G_TIMER_TYPE_MCS_TIMER);

    ds3gi_configure_timer_periodicity(dl_tput_est_tmr, TRUE);

    /*------------------------------------------------------------------------- 
      Initialize Both Throughput Info Indication refresh actual throughput
      timer duration
      -------------------------------------------------------------------------*/

    ds3g_actual_throughput_interval[index]   = 0;
    ds3g_throughput_info_ind_interval[index] = 0;
    ds3g_estimated_dl_throughput_interval[index] = 0;

    /*------------------------------------------------------------------------- 
      Register for interested System Events soon after Init is complete
      -------------------------------------------------------------------------*/
    if (0 > ps_sys_event_reg_ex(PS_SYS_TECH_ALL,
                                PS_SYS_EVENT_UPDATED_THROUGHPUT_TIMER,
          (ps_sys_subscription_enum_type)ds3gsubsmgr_subs_id_ds3g_to_ds(index),
                                ds3gi_update_throughput_info_ind_interval,
                                NULL,
                                &dss_errno))
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,"Couldn't reg for UPDATED_THRUPUT_TIMER"
                    "event Err : (%d)", dss_errno);
    }

    if (0 > ps_sys_event_reg_ex(PS_SYS_TECH_ALL,
                                PS_SYS_EVENT_UPDATED_DL_THROUGHPUT_TIMER,
          (ps_sys_subscription_enum_type)ds3gsubsmgr_subs_id_ds3g_to_ds(index),
                                ds3gi_update_downlink_throughput_estimation_info_ind_interval,
                                NULL,
                                &dss_errno))
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,"Couldn't reg for UPDATED_DL_THROUGHPUT_TIMER"
                    "event Err : (%d)", dss_errno);
    }
    
  }
} /* ds3gi_timer_process_init_complete_cmd() */

/*===========================================================================
FUNCTION      DS3GI_TIMER_REGISTER

DESCRIPTION   Clients register their timer info with timer services. 
              The passed in ID must come from the ds3g_timer_enum_type.Once
              registered, the timers cannot be de-registered (though they can
              be stopped/started).
 
              Registers the timer with REX (or) MCS.  Timer is not started
              during registration

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds3gi_timer_register
(
  void                  *timer_ptr,
  void                  *timer_cb_f_ptr,
  ds3g_timer_enum_type   timer_id,
  ds3g_timer_type_e      timer_type
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
    Validate Timer ID
    -----------------------------------------------------------------------*/
  if ((timer_id <= DS3G_TIMER_MIN) ||(timer_id >= DS3G_TIMER_MAX))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,"Bad timer id on init %d",timer_id);
    ASSERT(0);
    return;
  }
  /*------------------------------------------------------------------------- 
    Check timer type
    -------------------------------------------------------------------------*/
  if ((timer_type <= DS3G_TIMER_TYPE_INVALID)||
      (timer_type >= DS3G_TIMER_TYPE_MAX_TYPE))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,"Invalid timer type %d",timer_type);
    ASSERT(0);
    return;
  }
  /*-------------------------------------------------------------------------
    Check if a timer object is already registered for the timer ID
    -------------------------------------------------------------------------*/
  if (ds3gi_timer_tbl[timer_id].timer_ptr != NULL)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,"Timer already reg'd at loc %d",timer_id);
    return;
  }
  /*------------------------------------------------------------------------- 
    Define/Create timers based on the timer type
    -------------------------------------------------------------------------*/
  ds3gi_timer_tbl[timer_id].timer_ptr   = timer_ptr;
  ds3gi_timer_tbl[timer_id].timer_type  = timer_type;

  if (timer_type == DS3G_TIMER_TYPE_REX_TIMER)
  {
    rex_def_timer_ex(timer_ptr, (rex_timer_cb_type)timer_cb_f_ptr,
                     (unsigned long) timer_id);
  }
  else if (timer_type == DS3G_TIMER_TYPE_MCS_TIMER)
  {
    mcs_timer_def(timer_ptr, NULL, (mcs_timer_t1_cb_type)timer_cb_f_ptr,
                  (mcs_timer_cb_data_type)timer_id);
  }
  else
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,"Cannot create timer with unknown timer"
                                     "type %d",timer_type);
  }
  return;
} /* ds3gi_timer_register() */

/*===========================================================================
FUNCTION      DS3GI_START_TIMER

DESCRIPTION   This function starts the specified timer for the specified 
               duration.

DEPENDENCIES  A valid registered timer id must be specified, otherwise the 
              timer is not started.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds3gi_start_timer
(
  ds3g_timer_enum_type  timer_id,                   /* Timer to be started */
  dword                 duration         /* Timer duration in milliseconds */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    /*-----------------------------------------------------------------------
        Validate Timer ID
      -----------------------------------------------------------------------*/
    if ((timer_id <= DS3G_TIMER_MIN) ||
        (timer_id >= DS3G_TIMER_MAX))
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,"Bad timer id on start %d",timer_id);
      return;
    }

    /*-----------------------------------------------------------------------
      Validate Timer Type
      -----------------------------------------------------------------------*/
    if ((ds3gi_timer_tbl[timer_id].timer_type <= DS3G_TIMER_TYPE_INVALID)||
        (ds3gi_timer_tbl[timer_id].timer_type >= DS3G_TIMER_TYPE_MAX_TYPE))
    {
      DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,"Invalid timer type %d Can't start tmr"
                      " %d",ds3gi_timer_tbl[timer_id].timer_type,timer_id);
      return ;
    }

    /*-----------------------------------------------------------------------
      Validate Timer object
      -----------------------------------------------------------------------*/
    if (ds3gi_timer_tbl[timer_id].timer_ptr == NULL)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Trying to start unregd timer %d",
                      timer_id);
      return;
    }

    /*----------------------------------------------------------------------- 
      Start timer based on the timer type
      -----------------------------------------------------------------------*/
    if (ds3gi_timer_tbl[timer_id].timer_type == DS3G_TIMER_TYPE_REX_TIMER)
    {
      (void) rex_set_timer(ds3gi_timer_tbl[timer_id].timer_ptr,
                           (rex_timer_cnt_type)duration);
      DATA_3GMGR_MSG1(MSG_LEGACY_MED,"Started Timer %d", timer_id);
    }
    else if (ds3gi_timer_tbl[timer_id].timer_type == DS3G_TIMER_TYPE_MCS_TIMER)
    {
      /*--------------------------------------------------------------------- 
        For MCS Timers, the current behaviour is to clear the timer if its
        already running, before starting the timer
        ---------------------------------------------------------------------*/
      if (ds3gi_is_timer_running(timer_id))
      {
        ds3gi_stop_timer(timer_id);
      }

      /*--------------------------------------------------------------------- 
        Start the MCS timer
        ---------------------------------------------------------------------*/

      if (ds3gi_is_timer_periodic(timer_id) == TRUE)
      {
        mcs_timer_set( ds3gi_timer_tbl[timer_id].timer_ptr,
                       (timetick_type)duration, 
                       duration, 
                       T_MSEC
                     );
      }
      else
      {
        mcs_timer_set( ds3gi_timer_tbl[timer_id].timer_ptr,
                       (timetick_type)duration,
                       0,
                       T_MSEC);
      }
    }
  }while (0);

  return; 
} /* ds3gi_start_timer() */

/*===========================================================================
FUNCTION      DS3GI_STOP_TIMER

DESCRIPTION   This function stops the specified timer.

DEPENDENCIES  A valid registered timer id must be specified, otherwise no action is taken.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds3gi_stop_timer
(
  ds3g_timer_enum_type  timer_id                    /* Timer to be stopped */
)
{
  uint32  tmsec_left = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    /*----------------------------------------------------------------------- 
      Validate Timer ID
      -----------------------------------------------------------------------*/

    if ((timer_id <= DS3G_TIMER_MIN) ||(timer_id >= DS3G_TIMER_MAX))
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,"Bad timer id on stop %d",timer_id);
      return;
    }
    /*----------------------------------------------------------------------- 
      Validate Timer Type
      -----------------------------------------------------------------------*/
    if ((ds3gi_timer_tbl[timer_id].timer_type <= DS3G_TIMER_TYPE_INVALID)||
        (ds3gi_timer_tbl[timer_id].timer_type >= DS3G_TIMER_TYPE_MAX_TYPE))
    {
      DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,
                      "Invalid timer type %d Cannot stop timer %d",
                      ds3gi_timer_tbl[timer_id].timer_type,timer_id);
      return ;
    }
    /*----------------------------------------------------------------------- 
      Validate Timer object
      -----------------------------------------------------------------------*/
    if (ds3gi_timer_tbl[timer_id].timer_ptr == NULL)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Trying stop unregd timer %d",timer_id);
      return;
    }
    /*----------------------------------------------------------------------- 
      Check if the timer has already stopped
      -----------------------------------------------------------------------*/
    if (ds3gi_is_timer_running(timer_id) == FALSE)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_HIGH, "Timer not running Skip stopping"
                                       "timer %d",timer_id);
      break;
    }
    /*----------------------------------------------------------------------- 
      Stop the timer, based on the timer type
      -----------------------------------------------------------------------*/
    if (ds3gi_timer_tbl[timer_id].timer_type == DS3G_TIMER_TYPE_REX_TIMER)
    {
      (void) rex_clr_timer(ds3gi_timer_tbl[timer_id].timer_ptr);
      DATA_3GMGR_MSG1(MSG_LEGACY_MED,"Stopped Timer %d", timer_id);
    }
    else if (ds3gi_timer_tbl[timer_id].timer_type == DS3G_TIMER_TYPE_MCS_TIMER)
    {
      tmsec_left = (uint32) mcs_timer_clr(ds3gi_timer_tbl[timer_id].timer_ptr,
                                          T_MSEC);
      DATA_3GMGR_MSG2(MSG_LEGACY_HIGH,"MCS Timer ID %d stopped before %d msec"
                      "to expire", timer_id, tmsec_left);
    }
    else
    {
      /*----------------------------------------------------------------------- 
        Not a valid timer type. Cannot stop timer
        -----------------------------------------------------------------------*/
    }
  }while (0); 

  return;
} /* ds3gi_stop_timer() */

/*===========================================================================
FUNCTION      DS3GI_IS_TIMER_RUNNING

DESCRIPTION   This function reports status of specified timer.

DEPENDENCIES  A valid timer id must be specified, otherwise no action is taken.

RETURN VALUE  TRUE if timer is active, FALSE otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean ds3gi_is_timer_running
(
  ds3g_timer_enum_type  timer_id                      /* Timer of interest */
)
{
  boolean ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    /*-----------------------------------------------------------------------
        Validate Timer ID
      -----------------------------------------------------------------------*/
    if ((timer_id <= DS3G_TIMER_MIN) ||(timer_id >= DS3G_TIMER_MAX))
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,"Bad timer id on stop %d",timer_id);
      return ret_val;
    }
    /*----------------------------------------------------------------------- 
      Validate Timer Type
      -----------------------------------------------------------------------*/
    if ((ds3gi_timer_tbl[timer_id].timer_type <= DS3G_TIMER_TYPE_INVALID)||
        (ds3gi_timer_tbl[timer_id].timer_type >= DS3G_TIMER_TYPE_MAX_TYPE))
    {
      DATA_3GMGR_MSG2(MSG_LEGACY_ERROR,
                      "Invalid timer type %d Cannot check timer %d run status",
                      ds3gi_timer_tbl[timer_id].timer_type,timer_id);
      return ret_val;

    }
    /*----------------------------------------------------------------------- 
      Validate Timer object
      -----------------------------------------------------------------------*/
    if (ds3gi_timer_tbl[timer_id].timer_ptr == NULL)
    {
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Cant check status of unregd "
                                        "timer %d",timer_id);
      return ret_val;
    }
    /*------------------------------------------------------------------------- 
      Depending on the timer type, check if the timer is running at present
      -------------------------------------------------------------------------*/
    if (ds3gi_timer_tbl[timer_id].timer_type == DS3G_TIMER_TYPE_REX_TIMER)
    {
      ret_val = (0 != rex_get_timer(ds3gi_timer_tbl[timer_id].timer_ptr));
    }
    else if (ds3gi_timer_tbl[timer_id].timer_type == DS3G_TIMER_TYPE_MCS_TIMER)
    {
      ret_val = (0 != mcs_timer_get(ds3gi_timer_tbl[timer_id].timer_ptr,T_MSEC));
    }
    else
    {
      /*----------------------------------------- 
        Invalid Timer type. Return 'Not Running'
        -----------------------------------------*/
    }
  }while (0); 

  return ret_val;
} /* ds3gi_is_timer_running() */

/*===========================================================================
FUNCTION      DS3GI_IS_TIMER_PERIODIC

DESCRIPTION   This function returns if the input timer is periodic or not
 
DEPENDENCIES  None

RETURN VALUE  TRUE  - If the timer is periodic 
              FALSE - IF the timer is not periodic

SIDE EFFECTS  None
===========================================================================*/
boolean ds3gi_is_timer_periodic
(
  ds3g_timer_enum_type      timer_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return (ds3gi_timer_tbl[timer_id].is_periodic_timer);
} /* ds3gi_is_timer_periodic() */

/*===========================================================================
FUNCTION      DS3GI_PROCESS_TIMER_EXPIRED_CMD

DESCRIPTION   This function processes the DS_TIMER_EXPIRED_CMD. It determines 
              which timer has expired, and calls the appropriate function to
              handle the timer expiry.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds3gi_process_timer_expired_cmd
(
  ds_cmd_type  *cmd_ptr                              /* Pointer to command */
)
{
#if !(defined(FEATURE_DATA_PS_CMI_REARCH) && defined(IMAGE_APPS_PROC))
  ds3g_siolib_status_e_type     ret_val;
  ds3g_siolib_port_e_type       port_id;
  ds3g_siolib_portid_e_type     port_index;  
  ds3g_timer_enum_type          timer_id      = DS3G_TIMER_MIN;
  uint16                        timer_exp_cnt = 0;
  ds3g_timer_expired_cmd_type  *timer_expired_cmd_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ( cmd_ptr == NULL ) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, "NULL cmd_ptr or payload ptr passed");
    return;
  }
  if(cmd_ptr->hdr.cmd_id != DS_CMD_TIMER_EXPIRED)
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Invalid command:%d",
                    cmd_ptr->hdr.cmd_id);
    return;
  }
  /*------------------------------------------------------------------------- 
    Check if the timer id is valid
    -------------------------------------------------------------------------*/
  timer_expired_cmd_ptr = cmd_ptr->cmd_payload_ptr;
  timer_id = timer_expired_cmd_ptr->timer_id;

  if ((timer_id <= DS3G_TIMER_MIN)||(timer_id >=DS3G_TIMER_MAX))
  {
    DATA_3GMGR_MSG1(MSG_LEGACY_ERROR,
                    "Invalid Timer ID %d to process timer expiry", timer_id);
    return;
  }

  /*------------------------------------------------------------------------- 
    Cache the timer expiry count before clearing
    -------------------------------------------------------------------------*/
  timer_exp_cnt = ds3gi_timer_get_timer_exp_cnt(timer_id);
  ds3gi_timer_clr_timer_exp_cnt(timer_id);
  DATA_3GMGR_MSG2(MSG_LEGACY_MED,"Current Timer ID %d expiry cnt %d"
                                   ,timer_id,timer_exp_cnt);
  /* Lint error handler */
  DATA_MDM_ARG_NOT_CONST(cmd_ptr);

  /*-------------------------------------------------------------------------
    Call the appropriate function, based on the timer that expired.
    -------------------------------------------------------------------------*/
  switch(timer_id)
  {
    case DS3G_TIMER_RING_TE_1:
    case DS3G_TIMER_RING_TE_2:
      ds3g_siolib_alert_user( ds3g_siolib_get_ds3g_siolib_current_ring_type(), 
                              TRUE );
      break;

    case DS3G_TIMER_WINK_DCD_1:
      port_index = DS3G_SIOLIB_PORTID_0;
      if (ds3g_siolib_port_allocated[port_index] == DS3G_SIOLIB_PORT_NONE)
      {
        port_id = ds3g_siolib_last_port_allocated[port_index];
      }
      else
      {
        port_id = ds3g_siolib_port_allocated[port_index];
      }
      ret_val = ds3g_siolib_ex_set_cd_state( DS3G_SIOLIB_CD_WINKING_END, 
                                             port_id );

      if(ret_val == DS3G_SIOLIB_NO_PORT_OPEN)
      {
        DATA_3GMGR_MSG0(MSG_LEGACY_ERROR,
                        "siolib_set_cd_state returned SIOLIB_NO_PORT_OPEN");
      }
      break;

     case DS3G_TIMER_WINK_DCD_2:
       port_index = DS3G_SIOLIB_PORTID_1;
       if (ds3g_siolib_port_allocated[port_index] == DS3G_SIOLIB_PORT_NONE)
       {
         port_id = ds3g_siolib_last_port_allocated[port_index];
       }
       else
       {
         port_id = ds3g_siolib_port_allocated[port_index];
       }
       ret_val = ds3g_siolib_ex_set_cd_state( DS3G_SIOLIB_CD_WINKING_END, 
                                              port_id );

       if(ret_val == DS3G_SIOLIB_NO_PORT_OPEN)
       {
         DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, 
                         "siolib_set_cd_state returned SIOLIB_NO_PORT_OPEN");
       }
       break;

     case DS3G_TIMER_REPORT_THROUGHPUT_INFO_IND_SUBS_ID_1:
       ds3g_report_throughput_info(DS3GSUBSMGR_SUBS_ID_1);
       break;

     case DS3G_TIMER_REFRESH_ACTUAL_THROUGHPUT_SUBS_ID_1:
       ds3g_refresh_actual_throughput(DS3GSUBSMGR_SUBS_ID_1,timer_exp_cnt);
       break;
#if defined(FEATURE_DUAL_SIM) || defined (FEATURE_TRIPLE_SIM)
     case DS3G_TIMER_REPORT_THROUGHPUT_INFO_IND_SUBS_ID_2:
       ds3g_report_throughput_info(DS3GSUBSMGR_SUBS_ID_2);
       break;
     case DS3G_TIMER_REFRESH_ACTUAL_THROUGHPUT_SUBS_ID_2:
       ds3g_refresh_actual_throughput(DS3GSUBSMGR_SUBS_ID_2,timer_exp_cnt);
       break;
#if defined(FEATURE_TRIPLE_SIM)
     case DS3G_TIMER_REPORT_THROUGHPUT_INFO_IND_SUBS_ID_3:
       ds3g_report_throughput_info(DS3GSUBSMGR_SUBS_ID_3);
       break;
     case DS3G_TIMER_REFRESH_ACTUAL_THROUGHPUT_SUBS_ID_3:
       ds3g_refresh_actual_throughput(DS3GSUBSMGR_SUBS_ID_3,timer_exp_cnt);
       break;
#endif
#endif

    case DS3G_TIMER_DOWNLINK_THROUGHPUT_ESTIMATION_SUBS_ID_1:
       ds3g_send_downlink_throughput_estimation_ind(DS3GSUBSMGR_SUBS_ID_1);
       break;
     case DS3G_TIMER_DOWNLINK_THROUGHPUT_ESTIMATION_SUBS_ID_2:
       ds3g_send_downlink_throughput_estimation_ind(DS3GSUBSMGR_SUBS_ID_2);
       break;
#if defined(FEATURE_TRIPLE_SIM)
     case DS3G_TIMER_DOWNLINK_THROUGHPUT_ESTIMATION_SUBS_ID_3:
       ds3g_send_downlink_throughput_estimation_ind(DS3GSUBSMGR_SUBS_ID_3);
       break;
#endif
    default:
      DATA_3GMGR_MSG1(MSG_LEGACY_ERROR, "Timer expired w/ bad timer id %d",
                      timer_id);
      break;
  } /* switch( timer_id ) */
#else
  DATA_3GMGR_MSG0(MSG_LEGACY_HIGH,"Processor timer expired not supported");
#endif /* !(FEATURE_DATA_PS_CMI_REARCH && IMAGE_APPS_PROC) */

  return;
} /* ds3gi_process_timer_expired_cmd() */