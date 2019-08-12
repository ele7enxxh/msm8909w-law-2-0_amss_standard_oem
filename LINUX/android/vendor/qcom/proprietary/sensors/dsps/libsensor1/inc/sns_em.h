#ifndef _SNS_EM_H
#define _SNS_EM_H

/*============================================================================
  @file sns_em.h

  The sensors subsystem event manager header.

  This header file contains the public interface for the sensors event manager.
  The Event Manager provides timer functionality to the various modules in the
  sensor subsystem.

  Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*---------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/
#include "sns_common.h"

/*---------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* -------------------------------------------------------------------------*/

/** This is the sleep clock speed (32.768 Khz) */
#define DSPS_SLEEP_CLK      32768

/*---------------------------------------------------------------------------
* Function Declarations and Documentation
* -------------------------------------------------------------------------*/

/*=====================================================================================
  FUNCTION:  sns_em_get_timestamp
=====================================================================================*/
/**
This API can be used by modules in apps, modules or DSPS to read the local timer value.
It reads the underlying hardware timer register and returns the value in ticks.

@retval uint32_t (ticks)
*/
uint32_t sns_em_get_timestamp( void );

/*=====================================================================================
  FUNCTION:  sns_em_get_timestamp
=====================================================================================*/
/**
This API can be used by modules in apps, modules or DSPS to read the local timer value.
It reads the underlying hardware timer register and returns the value in ticks.

@param timestamp_p  [OUT] Pointer to a 64-bit number to place the timestamp.

@retval SNS_SUCCESS if the timestamp is set successfully.
*/
sns_err_code_e
sns_em_get_timestamp64( uint64_t *timestamp_p );

/*=====================================================================================
  FUNCTION:  sns_em_sleep
=====================================================================================*/
/**
  This API is used to suspend the thread from execution until the specified duration
  has elapsed.

  @param time_us  [IN] Value of the time to be suspended in unit of us
*/
void sns_em_sleep( uint32_t time_us );

/*=====================================================================================
  FUNCTION:  sns_em_convert_dspstick_to_usec
=====================================================================================*/
/**
This is a utility API which can be used to by modules to covert the DSPS ticks into
microsecond units.

@retval uint32_t (time in microseconds)

@param dsps_tick  [IN]  Time in DSPS ticks

@see sns_em_convert_usec_to_dspstick()
*/
uint32_t sns_em_convert_dspstick_to_usec( uint32_t dsps_tick );

/*=====================================================================================
  FUNCTION:  sns_em_convert_localtick_to_usec
=====================================================================================*/
/**
This is a utility API which can be used to by modules to covert the local tick unit into
microsecond units.

@retval uint32_t (time in microseconds)

@param local_tick  [IN]  Time in local processor ticks

*/
uint32_t sns_em_convert_localtick_to_usec( uint32_t local_tick );

/*=====================================================================================
  FUNCTION:  sns_em_convert_usec_to_dspstick
=====================================================================================*/
/**
This is a utility API which can be used to by modules to covert the microsecond time
into DSPS ticks.

@retval uint32

@param microsec_time  [IN]  Time in microseconds

@see sns_em_convert_dspstick_to_usec()
*/
uint32_t sns_em_convert_usec_to_dspstick( uint32_t microsec_time );

/*=====================================================================================
  FUNCTION:  sns_em_convert_usec_to_localtick
=====================================================================================*/
/**
This is a utility API which can be used to by modules to covert the microsecond time
into local processor ticks.

@retval uint32_t representing ticks in local processor units

@param microsec_time  [IN]  Time in microseconds

@see sns_em_convert_dspstick_to_usec()
*/
uint32_t sns_em_convert_usec_to_localtick( uint32_t microsec_time );

/*=====================================================================================
  FUNCTION:  sns_em_convert_dspstick_to_appstime
=====================================================================================*/
/**
This is a utility API which can be used to by modules to covert the DSPS ticks into
Apps processor ticks.

@retval uint32_t (time in apps tick)

@param dsps_tick  [IN] Time in DSPS ticks
*/
uint32_t sns_em_convert_dspstick_to_appstime( uint32_t dsps_tick );

/*=====================================================================================
  FUNCTION:  sns_em_convert_dspstick_to_gpstime
=====================================================================================*/
/**
This is a utility API which can be used to by modules to covert the DSPS ticks into
GPS time.

@retval uint32_t (GPS time)

@param dsps_tick  [IN] Time in DSPS ticks
*/
uint32_t sns_em_convert_dspstick_to_gpstime( uint32_t dsps_tick );

/*=====================================================================================
  FUNCTION:  sns_em_convert_sec_in_q16_to_localtick
=====================================================================================*/
/**
This is a utility API which can be used by modules to covert the time in seconds
in Q16 format into local processor ticks.

@retval uint32_t representing ticks in local processor units

@param time_sec_q16  [IN]  Time in seconds in Q16 format
*/
uint32_t sns_em_convert_sec_in_q16_to_localtick( uint32_t time_sec_q16 );

#endif /*#ifndef _SNS_EM_H*/
