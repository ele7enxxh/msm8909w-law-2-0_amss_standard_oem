/** 
  @file ds707_s101_vctm.c
  @brief 
   Handles virtual connection throttling functionality.
*/

/*===========================================================================

                            D S 7 0 7 _ S101 _ VCTM
GENERAL DESCRIPTION
  This file implements virtual connection throttling module over S101 tunnel.

EXTERNALIZED FUNCTIONS 
 
INTERNAL FUNCTIONS  


 Copyright (c) 2011-2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_s101_vctm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/30/14   niv     Added wrapper functions & API change for 
                   NV_REFRESH 
08/13/12   ss      Moving efs write operation to dsutil performed during 
                   bootup.
10/21/11   sc      Created module
===========================================================================*/

/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"

#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

#ifdef FEATURE_DATA_OPTHO
#include "data_msg.h"
#include "ds707_s101_vctm.h"
#include "ds3gcfgmgr.h"
#include "ds707_pkt_mgr.h"
#include "ds707_nv_util.h"
#include "dsutil.h"
#include "ds707_s101.h"

/*===========================================================================
                          LOCAL  DATA DECLARATIONS
===========================================================================*/
/**
This data structure stores throttle count and backoff timer value. It is	
initiated at DS task initialization by reading NV item ...	
*/

static ds707_s101_vctm_info_type ds707_s101_vctm_info;

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
void ds707_s101_vctm_init( void )
{
  char item_file_path[] = "/nv/item_files/data/3gpp2/prereg_backoff_timer_virtual_conn";
  int retval;
  int i = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

	 /** Read the pre-reg backoff timer value from NV */
	 retval = ds3gcfgmgr_read_efs_nv_ex(item_file_path,
				 &(ds707_s101_vctm_info.vctm_config),
				 sizeof(ds707_s101_vctm_info.vctm_config), ds707_curr_subs_id());

	 do
	 {
	   if ( retval != NV_DONE_S )
	   {
	     DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
			   "NV item: prereg_backoff_timer_virtual_conn read failed,"
			   " using default values");
	     break;
	   }
	   if ( ds707_s101_vctm_info.vctm_config.no_valid_timer_val > VCTM_MAX_NUM_TIMER_VAL )
	   {
	     DATA_IS707_MSG2(MSG_LEGACY_ERROR, 
			     "NV item: prereg_backoff_timer_virtual_conn"
			     " read successful but num of valid timer values: %d "
			     "is more than max allowed number: %d",
			     ds707_s101_vctm_info.vctm_config.no_valid_timer_val,
			     VCTM_MAX_NUM_TIMER_VAL);
	     break;
	   }

           DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                           "NV item: prereg_backoff_timer_virtual_conn"
                           " read successful. num of valid timer values: %d",
                           ds707_s101_vctm_info.vctm_config.no_valid_timer_val);

           /** Though # of valid timer vals will be less than or equal to
            *  VCTM_MAX_NUM_TIMER_VAL, we'll print all the timer vals here just
            *  for debug purpose */           
           for (i = 0 ; i < VCTM_MAX_NUM_TIMER_VAL; i++)
           {
             DATA_IS707_MSG2(MSG_LEGACY_HIGH, 
                             "prereg_backoff_timer_virtual_conn[%d] : %d",
                             i, ds707_s101_vctm_info.vctm_config.backoff_timer_val[i]);
           }
	   return;

	 } while ( 0 );

	 DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
			   "using default values for NV prereg_backoff_timer_virtual_conn");

	 ds707_s101_vctm_info.vctm_config.no_valid_timer_val = 
	     VCTM_DEFAULT_NO_TIMER_VAL;
	 ds707_s101_vctm_info.vctm_config.backoff_timer_val[0] = 
	   VCTM_DEFAULT_FIRST_BACKOFF_TIMER_VAL;
	 ds707_s101_vctm_info.vctm_config.backoff_timer_val[1] = 
	   VCTM_DEFAULT_FIRST_BACKOFF_TIMER_VAL;
	 ds707_s101_vctm_info.vctm_config.backoff_timer_val[2] = 
	   VCTM_DEFAULT_FIRST_BACKOFF_TIMER_VAL;
	 ds707_s101_vctm_info.vctm_config.backoff_timer_val[3] = 
	   VCTM_DEFAULT_FIRST_BACKOFF_TIMER_VAL * 2;
	 ds707_s101_vctm_info.vctm_config.backoff_timer_val[4] = 
	   VCTM_DEFAULT_FIRST_BACKOFF_TIMER_VAL * 4;
	 ds707_s101_vctm_info.vctm_config.backoff_timer_val[5] = 
	   VCTM_DEFAULT_FIRST_BACKOFF_TIMER_VAL * 8;

	   return;

}
/*todo: add feature for test*/
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
)
{
	 *temp_vctm_info = ds707_s101_vctm_info;
}

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
void ds707_s101_vctm_set_thrtl_count( uint8 thrtl_count )
{
	 DATA_IS707_MSG2(MSG_LEGACY_HIGH,
			 "S101_VCTM: current throttle count is %d"
			 "setting throttle count to %d", 
			 ds707_s101_vctm_info.thrtl_count,
			 thrtl_count);
	 
	 ds707_s101_vctm_info.thrtl_count = thrtl_count;
}

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
void ds707_s101_vctm_reset_throttle( void )
{
	 DATA_IS707_MSG1(MSG_LEGACY_HIGH,
			 "S101_VCTM: Resetting s101 vctm throttling "
			 "S101_VCTM: current throttle count: %d", 
			 ds707_s101_vctm_info.thrtl_count);

	 if ( ds707_s101_vctm_info.thrtl_count > 0 )
	 {
	   ds707_s101_vctm_info.thrtl_count = 0;
	   DATA_IS707_MSG1(MSG_LEGACY_HIGH,
			   "S101_VCTM: decrementing throttle count to %d", 
			   ds707_s101_vctm_info.thrtl_count);
	 }

	 /*if the timer is running stop it*/
	 if ( rex_get_timer( &ds707_s101_vctm_info.thrtl_timer ) != 0 )
	 {
	    DATA_IS707_MSG0(MSG_LEGACY_HIGH,
			   "S101_VCTM: throttling timer running, freeing it");
	    rex_undef_timer( &ds707_s101_vctm_info.thrtl_timer );

	    /*post throttle indication to s101 SM*/
	    ds707_s101_virt_conn_throttled(FALSE);
	 }
}

/*===========================================================================
FUNCTION DS707_S101_VCTM_PROCESS_CALL_FAILURE
===========================================================================*/
/**
	 This funtion is called when virtual connection bringup fails while in
	 tunnel mode. It increments the throttle count. It starts a throttle timer
	 for value corresponding to no.of throttle count. 

  @param
  None

  @return 
  None
	
  @dependencies 
  None
*/
void ds707_s101_vctm_process_call_failure( void )
{
	 uint8  thrtl_count = 0;
	 uint32 thrtl_timer_value = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	 DATA_IS707_MSG1(MSG_LEGACY_HIGH,
			 "S101_VCTM: processing virtual connection failure "
			 "S101_VCTM: current throttle count: %d", 
			 ds707_s101_vctm_info.thrtl_count);

	 /* start throttling timer only if no of valid timer val is greater than 0
	    if it is 0 that means NV configuration has disabled vctm throttling  */
	 if ( ds707_s101_vctm_info.vctm_config.no_valid_timer_val > 0 )
	 {
	   ds707_s101_vctm_info.thrtl_count++;
	   DATA_IS707_MSG1(MSG_LEGACY_HIGH,
			     "S101_VCTM: incrementing throttle count to %d", 
			     ds707_s101_vctm_info.thrtl_count);
	 
	   if ( ds707_s101_vctm_info.thrtl_count <= ds707_s101_vctm_info.vctm_config.no_valid_timer_val )
	   {
	     thrtl_count = ds707_s101_vctm_info.thrtl_count;
	   }
	   else 
	   {
	     thrtl_count = ds707_s101_vctm_info.vctm_config.no_valid_timer_val;
	   }
	   
	   /*if the timer is running stop it before starting with new value*/
	   if ( rex_get_timer( &ds707_s101_vctm_info.thrtl_timer ) != 0 )
	   {
	      DATA_IS707_MSG0(MSG_LEGACY_ERROR,
			     "S101_VCTM: throttling timer already running, stopping it");
	      (void)rex_clr_timer( &ds707_s101_vctm_info.thrtl_timer );
	   }
	 
	   thrtl_timer_value = 
	     SEC_TO_MSEC(
	       ds707_s101_vctm_info.vctm_config.backoff_timer_val[--thrtl_count]);
	 
	   if ( thrtl_timer_value > 0 )
	   {
	     DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
			      "S101_VCTM: starting throttling timer for %d msec", 
			      thrtl_timer_value );
	     rex_def_timer_ex( &(ds707_s101_vctm_info.thrtl_timer), 
			       ds707_s101_vctm_thrtl_timer_cb, 
			       0);
	     (void)rex_set_timer( &ds707_s101_vctm_info.thrtl_timer, 
				  thrtl_timer_value );
	 
	     /*post throttle indication to s101 SM only if throttling timer is started*/
	     ds707_s101_virt_conn_throttled(TRUE);
	   }
	   else
	   {
	     DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
			      "S101_VCTM: throttling timer value is %d msec,"
			      " no need to start the timer", 
			      thrtl_timer_value );
	   }
	 }
	 else
	 {
	   DATA_IS707_MSG0(MSG_LEGACY_ERROR,
			   "S101_VCTM: throttling not enabled");
	 }
}

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
void ds707_s101_vctm_process_call_success( void )
{
	 DATA_IS707_MSG0(MSG_LEGACY_HIGH,
			 "S101_VCTM: processing virtual connection success");

	 ds707_s101_vctm_reset_throttle();
}

/*===========================================================================
FUNCTION DS707_S101_VCTM_THRTL_TIMER_CB
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
void ds707_s101_vctm_thrtl_timer_cb( unsigned long dummy_user_data)
{
	  DATA_IS707_MSG1(MSG_LEGACY_HIGH,
			 "S101_VCTM: throttle timer expired. Current throttle count: %d", 
			 ds707_s101_vctm_info.thrtl_count);
	  /*post throttle indication to s101 SM*/
	  ds707_s101_virt_conn_throttled(FALSE);
          rex_undef_timer( &ds707_s101_vctm_info.thrtl_timer );
}

#endif /* FEATURE_DATA_OPTHO */
