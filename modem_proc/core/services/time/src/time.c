/*=============================================================================

                   T I M E   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements time-keeping functions using various sources.


EXTERNALIZED FUNCTIONS
  time_init( )
    Initializes the time services subsystem

  time_to_ms( )
    Converts a time stamp into milliseconds.

  time_get( )
    Retrieves time according to CDMA, HDR, GSM, or slow-clock Time-of-Day

  time_get_ms( )
    Retrieves system time in milliseconds

  time_get_sec( )
    Retrieves system time, in seconds

  time_get_uptime_ms( )
    Retrieves the up-time, in milliseconds

  time_get_uptime_secs( )
    Retrieves the up-time, in seconds

  time_sync_time_of_day( )
    Syncs the current time, from CDMA, HDR, or GSM, to the slow clock based
    time-of-day.

  time_set_from_pmic( )
    Set the time-of-day from the PMIC's RTC (if present)

  time_to_ms_native()
    Convert the current time to milliseconds in native uint64 format.

  time_get_ms_native()
    Get the current time in milliseconds in native uint64 format.    

REGIONAL FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  time_init( ) must be called to initalize the time subsystem state variables
  and install necessary ISRs.


Copyright (c) 2003 - 2015 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/time/src/time.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/15   abh     Changes for correctly handling ATS_PRIVATE bases.
04/29/14   abh     Added optimized call to get time using time_get_optimized_till_msecs.
07/12/10   din     Introduced features to selectively compile legacy time_tod 
                   and time_secure
06/23/10   din     Attached proxy bases for few bases.
04/27/10   din     Fixed compiler warnings.
02/22/10   din     Removed function time_get_local_secs().
12/23/09   din     Changed time_to_ms[sec] to use time_conv_to_ms[sec].
12/23/09   din     Removed time_compute_power_on call and updated 
                   time_get_uptime_ms to use time_sclk_get.
12/16/09   din     Removed explicit setting of secure time in 
                   time_sync_time_of_day().
12/16/09   din     In time_get_uptime_ms(), set Uptime as zero and return if 
                   time is not initialized. 
12/08/09   din     Removed old dem portion. 
12/5/09    din     Added initilization for Generic Framework.
06/17/09   din     Added time_restart
06/12/09   din     Added time_get_apps_user_time
05/22/09   din     Replaced timetick_mpm_sync() with time_mpm_sync().
04/30/09   sep     Do not sync clocks with the modem on time_init for WinMobile
12/22/08   din     Consolidation of time sources. Replace time_remote_init() 
                   with DEM time synchronization within FEATURE_TIME_DEM_SYNCHRONIZE.
                   Added FEATURE_TIME_UMTS_TARGETS. Winmobile portion of the 
                   code is transferred to time_wm.c
12/15/08   tev     Added explicit include of timetick_v.h to use regional
                   functions defined therein.
11/03/08   sep     Add code to update the time zone offset on WinMobile
09/23/08   sep     Keep user time separate from Nitz time on WM6 for 7500 
                   worldmode phones, also fix uninitialized dwSize in registry reads
08/19/08   jhs     Remove power_on_sec and calculate uptime in seconds from
                   uptime in milliseconds
08/19/08   jhs     Protect against preemption in uptime set calculations
08/13/08   sep     Add write to registry key for user_to_modem_offset for WinMobile
                   to avoid a deadlock when doing it in the IOCTL handler.
07/25/08   jhs     Move timer_init to after time_remote_slow_clock_sync
                   FEATURE_SEC_TIME_NOT_FROM_NITZ now causes secure time to
                   not be set from Network Time.
04/25/08   jhs     Support for Secure time from network time
04/22/08   jhs     Fix WM DEBUG build compile errors.
12/17/07   jhs     Call sclk_init() at DLL_PROCESS_ATTACH to initialize
                   sclk variables used by sclk_to_timestamp().
10/31/07   jhs     Fixed dynamic load DLL to use the correct name.  Was
                   swapped in previous fix.
09/28/07   jhs     Changed from ONCRPC_LIBRARY to REMOTE_APIS_LIBRARY.
                   Changed from oncrpc.dll to remote_apis_clnt.dll
09/17/07   ih      Use dynamic loading for RIL calls
08/31/07   jhs     Support for AMSS RESTART
                   Fix dynamic loading for RPC.
08/28/07   trc     Protect against preemption in uptime calculations
08/01/07   jhs     Use dynamic loading for RPC call on WM.
07/31/07   jhs     Support for Linux builds
07/11/07   ebb     Add 64-bit pseudo-timetick service initialization
07/09/07   jhs     Improved support for TMS driver
                   Added request for TOD update when TMS driver is initialized
                   Improved support for Smartfon and PPC builds for Time Applet/
                   Time Reg Settings.
05/22/07   jhs     Call free_virtual_address when process detaches.
12/19/06   jhs     Fix for not passing TOD bases to Apps after first time.
10/13/06   gb/ip   Changes for GPLT builds. Fixed warning
09/27/06   gb      Changes to get user set time working
09/21/06   trc     Changed time_sync_time_of_day() failure to a non-fatal ERR()
08/19/06   gb      Changed FEATURE_POWER_COLLAPSE to FEATURE_TIME_POWER_COLLAPSE
05/09/06   hn      Protected references to DB with FEATURE_NO_DB
02/22/06   ajn     Synchronize MPM & Modem SCLK counters on startup.
11/18/05   mod/ajn Added MediaFLO time support.
09/26/05   jhs     Added call to time_remote_init.
09/20/05   ajn/cr  Added GPS based time.
08/11/05   jhs     Determine delta between modem & app sclk counters at init.
06/24/05   ajn     Include partial 7500 support
05/10/05   ajn     Added time_set_from_pmic( ) function, to correct init order.
11/24/04   grl     Added support for getting time in ms in native uint64 types.
02/09/04   ajn     Renamed time.h to time_svc.h to avoid OS conflict
08/08/03   ajn     Changed timestamp type from qword to time_type (a qword)
08/01/03   ajn     Moved _get_sec, get_ms from time_tod.  Added _get_local_sec
07/24/03   ajn     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include <stringl/stringl.h>
#include <string.h>
#include <stdio.h>
#include "time_svc.h"
#include "time_conv.h"
#include "timetick_sclk64.h"
#include "time_dal_v.h"
#include "ats_v.h"

#ifdef FEATURE_TIME_QDSP6
#include "qurt.h"
#endif /*FEATURE_TIME_QDSP6*/

#include "time_genoff_v.h"
#ifdef FEATURE_TIME_SINGLE_MULTI_SERVER_PROC
#include "time_diag.h"
#endif /* FEATURE_TIME_SINGLE_MULTI_SERVER_PROC */

#include "err.h"

#if defined(FEATURE_MULTIPROCESSOR) && !defined(FEATURE_TIME_NO_RPC)
  #include "time_remote.h"
  #include "dem.h"
#endif /* FEATURE_MULTIPROCESSOR && !FEATURE_TIME_NO_RPC */

#include "time_fn_map.h"

/* Include files for RCINIT shutdown callback notification */
#include "rcinit.h"

/* Ulog header files */
#include "ULog.h"
#include "ULogFront.h"

/*-----------------------------------------------------------------------------
  Time source includes
-----------------------------------------------------------------------------*/

/* Time-of-day */
#include "time_jul.h"
#include "ats_rtc.h"
#include "ats_bases.h"
#include "timetick_sclk64.h"

#ifdef FEATURE_TIME_IPC_TASK
#include "time_ipc.h"
#endif

extern void timetick_sclk64_pimutex_init(void);

#ifndef FEATURE_TIME_NO_RPC
extern void time_remote_pimutex_init(void); 
#endif

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/


/*-----------------------------------------------------------------------------

                      SYSTEM TIMESTAMP FORMAT


   |<------------ 48 bits --------------->|<----- 16 bits ------->|
   +--------------------------------------+-----------------------+
   |      1.25 ms counter                 |   1/32 chip counter   |
   +--------------------------------------+-----------------------+
         (11K years of dynamic range)          (1.25 ms dynamic
                                                range. Rolls over
                                                at count 49152)

-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
  Useful constants
-----------------------------------------------------------------------------*/

/* To convert 1.25ms units to TS units, shift in cx32 bits of timestamp */
#define TIME_1p25MS_TO_TS_SHIFT   16

/* Seconds per TimeZone (30 min/zone x 60sec/min) */
#define TIME_SEC_PER_TIME_ZONE    (30 * 60)


#ifdef FEATURE_OS_MEMORY_PROTECTION
  /*--------------------------------------------------------------------------
    We need to allocate virtual memory and initialize this pointer before
    accessing the hardware
  -------------------------------------------------------------------------*/
  byte*                           A11S_CSR_BASE = NULL;
#endif /* FEATURE_OS_MEMORY_PROTECTION */

/*-----------------------------------------------------------------------------
  Time static information
-----------------------------------------------------------------------------*/
#if (((defined(IMAGE_MODEM_PROC)&& !defined T_MDM9X00) || defined FEATURE_TIME_QDSP6) && defined(FEATURE_TOD_MULTIPROC_SYNC))
/* Apps to modem offset For WM builds */
static int64 time_apps_to_modem_offset = 0;
#endif /*(((defined(IMAGE_MODEM_PROC)&& !defined T_MDM9X00) || defined FEATURE_TIME_QDSP6) && defined(FEATURE_TOD_MULTIPROC_SYNC))*/

/*This data structure will store the configuration data*/
time_defines_type time_defines;
time_fn_ptr_type  time_fn_ptr;

/*sequence number for remote modem updates*/
static uint32 time_remote_udpate_seq_num = 0;

/* Stores the base that needs to be used by time_get() function */
static time_bases_type time_get_base = 2;

/* Timer to print out timetick, time related details periodically */
static timer_type time_timetick_log_timer;
#define TIME_TIMETICK_PRINT_FREQ       5 /* Print for every 5secs */
ULogHandle time_ulog_handle = 0;


/*This function will be defined in the OS specific file */
extern void time_init_fn_ptr(time_fn_ptr_type*);
extern void timetick_sclk64_qmutex_init(void);
extern void time_rex_q6_qmutex_init(void);
extern void time_remote_qmutex_init(void);

/*Populates the time_defines_type data struncture supplied through
  a pointer*/
void time_get_time_defines(time_defines_type* time_defines_copy);

/**define a timer for registration of this modem with the othe 
 modem proc in case of a fusion product*/
#ifdef FEATURE_EXPORT_TIME_REMOTE_MM_FUSION_CLNT

  /** Timer to fire off a registration event to the other modem */
  timer_type                      mm_registration_timer;

  /** A timer group, to ensure registration timer is never
   disabled. */
  timer_group_type                mm_registration_timer_group;

  /*As of now, the timer has been set to 2 secs or 0x10000 ticks*/
  #define MM_REG_TIMER            0x10000

#endif /*FEATURE_EXPORT_TIME_REMOTE_MM_FUSION_CLNT*/


/*Mapping for intlock*/
#ifndef FEATURE_TIME_QDSP6
 
#define ATS_INTLOCK() INTLOCK()
#define ATS_INTFREE() INTFREE()

#define ATS_INTLOCK_SAV(x) INTLOCK_SAV(x)
#define ATS_INTFREE_SAV(x) INTFREE_SAV(x)      

#else /*else for Q6*/

/* Pointer to the mutex object for protecting critical-section of time
   code*/
qurt_mutex_t time_q6_mutex;

/*function protect the critical section of time code*/
static void time_pimutex_init(void);


/*MACRO for locking/unlocking the pimutex*/
#define ATS_INTLOCK()  \
  qurt_pimutex_lock(&time_q6_mutex); \
  
#define ATS_INTFREE()  \
  qurt_pimutex_unlock(&time_q6_mutex); \

#define ATS_INTLOCK_SAV(X) \
  qurt_pimutex_lock(&time_q6_mutex); \
  
#define ATS_INTFREE_SAV(X)   \
  qurt_pimutex_unlock(&time_q6_mutex); \

#endif /*FEATURE_TIME_QDSP6*/

#ifdef FEATURE_TIME_QDSP6
/*=============================================================================

FUNCTION TIME_PIMUTEX_INIT

DESCRIPTION
  This function initializes the pimutex that will be used for protecting
  the crit-section portion of time code.

DEPENDENCIES
  This function is applicable only for qurt.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
static void time_pimutex_init(void)
  {
  qurt_pimutex_init ( &time_q6_mutex );
}

#endif

/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================

FUNCTION TIME_REMOTE_UPDATE_INC_SEQ_NUM

DESCRIPTION
  Increments the sequence number by one. The sequence
  number is incremented everytime a modem receives an 
  update

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_remote_update_inc_seq_num
(
  void
)
{
  time_remote_udpate_seq_num++;
}

/*=============================================================================

FUNCTION TIME_REMOTE_UPDATE_GET_SEQ_NUM

DESCRIPTION
  API to obtain the sequence number 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
uint32 time_remote_update_get_seq_num
(
  void
)
{
  return time_remote_udpate_seq_num;
}

extern void ats_cl_init
(
  void
);

#ifdef FEATURE_EXPORT_TIME_REMOTE_ATOM_FUSION
extern void ats_mm_cl_init
(
  void
);
#endif

/*=============================================================================

FUNCTION TIME_INIT

DESCRIPTION
  Initialize Timekeeping Subsystem

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Installs timekeeping ISR, timers, etc.

=============================================================================*/

void time_init( void )
{
  /*Init the time structure which is based on the values in the xml file*/
  time_dal_config_init(&time_defines);

  /*populate the function pointers with appropriate functions*/
  time_init_fn_ptr(&time_fn_ptr);
    
  /*for modem images, we need to initialize the following mutex*/
  #ifdef FEATURE_TIME_QDSP6
  time_pimutex_init(); 
  timetick_sclk64_pimutex_init();
  #ifndef FEATURE_TIME_NO_RPC
  time_remote_pimutex_init(); 
  #endif
  #endif
        
  /*Call the initialization functions here. This function will
   be defined in the OS specific files.*/
  if(time_fn_ptr.time_call_init_functions_fp)
  {
    time_fn_ptr.time_call_init_functions_fp();
  }
  /* Initialize time-of-day subsystem, and roll-over timer */
  timetick_sclk64_init();
  
#ifdef FEATURE_TIME_MULTI_SERVER_PROC
  /* Initializes Callback registraion Mechanism for remote callbacks */
  ats_cl_init();
  #ifdef FEATURE_EXPORT_TIME_REMOTE_ATOM_FUSION
  ats_mm_cl_init();
  #endif
#endif /* FEATURE_TIME_MULTI_SERVER_PROC */

#ifdef T_MDM9X00
#ifdef FEATURE_TIME_QDSP6
  time_services_test_init();
#endif
#else
#ifdef IMAGE_MODEM_PROC
  time_services_test_init();
#endif  
#endif /* FEATURE_TIME_SINGLE_MULTI_SERVER_PROC */

} /* time_init */


#if (defined IMAGE_APPS_PROC || (defined T_MDM9X00 && !defined FEATURE_TIME_QDSP6))
/*=============================================================================

FUNCTION TIME_REMOTE_GENOFF_CB                                          REGIONAL

DESCRIPTION
  Callback called to remotely update offset

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None

=============================================================================*/

static void time_remote_genoff_cb
(
  ats_cfg_event_e_type         event,
  ats_cfg_event_info_s_type    *info_ptr
)
{


    
  /* Pointer to Time_genoff instance in consideration */
  time_genoff_ptr                 ptime_genoff = NULL;
     
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (event != ATS_CFG_EVENT_GENOFF_CHANGE )
   return ;

  /* Get the pointer to genoff */
  ptime_genoff = time_genoff_get_pointer(  info_ptr->ats_genoff_update.base_type );


  time_genoff_set_generic_offset( ptime_genoff, 
                                  info_ptr->ats_genoff_update.offset );



} /* time_remote_genoff_cb */

#endif /* IMAGE_APPS_PROC */

#ifdef FEATURE_EXPORT_TIME_REMOTE_MM_FUSION_CLNT
/*=============================================================================

FUNCTION TIME_REMOTE_GENOFF_MM_CB                                          
REGIONAL

DESCRIPTION
  Callback called to remotely update offset

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 None

=============================================================================*/

static void time_remote_genoff_mm_cb
(
  ats_cfg_event_e_type         event,
  ats_cfg_event_info_s_type    *info_ptr
)
{
  /* Pointer to Time_genoff instance in consideration */
  time_genoff_ptr                 ptime_genoff = NULL;
  time_genoff_args_struct_type    temp_genoff_args;
  time_type                       ts_value;
  uint64                          temp_value = 0;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (event != ATS_CFG_EVENT_GENOFF_CHANGE )
   return ;

  /*Only call time_tod_set when the base is TOD*/
  if(ATS_TOD == info_ptr->ats_genoff_update.base_type) 
  {
    /*increment the sequence count here since we have received an
     update from the other modem. The sequence on the other modem
     should have been already incremented*/
    time_remote_update_inc_seq_num();
    ATS_MSG_1("time_remote_update_seq_num: Remote modem update seq num =  %d",
            time_remote_update_get_seq_num());

    /*copy over the contents of the parameters to ts_value variable*/
    memscpy(&ts_value[0],sizeof(time_type),&info_ptr->ats_genoff_update.offset,sizeof(time_type));
    #if (defined( FEATURE_TIME_UMTS_TARGETS ) && defined(FEATURE_SEC_TIME_V2) \
        && !defined(BUILD_BOOT_CHAIN))
         time_secure_set_secure_time(ts_value);
    #else /* FEATURE_TIME_UMTS_TARGETS && FEATURE_SEC_TIME_V2 && !BUILD_BOOT_CHAIN */
         /* Set time of day to the current time from the given source */
         
         ATS_MSG_HIGH("mm_fusion_debug:7K:ts_val[0]=0x%x, ts_val[1]=0x%x, offset=0x%x",ts_value[0],ts_value[1],info_ptr->ats_genoff_update.offset);
         time_conv_ms_to_timestamp(ts_value);

         /*Since this update is coming from remote modem, we do not want
          to send back the update when the genoff is updated and hence
          disable the update*/
         time_genoff_set_remote_modem_update(FALSE);

         /*Set the tod value. This should trigger a genoff update*/
         time_tod_set( ts_value );

         /*Enable the remote modem updates, since we will need those when
          tod is set by other modem functions*/
         time_genoff_set_remote_modem_update(TRUE);
         
    #endif /* FEATURE_TIME_UMTS_TARGETS && FEATURE_SEC_TIME_V2 && !BUILD_BOOT_CHAIN */
  }

} /* time_remote_genoff_mm_cb */

#endif /* IMAGE_APPS_PROC */

/*=============================================================================

FUNCTION TIME_SHUTDOWN_EVENT_CB

DESCRIPTION
  Function called when shutdown event is occuring.
  To prepare time-services for the shutdown event handling

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_shutdown_event_cb(void)
{
  /* Call the time-genoff shutdown notifier function */
  time_genoff_shutdown_notification_fn();

  return;
}

/*=============================================================================

FUNCTION TIME_FIND_TIME_GET_BASE

DESCRIPTION
  Function called whenever ATS_TOD is updated.
  In this we can check if ATS_TOD has time or not and update time_get_base 
  variable whether to use ATS_TOD or ATS_USER for time_get() operations.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_find_time_get_base(void *registered_genoff_ptr, int64 data)
{
   time_get_base = (0 != time_genoff_get_offset(ATS_TOD)) ? ATS_TOD : ATS_USER;
   return;
} /* time_find_time_get_base */


/*=============================================================================

FUNCTION TIME_TIMETICK_LOG_CB

DESCRIPTION
  Function called whenever ATS_TOD is updated.
  In this we can check if ATS_TOD has time or not and update time_get_base 
  variable whether to use ATS_TOD or ATS_USER for time_get() operations.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_timetick_log_cb(timer_cb_data_type data)
{
   uint64 now_tt = timetick_sclk64_get();
   time_type curr_time;
   time_type sum_genoff_ts_val;
   boolean ts_is_negative = time_genoff_get_sum_genoff_ts(time_get_base, sum_genoff_ts_val);
   
   /* Obtain current timestamp */
   (void)time_get(curr_time);
   
   /* Print to Diag */
   ATS_MSG_6("curr timetick: 0x%.8x%.8x, base: %d, sum_genoff_ts: 0x%.8x%.8x, is_negative: %d", 
                        now_tt>>32, (uint32)now_tt, time_get_base, 
                        sum_genoff_ts_val[1], sum_genoff_ts_val[0], ts_is_negative);

   if(data != 0)
      ULogFront_RealTimePrintf(time_ulog_handle, 1, "time set on base %d", data);

   /* Print to Ulog */
   ULogFront_RealTimePrintf(time_ulog_handle, 3, "base: %d ts: 0x%.8x%.8x", time_get_base, (uint32)(curr_time[1]), (uint32)curr_time[0]);
   
      
   return;
} /* time_timetick_log_cb */

/*=============================================================================

FUNCTION TIME_INIT_CONFIG

DESCRIPTION
  Registers for Callback Registration for Time_tod updates remote updates

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_init_config( void )
{
  /* Index for iteration */
  int i;

  /* EFS item file name */
  char f_name[120];
  
  ULogResult ulog_result = DAL_ERROR;

  /* Image proc or arm9 on MDM9x */
  #if (defined IMAGE_APPS_PROC || (defined T_MDM9X00 && !defined FEATURE_TIME_QDSP6)) 
  ats_client_id_type   time_genoff_set_id;
  time_genoff_set_id = time_genoff_set_id;
  #endif /* IMAGE_APPS_PROC || (T_MDM9X00 && !FEATURE_TIME_QDSP6 */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* We must init pmic before doing anything else in here */
  time_set_from_pmic();

  /* Initialize time efs */
  time_efs_init();

  /* Initialize for time_genoff */
  time_genoff_init();

  ats_rtc_init(); 

  /* Initialize Time_bases, Do not init ATS_RTC */
  for ( i =1; i < ATS_MAX; i++ )
  {
    (void) snprintf( f_name,sizeof(f_name),"ats_%d",i );
    ats_bases_init ( (time_bases_type)i, ATS_RTC, f_name);
  }
 
  time_genoff_add_proxy_base ( ATS_1X, ATS_TOD);
  time_genoff_add_proxy_base ( ATS_HDR, ATS_TOD);
  time_genoff_add_proxy_base ( ATS_WCDMA, ATS_TOD);
  time_genoff_add_proxy_base ( ATS_3GPP, ATS_TOD);
  time_genoff_add_proxy_base ( ATS_LTE_HR, ATS_TOD);
  time_genoff_add_proxy_base ( ATS_LTE_HR_GPS, ATS_TOD);
  
  /* Register for callback when ATS_TOD is set */
  (void)time_genoff_attach_cb(ATS_TOD, time_find_time_get_base, NULL);
  
  /* Call once just at init to know if TOD is ready or not */
  time_find_time_get_base(NULL, 0x1234567890);

  /* Register for the shutdown event callback notification */
  rcinit_register_termfn_group(RCINIT_GROUP_7, time_shutdown_event_cb);
  
  /*For 9K, A9 boots up faster than the Q6. Hence when
   Q6 comes to this stage, it needs to push all the 
   ats_bases update to A9*/
  #ifdef FEATURE_TIME_QDSP6
  ats_bases_set_remote_update(TRUE,FALSE);
  #endif

  #ifndef FEATURE_TIME_NO_RPC
  /* Image proc or arm9 on MDM9x */
  #if (defined IMAGE_APPS_PROC || (defined T_MDM9X00 && !defined FEATURE_TIME_QDSP6)) 
  ats_client_init( ATS_CLIENT_TYPE_GENOFF_UPDATE, &time_genoff_set_id, ATS_CLIENT_PROCESSOR_APP1  );
  ats_client_reg_cfg_cb( time_genoff_set_id, &time_remote_genoff_cb );

  #endif /* IMAGE_APPS_PROC */
  #endif
  
  /* Trigger the timer to print time, timetick info periodically */
  timer_def(&time_timetick_log_timer, NULL, NULL, 0, time_timetick_log_cb, (timer_cb_data_type)0);
  timer_set(&time_timetick_log_timer, TIME_TIMETICK_PRINT_FREQ, TIME_TIMETICK_PRINT_FREQ, T_SEC);
  ulog_result = ULogFront_RealTimeInit(&time_ulog_handle,           // A ULogHandle.
                           "timetick-time",     // 
                           1024,             // Allocate a 1k circular buffer.
                           ULOG_MEMORY_LOCAL, // 
                           ULOG_LOCK_OS);     // Will use an OS lock when writing
  if(ulog_result != DAL_SUCCESS)
     time_ulog_handle = 0;
} /*  time_init_config */




/*=============================================================================

FUNCTION TIME_TO_MS

DESCRIPTION
  Convert a timestamp from System Time to millisecond units

DEPENDENCIES
  None

RETURN VALUE
  The converted value is stored in the 'time' parameter.

SIDE EFFECTS
  None

=============================================================================*/

void time_to_ms
(
  /* In: Time in time-stamp format;  Out: Time in milliseconds */
  time_type                       ts_val
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Redirected call to time_conv_to_ms */
  time_conv_to_ms (ts_val);
  
} /* time_to_ms */



/*=============================================================================

FUNCTION TIME_TO_SEC

DESCRIPTION
  Convert a timestamp from System Time to seconds

DEPENDENCIES
  None

RETURN VALUE
  Seconds, corresponding to the time stamp

SIDE EFFECTS
  Input argument value is destroyed.

=============================================================================*/

uint32 time_to_sec
(
  /* In: Time in time-stamp format;  Out: Garbage */
  time_type                       ts_val
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Redirected call to time_conv_to_sec */
  return time_conv_to_sec(ts_val);
  
} /* time_to_sec */


/*=============================================================================

FUNCTION TIME_GET_SUM_GENOFF_TS

DESCRIPTION
  Returns sum generic offset value of system base in timestamp format

DEPENDENCIES
  None

RETURN VALUE
  boolean as return value
  Provides sum generic ts offset value as part of inputs arguments

SIDE EFFECTS
  None

=============================================================================*/
boolean time_get_sum_genoff_ts
( 
    time_type        sum_genoff_ts_val
)
{
  return time_genoff_get_sum_genoff_ts(time_get_base, sum_genoff_ts_val);
} /*  time_get_sum_genoff_ts */


/*=============================================================================

FUNCTION TIME_GET

DESCRIPTION
  Returns the current time

DEPENDENCIES
  A valid CDMA time, HDR time, GSM time, or a Time-of-Day, etc.

RETURN VALUE
  Time-stamp returned to ts_val parameter.
  Time source used to determine time is returned

SIDE EFFECTS
  None

=============================================================================*/

time_source_enum_type time_get
(
  /* OUT: The current time */
  time_type                       ts_val
)
{
  
  /* Agrument to time_genoff_opr */
  time_genoff_args_struct_type    temp_genoff_args;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  temp_genoff_args.base        = time_get_base;
  temp_genoff_args.base_source = TIME_SCLK;
  temp_genoff_args.ts_val      = ts_val;
  temp_genoff_args.unit        = TIME_STAMP;
  temp_genoff_args.operation   = T_GET;
  temp_genoff_args.dal_timetick_val_valid = 0;
  temp_genoff_args.dal_timetick_val       = 0;
  time_genoff_opr( &temp_genoff_args );

  return TIME_SOURCE_32KHZ;

} /* time_get */


/*=============================================================================

FUNCTION TIME_GET_OPTIMIZED_TILL_MSEC

DESCRIPTION
  Returns the current time in msecs resolution.

DEPENDENCIES
  A valid CDMA time, HDR time, GSM time, or a Time-of-Day, etc.

RETURN VALUE
  Time-stamp returned to ts_val parameter.
  Time source used to determine time is returned

SIDE EFFECTS
  None

=============================================================================*/

time_source_enum_type time_get_optimized_till_msec
(
  /* OUT: The current time */
  time_type                       ts_val
)
{
  static time_type ts_val_msecs = {0,0};
  uint64 tt_now = 0; 
  static uint64 tt_msec_boundry = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* get the current timetick */
  tt_now = timetick_sclk64_get();

  /* 
   * Check to see if the msecs boundary has been crossed or if there
   * is a time update in between, then we need to re-calculate the 
   * latest time using time_get(). 
   */
  if ( tt_now >= tt_msec_boundry )
    {
	/* Get the time */
    time_get(ts_val);
    
    /* Update the msecs ts_val value */
	ts_val_msecs[0] = ts_val[0];
	ts_val_msecs[1] = ts_val[1];
    
	/* Advance the timetick boundary to the nearest 1 msecs timetick value */
	tt_msec_boundry += 19200;
	
	/* In case the 1 msecs increment was not enough, recalculate freshly */
	if(tt_msec_boundry <= tt_now)
		{
		/* Recalculate the new boundary */
		/* Recalculate the new boundary */
		tt_now /= 19200;
		tt_now *= 19200;
		tt_now += 19200;
		tt_msec_boundry = tt_now;
		}
    }
  else
    {
    /* we have not yet crossed the msecs boundary, so the same time can be used */
	ts_val[0] = ts_val_msecs[0];
	ts_val[1] = ts_val_msecs[1];
    }

	return TIME_SOURCE_32KHZ;

} /* time_get_optimized_till_msec */



/*=============================================================================

FUNCTION TIME_GET_MS

DESCRIPTION
  Get the system time, in # of milliseconds since "the beginning of time".

DEPENDENCIES
  A valid CDMA time, or a valid HDR time, or a valid Time-of-Day, etc.

RETURN VALUE
  Milliseconds since 6 Jan 1980 00:00:00 returned to the time parameter

SIDE EFFECTS
  None

=============================================================================*/

void time_get_ms
(
  /* Out: Time in milliseconds since 6 Jan 1980 00:00:00 */
  time_type                       ts_val
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Read the current time of day */
  (void) time_get( ts_val );

  /* Convert time-stamp format to milliseconds */
  time_conv_to_ms( ts_val );

} /* time_get_ms */



/*=============================================================================

FUNCTION TIME_SET_MS

DESCRIPTION
  Number of milliseconds since "the beginning of time" that the system time
  needs to be set to

DEPENDENCIES
  A valid CDMA time, or a valid HDR time, or a valid Time-of-Day, etc.

RETURN VALUE
  None

SIDE EFFECTS
  Timestamp changed to the a_time sent as parameter to this function.

=============================================================================*/

void time_set_ms
(
  /* In: Time in milliseconds since 6 Jan 1980 00:00:00 */
  time_type                       a_time
)
{
#ifdef FEATURE_TIME_UMTS_TARGETS 
  /* Timestamp */
  time_type                       timestamp;

  /* Arguments to be sent to set  ATS_TOD */
  time_genoff_args_struct_type    temp_genoff_args;
  

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Convert from ms to 1.25 ms units */
  qw_equ(timestamp, a_time);
  qw_mul(timestamp, timestamp, 4);
  (void) qw_div(timestamp, timestamp, 5);

  /* Shift up by 16 to convert to get timestamp */
  qw_shift(timestamp, TIME_1p25MS_TO_TS_SHIFT);


#if (defined( FEATURE_TIME_UMTS_TARGETS ) && defined(FEATURE_SEC_TIME_V2) \
      && !defined(BUILD_BOOT_CHAIN))
  time_secure_set_user_time(timestamp);
#else /* FEATURE_TIME_UMTS_TARGETS && FEATURE_SEC_TIME_V2 && !BUILD_BOOT_CHAIN */
    
  temp_genoff_args.base        = ATS_TOD; 
  temp_genoff_args.base_source = TIME_SCLK ;
  temp_genoff_args.ts_val      = &a_time;
  temp_genoff_args.unit        = TIME_MSEC;
  temp_genoff_args.operation   = T_SET;
  temp_genoff_args.dal_timetick_val_valid = 0;
  temp_genoff_args.dal_timetick_val       = 0;
  time_genoff_opr( &temp_genoff_args );
    
#endif /* FEATURE_TIME_UMTS_TARGETS && FEATURE_SEC_TIME_V2 && !BUILD_BOOT_CHAIN */

#else
  ATS_MSG("time_set_ms(): Not implemented");
#endif /* FEATURE_TIME_UMTS_TARGETS */
  
} /* time_set_ms */



/*=============================================================================

FUNCTION TIME_GET_SECS

DESCRIPTION
  Get the system time, in # of seconds since "the beginning of time".
  136 year range, beginning 6 Jan 1980 00:00:00.

DEPENDENCIES
  A valid CDMA time, or a valid HDR time, or a valid Time-of-Day, etc.

RETURN VALUE
  Seconds since 6 Jan 1980 00:00:00

SIDE EFFECTS
  None

=============================================================================*/

uint32 time_get_secs( void )
{
  /* Timestamp */
  time_type                       ts_val;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Read the current time of day */
  (void) time_get( ts_val );

  /* Convert to seconds, and return */
  return time_conv_to_sec( ts_val );

} /* time_get_secs */


/*=============================================================================

FUNCTION TIME_GET_JULIAN

DESCRIPTION
  Get the system time, in julian format

DEPENDENCIES
  A valid CDMA time, or a valid HDR time, or a valid Time-of-Day, etc.

RETURN VALUE
  None

SIDE EFFECTS
  Modified input parameter and store julian time

=============================================================================*/
void time_get_julian
(
  /* Time value prior to updating to the correct time */
  time_julian_type *jul_time
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  time_jul_from_secs( time_get_secs(), jul_time );

} /* time_get_julian */


/*=============================================================================

FUNCTION TIME_GET_JULIAN_LEAP_SEC

DESCRIPTION
  Get the system time along with leap seconds, in julian format

DEPENDENCIES
  A valid CDMA time, or a valid HDR time, or a valid Time-of-Day, etc.

RETURN VALUE
  None

SIDE EFFECTS
  Modified input parameter and store julian time

=============================================================================*/
void time_get_julian_leap_sec
(
  /* Time value prior to updating to the correct time */
  time_julian_type *jul_time
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  uint32 sys_time = 0;
    
  sys_time = time_get_secs();
  
  time_jul_from_secs( sys_time, jul_time );
  return;
} /* time_get_julian */



/*=============================================================================

FUNCTION TIME_SET_JULIAN

DESCRIPTION
  Set the system time

DEPENDENCIES
  A valid CDMA time, or a valid HDR time, or a valid Time-of-Day, etc.

RETURN VALUE
  None

SIDE EFFECTS
  Modifies system timestamp and also updates PMIC time

=============================================================================*/
void time_set_julian
(
  /* Time value to set time to */
  time_julian_type                *jul_time
)
{
  /* Arguments to be sent to set  ATS_TOD */
  time_genoff_args_struct_type    temp_genoff_args;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #if (defined( FEATURE_TIME_UMTS_TARGETS ) && defined(FEATURE_SEC_TIME_V2) \
      && !defined(BUILD_BOOT_CHAIN))
     time_secure_set_user_time_secs(secs);
  #else /* FEATURE_TIME_UMTS_TARGETS && FEATURE_SEC_TIME_V2 && !BUILD_BOOT_CHAIN */
 
      temp_genoff_args.base        = ATS_TOD; 
      temp_genoff_args.base_source = TIME_SCLK ;
      temp_genoff_args.ts_val      = jul_time;
      temp_genoff_args.unit        = TIME_JULIAN;
      temp_genoff_args.operation   = T_SET;
      temp_genoff_args.dal_timetick_val_valid = 0;
      temp_genoff_args.dal_timetick_val       = 0;
      time_genoff_opr( &temp_genoff_args );
    
  #endif /* FEATURE_TIME_UMTS_TARGETS && FEATURE_SEC_TIME_V2 && !BUILD_BOOT_CHAIN */

} /* time_set_julian */



/*=============================================================================

FUNCTION TIME_GET_UPTIME

DESCRIPTION
  Returns the uptime in timestamp format

DEPENDENCIES
  
RETURN VALUE
  Source of the time

SIDE EFFECTS
  None

=============================================================================*/

time_source_enum_type time_get_uptime
(
  time_type                       ts_val
)
{
  qw_set( ts_val, 0, 0 );
  
  /* Get uptime from sclk_time */
  time_sclk_get( ts_val, (uint64)-1);

  return TIME_SOURCE_32KHZ;
} /* time_get_uptime */


/*=============================================================================

FUNCTION TIME_GET_UPTIME_SECS

DESCRIPTION
  Get time the phone has been powered on for

DEPENDENCIES
  A valid CDMA time, or a valid HDR time, or a valid Time-of-Day, etc.

RETURN VALUE
  Number of seconds phone has been powered on for.

SIDE EFFECTS
  None

=============================================================================*/

uint32 time_get_uptime_secs( void )
{
  /* System time in seconds */
  uint32                          system_time;

  /* Milliseconds phone has been powered on for */
  time_type                       uptime_ms;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get uptime in ms */
  time_get_uptime_ms( uptime_ms );

  /* Convert to seconds */
  (void)qw_div( uptime_ms, uptime_ms, 1000 );
  system_time = qw_lo( uptime_ms );

  /* Return calculated value */
  return system_time;

} /* time_get_uptime_secs */



/*=============================================================================

FUNCTION TIME_GET_UPTIME_MS

DESCRIPTION
  Get time the phone has been powered on for

DEPENDENCIES
  A valid CDMA time, or a valid HDR time, or a valid Time-of-Day, etc.

RETURN VALUE
  Number of milliseconds phone has been powered on for returned to 'uptime_ms'

SIDE EFFECTS
  None

=============================================================================*/

void time_get_uptime_ms
(
  /* Output: Milliseconds phone has been powered on for */
  time_type                       uptime_ms
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  qw_set( uptime_ms, 0, 0 );
  
  /* Get uptime in timestamp format */
  time_get_uptime( uptime_ms );

  /* Convert uptime in millisecond */
  time_conv_to_ms( uptime_ms );

} /* time_get_uptime_ms */



/*=============================================================================

FUNCTION TIME_SYNC_TIME_OF_DAY

DESCRIPTION
  Synchronize the slow-clock based time-of-day to the current "ts" time.

DEPENDENCIES
  A valid CDMA, HDR, GSM, GPS, WCDMA (etc) time.  time_get( ) must not be
  using time_tod_get( ) when this function is called.

RETURN VALUE
  None

SIDE EFFECTS
  An diagnostic message is generated if there is a sudden jump in time-of-day.

=============================================================================*/

void time_sync_time_of_day( void )
{
  /* Timestamp to set time of day to */
  time_type                       ts_val;

  /* Timesource of timestamp */
  time_source_enum_type           source;

  time_genoff_args_struct_type    temp_genoff_args;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ATS_INTLOCK();

  /* Get current time */
  source = time_get( ts_val );

  if ( source != TIME_SOURCE_32KHZ )
  {
    
   #if (!defined(FEATURE_EXPORT_TIME_REMOTE_MM_FUSION_CLNT) &&\
          !defined (FEATURE_SEC_TIME_V2) )   
   if ( source == TIME_SOURCE_CDMA )
   {
     temp_genoff_args.base        = ATS_1X;
   }
   else if ( source == TIME_SOURCE_HDR )
   {
     temp_genoff_args.base        = ATS_HDR;
   }
   else if ( source == TIME_SOURCE_GPS )
   {
     temp_genoff_args.base        = ATS_GPS;
   }
   else if ( source == TIME_SOURCE_MFLO )
   {
     temp_genoff_args.base        = ATS_MFLO;
   }
   else /*do not set any time by default*/
   {
     ATS_ERR_1("Invalid technology %d trying to set time. Ignored", source);
     return;
   }

   temp_genoff_args.base_source = TIME_SCLK;
   temp_genoff_args.ts_val      = ts_val;
   temp_genoff_args.unit        = TIME_STAMP;
   temp_genoff_args.operation   = T_SET;
   temp_genoff_args.dal_timetick_val_valid = 0;
   temp_genoff_args.dal_timetick_val       = 0;
   time_genoff_opr( &temp_genoff_args );

   #endif /* FEATURE_EXPORT_TIME_REMOTE_MM_FUSION */
 
  }
  else
  {
    ATS_ERR("Can't sync time-of-day to time-of-day");
  }

  ATS_INTFREE();

} /* time_sync_time_of_day */



/*=============================================================================

FUNCTION TIME_SET_FROM_PMIC

DESCRIPTION
  Initialize the slow-clock based time-of-day to the PMIC's RTC time.

DEPENDENCIES
  Time-of-day must not have been initialized be another time source.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_set_from_pmic( void )
{

  /*At this point in time, the pmic has init. Therefore, 
  initialize the time_pm fn ptrs here which requires
  querying the pmic model*/
  time_fn_map_initialize_fp();
  
} /* time_set_from_pmic */



/*=============================================================================

FUNCTION TIME_TO_MS_NATIVE

DESCRIPTION
  Convert a timestamp from System Time to millisecond units in native uint64
  format.

DEPENDENCIES
  None

RETURN VALUE
  The converted value.

SIDE EFFECTS
  None

=============================================================================*/

uint64 time_to_ms_native
(
  /* Time in timestamp format. */
  time_type                       time_stamp 
)
{
  /* Time in quadword format (time_type). */
  time_type                       temp_time;   

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   
  qw_equ( temp_time, time_stamp );

  /* Use the helper function to convert the time to milliseconds. */
  time_conv_to_ms( temp_time );

  /* Then convert to uint64 format. */
  return QW_CVT_Q2N( temp_time );

} /* time_to_ms_native */



/*=============================================================================

FUNCTION TIME_GET_MS_NATIVE

DESCRIPTION
  Get a timestamp from System Time in millisecond units from 6 Jan 1980 
  00:00:00.

DEPENDENCIES
  None

RETURN VALUE
  The time in ms from 6 Jan 1980 00:00:00.

SIDE EFFECTS
  None

=============================================================================*/

uint64 time_get_ms_native( void )
{
  /* Time in quadword format (time_type). */
  time_type                       ts_val;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the current time in ms and return it in native uint64 format. */
  time_get_ms( ts_val );

  return QW_CVT_Q2N( ts_val );

} /* time_get_ms_native */

/*In 9K, both ARM9 and Q6 are defined as modem proc and hence the below 
conditions.*/
#if (((defined(IMAGE_MODEM_PROC)&& !defined T_MDM9X00) || defined FEATURE_TIME_QDSP6) && defined(FEATURE_TOD_MULTIPROC_SYNC))
/*=============================================================================

FUNCTION TIME_REMOTE_UPDATE_APPS_TO_MODEM_OFFSET

DESCRIPTION
  Updates apps_to_modem_offset on modem side.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_remote_update_apps_to_modem_offset
(
  int32 offset
)
{
  time_apps_to_modem_offset = offset; 
} /* time_remote_update_apps_to_modem_offset */

/*=============================================================================

FUNCTION TIME_GET_APPS_USER_TIME

DESCRIPTION
  Get Apps user time.

DEPENDENCIES
  Used only for WM . This will return same as time_get on Brew.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_get_apps_user_time
(
  /* OUT: The current time */
  time_type                       ts_val
)
{
  time_type timestamp;
  
  /* Get time_get(timetype ts_val) */
  time_get( QW_CVT_N2Q(ts_val) );

  timestamp[1] = (uint32)((time_apps_to_modem_offset >> 32) & 0xFFFFFFFFuL);
  timestamp[0] = (time_apps_to_modem_offset & 0xFFFFFFFFuL);
  qw_mul(timestamp, timestamp, 800);
  qw_shift(timestamp, 16);
  qw_add(ts_val,ts_val,timestamp);

} /* time_get_apps_user_time */
#endif /* IMAGE_MODEM_PROC && !T_MDM9X00 || !FEATURE_TIME_QDSP6 */


/*=============================================================================

FUNCTION TIME_GET_TIME_DEFINES

DESCRIPTION
  Populates the time_defines_type data struncture supplied through
  a pointer  .

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_get_time_defines(time_defines_type* time_defines_copy)
{
  *time_defines_copy = time_defines;
}

/*=============================================================================

FUNCTION TIME_START_MM_REGISTRATION

DESCRIPTION
  Kicks off the registration of the client modem with the server modem

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_start_mm_registration
(
  void
) 
{
#ifdef FEATURE_EXPORT_TIME_REMOTE_MM_FUSION_CLNT
  ats_client_id_type   time_genoff_set_id;
  
  ats_mm_client_init( ATS_CLIENT_TYPE_MM_GENOFF_UPDATE, 
                      &time_genoff_set_id,
                      ATS_CLIENT_PROCESSOR_MM_MODEM  );
  ats_mm_client_reg_cfg_cb( time_genoff_set_id, 
                            &time_remote_genoff_mm_cb );

  ATS_MSG("mm_fusion_debug:Registered with Server Modem");

#endif /*#endif FEATURE_EXPORT_TIME_REMOTE_MM_FUSION_CLNT*/
}


