/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            S E S SI O N   M A N A G E M E N T   P R O T O C O L
                      K E E P   A L I V E   M O D U L E

GENERAL DESCRIPTION
  Keep Alive functionality defines timers monitoring expiration of the
    session due to lack of communication between the AT and the AN.

EXTERNALIZED FUNCTIONS  (Global)
  hdrsmpka_def_timer()        - Defines the KA timer
  hdrsmpka_powerup_init()     - Powerup Initialization command
  hdrsmpka_initialize_timer() - Checks for KA timer exp
  hdrsmpka_set_timer()        - Sets the timer from the beginning
  hdrsmpka_update_timer()     - Restarts the timer if there is still time
  hdrsmpka_clr_timer()        - Stops the KA timer
  hdrsmpka_save_to_nv()       - Writes KA start and sent to NV

EXTERNALIZED FUNCTIONS  (Regional)
  None

Copyright (c) 2000 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */ 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrsmpka.c_v   1.11   28 May 2002 15:30:46   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrsmpka.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/24/13   cnx     Only write KeepAlive NV on powerdown.
09/24/11   cnx     Retrieved CDMA system time from LTE when HDR in tunnel mode.
07/27/11   cnx     Merged Optimized Handoff changes.
05/31/11   cnx     Added hdrsmpka_get_current_ka_interval().
09/09/09   wsh     CMI: to support changes in clk and ts APIs
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/18/09   wsh     Updated file to include private header hdrmc_v.h
10/01/08   wsh     Fixed critical/high lint warnings
03/27/07   wsh     Added support for new prior session handling
05/09/06   yll     Added support for Minimal UATI Mode.
03/27/06   mpa     Expire KA timer if saved KA start time is in the future.
04/04/05   pba     Removed Lint errors
03/24/05   pba     Lint cleanup
12/20/04   pba     Fixed compiler warnings.
12/17/04   mpa     Fixed detection of KA timer expiry.
10/07/03   mpa     Compute KA interval in units of seconds
09/10/03   mpa     Converted F3 messages to use new MSG2.0 HDRPROT SSID
05/28/02   mpa     ka write interval = 30 minutes
04/15/02   mpa     Added 160ms timer for sync time. Added get_time_source,
                   restore_ka_time. In update_timer, removed time_source
                   param and fixed the trace reporting.
02/28/02   mpa     Added hdrsmpka_save_to_nv() to save on powerdown too.
01/16/02   mpa     Added hdrsmpka_powerup_init, hdrsmpka_initialize_timer.
                   Removed hdrsmpka_set_timer_from_nv. Checks KA exp after
                   acquisition when system time is valid. Various bug fixes.
09/14/01   nmn     fixed a bug: using hdrts_get instead of ts_get (hdrts
                   doesn't work when offline)
07/09/01   nmn     added trace events
07/02/01   nmn     set current interval to default interval in minutes when
                   timers are defined

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "target.h"
#include "dsm.h"
#include "hdrdebug.h"
#include "err.h"
#include "nv.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif


#include "hdrutil.h"
#include "hdrhai.h"
#include "hdrinit.h"

#ifdef FEATURE_CMI
#include "hdrmc_v.h"
#endif

#include "hdrmci.h"
#include "hdrerrno.h"
#include "hdrhmp.h"
#include "hdrtrace.h"
#include "time_svc.h"
#include "qw.h"
#include "nv.h"
#include "hdrscmdb.h"
#include "hdrsmpi.h"
#include "hdrsmps.h"
#include "hdrsmpka.h"
#ifdef FEATURE_HDRMC_DEBUG
#error code not present
#endif

/* <EJECT> */ 
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Default Keep Alive Time (Tsmpclose) = 54 hours (0x0CA8) */
#define HDRSMPKA_DEFAULT_T_SMPCLOSE (54UL * 60L)

/* Max number of keep alive transactions within Tsmpclose (NsmpKeepAlive) */
#define HDRSMPKA_NUM_INTERVALS 3

/* Compute Keep Alive Interval in seconds ( Tsmpclose is stored in minutes,
   but the keep alive interval is computed in seconds ) */
#define HDRSMPKA_GET_INTERVAL( time ) \
   ( ( time * 60UL ) / HDRSMPKA_NUM_INTERVALS )

/*  Default Keep Alive Interval in seconds (Tsmpclose * 3600) / NsmpKeepAlive */
#define HDRSMPKA_DEFAULT_KEEP_ALIVE_INTERVAL_SECS \
   HDRSMPKA_GET_INTERVAL (HDRSMPKA_DEFAULT_T_SMPCLOSE)

/* Conversion between minutes (as the wait time is stored)
   and 80ms units (as the system time will be read).  This is calculated as
   60 * 1000 / 80 */
#define HDRSMPKA_MIN_TO_80MS ( (60 * 1000) / 80 )
#define HDRSMPKA_MIN_TO_80MS_CONV( time ) \
   ( time * HDRSMPKA_MIN_TO_80MS )

#define HDRSMPKA_SEC_TO_80MS_CONV( time ) \
   ( ( time * 1000UL ) / 80UL )

/* Conversion between seconds (as the wait time is stored)
   and miliseconds (as the timer is set) */
#define HDRSMPKA_SEC_TO_MSEC_CONV( time ) \
   ( time * 1000UL )

/* Conversion between 80ms units (as the system time and expiration time
   are calculated) and miliseconds (as the timer is set) */
#define HDRSMPKA_80MS_TO_MSEC_CONV 80UL

#define HDRSMPKA_NV_KEEP_ALIVE_WRITE_INTERVAL_MINS 30UL

#define HDRSMPKA_NV_KEEP_ALIVE_WRITE_INTERVAL \
          (HDRSMPKA_NV_KEEP_ALIVE_WRITE_INTERVAL_MINS * \
           HDRSMPKA_MIN_TO_80MS)

/* conversion from ts_get units to 80ms units */
#define HDRSMPKA_TS_TO_80MS_CONVERSION -22

/* Duration of the sync time valid timer (160ms) */
#define HDRSMPKA_SYNC_TIME_VALID_MSEC 160

/* define smpka timer types, the keep alive timer is not defined here as
   it is handled in hdrsmp */
typedef enum
{
  HDRSMPKA_SYNC_TIME_VALID_TIMER /* 160ms timer during which we use the
                                    sync time as our current time */
} hdrsmpka_timer_enum_type;

/* This structure contains times related to the Keep Alive functionality */
typedef struct
{
  qword expiration_time;     /* system time when Keep Alive Time expires */
  qword keep_alive_start;    /* Time keep alive was started              */
  qword keep_alive_sent;     /* Time last keep alive msg was last sent   */
  boolean ka_start_is_valid; /* Was ka start succesfully read from NV?   */
} hdrsmpka_keep_alive_times_type;

typedef struct
{
  uint32 current_interval; /* in units of seconds */
  hdrsmpka_keep_alive_times_type keep_alive_times;
  rex_timer_type sync_time_valid_timer;
} hdrsmpka_data_type;

LOCAL hdrsmpka_data_type hdrsmpka;

/*---------------------------------------------------------------------------
                           SMP KEEP ALIVE TIMER

                             Keep Alive Timer
---------------------------------------------------------------------------*/

rex_timer_type hdrsmpka_keep_alive_timer; /* keep alive timer */

/*---------------------------------------------------------------------------
                           LOCAL FUNCTIONS
---------------------------------------------------------------------------*/
/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMPKA_GET_TIME_SOURCE

DESCRIPTION
  This function retrieves an accurate current time.

DEPENDENCIES
  None.

PARAMETERS
  current_system_time - address of the qword in which to return the current
                        system time.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrsmpka_get_time_source
(
  qword current_system_time
)
{
  if ( rex_get_timer(&hdrsmpka.sync_time_valid_timer) != 0 )
  {
    /*
    ** call hdrinit_get_sync_time instead of ts_get if we have just acquired.
    ** At this time, the system time is not available through ts_get yet (it
    ** takes about 160ms before it becomes available).  Use the system_time
    ** we stored in hdrinit after we received our sync msg.
    */

    hdrinit_get_sync_time( current_system_time );

    /* we get the sys time in units of 26.666ms. */
    /* Div by 3 to convert to units of 80ms.     */
    (void) qw_div(current_system_time,current_system_time, ( word )3);
  }
  else
  {
#ifdef FEATURE_LTE_TO_HDR_OH
    /* In the tunnel mode, retrieve CDMA system time from LTE */
    if ( hdrsap_tunnel_mode_is_enabled( HDRHAI_SESSION_MANAGEMENT_PROTOCOL ) )
    {
      /* LTE sys time may not be availabe yet when HDR receive SIB8 from LTE. */
      hdrsap_get_80ms_time( current_system_time );
    }
    else
#endif /* FEATURE_LTE_TO_HDR_OH */
    {
      /* ts_get should now safely return a valid system time */
      (void) time_get( current_system_time );
      qw_shift( current_system_time, HDRSMPKA_TS_TO_80MS_CONVERSION );
    }
  }

} /* hdrsmpka_get_time_source */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMPKA_RESTORE_KA_TIME

DESCRIPTION
  This helper function checks for keep alive timer expiration and starts a
  new one if necessary.

DEPENDENCIES
  There is a dependency that this function only be called upon the first
  acquisition of an HDR system after powerup.

PARAMETERS
  expiration_length - length of the time of expiration (Tsmpclose in minutes)

RETURN VALUE
  TRUE  - the keep alive timer is running
  FALSE - the keep alive timer is not running

SIDE EFFECTS
  May set the keep alive timer

===========================================================================*/
LOCAL boolean hdrsmpka_restore_ka_time
(
  uint16 expiration_length
)
{
  qword  current_system_time;   /* valid system time (right after acqu */
  qword  null_time;             /* zero time                           */
  qword  interval_in_80ms;      /* calculated interval in 80ms units   */
  qword  send_message_time;     /* time message will be sent           */
  qword  send_message_interval; /* interval until message will be sent */
  uint32 interval_in_msec; /* 32bits of msecs for interval (for rex timer) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (expiration_length == HDRSMPKA_KEEP_ALIVE_DISABLED)
  {
    HDRTRACE_HDRSMPKA_TIMER_DISABLED();
    return ( FALSE );
  }

  /* get current system time */
  hdrsmpka_get_time_source(current_system_time);

  /* if we couldn't read the ka start time from nv at powerup, reset to the
     current time */
  if (!hdrsmpka.keep_alive_times.ka_start_is_valid)
  {
    qw_equ(hdrsmpka.keep_alive_times.keep_alive_start, current_system_time);
  }

  /* Timing issue - if the saved keep alive start time is in the future,
     recover by expiring the keep alive timer */
  if ( qw_cmp(hdrsmpka.keep_alive_times.keep_alive_start,
              current_system_time) == 1 )
  {
    HDR_MSG_PROT(MSG_LEGACY_ERROR,
                 "Expire KA timer (KA start in the future)");
    hdrsmpka.current_interval = HDRSMPKA_DEFAULT_KEEP_ALIVE_INTERVAL_SECS;
    HDRTRACE_HDRSMPKA_SET_TIMER_FROM_NV(FALSE);
    return ( FALSE );
  }

  /* Compute the ka expiration time */
  qw_equ(hdrsmpka.keep_alive_times.expiration_time,
         hdrsmpka.keep_alive_times.keep_alive_start);
  qw_inc(hdrsmpka.keep_alive_times.expiration_time,
         ( HDRSMPKA_MIN_TO_80MS_CONV( expiration_length )));

  HDR_MSG_PROT_6(MSG_LEGACY_MED,
                 "RestoreKA: KA start(0x%x%x) cur_time(0x%x%x), exp_time(0x%x%x)",
                 qw_hi(hdrsmpka.keep_alive_times.keep_alive_start),
                 qw_lo(hdrsmpka.keep_alive_times.keep_alive_start),
                 qw_hi(current_system_time),
                 qw_lo(current_system_time),
                 qw_hi(hdrsmpka.keep_alive_times.expiration_time),
                 qw_lo(hdrsmpka.keep_alive_times.expiration_time)
                 );

  /* if the expiration time is not after the current system time, the keep
     alive has expired */
  if (!(qw_cmp(hdrsmpka.keep_alive_times.expiration_time,
               current_system_time) == 1))
  {
    hdrsmpka.current_interval = HDRSMPKA_DEFAULT_KEEP_ALIVE_INTERVAL_SECS;
    HDRTRACE_HDRSMPKA_SET_TIMER_FROM_NV(FALSE);
    return ( FALSE );
  }

  /*-- session is still good.  start timer --*/

  /* first, get a single interval in 80ms units */
  hdrsmpka.current_interval  = HDRSMPKA_GET_INTERVAL (expiration_length);
  qw_set(interval_in_80ms, 0x00000000,
         HDRSMPKA_SEC_TO_80MS_CONV( hdrsmpka.current_interval ));

  qw_set(null_time,0x00000000,0x00000000);

  /* if keep alive was sent */
  if (qw_cmp(hdrsmpka.keep_alive_times.keep_alive_sent, null_time) == 1)
  {
    HDR_MSG_PROT(MSG_LEGACY_LOW,"RestoreKA: KA was sent");
    qw_add(send_message_time, hdrsmpka.keep_alive_times.keep_alive_sent,
                              interval_in_80ms);
  }
  else /* keep alive was not sent */
  {
    HDR_MSG_PROT(MSG_LEGACY_LOW,"RestoreKA: KA was not sent");
    qw_add(send_message_time, hdrsmpka.keep_alive_times.keep_alive_start,
                              interval_in_80ms);
  }

  /* if time when message should have been sent has already passed */
  if (qw_cmp(current_system_time, send_message_time) == 1)
  {
    /* Set keep alive timer to expire immediately.  This case is then
       handled the same as if the timer expired naturally */
//    interval_in_msec = 0;
      HDR_MSG_PROT_4(MSG_LEGACY_MED,
                   "send msg time (0x%x%x) has passed (ka sent 0x%x%x).  Expire KA.",
                   qw_hi(send_message_time),
                   qw_lo(send_message_time),
                   qw_hi(hdrsmpka.keep_alive_times.keep_alive_sent),
                   qw_lo(hdrsmpka.keep_alive_times.keep_alive_sent)
                   );
      return ( FALSE );
  }
  else
  {
    /* time when message should have been sent has not passed */

    /* make send_message_interval be time from now to time to send message */
    qw_sub(send_message_interval, send_message_time, current_system_time);
    qw_mul(send_message_interval, send_message_interval,
                                HDRSMPKA_80MS_TO_MSEC_CONV);

    /* if interval in msec is longer than the rex timer can handle, set the
       timer for max.  Rex timers are only 32 bits long, so if the interval
       would be more than 32 bits worth of milliseconds, use the maximum
       Rex timer.  */
    if (qw_hi(send_message_interval) > 0)
    {
      HDR_MSG_PROT (MSG_LEGACY_ERROR, "Keep Alive Timer overflow");
      interval_in_msec = 0xFFFFFFFFUL;
    }
    else
    {
      interval_in_msec = qw_lo(send_message_interval);
    }
  }

  HDR_MSG_PROT_2(MSG_LEGACY_MED,
                 "RestoreKA: SetTimer for %dms (curr_interval %ds)",
                 interval_in_msec,
                 hdrsmpka.current_interval);

  /* reset the KA timer */
  (void)rex_set_timer(&hdrsmpka_keep_alive_timer, interval_in_msec);
  HDRTRACE_HDRSMPKA_SET_TIMER_FROM_NV(TRUE);

  return ( TRUE );

} /* hdrsmpka_restore_ka_time */

/* <EJECT> */ 

/*---------------------------------------------------------------------------
                           EXTERNALIZED FUNCTIONS
---------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION HDRSMPKA_SAVE_TO_NV

DESCRIPTION
  This function writes KA start and sent times to NV.  KA parameters are
  saved to NV every 30 minutes and on powerdown.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrsmpka_save_to_nv( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Write start and sent KA times to NV */
    hdrutil_write_nv_item(NV_HDRSMPKA_START_TIME_I,
         (nv_item_type*)hdrsmpka.keep_alive_times.keep_alive_start);

    hdrutil_write_nv_item(NV_HDRSMPKA_SENT_TIME_I,
         (nv_item_type*)hdrsmpka.keep_alive_times.keep_alive_sent);

    HDR_MSG_PROT_4(MSG_LEGACY_MED,
                   "Save to NV: KA start (0x%x%x) KA sent (0x%x%x)",
                   qw_hi(hdrsmpka.keep_alive_times.keep_alive_start),
                   qw_lo(hdrsmpka.keep_alive_times.keep_alive_start),
                   qw_hi(hdrsmpka.keep_alive_times.keep_alive_sent),
                   qw_lo(hdrsmpka.keep_alive_times.keep_alive_sent)
                   );

} /* hdrsmpka_save_to_nv */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMPKA_SET_TIMER

DESCRIPTION
  This function sets the timer from the beginning

DEPENDENCIES
  None.

PARAMETERS
  expiration_length - length of time of the expiration (Tsmpclose in minutes)

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void hdrsmpka_set_timer
(
  uint16 expiration_length
)
{
  qword current_system_time; /* current system time */
  qword interval_in_80ms;    /* request interval converted to 80ms units */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If AT doesn't maintain EVDO session due to Minimal UATI mode 
   * (MUM) settings, and the timer is already running, we don't set 
   * the KA timer. However if this is the first time to set the 
   * KA timer, for example, after the data call ends and the 
   * connection closes, we still want to set the timer so when
   * it's expired, AT can close the session.
   */
  if ( !hdrsmp_is_to_maintain_evdo_session() &&
       (rex_get_timer(&hdrsmpka_keep_alive_timer) !=0 )
     )
  {
    HDR_MSG_PROT(MSG_LEGACY_HIGH, " MUM: Don't set KA timer" );
    return;
  }

  HDRTRACE_HDRSMPKA_SET_TIMER();

  /* Compute the keep alive interval in seconds */
  hdrsmpka.current_interval = HDRSMPKA_GET_INTERVAL(expiration_length);

  /* clear timer if it's running */
  (void)rex_clr_timer(&hdrsmpka_keep_alive_timer);

  if (hdrsmpka.current_interval == HDRSMPKA_KEEP_ALIVE_DISABLED)
  {
    HDRTRACE_HDRSMPKA_TIMER_DISABLED();
    return;
  }

  /* Get the current system time in units of 80ms */
  hdrsmpka_get_time_source(current_system_time);

  /* Keep track of current keep_alive_start and keep_alive_sent times */
  qw_equ(hdrsmpka.keep_alive_times.keep_alive_start, current_system_time);
  qw_set(hdrsmpka.keep_alive_times.keep_alive_sent, 0x00000000, 0x00000000);

  HDR_MSG_PROT_4(MSG_LEGACY_LOW,
                 "SetTimer: cur_time=ka_start(0x%x%x), exp_time(0x%x%x), ka_sent 0",
                 qw_hi(current_system_time),
                 qw_lo(current_system_time),
                 qw_hi(hdrsmpka.keep_alive_times.expiration_time),
                 qw_lo(hdrsmpka.keep_alive_times.expiration_time)
                 );

  /* Compute the new expiration time */
  qw_set(interval_in_80ms, 0x00000000,
         HDRSMPKA_MIN_TO_80MS_CONV( (uint32) expiration_length ));
  qw_add(hdrsmpka.keep_alive_times.expiration_time, interval_in_80ms,
         current_system_time);

  HDR_MSG_PROT_3(MSG_LEGACY_MED,
                 "SetTimer for %dms (exp time: 0x%x%xms)",
                 HDRSMPKA_SEC_TO_MSEC_CONV(hdrsmpka.current_interval),
                 qw_hi(hdrsmpka.keep_alive_times.expiration_time),
                 qw_lo(hdrsmpka.keep_alive_times.expiration_time)
                );

  /* reset timer */
  (void)rex_set_timer(&hdrsmpka_keep_alive_timer,
        HDRSMPKA_SEC_TO_MSEC_CONV(hdrsmpka.current_interval) );

} /* hdrsmpka_set_timer */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMPKA_POWERUP_INIT

DESCRIPTION
  This function commands hdrsmpka to initialize.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrsmpka_powerup_init ( void )
{


  hdrsmpka.keep_alive_times.ka_start_is_valid = ( TRUE );

  /* Read keep_alive_start and keep_alive_sent from NV */
  if (hdrutil_read_nv_item(NV_HDRSMPKA_START_TIME_I,
     (nv_item_type*)hdrsmpka.keep_alive_times.keep_alive_start) != NV_DONE_S)
  {
    /* indicate that the start time is not valid.  Initialize to current
       time after first acquisition (when system time becomes valid) */
    qw_set(hdrsmpka.keep_alive_times.keep_alive_start, 0x00000000, 0x00000000);
    hdrsmpka.keep_alive_times.ka_start_is_valid = ( FALSE );
  }

  if (hdrutil_read_nv_item(NV_HDRSMPKA_SENT_TIME_I,
     (nv_item_type*)hdrsmpka.keep_alive_times.keep_alive_sent) != NV_DONE_S)
  {
    qw_set(hdrsmpka.keep_alive_times.keep_alive_sent,0x00000000, 0x000000000);
  }

  HDR_MSG_PROT_4(MSG_LEGACY_MED,
                 "Powerup: KA start (0x%x%x) KA sent (0x%x%x)",
                 qw_hi(hdrsmpka.keep_alive_times.keep_alive_start),
                 qw_lo(hdrsmpka.keep_alive_times.keep_alive_start),
                 qw_hi(hdrsmpka.keep_alive_times.keep_alive_sent),
                 qw_lo(hdrsmpka.keep_alive_times.keep_alive_sent)
                 );

} /* hdrsmpka_powerup_init */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMPKA_INITIALIZE_TIMER

DESCRIPTION
  This function checks for keep alive timer expiration and starts the
  sync time valid timer

DEPENDENCIES
  There is a dependency that this function only be called upon the first
  acquisition of an HDR system after powerup.

PARAMETERS
  session_is_open   - whether we have a current session open
  expiration_length - length of the time of expiration (Tsmpclose in minutes)

RETURN VALUE
  TRUE  - the keep alive timer is running
  FALSE - the keep alive timer is not running

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrsmpka_initialize_timer
(
  boolean session_is_open,
  uint16  expiration_length
)
{
  boolean ka_running = ( TRUE );

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
     After we get a network acquired indication, we need to wait for about
     160ms to ensure that the system time is valid. Hence, we start the sync
     time valid timer for 160ms. While the timer is running, our current time
     will be the time we stored from the sync message.
  */
#ifdef FEATURE_LTE_TO_HDR_OH
  if ( !hdrsap_tunnel_mode_is_enabled( HDRHAI_SESSION_MANAGEMENT_PROTOCOL ) )
#endif /* FEATURE_LTE_TO_HDR_OH */
  {
    (void)rex_set_timer( &hdrsmpka.sync_time_valid_timer,
                         (uint32)HDRSMPKA_SYNC_TIME_VALID_MSEC );
  }

  if ( session_is_open )
  {
    ka_running = hdrsmpka_restore_ka_time( expiration_length );
  }

  return ( ka_running );

} /* hdrsmpka_initialize_timer() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMPKA_UPDATE_TIMER

DESCRIPTION
  This function restarts the timer if there is still time

DEPENDENCIES
  None

PARAMETERS
  update_timer_reason - reason why the timer is being updated

RETURN VALUE
  TRUE   - the keep alive timer is still running
  FALSE  - the keep alive timer is not running

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrsmpka_update_timer
(
  hdrsmpka_update_timer_enum_type update_timer_reason
)
{
  qword current_system_time; /* current system time */
  qword temp_dif; /* temp var used in calculations for difference */
  qword current_interval_in_80ms; /* current interval in 80ms units */
  uint32 interval_in_msec; /* 32bits of msecs for interval (for rex timer) */
  uint8 result; /* Result of comparison between current and exp time */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SYS_ARG_NOT_USED(update_timer_reason); /* remove this line when 
                                            update_timer_reason is used */

  if ( hdrsmpka.current_interval == HDRSMPKA_KEEP_ALIVE_DISABLED )
  {
    HDRTRACE_HDRSMPKA_TIMER_DISABLED();
    return ( FALSE );
  }

  /* get the current system time */
  hdrsmpka_get_time_source(current_system_time);


  HDR_MSG_PROT_4(MSG_LEGACY_LOW,
                 "UpdateTimer: cur_time(0x%x%x), exp_time(0x%x%x)",
                 qw_hi(current_system_time),
                 qw_lo(current_system_time),
                 qw_hi(hdrsmpka.keep_alive_times.expiration_time),
                 qw_lo(hdrsmpka.keep_alive_times.expiration_time)
                 );

  /* Compare the current system time against the expected expiration time */
  result = qw_cmp(current_system_time,
                  hdrsmpka.keep_alive_times.expiration_time);

  /* if expiration time has passed, return */
  if ( (result == 1) || (result == 0) )
  {
    /* the trace event indicates whether the expiration time has
    passed, not the status of the ka timer (running or not) */
    HDRTRACE_HDRSMPKA_UPDATE_TIMER( TRUE );
    return ( FALSE );
  }

  /* Save ka sent time */
  qw_equ( hdrsmpka.keep_alive_times.keep_alive_sent, current_system_time );

  /* if the session has not already expired, set the timer for either the
     time left before the session expires or the time before we need to
     send another keep alive message, whichever is shorter */

  /* calculate time to expiration time */
  qw_sub( temp_dif,hdrsmpka.keep_alive_times.expiration_time,
          current_system_time);

  qw_set(current_interval_in_80ms, 0x00000000,
         HDRSMPKA_SEC_TO_80MS_CONV( hdrsmpka.current_interval ));

  /* if time to expiration time is longer than current interval */
  if (qw_cmp(temp_dif,current_interval_in_80ms) == 1)
  {
    /* use interval instead of time to expiration time */
    qw_equ(temp_dif, current_interval_in_80ms);
  }

  /* turn time into msec */
  qw_mul(temp_dif, temp_dif, HDRSMPKA_80MS_TO_MSEC_CONV);

  /* if interval in msec is longer than the rex timer can handle, set the
     timer for max.  Rex timers are only 32 bits long, so if the interval
     would be more than 32 bits worth of miliseconds, use the maximum
     Rex timer.  */
  if (qw_hi(temp_dif) > 0)
  {
    HDR_MSG_PROT (MSG_LEGACY_ERROR, "Keep Alive Timer overflow");
    interval_in_msec = 0xFFFFFFFFUL;
  }
  else
  {
    interval_in_msec = qw_lo(temp_dif);
  }

  HDR_MSG_PROT_3(MSG_LEGACY_MED,
                 "SetTimer for %dms (temp_dif: 0x%xms, curr_interval %ds)",
                 interval_in_msec,
                 qw_lo(temp_dif),
                 hdrsmpka.current_interval);

  /* Set the KA timer */
  (void) rex_set_timer( &hdrsmpka_keep_alive_timer, interval_in_msec );
  HDRTRACE_HDRSMPKA_UPDATE_TIMER( FALSE );

  return ( TRUE );
} /* hdrsmpka_update_timer */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMPKA_CLR_TIMER

DESCRIPTION
  This function stops the keep alive timer

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsmpka_clr_timer ( void )
{
  HDRTRACE_HDRSMPKA_CLR_TIMER();
  (void)rex_clr_timer(&hdrsmpka_keep_alive_timer);
} /* hdrsmpka_clr_timer */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMPKA_DEF_TIMER

DESCRIPTION
  This function defines the timers

DEPENDENCIES
  None

PARAMETERS
  tcb_ptr - pointer to the tcb of the task where the keep alive functionality
            is being run

  sig - sig to set upon timer expiration during definition

RETURN VALUE
  None

SIDE EFFECTS
  Defines a rex timer

===========================================================================*/

void hdrsmpka_def_timer
(
  rex_tcb_type * tcb_ptr,
  rex_sigs_type sig
)
{
  HDRTRACE_HDRSMPKA_DEF_TIMER();

  /* Define the sync time is valid timer */
  rex_def_timer_ex( &hdrsmpka.sync_time_valid_timer,
                    NULL, /* no callback necessary */
                    HDRSMPKA_SYNC_TIME_VALID_TIMER );

  /* Define the keep alive timer */
  rex_def_timer(&hdrsmpka_keep_alive_timer,
                tcb_ptr, sig );

  /* Initialize the current interval to default duration */
  hdrsmpka.current_interval = HDRSMPKA_DEFAULT_KEEP_ALIVE_INTERVAL_SECS;
} /* hdrsmpka_def_timer */

/*===========================================================================

FUNCTION HDRSMPKA_GET_KA_EXPIRE_TIME

DESCRIPTION
  This function returns the expiration time of current session

DEPENDENCIES
  There is a should only be called when current session is valid

PARAMETERS
  expiration_time - returns the expiration time of current session

RETURN VALUE
  None

SIDE EFFECTS
  May set the keep alive timer

===========================================================================*/
void hdrsmpka_get_ka_expire_time
(
  qword  expiration_time   /* valid system time (right after acq.) */
)
{
  /* Compute the ka expiration time */
  qw_equ(expiration_time,
         hdrsmpka.keep_alive_times.expiration_time);
} /* hdrsmpka_restore_ka_time */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMPKA_SET_EXPIRE_TIME

DESCRIPTION
  This function sets the keep alive timer to expire at the given time

DEPENDENCIES
  This function must be called after AT has acquired system and has valid
  system time.

PARAMETERS
  time_to_expire - the time keep alive timer will expire.

RETURN VALUE
  True - timer is successfully set
  False - time_to_expire has already passed, timer is not modified.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrsmpka_set_expire_time
(
  qword time_to_expire
)
{
  qword current_system_time;
  uint32 time_interval;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  hdrsmpka_get_time_source(current_system_time);
  if (qw_cmp(current_system_time, time_to_expire) == 1)
  {
    /* time_to_expire has already passed, return false */
    return FALSE;
  }
  else
  {
    /* calculate the time difference with regard to current time, and 
       convert to 80ms slots. current_system_time also acts as a temp.
       storage for time interval */
    qw_sub(current_system_time, time_to_expire, current_system_time);
    (void) qw_div(current_system_time, current_system_time, (uint16)HDRSMPKA_MIN_TO_80MS);
    time_interval = qw_lo(current_system_time);
    if (time_interval > 0xFFFF)
    {
      /* if the time interval in minutes exceeds uint16, which is the
         capacity of the hdrsmpka_set_timer function, consider it a failure. */
      return FALSE;
    }

    hdrsmpka_clr_timer();
    hdrsmpka_set_timer((uint16)time_interval);
    return TRUE;
  }
} /* hdrsmpka_set_expire_time */

/*===========================================================================

FUNCTION HDRSMPKA_GET_CURRENT_KA_INTERVAL

DESCRIPTION
  This function returns the interval to the time that Keep Alive
  timer expires.

DEPENDENCIES
  None
 
PARAMETERS
  None

RETURN VALUE
  The interval to the time that Keep Alive timer expires.

SIDE EFFECTS
  None

===========================================================================*/
uint32 hdrsmpka_get_current_ka_interval( void )
{
  return hdrsmpka.current_interval;
} /* hdrsmpka_get_current_ka_interval */
