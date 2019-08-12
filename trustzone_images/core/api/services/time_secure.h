#ifndef _TIME_SECURE_H__
#define _TIME_SECURE_H__
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

                 S E C U R E   T I M E   S E R V I C E S

GENERAL DESCRIPTION
  Provides a secure interface to access the time services subsystem.
  Notion:
    Secure time : User time + NV Offset

EXTERNALIZED FUNCTIONS
  void time_secure_get(time_type ts_val)
    Returns the secure time in timestamp format

  void time_secure_get_ms(time_type ms_val)
    Returns the secure time in milliseconds

  uint32 time_secure_get_secs(void)
    Returns the secure time in seconds

  void time_secure_set(time_type ts_val)
    Sets the secure time with the timestamp value provided

  void time_secure_set_secs(uint32 seconds)
    Sets the secure time with the seconds provided

  void time_secure_set_ms(time_type ms_val)
    Sets the secure time in milliseconds

  void time_secure_init( void )
    Initializes the secure time offset from the NV if set  

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright(c) 2008-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/time_secure.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/22/09   din     Removed feature IMAGE_MODEM_PROC
02/23/09   din     Consolidation of time.
04/25/08   jhs     Add time_remote_valid API.
04/10/08    mk     File created.

=============================================================================*/



#include "comdef.h"
#include "time_tod.h"

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
@file time_secure.h
This file contains public interface defined by this module
*/

/** 
@ingroup Functions for Secure Time
         
@brief  
  Returns if the secure time is valid                                    \n\n

@param[in]                                                               
   None                                                                  \n\n  
 
@dependencies                                                            
  Secure Time must be initialized                                        \n\n
 
@param[out]
  boolean - returns whether the secure time is valid or not              \n\n
 
@sideffects 
  None																	 \n\n

*/

boolean time_secure_valid( void );


/** 
@ingroup Functions for Secure Time

@brief  
 Returns the secure time in timestamp format                            \n\n

@param[in]                                                               
 time_type* - To get the secure time in timestamp format               \n\n  
 
@dependencies                                                            
 Secure Time must be active to get secure time                          \n\n
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None																	 \n\n

*/
void time_secure_get
(
  time_type                       *ts_val
);


/** 
@ingroup Functions for Secure Time

@brief  
 Returns the secure time in milliseconds                            \n\n

@param[in]                                                               
 time_type - To get the secure time in milliseconds                \n\n  
 
@dependencies                                                            
 Secure Time must be active to get secure time                      \n\n
 
@param[out]
 None                                                               \n\n
 
@sideffects 
 None																 \n\n

*/

void time_secure_get_ms
(
  time_type                       ms_val
);


/** 
@ingroup Functions for Secure Time

@brief  
 Returns the secure time in seconds                                 \n\n

@param[in]                                                               
 None                                                              \n\n  
 
@dependencies                                                            
 Secure Time must be active to get secure time                      \n\n
 
@param[out]
 uint32 - returns the secure time in msec                           \n\n
 
@sideffects 
 None																 \n\n

*/

uint32 time_secure_get_secs( void );


/** 
@ingroup Functions for Secure Time

@brief  
 Sets the secure time with the timestamp value provided             \n\n

@param[in]                                                               
 time_type*   Time to set in timestamp format                      \n\n  
 
@dependencies                                                            
 Secure time must be set from a secure source                       \n\n
 
@param[out]
 None                                                               \n\n
 
@sideffects 
 None																 \n\n

*/
void time_secure_set
(
  time_type                       *ts_val
);


/** 
@ingroup Functions for Secure Time

@brief  
 Sets the secure time with the seconds provided                     \n\n

@param[in]                                                               
 uint32   Time to set in seconds                                    \n\n  
 
@dependencies                                                            
 Secure time must be set from a secure source                       \n\n
 
@param[out]
 None                                                               \n\n
 
@sideffects 
 None																 \n\n

*/
void time_secure_set_secs
(
  uint32                          seconds
);


/** 
@ingroup Functions for Secure Time

@brief  
 Sets the secure time with the milliseconds provided                \n\n

@param[in]                                                               
 time_type   Time to set in millids                                \n\n  
 
@dependencies                                                            
 Secure time must be set from a secure source                       \n\n
 
@param[out]
 None                                                               \n\n
 
@sideffects 
 None																 \n\n

*/
void time_secure_set_ms
(
  time_type                       ms_val
);


/** 
@ingroup Functions for Secure Time

@brief  
 Update the offsets to adjust to the new time_tod value              \n\n

                                                               
@param[in] time_type -  Old Time Tod value on which the securetime offset is 
               based off                                              
@param[in] time_type -  New Time Tod value on which the secure time offset     
               should be based off                                  \n\n  

@dependencies                                                            
 Should be called only from Modem Proc                              \n\n
 
@param[out]
 None                                                               \n\n
 
@sideffects 
 None																 \n\n

*/
void time_secure_update_offsets
(
   time_type                      old_time,
   time_type                      new_time
);


/** 
@ingroup Functions for Secure Time

@brief  
 Initializes the user and local offsets from the NV if set          \n\n

@param[in]                                                               
 None                                                              \n\n  
 
@dependencies                                                            
 RTC must be operational. Should be called only from modem proc     \n\n
 
@param[out]
 None                                                               \n\n
 
@sideffects 
 If the RTC was restarted then the user and local offset will be      
 blown away                                                           \n\n

*/
void time_secure_init( void );


#endif /* #ifndef _TIME_SECURE_H__ */
