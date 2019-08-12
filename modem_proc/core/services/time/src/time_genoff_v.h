#ifndef TIME_GENOFF_V_H
#define TIME_GENOFF_V_H
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

  time_genoff_get_offset
    Gets Generic offset

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

$Header: //components/rel/core.mpss/3.5.c12.3/services/time/src/time_genoff_v.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/19/15   abh     Changes for properly handling ATS_PRIVATE base operations.
04/26/11   din     Fixed compiler warnings.
08/20/10   din     Added a new optimized time_genoff_get_optimized call.
06/23/10   din     Added new time bases and attached a proxy base for a base.
05/18/10   din     Shifted time_remote_opr_type to time_genoff.h
02/19/10   din     Moved genoff RPC calls from modem proc to TIME_IPC task.
10/07/09   din     File created.

=============================================================================*/


/*===========================================================================
                           INCLUDE FILES
=============================================================================*/

#include "time_genoff.h"
#include "time_osal.h"
#include "rex.h"

/*===========================================================================
                           MACRO DEFINITION
=============================================================================*/

/* String to prepend to the passed ats private base file name */
#define ATS_PRIV_APPEND_STRING              "ats_private"

/* Length of the ATS_PRIV_APPEND_STRING prepended string */
#define ATS_PRIV_APPEND_STRING_SIZE         11

/* Size of combined string for the filename.
 * Keeping +1 at the end to cater for the NUL character.
 */
#define TIME_GENOFF_MAX_FILE_NAME_LENGTH   (MAX_EFS_FILE_NAME_SIZE+ATS_PRIV_APPEND_STRING_SIZE+1)

/* Maximum nuber of external private bases supported */
#define ATS_PRIVATE_MAX 10

/* Threshold to update time base */
#define TIME_GENOFF_INTERNAL_UPDATE_THRESHOLD_MS 10

/* Timetick value valid flag */
#define TIME_GENOFF_TIMETICK_VALID 0x5E15E15E

/* Dummy PD constant for assigning to genoff structures in their pre init */
#define TIME_GENOFF_DUMMY_PD_NUMBER 0xABCD0000

/* 30 sec threshold for efs write */
#define TIME_GENOFF_UPDATE_THRESHOLD_MS 30000

/* Max number of callback functions supported by a base.
  * This needs to be kept as the total internal + private available,
  * since all these need to be updated with the changes in the RTC
  * base.
  */
#define MAX_CB_FUNC              (ATS_MAX)

/*===========================================================================
                           ENUMERATIONS
=============================================================================*/


/*===========================================================================
                           STRUCTURES & TYPEDEF
=============================================================================*/

/* Msecs threshold for updating remote offsets */
#define TIME_GENOFF_REMOTE_UPDATE_MS          10 

/* Threshold for resetting the ATS_RTC value */
#define PMIC_RTC_RESET_CHECK_TIME_THRESHOLD   60000 /* 60k msecs = 1 min */

/* Constant for encoding / decoding the bases index */
#define TIME_GENOFF_ENCODE_CONST 0x0000C3C3

/* Time genoff structure for maintaining efs file items */
typedef struct 
{
  /* Mark as TRUE if persistent storage is needed */
  boolean                        initialized;

  /* file name for efs item file */
  char                           f_name[TIME_GENOFF_MAX_FILE_NAME_LENGTH];

  /* Threshold in ms for writing back in efs item file */
  int64                          threshold;

} time_genoff_per_storage_type;


/** Enums to indicate the state time_genoff structure.
*/
typedef struct time_genoff_struct_state
{
  uint8 time_genoff_state_pre_init;
  uint8 time_genoff_state_initialized;
  uint8 time_genoff_state_set;
  uint8 time_genoff_state_deinit;
} time_genoff_state_type ;


/** Time generic offset structure 
*/
typedef struct time_genoff_ext_struct
{
  /* Genoff structure state */
  time_genoff_state_type          state;

  /* Generic Offset, always stored in ms */
  int64                           generic_offset;

  /* Sum of generic_offset of current base and its bases 
     in milliseconds format */
  uint64                          sum_generic_offset;

  /* Flag to indicate if the absloute value in sum_generic_offset_ts
      is to be added or substracted from the uptime */
  boolean                         sum_generic_offset_ts_is_negative;

  /* sum_generic_offset in timestamp format */
  time_type                        sum_generic_offset_ts;  

  /* Flag to indicate if time-base has ever been set before.
     It is not set to TRUE in time_genoff_set call and 
     not set to TRUE, after value from persistent storage */
  boolean                         valid;

  /* Time base Type */
  time_bases_type                 bases_type;               
  
  /* Initialization Sequence */
  time_genoff_t1_cb_type          init_func;

  /* Mark if subsys based on another subsys */
  boolean                         reqd_base_genoff;

  /* Base Subsys */
  time_bases_type                 subsys_base;

  /* Attach Callback of type2 for set offset */
  time_genoff_t2_cb_type          cb_func[MAX_CB_FUNC];

  /* Bases that have registered for callback */
  struct time_genoff_ext_struct*  registered_genoff[MAX_CB_FUNC];

  /* Currently, Number of Call Back Attached */
  uint8                           number_of_cb_func;

  /* Specification for persistent storage */
  time_genoff_per_storage_type    per_storage_spec;

  /* Flag to indicate if remote call is needed */
  boolean                         remote_call_needed;

  /* Flag to indicate if remote updates is allowed across power cycle */
  boolean                         allow_remote_updates;

  /* Proxy base : Also updated during set opr if current base is updated */
  time_bases_type                 proxy_base;

  /*variable to record if the time is out of coverage. This variable will
    be set to false by technologies going out of coverage and will be set
    to true whenever the new time is set*/
  boolean                         in_coverage_time;

  /* last written value in the efs */
  int64							  time_genoff_last_efs_write_val ;

  /* pimutex lock */
  time_osal_mutex_t *             mutex;

  /* Specific to private genoff base from other process domains */
  uint32                          remote_pd;

} time_genoff_struct_type, *time_genoff_ptr;


#ifdef FEATURE_QMI_TIME_REMOTE_CLNT
typedef struct
{
  boolean leap_sec_valid;
  uint8   leap_sec_value;
}time_genoff_leap_sec_info_type;
#endif

/*--------------------------------------------------------------------------
  Time-services debug structures
--------------------------------------------------------------------------*/
#ifdef FEATURE_TIME_DEBUG

/* Log containing last TOD update value */
typedef struct {
	time_bases_type base;
	char task_name[20];
} time_genoff_last_tod_update_log_type;

/* Base structure type for time-set logging */
typedef struct
{
  time_bases_type  base;           /* base for the genoff in consideration */
  time_bases_type  parent_base;    /* parent base if it is proxy updation */
  time_bases_type  proxy_base;     /* proxy base info */
  int64            new_offset;     /* new generic offset for the base */
  int64            delta_ms;       /* delta b/w new & old offsets in ms */
  int64            old_tod_offset; /* ATS_TOD offset before the updation*/
  rex_tcb_type*    caller_tcb;     /* tcb pointer of the calling task */
  uint64           timetick_now;   /* 19.2Mhz based sclk value */
  char             tcb_string[20]; /* task name which called the time set operation */
}time_genoff_set_log_type;

#define MAX_TIME_GENOFF_TIME_SET_LOG_SIZE 20


/* Logging Function prototype */
void time_genoff_set_logging
(
  /* Pointer to genoff in consideration */
  time_genoff_ptr,
  /* Old Genoff time, in ms */
  int64, 
  /* Parent base if proxy updation is going on */
  time_bases_type
);

/* Log for arguments passed to time_genoff_opr() with SET operation */
typedef struct
{
  time_bases_type  base;
  time_unit_type   unit;
  uint32 dal_timetick_val_valid;
  union   {
	  uint64		   msecs; 	/* Passed argument for time_set in msecs */
	  uint64		   secs;	/* Passed argument for time_set in secs */
	  time_type 	   ts; 	/* Passed argument for time_set in ts */
	  time_julian_type julian; /* Passed argument for time_set in julian */
	} params;
}time_genoff_set_params_log_type;

#endif /* FEATURE_TIME_DEBUG */


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

void time_genoff_init(  void  );


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
);



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
);



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
);



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
);



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
);



/*=============================================================================

FUNCTION TIME_GENOFF_ATTACH_CB

DESCRIPTION
  Attaches call back which are called during after set operation

DEPENDENCIES
  Should be called between time_genoff_pre_init() and time_genoff_post_init() 

RETURN VALUE
  None

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
);


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
);



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
);


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
);


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
);


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

void time_genoff_remote_through_time_ipc(void);



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
);



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
);



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
);



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
  time_genoff_ptr time_genoff,

  /* Generic Offset to be set */
  int64           offset
);

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
);

/*=============================================================================

FUNCTION TIME_GENOFF_SET_SERVER_MODEM_TIME

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None

=============================================================================*/
void time_genoff_set_server_modem_time
(
  time_bases_type base_to_be_updated
);

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
);
#endif
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
);

void time_genoff_set_server_modem_time
(
  time_bases_type base_to_be_updated
);

/*=============================================================================

FUNCTION TIME_GENOFF_SHUTDOWN_NOTIFICATION_FN

DESCRIPTION
  Write the final updates to efs file item on shutdown event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void time_genoff_shutdown_notification_fn( void );

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
void time_genoff_print_debug_stats ( void );

#endif /* TIME_GENOFF_V_H */