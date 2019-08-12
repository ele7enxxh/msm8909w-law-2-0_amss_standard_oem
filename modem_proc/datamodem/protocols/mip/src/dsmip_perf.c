/*===========================================================================
                                      
                           D S M I P _ P E R F . C

DESCRIPTION

  The Data Services Mobile IP call performance implementation.  Contains the
  API and internal functions to determine the various delays that are 
  involved in the Mobile IP call metrics. 

EXTERNALIZED FUNCTIONS

  mip_perf_init()
    Initializes Mobile IP performance timers.
     
  mip_perf_delay()
    Sets start and end times for specified delay.

  mip_get_perf_info_ptr()
    Access funtion for mip_perf_info array.


Copyright (c) 2002-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_perf.c_v   1.2   16 Jul 2002 15:20:28   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/src/dsmip_perf.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
                          
                          
when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/20/11    op     Fixed error when FEATURE_DS_MOBILE_IP_PERF is disabled
05/20/11    op     Migrated to MSG 2.0 macros
03/23/11    op     Data SU API cleanup
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
03/15/10    gc     remove CMI violation ts.h, add time_svc.h and change API
10/15/09    vs     SU Level Api Defeaturization
04/20/04    jd     Added logging support for MIP performance delays
11/10/03    igt    Added PERF_CANCEL_TS to the mip_perf_delay() to be able 
                   to reset timers for specific delays when conditions warrant.
02/19/02    sjy    Now featurized under FEATURE_DS_MOBILE_IP_PERF
01/24/02    sjy    Created module
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include "dsmip_api.h"
#include "data_msg.h"

#ifdef FEATURE_DS_MOBILE_IP_PERF

#include "dsmip_perf.h"
#include "time_svc.h"
#include "time_tod.h"
#include "time_svc.h"

#include "log.h"
#include "log_codes.h"


/*===========================================================================

                             INTERNAL DATA 

===========================================================================*/

#ifndef T_ARM
/*---------------------------------------------------------------------------
  These names need to be kept in sync with the enums which they describe.
---------------------------------------------------------------------------*/
static char *perf_delay_names[PERF_MAX_DELAY] =
{
  "Um PPP delay",
  "Rm PPP delay",
  "M.IP reg delay",
  "M.IP re-reg delay",
  "Total call setup delay",
  "Total handoff delay"
};
#endif

/*---------------------------------------------------------------------------
  Mobile IP performance info array.
---------------------------------------------------------------------------*/
static mip_perf_ts_info_type mip_perf_info[PERF_MAX_DELAY];
   

/*---------------------------------------------------------------------------
  The null time stamp.  The start time stamp variable for each delay is 
  given this value initially.
---------------------------------------------------------------------------*/
static qword ts_null;

/*---------------------------------------------------------------------------
  Packet structure definition for extended PPP logging
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(LOG_MOBILE_IP_PERFORMANCE_C)
  uint32               delay_value;       /* measured delay value          */
  mip_perf_delay_type  delay_type;        /* which delay is being reported */
LOG_RECORD_END

/*---------------------------------------------------------------------------
  Log entry type for naming compatibility.
---------------------------------------------------------------------------*/
typedef LOG_MOBILE_IP_PERFORMANCE_C_type log_mip_perf_type;



/*lint -save -e641 Reason for suppressing error 641*/

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
  FUNCTION MIP_PERF_INIT

  DESCRIPTION
    This function initializes the Mobile IP performance info array.
      
  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void mip_perf_init
(
  void
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Set null time stamp.
  -------------------------------------------------------------------------*/
  qw_set(ts_null, 0, 0);

  /*-------------------------------------------------------------------------
    Initialize start time stamps.
  -------------------------------------------------------------------------*/    
  for (i = 0; i < PERF_MAX_DELAY; i++) 
  {
    qw_equ(mip_perf_info[i].ts_start, ts_null);
  }
} /* mip_perf_init */

/*===========================================================================
FUNCTION       MIP_GET_PERF_INFO_PTR

DESCRIPTION    Access method for mip_perf_info

PARAMETERS
  Delay type
 
RETURN VALUE   
  Pointer to mip_perf_info

DEPENDENCIES   
  None

SIDE EFFECTS   
  None
===========================================================================*/
mip_perf_ts_info_type *mip_get_perf_info_ptr
(
  mip_perf_delay_type delay_type
)
{
  return &mip_perf_info[delay_type];
}

#endif /* FEATURE_DS_MOBILE_IP_PERF */

/*===========================================================================
  FUNCTION MIP_PERF_DELAY

  DESCRIPTION
    This function manages the start timestamp and end time stamps, as well
    as the calculation of the delta times for the delay types in the 
    performance info block.
    
  PARAMETERS
    ts_info   : the time stamp info struct for the delay type
    delay_type: the delay type
    time_stamp_type   : type of timestamp to be set
  
  RETURN VALUE
    None
 
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void mip_perf_delay
(
  mip_perf_ts_info_type *  ts_info,
  mip_perf_delay_type      delay_type,
  mip_perf_ts_type         time_stamp_type
)
{
#ifdef FEATURE_DS_MOBILE_IP_PERF
  log_mip_perf_type *  log_ptr;        /* pointer to log buffer            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Intermediate delta time stamp variable.
  -------------------------------------------------------------------------*/
  qword ts_diff; 

  /*-------------------------------------------------------------------------
    Delta time in seconds and milliseconds for diag output.
  -------------------------------------------------------------------------*/
  uint32 t_diff_s;                    
  uint32 t_diff_ms;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (time_stamp_type)
  {
    case PERF_START_TS:
    {
      /*---------------------------------------------------------------------
        Write system time stamp to start time stamp.
      ---------------------------------------------------------------------*/   
      (void)time_get(ts_info->ts_start);
    
#ifdef T_ARM
      DATA_MIP_MSG1(MSG_LEGACY_LOW, "Delay '%d': SET", delay_type);
#else
      DATA_MIP_MSG1(MSG_LEGACY_LOW, "%s: SET", perf_delay_names[delay_type]); 
#endif

      break;
    }

    case PERF_END_TS:
    {
      /*---------------------------------------------------------------------
        Sanity check: write end time stamp and calculate delay only if the 
        start time stamp has been started.
      ---------------------------------------------------------------------*/
      if (qw_cmp(ts_info->ts_start, ts_null) != 0)
      {     
        /*-------------------------------------------------------------------
          Write system time stamp to end time stamp.
        -------------------------------------------------------------------*/
        (void)time_get(ts_info->ts_end);
    
        /*-------------------------------------------------------------------
          Calculate the difference between the start and end time stamps.
        -------------------------------------------------------------------*/
        qw_sub(ts_diff, 
               ts_info->ts_end, 
               ts_info->ts_start);
    
        /*-------------------------------------------------------------------
          Convert time stamp from System Time to millisecond units.
        -------------------------------------------------------------------*/
        time_to_ms(ts_diff);
        ts_info->delay_in_ms = ts_diff[0];
    
        /*-------------------------------------------------------------------
          Calculate delta time in seconds and milliseconds.
        -------------------------------------------------------------------*/
        t_diff_s = ts_info->delay_in_ms / 1000;
        t_diff_ms = ts_info->delay_in_ms % 1000;
    
#ifdef T_ARM
        DATA_MIP_MSG3(MSG_LEGACY_HIGH, "Delay '%d': %ds %dms", 
                 delay_type,
                 t_diff_s, 
                 t_diff_ms);  
#else
        DATA_MIP_MSG3(MSG_LEGACY_HIGH, "%s: %ds %dms", 
                 perf_delay_names[delay_type],
                 t_diff_s, 
                 t_diff_ms);
#endif
        /*-------------------------------------------------------------------
          If Mobile IP Performance logging is enabled,
          - Allocate log buffer space from diag.
          - Save delay type and value (in milliseconds)
          - Commit MIP perf delay log buf
        -------------------------------------------------------------------*/
        if( log_status(LOG_MOBILE_IP_PERFORMANCE_C) )
        {
          log_ptr = 
            (log_mip_perf_type *) log_alloc( LOG_MOBILE_IP_PERFORMANCE_C, 
                                             sizeof(log_mip_perf_type) );
  
          if (log_ptr != NULL)
          {
            log_ptr->delay_type  = delay_type;
            log_ptr->delay_value = ts_diff[0];
            DATA_MIP_MSG0(MSG_LEGACY_MED, "Logging MIP performance stuff"); 
            log_commit (log_ptr);
          } /* else don't bother - log code is off or no logbuf available */
        }

        /*-------------------------------------------------------------------
          Initialize start time stamp to null.
        -------------------------------------------------------------------*/
        qw_equ(ts_info->ts_start, ts_null);
      }
      else
      {
        DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Delay '%d' not calculated, start TS not set", 
                   delay_type);
      }
      break;
    }

    /*-----------------------------------------------------------------------
      add this to be able to reset a start timestamp back to zero
    -----------------------------------------------------------------------*/
    case PERF_CANCEL_TS:
    {
      /*---------------------------------------------------------------------
        Set the start time back to zero as the timer needs to be canceled
      ---------------------------------------------------------------------*/
      if (qw_cmp(ts_info->ts_start, ts_null) != 0) 
      {
        qw_equ(ts_info->ts_start, ts_null);

#ifdef T_ARM
        DATA_MIP_MSG1(MSG_LEGACY_MED, "Delay '%d': RESET back to 0", delay_type);
#else
        DATA_MIP_MSG1(MSG_LEGACY_MED, "%s: RESET back to 0", perf_delay_names[delay_type]); 
#endif
      
      }/* if() */
      break;
    }

    default:
    {
      DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid M.IP perf delay '%d'", time_stamp_type);
    }
  }

#else
  DATA_MIP_MSG0(MSG_LEGACY_ERROR, "mip_perf_delay() not supported");
  return;
#endif /*FEATURE_DS_MOBILE_IP_PERF*/


} /* mip_perf_delay() */

/*lint -restore Restore lint error 641*/


