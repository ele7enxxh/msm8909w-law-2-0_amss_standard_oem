#ifndef TIME_REMOTE_ATOM_H
#define TIME_REMOTE_ATOM_H

#ifdef __cplusplus
extern "C"
{
#endif


/*===========================================================================

                        T I M E R   S E R V I C E S
                        
              A P P ' S   TO   M O D E M  F U N C T I O N S

DESCRIPTION
  This header file contains definitions of:
  Modem processor functions called remotely from Apps processor
  (a to m).
 
REFERENCES
  None

Copyright (c) 2005 - 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/time_remote_atom.h#1 $ 
$DateTime: 2016/06/17 14:31:11 $ 
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/5/09    din     Added functions for generic framework.
08/27/09   din     Added time_remote_secure_set_julian(). Regenerated RPC 
                   files with new version number.
06/29/09   din     Extended Alarm period by adding time_alarm_set_secs(). 
                   Also added time_alarm_set_ms().
06/12/09   din     Added time_remote_update_apps_to_modem_offset
03/12/09   din     Added time_remote_secure_get_julian()
08/19/08   jhs     Switch remote secure APIs to use uint64
07/25/08   jhs     Remove featurization in remote header file
04/25/08   jhs     Add time_remote_valid API
04/15/08    mk     Added Secure Time APIs
04/01/08    mk     Made ts_val argument to time_remote_set_user_time an 
                   explicit ptr
04/01/08    mk     Remoted time_get/set_julian APIS 
07/09/07   jhs     Support for Apps to request TOD update from Modem.
06/07/07    mk     Added time_get_user_time functionality
05/04/07   jhs     Added time_get_local_offset_info functionality.
03/07/07   jhs     Fix time_alarm_set function header.
12/01/06   jhs     Fix ATOM implementation.
09/27/06   gb      Created initial version of file
   

===========================================================================*/

/**
@mainpage Time Services CDMA
This service implements implements cdma time-keeping functions 
using the symbol combiner 
*/

/**
@defgroup Time Services CDMA  - Public Interface
This section contains definitions for various public interface defined 
by this module.
*/

/** 
@ingroup Remoted Functions for Time  - Public Interface
@file time_remote_atom.h
This file contains public interface defined by this module
*/


/*============================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "time_types.h"
#include "time_tod.h"
#include "time_remote_reg.h"
#include "time_genoff.h"


/*===========================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items required by this module.

===========================================================================*/


/*===========================================================================

                             FUNCTION DEFINITIONS

===========================================================================*/

/** 
@ingroup Remoted Functions for Time 

@brief  
 Receive RTC time change requests from Apps and update Modem          
 time-of-day.                                                       \n\n

@param[in] 
 time_type - time stamp to set remote user time                     \n\n  
 
@dependencies 
 None													             \n\n
 
@param[out] 
 None													             \n\n
 
@sideffects 
 Synchronizes the slow-clock based time of day to a given timestamp.\n\n

*/

extern void time_remote_set_user_time
(
  time_type                ts_val 
);
/*~ FUNCTION time_remote_set_user_time CLNTNAME time_remote_set_user_time_external */



/** 
@ingroup Remoted Functions for Time 

@brief  
 Set an alarm with the given expiration.                            
 Existing alarm will be canceled.                                   \n\n

@param[in] 
 uint32 Amount of time in milliseconds in which to expire the timer \n\n
 
@dependencies 
 None													             \n\n
 
@param[out] 
 int32 alarm time set                                               \n\n
 
@sideffects 
 None													             \n\n

*/
extern int32 time_alarm_set
(
  uint32         expiration 
);
/*~ FUNCTION time_alarm_set */



/** 
@ingroup Remoted Functions for Time 
 
@brief  
 Retrieves the information needed for BREW for calculating Local   
 Offsets for time.                                                 \n\n

@param[in] uint32*  local timezone in seconds                                                      
@param[in] boolean* value of daylight savings                      \n\n
 
@dependencies None                                                 \n\n
 
@param[out] None                                                   \n\n
 
@sideffects 
 None                                                              \n\n
*/
extern void time_get_local_offset_info
(
  int32                           *local_timezone, 
  boolean                         *daylight_savings 
);
/*~ FUNCTION time_get_local_offset_info */
/*~ PARAM OUT local_timezone POINTER */
/*~ PARAM OUT daylight_savings POINTER */



/** 
@ingroup Remoted Functions for Time 

@brief  
 Request an update to the Apps TOD bases.                           \n\n
 
@param[in] 
 None													             \n\n
 
@dependencies 
 None													             \n\n
 
@param[out] 
 None													             \n\n
 
@sideffects 
 None													             \n\n

*/
extern void time_tod_request_apps_bases
( 
  void 
);
/*~ FUNCTION time_tod_request_apps_bases */


/** 
@ingroup Remoted Functions for Time 
 
@brief  
 Request that the TOD be reset to 6 Jan 1980 00:00:00 and allow OTA
 time updates.                                                     \n\n 
 
@param[in] 
 None													             \n\n
 
@dependencies 
 None													             \n\n
 
@param[out] 
 None													             \n\n
 
@sideffects 
 None													             \n\n

*/
extern void time_tod_request_reset
( 
  void 
);
/*~ FUNCTION time_tod_request_reset */

/** 
@ingroup Remoted Functions for Time                                   

@brief  
 Request from apps to set the system time on the modem side           \n\n
 
@param[in] 
 time_julian_type* Timestamp to set time of day to                   \n\n
 
@dependencies 
 A valid CDMA time, or a valid HDR time, or a valid Time-of-Day, etc.\n\n
 
@param[out]
 None													             \n\n
 
@sideffects 
  Modifies system timestamp								             \n\n

*/
extern void time_remote_set_julian
(
  time_julian_type                *jul_time 
);
/*~ FUNCTION time_remote_set_julian */
/*~ PARAM jul_time POINTER  */



/** 
@ingroup Remoted Functions for Time 
 
@brief  
 Request the system time, in julian format from the modem          \n\n
 
@param[in] 
 time_julian_type* To get the TOD in julian format                 \n\n

@dependencies 
 None													             \n\n
 
@param[out] 
 None													             \n\n
 
@sideffects 
 Modified input parameter and store julian time                    \n\n

*/
extern void time_remote_get_julian
(
  time_julian_type                *jul_time
);
/*~ FUNCTION time_remote_get_julian  */
/*~ PARAM OUT jul_time POINTER  */


/** 
@ingroup Remoted Functions for Time 
 
@brief  
 Request that the TOD be reset to 6 Jan 1980 00:00:00 and allow OTA
 time updates.                                                     \n\n  
 
@param[in]
 None													             \n\n
 
@dependencies 
 None													             \n\n
 
@param[out] 
 None													             \n\n
 
@sideffects 
 None													             \n\n

*/
extern void time_tod_request_reset
( 
  void 
);
/*~ FUNCTION time_tod_request_reset */


/** 
@ingroup Remoted Functions for Time 
 
@brief  
 Set the secure time                                                \n\n
 
@param[in] 
 uint64* timestamp to set secure time                              \n\n
 
@dependencies 
 Secure time must have been initialized in the modem side          \n\n
 
@param[out]
 None													             \n\n
 
@sideffects
 None													             \n\n

*/
extern void time_remote_secure_set(
  uint64                          *ts_val
);
/*~ FUNCTION time_remote_secure_set */
/*~ PARAM ts_val POINTER */


/** 
@ingroup Remoted Functions for Time 
 
@brief  
 Request the secure time in timestamp format                        \n\n
 
@param[in] 
 uint64* timestamp to get secure time                              \n\n
 
@dependencies 
 Secure time must have been initialized in the modem side          \n\n
 
@param[out]
 None													             \n\n
 
@sideffects 
 Modified input parameter and store julian time                    \n\n

*/
extern void time_remote_secure_get(

  uint64                *ts_val  
);
/*~ FUNCTION time_remote_secure_get */
/*~ PARAM OUT ts_val POINTER*/


/** 
@ingroup Remoted Functions for Time 
 
@brief  
 Request the secure time in Julian format                           \n\n
 
@param[in]  
 time_julian_type* to get julian time                               \n\n

@dependencies 
 Secure time must have been initialized in the modem side          \n\n
 
@param[out] 
 None													             \n\n
 
@sideffects 
 Modified input parameter and store julian time                    \n\n

*/
extern void time_remote_secure_get_julian(
  time_julian_type                  *julian
);
/*~ FUNCTION time_remote_secure_get_julian */
/*~ PARAM OUT julian POINTER*/


/** 
@ingroup Remoted Functions for Time 
 
@brief  
 Returns if the secure time is valid                                \n\n
 
@param[in]
 None													             \n\n
 
@dependencies 
 None													             \n\n
 
@param[out] 
 boolean - To return whether the secure time is valid or not		 \n\n
 
@sideffects 
 None													             \n\n

*/
extern boolean time_secure_valid
( 
  void 
);
/*~ FUNCTION time_secure_valid CLNTNAME time_remote_secure_valid */


/** 
@ingroup Remoted Functions for Time 
 
@brief  
 Set an alarm with the given expiration.                            
 Existing alarm will be canceled.                                   \n\n 
 
@param[in] 
 uint32 timer value in msec                                        \n\n
 
@dependencies 
 Secure time must have been initialized from modem side            \n\n
 
@param[out] 
 int32  0 - Success, Other - Failure                               \n\n
 
@sideffects
 None													             \n\n

*/
extern int32 time_alarm_set_ms
(
  uint32 expiration 
);  
/*~ FUNCTION time_alarm_set_ms */


/** 
@ingroup Remoted Functions for Time 
 
@brief  
 Set an alarm with the given expiration.                            
 Existing alarm will be canceled.                                   \n\n 
 
@param[in] 
 uint32 timer value in sec                                         \n\n
 
@dependencies 
 None													             \n\n

@param[out] 
 int32   0 - Success, Other - Failure                              \n\n
 
@sideffects
 None													             \n\n

*/
extern int32 time_alarm_set_secs
(
  uint32 expiration 
);
/*~ FUNCTION time_alarm_set_secs */


/** 
@ingroup Remoted Functions for Time 
 
@brief  
 Updates apps to modem offset on Modem                           \n\n
 
@param[in] 
 int32 offset value                                               \n\n
 
@dependencies 
 Used only for WM.                                                \n\n
 
@param[out]
 None													             \n\n
 
@sideffects
 None													             \n\n

*/
void time_remote_update_apps_to_modem_offset
(
  int32 offset 
);
/*~ FUNCTION time_remote_update_apps_to_modem_offset */


/** 
@ingroup Remoted Functions for Time 
 
@brief  
 Request the secure time in timestamp format                      \n\n
 
@param[in] 
 time_julian_type* to put secure time of day                       \n\n
 
@dependencies 
 Secure time must have been initialized in the modem side          \n\n
 
@param[out]
 None													             \n\n
 
@sideffects
 None													             \n\n

*/
extern void time_remote_secure_set_julian
(
  time_julian_type                  *julian 
);
/*~ FUNCTION time_remote_secure_set_julian */
/*~ PARAM julian POINTER */


/** 
@ingroup Remoted Functions for Time 
 
@brief  
 Allow the client to register itself with the API.               \n
 
@param[in] ats_client_type_e_type        client_type                         
@param[in] ats_client_id_type*           to return client id                
@param[in] ats_client_processor_e_type   processor              \n\n
 
@dependencies 
 None													             \n\n
 
@param[out] 
 ats_client_err_e_type client error codes                         \n\n
 
@sideffects 
 Internal data initialized for the new client.                     \n\n

*/
ats_client_err_e_type ats_client_init
(
  ats_client_type_e_type   client_type,
  ats_client_id_type       *client_id_ptr,
  ats_client_processor_e_type  processor
);
/*~ FUNCTION ats_client_init */
/*~ PARAM INOUT client_id_ptr POINTER */
/*~ RELEASE_FUNC ats_client_release(*client_id_ptr)
    ONERROR return ATS_CLIENT_ERR_RPC */


/** 
@ingroup Remoted Functions for Time 
 
@brief  
 Allow the client to release itself from the API.                 \n\n
 
@param[in] 
 ats_client_id_type client id                                      \n\n
            
@dependencies 
 None													             \n\n
 
@param[out] 
 ats_client_err_e_type client error codes                           \n\n
 
@sideffects 
 Internal data reset for the new client.                            \n\n

*/
ats_client_err_e_type ats_client_release
(
  ats_client_id_type       client_id
);
/*~ FUNCTION ats_client_release
    ONERROR return ATS_CLIENT_ERR_RPC */



/** 
@ingroup Remoted Functions for Time 
 
@brief  
 The client tells the API that it is not ready to use the services\n\n
 
@param[in] 
 ats_client_id_type client id                                      \n\n
            
@dependencies
 None													             \n\n
 
@param[out] 
 ats_client_err_e_type client error codes                          \n\n
 
@sideffects 
 Internal data changed.                                             \n\n

*/
ats_client_err_e_type ats_client_deactivate
(
  ats_client_id_type       client_id
);
/*~ FUNCTION ats_client_deactivate
    ONERROR return ATS_CLIENT_ERR_RPC */


/** 
@ingroup Remoted Functions for Time 
 
@brief  
 Allow the client to register its configuration event callback function.
 If a null pointer is passed, the callback is de-registered.         \n\n
 
@param[in] ats_client_id_type    client id                             
@param[in] ats_cfg_event_cb_type call back function                  \n\n 
            
@dependencies 
 None													             \n\n
 
@param[out] 
 ats_client_err_e_type client error codes                          \n\n
 
@sideffects 
 Internal data changed.                                            \n\n

*/
ats_client_err_e_type ats_client_reg_cfg_cb
(
  ats_client_id_type       client_id,
  ats_cfg_event_cb_type    cfg_event_cb
);
/*~ FUNCTION ats_client_reg_cfg_cb
    ONERROR return ATS_CLIENT_ERR_RPC */

/** 
@ingroup Remoted Functions for Time 
 
@brief  
 The client tells the API that it is now ready to use the services\n\n
 
@param[in] 
 ats_client_id_type client id                                      \n\n
            
@dependencies 
 None													             \n\n
 
@param[out] 
 ats_client_err_e_type client error codes                          \n\n
 
@sideffects 
 Internal data changed.                                            \n\n

*/
ats_client_err_e_type ats_client_activate
(
  ats_client_id_type       client_id
);
/*~ FUNCTION ats_client_activate
    ONERROR return ATS_CLIENT_ERR_RPC */



/** 
@ingroup Remoted Functions for Time 
 
@brief  
 Does time set/get operation on the basis of Inputs.               \n\n
 
@param[in] time_bases_type base for setting/getting                         
@param[in] unit uint to set/get the offset                                  
@param[in] time_genoff_opr_type get/set operation                           
@param[in] base_time_source_type base source                                
@param[in] time_remote_opr_type* time to be passed/returned                 \n\n

            
@dependencies
 None													             \n\n
 
@param[out] 
 None													             \n\n
 
@sideffects 
 None													             \n\n

*/

void time_remote_genoff_opr
(
  time_bases_type                 base,
  time_unit_type                  unit,
  time_genoff_opr_type            operation,     
  base_time_source_type           base_source,
  time_remote_opr_type           *ts_val
);
/*~ FUNCTION time_remote_genoff_opr */
/*~ PARAM INOUT ts_val POINTER DISC unit */



/** 
@ingroup Remoted Functions for Time 
 
@brief  
 Sets Generic offset.                                            \n\n
 
 
@param[in] time_bases_type base                                             
@param[in] uint64          offset                               \n\n

            
@dependencies 
 Should be used only by remote calls                              \n\n
 
@param[out]
 None													             \n\n
 
@sideffects
 None													             \n\n

*/
void time_remote_atom_genoff_set_generic_offset
(
  time_bases_type base,
  int64           offset
);
/*~ FUNCTION time_remote_atom_genoff_set_generic_offset */


/** 
@ingroup Remoted Functions for Time 
 
@brief  
 Sets timer_ipc in the timer subsystem.                              \n\n
 

@param[in] timetick_type Time (in units below) until first timer expiry      
@param[in] timetick_type Period (in units) between repeated expiries         
           (0 = not periodic) 
@param[in] timer_unit_type Unit to measure "ticks" in.                       
@param[in] boolean - If TRUE, Attach it to defferable group                  
@param[in] ats_cfg_event_cb_type - Remote callback                           
@param[in] uint32* - handle for the timer                                    \n\n

            
@dependencies
 None													             \n\n
 
@param[out]
 None													             \n\n
 
@sideffects
 None													             \n\n

*/

void timer_ipc_remote_set
(
  timetick_type                   time,
  timetick_type                   reload,
  timer_unit_type                 unit,
  boolean                         defer,
  ats_cfg_event_cb_type           cfg_cb,
  uint32                          *handle
 
);
/*~ FUNCTION timer_ipc_remote_set */
/*~ PARAM OUT handle POINTER */


/** 
@ingroup Remoted Functions for Time 
 
@brief  
 Clears the timer_ipc associated with the handle                  \n\n
 
@param[in] uint32           Timer Handle                                     
@param[in] timer_unit_type  Unit to measure "ticks before expiry"            
@param[in] timetick_type*   Remaining Time before expiry          \n\n 

@dependencies 
 Timer should be set
 
@param[out]
 None													             \n\n
 
@sideffects
 None													             \n\n

*/
void timer_ipc_remote_clr
(
  uint32                          handle,
  timer_unit_type                 unit,
  timetick_type                   *retval
);
/*~ FUNCTION timer_ipc_remote_clr */
/*~ PARAM OUT retval POINTER */



/** 
@ingroup Remoted Functions for Time 
 
@brief  
 Pauses timer_ipc.                                                  \n\n
 
@param[in] 
 uint32 Timer handle                                               \n\n

@dependencies 
 Timer should be set                                               \n\n
 
@param[out]
 None													             \n\n
 
@sideffects
 None													             \n\n

*/
void timer_ipc_remote_pause
(
  uint32                             handle
);
/*~ FUNCTION timer_ipc_remote_pause */




/** 
@ingroup Remoted Functions for Time 
 
@brief  
 Resumes timer_ipc if paused                                        \n\n
 
@param[in] 
 uint32 Timer handle                                               \n\n

@dependencies 
 Must only be called if timer_ipc is paused.                       \n\n
 
@param[out]
 None													             \n\n
 
@sideffects
 None													             \n\n

*/
void timer_ipc_remote_resume
(
  uint32                             handle
);
/*~ FUNCTION timer_ipc_remote_resume */


/** 
@ingroup Remoted Functions for Time 
 
@brief  
 Get number of ticks before timer_ipc expires                       \n\n
 
@param[in] uint32 Timer handle
@param[in] timer_unit_type unit to measure ticks                             
@param[in] timetick_type* Number of ticks before timer expires.              
                  Zero is returned for inactive timers, and timers 
				  that have expired or are expiring. 
				  Fractional value which might truncate to zero 
                  depending on unit passed as parameter is rounded 
				  to 1.                                              \n\n

@dependencies 
 None													             \n\n
 
@param[out]
 None													             \n\n
 
@sideffects
 None													             \n\n

*/
void timer_ipc_remote_get
(
  uint32                          handle,
  timer_unit_type                 unit,
  timetick_type                   *retval
);
/*~ FUNCTION timer_ipc_remote_get */
/*~ PARAM OUT retval POINTER */



/** 
@ingroup Remoted Functions for Time 
 
@brief  
 Curbs remote genoff updates for time-bases based on bits set in input uint32.                                       \n\n
 
 
@param[in] uint32          curbs
            
@dependencies 
 Should be used only by remote calls.                                                     \n\n
 
@param[out]
 None													             \n\n
 
@sideffects
 None													             \n\n

*/

void time_remote_genoff_set_allow_remote_updates
(
  /* Curbs genoff updates for time_bases */
  uint32 curbs
);
/*~ FUNCTION time_remote_genoff_set_allow_remote_updates */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TIME_REMOTE_ATOM_H */

