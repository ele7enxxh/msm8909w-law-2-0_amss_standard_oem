#ifndef DS707_S101_DCTM_H
#define DS707_S101_DCTM_H

/** 
  @file ds707_s101_vctm.h
  @brief 
   Handles LCP and AUTH throttling functionality in s101 tunnel      
   mode.
*/

/*===========================================================================

 D S 7 0 7 _ S101 _ VCTM
GENERAL DESCRIPTION
  This file implements LCP and AUTH throttling functionality in s101 tunnel      
   mode.

EXTERNALIZED FUNCTIONS


 Copyright (c) 2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_s101_dctm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/05/11   sc      Created module
===========================================================================*/

/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "rex.h"
#include "ps_ppp_defs.h"
#include "dsdctm.h"

#ifdef FEATURE_DATA_OPTHO
/*===========================================================================
                           DATA DECLARATIONS
===========================================================================*/
/**
This data structure stores throttle count and backoff timer value. It is	
initiated at DS task initialization by reading NV item ...	
*/
/*todo: add ds707_s101*/
#define DCTM_MAX_NUM_TIMER_VAL               10
#define DCTM_DEFAULT_NO_TIMER_VAL             6
#define DCTM_DEFAULT_FIRST_BACKOFF_TIMER_VAL  0  /*min*/
#define DCTM_DEFAULT_SECOND_BACKOFF_TIMER_VAL 0  /*min*/
#define DCTM_DEFAULT_THIRD_BACKOFF_TIMER_VAL  1  /*min*/
#define DCTM_DEFAULT_FOURTH_BACKOFF_TIMER_VAL 2  /*min*/
#define DCTM_DEFAULT_FIFTH_BACKOFF_TIMER_VAL  8  /*min*/
#define DCTM_DEFAULT_SIXTH_BACKOFF_TIMER_VAL 15  /*min*/

typedef PACKED struct PACKED_POST
{
	 uint8                no_valid_timer_val;
	 uint16               backoff_timer_val[DCTM_MAX_NUM_TIMER_VAL]; 
} ds707_s101_dctm_config_type;

typedef struct
{
	 uint8                       thrtl_count;
	 ds707_s101_dctm_config_type dctm_config;
	 rex_timer_type              thrtl_timer;
} ds707_s101_dctm_info_type;

/*===========================================================================
                         EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/
/*todo: add feature for test*/
/*===========================================================================
FUNCTION DS707_S101_DCTM_GET_DCTM_INFO
===========================================================================*/
/**
	 This funtion returns the current dctm info, eg. no of failures,
	 timer values, etc.

  @param[in,out]
  ds707_s101_dctm_info_type 

  @return 
  None
	
  @dependencies 
  None
*/
void ds707_s101_dctm_get_dctm_info
(
	 ds707_s101_dctm_info_type* temp_dctm_info
);

/*===========================================================================
FUNCTION DS707_S101_DCTM_SET_THRTL_COUNT
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
void ds707_s101_dctm_set_thrtl_count( uint8 thrtl_count );


/*===========================================================================
FUNCTION DS707_S101_DCTM_INIT
===========================================================================*/
/**
	 This funtion reads NV item ... and initializes S101 DCTM data structures.
	 It is called during ds task init.

  @param
  None

  @return 
  None
	
  @dependencies 
  None
*/
void ds707_s101_dctm_init( void );

/*===========================================================================
FUNCTION DS707_S101_DCTM_POST_PPP_CALL_STATUS
===========================================================================*/
/**
	 This function  posts call fail type to DCTM based on PPP call fail/success
  event and the reason passed.

  @param
  None

  @return 
  None
	
  @dependencies 
  None
*/
void ds707_s101_dctm_post_ppp_call_status
(
  ppp_event_e_type       ppp_event,                /* Down / Up            */
  dsdctm_ppp_protocol_e_type protocol,                 /* LCP, IPCP, Auth    */
  ppp_fail_reason_e_type ppp_fail_reason           /* timeout, auth_fail   */
);

/*===========================================================================
FUNCTION DS707_S101_DCTM_THRTL_TIMER_CB
===========================================================================*/
/**
	 This funtion is called when throttling timer expires. It posts unthrottle
	 indication to s101 state machine.

  @param
  None

  @return 
  None
	
  @dependencies 
  None
*/
void ds707_s101_dctm_thrtl_timer_cb( unsigned long dummy_user_data);

/*===========================================================================
FUNCTION DS707_S101_DCTM_RESET_THROTTLE
===========================================================================*/
/**
	 This function resets the S101_DCTM throttle count to 0 and also stop the
	 throttling timer if it is running. this is called when LCP and auth are
	 successfully brought up either in tunnel or in direct mode.

  @param
  None

  @return 
  None
	
  @dependencies 
  None
*/
void ds707_s101_dctm_reset_throttle( void );

#endif /* FEATURE_DATA_OPTHO*/
#endif /* DS707_S101_DCTM_H */
