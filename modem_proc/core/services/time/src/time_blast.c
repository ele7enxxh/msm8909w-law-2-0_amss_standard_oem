#include "core_variation.h"
#ifndef TIME_9K_Q6_H
#define TIME_9K_Q6_H  

/*===========================================================================

                        T I M E R   S E R V I C E S
                        
                        9K Q6 SPECIFIC FUNCTIONS

DESCRIPTION
  This is a 9k-Q6 specific file. This file contains functions which are Q6 
  specific.

  The stub functions are needed since the introduction of time genoff, no 
longer needs the compilation of time_tod.c etc, but those functions still 
need to be supported on other targets. Due to this, these functions (which 
are no longer needed on Q6), still appear in time_remote_atom.h. Hence the 
need for stubs. A majority of the functions are under #ifdef 
FEATURE_GENOFFALONE.

REFERENCES
  None

Copyright (c) 2005 - 2013 by QUALCOMM Technologies Incorporated.  All Rights Reserved.
===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
$Header: //components/rel/core.mpss/3.5.c12.3/services/time/src/time_blast.c#1 $ 
$DateTime: 2016/12/13 07:59:23 $ 
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/26/11   din     Fixed compiler warnings.
05/14/10   gp      Creation of this file
   

===========================================================================*/



/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "time_types.h"
#include "time_remote_reg.h"
#include "time_genoff_v.h"
#include "time_dal_v.h"
#include "time_conv.h"
#include "DALSys.h"
#include "err.h"

#ifdef FEATURE_TIME_QDSP6
#include "qurt.h"
#include "DDITimetick.h"
#endif

/**
@mainpage Time Services qurt
 This is a 9k-Q6 specific file. This file contains functions which are Q6 
 specific.

  The stub functions are needed since the introduction of time genoff, no 
longer needs the compilation of time_tod.c etc, but those functions still 
need to be supported on other targets. Due to this, these functions (which 
are no longer needed on Q6), still appear in time_remote_atom.h. Hence the 
need for stubs. A majority of the functions are under #ifdef 
FEATURE_GENOFFALONE.. 
*/

/**
@defgroup Time Services qurt  - Public Interface
This section contains does not contain definitions for public 
interface defined by this module. 
*/

/** 
@ingroup Functions specific to qurt - Private Interface
@file time_qurt.c
This file does contains private interface defined by this
module
*/


/*Define all the extern functions here that will be used to initialize
 the function pointers and may be called.*/
extern void time_get_time_defines(time_defines_type*);

/*static functions go here*/
void time_init_fn_ptr(time_fn_ptr_type* time_fn_ptr);
void time_q6_call_init_fn(void);

/** 
@ingroup Functions for Time - qurt
         
@brief  
 This function will initialize all the function pointers for qurt OS \n\n

@param[in] 
time_fn_ptr_type* - pointer to structure which stores the fn 
ptrs                                                                   \n\n 
 
 
@dependencies  
 qurt is needed for this function                                     \n\n 
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None                                                                    \n\n

*/
void time_init_fn_ptr(time_fn_ptr_type* time_fn_ptr)
{
  time_fn_ptr->time_call_init_functions_fp=time_q6_call_init_fn;

}

/** 
@ingroup Functions for Time - qurt
         
@brief  
 Calls the initializing functions                                     \n\n

@param[in]                                                               
 None                                                                  \n\n  
 
@dependencies  
 qurt is needed for this function                                     \n\n 
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None                                                                    \n\n

*/
void time_q6_call_init_fn
(
  void
)
{
}

/** 
@ingroup Functions for Time - qurt
         
@brief  
 Set an alarm with the given expiration.  Existing alarm will be 
 canceled.                                                            \n\n

@param[in] 
uint32 - expiration                                                   \n\n 
 
 
@dependencies  
 None                                                                  \n\n 
 
@param[out]
 uint32 - Returns 0 as of now                                           \n\n
 
@sideffects 
 None                                                                    \n\n

*/
int32 time_alarm_set
(
  /* Amount of time in milliseconds in which to expire the timer */
  uint32                          expiration
)
{
  return 0;
}


/** 
@ingroup Functions for Time - qurt
         
@brief  
 Request that the TOD be reset to 6 Jan 1980 00:00:00 and allow OTA
  time updates.                                                            \n\n

@param[in] 
 None                                                                    \n\n
 
 
@dependencies  
 None                                                                  \n\n 
 
@param[out]
 None                                                                    \n\n
 
@sideffects 
 None                                                                    \n\n

*/
void time_tod_request_reset( void )
{
}

/** 
@ingroup Functions for Time - qurt
         
@brief  
 Returns if the secure time is valid                                    \n\n

@param[in] 
 None                                                                    \n\n
 
 
@dependencies  
 Secure time must have been initialized in the modem side                \n\n 
 
@param[out]
 boolean - As of now, returns FALSE                                      \n\n
 
@sideffects 
 None                                                                    \n\n

*/
boolean time_secure_valid( void )
{
  return FALSE;
}

/** 
@ingroup Functions for Time - qurt
         
@brief  
 Set an alarm with the given expiration.  Existing alarm will be 
 canceled.                                                               \n\n

@param[in] 
 uint32 - expiration                                                     \n\n
 
 
@dependencies  
 None                                                                    \n\n 
 
@param[out]
 int32 - As of now returns 0.                                            \n\n 
 
@sideffects 
 None                                                                    \n\n

*/
int32 time_alarm_set_ms
(
  /* Amount of time in milliseconds in which to expire the timer */
  uint32 expiration 
)
{
  return 0;
}  

/** 
@ingroup Functions for Time - qurt
         
@brief  
 Set an alarm with the given expiration.  Existing alarm will be canceled.\n\n

@param[in] 
 uint32 - expiration                                                     \n\n
 
 
@dependencies  
 None                                                                    \n\n 
 
@param[out]
 int32 - As of now returns 0.                                           \n\n 
 
@sideffects 
 None                                                                    \n\n

*/
int32 time_alarm_set_secs
(
  /* Amount of time in seconds in which to expire the timer */
  uint32 expiration
)
{
  return 0;
}

/** 
@ingroup Functions for Time - qurt
         
@brief  
 Until the external entities switch to using genoff based functions, 
  this function needs to be there.                                      \n\n

@param[in] 
 time_type - pointer to time_stamp to return tod                         \n\n
 
 
@dependencies  
 None                                                                    \n\n 
 
@param[out]
 None                                                                    \n\n
 
@sideffects 
 None                                                                    \n\n

*/
void time_tod_get_no_mutex
(
  /* Output: Slow-clock based time-of-day in ts_timestamp format */
  time_type                       ts_val
)
{
  /* Arguments to be sent to set time_rtc */
  time_genoff_args_struct_type    temp_genoff_args;

  /*As of now, we can return the time got from time genoff 
   as there are no mutex being used in time_genoff_opr function*/
  temp_genoff_args.base        = ATS_TOD; 
  temp_genoff_args.base_source = TIME_SCLK;
  temp_genoff_args.ts_val      = ts_val;
  temp_genoff_args.unit        = TIME_STAMP;
  temp_genoff_args.operation   = T_GET;
  temp_genoff_args.dal_timetick_val_valid = 0;
  temp_genoff_args.dal_timetick_val       = 0;
  time_genoff_opr( &temp_genoff_args );
}

/** 
@ingroup Functions for Time - qurt
         
@brief  
Until the external 
entities switch to using genoff based functions, this function needs to be 
there.                                                                  \n\n

@param[in] 
 time_type - pointer to time_stamp to return tod                         \n\n
 
 
@dependencies  
 None                                                                    \n\n 
 
@param[out]
 None                                                                    \n\n
 
@sideffects 
 None                                                                    \n\n

*/
void time_tod_get_ms (time_type ts_val )
{
  /* Arguments to be sent to set time_rtc */
  time_genoff_args_struct_type    temp_genoff_args;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  temp_genoff_args.base        = ATS_TOD; 
  temp_genoff_args.base_source = TIME_SCLK;
  temp_genoff_args.ts_val      = ts_val;
  temp_genoff_args.unit        = TIME_MSEC;
  temp_genoff_args.operation   = T_GET;
  temp_genoff_args.dal_timetick_val_valid = 0;
  temp_genoff_args.dal_timetick_val       = 0;
  time_genoff_opr( &temp_genoff_args );

}

/** 
@ingroup Functions for Time - qurt
         
@brief  
Until the external entities switch to using genoff based functions, this 
  function needs to be there.                                           \n\n

@param[in] 
 time_type - pointer to time_stamp to set tod                           \n\n
 
 
@dependencies  
 None                                                                    \n\n 
 
@param[out]
 None                                                                    \n\n
 
@sideffects 
 None                                                                    \n\n

*/
void time_tod_set(time_type ts_val )
{
  /* Arguments to be sent to set time_rtc */
  time_genoff_args_struct_type    temp_genoff_args;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  temp_genoff_args.base        = ATS_TOD; 
  temp_genoff_args.base_source = TIME_SCLK ;
  temp_genoff_args.ts_val      = ts_val;
  temp_genoff_args.unit        = TIME_STAMP;
  temp_genoff_args.operation   = T_SET;
  temp_genoff_args.dal_timetick_val_valid = 0;
  temp_genoff_args.dal_timetick_val       = 0;
  time_genoff_opr( &temp_genoff_args );
}

/** 
@ingroup Functions for Time - qurt
         
@brief  
Until the external entities switch to using genoff based functions, this 
  function needs to be there.                                           \n\n

@param[in] 
 uint32 - seconds to set                                                \n\n
 
 
@dependencies  
 None                                                                    \n\n 
 
@param[out]
 None                                                                    \n\n
 
@sideffects 
 None                                                                    \n\n

*/
void time_secure_set_user_time_secs(uint32 secs)
{
  /* Arguments to be sent to set time_rtc */
  time_genoff_args_struct_type    temp_genoff_args;
  time_type                       ts_val;
  ts_val[0]=secs;
  ts_val[1]=0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  time_conv_secs_to_timestamp(ts_val);

  temp_genoff_args.base        = ATS_USER; 
  temp_genoff_args.base_source = TIME_SCLK ;
  temp_genoff_args.ts_val      = ts_val;
  temp_genoff_args.unit        = TIME_SECS;
  temp_genoff_args.operation   = T_SET;
  temp_genoff_args.dal_timetick_val_valid = 0;
  temp_genoff_args.dal_timetick_val       = 0;
  time_genoff_opr( &temp_genoff_args );

}

#endif /* TIME_9K_Q6_H */

