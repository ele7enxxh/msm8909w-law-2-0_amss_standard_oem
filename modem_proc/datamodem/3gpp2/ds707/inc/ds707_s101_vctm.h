#ifndef DS707_S101_VCTM_H
#define DS707_S101_VCTM_H

/** 
  @file ds707_s101_vctm.h
  @brief 
   Handles virtual connection throttling functionality.
*/

/*===========================================================================

 D S 7 0 7 _ S101 _ VCTM
GENERAL DESCRIPTION
  This file implements virtual connection throttling module over S101 tunnel

EXTERNALIZED FUNCTIONS


 Copyright (c) 2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_s101_vctm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/21/11   sc      Created module
===========================================================================*/

/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "rex.h"

#ifdef FEATURE_DATA_OPTHO
/*===========================================================================
                           DATA DECLARATIONS
===========================================================================*/
/**
This data structure stores throttle count and backoff timer value. It is	
initiated at DS task initialization by reading NV item ...	
*/
/*todo: add ds707_s101*/
#define VCTM_MAX_NUM_TIMER_VAL               10
#define VCTM_DEFAULT_NO_TIMER_VAL            6
#define VCTM_DEFAULT_FIRST_BACKOFF_TIMER_VAL 15 /*sec*/

typedef PACKED struct PACKED_POST
{
	 uint8                no_valid_timer_val;
	 uint16               backoff_timer_val[VCTM_MAX_NUM_TIMER_VAL]; 
} ds707_s101_vctm_config_type;

typedef struct
{
	 uint8                       thrtl_count;
	 ds707_s101_vctm_config_type vctm_config;
	 rex_timer_type              thrtl_timer;
} ds707_s101_vctm_info_type;

/*===========================================================================
                         EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/
/*===========================================================================
FUNCTION DS707_S101_VCTM_INIT
===========================================================================*/
/**
	 This funtion reads NV item ... and initializes VCTM data structures. It
	 is called during ds task init.

  @param
  None

  @return 
  None
	
  @dependencies 
  None
*/
void ds707_s101_vctm_init( void );

/*===========================================================================
FUNCTION DS707_S101_VCTM_GET_VCTM_INFO
===========================================================================*/
/**
	 This funtion returns the current vctm info, eg. no of failures,
	 timer values, etc.

  @param[in,out]
  ds707_s101_vctm_info_type 

  @return 
  None
	
  @dependencies 
  None
*/
void ds707_s101_vctm_get_vctm_info
(
	 ds707_s101_vctm_info_type* temp_vctm_info
);

/*===========================================================================
FUNCTION DS707_S101_VCTM_SET_THRTL_COUNT
===========================================================================*/
/**
	 This funtion sets the throttle count given value

  @param
  count uint8 

  @return 
  None
	
  @dependencies 
  None
*/
void ds707_s101_vctm_set_thrtl_count( uint8 );

/*===========================================================================
FUNCTION DS707_S101_VCTM_RESET_THROTTLE
===========================================================================*/
/**
	 This funtion clears the throttle count. It also posts unthrottle indication
	 to s101 state machine. This is needed in case VC is throttled and we get an
	 incoming call from HDR over tunnel. We need to clear throttling and move to
	 call_allowed state otherwise incoming call connect (and phys link bring up
	 related to that) will not be successful. This function is also called when
	 color_code change indication is received from HDRPAC. 

  @param
  None

  @return 
  None
	
  @dependencies 
  None
*/
void ds707_s101_vctm_reset_throttle( void );

/*===========================================================================
FUNCTION DS707_S101_VCTM_PROCESS_CALL_FAILURE
===========================================================================*/
/**
	 This funtion is called when virtual connection bringup fails while in
	 tunnel mode. It increments the throttle count until it reaches max no. of
	 count allowd (which is same as no of valid timer values provided in the
	 backoff series. It starts a throttle timer for value corresponding to no.
	 of throttle count. 

  @param
  None

  @return 
  None
	
  @dependencies 
  None
*/
void ds707_s101_vctm_process_call_failure( void );

/*===========================================================================
FUNCTION DS707_S101_VCTM_PROCESS_CALL_SUCCESS
===========================================================================*/
/**
	 This funtion is called when virtual connection bringup is successful while
	 in tunnel mode. This funtion clears the throttle count. It also posts
	 unthrottle indication to s101 state machine. This is needed in case VC is
	 throttled and we get an incoming call from HDR over tunnel. We need to
	 clear throttling and move to call_allowed state otherwise incoming call
	 connect (and phys link bring up related to that) will not be successful.

  @param
  None

  @return 
  None
	
  @dependencies 
  None
*/
void ds707_s101_vctm_process_call_success( void );

/*===========================================================================
FUNCTION DS707_S101_VCTM_THRTL_TIMER_CB
===========================================================================*/
/**
	 This funtion is called when throttling timer expires. This funtion posts
	 unthrottle indication to s101 state machine.

  @param
  None

  @return 
  None
	
  @dependencies 
  None
*/
void ds707_s101_vctm_thrtl_timer_cb( unsigned long );

#endif /* FEATURE_DATA_OPTHO*/
#endif /* DS707_S101_VCTM_H */

