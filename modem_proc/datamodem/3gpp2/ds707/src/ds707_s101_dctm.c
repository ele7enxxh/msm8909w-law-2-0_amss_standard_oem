/** 
  @file ds707_s101_dctm.c
  @brief 
   Handles LCP and AUTH throttling functionality in s101 tunnel      
   mode.      
*/

/*===========================================================================

                            D S 7 0 7 _ S101 _ DCTM
GENERAL DESCRIPTION
  This file implements LCP and AUTH throttling functionality in s101 tunnel      
   mode.      

EXTERNALIZED FUNCTIONS 
 
INTERNAL FUNCTIONS  


 Copyright (c) 2011-2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_s101_dctm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/13/12   ss      Moving efs write operation to dsutil performed during 
                   bootup.
12/05/11   sc      Created module
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
#include "ds707_s101_dctm.h"
#include "ds707_pkt_mgr.h"
#include "dsutil.h"
#include "ds707_s101.h" 
#include "hdrpac.h"
#include "ds3gcfgmgr.h"

/*===========================================================================
                          LOCAL  DATA DECLARATIONS
===========================================================================*/
/**
This data structure stores throttle count and backoff timer value. It is	
initiated at DS task initialization by reading NV item ...	
*/

static ds707_s101_dctm_info_type ds707_s101_dctm_info;

/*===========================================================================
                         EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/
#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

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
void ds707_s101_dctm_set_thrtl_count( uint8 thrtl_count )
{
#ifdef FEATURE_CALL_THROTTLE
  DATA_IS707_MSG2(MSG_LEGACY_HIGH,
  "S101_DCTM: current throttle count is %d"
  "setting throttle count to %d", 
  ds707_s101_dctm_info.thrtl_count,
  thrtl_count);

  ds707_s101_dctm_info.thrtl_count = thrtl_count;
#else 
  DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                  "ds707_s101_dctm_set_thrtl_count(): "
                  "Call Throttle feature Unsupported. No-Op");
#endif /* FEATURE_CALL_THROTTLE */
}
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
void ds707_s101_dctm_init( void )
{
#ifdef FEATURE_CALL_THROTTLE
  char item_file_path[] = "/nv/item_files/data/3gpp2/prereg_backoff_timer_lcp";
  int retval;
  int i =0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** Initialize the data structure with all 0s */

  memset( &ds707_s101_dctm_info, 0, sizeof(ds707_s101_dctm_info) );

  /** Read the pre-reg backoff timer value from NV */
  retval = ds3gcfgmgr_read_efs_nv_ex(item_file_path,
      &(ds707_s101_dctm_info.dctm_config),
      sizeof(ds707_s101_dctm_info.dctm_config), ds707_curr_subs_id());

  do
  {
  if ( retval != NV_DONE_S )
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
      "NV item: prereg_backoff_timer_lcp read failed,"
      " using default values");
    break;
  }
  if ( ds707_s101_dctm_info.dctm_config.no_valid_timer_val > DCTM_MAX_NUM_TIMER_VAL )
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR, 
        "NV item: prereg_backoff_timer_lcp"
        " read successful but num of valid timer values: %d "
        "is more than max allowed number: %d",
        ds707_s101_dctm_info.dctm_config.no_valid_timer_val,
        DCTM_MAX_NUM_TIMER_VAL);
    break;
  }
  DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                  "NV item: prereg_backoff_timer_lcp"
                  " read successful. num of valid timer values: %d",
                  ds707_s101_dctm_info.dctm_config.no_valid_timer_val);
  /** Though # of valid timer vals will be less than or equal to
   *  DCTM_MAX_NUM_TIMER_VAL, we'll print all the timer vals here just
   *  for debug purpose */
  for (i =0 ; i <DCTM_MAX_NUM_TIMER_VAL; i++)
  {
    DATA_IS707_MSG2(MSG_LEGACY_HIGH, 
                    "prereg_backoff_timer_lcp[%d] : %d",
                    i, ds707_s101_dctm_info.dctm_config.backoff_timer_val[i]);
  }

  return;

  } while ( 0 );

  DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
      "using default values for NV prereg_backoff_timer_lcp");

  ds707_s101_dctm_info.dctm_config.no_valid_timer_val = 
    DCTM_DEFAULT_NO_TIMER_VAL;
  ds707_s101_dctm_info.dctm_config.backoff_timer_val[0] = 
  DCTM_DEFAULT_FIRST_BACKOFF_TIMER_VAL;
  ds707_s101_dctm_info.dctm_config.backoff_timer_val[1] = 
  DCTM_DEFAULT_SECOND_BACKOFF_TIMER_VAL;
  ds707_s101_dctm_info.dctm_config.backoff_timer_val[2] = 
  DCTM_DEFAULT_THIRD_BACKOFF_TIMER_VAL;
  ds707_s101_dctm_info.dctm_config.backoff_timer_val[3] = 
  DCTM_DEFAULT_FOURTH_BACKOFF_TIMER_VAL;
  ds707_s101_dctm_info.dctm_config.backoff_timer_val[4] = 
  DCTM_DEFAULT_FIFTH_BACKOFF_TIMER_VAL;
  ds707_s101_dctm_info.dctm_config.backoff_timer_val[5] = 
  DCTM_DEFAULT_SIXTH_BACKOFF_TIMER_VAL;

  return;
#else 
  DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                  "ds707_s101_dctm_init(): "
                  "Call Throttle feature Unsupported. No-Op");
#endif /* FEATURE_CALL_THROTTLE */
}

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
void ds707_s101_dctm_reset_throttle( void )
{
#ifdef FEATURE_CALL_THROTTLE
  DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                  "S101_DCTM: Resetting s101 dctm throttling");

  DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                  "S101_DCTM: current throttle count: %d", 
                  ds707_s101_dctm_info.thrtl_count);

  if ( ds707_s101_dctm_info.thrtl_count > 0 )
  {
    ds707_s101_dctm_info.thrtl_count = 0;
    DATA_IS707_MSG1(MSG_LEGACY_HIGH,
        "S101_DCTM: decrementing throttle count to %d", 
        ds707_s101_dctm_info.thrtl_count);
  }
 
  /*if the timer is running stop it*/
  if ( rex_get_timer( &ds707_s101_dctm_info.thrtl_timer ) != 0 )
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,
            "S101_DCTM: throttling timer running, freeing it");
    rex_undef_timer( &ds707_s101_dctm_info.thrtl_timer ); 

    /*post throttle indication to s101 SM*/
    ds707_s101_lcp_throttled(FALSE);
  }
  
#else 
  DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                  "ds707_s101_dctm_reset_throttle(): "
                  "Call Throttle feature Unsupported. No-Op");
#endif /* FEATURE_CALL_THROTTLE */	
}

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
)
{
#ifdef FEATURE_CALL_THROTTLE
  uint8  thrtl_count = 0;
  uint32 thrtl_timer_value = 0;
  hdraddr_type subnet;
  hdrerrno_enum_type error_no;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - - */
  DATA_IS707_MSG3(MSG_LEGACY_HIGH, 
                "S101_DCTM:Posting PPP status event: %d protocol: %d reason: %d "
                "in tunnel mode",
                ppp_event,
                protocol,
                ppp_fail_reason);

  /* CHECK IF UE is in tunnel mode if not then return */
  if ( ds707_s101_query_sm_state() == DS707_S101_SM_NON_TUNNEL )
  {
   DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                   "S101_DCTM:Ignore the DCTM PPP report in non tunnel mode");
  return;
  }

  switch(ppp_event)
  {
    /*-----------------------------------------------------------------------
        Post PPP SUCCESS event if call is successful. This means that LCP as
        well as Auth is successful. This will clear the throttle count.
    -----------------------------------------------------------------------*/
    case PPP_UP_EV:
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                      "S101_DCTM: processing LCP and Auth success");

      ds707_s101_dctm_reset_throttle();
     
      /*-----------------------------------------------------------------------
        Query HDR CP to get the current subnet id. Clear DCTM over direct mode
        for this subnet id since the call succeeded in tunnel mode
      -----------------------------------------------------------------------*/
      error_no = hdrpac_get_subnetid( subnet );
      if ( error_no != E_SUCCESS )
      {
        DATA_IS707_MSG1(MSG_LEGACY_ERROR,
                        "S101_DCTM: failed to get subnetid from HDRCP, "
                        "error_no: %d",
                        error_no);
      }
      else
      {
        dctm_clear_ppp_throttle( subnet );
      }
      break;
   }

  /*-----------------------------------------------------------------------
     This event should only be posted  if PPP went down while the call is
     coming UP nto after the call is established.
  -----------------------------------------------------------------------*/

    case PPP_DOWN_EV:
    {
      switch(protocol)
      {
        case DSDCTM_PPP_PROTOCOL_LCP:
        case DSDCTM_PPP_PROTOCOL_EHRPD_AUTH:
        {
          DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                         "S101_DCTM: processing LCP or Auth failure "
                         "S101_DCTM: current throttle count: %d", 
                         ds707_s101_dctm_info.thrtl_count);

          /* start throttling timer only if num of valid timer val is greater than 0
          if it is 0 that means NV configuration has disabled dctm throttling  */
          if ( ds707_s101_dctm_info.dctm_config.no_valid_timer_val > 0 )
          {
            ds707_s101_dctm_info.thrtl_count++;
            DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                            "S101_DCTM: incrementing throttle count to %d", 
                            ds707_s101_dctm_info.thrtl_count);
     
            if ( ds707_s101_dctm_info.thrtl_count <= ds707_s101_dctm_info.dctm_config.no_valid_timer_val )
            {
             thrtl_count = ds707_s101_dctm_info.thrtl_count;
            }
            else 
            {
             thrtl_count = ds707_s101_dctm_info.dctm_config.no_valid_timer_val;
            }
       
            /*if the timer is running stop it before starting with new value*/
            if ( rex_get_timer( &ds707_s101_dctm_info.thrtl_timer ) != 0 )
            {
              DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                 "S101_DCTM: throttling timer already running, stopping it");
              (void)rex_clr_timer( &ds707_s101_dctm_info.thrtl_timer );
            }
     
            thrtl_timer_value = 
              MINUTE_TO_MSEC(
                ds707_s101_dctm_info.dctm_config.backoff_timer_val[--thrtl_count]);
     
            if ( thrtl_timer_value > 0 )
            {
              DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                               "S101_DCTM: starting throttling timer for %d msec", 
                               thrtl_timer_value );
              rex_def_timer_ex( &(ds707_s101_dctm_info.thrtl_timer), 
                                ds707_s101_dctm_thrtl_timer_cb, 
                                0);
              (void)rex_set_timer( &ds707_s101_dctm_info.thrtl_timer, 
                                   thrtl_timer_value );

              /*post throttle indication to s101 SM*/
              ds707_s101_lcp_throttled(TRUE);
            }
            else
            {
              DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                               "S101_DCTM: throttling timer value is %d msec,"
                               " no need to start the timer", 
                               thrtl_timer_value );
            }
          }
          else
          {
          DATA_IS707_MSG0(MSG_LEGACY_ERROR,
          "S101_DCTM: throttling not enabled");
          }
          break;
       }
      
          default:
            DATA_IS707_MSG2(MSG_LEGACY_ERROR, 
                            "S101_DCTM: Unsupported PPP protocol %d for event %d "
                            "not starting the throttling timer",
                            protocol,
                            ppp_event);
          break;
      }
      break;
    }

    default:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                      "S101_DCTM:Unsupported PPP event %d",
                      ppp_event);
    break;
  }
#else 
  DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                  "ds707_s101_dctm_post_ppp_call_status(): "
                  "Call Throttle feature Unsupported. No-Op");
#endif /* FEATURE_CALL_THROTTLE */
}
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
void ds707_s101_dctm_thrtl_timer_cb( unsigned long dummy_user_data)
{
#ifdef FEATURE_CALL_THROTTLE
  DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                  "S101_DCTM: throttle timer expired. Current throttle count: %d", 
                  ds707_s101_dctm_info.thrtl_count);
  /*post throttle indication to s101 SM*/
  ds707_s101_lcp_throttled(FALSE);
  rex_undef_timer( &ds707_s101_dctm_info.thrtl_timer );
#else 
  DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
                  "ds707_s101_dctm_thrtl_timer_cb(): "
                  "Call Throttle feature Unsupported. No-Op");
#endif /* FEATURE_CALL_THROTTLE */
}

#endif /* FEATURE_DATA_OPTHO */

