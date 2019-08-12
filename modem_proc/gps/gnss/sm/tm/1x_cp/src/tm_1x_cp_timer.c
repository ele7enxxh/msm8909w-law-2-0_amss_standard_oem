#include "comdef.h"
#include "customer.h"
#include "msg.h"
#include "event.h"
#include "pdapibuf.h"
#include "paixlate.h"
#include "tm_prtl_iface.h"
#include "sm_nv.h"
#include "sm_dm_event.h"
#include "sm_log.h"
#include "tm_1x_cp.h"
#include "tm_cm_iface.h"
#include "tm_is801_util.h"
#include "tm_1x_cp_internal.h"

/*===========================================================================

FUNCTION tm_1x_cp_handle_sess_timeout

DESCRIPTION
  This function is used to handle Session timeout.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
static void tm_1x_cp_handle_sess_timeout( void )
{
  MSG_HIGH("Handle Sess timeout",0,0,0);
  if ( tm_1x_cp_session_active())
  {
    tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE,TM_STOP_REASON_FIX_TIMEOUT);
  }
  else
  {
    MSG_ERROR("Timer event not handled",0,0,0);
  }
} /* End tm_1x_cp_handle_sess_timeout() */

/*===========================================================================

FUNCTION tm_1x_cp_handle_is801_timeout

DESCRIPTION
  This function is used to handle IS801 timeout.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
static void tm_1x_cp_handle_is801_timeout( void )
{
  MSG_HIGH("Handle IS801 timeout",0,0,0);
  if ( tm_1x_cp_session_active())
  {
    event_report(EVENT_GPS_IS801_TIMEOUT);

    if (tm_1x_cp_session_info.is801_req_wait_bmap)
    {
      tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE,TM_STOP_REASON_IS801_SERVER_TIMEOUT);
    } /* end if wait_bmap */
  }
  else
  {
    MSG_ERROR("Timer event not expected",0,0,0);
  }
} /* End tm_1x_cp_handle_is801_timeout() */

/*===========================================================================

FUNCTION tm_1x_cp_handle_ppm_trigger_timeout

DESCRIPTION
  This function is used to handle PPM trigger timeout.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/
static void tm_1x_cp_handle_ppm_trigger_timeout( void )
{
  MSG_HIGH("Handle get PPM timeout",0,0,0);
  if ( !tm_1x_cp_get_ppm( ) )
  {
    /* Failed to send PPM command to TM-Core
    */
    MSG_ERROR("Unable to Obtain Post-fix PPM",0,0,0);
    tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE, TM_STOP_REASON_GENERAL_ERROR);
  }
} /* End tm_1x_cp_handle_ppm_trigger_timeout() */

/*===========================================================================

FUNCTION tm_1x_cp_handle_ppm_timeout

DESCRIPTION
  This function gets called once PPM timeout has expired.

RETURN VALUE

DEPENDENCIES

===========================================================================*/
static void tm_1x_cp_handle_ppm_timeout(void)
{
  MSG_MED("Handle PPM timeout",0,0,0);
  if (tm_1x_cp_session_active() != TRUE) 
  {
    MSG_ERROR("PPM timeout not expected",0,0,0);
    return;
  }
          
  if(tm_1x_cp_session_info.ppm_timeout == TM_MGP_GET_PPM_TIMEOUT_MIN)
  {
    /* Start timer = MAX timeout */
    tm_1x_cp_session_info.ppm_timeout = TM_MGP_GET_PPM_TIMEOUT_MAX;
    if(!os_TimerStart(tm_1x_cp_session_info.tm_1x_cp_ppm_timer,tm_1x_cp_session_info.ppm_timeout,NULL))
    {
      MSG_ERROR("Failed starting MGP PPM timer",0,0,0);
    }
    MSG_HIGH("Send MSInfo resp",0,0,0);
    if(!i_tm_1x_cp_send_rlink_pddm(0, tm_1x_cp_session_info.pde_version))
    {
      tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE,TM_STOP_REASON_GENERAL_ERROR);
    }
  }
  else
  {
    tm_1x_cp_terminate_session(TM_1X_CP_TERM_SRC_NONE,TM_STOP_REASON_PPM_TIMEOUT);
  }
}

/*===========================================================================


FUNCTION tm_1x_cp_timer_cb_handler


DESCRIPTION

DEPENDENCIES

  None.

RETURN VALUE

SIDE EFFECTS

  None.
===========================================================================*/

void tm_1x_cp_timer_cb_handler
(
  void *timer_data
)
{
    os_TimerExpiryType *timer_param = (os_TimerExpiryType *)timer_data;

    if(timer_param == NULL)
    {
      MSG_ERROR("Can not proess null timer msg", 0, 0, 0);  
      return;
    }

    MSG_HIGH("timerId 0x%x expired", timer_param->q_TimerId, 0, 0);                    
    if((timer_param->q_TimerId & TM_1X_CP_TIMER_DISPATCH_MASK) == 0)
    {
      return;
    }

    switch(timer_param->q_TimerId)
    {
      case TM_1X_CP_SESS_TIMER_ID:
        tm_1x_cp_handle_sess_timeout();
        break;

      case TM_1X_CP_PPM_TIMER_ID:
        tm_1x_cp_handle_ppm_timeout();
        break;

      case TM_1X_CP_IS801_TIMER_ID:
        tm_1x_cp_handle_is801_timeout();
        break;

      case TM_1X_CP_PPM_TRIGGER_TIMER_ID:
        tm_1x_cp_handle_ppm_trigger_timeout();
        break;

      default:
        break;
    }
}

