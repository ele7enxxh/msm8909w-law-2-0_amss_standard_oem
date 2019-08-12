/*=============================================================================

 T I M E   S E R V I C E   G E N E R I C  F R A M E W O R K  S U B S Y S T E M

GENERAL DESCRIPTION
  Implements Generic framework to handle Time bases.

EXTERNALIZED FUNCTIONS
  time_genoff_pre_init
    Pre -Initializes Generic offset

  time_genoff_post_init
    Completes initialization sequence of Generic offset

  time_genoff_add_base_subsys
    Adds a genoff instance as base_subsystem for another genoff instance

  time_genoff_attach_cb
    Attaches call back which are called during after set operation

  time_genoff_set_remote_genoff
    Updates genoff in other proc

  time_genoff_add_init_call
    Adds initialization sequence for the genoff in consideration

  time_genoff_updates_per_storage
    Updates persistent storage info

  time_genoff_get_pointer
    Returns the Pointer to different time bases

  time_genoff_set_generic_offset
    Sets Generic offset.

  time_genoff_opr
   Does time set/get operation on the basis of Inputs

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Should be initialized with following sequence

  1) time_genoff_pre_init
  2) time_genoff_add_init_func ( Only if init_func is needed  ) 
  3) time_genoff_add_base_subsys ( Only if base_subsys is needed ) 
  4) time_genoff_updates_per_storage ( Only on Modem Proc, If NV is not used )
  5) time_genoff_attach_cb ( Only if cb_func is needed )
  6) time_genoff_post_init
 
    
Copyright (c) 2009 - 2015 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/time/src/time_genoff.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/15   abh     Changes for correctly handling ATS_PRIVATE bases.
04/26/11   din     Fixed compiler warnings.
03/11/11   ab      Removed compiler warnings.
08/20/10   din     Added a new optimized time_genoff_get_optimized call.
07/12/10   din     Proxy base for a base is updated directly. Added validity member in 
                   genoff_structure to control remote updates.
07/12/10   din     Used internal features for better readability.
06/23/10   din     Added new time bases and attached a proxy base for a base.
04/19/10   din     Prevented Multiple efs_writes for same file. 
02/22/10   din     Fixed compiler error incase IMAGE_MODEM_PROC is not defined.
02/19/10   din     Moved genoff RPC calls from modem proc to TIME_IPC task.
12/13/09   din     Changed sclk64_to_timestamp to sclk_to_timestamp_64.
12/08/09   din     Removed compilation warnings.
10/07/09   din     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "time_genoff_v.h"

#if defined(FEATURE_MULTIPROCESSOR) && !defined(FEATURE_TIME_NO_RPC)
#include "time_remote.h"
#endif /* FEATURE_MULTIPROCESSOR && !FEATURE_TIME_NO_RPC */

#include <stringl/stringl.h>
#include "time_svc.h"
#include "time_jul.h"
#include "time_conv.h"
#include "time_efs.h"
#include "time_ipc.h"
#include "timetick_sclk64.h"
#include "ats_v.h"
#include "stringl/stringl.h"
#include "amssassert.h"
#include "queue.h"
#include "osal.h"
#include "log.h"
#include "timetick.h"
#include "rcinit.h"

#ifdef FEATURE_QMI_TIME_REMOTE_CLNT
#include "time_service_v01.h"
#include "qmi_time_client.h"
#endif
                           
#include "time_fn_map.h"



/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/


/* This is to check for offset boundaries.
  * During msecs to timestamp conversion, 
  * any values in the upper word which is 
  * greater than the below threshold will 
  * be lost, causing the ouput to be incorrect.
  * Thus we need to check the offset values against this threashold.
  */
#define TIME_GENOFF_OFFSET_MAX_VAL_UPPER_WORD     0x14000


/*--------------------------------------------------------------------------
  Time-services debug logs
--------------------------------------------------------------------------*/
#ifdef FEATURE_TIME_DEBUG

/* Time set log */
static time_genoff_set_log_type time_genoff_set_log[MAX_TIME_GENOFF_TIME_SET_LOG_SIZE];
static uint32 time_genoff_set_log_cntr = 0;

/* Log containing last TOD update information */
time_genoff_last_tod_update_log_type time_genoff_last_tod_update_log;

/* Log for time_genoff_set_params API */
static time_genoff_set_params_log_type time_genoff_set_params_log;

#endif /* FEATURE_TIME_DEBUG */


/*--------------------------------------------------------------------------
  Function Prototypes
--------------------------------------------------------------------------*/
static void time_genoff_set_helper
(
	time_genoff_ptr ptime_genoff,
	int64           new_ms,
    time_bases_type parent_base
);

static void time_genoff_send_tod_error_notification
(
  uint64,
  uint64,
  int64
);

static void time_genoff_write_to_efs
(
  time_genoff_ptr,
  boolean
);

extern void time_timetick_log_cb(timer_cb_data_type data);

/* To store which bases have their offsets reset to zero.
    Each set bit indicates the reset of a base per the bit 
    number. */
static uint32 time_genoff_efs_reset_bases = 0;

/* To store which bases have their offsets reset to zero,
   each bit if set will denote the respective base based
   their position */
static uint32 time_genoff_reset_bases = 0;

/*--------------------------------------------------------------------------
  Linting...
--------------------------------------------------------------------------*/

/*lint -esym(715,unused_*)  parameters prefixed with unused_ are not used */


/*-----------------------------------------------------------------------------
  Macros used for converting to/from ts_timestamp format
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  Generic offset information
-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
  Time Bases static information
-----------------------------------------------------------------------------*/

time_genoff_struct_type              ats_bases[ATS_MAX] = {{0}};

/* Array for pointers to external genoff bases */
time_genoff_ptr                      ats_private_bases[ATS_PRIVATE_MAX] = {0};

/* Mutex for operation on ats_private_bases */
static time_osal_mutex_t             ats_private_mutex;

static boolean                       ats_private_bases_inited = FALSE;

#ifdef FEATURE_TIME_MULTI_SERVER_PROC
static ats_cfg_event_info_s_type         cfg_event_info;
#endif 

#if(defined FEATURE_TIME_MULTI_SERVER_PROC || defined FEATURE_QMI_TIME_REMOTE)

/* Queue to store base to be remoted through TIME IPC TASK */
static q_type                            rpc_q;

/* Flag to indicate the initialization status of rpc_q */
static boolean                           rpc_q_initialized = FALSE;
	
typedef enum queue_enum
{
NOT_IN_QUEUE = 0,
IN_QUEUE
} queue_q;

typedef struct rpc_struct_type
{
  /* Link for the next queue */
  q_link_type                            link;

  /* time base stored as data in the node of queue */
  time_bases_type                        base;

  /* Enum to check if element is in queue or outside queue */
  queue_q                                queue;

} rpc_struct;

static rpc_struct                        rpc_base[ATS_MAX];

#endif /* FEATURE_TIME_MULTI_SERVER_PROC */

static uint8 time_genoff_update_remote_modem = 1;

#ifdef FEATURE_QMI_TIME_REMOTE_CLNT
extern qmi_client_type time_qmi_client_handle;
static time_genoff_leap_sec_info_type time_genoff_leap_sec_info={FALSE,0};
#endif

static time_delta_report_log_type time_delta_log;

#if(defined FEATURE_TIME_MULTI_SERVER_PROC || defined FEATURE_QMI_TIME_REMOTE)
static rex_tcb_type *time_ipc_tcb_genoff_ptr = NULL;
#endif

#ifdef FEATURE_QMI_TIME_REMOTE
static uint32 time_genoff_fps_initialized = 0;
#endif /*FEATURE_QMI_TIME_REMOTE*/

#if (defined FEATURE_TIME_MULTI_SERVER_PROC || defined FEATURE_QMI_TIME_REMOTE)
static time_fn_map_fp_type * time_genoff_fps;
#endif

/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================

FUNCTION TIME_GENOFF_INIT

DESCRIPTION
  Initializes Generic offset related structures to be used later in time genoff operations. 

DEPENDENCIES
  This function should be called before any base initialization is called for.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_init(  void  )
{
  /* Memset the mutex object  */
  memset(&ats_private_mutex,0x0,sizeof(time_osal_mutex_t));

  /* Init the time genoff mutex */
  TIME_OSAL_MUTEX_INIT(&ats_private_mutex);

  /* Mark flag to indicate completion of time genoff private base structure */
  ats_private_bases_inited = TRUE;

  /* Initialize rpc_q */
#if (defined FEATURE_TIME_MULTI_SERVER_PROC || defined FEATURE_QMI_TIME_REMOTE)
  if (rpc_q_initialized == FALSE)
  {
    rpc_q_initialized = TRUE;
    q_init ( &rpc_q );
  }

#endif /* FEATURE_TIME_MULTI_SERVER_PROC */
  

  /* For now, there is no other 
     * initialization being done 
     * here for time genoff. 
     */
} /*  time_genoff_init */



/*=============================================================================

FUNCTION TIME_GENOFF_ARGS_INIT

DESCRIPTION
  Initializes Generic offset args to be used later in time_genoff_opr. 

DEPENDENCIES
  None. 

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_args_init
(
  /* Pointer to Genoff args in consideration */
  time_genoff_args_ptr pargs
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (pargs == NULL)
  {
    ERR_FATAL("time_genoff_args_init: pargs is NULL",0,0,0);
	return;
  }

  /* Initialize the genoff arg to default values */
  pargs->base                   = ATS_INVALID;
  pargs->ts_val                 = NULL;
  pargs->unit                   = TIME_STAMP;
  pargs->operation              = T_GET;
  pargs->base_source            = TIMETICK_SCLK64;
  pargs->dal_timetick_val_valid = 0;
  pargs->dal_timetick_val       = 0;
  
} /*  time_genoff_args_init */



/*=============================================================================

FUNCTION TIME_GENOFF_PRE_INIT

DESCRIPTION
  Pre -Initializes Generic offset. 

DEPENDENCIES
  Should be followed by other initialization calls and finally followed by 
  time_genoff_post_init 

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_pre_init
(
  /* Pointer to Genoff in consideration */
  time_genoff_ptr time_genoff,

  /* Time base type for the Genoff in consideration */
  time_bases_type base  
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Mark time_genoff as not valid */
  time_genoff->valid = FALSE;  
  
  /* Initially mark time_genoff with no subsys */
  time_genoff->reqd_base_genoff = FALSE ;

  /* Initially set Generic Offset as 0 */
  time_genoff->generic_offset = 0;  

  /* Initially set sum_generic_offset as 0 */
  time_genoff->sum_generic_offset = 0;

  /* Initially set sum_generic_offset_ts to 0 */
  qw_set(time_genoff->sum_generic_offset_ts, 0, 0);

  /* Mark  sum_generic_offset_ts_is_negative as FALSE */
  time_genoff->sum_generic_offset_ts_is_negative = FALSE ;  

  /* Initially set number_of_cb_func as 0 */
  time_genoff->number_of_cb_func = 0;

  /* Initialize time-base type */
  time_genoff->bases_type =  base;

  /* Initially set init_func as NULL */
  time_genoff->init_func = NULL;
      
  /* Initially set storage spec as uninitilized */
  time_genoff->per_storage_spec.initialized = FALSE;
  
  /* Initially set remote_call_needed as FALSE */
  time_genoff->remote_call_needed = FALSE;

  /* Initially set allow_remote_updates as TRUE */
  time_genoff->allow_remote_updates = TRUE;

  /* Initially set proxy base with ATS_INVALID */
  time_genoff->proxy_base = ATS_INVALID ;

  /*Initially set the in_coverage flag as FALSE*/
  time_genoff->in_coverage_time = FALSE;
  
  /* Initially set the last_efs_written value to zero */
  time_genoff->time_genoff_last_efs_write_val = 0;

  /* Initialize the time genoff mutex */
  time_genoff->mutex = malloc(sizeof(time_osal_mutex_t));
  if (time_genoff->mutex != NULL)
  {
   /* Memset the obtained memory before passing to init function */
   memset(time_genoff->mutex,0x0,sizeof(time_osal_mutex_t));

   /* Call the init function */
  TIME_OSAL_MUTEX_INIT(time_genoff->mutex);
  }
  else
  {
   ERR_FATAL("Failed to allocate memory for time_genoff mutex object",0,0,0);
  }

  /* Initialize the remote_pd number */
  time_genoff->remote_pd = TIME_GENOFF_DUMMY_PD_NUMBER;

  /* Update the time_genoff state */
  time_genoff->state.time_genoff_state_pre_init = 1;

} /*  time_genoff_pre_init */



/*=============================================================================

FUNCTION TIME_GENOFF_CB_UPDATE_SUM_GEN_OFF

DESCRIPTION
  Updates Sum of Generic offset in ts_val format  

DEPENDENCIES
  None

RETURN VALUE
  TRUE if valid, FALSE if not valid

SIDE EFFECTS
  None

=============================================================================*/

static void time_genoff_cb_update_sum_gen_off
( 
  /* Base that has registered for callback */
  void  *registered_genoff_ptr,
  
  /* Pointer to Genoff in consideration */
  int64 data
)
{
  /* Pointer to 'base' in consideration */
  time_genoff_ptr ptime_genoff, ptime_genoff_local;

  /* Pointer to sum_generic_offset */
  uint64   sum_generic_offset_local=0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( registered_genoff_ptr == NULL )
  {
    return;
  }
  
  /* Set data to zero , to avoid compiler warning */
  data = 0 * data ;

  /* Get pointer to base in consideration */
  ptime_genoff = (time_genoff_ptr)registered_genoff_ptr;

  ptime_genoff_local = ptime_genoff;

  /* Copy generic offset to sum_generic_offset */
  sum_generic_offset_local = ptime_genoff->generic_offset;

  while ( ptime_genoff->reqd_base_genoff != FALSE )
  {
    /* Get pointer to subsys_base in consideration */
    ptime_genoff = time_genoff_get_pointer( ptime_genoff->subsys_base );

    /* Add generic offset of subsys_base to sum_generic_offset */
    sum_generic_offset_local += ptime_genoff->generic_offset;
  }

  /* Update the sum generic offset field*/
  ptime_genoff_local->sum_generic_offset = sum_generic_offset_local;

  /* Check if the sum_generic_offset is negative or positive */
  if((int64)sum_generic_offset_local < 0)
  	{
	/* if negative, then assign sum_generic_offset_ts_is_negative as TRUE
	    and get the absolute value of sum_generic_offset */
  	sum_generic_offset_local = (-1)*sum_generic_offset_local ;
	ptime_genoff_local->sum_generic_offset_ts_is_negative = TRUE ;
  	}
  else
  	{
  	/* Update the sum_generic_offset_ts_is_negative field */
  	ptime_genoff_local->sum_generic_offset_ts_is_negative = FALSE;
  	}

  /* Finally copy the sum of generic offset in ms format to ts format */
  qw_equ_misaligned((void *)ptime_genoff_local->sum_generic_offset_ts,(void *)&sum_generic_offset_local);
  time_conv_ms_to_timestamp(ptime_genoff_local->sum_generic_offset_ts);  
 
} /* time_genoff_cb_update_sum_gen_off */

/*=============================================================================

FUNCTION TIME_GENOFF_CHECK_TIME

DESCRIPTION
  To get the total time offset in msecs format that can be added to EPOCH,
  If the total time comes out to be negative then returns the uptime only
  and resets the generic offsets for the passed time genoff pointer to zero.

DEPENDENCIES
  None 

RETURN VALUE
  Time offset to be added to EPOCH in msecs format.

SIDE EFFECTS
  None

=============================================================================*/

static boolean time_genoff_check_time
(
  /* Pointer to Genoff in consideration */
  time_genoff_ptr time_genoff
)
{
  /* Pointer to sum_generic_offset */
  int64         time_temp = 0;

  /*temporary timestamp value*/
  time_type 	ts_temp_val;

  qw_set( ts_temp_val, 0, 0 );
  
  /* Get the uptime */
  time_sclk_get( ts_temp_val, (uint64)-1 );
  
  time_to_ms(ts_temp_val);

  qw_equ_misaligned((void*)&time_temp, ts_temp_val);

  /* get the total time using sum_generic_offset + uptime */
  time_temp = time_temp + time_genoff->sum_generic_offset;

  /* The time in msecs to be added to EPOCH can not be negative,
     check for this & return FALSE if it is so. */
  if( time_temp < 0 ) 
	{
	  	  ATS_MSG_ERROR("TIME_DEBUG: time_genoff_check_time: Total time is invalid, sum_genoff=0x%.8x%.8x base=0x%x",
	                (uint32)(time_genoff->sum_generic_offset >> 32), 
				    (uint32)(time_genoff->sum_generic_offset & 0xFFFFFFFF),
				    time_genoff->bases_type);
		return FALSE;
	}
  else
  	{
  		return TRUE;
  	}

} /*  time_genoff_check_time */


/*=============================================================================

FUNCTION TIME_GENOFF_POST_INIT

DESCRIPTION
  Completes initialization sequence of generic offset

DEPENDENCIES
  Should be preceeded by time_genoff_pre_init and other init calls

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_post_init
(
  /* Pointer to Genoff in consideration */
  time_genoff_ptr time_genoff
)
{

  /* For notifying the reset of a base for EFS updation */
  boolean time_genoff_base_reset_done = FALSE;
  int64   temp_offset;
    
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Acquire the mutex */
  TIME_OSAL_MUTEX_LOCK(time_genoff->mutex);

  #ifdef FEATURE_TIME_SINGLE_MULTI_SERVER_PROC
  
  /* If persistent storage spec is updated */
  if ( time_genoff->per_storage_spec.initialized == TRUE )
  {
    /* Read generic offset from efs item file */
    if ( TIME_EFS_RW_STAT_OK != 
         time_efs_rw( time_genoff->per_storage_spec.f_name,
                      &(time_genoff->generic_offset),
                      TIME_EFS_RW_CMD_RD ))
    {
      ATS_MSG_SPRINTF_2("time_genoff_init : Error reading EFS file, base: 0x%x;f_name=%s"
	  	                 ,time_genoff->bases_type,time_genoff->per_storage_spec.f_name);
    }
	else
	{
	 /* update the efs value read for the respective offset */
	 time_genoff->time_genoff_last_efs_write_val = time_genoff->generic_offset ;
	}
  }

  #endif /* FEATURE_TIME_SINGLE_MULTI_SERVER_PROC */

  /* Add callback for all offsets except ATS_RTC */
  if ( time_genoff->bases_type != ATS_RTC )
  {
    (void)time_genoff_attach_cb( ATS_RTC, 
                           (time_genoff_t2_cb_type)&time_genoff_cb_update_sum_gen_off, 
                           time_genoff );
  }

  /* Initialize sum_generic_offset */
  time_genoff_cb_update_sum_gen_off ( (void*)time_genoff, 0 );

  //Sanity check on offset read from EFS
  temp_offset = time_genoff->sum_generic_offset;
  if(temp_offset < 0)
     temp_offset *= -1;

    /* Reinitialize generic offsets if total time is coming out to be invalid or 
           the PMIC RTC time is read as very small value during bootup, since
           it could mean that the target was reset & PMIC RTC was also started
           from 0. This is a temporary fix until PMIC team provides a better
           alternative to check the PMIC RTC reset status from time-services.
       */
  if( (time_genoff->bases_type != ATS_RTC) &&
      ((FALSE == time_genoff_check_time(time_genoff)) || 
        (ats_bases[ATS_RTC].generic_offset < PMIC_RTC_RESET_CHECK_TIME_THRESHOLD) || 
		((temp_offset>>32) > TIME_GENOFF_OFFSET_MAX_VAL_UPPER_WORD)))
	  {
		time_genoff->generic_offset = 0;
		time_genoff->sum_generic_offset = 0;	
        time_genoff->time_genoff_last_efs_write_val = 0;
		
	    /* Re-initialize sum_generic_offset in case total time was invalid */
		    time_genoff_cb_update_sum_gen_off ( (void*)time_genoff, 0 );
		
		/* Update the global var to indicate which base got their offsets reset */
		time_genoff_efs_reset_bases |= 1<<(time_genoff->bases_type) ;

		/* Flag to update EFS file item later */
		time_genoff_base_reset_done = TRUE;

		/* Print messages for the offset reset done information */
		ATS_MSG_1("TIME_DEBUG: time_genoff_post_init: Generic offsets reset: base=0x%d",
		           time_genoff->bases_type);
	  }

  /* Update the time_genoff state */
  time_genoff->state.time_genoff_state_initialized = 1;

  /* Call the init function for this base, if any */
  if( time_genoff->init_func != NULL )
  {
    /* Call Initialization Sequence */
    time_genoff->init_func();
  }  

  /* Update the EFS if base was reset */
  if ( time_genoff_base_reset_done == TRUE )
  	{
      time_genoff_write_to_efs( time_genoff, TRUE );
  	}

  /* Release the mutex */  
  TIME_OSAL_MUTEX_UNLOCK(time_genoff->mutex);

  ATS_MSG_5("time_genoff_post_init: base=%d, generic_offset=0x%.8x%.8x sumn_generic_offset=0x%.8x%.8x", 
            time_genoff->bases_type, 
            (uint32)(time_genoff->generic_offset >> 32), 
            (uint32)(time_genoff->generic_offset & 0xFFFFFFFF),
            (uint32)(time_genoff->sum_generic_offset >> 32), 
            (uint32)(time_genoff->sum_generic_offset & 0xFFFFFFFF));
 
  {
    time_genoff_args_struct_type    temp_genoff_args ;
     
    /* Time julian Type */
    time_julian_type                jul_val;

    /* - - - - - - - - Debug: Time in julian - - - - - - - - - - - - - - */
    temp_genoff_args.base        = time_genoff->bases_type; 
    temp_genoff_args.base_source = TIME_SCLK;
    temp_genoff_args.ts_val      = (void *)(&jul_val);
    temp_genoff_args.unit        = TIME_JULIAN;
    temp_genoff_args.operation   = T_GET;
	temp_genoff_args.dal_timetick_val_valid = 0;
    time_genoff_opr( &temp_genoff_args );

    ATS_ERR_6("time_genoff_post_init: date/time %.2d/%.2d/%d %.2d:%.2d:%.2d",
				      jul_val.month,
				      jul_val.day,
				      jul_val.year,
				      jul_val.hour,
				      jul_val.minute,
				      jul_val.second );	  
  }
 
} /* time_genoff_post_init */



/*=============================================================================

FUNCTION TIME_GENOFF_IS_VALID

DESCRIPTION
  Checks if genoff is valid

DEPENDENCIES
  None

RETURN VALUE
  TRUE if valid, FALSE if not valid

SIDE EFFECTS
  None

=============================================================================*/

boolean time_genoff_is_valid
( 
  /* Pointer to Genoff in consideration */
  time_genoff_ptr time_genoff
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  return (time_genoff->valid);
} /*  time_genoff_is_valid */

/*=============================================================================

FUNCTION TIME_GENOFF_GET_OFFSET

DESCRIPTION
  Returns generic offset value of a given time base

DEPENDENCIES
  None

RETURN VALUE
  int64 value

SIDE EFFECTS
  None

=============================================================================*/
int64 time_genoff_get_offset
( 
    time_bases_type  base  
)
{
  /* Pointer to 'base' in consideration */
  time_genoff_ptr ptime_genoff;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if ( base >= ATS_MAX ) 
    return 0;
 
  /* Get pointer to base in consideration */
  ptime_genoff = time_genoff_get_pointer(base);
  return ptime_genoff->generic_offset;
} /*  time_genoff_get_offset */

/*=============================================================================

FUNCTION TIME_GENOFF_GET_SUM_GENOFF_TS

DESCRIPTION
  Returns sum generic offset value of a given time base in timestamp format

DEPENDENCIES
  None

RETURN VALUE
  boolean as return value
  Provides sum generic ts offset value as part of inputs arguments

SIDE EFFECTS
  None

=============================================================================*/
boolean time_genoff_get_sum_genoff_ts
( 
    time_bases_type  base,
    time_type        sum_genoff_ts_val
)
{
  /* Pointer to 'base' in consideration */
  time_genoff_ptr ptime_genoff;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Get pointer to base in consideration */
  ptime_genoff = time_genoff_get_pointer(base);
  
  qw_equ(sum_genoff_ts_val, ptime_genoff->sum_generic_offset_ts);
  return ptime_genoff->sum_generic_offset_ts_is_negative;
} /*  time_genoff_get_sum_genoff_ts */



/*=============================================================================

FUNCTION TIME_GENOFF_GET_GENOFF_OFFSET

DESCRIPTION
  Returns generic offset value of a given external genoff pointer.

DEPENDENCIES
  None

RETURN VALUE
  int64 value

SIDE EFFECTS
  None

=============================================================================*/
int64 time_genoff_get_genoff_offset
( 
    time_genoff_ptr  genoff_ptr  
)
{
  if ( genoff_ptr == NULL ) 
    return 0;
 
  return genoff_ptr->generic_offset;
} /*  time_genoff_get_genoff_offset */


/*=============================================================================

FUNCTION TIME_GENOFF_ADD_BASE_SUBSYS

DESCRIPTION
  Adds time_genoff1 as base_subsys for time_genoff

DEPENDENCIES
  Should be called between time_genoff_pre_init() and time_genoff_post_init() 

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_add_base_subsys 
(
  /* Pointer to Genoff in consideration */
  time_genoff_ptr time_genoff,

  /* Subsys base */
  time_bases_type base 
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Acquire the genoff mutex */
  TIME_OSAL_MUTEX_LOCK(time_genoff->mutex);

  /* Set Base Genoff */
  time_genoff->subsys_base = base;

  /* Set reqd_base_genoff to TRUE */
  time_genoff->reqd_base_genoff = TRUE;

  /* Release the mutex */
  TIME_OSAL_MUTEX_UNLOCK(time_genoff->mutex);

} /* time_genoff_add_base_subsys */



/*=============================================================================

FUNCTION TIME_GENOFF_ADD_PROXY_BASE

DESCRIPTION
  Adds base1 as proxy_subsys for base

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_add_proxy_base
(
  /* Base in consideration */
  time_bases_type base,

  /* Proxy base */
  time_bases_type base1 
)
{
  /* Pointer to Genoff in consideration */
  time_genoff_ptr ptime_genoff;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get pointer to base */
  ptime_genoff = time_genoff_get_pointer( base );

  if ( ptime_genoff != NULL )
  {
    /* Set proxy Genoff */
    ptime_genoff->proxy_base = base1;
  }

} /* time_genoff_add_base_subsys */



/*=============================================================================

FUNCTION TIME_GENOFF_ATTACH_EXT_CB

DESCRIPTION
  Attaches user callback function for an external client with the passed time base.
  The cb function will get called whenever there is a time set operation on this base.

DEPENDENCIES
  Should be called only after the base is initialized. 

RETURN VALUE
  Error return.

SIDE EFFECTS
  None

=============================================================================*/

ats_error_type time_genoff_attach_ext_cb
(
  /* Base to which callback needs to attached */
  time_bases_type  time_base,

  /* Call back Func to be attached */
  time_genoff_t2_cb_type cb
)
{
  /* Input Validation */
  if (cb == NULL)
  {
    return ATS_FAILURE;
  }

  return time_genoff_attach_cb(time_base,cb,NULL);
  
} /* time_genoff_attach_cb */



/*=============================================================================

FUNCTION TIME_GENOFF_ATTACH_CB

DESCRIPTION
  Attaches call back which are called during after set operation

DEPENDENCIES
  Should be called between time_genoff_pre_init() and time_genoff_post_init() 

RETURN VALUE
  Error return.

SIDE EFFECTS
  None

=============================================================================*/

ats_error_type time_genoff_attach_cb
(
  /* Base to which callback needs to attached */
  time_bases_type  base_cb,

  /* Call back Func to be attached */
  time_genoff_t2_cb_type cb,

  /* Base that has registered for callback */
  time_genoff_ptr  registered_genoff_ptr
)
{
  /* Pointer to Genoff in consideration */
  time_genoff_ptr time_genoff;
          
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get pointer to base_cb */
  time_genoff = time_genoff_get_pointer( base_cb );

  /* Check if the time base is in correct state for this operation */
  if (!time_genoff->state.time_genoff_state_pre_init)
  {
    return ATS_FAILURE;
  }

  /* Acquire the mutex */
  TIME_OSAL_MUTEX_LOCK(time_genoff->mutex);

  if ( time_genoff->number_of_cb_func < MAX_CB_FUNC )
  {
    /* Attach Callback */
    time_genoff->cb_func[ time_genoff->number_of_cb_func ] = cb;

    /* Keep record of registered base */
    time_genoff->registered_genoff[time_genoff->number_of_cb_func++] = registered_genoff_ptr;
  }
  else
  {
    /* Release the mutex */
    TIME_OSAL_MUTEX_UNLOCK(time_genoff->mutex);
    /* This indicated that the attachable number of callbacks are already exhausted */
    return ATS_FAILURE;
  }

  /* Release the mutex */
  TIME_OSAL_MUTEX_UNLOCK(time_genoff->mutex);

  /* Return SUCCESS for the operation */
  return ATS_SUCCESS;

} /* time_genoff_attach_cb */


/*=============================================================================

FUNCTION TIME_GENOFF_SET_REMOTE_CALL_NEEDED

DESCRIPTION
  Sets remote_call_needed to TRUE/FALSE, to update/not update genoff offset 
  in another proc. Also updates genoff in other proc

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_set_remote_call_needed
(
  /* Pointer to Genoff in consideration */
  time_genoff_ptr time_genoff,

  /* Value set to remote_call_needed */
  boolean         value
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Acquire the mutex */
  TIME_OSAL_MUTEX_LOCK(time_genoff->mutex);
  
  /* Update remote_call_needed with value */
  time_genoff->remote_call_needed = value;

  /* Release the mutex */
  TIME_OSAL_MUTEX_UNLOCK(time_genoff->mutex);

} /* time_genoff_set_remote_call_needed */



/*=============================================================================

FUNCTION TIME_GENOFF_SET_ALLOW_REMOTE_UPDATES

DESCRIPTION
  Sets allow_remote_updates to TRUE/FALSE, which is used to update 
  remote_call_needed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_set_allow_remote_updates
(
  /* Pointer to Genoff in consideration */
  time_genoff_ptr time_genoff,

  /* Value set to allow_remote_updates */
  boolean         value
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Update allow_remote_updates with value */
  time_genoff->allow_remote_updates = value;

  /* Update remote_call_needed with value */
  time_genoff_set_remote_call_needed( time_genoff, value );

} /* time_genoff_set_allow_remote_updates */



/*=============================================================================

FUNCTION TIME_GENOFF_GET_ALLOW_REMOTE_UPDATES

DESCRIPTION
  Get allow_remote_updates .

DEPENDENCIES
  None

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  None

=============================================================================*/

boolean time_genoff_get_allow_remote_updates
(
  /* Pointer to Genoff in consideration */
  time_genoff_ptr time_genoff
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Return allow_remote_updates */
  return time_genoff->allow_remote_updates ;

} /* time_genoff_get_allow_remote_updates */




/*=============================================================================

FUNCTION TIME_GENOFF_SET_REMOTE_GENOFF

DESCRIPTION
  Updates genoff in other proc

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_set_remote_genoff
(
  /* Pointer to Genoff in consideration */
  time_genoff_ptr time_genoff
)
{
  #ifdef FEATURE_QMI_TIME_REMOTE_CLNT
  int rc;
  time_set_genoff_offset_req_msg_v01  time_req;
  time_set_genoff_offset_resp_msg_v01 time_resp;
  #endif

  #ifdef FEATURE_TIME_MULTI_SERVER_PROC

  /* Information to be updated on other proc */
  ats_cfg_event_info_s_type   * cfg_event_ptr = &cfg_event_info; 

  #endif /* FEATURE_TIME_MULTI_SERVER_PROC */
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #ifndef FEATURE_MULTIPROCESSOR
  return ;
  #elif defined(FEATURE_MULTIPROCESSOR) && !defined(FEATURE_TIME_NO_RPC) 
  /* FEATURE_MULTIPROCESSOR && !FEATURE_TIME_NO_RPC */

  if ( time_genoff->remote_call_needed != TRUE )
    return;

  /* Check to control remote updates during intial updates to apps proc */
  if ( time_genoff->generic_offset == 0 )
    return;

  #ifdef FEATURE_TIME_MULTI_SERVER_PROC
  
  /* Update base type */
  cfg_event_ptr->ats_genoff_update.base_type = time_genoff->bases_type;
  
  /* Update offset for base */
  cfg_event_ptr->ats_genoff_update.offset = time_genoff->generic_offset;
  
  /* Notify and call remote call */
  ats_cfg_event_notify( ATS_CFG_EVENT_GENOFF_CHANGE, &cfg_event_info );

  #else /* FEATURE_TIME_MULTI_SERVER_PROC */
 
  time_remote_atom_genoff_set_generic_offset( time_genoff->bases_type, time_genoff->generic_offset );
  
  #endif /* FEATURE_TIME_MULTI_SERVER_PROC */
  #elif defined FEATURE_QMI_TIME_REMOTE_CLNT  
    /*decide on message id*/
    ATS_MSG_3("Setting time on modem..base=0x%x,generic_offset=0x%.8x%.8x",
            time_genoff->bases_type, 
            (uint32)(time_genoff->generic_offset >> 32),
            (uint32)(time_genoff->generic_offset & 0xFFFFFFFF));

    time_req.base = (time_service_base_enum_v01)time_genoff->bases_type;
    time_req.generic_offset = time_genoff->generic_offset;

    /*send the data over to modem*/
    rc = qmi_client_send_msg_sync(time_qmi_client_handle,
                                   QMI_TIME_GENOFF_SET_REQ_V01,
                                   &time_req,
                                   sizeof(time_req),
                                   &time_resp,
                                   sizeof(time_resp),
                                   1000);
    if(rc != QMI_NO_ERR) 
    {
      ATS_ERR("Could not set time on modem");
    }
  #endif /* FEATURE_MULTIPROCESSOR */

} /* time_genoff_set_remote_genoff */



/*=============================================================================

FUNCTION TIME_GENOFF_REMOTE_THROUGH_TIME_IPC

DESCRIPTION
  Called from time_ipc to update genoff to another proc

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_remote_through_time_ipc(void)
{
  
#if (defined FEATURE_TIME_MULTI_SERVER_PROC || defined FEATURE_QMI_TIME_REMOTE)

  /* Pointer to Genoff in consideration */
  time_genoff_ptr             ptime_genoff;

  rpc_struct                  *rpc_base_ptr;
      
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get base */
  rpc_base_ptr = ( rpc_struct * ) q_get( &rpc_q );
    
  while ( rpc_base_ptr != NULL )
  {
    /* Mark entry as not in queue */
    rpc_base_ptr->queue = NOT_IN_QUEUE;
        
    /* Get pointer to genoff */
    ptime_genoff= time_genoff_get_pointer( rpc_base_ptr->base );

    #ifdef FEATURE_QMI_TIME_REMOTE
    /* Initialize Function Pointers to eliminate dependencies */
    if ( !time_genoff_fps_initialized )
    {
      time_genoff_fps = time_fn_map_get_fp();
      time_genoff_fps_initialized++;
    }
    
    /* Send QMI notification if QMI is available */
    if ( time_genoff_fps->time_server_call_ind_fp != NULL )
    {
      ATS_MSG_HIGH("Calling time_server_call_ind: base=0x%x,generic_offset=0x%.8x%.8x",
                ptime_genoff->bases_type,
                (uint32)(ptime_genoff->generic_offset >> 32),
                (uint32)(ptime_genoff->generic_offset & 0xFFFFFFFF));
      time_genoff_fps->time_server_call_ind_fp(ptime_genoff->bases_type,
                                              ptime_genoff->generic_offset);
    }
    #else
    /* Set remote updates */
    time_genoff_set_remote_genoff( ptime_genoff );
    #endif

    /* Get base */
    rpc_base_ptr = ( rpc_struct * ) q_get( &rpc_q );

  } /* while ( rpc_base_ptr != NULL ) */

#endif /* FEATURE_TIME_MULTI_SERVER_PROC */

} /* time_genoff_remote_through_time_ipc */




/*=============================================================================

FUNCTION TIME_GENOFF_ADD_INIT_CALL

DESCRIPTION
  Adds initialization sequence for the genoff in consideration

DEPENDENCIES
  Should be called between time_genoff_pre_init() and time_genoff_post_init()

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_add_init_call
(
  /* Pointer to Genoff in consideration */
  time_genoff_ptr time_genoff,

  /* Call to be added */
  time_genoff_t1_cb_type func
)
{
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Update initialization sequence */
  time_genoff->init_func = func;

} /* time_genoff_add_init_call */



/*=============================================================================

FUNCTION TIME_GENOFF_UPDATE_PER_STORAGE

DESCRIPTION
  Updates persistent storage info 

DEPENDENCIES
  Should be called between time_genoff_pre_init() and time_genoff_post_init()

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_updates_per_storage
(
  /* Pointer to Genoff in consideration */
  time_genoff_ptr time_genoff,

  /* File name for efs item file */
  char            *name,

  /* Threshold in ms for writing back in efs item file */
  int64           thold
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!time_genoff->state.time_genoff_state_pre_init)
  {
    ERR_FATAL("time_genoff_updates_per_storage: PRE_INIT not complete",0,0,0);
	return;
  }

  /* Acquire the mutex */
  TIME_OSAL_MUTEX_LOCK(time_genoff->mutex);

  /* Initialze storage specs */
  time_genoff->per_storage_spec.initialized = TRUE;
  
  /* Update threshhold */
  time_genoff->per_storage_spec.threshold   = thold;
  
  /* Copy the name of efs item file */
  strlcpy( (time_genoff->per_storage_spec.f_name), name, (TIME_GENOFF_MAX_FILE_NAME_LENGTH) );

  /* Release the mutex */  
  TIME_OSAL_MUTEX_UNLOCK(time_genoff->mutex);

} /* time_genoff_updates_per_storage */

/*=============================================================================

FUNCTION TIME_GENOFF_ENCODE_INDEX

DESCRIPTION
  Encodes the passed index value.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
  
=============================================================================*/
static inline time_bases_type time_genoff_encode_private_base
(
  uint32           base
)
{
   /* Input validation */
   if (base >= ATS_PRIVATE_MAX)
   	{
   	  return ATS_INVALID;
   	}
   

   /* Encode this private base index value with a constant & shift left */
   base = ( (base ^ TIME_GENOFF_ENCODE_CONST) << 8 ) | (base);

   /* Add ATS_PRIVATE enum to this index value to
         indicate that this is a external private base. */
   base += ATS_PRIVATE;

   return base;
}



/*=============================================================================

FUNCTION TIME_GENOFF_ENCODE_INDEX

DESCRIPTION
  Encodes the passed index value.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
  
=============================================================================*/
static inline boolean time_genoff_decode_private_base
(
  uint32            base,
  uint32          * ret_val
)
{
   uint32 temp1 =0;
   uint32 temp2 =0;

   /* Subtract the private base offset from this decoded 
		value to get the index for ats_private_bases array. */ 
   base -= ATS_PRIVATE;

   /* Get the decode values for checking sanity */
   temp1 = ( (base >> 8) ^ TIME_GENOFF_ENCODE_CONST );
   temp2 = (base & 0xFF);

   /* Check if the value passed is proper or not */
   if (temp1 != temp2)
   {
     /* Return FALSE */
     return FALSE;
   }
   else
   {
    *ret_val = temp2;
     return TRUE;
   }
}


/*=============================================================================

FUNCTION TIME_GENOFF_GET_POINTER                                 

DESCRIPTION
  Returns the Pointer to different time bases 

DEPENDENCIES
  None

RETURN VALUE
  The pointer to time_genoff_struct_type

SIDE EFFECTS
  None

=============================================================================*/

time_genoff_ptr time_genoff_get_pointer
( 
  /* Time bases whose pointer needs to be returned */
  time_bases_type               base
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  time_genoff_ptr  temp_base_ptr = NULL;
  uint32           temp_base = 0;

  /* Return NULL in case the base is a private base */
  if ((uint32)base < ATS_MAX)
  {
    return &(ats_bases[base]);
  }
  else if (base > ATS_PRIVATE)
  {
    if (FALSE == time_genoff_decode_private_base(base,&temp_base))
    {
      ERR_FATAL("Base=%d is incorrect",base,0,0);
	  return NULL;
    }
    else
    {
      if (temp_base < ATS_PRIVATE_MAX)
      {
        temp_base_ptr = ats_private_bases[temp_base];

        /* ERR FATAL if the base is not proper for external genoff pointer */
        if (temp_base_ptr)
        {
          return temp_base_ptr;
        }
        else
        {
          ERR_FATAL("Private base=%d is incorrect",base,0,0);
		  return NULL;
        }
      }
	  return NULL;
    }
  }
  else
  {
    ERR_FATAL("base=%d is incorrect",base,0,0);
	return NULL;
  }
     
} /* time_genoff_get_pointer */



/*=============================================================================

FUNCTION TIME_GENOFF_SET_GENERIC_OFFSET                               

DESCRIPTION
  Sets Generic offset.

DEPENDENCIES
  Should be used only by remote calls

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_set_generic_offset
(
  /* Pointer to Genoff in consideration */
  time_genoff_ptr ptime_genoff,

  /* New Generic Offset to be set */
  int64           offset
)
{
  /* Backup for original Generic Offset values */  
  time_genoff_ptr           proxy_genoff_ptr;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Return if the genoff is uninitialized */
  if (!ptime_genoff->state.time_genoff_state_initialized)
  {
    return;
  }
  
  time_genoff_set_helper( ptime_genoff, 
                          offset,
                          ATS_INVALID );

  while ( ptime_genoff->proxy_base != ATS_INVALID )
  {
     /* Get pointer to genoff */
     proxy_genoff_ptr = time_genoff_get_pointer( ptime_genoff->proxy_base );
	 time_genoff_set_helper( proxy_genoff_ptr , 
	 	                     ptime_genoff->generic_offset,
	 	                     ptime_genoff->bases_type );

     /* Update time_genoff pointer to point to proxy base for while loop operation */
	 ptime_genoff = proxy_genoff_ptr;
  }

} /* time_genoff_set_generic_offset */



/*=============================================================================

FUNCTION TIME_GENOFF_GET_TS_FROM_OFFSET_AND_TIMETICK

DESCRIPTION
  Calculates timestamp from offset and timetick that are passed as inptu args.

DEPENDENCIES
  None

RETURN VALUE
  time in timestamp format

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_get_ts_from_offset_and_timetick
(
  /* Generic offset in timestamp format */
  time_type           sum_genoff_ts,
  /* Sign of offset. FALSE if +ve, True if -ve */
  boolean             is_offset_negative,
  /* timetick at which time is required */
  uint64              timetick,
  /* Calculated timestamp is returned */
  time_type           ts_val  
)
{

   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
   qw_set( ts_val, 0, 0 );
  
   /* If offset is not negative, directly use it for
      addition of timetick by copying offset to ts_val */
   if(is_offset_negative == FALSE)
      qw_equ(ts_val, sum_genoff_ts);

   /* Advance the output timestamp by the elapsed slow clocks */
   time_sclk_get(ts_val, timetick);

   /* add / sub sum_generic_offset_ts from uptime based
	  on the state of sum_generic_offset_ts_is_negative */
   if(is_offset_negative == TRUE)
   {
      time_conv_ts_sub(ts_val, sum_genoff_ts);
   }

   return;
} /* time_genoff_get_ts_from_offset_and_timetick */

/*=============================================================================

FUNCTION TIME_GENOFF_GET_OPTIMIZED_TS

DESCRIPTION
  Gets Genoff Time for the time base in consideration in 64 bit Timestamp format, only if time_genoff is 
  initialized 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/


void time_genoff_get_optimized_ts
(
  /* Pointer to time_genoff_args_type with genoff_ptr in consideration */
  time_genoff_args_ptr         pargs
)
{
  /* Pointer to genoff in consideration */
  time_genoff_ptr              ptime_genoff;
   
  /*to store the generic offset in a temp variable*/
  uint32     old_generic_offset=0;  
  int64      temp_generic_offset=0; 
  int64      temp_sum_generic_offset = 0;

  /*temporary timestamp value*/
  time_type                    ts_val_temp;
  
  uint64     timetick;

  /* This variable is used to erase TIME_GENOFF_TIMETICK_VALID from getting onto stack */
  volatile uint32 timetick_valid_flag = TIME_GENOFF_TIMETICK_VALID;  

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  qw_set( ts_val_temp, 0, 0 );
  
  /* Get pointer to genoff */
    ptime_genoff = time_genoff_get_pointer( pargs->base );
  
  /* If ptime_genoff is not initilazed */
  if (!ptime_genoff->state.time_genoff_state_initialized)
  {
    qw_set( pargs->ts_val, 0, 0 );
    return;
  }

  do
  {    

	/*record the generic offset before we begin*/
	old_generic_offset = ptime_genoff->sum_generic_offset>>32;
    
    //Check with max value of the offset
    temp_sum_generic_offset = ptime_genoff->sum_generic_offset;
	temp_generic_offset = ptime_genoff->generic_offset;

    if(temp_sum_generic_offset < 0)
      temp_sum_generic_offset *= -1;

    if(temp_generic_offset < 0)
      temp_generic_offset *= -1;
  
    if ( ((temp_sum_generic_offset >> 32) > TIME_GENOFF_OFFSET_MAX_VAL_UPPER_WORD) ||
		 ((temp_generic_offset     >> 32) > TIME_GENOFF_OFFSET_MAX_VAL_UPPER_WORD) )
	   ERR_FATAL("Time Genoff value is corrupted: 0x%.8x%.8x, base: %d", 
	                                             temp_sum_generic_offset>>32, 
	                                             temp_sum_generic_offset, 
                                                 ptime_genoff->bases_type);
	


	/* Get time from sclk */
	if ( pargs->dal_timetick_val_valid != timetick_valid_flag )
	{
	  /* Erase TIME_GENOFF_TIMETICK_VALID from getting onto stack.
         This will make sure there will not be any issues if clients
		 doesnt explicitly set dal_timetick_val_valid to 0 if they dont use it */
	  timetick_valid_flag = 0;

	  timetick = (uint64)-1;
	}
	else
	{
      timetick_valid_flag = 0;	
	  timetick = pargs->dal_timetick_val;
	}

	time_genoff_get_ts_from_offset_and_timetick(ptime_genoff->sum_generic_offset_ts,
	                                            ptime_genoff->sum_generic_offset_ts_is_negative,
												timetick,
												ts_val_temp);

  }while( FALSE == osal_atomic_compare_and_set(
                                  (osal_atomic_word_t *)&old_generic_offset,
                                  (ptime_genoff->sum_generic_offset)>>32,
                                  (old_generic_offset)) );

     qw_equ(pargs->ts_val, ts_val_temp);
     
} /* time_genoff_get_optimized_ts */


#if 0 /* These functions are not needed, but keeping for idea */
/*=============================================================================

FUNCTION TIME_GENOFF_GET_OPTIMIZED

DESCRIPTION
  Gets Genoff Time for the time base in consideration, only if time_genoff is 
  initialized

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void time_genoff_get_optimized
(
  /* Pointer to time_genoff_args_type with genoff_ptr in consideration */
  time_genoff_args_ptr         pargs
)
{

  /* Stores time */
  uint64                       temp_time;
  
  /* Pointer to genoff in consideration */
  time_genoff_ptr              ptime_genoff;
   
  /*to store the generic offset in a temp variable*/
  uint32     old_generic_offset = 0;

  /*temporary timestamp value*/
  time_type                    ts_val_temp; 

  /* This variable is used to erase TIME_GENOFF_TIMETICK_VALID from getting onto stack */
  volatile uint32 timetick_valid_flag = TIME_GENOFF_TIMETICK_VALID;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  qw_set( ts_val_temp, 0, 0 );
  
  /* Get pointer to genoff */
    ptime_genoff = time_genoff_get_pointer( pargs->base );
  
  /* If ptime_genoff is not initilazed */
  if (!ptime_genoff->state.time_genoff_state_initialized)	
  {
    qw_set( pargs->ts_val, 0, 0 );
    return;
  }

  do
  {    
	/*initialize the variable to zero. This is important
	 since if do-while loop iterates more than once, we 
	 need to start with a zero*/
	temp_time = 0;
  
	/*record the generic offset before we begin*/
	old_generic_offset = ptime_genoff->sum_generic_offset>>32;
  
	if ( pargs->dal_timetick_val_valid != timetick_valid_flag)
	{
	  /* Erase TIME_GENOFF_TIMETICK_VALID from getting onto stack.
         This will make sure there will not be any issues if clients
		 doesnt explicitly set dal_timetick_val_valid to 0 if they dont use it */
	  timetick_valid_flag = 0;
	  /* Get time from sclk */
	  time_sclk_get(ts_val_temp, (uint64)-1);
	}
	else
	{
      timetick_valid_flag = 0;
	  time_sclk_get(ts_val_temp, pargs->dal_timetick_val);
	}
  
	/*convert it to msec */
	time_to_ms(ts_val_temp);
  
	/*copy it over in temp_time*/
	qw_equ_misaligned((void*)&temp_time, ts_val_temp);
  
	/* Copy the ts_val before converting to timestamp */
	temp_time = temp_time + ptime_genoff->sum_generic_offset ;	  
  }while( FALSE == osal_atomic_compare_and_set(
                                  (osal_atomic_word_t *)&old_generic_offset,
                                  (ptime_genoff->sum_generic_offset)>>32,
                                  (old_generic_offset)) );

  qw_equ_misaligned( (void*)pargs->ts_val, (void*)&temp_time );   
  time_conv_ms_to_timestamp(pargs->ts_val);
  
} /* time_genoff_get_optimized */


/*=============================================================================

FUNCTION TIME_GENOFF_GET

DESCRIPTION
  Gets Genoff Time for the time base in consideration, only if time_genoff is 
  initialized

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void time_genoff_get
(
  /* Pointer to time_genoff_args_type with genoff_ptr in consideration */
  time_genoff_args_ptr         pargs
)
{
  /* Temporary genoff argument to get time from base genoff*/
  time_genoff_args_struct_type base_genoff_args;

  /* Stores uptime */
  uint64                       uptime;
  
  /* Pointer to genoff in consideration */
  time_genoff_ptr              ptime_genoff;
  
  /* This variable is used to erase TIME_GENOFF_TIMETICK_VALID from getting onto stack */
  volatile uint32 timetick_valid_flag = TIME_GENOFF_TIMETICK_VALID;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Get pointer to genoff */
    ptime_genoff = time_genoff_get_pointer( pargs->base );
  
  /* If ptime_genoff is not initilazed */
  if (!ptime_genoff->state.time_genoff_state_initialized)
  {
    qw_set( pargs->ts_val, 0, 0 );
    return;
  }
  
  if ( ptime_genoff->reqd_base_genoff == FALSE )
  {
    if ( pargs->dal_timetick_val_valid != timetick_valid_flag )
    {
   /* Erase TIME_GENOFF_TIMETICK_VALID from getting onto stack.
      This will make sure there will not be any issues if clients
	  doesnt explicitly set dal_timetick_val_valid to 0 if they dont use it */
	  timetick_valid_flag = 0;
	  
      /* Get stime from Timetick64 */
      if ( pargs->base_source == TIMETICK_SCLK64 )  
      {
        uptime = timetick_sclk64_get();

        /* Reset the ts_val before converting to timestamp */
        memset(pargs->ts_val, 0, sizeof(time_type));

        timetick_sclk64_add_xo( pargs->ts_val, uptime );
      }
      /* Get time from time_sclk  */
      else if ( pargs->base_source == TIME_SCLK )  
      {
        /*  Get Time from sclk */
        time_sclk_get( pargs->ts_val, (uint64)-1 );
      }
    }
    else
    {
	  timetick_valid_flag = 0;
	  
      timetick_sclk64_add_xo(pargs->ts_val, pargs->dal_timetick_val); 
    }
  }
  else
  {
    /* Update base_genoff to retrieve ts_val */
    base_genoff_args.base        = ptime_genoff->subsys_base ;
    base_genoff_args.base_source = pargs->base_source;
    base_genoff_args.ts_val      = pargs->ts_val;
      
    /* Get Time from base Genoff */
    time_genoff_get( &base_genoff_args );
   }

  /* Add offset to timestamp */
  time_conv_add_offset_to_timestamp( pargs->ts_val, ptime_genoff->generic_offset );
  
} /* time_genoff_get */
#endif /* #if 0 */


/*=============================================================================

FUNCTION TIME_GENOFF_WRITE_TO_EFS

DESCRIPTION
  Checks and calls function to write the offset to efs.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void time_genoff_write_to_efs
(
  /* Pointer to genoff in consideration */
  time_genoff_ptr         ptime_genoff,

  /* option to force efs write operation */
  boolean                 force
)
{
  /* Delta between Old and New efs write valie , in ms */
  int64                   delta_ms ;
  
  #ifdef FEATURE_TIME_SINGLE_MULTI_SERVER_PROC
  
  /* If persistent storage spec is updated and delta_ms is greater 
     then threshold */
  if ( ptime_genoff->per_storage_spec.initialized == TRUE )	 
  {
    /* Get the difference between the last efs write value & 
	   the current value which needs to be written to efs  */
	delta_ms = ptime_genoff->generic_offset - ptime_genoff->time_genoff_last_efs_write_val ;

    /* Write to efs only if the last update was before the given threshold value */ 
	if ( ( force == TRUE ) 
	     || ( ( delta_ms > ptime_genoff->per_storage_spec.threshold )  
		 ||   ( delta_ms < ((-1)*ptime_genoff->per_storage_spec.threshold)) )
	   )
	   {
      /* Write generic offset into efs item file */
      if ( TIME_EFS_RW_STAT_OK !=
           time_efs_rw( ptime_genoff->per_storage_spec.f_name,
                        &( ptime_genoff->generic_offset ),
                       TIME_EFS_RW_CMD_WR ))
        {
         ATS_ERR_3("time_genoff_write_to_efs : Error writing into EFS file bases_type=0x%x  delta_ms=0x%.8x%.8x",
                    ptime_genoff->bases_type,
                    (uint32)(delta_ms>>32),
                    (uint32)(delta_ms & 0xFFFFFFFF));
        }
	  else
		{
		/* Update the last written value in the efs file item */
		ptime_genoff->time_genoff_last_efs_write_val = ptime_genoff->generic_offset ;
		}
	  }
  }

  #endif /* FEATURE_TIME_SINGLE_MULTI_SERVER_PROC */
} /* time_genoff_write_to_efs */


/*=============================================================================

FUNCTION TIME_GENOFF_SHUTDOWN_NOTIFICATION_FN

DESCRIPTION
  Write the final updates to efs file item on shutdown event.
  Also Deletes the file items for all the private bases.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_shutdown_notification_fn( void )
{
  /* index for iterating all the ATS bases */
  uint32 				  index;
  
  /* Generic offset pointer */
  time_genoff_ptr         ptime_genoff;

/* keep only for debugging purpose */
  ATS_MSG("time_genoff_shutdown_notification_fn: Got the shutdown notification event");
  
/* Update the efs file entries for generic offsets for all the bases */
for (index = 0 ; index < ATS_MAX  ; index++)
	{
	  /* Get the pointer to time_base instance */
      ptime_genoff = time_genoff_get_pointer( (time_bases_type)index );
	  if ( ptime_genoff->time_genoff_last_efs_write_val != ptime_genoff->generic_offset )
	  {
	    ATS_MSG_5("time_genoff_shutdown_notification_fn: Writing : base = 0x%x ; efs_val = 0x%.8x%.8x ; genoff = 0x%.8x%.8x",
				  ptime_genoff->bases_type,
				  (uint32)(ptime_genoff->time_genoff_last_efs_write_val >> 32),
				  (uint32)(ptime_genoff->time_genoff_last_efs_write_val & 0xFFFFFFFF),
				  (uint32)(ptime_genoff->generic_offset >> 32),
				  (uint32)(ptime_genoff->generic_offset & 0xFFFFFFFF));

      /* Write the offset value to EFS */
	    time_genoff_write_to_efs( ptime_genoff, TRUE );
	  }
	}

  /* Update the efs file entries for Private offsets as well */
  for (index = 0 ; index < ATS_PRIVATE_MAX  ; index++)
  {
    /* Get the pointer to time_base instance */
    ptime_genoff = ats_private_bases[index];
    if ( (ptime_genoff!= NULL) && 
		 (ptime_genoff->time_genoff_last_efs_write_val != ptime_genoff->generic_offset) )
    {
      ATS_MSG_5("time_genoff_shutdown_notification_fn: Writing : Private base = 0x%x ; efs_val = 0x%.8x%.8x ; genoff = 0x%.8x%.8x",
                 index,
                 (uint32)(ptime_genoff->time_genoff_last_efs_write_val >> 32),
                 (uint32)(ptime_genoff->time_genoff_last_efs_write_val & 0xFFFFFFFF),
                 (uint32)(ptime_genoff->generic_offset >> 32),
                 (uint32)(ptime_genoff->generic_offset & 0xFFFFFFFF));

      /* Write the offset value to EFS */
	    time_genoff_write_to_efs( ptime_genoff, TRUE );
	  }
	}

} /* time_genoff_shutdown_notification_fn */


/*=============================================================================

FUNCTION TIME_GENOFF_REMOTE_OFFSET

DESCRIPTION
  Checks and calls function to remote offset

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void time_genoff_remote_offset
(
  /* Pointer to genoff in consideration */
  time_genoff_ptr         ptime_genoff,

  /* Delta between Old and New Genoff time, in ms */
  int64                   delta_ms
)
{
  if ((delta_ms > TIME_GENOFF_REMOTE_UPDATE_MS) || 
      (delta_ms < ((-1)*TIME_GENOFF_REMOTE_UPDATE_MS)) )
  {
    #if(defined FEATURE_TIME_MULTI_SERVER_PROC || defined FEATURE_QMI_TIME_REMOTE)

    /* Allocate base */
    rpc_base[ptime_genoff->bases_type].base =  ptime_genoff->bases_type;
    
    if ( rpc_base[ptime_genoff->bases_type].queue == NOT_IN_QUEUE )
    {
      /* Mark entry as in queue */
      rpc_base[ptime_genoff->bases_type].queue = IN_QUEUE;

      /* Link roc_data to rpc_q */
      (void) q_link( &rpc_base[ptime_genoff->bases_type], 
             &rpc_base[ptime_genoff->bases_type].link );
    
      /* Enqueue node in rpc_q */
      q_put( &rpc_q, &rpc_base[ptime_genoff->bases_type].link);
         
      /* Set signal to TIME_IPC task for further processing */
      if ( time_ipc_tcb_genoff_ptr != NULL )
      {
        (void) rex_set_sigs( time_ipc_tcb_genoff_ptr, TIME_IPC_GENOFF_UPDATE );
      }
      else
      {
        time_ipc_tcb_genoff_ptr = rcinit_lookup_rextask("time_ipc");
        (void) rex_set_sigs( time_ipc_tcb_genoff_ptr, TIME_IPC_GENOFF_UPDATE );
      }

    }

    /*update the remote modem processor if we have to*/
    /*if it is a set operation, and if the base was TOD, then call
     the below function to send over the time to the server modem*/
    #if(defined FEATURE_EXPORT_TIME_REMOTE_MM_FUSION || defined FEATURE_EXPORT_TIME_REMOTE_MM_FUSION_CLNT)
    if(ATS_TOD ==  ptime_genoff->bases_type && time_genoff_get_remote_modem_update()) 
    {
      ATS_MSG("mm_debug_fusion:Queuing up request for remote modem update");
      if ( time_ipc_tcb_genoff_ptr != NULL )
      {
        (void) rex_set_sigs(time_ipc_tcb_genoff_ptr, TIME_IPC_SEND_REMOTE_MODEM_UDPATE);
      }
      else
      {
        time_ipc_tcb_genoff_ptr = rcinit_lookup_rextask("time_ipc");
        (void) rex_set_sigs( time_ipc_tcb_genoff_ptr, TIME_IPC_GENOFF_UPDATE );
      }
    } /* End if(ATS_TOD ==  ptime_genoff->bases_type ) */

    if(ATS_USER ==  ptime_genoff->bases_type && time_genoff_get_remote_modem_update()) 
    {
      ATS_MSG("mm_debug_fusion:Queuing up request for remote modem update");
      if ( time_ipc_tcb_genoff_ptr != NULL )
      {
        (void) rex_set_sigs(time_ipc_tcb_genoff_ptr, TIME_IPC_SEND_REMOTE_USER_UDPATE);
      }
      else
      {
        time_ipc_tcb_genoff_ptr = rcinit_lookup_rextask("time_ipc");
        (void) rex_set_sigs( time_ipc_tcb_genoff_ptr, TIME_IPC_GENOFF_UPDATE );
      }
    } /* End if(ATS_USER ==  ptime_genoff->bases_type ) */

    if(ATS_USER_TZ_DL ==  ptime_genoff->bases_type && time_genoff_get_remote_modem_update()) 
    {
      ATS_MSG("mm_debug_fusion:Queuing up request for remote modem update");
      if ( time_ipc_tcb_genoff_ptr != NULL )
      {
        (void) rex_set_sigs(time_ipc_tcb_genoff_ptr, TIME_IPC_SEND_REMOTE_USER_TZ_DL_UDPATE);
      }
      else
      {
        time_ipc_tcb_genoff_ptr = rcinit_lookup_rextask("time_ipc");
        (void) rex_set_sigs( time_ipc_tcb_genoff_ptr, TIME_IPC_GENOFF_UPDATE );
      }
    } /* End if(ATS_USER_TZ_DL ==  ptime_genoff->bases_type ) */


    #endif

   #elif (defined IMAGE_APPS_PROC || (defined T_MDM9X00 && !defined FEATURE_TIME_QDSP6)|| defined FEATURE_QMI_TIME_REMOTE_CLNT)
   
   /* Update Remote Generic offset */
   time_genoff_set_remote_genoff( ptime_genoff );
   
   #endif /* IMAGE_APPS_PROC */
    
  }
} /* time_genoff_remote_offset */

#ifdef FEATURE_TIME_DEBUG
/*=============================================================================

FUNCTION TIME_GENOFF_SEND_TOD_ERROR_NOTIFICATION

DESCRIPTION
  Sends F3 message and log packet whenever the delta in the last TOD
  vs. the TOD being set is greater than the threshold.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
static void time_genoff_send_tod_error_notification
(
  uint64 old_ms,
  uint64 new_ms,
  int64  delta_ms
)
{
  uint8 *log_ptr;

    /*populate the log params here*/
    time_delta_log.u_Version=(uint8)0;
    time_delta_log.old_ms = old_ms;
    time_delta_log.new_ms = new_ms;
    time_delta_log.delta = delta_ms;
    time_delta_log.rex_tcb = (uint32)rex_self();
    rex_get_task_name(rex_self(),&time_delta_log.tcb_string[0],20,NULL);
    log_ptr = (uint8 *) log_alloc (LOG_TIME_TOD_ERROR_NOTIFY_C,
          sizeof(time_delta_report_log_type) + sizeof(log_hdr_type));

    /*Print the F3 error message, we can use timetick_get() since this wont be called often */
    ATS_MSG_2("TOD: Delta from last TOD set = 0x%.8x%.8x",
              (uint32)(delta_ms>>32),
              (uint32)(delta_ms & 0xFFFFFFFF));

    if (log_ptr != NULL) 
    {
        memscpy((void*)(log_ptr + sizeof(log_hdr_type)), 
                sizeof(time_delta_report_log_type), 
                (void*)&(time_delta_log),  
                sizeof(time_delta_report_log_type));
        log_commit(log_ptr);
    }
} /* time_genoff_send_tod_error_notification */
#endif /* FEATURE_TIME_DEBUG */


/*=============================================================================

FUNCTION TIME_GENOFF_SET

DESCRIPTION
  Sets Genoff Time for the time base in consideration, only if ptime_genoff is 
  initialized

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void time_genoff_set
(
  /* Pointer to time_genoff_args_type with genoff_ptr in consideration */
  time_genoff_args_ptr         pargs
)
{
  /* Old Genoff time, in ms */
  time_type       old_ts_val_ms;

  /* Arguments to be passed to base_genoff */
  time_genoff_args_struct_type temp_genoff_args;

  /* Operation needed to get the current time based on external or internal */
  time_genoff_opr_type         temp_opr;

  /* Pointer to genoff in consideration */
  time_genoff_ptr              ptime_genoff;

  /* Delta between Old and New Genoff time, in ms */
  int64           delta_ms = 0;
  int64           new_ms = 0;
  int64           old_ms = 0;  
  int64           temp_generic_offset = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
  /* Get pointer to genoff */
    ptime_genoff = time_genoff_get_pointer( pargs->base );
    temp_opr = T_GET;
  
  if (!ptime_genoff->state.time_genoff_state_initialized)

  {
    return;
  }

  //Check with max value of the offset
  temp_generic_offset = ptime_genoff->sum_generic_offset;

  if(temp_generic_offset < 0)
    temp_generic_offset *= -1;

  /* Reset the time base offsets to zero in case
        the upper word of the genoff base is corrupted. */
  if((temp_generic_offset >> 32) > TIME_GENOFF_OFFSET_MAX_VAL_UPPER_WORD)
  {
      TIME_OSAL_MUTEX_LOCK(ptime_genoff->mutex);
	  
	  ptime_genoff->generic_offset = 0 ;
	  ptime_genoff->sum_generic_offset = 0 ;

	  /* Re-initialize sum_generic_offset in case total time was invalid */
      time_genoff_cb_update_sum_gen_off ( (void*)ptime_genoff, 0 );
 
	  /* Update the global var to indicate which base got their offsets reset */
	  if (ptime_genoff->bases_type != ATS_PRIVATE)
	    time_genoff_reset_bases |= 1<<(ptime_genoff->bases_type) ;
	  
	  TIME_OSAL_MUTEX_UNLOCK(ptime_genoff->mutex);

	  ATS_ERR_3("TIME_DEBUG: time base reset: base=0x%x reset_offset_val=0x%.8x%.8x",
             ptime_genoff->bases_type,
             (uint32)(temp_generic_offset>>32),
             (uint32)(temp_generic_offset&0xFFFFFFFF));

  }  


  /* Get Current Time in Genoff in msecs */
  temp_genoff_args.base                     = pargs->base;
  temp_genoff_args.base_source              = pargs->base_source;
  temp_genoff_args.ts_val                   = old_ts_val_ms;
  temp_genoff_args.unit                     = TIME_MSEC;
  temp_genoff_args.operation                = temp_opr;
  temp_genoff_args.dal_timetick_val         = pargs->dal_timetick_val;
  temp_genoff_args.dal_timetick_val_valid   = pargs->dal_timetick_val_valid;
  time_genoff_opr( &temp_genoff_args );

  qw_equ_misaligned((void*)&new_ms,pargs->ts_val);
  qw_equ_misaligned((void*)&old_ms,old_ts_val_ms);
  delta_ms = new_ms - old_ms;

  ATS_MSG_5("time_genoff_set: base = 0x%x, curr_ms=0x%.8x%.8x, new_ms=0x%.8x%.8x",
             ptime_genoff->bases_type,
             (uint32)(old_ms>>32),
             (uint32)(old_ms&0xFFFFFFFF),
             (uint32)(new_ms>>32),
             (uint32)(new_ms&0xFFFFFFFF));

  /* Continue setting in time `*/
  time_genoff_set_generic_offset(ptime_genoff,
                                 ptime_genoff->generic_offset + delta_ms);

} /* time_genoff_set */

#ifdef FEATURE_TIME_DEBUG
/*=============================================================================

FUNCTION TIME_GENOFF_SET_LOGGING

DESCRIPTION
  Updates debug logs during calls to time_genoff_set_helper()
  and time_genoff_set_generic_offet()

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_genoff_set_logging
(
  /* Pointer to genoff in consideration */
  time_genoff_ptr      ptime_genoff,

  /* Old offset value */
  int64                old_ms, 

  /* parent base for proxy bases */
  time_bases_type      parent_base
)
{
/* Delta b/w new & old generic offsets */
int64                 new_ms = 0,delta_ms = 0;

new_ms = ptime_genoff->generic_offset;

delta_ms = new_ms - old_ms ;

if ((delta_ms > TIME_GENOFF_REMOTE_UPDATE_MS) || 
    (delta_ms < ((-1)*TIME_GENOFF_REMOTE_UPDATE_MS)) )
	{
	if (ptime_genoff->bases_type == ATS_TOD)
		{
		 time_genoff_send_tod_error_notification(old_ms,new_ms,delta_ms);
		}

		time_genoff_set_log[time_genoff_set_log_cntr].base = ptime_genoff->bases_type;
		time_genoff_set_log[time_genoff_set_log_cntr].parent_base = parent_base;
		time_genoff_set_log[time_genoff_set_log_cntr].proxy_base=ptime_genoff->proxy_base;
		time_genoff_set_log[time_genoff_set_log_cntr].new_offset=new_ms;
		time_genoff_set_log[time_genoff_set_log_cntr].delta_ms = delta_ms;
		time_genoff_set_log[time_genoff_set_log_cntr].old_tod_offset = ats_bases[ATS_TOD].generic_offset;
		time_genoff_set_log[time_genoff_set_log_cntr].caller_tcb = rex_self();
		time_genoff_set_log[time_genoff_set_log_cntr].timetick_now = timetick_sclk64_get();
		rex_get_task_name(rex_self(), &time_genoff_set_log[time_genoff_set_log_cntr].tcb_string[0], 20, NULL);

	if (parent_base == ATS_INVALID)
		{
		  ATS_MSG_5("TIME_DEBUG: Time set on Base = 0x%x, delta_ms=0x%.8x%.8x, new_offset_ms=0x%.8x%.8x",
  		   			time_genoff_set_log[time_genoff_set_log_cntr].base,
  		 			(uint32)(delta_ms>>32),
  		 			(uint32)(delta_ms&0xFFFFFFFF),
                    (uint32)(new_ms>>32),
                    (uint32)(new_ms));
          ATS_MSG_4("TIME_DEBUG: sum_offset_ms: 0x%.8x%.8x sum_offset_ts: 0x%.8x%.8x",
                    ptime_genoff->sum_generic_offset>>32, 
                    ptime_genoff->sum_generic_offset,
                    ptime_genoff->sum_generic_offset_ts[1],
                    ptime_genoff->sum_generic_offset_ts[0]);
		  ATS_MSG_SPRINTF_HIGH("TIME_DEBUG: Time set TaskName %s ; timetick=0x%.8x%.8x", 
  			   		&time_genoff_set_log[time_genoff_set_log_cntr].tcb_string[0],
  			   		(uint32)(time_genoff_set_log[time_genoff_set_log_cntr].timetick_now>>32),
  			   		(uint32)(time_genoff_set_log[time_genoff_set_log_cntr].timetick_now&0xFFFFFFFF));
		}
	else
		{
		ATS_MSG_4("TIME_DEBUG: PROXY base update: delta_ms=0x%.8x%.8x , "
			      "Parent_base = 0x%x ; Proxy base = 0x%x",
						(uint32)(delta_ms>>32),
						(uint32)(delta_ms&0xFFFFFFFF),
						parent_base,
						ptime_genoff->bases_type);
		ATS_MSG_SPRINTF_HIGH("TIME_DEBUG: PROXY base update: Time set TaskName=%s : timetick=0x%.8x%.8x",
			            &time_genoff_set_log[time_genoff_set_log_cntr].tcb_string[0],
						(uint32)(time_genoff_set_log[time_genoff_set_log_cntr].timetick_now>>32),
						(uint32)(time_genoff_set_log[time_genoff_set_log_cntr].timetick_now&0xFFFFFFFF));

        /* TOD updated as proxy base, update the debug info log as well */
		time_genoff_last_tod_update_log.base = parent_base ;
        memscpy((void*)(&time_genoff_last_tod_update_log.task_name[0]), 
                20, 
                (void*)(&time_genoff_set_log[time_genoff_set_log_cntr].tcb_string[0]),  
                20);
		}
			 
	time_genoff_set_log_cntr = (time_genoff_set_log_cntr+1)%MAX_TIME_GENOFF_TIME_SET_LOG_SIZE;
    
    time_timetick_log_cb((timer_cb_data_type)ptime_genoff->bases_type);
	}

} /* time_genoff_set_logging */
#endif /* FEATURE_TIME_DEBUG */



/*=============================================================================

FUNCTION TIME_GENOFF_PRINT_DEBUG_STATS

DESCRIPTION
  Prints the debug statistics from various time debug logs.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_genoff_print_debug_stats( void )
{
#ifdef FEATURE_TIME_DEBUG
  uint32 index = 0;

 /* Print stats for internal ATS bases */ 
 ATS_ERR(" ");
 ATS_ERR("PRINTING INFORMATION FOR INTERNAL ATS_BASES");
 for (index = 0 ; index < ATS_MAX  ; index++)
 {
    ATS_ERR_7("BASE=%d genoff=0x%.8x%.8x, sum_offset=0x%.8x%.8x, sum_ts=0x%.8x%.8x",
              index,
              ats_bases[index].generic_offset>>32,
              ats_bases[index].generic_offset,
              ats_bases[index].sum_generic_offset>>32,
              ats_bases[index].sum_generic_offset,
              ats_bases[index].sum_generic_offset_ts[1],
              ats_bases[index].sum_generic_offset_ts [0]);

   ATS_ERR_5("pre_init=%d, initialized=%d, set=%d, deinit=%d",
              index,
              ats_bases[index].state.time_genoff_state_pre_init,
              ats_bases[index].state.time_genoff_state_initialized,
              ats_bases[index].state.time_genoff_state_set,
              ats_bases[index].state.time_genoff_state_deinit);
 }
 /* Printing empty line for readability */
 ATS_ERR(" ");


 /* Print stats for private bases */ 
 ATS_ERR("PRINTING INFORMATION FOR EXTERNAL PRIVATE BASES"); 
 for (index = 0 ; index < ATS_PRIVATE_MAX  ; index++)
 {
    if(ats_private_bases[index]!=NULL)
    {
      ATS_ERR_7("BASE=%d genoff: 0x%.8x%.8x, sum_offset: 0x%.8x%.8x, sum_ts: 0x%.8x%.8x",
                index,
                ats_private_bases[index]->generic_offset>>32,
                ats_private_bases[index]->generic_offset,
                ats_private_bases[index]->sum_generic_offset>>32,
                ats_private_bases[index]->sum_generic_offset,
                ats_private_bases[index]->sum_generic_offset_ts[1],
                ats_private_bases[index]->sum_generic_offset_ts [0]);

      ATS_ERR_5("BASE=%d pre_init=%d, initialized=%d, set=%d, deinit=%d",
                 index,	  	
                 ats_private_bases[index]->state.time_genoff_state_pre_init,
                 ats_private_bases[index]->state.time_genoff_state_initialized,
                 ats_private_bases[index]->state.time_genoff_state_set,
                 ats_private_bases[index]->state.time_genoff_state_deinit);
    }
 }

 /* Printing empty line for readability */
 ATS_ERR(" "); 

 /* Print the last update information for TOD base */
  ATS_MSG_SPRINTF_2("TIME_DIAG_DEBUG: TOD last updated by Base = 0x%x, Task = %s",
  	             time_genoff_last_tod_update_log.base,
  	             time_genoff_last_tod_update_log.task_name);

  /* Print the last update information for TOD base */
  ATS_ERR_2("TIME_DIAG_DEBUG: efs_reset_bases=0x%.8x;set_reset_bases=0x%.8x",
  	             time_genoff_efs_reset_bases,time_genoff_reset_bases);

  /* Print the time set params passed */
  ATS_ERR_3("TIME_DIAG_DEBUG: Last time set done on : base=0x%x, unit=0x%x dal_tt_valid=0x%.8x",
             time_genoff_set_params_log.base,
             time_genoff_set_params_log.unit,
             time_genoff_set_params_log.dal_timetick_val_valid);

  /* Print the time set passed params */
  if (time_genoff_set_params_log.unit == TIME_MSEC)
  	{
	  ATS_ERR_2("         passed_params_ms=0x%.8x%.8x",
				(uint32)(time_genoff_set_params_log.params.msecs >> 32),
				(uint32)(time_genoff_set_params_log.params.msecs & 0xFFFFFFFF));
  	}
  else if (time_genoff_set_params_log.unit == TIME_SECS)
  	{
	  ATS_ERR_2("         passed_params_secs=0x%.8x%.8x",
				(uint32)(time_genoff_set_params_log.params.secs >> 32),
				(uint32)(time_genoff_set_params_log.params.secs & 0xFFFFFFFF));
  	}
  else if (time_genoff_set_params_log.unit == TIME_STAMP)
  	{
	  ATS_ERR_2("         passed_params_ts=0x%.8x%.8x",
				(uint32)(time_genoff_set_params_log.params.ts[1]),
				(uint32)(time_genoff_set_params_log.params.ts[0]));
  	}
  else if (time_genoff_set_params_log.unit == TIME_JULIAN)
    {
      ATS_ERR_6("         passed_params_julian== %.2d/%.2d/%d %.2d:%.2d:%.2d",
				      time_genoff_set_params_log.params.julian.month,
				      time_genoff_set_params_log.params.julian.day,
				      time_genoff_set_params_log.params.julian.year,
				      time_genoff_set_params_log.params.julian.hour,
				      time_genoff_set_params_log.params.julian.minute,
				      time_genoff_set_params_log.params.julian.second );
    }   
    
  /* Printing empty line for readability */
  ATS_ERR(" "); 

  ATS_MSG_ERROR("TIME_DIAG_DEBUG: Printing from time_genoff_set_log structure",0,0,0);
  
  for ( index = 0 ; index < MAX_TIME_GENOFF_TIME_SET_LOG_SIZE ; index++ )
  	{
  	  if (time_genoff_set_log[index].caller_tcb == NULL)
  	  	{
  	  	 /* Bail out of the loop, since the log is not filled yet */
  	  	 break;
  	  	}
	  ATS_MSG_SPRINTF_2("time_genoff_set_log[%d], updated by task = %s",
	  	         index,&time_genoff_set_log[index].tcb_string[0]);

      ATS_ERR_5("         base=0x%x, parent_base=0x%x, proxy_base=0x%x, new_offset=0x%.8x%.8x",
                time_genoff_set_log[index].base,
                time_genoff_set_log[index].parent_base,
                time_genoff_set_log[index].proxy_base,
                (uint32)(time_genoff_set_log[index].new_offset >> 32),
                (uint32)(time_genoff_set_log[index].new_offset & 0xFFFFFFFF));
                
      ATS_ERR_6("         delta_ms=0x%.8x%.8x, old_tod_offset=0x%.8x%.8x, timetick=0x%.8x%.8x",
                (uint32)(time_genoff_set_log[index].delta_ms >> 32),
                (uint32)(time_genoff_set_log[index].delta_ms & 0xFFFFFFFF),
                (uint32)(time_genoff_set_log[index].old_tod_offset >> 32),
                (uint32)(time_genoff_set_log[index].old_tod_offset & 0xFFFFFFFF),
                (uint32)(time_genoff_set_log[index].timetick_now >> 32),
                (uint32)(time_genoff_set_log[index].timetick_now & 0xFFFFFFFF));
  	}  
#endif /* FEATURE_TIME_DEBUG */
} /* time_genoff_print_debug_stats */


/*=============================================================================

FUNCTION TIME_GENOFF_SET_HELPER

DESCRIPTION
  Continuation of function time_genoff_set_generic_offset()

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
static void time_genoff_set_helper
(
  /* Pointer to genoff in consideration */
  time_genoff_ptr      ptime_genoff,

  /* New generic offset value */
  int64                new_offset_ms,

  /* parent base for proxy bases */
  time_bases_type 	   parent_base
)
{
  /* Old generic offset value */
  int64				 old_offset_ms = 0;

  /* delta_ms for proxy base */
  int64					   delta_ms;

  /* Iteration variable */
  uint8           iter;

  /* for marking proxy base of ATS_3GPP invalid once TOD is updated */
  static boolean proxy_3gpp_marked_invalid = FALSE;

  /* Backup for original Offset values */  
  int64             temp_generic_offset = 0;
  int64             temp_sum_generic_offset  = 0;
  int64             temp_sum_generic_offset2  = 0;
  time_type         temp_sum_generic_offset_ts;
  boolean           temp_sum_generic_offset_ts_is_negative;  

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Backup the original offsets in case there is invalid time-set operation */
  old_offset_ms = temp_generic_offset = ptime_genoff->generic_offset ;
  temp_sum_generic_offset = ptime_genoff->sum_generic_offset ;
  qw_equ(temp_sum_generic_offset_ts, ptime_genoff->sum_generic_offset_ts);  
  temp_sum_generic_offset_ts_is_negative = ptime_genoff->sum_generic_offset_ts_is_negative;  

  /* Acquire the mutex before changing the offset values */
  TIME_OSAL_MUTEX_LOCK(ptime_genoff->mutex);

  /* Set Generic Offset */
  ptime_genoff->generic_offset = new_offset_ms;

  /* Set sum_generic_offset value */
  time_genoff_cb_update_sum_gen_off( (void*)ptime_genoff, 0 );

  /* Check if the time-set is valid or not */
  if( FALSE == time_genoff_check_time( ptime_genoff ) )
	{
	  /* Restore the original values of the offsets & return
	        from the call rejecting invalid time set operation. */
	  ptime_genoff->generic_offset = temp_generic_offset ;
	  ptime_genoff->sum_generic_offset = temp_sum_generic_offset ;
	  qw_equ(ptime_genoff->sum_generic_offset_ts, temp_sum_generic_offset_ts);
	  ptime_genoff->sum_generic_offset_ts_is_negative = temp_sum_generic_offset_ts_is_negative;	  

	  /* Update the global var to indicate which base got their offsets reset */
	  if (ptime_genoff->bases_type != ATS_PRIVATE)
	    time_genoff_reset_bases |= 1<<(ptime_genoff->bases_type) ;
	  
      TIME_OSAL_MUTEX_UNLOCK(ptime_genoff->mutex);
	  ATS_ERR_2("TIME_DEBUG: time_genoff_check_time failed, base=0x%x dal_tt_valid=0x%.8x",
             time_genoff_set_params_log.base,
             time_genoff_set_params_log.dal_timetick_val_valid);
	  return ;
	}

  /* Get the value from the new sum_generic_offset */
  temp_sum_generic_offset2 = ptime_genoff->sum_generic_offset;

  if(temp_sum_generic_offset2 < 0)
    temp_sum_generic_offset2 *= -1;

  /*Check if the new offset passed is proper or is a corrupted one */  
  if((temp_sum_generic_offset2 >> 32) > TIME_GENOFF_OFFSET_MAX_VAL_UPPER_WORD)
  {
	  /* Revert to the original values of the offsets & return
	        from the call rejecting invalid time set operation. */
	  ptime_genoff->generic_offset = temp_generic_offset ;
	  ptime_genoff->sum_generic_offset = temp_sum_generic_offset ;
	  qw_equ(ptime_genoff->sum_generic_offset_ts, temp_sum_generic_offset_ts);
	  ptime_genoff->sum_generic_offset_ts_is_negative = temp_sum_generic_offset_ts_is_negative;	  

	  /* Update the global var to indicate which base got their offsets reset */
	  if (ptime_genoff->bases_type != ATS_PRIVATE)
	    time_genoff_reset_bases |= 1<<(ptime_genoff->bases_type) ;
	  
      TIME_OSAL_MUTEX_UNLOCK(ptime_genoff->mutex);
	  ATS_ERR_3("TIME_DEBUG: Offset crossed threshold :base=0x%x reverted_offset_val=0x%.8x%.8x",
             time_genoff_set_params_log.base,
             (uint32)(temp_sum_generic_offset2>>32),
             (uint32)(temp_sum_generic_offset2&0xFFFFFFFF));	

	  return ;
  }
	
  /* Log the time set operation */
  #ifdef FEATURE_TIME_DEBUG
  time_genoff_set_logging(ptime_genoff,
						  old_offset_ms,
					      parent_base);
  #endif /* FEATURE_TIME_DEBUG */

  /* Get the delta msecs */
  delta_ms = new_offset_ms - old_offset_ms;

  /* Write offset to efs */
  time_genoff_write_to_efs( ptime_genoff, FALSE );

  /* Remote offset.
     * Call only if the base is one of the internal ATS bases.
     */
  if (ptime_genoff->bases_type < ATS_MAX)
  {
  time_genoff_remote_offset( ptime_genoff, delta_ms );
  }

  /* Mark valid as TRUE */
  ptime_genoff->valid = TRUE;

  /*Mark the time as in coverage*/
  ptime_genoff->in_coverage_time = TRUE;  

  /* If base is TOD then call fn to send log packet */
  if( proxy_3gpp_marked_invalid == FALSE && ATS_TOD == ptime_genoff->bases_type )
   {

	/* Mark the bool as TRUE */
    proxy_3gpp_marked_invalid = TRUE;
 
    /*Requirements is as follows:
          If ATS_TOD is being set by any Radio Tech (RT), do not allow
          ATS_3GPP or ATS_WCDMA to set the time again. Even if 
          ATS_3GPP or ATS_WCDMA is now setting this ATS_TOD, disallow
          them to set the ATS_TOD in future (until the next coverage outage).
          The reason is that time services maintains time in msec granularity
          and ATS_3GPP and ATS_WCDMA set time in seconds granularity.
          If we do not allow ATS_3GPP and ATS_WCDMA to repeatedly set ATS_TOD,
          then there is a possibility of timestamp jumping back due to loss 
          of accuracy.
          For example, let's say ATS_3GPP sets time = 10000 seconds since epoch
          After 250 msec, time_get_ms() should return 10000250 
          At this point if ATS_3GPP again sets time = 10000 seconds, 
          time_get_ms() will return a non-monotonically-increasing value of 
          10000000 and will be manifested as timestamps on QXDM jumping back.
       */
 
    time_genoff_add_proxy_base(ATS_3GPP,ATS_INVALID);
    time_genoff_add_proxy_base(ATS_WCDMA,ATS_INVALID);
    time_genoff_add_proxy_base(ATS_LTE_HR,ATS_INVALID);
   } 

  /* Update the time_genoff state */
  ptime_genoff->state.time_genoff_state_set = 1;

  /* Release the mutex */
  TIME_OSAL_MUTEX_UNLOCK(ptime_genoff->mutex);

  /* Call callback functions */
  for( iter = 0; iter < ptime_genoff->number_of_cb_func; iter ++)
	{
	  ( ptime_genoff->cb_func[iter] )( ptime_genoff->registered_genoff[iter], 
							   delta_ms );
	}

} /* time_genoff_set_helper */


/*=============================================================================

FUNCTION TIME_GENOFF_EXT_ADD_PD_NUM

DESCRIPTION
  Assigns the respective PD number to the external genoff base.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
ats_error_type time_genoff_ext_add_pd_num
(
  uint32            base_num,
  uint32            pd_num
)
{

  /* Pointer for the external genoff base */
  time_genoff_ptr	ptime_genoff = NULL;

  ptime_genoff = time_genoff_get_pointer(base_num);

  /* Check if the ext genoff is in proper state */
  if (!ptime_genoff->state.time_genoff_state_pre_init)
  {
    return ATS_FAILURE;
  }

  TIME_OSAL_MUTEX_LOCK(ptime_genoff->mutex);

  /* Update the PD num for this external genoff base */
  ptime_genoff->remote_pd = pd_num;
    
  TIME_OSAL_MUTEX_LOCK(ptime_genoff->mutex);

  return ATS_SUCCESS;

}/* time_genoff_ext_add_pd_num */


/*=============================================================================

FUNCTION TIME_GENOFF_PRIVATE_BASE_INIT

DESCRIPTION
  Does initialization of the external genoff pointer.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
ats_error_type time_genoff_private_base_init
(
  /* Filename to associate with the external genoff pointer */
  char *                       f_name,

  /* Pointer where the base value will be returned */
  uint32 *                     ret_base
)
{

  /* Pointer to ats_bases in consideration */
  time_genoff_ptr              ptime_genoff;

  /* Array index iterator */
  uint32                       index = 0, free_index = 0;
  boolean                      found = FALSE;

  char                         temp_string[TIME_GENOFF_MAX_FILE_NAME_LENGTH] = {0};

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*
   1) time_genoff_pre_init
   2) time_genoff_add_init_func ( Only if init_func is needed  ) 
   3) time_genoff_add_base_subsys ( Only if base_subsys is needed ) 
   4) time_genoff_updates_per_storage ( Only on Modem Proc, If NV is not used )
   5) time_genoff_post_init 
   */ 

  if (ats_private_bases_inited != TRUE )
  {
    ATS_MSG_SPRINTF_1("time_genoff_private_base_init: ats_private_bases init incomplete , f_name=%s",f_name);
	return ATS_EARLY_INIT_CALLED;
  }


  /* The passed filename should not exceed the destination size */
  if  (f_name != NULL)
  {
    if ((strlen(f_name) > MAX_EFS_FILE_NAME_SIZE))
   	{
      ATS_MSG_SPRINTF_1("time_genoff_private_base_init: file length exceeded, f_name=%s",f_name);
      return ATS_INVALID_INPUT;
    }
  }
  else
  {
    ATS_ERR("time_genoff_private_base_init: NULL argument in f_name");
    return ATS_INVALID_INPUT;
  }

  /* Input validation */
  if(ret_base == NULL)
  {
    ATS_MSG_SPRINTF_1("time_genoff_private_base_init: NULL pointer in args ,f_name=%s",f_name);
    return ATS_INVALID_INPUT;
  }

  /* Init the ret_base with ATS_INVALID value */
  *ret_base = ATS_INVALID;

  /* Copy the string to append to a local buffer */
  strlcpy(temp_string,ATS_PRIV_APPEND_STRING,ATS_PRIV_APPEND_STRING_SIZE);

  /* Before continuing witht the genoff initialization
     * append this ats_priv string to the passed filename.
     */
  strlcat(temp_string,f_name,MAX_EFS_FILE_NAME_SIZE);

  /* Lock the ats private bases array access mutex */
  TIME_OSAL_MUTEX_LOCK(&ats_private_mutex);
  
  /* Iterate through array of pointers to the private bases */
  for( index=0; index<ATS_PRIVATE_MAX; index++ )
  {
    /* Verify that this filname is unique & the associated base does not already exists */
    if((ats_private_bases[index]!= NULL) && 
       (!strncmp(ats_private_bases[index]->per_storage_spec.f_name,
		        temp_string,
		        TIME_GENOFF_MAX_FILE_NAME_LENGTH)))
    {
      ATS_MSG_SPRINTF_1("time_genoff_private_base_init: private base f_name=%s already inited",f_name);
	  TIME_OSAL_MUTEX_UNLOCK(&ats_private_mutex);
	  return ATS_PRIVATE_BASE_EXISTS;
    }

	/* Get the index to the free element from this array */
	if (found == FALSE && ats_private_bases[index] == NULL)
	{
	  free_index = index;
	  found = TRUE;
	}
  }

  /* Check if there is an exhaustion */
  if ( found != TRUE )
  {
    ERR_FATAL("ats_private_bases exhausted; f_name=%s",f_name,0,0);
    TIME_OSAL_MUTEX_UNLOCK(&ats_private_mutex);
	return ATS_FAILURE;
  }

  /* Get the pointer to time_base instance */
  ptime_genoff = malloc(sizeof(time_genoff_struct_type));

  if ( ptime_genoff == NULL)
  {
    ATS_MSG_SPRINTF_1("time_genoff_private_base_init: Unable to allocate memory f_name=%s",f_name);
	TIME_OSAL_MUTEX_UNLOCK(&ats_private_mutex);
	return ATS_MALLOC_FAIL;
  }

  /* Assign this genoff pointer to the array of pointers to private genoff structure */
  ats_private_bases[free_index] = ptime_genoff;

  /* Release the ats private bases array mutex */
  TIME_OSAL_MUTEX_UNLOCK(&ats_private_mutex);

  /* Memset this malloced memory */
  memset(ptime_genoff,0x0,sizeof(time_genoff_struct_type));

  /* Encode the index with XOR logic before using it */
  free_index = time_genoff_encode_private_base((uint32)free_index);

  /* Initialize ats_bases */
  time_genoff_pre_init( ptime_genoff, free_index );

  /* Acquire the mutex for this private base */
  TIME_OSAL_MUTEX_LOCK(ptime_genoff->mutex);
   
  /* Add base subsys to ats_bases */
  time_genoff_add_base_subsys( ptime_genoff, ATS_RTC );
   
  #if defined(FEATURE_MULTIPROCESSOR) && !defined(FEATURE_TIME_NO_RPC)
    time_genoff_set_remote_call_needed( ptime_genoff, FALSE );
  #endif /* FEATURE_MULTIPROCESSOR && !FEATURE_TIME_NO_RPC */
   
  #ifndef IMAGE_APPS_PROC
    /* Initialize storage spec */
    time_genoff_updates_per_storage(ptime_genoff,
                                    temp_string,
                                    TIME_GENOFF_UPDATE_THRESHOLD_MS);
  #endif /* IMAGE_APPS_PROC */
   
  /* Initialize ats_bases */
  time_genoff_post_init( ptime_genoff );

  /* Update the deinit status flag */
  ptime_genoff->state.time_genoff_state_deinit = 0;

  /* Release the mutex */  
  TIME_OSAL_MUTEX_UNLOCK(ptime_genoff->mutex);

  /* Assign the private genoff base to the passed pointer */
  *ret_base = free_index;

  /* Return SUCCESS */
  return ATS_SUCCESS;

} /* time_genoff_private_base_init */


/*=============================================================================

FUNCTION TIME_GENOFF_EXT_DEINIT

DESCRIPTION
  Does initialization of the external genoff pointer.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
ats_error_type time_genoff_private_base_deinit
(
  /* Pointer to time_genoff_args_type with genoff_ptr in consideration */
  uint32              base,

  /* Filename for the associated base */
  char *              f_name  
)
{
	/* Pointer to the genoff base in consideration */
	time_genoff_ptr 			 ptime_genoff;

    /* Index value for the private base */
	uint32                       index = 0;
    
	void *                       temp_ptr;

    char                         temp_string[TIME_GENOFF_MAX_FILE_NAME_LENGTH] = {0};
	
	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (FALSE == time_genoff_decode_private_base(base,&index))
  {
    ATS_ERR_1("time_genoff_private_base_deinit: Invalid base=0x%.8x",base);
	return ATS_INVALID_BASE_NUM;
  }

  /* Get the pointer to time_base instance */
  ptime_genoff = time_genoff_get_pointer((time_bases_type)base); 
 
  /* Copy the string to append to a local buffer */
  strlcpy(temp_string,ATS_PRIV_APPEND_STRING,ATS_PRIV_APPEND_STRING_SIZE);

  /* Before continuing witht the genoff initialization
     * append this ats_priv string to the passed filename.
     */
  strlcat(temp_string,f_name,MAX_EFS_FILE_NAME_SIZE);

  /* Lock the ats private bases array access mutex */
  TIME_OSAL_MUTEX_LOCK(&ats_private_mutex);

  /* Check if the filename & base number matches */
  if (strncmp(ats_private_bases[index]->per_storage_spec.f_name,
		        temp_string,
		        TIME_GENOFF_MAX_FILE_NAME_LENGTH))
  	{
  	  ATS_MSG_SPRINTF_2("time_genoff_private_base_deinit: Invalid combination;base=0x%.8x;f_name=%s",
		               base,f_name);
	  TIME_OSAL_MUTEX_UNLOCK(&ats_private_mutex);
  	  return ATS_FAILURE;
  	}

  /* Remove this external genoff pointer from the array of extern pointers */
  ats_private_bases[index] = NULL;  

  TIME_OSAL_MUTEX_UNLOCK(&ats_private_mutex);

  
  /* Acquire the mutex */
  TIME_OSAL_MUTEX_LOCK(ptime_genoff->mutex);

  /* Reset the mutex pointer */
  temp_ptr = ptime_genoff->mutex;
  ptime_genoff->mutex = NULL;

  /* Update the state for this external genoff */
  ptime_genoff->state.time_genoff_state_deinit = 1;
  ptime_genoff->state.time_genoff_state_initialized = 0;
  ptime_genoff->state.time_genoff_state_set = 0;
  ptime_genoff->state.time_genoff_state_pre_init = 0;
  
  /* Release the mutex */  
  TIME_OSAL_MUTEX_UNLOCK(temp_ptr);

  /* Delete the file system node for this current base */
  if (TIME_EFS_RW_STAT_OK != time_efs_del_file(ptime_genoff->per_storage_spec.f_name))
  {
    ATS_MSG_SPRINTF_2("time_genoff_private_base_deinit: EFS operation failed;base=0x%.8x;f_name=%s",
                       base,f_name);

	/* Continuing removal of the private base.
	return ATS_FAILURE;
	  */
  }

  /* Free the mutex object */
  free(temp_ptr);
  
  /* Free the private genoff structure */
  free(ptime_genoff);

  return ATS_SUCCESS;

} /* time_genoff_private_base_deinit */

/*=============================================================================

FUNCTION TIME_GENOFF_EXT_DEL_PD_BASE

DESCRIPTION
  Deletes all private bases which are from the passed PD.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
  
=============================================================================*/
void time_genoff_ext_delete_pd_base
(
  uint32            pd_num
)
{
  /* index for traversing */
  uint32            index = 0;

  for( index=0 ; index < ATS_PRIVATE_MAX ; index++ )
  {
    if(ats_private_bases[index] != NULL && ats_private_bases[index]->remote_pd == pd_num)
    {
      /* De-initialize this time genoff base */
      (void)time_genoff_private_base_deinit(time_genoff_encode_private_base(index),
                                   ats_private_bases[index]->per_storage_spec.f_name);
    }
  }

}/* time_genoff_ext_delete_pd_base */


/*=============================================================================

FUNCTION TIME_GENOFF_OPR

DESCRIPTION
  Does time set/get operation on the basis of Inputs.

DEPENDENCIES
  Time to be set should never be less than uptime of the UE.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_opr
(
  /* Pointer to time_genoff_args_type with genoff_ptr in consideration */
  time_genoff_args_ptr         pargs
)
{
  /* Arguments to be passed to base_genoff */
  time_genoff_args_struct_type temp_genoff_args;

  /* Genoff time, in secs */
  time_type                    ts_val_secs;

  /* Genoff time in secs temp variable */
  uint32                       lsecs;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(pargs->base == ATS_PRIVATE)
  {
    ERR_FATAL("time_genoff_opr: ATS_PRIVATE operations not supported pargs=0x%.8x",pargs,0,0);
    return;
  }

  /* GET operation */
  if ( pargs->operation & TIME_GENOFF_OPR_GET_MASK )
  {

    /* unit of time */
    switch( pargs->unit )
    {
      case TIME_STAMP:
      {
        /* Get time in timestamp format */
        time_genoff_get_optimized_ts( pargs );

    break;
      }
      case TIME_MSEC:
      {
        /* Get time in timestamp format */
        time_genoff_get_optimized_ts( pargs );
    
    /* Convert time into msecs */
    time_conv_to_ms( pargs->ts_val );
    break;
      }
      case TIME_SECS:
      {
        /* Get time in timestamp format */
        time_genoff_get_optimized_ts( pargs );

        /* Convert time into secs */
        lsecs = time_conv_to_sec( pargs->ts_val );

        /* Set the lower 32 bits as the value in seconds */
        qw_set(pargs->ts_val, 0, lsecs);
    break;
      }
      case TIME_JULIAN:
      {
        /* Get time in secs */
        temp_genoff_args.base                   = pargs->base;
        temp_genoff_args.base_source            = pargs->base_source;
        temp_genoff_args.ts_val                 = &ts_val_secs;
        temp_genoff_args.unit                   = TIME_SECS;
          temp_genoff_args.operation   = T_GET;
        time_genoff_opr( &temp_genoff_args );

        lsecs = ts_val_secs[0];

        /* Convert time into julian */
        time_jul_from_secs( lsecs, pargs->ts_val);

    break;
      }
      case TIME_20MS_FRAME:
      {
        /* Get time in timestamp format */
    time_genoff_get_optimized_ts( pargs );

    /* Convert time into 20ms frame time */
        time_conv_to_20ms_frame_time( pargs->ts_val );
    break;
      }
      default:
        ATS_ERR_1("Invalid time unit %d", pargs->unit);
        break;
    }
  }
  else if ( pargs->operation & TIME_GENOFF_OPR_SET_MASK ) /* SET operation */
  {
    if ( pargs->base == ATS_RTC )
      return;

    /* Log the set parameters passed */
	time_genoff_set_params_log.base = pargs->base;
	time_genoff_set_params_log.unit = pargs->unit;
	time_genoff_set_params_log.dal_timetick_val_valid = pargs->dal_timetick_val_valid;


    /* unit of time */
    switch( pargs->unit )
    {
      case TIME_STAMP:
      {
        /* Capture the passed param */
		qw_equ(time_genoff_set_params_log.params.ts, pargs->ts_val);

        /* convert time into msec format */
        time_conv_to_ms( pargs->ts_val );

        /* Set time in timestamp format */
        time_genoff_set( pargs );
    break;
      }
      case TIME_MSEC:
      {
        /* Capture the passed param */
		qw_equ_misaligned((void*)&time_genoff_set_params_log.params.msecs,pargs->ts_val);
		
        /* Set time in timestamp format */
        time_genoff_set( pargs );
    break;
      }
      case TIME_SECS:
      {
        uint64 ts64 = 0;
        void* old_ts_val_ptr = (void*)pargs->ts_val;

        /*do not assume that pargs->ts_val is aligned
         at 8-byte boundary*/
        qw_equ_misaligned((void*)&ts64,pargs->ts_val);

		/* Capture the passed param */
		time_genoff_set_params_log.params.secs = ts64;

        /* convert time into msec format */
        ts64 = ts64 * 1000;

        /*set the pargs->ts_val to point to this
         new 64 bit variable*/
        pargs->ts_val = &ts64;
    
        /* Set time in timestamp format */
        time_genoff_set( pargs );

        /*restore the old pointer since ts64 will go
         out of scope*/
        pargs->ts_val = old_ts_val_ptr;
    break;
      }
      case TIME_JULIAN:
      {
        /* Convert time into julian */
          ts_val_secs[0] = time_jul_to_secs( pargs->ts_val );
        ts_val_secs[1] = 0;

        /* Capture the passed param */
        memscpy((void*)(&time_genoff_set_params_log.params.julian), 
                sizeof(time_julian_type), 
                pargs->ts_val,  
                sizeof(time_julian_type));


        
          /* Set time in secs */
        temp_genoff_args.base                   = pargs->base;
        temp_genoff_args.base_source            = pargs->base_source;
        temp_genoff_args.ts_val                 = &ts_val_secs;
        temp_genoff_args.dal_timetick_val       = pargs->dal_timetick_val;
        temp_genoff_args.dal_timetick_val_valid = pargs->dal_timetick_val_valid;		
        temp_genoff_args.unit                   = TIME_SECS;
          temp_genoff_args.operation   = T_SET;
		
        time_genoff_opr( &temp_genoff_args );
        break;
      }
      default:
        ATS_ERR_1("Invalid time unit %d", pargs->unit);
        break;

    } /* End of switch statements */

  }
  else
    {
    ATS_ERR_3("Invalid pargs->operation=0x%.8x, base=0x%.8x from pargs=0x%.8x",pargs->operation,pargs->base,pargs);
      return;
    }


} /* time_genoff_opr */

/*=============================================================================

FUNCTION TIME_GENOFF_SET_SERVER_MODEM_TIME

DESCRIPTION
  This function sets the time on the remote modem. 
  If called from the server modem, it will call the callback function that the 
  client registered at bootup.
  If called from client, it will make an async call to the modem

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_genoff_set_server_modem_time
(
  time_bases_type base_to_be_updated
)
{

#ifdef FEATURE_EXPORT_TIME_REMOTE_MM_FUSION

  /*this feature is suppose to be only on modems*/
  #if((!defined T_MDM9X00 && defined IMAGE_MODEM_PROC) || defined FEATURE_TIME_QDSP6)

  /* Arguments to be sent to set time_rtc */
  time_genoff_args_struct_type    temp_genoff_args ;
  uint64                          msec=0;
  
  temp_genoff_args.base        = base_to_be_updated; 
  temp_genoff_args.base_source = TIME_SCLK;
  temp_genoff_args.ts_val      = (void *)(&msec);
  temp_genoff_args.unit        = TIME_MSEC;
  temp_genoff_args.operation   = T_GET;
  time_genoff_opr( &temp_genoff_args );

  /*increment the sequence number of the update. The corresponding 
   sequence number should be updated in time_remote_genoff_mm_cb()
   in time.c*/
  time_remote_update_inc_seq_num();
  ATS_MSG_1("time_remote_update_seq_num: Remote modem update seq num =  %d",
            time_remote_update_get_seq_num());

  ATS_MSG_HIGH("remote modem update-base=0x%x,msec=0x%.8x%.8x",
            base_to_be_updated,(msec>>32)&0xFFFFFFFF,msec&0xFFFFFFFF);

  /*update the base and the offset info for the cfg_event_info var*/
  /*IMPORTANT !! - For updating the remote modem proc, we are not 
  sending the offset, but the "whole" time itself. */
  cfg_event_info.ats_genoff_update.offset = msec;
  cfg_event_info.ats_genoff_update.base_type = base_to_be_updated;
  ats_cfg_mm_event_notify(ATS_CFG_EVENT_GENOFF_CHANGE, &cfg_event_info );
  #endif

#elif (defined FEATURE_EXPORT_TIME_REMOTE_MM_FUSION_CLNT)
  /* Arguments to be sent to set time_rtc */
  time_genoff_args_struct_type    temp_genoff_args ;
     
    /* Time julian Type */
    time_julian_type                jul_val;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  temp_genoff_args.base        = base_to_be_updated; 
  temp_genoff_args.base_source = TIME_SCLK;
  temp_genoff_args.ts_val      = (void *)(&jul_val);
  temp_genoff_args.unit        = TIME_JULIAN;
  temp_genoff_args.operation   = T_GET;
  time_genoff_opr( &temp_genoff_args );

  /*increment the sequence number of the update. The corresponding 
   sequence number should be updated in time_remote_mm_set_julian()
   in time_remote_modem_mm_fusion.c*/
  time_remote_update_inc_seq_num();
  ATS_MSG_1("time_remote_update_seq_num: Remote modem update seq num =  %d",
            time_remote_update_get_seq_num());

  if(ATS_TOD == base_to_be_updated)
  {
    /*send it over to the server modem*/  
    time_remote_mm_set_julian(&jul_val);
  }
  else if((ATS_USER == base_to_be_updated)
          || (ATS_USER_TZ_DL == base_to_be_updated))
  {
    time_remote_mm_set_time_bases(base_to_be_updated,&jul_val);
  }

  ATS_MSG("mm_fusion_debug:Sending over time update to server modem");  
#endif
} /* time_genoff_set_server_modem_time */

/*=============================================================================

FUNCTION TIME_GENOFF_SYNC_WITH_TIME_TOD

DESCRIPTION
  This function syncs is called from time_tod_set. The purpose of the 
  function is to sync the time maintained by time_tod module and the genoff
  module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_genoff_sync_with_tod
(
  time_type ts_val
)
{
  time_genoff_args_struct_type time_genoff_args;

  time_genoff_args.base        = ATS_TOD; 
  time_genoff_args.base_source = TIME_SCLK;
  time_genoff_args.ts_val      = ts_val;
  time_genoff_args.unit        = TIME_STAMP;
  time_genoff_args.operation   = T_SET;
  time_genoff_opr(&time_genoff_args);
}

/*=============================================================================

FUNCTION TIME_GENOFF_SET_REMOTE_MODEM_UPDATE

DESCRIPTION
  This function sets the static variable which is used to indicate whether
  the remote modem should be updated or not

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_genoff_set_remote_modem_update
(
  boolean update_remote_modem
)
{
  time_genoff_update_remote_modem = (uint8)update_remote_modem;
  ATS_MSG_1("mm_fusion_debug:remote modem update set to %d",time_genoff_update_remote_modem);
}

/*=============================================================================

FUNCTION TIME_GENOFF_GET_REMOTE_MODEM_UPDATE

DESCRIPTION
  This function gets the static variable which is used to indicate whether
  the remote modem should be updated or not

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
boolean time_genoff_get_remote_modem_update
(
  void
)
{
  ATS_MSG_1("mm_fusion_debug:remote modem update queried = %d", time_genoff_update_remote_modem);
  return (boolean)time_genoff_update_remote_modem; 
}


#ifdef FEATURE_QMI_TIME_REMOTE_CLNT
/*=============================================================================

FUNCTION TIME_GENOFF_SET_LEAP_SEC_INFO

DESCRIPTION
  This function sets the leap second info on apps

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_genoff_set_leap_sec_info
(
  time_genoff_leap_sec_info_type lp_sec
)
{
  time_genoff_leap_sec_info.leap_sec_valid = lp_sec.leap_sec_valid;
  time_genoff_leap_sec_info.leap_sec_value = lp_sec.leap_sec_value;
}

/*=============================================================================

FUNCTION TIME_GENOFF_GET_SYS_TIME_IN_SEC_WITH_LP_SEC

DESCRIPTION
  This function sets gets the system time alongwith leap second info

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
uint32 time_genoff_get_sys_time_in_sec_with_lp_sec
(
  void
)
{
  uint32 secs;


  secs = time_get_secs();

  /*if the leap sec info is valid, add the leap sec
   to system time*/
  if(time_genoff_leap_sec_info.leap_sec_valid) 
  {
    secs = secs + time_genoff_leap_sec_info.leap_sec_value;     
  }

  return secs;
}
#endif /*FEATURE_QMI_TIME_REMOTE_CLNT*/

/*=============================================================================

FUNCTION TIME_GENOFF_REPORT_OUT_OF_COVERAGE_TIME

DESCRIPTION
  This function records when a radio technology goes out of coverage

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_genoff_report_out_of_coverage
(
  time_bases_type base
)
{
  if(base < ATS_MAX) 
  {
    ats_bases[base].in_coverage_time = FALSE;
    ATS_MSG_LOW("Time Base = 0x%x reported out of coverage", base,0,0);
    /* Currently not doing anything else other than logging that given network 
       has gone out of coverage */  
  }  
}
