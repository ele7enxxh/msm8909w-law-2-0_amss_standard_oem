#ifndef __TIME_SECURE_H__
#define __TIME_SECURE_H__
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

                 S E C U R E   T I M E   S E R V I C E S

GENERAL DESCRIPTION
  Provides a secure interface to access the time services subsystem.
  Notion:
    Secure time : RTC time + NV Offset
    User time   : Secure Time + User Time offset
    local time  : User Time + GMT Offset to the locality (local offset)

EXTERNALIZED FUNCTIONS
  void time_secure_get_secure_time(time_type ts_val)
    Returns the secure time in timestamp format

  void time_secure_get_secure_time_ms(time_type ms_val)
    Returns the secure time in milliseconds

  uint32 time_secure_get_secure_time_secs(void)
    Returns the secure time in seconds

  void time_secure_set_secure_time(time_type ts_val)
    Sets the secure time with the timestamp value provided

  void time_secure_set_secure_time_secs(uint32 seconds)
    Sets the secure time with the seconds provided

  void time_secure_get_user_time_ms(time_type ms_val)
    Returns the user time in milliseconds

  uint32 time_secure_get_user_time_secs()
    Get the user time in seconds

  void time_secure_get_user_time(time_type ts_val)
    Get the user time in timestamp format

  void time_secure_set_user_time(time_type ts_val)
    Set the user time with the timestamp value provided

  void time_secure_set_user_time_secs(uint32 seconds)
    Set the user time with the seconds value provided

  void time_secure_get_local_time_ms(time_type ms_val)
    Get the local user time in milliseconds

  uint32 time_secure_get_local_time_secs()
    Get the local user time in seconds

  void time_secure_get_local_time(time_type ts_val)
    Get the local user time in timestamp format

  void time_secure_set_local_time(time_type ts_val)
    Set the user time with the timestamp value provided

  void time_secure_set_local_time_secs(uint32 seconds)
    Set the local time with the seconds value provided

  void time_secure_init( void )
    Initializes the user and local offsets from the NV if set  

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright(c) 2008-09 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/time_secure_v2.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/23/09   din     Consoldation of time. Renamed file to time_secure_v2.h
08/21/07    mk     Moved this feature to FEATURE_SEC_TIME_V2
07/13/07    mk     File created.

=============================================================================*/


#include "comdef.h"
#include "time_svc.h"

/**
@mainpage Time Services CDMA
Provides a secure interface to access the time services subsystem. 
*/

/**
@defgroup Time Services CDMA  - Public Interface
This section contains definitions for various public interface defined 
by this module.
*/

/** 
@ingroup Functions for Secure Time
@file time_secure_v2.h
This file contains public interface defined by this module
*/

/** 
@ingroup Functions for Secure Time                   
         
@brief  
 Returns the secure time in timestamp format                            \n\n

@param[in]                                                               
 time_type - Secure Time in timestamp format                           \n\n  
 
@dependencies                                                            
 Secure Time must be active to get secure time                          \n\n
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None																	 \n\n

*/
void time_secure_get_secure_time(
	 time_type ts_val);


/** 
@ingroup Functions for Secure Time                   
         
@brief  
 Returns the secure time in milliseconds                                 \n\n

@param[in]                                                               
 time_type - Secure Time in milliseconds format                         \n\n  
 
@dependencies                                                            
 Secure Time must be active to get secure time                          \n\n
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None																	 \n\n

*/
void time_secure_get_secure_time_ms(
     time_type ms_val);

/** 
@ingroup Functions for Secure Time                   
         
@brief  
 Returns the secure time in seconds                                     \n\n

@param[in]                                                               
 None                                                                  \n\n  
 
@dependencies                                                            
 Secure Time must be active to get secure time                          \n\n
 
@param[out]
 uint32 - Returns the time in seconds                                   \n\n
 
@sideffects 
 None																	 \n\n

*/
uint32 time_secure_get_secure_time_secs(void);

/** 
@ingroup Functions for Secure Time                   
         
@brief  
 Sets the secure time with the timestamp value provided                 \n\n

@param[in]                                                               
 time_type - Time to set in timestamp format                           \n\n  
 
@dependencies                                                            
 Secure time must be set from a secure source                           \n\n
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None																	 \n\n

*/
void time_secure_set_secure_time(
 	 time_type ts_val);

/** 
@ingroup Functions for Secure Time                   
         
@brief  
 Sets the secure time with the seconds provided                         \n\n

@param[in]                                                               
 uint32 - Time to set in seconds                                       \n\n  
 
@dependencies                                                            
 Secure time must be set from a secure source                           \n\n
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None																	 \n\n

*/
void time_secure_set_secure_time_secs(
 	 uint32 seconds);

/** 
@ingroup Functions for Secure Time                  
         
@brief  
 Sets the secure time with the milliseconds provided                    \n\n

@param[in]                                                               
 time_type - Time to set in milliseconds                               \n\n  
 
@dependencies                                                            
 Secure time must be set from a secure source                           \n\n
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None																	 \n\n

*/
void time_secure_set_secure_time_ms(
 	 time_type ms_val);

/** 
@ingroup Functions for Secure Time                   
         
@brief  
 Returns the user time in milliseconds                                  \n\n

@param[in]                                                               
 time_type - User time in milliseconds                                 \n\n  
 
@dependencies                                                            
 User time must have been set. Otherwise will return the secure time if 
 set. Else, the default RTC time However, the user will not be notified 
 of the type of the time returned                                       \n\n
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None																	 \n\n

*/

void time_secure_get_user_time_ms(
 	 time_type ms_val);

/** 
@ingroup Functions for Secure Time                  
         
@brief  
 Returns the user time in seconds                                       \n\n

@param[in]                                                               
 None                                                                  \n\n  
 
@dependencies                                                            
 User time must have been set. Otherwise will return the secure time if 
 set. Else, the default RTC time However, the user will not be notified 
 of the type of the time returned                                       \n\n
 
@param[out]
 uint32 - Returns the user time in seconds                              \n\n
 
@sideffects 
 None																	 \n\n

*/
uint32 time_secure_get_user_time_secs( void );

/** 
@ingroup Functions for Secure Time                   
         
@brief  
 Returns the user time in time stamp format                             \n\n

@param[in]                                                               
 time_type - Returns the user time in time stamp format                \n\n  
 
@dependencies                                                            
 User time must have been set. Otherwise will return the secure time if 
 set. Else, the default RTC time However, the user will not be notified 
 of the type of the time returned                                       \n\n
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None																	 \n\n

*/
void time_secure_get_user_time(
 	 time_type ts_val);

/** 
@ingroup Functions for Secure Time                   
         
@brief  
 Set the user time with the timestamp value provided                    \n\n

@param[in]                                                               
 time_type - sets the secure time in time stamp format                 \n\n  
 
@dependencies                                                            
 None	                                                                 \n\n
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None																	 \n\n

*/
void time_secure_set_user_time(
 	 time_type ts_val);

/** 
@ingroup Functions for Secure Time                   
         
@brief  
 Set the user time with the seconds value provided                      \n\n

@param[in]                                                               
 uint32 - User time to be set in seconds format                        \n\n  
 
@dependencies                                                            
 None	                                                                 \n\n
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None																	 \n\n

*/
void time_secure_set_user_time_secs(
 	 uint32 seconds);

/** 
@ingroup Functions for Secure Time                   
         
@brief  
 Set the user time with the milliseconds value provided                 \n\n

@param[in]                                                               
 time_type - User time to be set in milliseconds format                \n\n  
 
@dependencies                                                            
 None	                                                                 \n\n
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None																	 \n\n

*/
void time_secure_set_user_time_ms(
 	 time_type ms_val);

/** 
@ingroup Functions for Secure Time                   
         
@brief  
 Get the local user time in milliseconds                                \n\n

@param[in]                                                               
 time_type - User time to be set in milliseconds                       \n\n  
 
@dependencies                                                            
 Local time must have been set. Otherwise will return the user time or  
 secure time if set. Else, the default RTC time However, the user will  
 not be notified of the type of the time returned                       \n\n
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None																	 \n\n

*/
void time_secure_get_local_time_ms(
 	 time_type ms_val);

/** 
@ingroup Functions for Secure Time                  
         
@brief  
 Get the local user time in seconds                                     \n\n

@param[in]                                                               
 None                                                                  \n\n  
 
@dependencies                                                            
 Local time must have been set. Otherwise will return the user time or  
 secure time if set. Else, the default RTC time However, the user will  
 not be notified of the type of the time returned                       \n\n
 
@param[out]
 uint32 - Local user time in seconds                                    \n\n
 
@sideffects 
 None																	 \n\n

*/
uint32 time_secure_get_local_time_secs(void);

/** 
@ingroup Functions for Secure Time                   
         
@brief  
 Get the local user time in time stamp format                           \n\n

@param[in]                                                               
 time-type -  To get the local user time in time stamp format          \n\n  
 
@dependencies                                                            
 Local time must have been set. Otherwise will return the user time or  
 secure time if set. Else, the default RTC time However, the user will  
 not be notified of the type of the time returned                       \n\n
 
@param[out]
  None                                                                   \n\n
 
@sideffects 
  None																	 \n\n

*/
void time_secure_get_local_time(
     time_type ts_val);

/** 
@ingroup Functions for Secure Time                   
         
@brief  
 Set the user time with the time stamp format                           \n\n

@param[in]                                                               
 time-type -  To set the user time in time stamp format               \n\n  
 
@dependencies                                                            
 None                                                                  \n\n
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None																	 \n\n

*/
void time_secure_set_local_time(
 	 time_type ts_val);

/** 
@ingroup Functions for Secure Time                   \n\n
         
@brief  
 Set the local time with the seconds value provided                     \n\n

@param[in]                                                               
 uint32  -  seconds - Local time to be set in seconds format           \n\n  
 
@dependencies                                                            
 None                                                                   \n\n
 
@param[out]
  None                                                                   \n\n
 
@sideffects 
  None																	 \n\n

*/
void time_secure_set_local_time_secs(
    uint32 seconds);

/** 
@ingroup Functions for Secure Time                   
         
@brief  
  Set the local time with the msecs value provided                       \n\n

@param[in]                                                               
   time_type  -  Local time to be set in msecs in time stamp format      \n\n  
 
@dependencies                                                            
  None                                                                   \n\n
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None																	 \n\n

*/
void time_secure_set_local_time_ms(
 	time_type ms_val);

/** 
@ingroup Functions for Secure Time                   
         
@brief  
 Returns whether the time returned by time_get is secure or not         \n\n

@param[in]                                                               
 None                                                                  \n\n

@dependencies                                                            
 None                                                                   \n\n
 
@param[out]
 int32 - 1 - Time is secure; 0 - Time is unsecure                       \n\n
 
@sideffects 
 None																	 \n\n

*/
int32 time_secure_is_time_secure( void );

/** 
@ingroup Functions for Secure Time                   
         
@brief  
 Initializes the user and local offsets from the NV if set              \n\n

@param[in]                                                               
 None                                                                  \n\n

@dependencies                                                            
 RTC must be operational.                                               \n\n
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 If the RTC was restarted then the user and local offset will be blown  
 away                                                                   \n\n

*/
void time_secure_init( void );

#endif /* #ifndef __TIME_SECURE_H__ */

