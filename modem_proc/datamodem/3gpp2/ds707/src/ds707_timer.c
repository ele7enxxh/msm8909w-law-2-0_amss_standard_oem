/*===========================================================================

                        D S 7 0 7 _ T I M E R
GENERAL DESCRIPTION
  This file contains the functionality for the IS-707 specific timers.  These
  are:
    The holddown timer
    The inactivitiy timer (which lets phone go dormant in a packet data call).

EXTERNALIZED FUNCTIONS
  ds707_timer_cb()
    Registered as the callback for the holddown and idle timers.  Based on 
    which timer expired, sends msg to DS task, letting it know of timer 
    expiration.
    
 ds707_timer_init()
   Registers the holddown and idle timer with DS task.  This creates the
   timer in REX and associates the callback with it.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Must call ds707_timer_init() on startup.

 Copyright (c) 2002-2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_timer.c_v   1.1   19 Nov 2002 18:41:34   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_timer.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/08/12   ssb     Dun Call Entitlement support using PS_SYS Interface
10/13/10   op      Migrated to MSG 2.0 macros
05/10/07   ac      Lint fix
01/10/07   sq      Added ds707_retry_delay_orig_timer
04/05/05   gr      Integrated EPZID functionality onto the main line
03/22/04   gr      Added abort dormant ppp timer (gonull timer)
10/01/03   ak      Added pzid delay timer.
09/02/03   sy      Added pzid hysteresis timer.
11/17/02   ak      Updated file header comments.
02/20/02   ak      First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "amssassert.h"
#include "ds3gtimer.h"
#include "dstask.h"
#include "ds707.h"
#include "ds707_timer.h"
#include "err.h"
#include "data_msg.h"
#include "rex.h"
#include "ds707_so_retrydel.h"
#include "ps_sys_ioctl.h"


/*===========================================================================
                            VARIABLES
===========================================================================*/

/*---------------------------------------------------------------------------
   Clocks
---------------------------------------------------------------------------*/  

LOCAL rex_timer_type        ds707_cta_timer;         /* idle timeout timer */
LOCAL rex_timer_type        ds707_holddown_timer;    /* holddown timer     */
LOCAL rex_timer_type        ds707_abort_dormant_ppp_timer;/*ppp abort timer*/
LOCAL rex_timer_type        ds707_dun_timer;              /*  DUN CTRL Timer  */

#ifdef FEATURE_DS_EPZID_HYSTERESIS
/*-------------------------------------------------------------------------
  Timer used for hysteresis activation timer\ hysteresis timer .
  -------------------------------------------------------------------------*/
LOCAL rex_timer_type        ds707_epzid_hyst_timer;   /* pzid hysteresis    */
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
LOCAL rex_timer_type        ds707_epzid_delay_timer;  /* pzid delay timer   */
LOCAL rex_timer_type        ds707_retry_delay_orig_timer;
#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
LOCAL rex_timer_type        ds707_pkt_auto_redial_timer;
#endif /* FEATURE_DEDICATED_C2K_MODE_DATA */

/*===========================================================================
                           INTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_TIMER_CB

DESCRIPTION   Callback for the AT+CTA ,holddown timer expiration or
              PZID Hysteresis tiemr expiration.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_timer_cb
(
  unsigned long    timer_id
)
{
  ds_cmd_type                         *cmd_ptr                = NULL;   
  ds707_retry_delay_orig_type         *retry_delay_orig_ptr   = NULL;
  ps_sys_ioctl_dun_ctrl_response_type *dun_ctrl_resp_ptr      = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch ((ds3g_timer_enum_type)timer_id)
  {
    case DS3G_TIMER_CTA:
    case DS3G_TIMER_HOLDDOWN:
#ifdef FEATURE_DS_EPZID_HYSTERESIS
     case DS3G_TIMER_EPZID_HYSTERESIS:
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
    case DS3G_TIMER_EPZID_DELAY:
#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
    case DS3G_TIMER_PKT_AUTO_REDIAL:
#endif /* FEATURE_DEDICATED_C2K_MODE_DATA */
    case DS3G_TIMER_ABORT_DORMANT_PPP:
      cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO);
      if(NULL == cmd_ptr)
      {
        ASSERT(0);
        return;
      }
      break;

    case DS3G_TIMER_RETRY_DELAY_ORIG:
      cmd_ptr = ds_allocate_cmd_buf(sizeof(ds707_retry_delay_orig_type));
      if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
      {
        ASSERT(0);
        return;
      }
      break;

    case DS3G_TIMER_DUN_CTRL_ENTL:
      cmd_ptr = ds_allocate_cmd_buf(sizeof(ps_sys_ioctl_dun_ctrl_response_type));
      if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
      {
        ASSERT(0);
        return;
      }
      break;

    default:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Bad timer id on callback %d",
                      timer_id);
      ASSERT(0);
      return;
  }

  switch ((ds3g_timer_enum_type)timer_id)
  {
    case DS3G_TIMER_CTA:
      cmd_ptr->hdr.cmd_id = DS_CMD_707_PKT_CTA_TIMER_EXPIRED;
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "AT+CTA expired... go dormant (1X)");
      break;

    case DS3G_TIMER_HOLDDOWN:
      cmd_ptr->hdr.cmd_id = DS_CMD_707_PKT_HOLDDOWN_TIMER_EXPIRED;
      break;

#ifdef FEATURE_DS_EPZID_HYSTERESIS
     case DS3G_TIMER_EPZID_HYSTERESIS:
      cmd_ptr->hdr.cmd_id = DS_CMD_707_PKT_EPZID_HYSTERESIS_TIMER_EXPIRED;
      break;
#endif /* FEATURE_DS_EPZID_HYSTERESIS */

    case DS3G_TIMER_EPZID_DELAY:
      cmd_ptr->hdr.cmd_id = DS_CMD_707_PKT_EPZID_DELAY_TIMER_EXPIRED;
      break;

    case DS3G_TIMER_RETRY_DELAY_ORIG:
      cmd_ptr->hdr.cmd_id = DS_CMD_707_RETRY_DELAY_ORIG;
      retry_delay_orig_ptr 
                  = (ds707_retry_delay_orig_type *)(cmd_ptr->cmd_payload_ptr);
      retry_delay_orig_ptr->delay_infinite = FALSE;
      retry_delay_orig_ptr->delay = 0; /*delay =0 means delay expires*/
      break;

#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
    case DS3G_TIMER_PKT_AUTO_REDIAL:
      cmd_ptr->hdr.cmd_id =   DS_CMD_707_PKT_AUTO_REDIAL_TIMER_EXPIRED;
      break;
#endif

    case DS3G_TIMER_ABORT_DORMANT_PPP:
      cmd_ptr->hdr.cmd_id = DS_CMD_707_ABORT_DORMANT_PPP_TIMER_EXPIRED;
      break;

    case DS3G_TIMER_DUN_CTRL_ENTL:
      cmd_ptr->hdr.cmd_id = DS_CMD_707_RMSM_ENTITLEMENT_CHECK_COMPLETE;

      dun_ctrl_resp_ptr 
        = (ps_sys_ioctl_dun_ctrl_response_type *)(cmd_ptr->cmd_payload_ptr);
      dun_ctrl_resp_ptr->cb_data = 0;
      dun_ctrl_resp_ptr->allow_call = TRUE;
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                      "DUN ENTL Timer expires, progress with Dun Call");
      break;

    default:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Bad timer id on callback %d",
                      timer_id);
      ASSERT(0);
      return;
  }
  
  ds_put_cmd( cmd_ptr );
}


/*===========================================================================
                           EXTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_TIMER_INIT

DESCRIPTION   Register 707 timers with 3G DSMGR Timer services.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_timer_init(void)
{
  ds3gi_timer_register(&ds707_cta_timer,
                       ds707_timer_cb,
                       DS3G_TIMER_CTA,
                       DS3G_TIMER_TYPE_REX_TIMER);

  ds3gi_timer_register(&ds707_holddown_timer,
                       ds707_timer_cb,
                       DS3G_TIMER_HOLDDOWN,
                       DS3G_TIMER_TYPE_REX_TIMER);

#ifdef FEATURE_DS_EPZID_HYSTERESIS
  ds3gi_timer_register(&ds707_epzid_hyst_timer,
                       ds707_timer_cb,
                       DS3G_TIMER_EPZID_HYSTERESIS,
                       DS3G_TIMER_TYPE_REX_TIMER);
#endif /* FEATURE_DS_EPZID_HYSTERESIS */
  
  ds3gi_timer_register(&ds707_epzid_delay_timer,
                       ds707_timer_cb,
                       DS3G_TIMER_EPZID_DELAY,
                       DS3G_TIMER_TYPE_REX_TIMER);

  ds3gi_timer_register(&ds707_retry_delay_orig_timer,
                       ds707_timer_cb,
                       DS3G_TIMER_RETRY_DELAY_ORIG,
                       DS3G_TIMER_TYPE_REX_TIMER);

#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
  ds3gi_timer_register(&ds707_pkt_auto_redial_timer,
                       ds707_timer_cb,
                       DS3G_TIMER_PKT_AUTO_REDIAL,
                       DS3G_TIMER_TYPE_REX_TIMER);
#endif

  ds3gi_timer_register(&ds707_abort_dormant_ppp_timer,
                       ds707_timer_cb,
                       DS3G_TIMER_ABORT_DORMANT_PPP,
                       DS3G_TIMER_TYPE_REX_TIMER);

  ds3gi_timer_register(&ds707_dun_timer,
                       ds707_timer_cb,
                       DS3G_TIMER_DUN_CTRL_ENTL,
                       DS3G_TIMER_TYPE_REX_TIMER);
  
} /* ds707_timer_init() */


#endif /* FEATURE_DATA_IS707 */

