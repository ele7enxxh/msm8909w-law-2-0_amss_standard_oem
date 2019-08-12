/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                        D A T A   S E R V I C E S

                     E P Z I D   H Y S T E R E S I S

GENERAL DESCRIPTION
  This file contains functions used to handle EPZID Hysteresis feature.

EXTERNALIZED FUNCTIONS

 DS707_EPZID_HYSTERESIS_PROCESS_EVENT
   This function is the Hysteresis event machine. It is the access point 
   for all the external files. It is called whenever there is a change in 
   call state to either going active/dormant or if the signal is set for the 
   timer or BS sends an SOCM to turn on/off hysteresis machine.
   
 DS707_EPZID_HYSTERESIS_PROCESS_HT 
    Function to process the hysteresis timer (HT).
 
 DS707_EPZID_HYSTERESIS_ADD_NEW_EPZID 
    Funciton to be called while in  hysteresis state to add new epzid to 
	the list and also starts a timer on all the existing entries.
 
 
 DS707_EPZID_HYSTERESIS_PRINT_EPZID_LIST
    Funtion which prints the hysteresis list along with the HT timer value.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
    Hysteresis state will be turned on by default.
   Copyright (c) 2003-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_epzid_hyst.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who     what, where, why
--------   ---     --------------------------------------------------------
03/15/11   mg      Global variable cleanup
10/14/10   sa      Adding debug messages before ASSERT.
10/13/10   op      Migrated to MSG 2.0 macros
09/13/10   op      Changed DS707_CBACK_MSG macro to use MSG 2.0 format
03/12/09   ms      Update the HAT timer value with the value read from NV
                   or RUIM card entry.
02/04/09   ss      Offtarget Lint fixes
09/28/08   ms      Reset the epzid hat timer after disconnecting data call.
09/15/08   ms      Pass the correct timer duration to epzid_start_timer.
08/05/08   ms      IS707B addendum Feature support.
05/18/06   gr      Fixed issue with reference to ignoring NEW_NET event
                   due to recent changes to IDM change notification
09/09/05   vrk/gr  EPZID enhancements releated to L2/L3 ack
04/05/05   gr      Initial version of the file to support IS707-B
===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"
#ifdef FEATURE_DS_EPZID_HYSTERESIS
#ifdef FEATURE_DATA_IS707
#include "amssassert.h"
#include "err.h"
#include "msg.h"
#include "data_msg.h"
#include "time_svc.h"
#include "time_tod.h"
#include "ds3gtimer.h"
#include "event.h"
#include "ds707_cback.h"
#include "ds707_epzid.h"
#include "ds707_epzid_hyst.h"
#include "ds707_epzid_util.h"
#include "ds707_pkt_mgr.h"

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

typedef enum
{
 EPZIDI_HYS_NULL_STATE = 0,
 EPZIDI_HYS_INIT_STATE = 1,
 EPZIDI_HYS_HAT_STATE  = 2,
 EPZIDI_HYS_HT_STATE   = 3
}epzidi_hysteresis_state_type;

/*------------------------------------------------------------------------
  Enable epzid hysteresis by default.
-------------------------------------------------------------------------*/
static epzidi_hysteresis_state_type epzidi_hys_state = EPZIDI_HYS_INIT_STATE;

/*-------------------------------------------------------------------------
  When Hysteresis is turned on, the maximum list length after the mobile 
  goes dormant becomes 4 (as per standard).
------------------------------------------------------------------------*/
#define EPZIDI_HYSTERESIS_DORM_LIST_LEN       0x04

/*-------------------------------------------------------------------------
   Hysteresis activation timer (HAT) and hysteresis timer (HT) 
   as defined by the standard in seconds.
-------------------------------------------------------------------------*/
static uint8 ds707_epzid_hysteresis_hat_timer_in_sec = EPZID_HYSTERESIS_HAT_TIMER_IN_SEC;
static uint8 ds707_epzid_hysteresis_ht_timer_in_sec =  EPZID_HYSTERESIS_HT_TIMER_IN_SEC;
#ifdef FEATURE_IS707B_ADDENDUM
static boolean ds707_epzid_hysteresis_hat_timer_is_running = FALSE;
static dword ds707_epzid_hat_start_time = 0;
static uint8 ds707_epzid_nv_hat_timer_in_sec = EPZID_HYSTERESIS_HAT_TIMER_IN_SEC;
#endif /* FEATURE_IS707B_ADDENDUM*/

/*-------------------------------------------------------------------------
 Variable to inform the status of the hysteresis state. By default
 hysteresis is always turned on , hence make it TRUE.
-------------------------------------------------------------------------*/
static boolean ds707_epzid_hysteresis_enabled = TRUE;

/*-------------------------------------------------------------------------
  Hysteresis activation indicator : 
  Make it TRUE, when, user data is Transmitted/Received(call active).
  Make it false, after HAT is started or when call is NULL state.
-------------------------------------------------------------------------*/
static boolean epzidi_hysteresis_hai = FALSE;
static boolean epzidi_timer_started = FALSE;/* hysteresis timer started */
/*===========================================================================
                        INTERNAL FUNCTION DECLARATIONS
===========================================================================*/

static boolean ds707_epzid_hysteresis_process_ht(void);
static void epzidi_hysteresis_start_hat(void);
static void epzidi_hysteresis_stop_hat(void);
static void epzidi_hysteresis_process_data_ready_cmd(void);
static boolean epzidi_hysteresis_process_hysteresis_timers_expired_cmd(void);
void epzidi_hyst_rlp_data_transfer_cb( byte rlp_instance );

/*===========================================================================

MACRO         EPZIDI_START_TIMER

DESCRIPTION    Starts EPZID hysteresis timer and keeps track the information
               of starting the timer.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define EPZIDI_START_TIMER( duration )                                     \
           if( epzidi_timer_started == FALSE)                              \
           {                                                               \
              ds3gi_start_timer( DS3G_TIMER_EPZID_HYSTERESIS,  duration ); \
              epzidi_timer_started = TRUE;                                 \
           }                                                               \

/*===========================================================================
MACRO          EPZIDI_STOP_TIMER

DESCRIPTION    Stops EPZID hysteresis timer only if it has been started.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
#define EPZIDI_STOP_TIMER()                                                \
                          if( epzidi_timer_started )                       \
                          {                                                \
                            ds3gi_stop_timer( DS3G_TIMER_EPZID_HYSTERESIS);\
                            epzidi_timer_started = FALSE;                  \
                          }
/*===========================================================================
                        INTERNAL FUNCTION DEFINTIONS
===========================================================================*/


/*===========================================================================
FUNCTION       EPZIDI_HYSTERESIS_START_HAT

DESCRIPTION    This function starts the hysteresis activation timer for the 
               time specified. It clears the hysteresis activation indicator.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
static void epzidi_hysteresis_start_hat
(
  void
)
{
  uint16 hat_timer_in_sec = 0;
 /*------------------------------------------------------------------------*/  
 
  /*-----------------------------------------------------------------------
  HAT has to be started only in INIT State. For all other states, it is an
  Error. ASSERT if the current state is anything else
 -----------------------------------------------------------------------*/
  if(epzidi_hys_state != EPZIDI_HYS_INIT_STATE)
  {
    DATA_IS707_MSG2(MSG_LEGACY_HIGH, 
                    "Start HAT not allowed in hyst_state = %d : HAI = %d", 
                    epzidi_hys_state, epzidi_hysteresis_hai);
    return;
  }

  /*-------------------------------------------------------------------------
  If there is no user data, then start hysteresis activation timer for 0 secs
  The timer will expire instantanneously and the mobile gets into 
  HT state without any delay. 
  ---------------------------------------------------------------------------*/
  DATA_IS707_MSG3(MSG_LEGACY_MED, 
    "start_hat(), HAI = %d, epzid_list_len = %d, hyst_state = %d",
    epzidi_hysteresis_hai, ds707_epzid_get_epzid_list_len(), epzidi_hys_state);
  /*-------------------------------------------------------------------------
    IF (HAI == TRUE) implies MS sent some data;
    THEN
        set HAT_TIMER to the user defined value
    ELSE
        retain the default value for the HAT_TIMER = 0
  ---------------------------------------------------------------------------*/
  if(epzidi_hysteresis_hai == TRUE)
  {
    hat_timer_in_sec = ds707_epzid_hysteresis_hat_timer_in_sec;
#ifdef FEATURE_IS707B_ADDENDUM
    /*WE SET THIS FLAG only if the HAT is not Zero */ 
    ds707_epzid_hysteresis_hat_timer_is_running = TRUE;
    ds707_epzid_hat_start_time  = time_get_uptime_secs();
#endif /* FEATURE_IS707B_ADDENDUM*/
  }

  /*-------------------------------------------------------------------------
  Before starting hysteresis activation timer, make sure we are in 
  HAT substate.
  Start the timer by making the hysteresis activation indicator false.
  -------------------------------------------------------------------------*/
  epzidi_hysteresis_hai = FALSE;          
  epzidi_hys_state = EPZIDI_HYS_HAT_STATE;

  DATA_IS707_MSG2(MSG_LEGACY_MED, "Starting HAT for %d sec, HAI %d",(hat_timer_in_sec),
            epzidi_hysteresis_hai);
  EPZIDI_START_TIMER( hat_timer_in_sec*1000 );
  event_report(EVENT_PZID_HAT_STARTED);
  
} /* epzidi_hysteresis_start_hat() */

/*===========================================================================

FUNCTION       EPZIDI_HYSTERESIS_STOP_HAT

DESCRIPTION    This function stops the hysteresis activation timer

DEPENDENCIES   None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void epzidi_hysteresis_stop_hat
(
  void
)
{
 /*-----------------------------------------------------------------------*/
  DATA_IS707_MSG3(MSG_LEGACY_MED, 
    "stop_hat(), HAI = %d, epzid_list_len = %d,hyst_state = %d",
    epzidi_hysteresis_hai, ds707_epzid_get_epzid_list_len(), epzidi_hys_state);

  if(epzidi_hys_state == EPZIDI_HYS_HAT_STATE)
  {
    EPZIDI_STOP_TIMER(); 

    epzidi_hys_state = EPZIDI_HYS_INIT_STATE;
#ifdef FEATURE_IS707B_ADDENDUM
    ds707_epzid_hysteresis_hat_timer_is_running = FALSE;
    ds707_epzid_hat_start_time = 0;
    DATA_IS707_MSG0(MSG_LEGACY_MED," Restoring HAT timer to its default value");
    ds707_epzid_hysteresis_hat_timer_in_sec = ds707_epzid_nv_hat_timer_in_sec;
    /* If the IS707b features is turned off we never modify the above global 
       variable hence no need to restore its value */
#endif /* FEATURE_IS707B_ADDENDUM*/
  }

  /* Need to add event for HAT stopped     */
   /* event_report(EVENT_PZID_HAT_STOPPED);  */
  
} /* epzidi_hysteresis_stop_hat() */

/*===========================================================================

FUNCTION       EPZIDI_HYSTERESIS_PROCESS_HAT

DESCRIPTION   Called when the hysteresis activation timer is expired.The
              list lenght is changed from 1 to 4 and goes into HYSTERESIS
			  state.
  
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void epzidi_hysteresis_process_hat
(
  void
)
{
/*-----------------------------------------------------------------------*/
  DATA_IS707_MSG3(MSG_LEGACY_MED, "process_hat(), hyst_enabled = %d, epzid_list_len = %d, hyst_state = %d",
  ds707_epzid_hysteresis_enabled, ds707_epzid_get_epzid_list_len(), epzidi_hys_state); 
    
  /*-------------------------------------------------------------------------
  Cancel timer before processing it.
  -------------------------------------------------------------------------*/
  EPZIDI_STOP_TIMER(); 
  
  /*-----------------------------------------------------------------------
  If the EPZID hysteresis feature was disabled and the timer signal was 
  disabeld make sure we exit without processing as the state is lost
  ------------------------------------------------------------------------*/
  if(ds707_epzid_hysteresis_enabled == FALSE)
  {
   DATA_IS707_MSG0(MSG_LEGACY_HIGH, "PZID Hysteresis feature is disabled");
   return;
  }

  /*----------------------------------------------------------------------
     Hysteresis Activation Timer expired, 
     increase the EPZID List length to 4. 
  -----------------------------------------------------------------------*/
  ds707_epzid_update_max_list_len(EPZIDI_HYSTERESIS_DORM_LIST_LEN);

   
  DATA_IS707_MSG1(MSG_LEGACY_HIGH, "HAT Expired. List : %d",ds707_epzid_get_epzid_max_list_len());
  event_report(EVENT_PZID_HAT_EXPIRED);

} /* epzidi_hysteresis_process_hat() */


/*===========================================================================

FUNCTION      DS707_EPZID_HYSTERESIS_PROCESS_HT

DESCRIPTION   This function processes the hysteresis timer(HT) .Whenever HT 
              expires it removes that EPZID entry from the list. It starts the 
              timer on the last EPZID entry in the list
  
DEPENDENCIES  None

RETURN VALUE  boolean reorig_flag 

SIDE EFFECTS  None
===========================================================================*/
static boolean ds707_epzid_hysteresis_process_ht
(
  void
)
{
  boolean             reorig_flag  = FALSE;
 /*------------------------------------------------------------------------*/  
    
   DATA_IS707_MSG3(MSG_LEGACY_MED, "process_ht(), hyst_enabled = %d, epzid_list_len = %d, hyst_state = %d",
   ds707_epzid_hysteresis_enabled, ds707_epzid_get_epzid_list_len(), epzidi_hys_state); 

  /*-----------------------------------------------------------------------
  If the EPZID hysteresis feature was disabled and the timer signal was 
  disabeld make sure we exit without processing as the state is lost
  ------------------------------------------------------------------------*/
  if(ds707_epzid_hysteresis_enabled == FALSE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "PZID Hysteresis feature is disabled");
    return reorig_flag;
  }
  
  /*-------------------------------------------------------------------------
  Cancel timer before processing it.
  -------------------------------------------------------------------------*/
  EPZIDI_STOP_TIMER();  

  /*-------------------------------------------------------------------------
  Process the Hysteresis Timer on the last EPZID element within the list.
  If the expired EPZID is the same as the current EPZID as seen by the MC,
  process timer sets the reorig_flag = TRUE
  -------------------------------------------------------------------------*/
  reorig_flag = ds707_epzid_process_timer();

  /*-------------------------------------------------------------------------
  Start HT timer only if the EPZID list has more than one entry
  -------------------------------------------------------------------------*/
  if (ds707_epzid_get_epzid_list_len() > 1)
  {
    ds707_epzid_hyst_start_ht();
  }
  /*---------------------------------------------------------------------------
    Now print the list
  ---------------------------------------------------------------------------*/
  ds707_epzid_print_epzid_list();
  return reorig_flag;

} /* ds707_epzid_hysteresis_process_ht() */

/*===========================================================================
FUNCTION      EPZIDI_HYSTERESIS_PROCESS_DATA_READY_CMD

DESCRIPTION   This function is called whenever data is ready to be 
              transmitted/received

DEPENDENCIES  NONE

RETURN VALUE  NONE
SIDE EFFECTS  NONE
===========================================================================*/
static void epzidi_hysteresis_process_data_ready_cmd
(
  void
)
{
  DATA_IS707_MSG3(MSG_LEGACY_MED, "data_ready_cmd(), max_list_len = %d, hyst_enabled = %d, hyst_state = %d", 
  ds707_epzid_get_epzid_max_list_len(), ds707_epzid_hysteresis_enabled, epzidi_hys_state); 

  if( ds707_epzid_hysteresis_enabled == TRUE ) 
  {
	/*-------------------------------------------------------------------------
    Transition to INIT state with parameter as TRUE. The input parameter is a
	boolean flag notifying if data transfer happened
    -------------------------------------------------------------------------*/
    ds707_epzid_hysteresis_init_state(TRUE);
  }
  else
  {
    ds707_epzid_hysteresis_null_state();
  }
} /* epzidi_hysteresis_process_data_ready_cmd() */

/*===========================================================================
FUNCTION 	EPZIDI_HYSTERESIS_PROCESS_HYSTERESIS_TIMERS_EXPIRED_CMD

DESCRIPTION  	This function is called whenever the hysteresis timer signal is
		set. This function  will process the hysteresis timer, HAT/HT
		depending on the hysteresis stae it is in right now.


DEPENDENCIES	NONE

RETURN VALUE  	boolean reorig_flag

SIDE EFFECTS  	NONE
===========================================================================*/
static boolean epzidi_hysteresis_process_hysteresis_timers_expired_cmd
(
  void
)
{
  boolean reorig_flag = FALSE;
 /*-----------------------------------------------------------------------*/
  
  DATA_IS707_MSG3(MSG_LEGACY_MED, "hyst_tmrs_exp(), max_list_len = %d, epzid_list_len = %d, hyst_state = %d",
             ds707_epzid_get_epzid_max_list_len(), ds707_epzid_get_epzid_list_len(), epzidi_hys_state); 
  
  switch(epzidi_hys_state)
  {
     case EPZIDI_HYS_NULL_STATE:
     case EPZIDI_HYS_INIT_STATE:
       DATA_IS707_MSG1(MSG_LEGACY_FATAL, "Never happen expired event:%d",epzidi_hys_state);
       break;
     
     case EPZIDI_HYS_HAT_STATE:
       epzidi_hysteresis_process_hat();
       epzidi_hys_state = EPZIDI_HYS_HT_STATE;
#ifdef FEATURE_IS707B_ADDENDUM
       ds707_epzid_hysteresis_hat_timer_is_running = FALSE;
       ds707_epzid_hat_start_time = 0;
       DATA_IS707_MSG0(MSG_LEGACY_HIGH," Restoring HAT timer to its default value");
       ds707_epzid_hysteresis_hat_timer_in_sec = ds707_epzid_nv_hat_timer_in_sec;
#endif /* FEATURE_IS707B_ADDENDUM*/
       break;

     case EPZIDI_HYS_HT_STATE:
       reorig_flag = ds707_epzid_hysteresis_process_ht();
       epzidi_hys_state = EPZIDI_HYS_HT_STATE;
#ifdef FEATURE_IS707B_ADDENDUM
       ds707_epzid_hysteresis_hat_timer_is_running = FALSE;
       ds707_epzid_hat_start_time = 0;
       DATA_IS707_MSG0(MSG_LEGACY_HIGH," Restoring HAT timer to its default value"); 
       ds707_epzid_hysteresis_hat_timer_in_sec = ds707_epzid_nv_hat_timer_in_sec;
#endif /* FEATURE_IS707B_ADDENDUM*/	 
       break;
    default:
      DATA_ERR_FATAL("Invalid state");
      break;
  }

  return reorig_flag;

}/* epzidi_hysteresis_process_hysteresis_timers_expired_cmd() */

/*===========================================================================
                        EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/


/*===========================================================================
FUNCTION       DS707_EPZID_HYST_DS707_EVENT_HDLR

DESCRIPTION    Event Handler for DS707 Events received  by the Hysteresis 
               engine

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_epzid_hyst_ds707_event_hdlr
(
  ds707_cback_event_type               event,
  const ds707_cback_event_info_type   *event_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(event_info_ptr == NULL)
  {
    DATA_ERR_FATAL("Invalid event_info_ptr passed");
    return;
  }

  if ((event != DS707_CBACK_NEW_NET_EV) &&   
      (event_info_ptr->curr_network == SYS_SYS_MODE_HDR))
  {
    DATA_IS707_MSG1(MSG_LEGACY_MED, "Ignoring DS707 event %d received while on HDR", event);
    return;
  }

  switch(event)
  {
    case DS707_CBACK_PHYS_LINK_COMING_UP_EV:
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "  %d. phy link coming up - "
                      "Reg notify ppp data", ds707_cback_get_nest_level());

      /*----------------------------------------------------------------
      Since Phys link is coming up, register the callback with RLP3 to 
      notify the EPZID hysterisis engine of data transfer. The EPZID
      hysterisis engine will be notified the first time a PPP packet
      is received or sent via RLP3.
      ----------------------------------------------------------------*/
      dsrlp_reg_ppp_data_transfer_notify_func
      (
        DSRLP_FIXED_SR_ID_INDEX,
        epzidi_hyst_rlp_data_transfer_cb
      );
      break;

    case DS707_CBACK_PHYS_LINK_UP_EV:
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "  %d.  phy link UP - stop HAT",
                      ds707_cback_get_nest_level());
      /*---------------------------------------------------------------------
       This should stop the HAT if the it has been running. 
      ---------------------------------------------------------------------*/
      epzidi_hysteresis_stop_hat();
      break;

    case DS707_CBACK_PHYS_LINK_DOWN_EV:
      if (event_info_ptr->going_dormant == TRUE)
      {
        DATA_IS707_MSG1(MSG_LEGACY_HIGH, "  %d.  phy link DOWN dorm - "
                        "start HAT", ds707_cback_get_nest_level());

        /*-------------------------------------------------------------------
         Mobile has gone dormant. Start hysteresis activation timer. This 
         should start the HAT if the hysteresis engine is still in the
         INIT state. Expiry of HAT would grow the EPZID hysteresis list 
        -------------------------------------------------------------------*/
        epzidi_hysteresis_start_hat();

        /*--------------------------------------------------------------
        Since we are going dormant, register the callback with RLP3 to 
        notify the EPZID hysterisis engine of data transfer. The EPZID
        hysterisis engine will be notified the first time a PPP packet
        is received or sent via RLP3.
        --------------------------------------------------------------*/
        dsrlp_reg_ppp_data_transfer_notify_func
        (
          DSRLP_FIXED_SR_ID_INDEX,
          epzidi_hyst_rlp_data_transfer_cb
        );
      }
      else /* going NULL */
      {
        DATA_IS707_MSG1(MSG_LEGACY_HIGH, "  %d.  phy link DOWN null - "
                        "hyst null state", ds707_cback_get_nest_level());
        
        /*-----------------------------------------------------------------
         Force the EPZID Hysteresis Engine back to NULL state after the
         PKT data session goes NULL
        -----------------------------------------------------------------*/
        ds707_epzid_hysteresis_null_state();

        /*--------------------------------------------------------------
        Since we are terminating the data session, deregister the
        callback with RLP3 to notify the EPZID hysterisis engine of 
        data transfer (if it was previously registered).

        Since the the PPP data transfer notify function will be called
        in the Rx task context and we are setting it to NULL here in 
        a lower priority (DS) task context, I don't think we need a 
        TASKLOCK. Add a TASKLOCK here if needed.
        --------------------------------------------------------------*/
        dsrlp_reg_ppp_data_transfer_notify_func
        (
          DSRLP_FIXED_SR_ID_INDEX, NULL
        );
      }
      break;

    case DS707_CBACK_ABORT_SESSION_EV:
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "  %d.  phy link ABORT - "
                      "hyst NULL state", ds707_cback_get_nest_level());
      
      /*---------------------------------------------------------------------
      Force the EPZID Hysteresis Engine back to NULL state after the
      PKT data session goes NULL
      ---------------------------------------------------------------------*/
      ds707_epzid_hysteresis_null_state();

      /*------------------------------------------------------------
      Since we are terminating the data session, deregister the
      callback with RLP3 to notify the EPZID hysterisis engine of 
      data transfer (if it was previously registered).

      Since the the PPP data transfer notify function will be called
      in the Rx task context and we are setting it to NULL here in 
      a lower priority (DS) task context, I don't think we need a 
      TASKLOCK. Add a TASKLOCK here if needed.
      ------------------------------------------------------------*/
      dsrlp_reg_ppp_data_transfer_notify_func(DSRLP_FIXED_SR_ID_INDEX,NULL);
      break;

    case DS707_CBACK_SID_CHANGED_EV:
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "  %d.  SID change - transition "
                      "hyst to NULL state", ds707_cback_get_nest_level());
      ds707_epzid_hysteresis_null_state();
      break;

    case DS707_CBACK_NEW_NET_EV:     
      if(ds707_pkt_is_handoff_from_1x_to_hdr(
       event_info_ptr->curr_network, event_info_ptr->new_network))
      {
         DATA_IS707_MSG1(MSG_LEGACY_HIGH, "  %d. new net HDR - transition "
                         "hyst to NULL state", ds707_cback_get_nest_level());

         ds707_epzid_hysteresis_null_state();
      }
      else if(ds707_pkt_is_handoff_from_hdr_to_1x(
       event_info_ptr->curr_network, event_info_ptr->new_network))
      {
        DATA_IS707_MSG1(MSG_LEGACY_HIGH, "  %d. new net CDMA - transitgion "
                        "hyst to INIT state", ds707_cback_get_nest_level());
        ds707_epzid_hysteresis_init_state(FALSE);
      }
      break;
  
    default:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR,"Unknown event %d",event);
      ASSERT(0);
      break;
  }
} /* ds707_epzid_hyst_ds707_event_hdlr() */

/*===========================================================================
FUNCTION 	DS707_EPZID_HYST_SOCM_ENABLE_HYST

DESCRIPTION	This function is called whenever BS sends SOCM to turn on
		hysteresis. All the hysteresis state variables are initialized.


DEPENDENCIES	NONE

RETURN VALUE	NONE

SIDE EFFECTS	NONE
===========================================================================*/
void ds707_epzid_hyst_socm_enable_hyst
(
  void
)
{
  /*-----------------------------------------------------------------------*/
  ds707_epzid_hysteresis_enabled = TRUE; 
  DATA_IS707_MSG3(MSG_LEGACY_MED, "socm_hyst_en(), max_list_len = %d, epzid_list_len = %d, hyst_state = %d",
   ds707_epzid_get_epzid_max_list_len(), ds707_epzid_get_epzid_list_len(), epzidi_hys_state); 
  switch(epzidi_hys_state)
  {
    case EPZIDI_HYS_NULL_STATE:
      /*-----------------------------------------------------------------
       If SOCM is received with list_len=1 during NULL state, transition
       the Hysteresis Engine to INIT state. The data_tranfer flag is set
       to FALSE. This would just leave the value of HAI as before
       -----------------------------------------------------------------*/
      ds707_epzid_hysteresis_init_state(FALSE);
      break;

    case EPZIDI_HYS_INIT_STATE:
      break;

    case EPZIDI_HYS_HAT_STATE:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Hysteresis Enable -> %d state", epzidi_hys_state);
      break;       

    case EPZIDI_HYS_HT_STATE:
      /*-----------------------------------------------------------------
       If SOCM is received with list_len=1 during HT state, the list size
       shrinks to 1 and also the epzidi_hys_state should be reverted to
       INIT state after canceling the HT timers. Otherwise, the mobile 
       would be left in a weird state hysteresis state with list size as
       1. The list will also never grow to 4
       -----------------------------------------------------------------*/
      ds707_epzid_hysteresis_init_state(FALSE);
      break;

    default:
      DATA_ERR_FATAL("Invalid state");
      break;
  }
} /* ds707_epzid_hyst_socm_enable_hyst() */

/*===========================================================================
FUNCTION 	DS707_EPZID_HYST_SOCM_DISABLE_HYST

DESCRIPTION	This function is called when the BS sends SOCM to disable the
		the hysteresis feature. Whenever hysteresis is disabled the
		hysteresis state variables are reset.

DEPENDENCIES	NONE

RETURN VALUE	NONE

SIDE EFFECTS	NONE
===========================================================================*/
void ds707_epzid_hyst_socm_disable_hyst
(
  void
)
{
  /*-----------------------------------------------------------------------*/
  DATA_IS707_MSG3(MSG_LEGACY_MED, "socm_hys_dis(), max_list_len = %d, epzid_list_len = %d, hyst_state = %d",
   ds707_epzid_get_epzid_max_list_len(), ds707_epzid_get_epzid_list_len(), epzidi_hys_state); 
  
  /*-----------------------------------------------------------------
  If SOCM is received with list_len>1,  Hysteresis is disabled 
  (hysteresis state machine is also reverted to NULL state) and 
  the list size shrinks to 1
  -----------------------------------------------------------------*/
  ds707_epzid_shrink_epzid_list(1);
  ds707_epzid_hysteresis_null_state();
  ds707_epzid_hysteresis_enabled = FALSE; 

}/* ds707_epzid_hyst_socm_disable_hyst() */


/*===========================================================================

FUNCTION      DS707_EPZID_HYSTERESIS_NULL_STATE

DESCRIPTION   Enters null state when packet call is closed or when 
              the EPZID reconnection feature is disabled

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_hysteresis_null_state
(
  void
)
{
  /*-------------------------------------------------------------------------
  Reset the epzid hysteresis variables
  -------------------------------------------------------------------------*/
  DATA_IS707_MSG2(MSG_LEGACY_MED, "null_state(), max_list_len = %d, epzid_list_len = %d",
              ds707_epzid_get_epzid_max_list_len(), ds707_epzid_get_epzid_list_len()); 

  epzidi_hysteresis_hai = FALSE;
  EPZIDI_STOP_TIMER();
  epzidi_hys_state      = EPZIDI_HYS_NULL_STATE;
  
  DATA_IS707_MSG2(MSG_LEGACY_MED, "HAI = %d, hyst_state = %d ",
                epzidi_hysteresis_hai, epzidi_hys_state); 
 #ifdef FEATURE_IS707B_ADDENDUM
       ds707_epzid_hysteresis_hat_timer_is_running = FALSE;
        ds707_epzid_hat_start_time = 0;
    DATA_IS707_MSG0(MSG_LEGACY_HIGH," Setting the Hyst Activation Timer to its default value");
    ds707_epzid_hysteresis_hat_timer_in_sec = ds707_epzid_nv_hat_timer_in_sec;
#endif /* FEATURE_IS707B_ADDENDUM*/
}/* ds707_epzid_hysteresis_null_state() */

/*===========================================================================
FUNCTION 		DS707_EPZID_HYSTERESIS_INIT_STATE

DESCRIPTION		This function is called when Hysteresis SM enters INIT state. 
				The Hys SM enters init state in the following scenarios:
                1. A non-zero PZID is detected as part of EPZID change
                2. A SOCM msg to enable Hysteresis is received  
				3. PPP data transfer using RLP/SDB happens 

                Scenario (3) turns the HAI=TRUE and hence  forces the HAT to 
                be started after 30 secs.

DEPENDENCIES	None

RETURN VALUE	None

SIDE EFFECTS	None
===========================================================================*/
void ds707_epzid_hysteresis_init_state
(
   boolean data_transfer_flag
)
{
/*--------------------------------------------------------------------------*/
  DATA_IS707_MSG3(MSG_LEGACY_MED, "init_state(), max_list_len = %d, epzid_list_len = %d, hyst_state = %d",
             ds707_epzid_get_epzid_max_list_len(), ds707_epzid_get_epzid_list_len(), epzidi_hys_state); 
  /*------------------------------------------------------------------------
  Check to see if EPZID feature is registered before we 
  go to the init state for hysteresis.
  -----------------------------------------------------------------------*/
  if(ds707_epzid_get_epzid_max_list_len() == 0)
  {
    return;
  }

  /*-------------------------------------------------------------------------
  Clears Hysteresis activation timer or Hysteresis timers 
  depending on the state. 
  -------------------------------------------------------------------------*/
  EPZIDI_STOP_TIMER();
    
   
  if(data_transfer_flag)
  { 
    /*----------------------------------------------------------------------
    Updates the Max List Length =1 and shrinks the list accordingly. This 
    also sets the HAI=1 and hence forces the HAT to be 30 secs
    -----------------------------------------------------------------------*/
    ds707_epzid_update_max_list_len(1);
    epzidi_hysteresis_hai          = TRUE;
  }
  epzidi_hys_state               = EPZIDI_HYS_INIT_STATE;
  DATA_IS707_MSG3(MSG_LEGACY_MED, "Init state :HAI = %d, hyst_state = %d, max list length = %d ",
              epzidi_hysteresis_hai, epzidi_hys_state, ds707_epzid_get_epzid_max_list_len()); 
#ifdef FEATURE_IS707B_ADDENDUM
  ds707_epzid_hysteresis_hat_timer_is_running = FALSE;
  ds707_epzid_hat_start_time = 0;
  DATA_IS707_MSG0(MSG_LEGACY_HIGH," Setting HAT timer to its default value");
  ds707_epzid_hysteresis_hat_timer_in_sec = ds707_epzid_nv_hat_timer_in_sec;  
#endif /* FEATURE_IS707B_ADDENDUM*/
} /* ds707_epzid_hysteresis_init_state() */

/*===========================================================================
FUNCTION 	DS707_EPZID_HYST_SEND_CMD

DESCRIPTION	Post a command to DS task whenever a SDB/PPP data tranfer
                happens
                
  
DEPENDENCIES	None

RETURN VALUE	None

SIDE EFFECTS	None
===========================================================================*/
void  ds707_epzid_hyst_send_cmd
(
  ds_cmd_enum_type cmd
)
{
  ds_cmd_type     *cmd_ptr = NULL;               /* DS Task msg            */
 /*------------------------------------------------------------------------*/
  DATA_IS707_MSG3(MSG_LEGACY_MED, "hyst_send_cmd() = %d, max_list_len = %d, hyst_state = %d",cmd,
  ds707_epzid_get_epzid_max_list_len(),  epzidi_hys_state); 

  cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
  if(NULL == cmd_ptr)
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = cmd;
  ds_put_cmd(cmd_ptr);
} /* ds707_epzid_hyst_send_cmd() */

/*===========================================================================
FUNCTION      DS707_EPZID_HYSTERESIS_PROCESS_CMD

DESCRIPTION   This function processes PZID specific commands.

DEPENDENCIES  None.

RETURN VALUE  boolean reorig_flag

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_epzid_hysteresis_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
)
{
  boolean reorig_flag = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ds_cmd_ptr == NULL)
  {
    DATA_ERR_FATAL("Invalid ds_cmd_ptr passed");
    return reorig_flag;
  }

  switch(ds_cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_707_PKT_EPZID_HYS_DATA_READY:
     /*-------------------------------------------------------------------
       Data has become ready, clear all the hysteris state variables.
     -------------------------------------------------------------------- */
     epzidi_hysteresis_process_data_ready_cmd();
     break;

    case DS_CMD_707_PKT_EPZID_HYS_SDB_DATA:
     /*-------------------------------------------------------------------
       SDB data transmitted/received - Leave hysteresis state and restart
       the Hysteresis Activation Timer (HAT)
     --------------------------------------------------------------------*/
     epzidi_hysteresis_process_data_ready_cmd();
     epzidi_hysteresis_stop_hat();
     epzidi_hysteresis_start_hat();
     break;

    case DS_CMD_707_PKT_EPZID_HYSTERESIS_TIMER_EXPIRED:
     /*-------------------------------------------------------------------
       Timer signal set. Process hysteresis hat/ht timer
     --------------------------------------------------------------------*/
     reorig_flag = epzidi_hysteresis_process_hysteresis_timers_expired_cmd();
     break;

    default:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "unknown PZID command %d",
                      ds_cmd_ptr->hdr.cmd_id);
      break;
  }

  return reorig_flag;

} /* ds707_epzid_process_cmd() */

/*===========================================================================
FUNCTION      DS707_EPZID_HYST_RLP_DATA_RCVD_CB

DESCRIPTION   This callback function should be called when a (new) RLP packet
              is sent/received after dormancy. It deregisters itself with
              RLP and notifies the EPZID hysterisis engine of the data
              transfer.

DEPENDENCIES  This callback should be registered with RLP when the data 
              session goes dormant and should be called by RLP when the first 
              PPP packet is sent received after dormancy.

              This function is only applicable or 1X RLP3.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void epzidi_hyst_rlp_data_transfer_cb( byte rlp_instance )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG1(MSG_LEGACY_HIGH, "DS707 EPZID RLP DATA TRANSFER CB - RLP isntance %d",
           rlp_instance);

  /*-------------------------------------------------------------------------
  Sanity check - Since this should only be called for 1X calls, this must
  always be called with the fixed SR_ID index. If this assumption changes
  in the future, remove this ASSERT.
  -------------------------------------------------------------------------*/
  if(rlp_instance != DSRLP_FIXED_SR_ID_INDEX)
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Invalid RLP INSTANCE: %d", rlp_instance);
    ASSERT(0);
    return;
  }

  /*----------------------------------------------------------------
  Add code here if we need to change the processing based on which 
  RLP the packet was received on.

  For now, rlp_instance is always passed as FIXED_SR_ID_INDEX
  ----------------------------------------------------------------*/
  /*----------------------------------------------------------------
  PPP received data from/to RLP after coming out of dormancy.
  Notify the EPZID hysterisis engine of data transfer and 
  deregister this function with RLP3. The function will be
  registered again when the call goes dormant.
  ----------------------------------------------------------------*/
  dsrlp_reg_ppp_data_transfer_notify_func(rlp_instance, NULL);

  ds707_epzid_hyst_send_cmd(DS_CMD_707_PKT_EPZID_HYS_DATA_READY);
} /* epzidi_rlp_data_transfer_cb() */

/*===========================================================================
FUNCTION       DS707_EPZID_HYST_START_HT

DESCRIPTION    This function starts the hysteresis timer on the last 
               EPZID entry in the list

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_epzid_hyst_start_ht
(
  void
)
{
  int                 ht_start_epzid_index;
  ds707_epzid_type    ht_start_epzid;
  dword               time_to_expire_in_sec;
 /*------------------------------------------------------------------------*/  
 
  /*-----------------------------------------------------------------------
  Hysteresis Timer has to be started only in HT State. For all other states, 
  it is an Error. ASSERT if the current state is anything else
  -----------------------------------------------------------------------*/
  if(epzidi_hys_state != EPZIDI_HYS_HT_STATE)
  {
    DATA_IS707_MSG2(MSG_LEGACY_HIGH, "Start HT not allowed in hyst_state = %d : HAI = %d", epzidi_hys_state, 
                                                    epzidi_hysteresis_hai);
    return;
  }

  /*-------------------------------------------------------------------------
  Cancel HT if it is running
  -------------------------------------------------------------------------*/
  EPZIDI_STOP_TIMER();  

  time_to_expire_in_sec = ds707_epzid_get_time_to_expire_in_sec();
  /*------------------------------------------------------------------------- 
   If new expiration time exists start the time. 
  ------------------------------------------------------------------------- */
  DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Starting HT for %d sec", time_to_expire_in_sec);
  EPZIDI_START_TIMER(time_to_expire_in_sec*1000);

  ht_start_epzid_index = ds707_epzid_get_epzid_list_len()-1;

  ds707_epzid_set_ht_expired(ht_start_epzid_index,FALSE);

  if(ht_start_epzid_index >= 0 )
  { 
    ds707_epzid_get_epzid_from_list((byte)(ht_start_epzid_index), &ht_start_epzid);
    event_report_payload( EVENT_PZID_HT_STARTED,sizeof(ht_start_epzid),
                                                        &ht_start_epzid );
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "EPZID List Empty, max_list_len = %d", 
                   ds707_epzid_get_epzid_max_list_len());
    ASSERT(0);
  }
     
} /* ds707_epzid_hyst_start_ht() */

/*===========================================================================
FUNCTION       DS707_EPZID_SET_HYSTERESIS_ENABLED

DESCRIPTION    This function sets the hysteresis_enabled flag

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_epzid_set_hysteresis_enabled(
  boolean hysteresis_enabled
)
{
  ds707_epzid_hysteresis_enabled = hysteresis_enabled;
}

#ifdef FEATURE_IS707B_ADDENDUM
/*===========================================================================
FUNCTION       DS707_EPZID_SET_HAT_HDLR

DESCRIPTION    This function sets the HAT to user defined value

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_epzid_set_hat_hdlr
(
  ds_cmd_type *ds_cmd_ptr
)
{
  ds707_epzid_hat_type  *epz_hat_ptr = NULL;

  if ((ds_cmd_ptr == NULL) || (ds_cmd_ptr->cmd_payload_ptr == NULL))
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,"NULL pointer passed");
    return;
  }

  epz_hat_ptr = (ds707_epzid_hat_type*)(ds_cmd_ptr->cmd_payload_ptr);

    if(ds707_epzid_hysteresis_hat_timer_is_running  == FALSE)
    {
      ds707_epzid_hysteresis_hat_timer_in_sec = epz_hat_ptr->timer_val;
      DATA_IS707_MSG1(MSG_LEGACY_HIGH," set HAT %d",ds707_epzid_hysteresis_hat_timer_in_sec);
      return;
    }    
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH,"Set HAT ioctl issued while HAT is running!");
      EPZIDI_STOP_TIMER();
      ds707_epzid_hysteresis_hat_timer_in_sec = epz_hat_ptr->timer_val;
      DATA_IS707_MSG1(MSG_LEGACY_HIGH," set HAT %d",ds707_epzid_hysteresis_hat_timer_in_sec);
      /*WE SET THIS FLAG only if the HAT is not Zero */ 
      if(ds707_epzid_hysteresis_hat_timer_in_sec != 0)
      {
        ds707_epzid_hysteresis_hat_timer_is_running = TRUE;
        ds707_epzid_hat_start_time  = time_get_uptime_secs();
      }
      DATA_IS707_MSG0(MSG_LEGACY_HIGH,"HAT is re-started with new Value");
      EPZIDI_START_TIMER(ds707_epzid_hysteresis_hat_timer_in_sec*1000);
      event_report(EVENT_PZID_HAT_STARTED);
    }
}
/*===========================================================================
FUNCTION      DS707_EPZID_HYST_REG_SUBSTATE_CB

DESCRIPTION   This function is the callback registered with mccsyobj_system,
              that gets called This call back gets called under MC task space whenever
              mobile station enters the registration access sub state due to 
              timer based registration, distance based registration, parameter change registration
              or use zone registration as registration type 
              
              Sends a command to DS, so that actual processing is done in 
              DS task context.
              
              This function is called from the context of the MC task.
       
              This function follows procedures from IS-707.B sec.2.2.2.1.2.4. 
              fetches the PZID list and check if HT is running for the current PZID. 
              If it is true, then DS will clear the HT and remove the current PZID from the list.
              And finally DS will call ps_physlink_up_cmd() to send DRS=0 origination. 
                                   
              This command handler is executed under the context of the DS task.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_epzid_hyst_reg_substate_cb
(
  word sid,                         /* System   Identifier */
  word nid,                         /* Network  Identifier */
  byte pzid                         /* Pkt Zone Identifier */
)
{
  ds707_epzid_type      curr_epzid;
  int                   list_location     = 0;
  byte                  epzid_list_length = 0;
  ds_cmd_type          *ds_cmd_ptr        = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  curr_epzid.sid = sid;
  curr_epzid.nid = nid;
  curr_epzid.pzid = pzid;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   DATA_IS707_MSG0(MSG_LEGACY_MED,"IS707B_ADD  AT entered REG_ACCESS_SUBSTATE."
                                  " MC Called REG_ACCESS_SUBSTATE_HDLR");
  if(curr_epzid.sid 	== 0 || curr_epzid.nid == 0 ||curr_epzid.pzid == 0)
  {
   DATA_IS707_MSG3(MSG_LEGACY_ERROR,"Invalid EPZID passed: Sid = %d NID = %D, PZID = %d", curr_epzid.sid,curr_epzid.nid,curr_epzid.pzid);
   return FALSE;
  }
   /*-----------------------------------------------------------------------
  If the EPZID hysteresis feature was disabled and return FALSE
  ------------------------------------------------------------------------*/
  if(ds707_epzid_hysteresis_enabled == FALSE)
  {
   DATA_IS707_MSG0(MSG_LEGACY_HIGH,"PZID Hysteresis feature is disabled");
    return FALSE;
  }
  if( epzidi_hys_state == EPZIDI_HYS_HT_STATE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,"IS707B Addendum: AT is in HT state");
    list_location = ds707_epzid_is_epzid_in_list(&curr_epzid);
    epzid_list_length = ds707_epzid_get_epzid_list_len();

    if(list_location != -1 && 
       ds707_epzid_is_ht_expired(list_location))
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH,"HT expired in process already for the EPZID entry found in the list");
      return TRUE;
    }

    if(list_location == -1 )
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH,"IS707B Addendum: But EPZID is not found in the list");
      return FALSE;
    }
    else if(list_location == 0)
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH,"IS707B Addendum: But EPZID is at the top in the list:i.e HT cant run on this EPZID");
      return FALSE;
    }
    else if(list_location  == epzid_list_length -1 )
    {
      ds707_epzid_set_ht_expired(list_location,TRUE);
      /*post a command HYESTERESIS_EXPIRED to DS task;     */
      DATA_IS707_MSG1(MSG_LEGACY_HIGH,"IS707B Addendum: EPZID found at list_location %d, bottom of the list and clean that entry",list_location);       
    }
    else
    {
      ds707_epzid_set_ht_expired(list_location,TRUE);
      DATA_IS707_MSG1(MSG_LEGACY_HIGH,"IS707B Addendum: EPZID found at list_location %d, clean that entry",list_location);
      /*-------------------------------------------------------------------------
         Make the Epzid entry last and post a command to clean up the entry
         FIrst save the Epzid entry in the list that needs to be moved to bottom of the list and
         then move the entries starting from list_location+1 till last entry to their left. 
         Finally copy the Epzid entry to be delted to the last entry, then post a command
      -------------------------------------------------------------------------*/
      ds707_epzidi_make_entry_last(list_location);      
    }
    EPZIDI_STOP_TIMER();

    ds_cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
    if(NULL == ds_cmd_ptr)
    {
      ASSERT(0);
      return FALSE;
    }

    ds_cmd_ptr->hdr.cmd_id = DS_CMD_707_PKT_EPZID_HYSTERESIS_TIMER_EXPIRED;
    ds_put_cmd(ds_cmd_ptr);
    return TRUE;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,"REG_ACCESS_SUBSTATE_HDLR : No HT running");
    return FALSE;
  }
  
}

/*===========================================================================
FUNCTION       DS707_EPZID_SET_NV_HAT_TIMER_IN_SEC

DESCRIPTION    This function sets the _nv_hat_timer_in_sec

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_epzid_set_nv_hat_timer_in_sec
(
  uint8 timer_val
)
{
  ds707_epzid_nv_hat_timer_in_sec = timer_val;
}

/*===========================================================================
FUNCTION       DS707_EPZID_GET_HYSTERESIS_HAT_TIMER_IS_RUNNING

DESCRIPTION    This function returns TRUE if epzid_hysteresis hat timer is 
               running.  

DEPENDENCIES   None

RETURN VALUE   boolean

SIDE EFFECTS   None
===========================================================================*/
boolean ds707_epzid_get_hysteresis_hat_timer_is_running(void)
{
  return ds707_epzid_hysteresis_hat_timer_is_running;
}

/*===========================================================================
FUNCTION       DS707_EPZID_GET_HAT_START_TIME

DESCRIPTION    This function returns value of ds707_epzid_hat_start_time

DEPENDENCIES   None

RETURN VALUE   dword

SIDE EFFECTS   None
===========================================================================*/
dword ds707_epzid_get_hat_start_time(void)
{
  return ds707_epzid_hat_start_time;
}

#endif /* FEATURE_IS707B_ADDENDUM*/

/*===========================================================================
FUNCTION       DS707_EPZID_HYSTERESIS_SET_HT_TIMER_IN_SEC

DESCRIPTION    This function sets value of ds707_epzid_hysteresis_ht_timer_in_sec

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void ds707_epzid_hysteresis_set_ht_timer_in_sec(
  uint8 timer_val
)
{
  ds707_epzid_hysteresis_ht_timer_in_sec = timer_val;
}

/*===========================================================================
FUNCTION       DS707_EPZID_HYSTERESIS_GET_HT_TIMER_IN_SEC

DESCRIPTION    This function returns value of ds707_epzid_hysteresis_ht_timer_in_sec

DEPENDENCIES   None

RETURN VALUE   uint8

SIDE EFFECTS   None
===========================================================================*/
uint8 ds707_epzid_hysteresis_get_ht_timer_in_sec(void)
{
  return ds707_epzid_hysteresis_ht_timer_in_sec;
}

/*===========================================================================
FUNCTION       DS707_EPZID_HYSTERESIS_SET_HAT_TIMER_IN_SEC

DESCRIPTION    This function sets value of ds707_epzid_hysteresis_hat_timer_in_sec

DEPENDENCIES   None

RETURN VALUE   uint8

SIDE EFFECTS   None
===========================================================================*/
void ds707_epzid_hysteresis_set_hat_timer_in_sec(
  uint8 timer_val
)
{
  ds707_epzid_hysteresis_hat_timer_in_sec = timer_val;
}

/*===========================================================================
FUNCTION       DS707_EPZID_HYSTERESIS_GET_HAT_TIMER_IN_SEC

DESCRIPTION    This function returns value of ds707_epzid_hysteresis_hat_timer_in_sec

DEPENDENCIES   None

RETURN VALUE   uint8

SIDE EFFECTS   None
===========================================================================*/
uint8 ds707_epzid_hysteresis_get_hat_timer_in_sec(void)
{
  return ds707_epzid_hysteresis_hat_timer_in_sec;
}


#endif /* FEATURE_DATA_IS707 */
#endif /* FEATURE_DS_EPZID_HYSTERESIS */

