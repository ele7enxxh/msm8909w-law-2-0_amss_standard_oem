/*===========================================================================

                          D S M I P _ S O L S M . C

DESCRIPTION

 The Data Services Mobile IP Agent Solicitation State Machine source file.
 This state machine takes care of soliciting the network for Mobility
 agents.

EXTERNALIZED FUNCTIONS
   mip_sol_sm_init()
     Initialize the Solicitation State Machine.
   mip_sol_sm_post_event()
     Post an event to the Solicitation State Machine.

Copyright (c) 2000-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_solsm.c_v   1.18   10 Dec 2002 23:12:40   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/src/dsmip_solsm.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/12/13    sd     Cryptographic vulnerability - replacing ran_next APIs.
05/04/11    ttv    Added changes to compute mip statistics.
02/24/11    ash    Cleaning up global variables for Q6 free floating effort.
10/07/10    ttv    Added messages which explain the reason for asserting.
02/02/04    jd     Removed flow contrl (handled by interface state now)
01/20/04    jd     Removed dependencies on ps_mip_compat.h
01/20/04    jd     Mainlined Multimode code (multimode only now)
09/23/03    jd     Limit max ASMs based on value in ssmi_info block.
                   Initialize max ASM retries to SSM_INITIAL_SOL_RETRIES (1)
12/10/02    jd     Added mip_sol_sm_active so regsm can determine if mobile
                   is awaiting AAM.
08/15/02    jay    Randomization of solicitation intervals uses pseudo random numbers,
                   instead of QDSP( For Taz only)
07/29/02    jd     Was soliciting indefinitely, now stop after 2 ASM's
11/21/01    mvl    Code review changes, including changes to timings - only
                   one retry and randomization of the intervals.
11/14/01    ss     Added function mip_sol_sm_get_random_byte()
10/24/01    js     Modified to support dsmip_info structure
09/16/01    js     Added header comment block for externalized functions. 
07/06/01    mvl    Added processing for Iface Starting event - shortened
                   delay time before we start soliciting to 300ms from 1s
06/27/01    mvl    Shorted solicitiation retry time if doing S.IP fallback
06/14/01    mvl    Updated to reflect latest design
11/29/00    mvl    Created module
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DS_MOBILE_IP
#include "amssassert.h"
#include "err.h"
#include "msg.h"
#include "data_msg.h"
#include "pstimer.h"
#include "ran.h"

#include "dsmip_metasm.h"
#include "dsmip_outmsg.h"
#include "dsmip_solsm.h"
#include "dsmip_regsm.h"
#include "dsntp.h"

#include "dsat707mipctab.h"
#include "ps_ppp.h"
#include "dsmip_v.h"
#include "ps_utils.h"

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                            INTERNAL DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
#define SSMI_SOL_RETRY_DELAY  500  /* retry delay.  With randomization the
                                      range = 500-755 ms                   */
#define SSMI_SOL_DELAY_TIME   250  /* delay before sending solicitation,
                                      w/ randomization range = 250-555 ms  */
#ifdef ASSERT
static boolean ssmi_inited = FALSE;
#endif


#ifndef T_ARM
/*---------------------------------------------------------------------------
  These are used for the off target test environment.
  These need to be kept in sync with the enums which they describe!
---------------------------------------------------------------------------*/
static char *ssmi_state_names[SSMI_MAX_STATE] =
{
  "Null",
  "Waiting to Solicit",
  "Send Solicitation"
};

static char *ssmi_event_names[SSM_MAX_EV] =
{
  "Solicitation Trigger",
  "Immediate Retry",
  "RXed Agent Advert",
  "Exit M.IP",
  "Immediate Solicitation",
  "Timer Expired",
  "Solicitation Failed"
};
#endif /* ! T_ARM */

/*---------------------------------------------------------------------------
  The state machine information
---------------------------------------------------------------------------*/
static ssmi_info_type ssmi_info;

/*===========================================================================
                            Forward Declarations
===========================================================================*/
void ssmi_timer_cb
(
  void *param
);

void ssmi_transition_state
(
  ssmi_state_type new_state  
);



/*lint -save -e641 Reason for suppressing error 641*/

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                      EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION MIP_SOL_SM_INIT()

DESCRIPTION
  This function initializes the agent solicitation state machine.

  THIS SHOULD ONLY BE CALLED AT POWER UP.

PARAMETERS
  None

RETURN VALUE
  0 on success
 -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int mip_sol_sm_init
(
  void
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Grab a PS timer that will be used for the solicitation timeouts.
  -------------------------------------------------------------------------*/
  ssmi_info.timer = ps_timer_alloc(ssmi_timer_cb, NULL);
  if(ssmi_info.timer == PS_TIMER_FAILURE)
  {
    DATA_ERR_FATAL("Failed to allocate PS timer for SSM!");
    return -1;
  }

  /*-------------------------------------------------------------------------
    Set the retry count and the initial state to their initial values
  -------------------------------------------------------------------------*/
  ssmi_info.state = SSMI_NULL_STATE;
  MIP_SOL_SM_SET_MAX_RETRIES( SSM_INITIAL_SOL_RETRIES );

#ifdef ASSERT
  ssmi_inited = TRUE;
#endif

  return 0;

} /* mip_sol_sm_init() */



/*===========================================================================
FUNCTION MIP_SOL_SM_POST_EVENT()

DESCRIPTION
  This function posts an event to the Agent solicitation state machine.

PARAMETERS
  event: The event that is being posted to the state machine.

RETURN VALUE
  None

DEPENDENCIES
  mip_sol_sm_init() has to have been called first.

SIDE EFFECTS
  None
===========================================================================*/
void mip_sol_sm_post_event
(
  mip_sol_sm_event_type event
)
{
  ps_timer_error_type timer_result;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(ssmi_inited == TRUE);

#ifndef T_ARM
  DATA_MIP_MSG2(MSG_LEGACY_MED, "'%s' event posted to Sol SM in state '%s'.",
          ssmi_event_names[event],
          ssmi_state_names[ssmi_info.state]);
#else
  DATA_MIP_MSG2(MSG_LEGACY_MED, "Event %d posted to Sol SM. in state %d",
          event,
          ssmi_info.state);
#endif

  /*-------------------------------------------------------------------------
    Take action based on event received and which state we are currently in.
  -------------------------------------------------------------------------*/
  switch(event)
  {
    /*-----------------------------------------------------------------------
                             SSM_SOL_TRIGGER_EV

      Solicitation Trigger Event
      NULL/SEND_SOL: go to WAIT_TO_SOL_STATE
      WAIT_TO_SOL_STATE: Ignore
    -----------------------------------------------------------------------*/
  case SSM_SOL_TRIGGER_EV:
    switch(ssmi_info.state)
    {
    case SSMI_NULL_STATE:
    case SSMI_SEND_SOL_STATE:
      ssmi_transition_state(SSMI_WAIT_TO_SOL_STATE);
      break;

    case SSMI_WAIT_TO_SOL_STATE:
      DATA_MIP_MSG2(MSG_LEGACY_MED, "Ignoring %d in state %d", event, ssmi_info.state);
      /* ignore */
      break;

    default:
	  DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid ssmi state %d", 
                        ssmi_info.state);
      ASSERT(0);

    } /* switch(state) */
    break;


    /*-----------------------------------------------------------------------
                             SSMI_TIMER_EXP_EV

      Timer Expired Event
      SEND_SOL/WAIT_TO_SOL_STATE:set retry count to 0 and go to Sol Send
                                 State
      NULL: INVALID event for that state - ASSERT
    -----------------------------------------------------------------------*/
  case SSMI_TIMER_EXP_EV:
    switch(ssmi_info.state)
    {
    case SSMI_SEND_SOL_STATE:
    case SSMI_WAIT_TO_SOL_STATE:
      ssmi_transition_state(SSMI_SEND_SOL_STATE);
      break;

    case SSMI_NULL_STATE:
    default:
	  DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid ssmi state %d", 
                        ssmi_info.state);
      ASSERT(0);

    } /* switch(state) */
    break;

    /*-----------------------------------------------------------------------
                             SSMI_IMMED_SOL_EV

      Timer Expired Event
      All states: set retry count to 0 and go to Sol Send State
    -----------------------------------------------------------------------*/
  case SSMI_IMMED_SOL_EV:
    switch(ssmi_info.state)
    {
    case SSMI_SEND_SOL_STATE:
    case SSMI_WAIT_TO_SOL_STATE:
    case SSMI_NULL_STATE:
      ssmi_info.retry_cnt = 0;
      ssmi_transition_state(SSMI_SEND_SOL_STATE);
      break;

    default:
	  DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid ssmi state %d", 
                        ssmi_info.state);	
      ASSERT(0);

    } /* switch(state) */
    break;

    /*-----------------------------------------------------------------------
                             SSM_RXED_AGT_ADV_EV
                               SSM_EXIT_MIP_EV

      Rxed Agent Advertisement, Exit M.IP and Iface starting Events
      Go to the NULL state or, if already there, ignore.
    -----------------------------------------------------------------------*/
  case SSM_RXED_AGT_ADV_EV:
  case SSM_EXIT_MIP_EV:
    switch(ssmi_info.state)
    {
    case SSMI_NULL_STATE:
      DATA_MIP_MSG1(MSG_LEGACY_MED, "Got event %d in NULL state!", event);
      /* ignore */
      break;

    case SSMI_SEND_SOL_STATE:
    case SSMI_WAIT_TO_SOL_STATE:
      ssmi_transition_state(SSMI_NULL_STATE);
      break;

    default:
      DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid ssmi state %d", 
                    ssmi_info.state);			
      ASSERT(0);

    } /* switch(state) */
    break;
    

    /*-----------------------------------------------------------------------
                             SSMI_SOL_FAILED_EV

      Solicitation Failed Event
      NULL/WAIT_TO_SOL_STATE: INVALID event for these states ASSERT 
      SEND SOL: Post Sol Failed event to all RegSMs and goto NULL
    -----------------------------------------------------------------------*/
  case SSMI_SOL_FAILED_EV:
    switch(ssmi_info.state)
    {

    case SSMI_SEND_SOL_STATE:
      mip_reg_sm_post_event_all(RSM_SOL_FAILED_EV);
      ssmi_transition_state(SSMI_NULL_STATE);
      break;

    case SSMI_NULL_STATE:
    case SSMI_WAIT_TO_SOL_STATE:
    default:
      DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid ssmi state %d", 
                    ssmi_info.state);	
      ASSERT(0);

    } /* switch(state) */
    break;


    /*-----------------------------------------------------------------------
                             SSM_IMMED_RETRY_EV

      Immediate Retry Event
      NULL/WAIT_TO_SOL_STATE: ignore
      SEND_SOL: decrement the retry count, cancel the timer, and reenter the
      SEND_SOL State
    -----------------------------------------------------------------------*/
  case SSM_IMMED_RETRY_EV:
    switch(ssmi_info.state)
    {

    case SSMI_SEND_SOL_STATE:
      ASSERT(ssmi_info.retry_cnt != -1);
      ssmi_info.retry_cnt -= 1;
      timer_result = ps_timer_cancel(ssmi_info.timer);
      ASSERT(timer_result != PS_TIMER_FAILURE); 
      ssmi_transition_state(SSMI_SEND_SOL_STATE);
      break;

    case SSMI_NULL_STATE:
    case SSMI_WAIT_TO_SOL_STATE:
      DATA_MIP_MSG2(MSG_LEGACY_MED, "Ignoring %d event in %d state.\n",
              event,
              ssmi_info.state);
      /* ignore */
      break;

      /*---------------------------------------------------------------------
                                   default

        This should NEVER happen!
      ---------------------------------------------------------------------*/
    default:
      DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid ssmi state %d", 
                    ssmi_info.state);	
      ASSERT(0);

    } /* switch(state) */
    break;


    /*-----------------------------------------------------------------------
                                   default

      This should NEVER happen!
    -----------------------------------------------------------------------*/
  default:
    DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid event posted to sol SM %d", 
                  event);	
    ASSERT(0); 
  } /* switch(event) */

} /* mip_sol_sm_post_event() */

/*===========================================================================
FUNCTION MIP_SOL_SM_ACTIVE()

DESCRIPTION
  Checks if the solicitation state machine is running

PARAMETERS
  None

RETURN VALUE
  TRUE  if solsm is active
  FALSE if solsm is not active

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mip_sol_sm_active
(
  void
)
{
  return (ssmi_info.state == SSMI_NULL_STATE ? FALSE : TRUE);
} /* mip_sol_sm_active() */


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                          LOCAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION SSMI_TIMER_CB()

DESCRIPTION
  This function is the callback for the expiration of the solicitation
  timer.  It posts the solicitation timeout event to the state machine.

PARAMETERS
  param: this is ignored (should be passed in as NULL)

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ssmi_timer_cb
(
  void *param
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(param == NULL);
  mip_sol_sm_post_event(SSMI_TIMER_EXP_EV);

} /* ssmi_timer_cb() */



/*===========================================================================
FUNCTION SSMI_TRANSITION_STATE()

DESCRIPTION
  This function effectuates the transition to the state that is passed in

PARAMETERS
  new_state: the state to transition to.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ssmi_transition_state
(
  ssmi_state_type new_state  
)
{
  ps_timer_error_type timer_result;
  uint16 timeout;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifndef T_ARM
  DATA_MIP_MSG2(MSG_LEGACY_MED, "Sol SM from '%s' to '%s' state",
          ssmi_state_names[ssmi_info.state],
          ssmi_state_names[new_state]
          );
#else
  DATA_MIP_MSG2(MSG_LEGACY_MED, "Sol SM from %d to %d state", ssmi_info.state, new_state);
#endif

  /*-------------------------------------------------------------------------
    Perform action associated with entering into this state - actions to be
    performed after entering the state are below.
  -------------------------------------------------------------------------*/
  switch(new_state)
  {
    /*-----------------------------------------------------------------------
                               SSMI_NULL_STATE

      Cancel the timer
    -----------------------------------------------------------------------*/
  case SSMI_NULL_STATE:
    timer_result = ps_timer_cancel(ssmi_info.timer);
    ASSERT(timer_result != PS_TIMER_FAILURE);
    break;


    /*-----------------------------------------------------------------------
                            SSMI_WAIT_TO_SOL_STATE

      Reset retry counter, and start timer.
    -----------------------------------------------------------------------*/
  case SSMI_WAIT_TO_SOL_STATE:
    ssmi_info.retry_cnt = 0;
    timeout = SSMI_SOL_DELAY_TIME + mip_sol_sm_get_random_byte(); 
    timer_result = ps_timer_start(ssmi_info.timer, timeout);
    DATA_MIP_MSG1(MSG_LEGACY_MED, "Set sol delay time to %d.", timeout);
    ASSERT(timer_result != PS_TIMER_FAILURE); 
    break;


    /*-----------------------------------------------------------------------
                             SSMI_SEND_SOL_STATE

      There are no pre state change events for this state.
    -----------------------------------------------------------------------*/
  case SSMI_SEND_SOL_STATE:
    break;


    /*-----------------------------------------------------------------------
                                   default

      This should NEVER happen!
    -----------------------------------------------------------------------*/
  default:
    DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid new state for Sol SM %d", 
                  new_state);	
    ASSERT(0);

  } /* switch( new state ) */

  /*-------------------------------------------------------------------------
                            Enter into new state
  -------------------------------------------------------------------------*/
  ssmi_info.state = new_state;

  /*-------------------------------------------------------------------------
    Perform actions after state has been entered.
  -------------------------------------------------------------------------*/
  switch(new_state)
  {
    /*-----------------------------------------------------------------------
                               SSMI_NULL_STATE
                            SSMI_WAIT_TO_SOL_STATE

      There is nothing to do here.
    -----------------------------------------------------------------------*/
  case SSMI_NULL_STATE:
  case SSMI_WAIT_TO_SOL_STATE:
    break;


    /*-----------------------------------------------------------------------
                             SSMI_SEND_SOL_STATE

      Check whether the max retry count has been exceeded, if so post
      Solicitation Failed event to ourselves.
    -----------------------------------------------------------------------*/
  case SSMI_SEND_SOL_STATE:
    if(ssmi_info.retry_cnt <= ssmi_info.max_retries)
    {
      mip_outmsg_send_sol();

      /*---------------------------------------------------------------------
        Backoff the solicitation timeout to allow for slow networks.

        Our algorithm timeline looks like this:

        SSMI_SOL_DELAY_TIME  = 250
        SSMI_SOL_RETRY_DELAY = 500
  
        r = a random value from 0-255

        Time since PPP up    retry_cnt    action
        ------------------   ---------    --------------
        t0 = 0                  0         (wait for AAM)
        t1 = t0 + 250 + r       0         Send ASM
        t2 = t1 + 500 + r       1         Send ASM
        t3 = t2 + 500 + r       2         During call setup,
                                           Send SSMI_SOL_FAILED_EV 
                                          Else
                                           Send ASM
        t4 = t3 + 500 + r       3         Send ASM
        ...                     ...       (Send ASM's)
        t21 = t20 + 500 + r     20        During handoff, 
                                           Send SSMI_SOL_FAILED_EV 

        So if the PDSN is dead or doesn't support MIP, the max time the MN 
        waits is (1250 + 3r) ms during call setup (1.25 - 2.15 s), or
                 (10250 + 20r) ms during handoff (10.25 - 15.35 s)
      ---------------------------------------------------------------------*/
      timeout = SSMI_SOL_RETRY_DELAY + mip_sol_sm_get_random_byte();
      timer_result= ps_timer_start(ssmi_info.timer, timeout);
      DATA_MIP_MSG1(MSG_LEGACY_MED, "Set retry time to %d.", timeout);
      ASSERT(timer_result != PS_TIMER_FAILURE);
      ssmi_info.retry_cnt += 1;
      mip_increment_mip_stat_info(SOL_CNT_ID);
    }
    else
    {
      DATA_MIP_MSG0(MSG_LEGACY_HIGH, "Agent solicitation failed");
      mip_sol_sm_post_event(SSMI_SOL_FAILED_EV);
    }
    break;


    /*-----------------------------------------------------------------------
                                   default

      This should NEVER happen!
    -----------------------------------------------------------------------*/
  default:
    DATA_MIP_MSG1(MSG_LEGACY_ERROR, "Invalid new state for Sol SM %d", 
                  new_state);	
    ASSERT(0);

  } /* switch( new state ) */

} /* ssmi_transition_state() */


/*===========================================================================
FUNCTION MIP_SOL_SM_GET_RANDOM_BYTE()

DESCRIPTION
  This function will return a random value between 0 and 255.

PARAMETERS
  None

RETURN VALUE
  uint8: A random value in the range of 0..255.

DEPENDENCIES
  As a pre-condition, this function assumes that ran_seed() has been called
  at least once. Currently, ran_seed() is called by the main control task 
  as part of initialization.

SIDE EFFECTS
  None
===========================================================================*/
uint8 mip_sol_sm_get_random_byte()
{
  uint8 delay;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    We use the Most Significant Byte because it is more likely to be random
    than the Least Significant Byte.
  -------------------------------------------------------------------------*/
  (void) ps_utils_generate_rand_num((void*)&delay, sizeof(delay));
  return delay;

} /* mip_sol_sm_get_random_byte() */

/*lint -restore Restore lint error 641*/

/*============================================================================
FUNCTION MIP_GET_SSMI_INFO()

DESCRIPTION
  Access function for ssmi_info.

PARAMETERS
  None

RETURN VALUE
  pointer to ssmi_info.

DEPENDENCIES
  None

============================================================================*/
ssmi_info_type* mip_get_ssmi_info_ptr(void)
{
  return &ssmi_info;
}
#endif /* FEATURE_DS_MOBILE_IP */
