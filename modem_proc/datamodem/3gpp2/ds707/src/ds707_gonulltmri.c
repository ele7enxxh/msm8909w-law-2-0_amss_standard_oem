/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                  G O  N U L L  T I M E R  C O N T R O L
                       
GENERAL DESCRIPTION
  This file contains functions used to handle the abort dormant ppp timer. This 
  functionality is also known as the go null timer.  It aborts the dormant ppp 
  as soon as the go null timer expires. 

EXTERNALIZED FUNCTIONS

  DS707_GONULLTMRI_START
    Interface to start the go null timer

  DS707_GONULLTMRI_STOP
    Interface to stop the go null timer

  DS707_GONULLTMRI_INIT
    Registers a callback for all the events to be handled by gonull timer
    module 

  DS707_GONULLTMRI_PROCESS_CMD
    Processes commands specific to the go null timer.    

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 2004-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_gonulltmri.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/30/14   niv     Added wrapper functions & API change for 
                   NV_REFRESH 
04/18/12   jz      Seperate old JCDMA and Simplified JCDMA 
03/15/11   mg      Global variable cleanup
10/13/10   op      Migrated to MSG 2.0 macros
09/13/10   op      Changed DS707_CBACK_MSG macro to use MSG 2.0 format
05/20/10   ms      Set Go_null_timer running to False when GO NULL timer expires.
11/10/09   as      Cleared Windows Compiler warnings for MOB builds
03/13/09   ss      Added event reporting for GoNull timer whenever it is 
                   started, stopped or has expired.
11/18/08   sn      Fixed issue of GO NULL TIMER value not getting set properly 
                   for DCM. Also GO NULL TIMER value will not be read from NV 
                   items anymore for DCM.
08/07/08   as      Lint error fixes.
07/25/08   hs      Fixed compilation warnings
07/02/07   sc      Replaced ds707_gonulltmri_tear_down_iface() with 
                   ps_iface_go_null_cmd().
05/10/07   ac      Lint fix
03/08/07   ara     Changed FEATURE_JCDMA_2 to FEATURE_JCDMA_1X 
10/13/05   az      Support for IPv6
11/14/06   rsj     Created two values for gonull timer based on JCDMA requirement.
                   1 Value used for 1x Session timeout and second value used for
                   DO session timeout.
11/07/05   vrk     Call ps_iface_tear_down_cmd multiple times to tear down 
                   ppp if multiple apps are running
07/18/05   vrk     replace ps_iface_tear_down_cmd with ps_flow_go_null_ind 
                   and ps_phys_link_gone_ind when session ends
03/18/04    gr     Initial creation of the module
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "ds3gtimer.h"
#include "ds707_cback.h"
#include "ds707_pkt_mgri.h"
#include "ds707_gonulltmri.h"
#include "ds3gcfgmgr.h"
#include "dssocket.h"
#include "dstask.h"
#include "dstaski.h"
#include "msg.h"
#include "data_msg.h"
#include "ds707_event_defs.h"
#include "ds707_gonulltmr.h"
#include "ds707_roaming.h"
#include "ds707_pdn_context.h"
#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
#include "ps_iface.h"
#include "ds707_rmsm.h"
#include "ds707_pkt_mgr.h"
#include "ps_phys_link.h"
#include "ds_flow_control.h"
#endif

#include "data_err_suppr.h"

/*===========================================================================
                        TYPEDEFS AND VARIABLES
===========================================================================*/

/*---------------------------------------------------------------------------
       GO NULL Timer value For Do and 1x (In Seconds)
---------------------------------------------------------------------------*/
static uint32 ds707_gonulltmri_val;

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*---------------------------------------------------------------------------
  State of the timer
---------------------------------------------------------------------------*/
static boolean gonulltmri_is_running = FALSE;

/*===========================================================================
                        INTERNAL FUNCTION DEFINTIONS
===========================================================================*/
#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
/*===========================================================================
FUNCTION      DS707_GONULLTMRI_CAN_CLEANUP_NETMDL_UM_PPP

DESCRIPTION   This is a function to check AT is in netmodel call and graceful
              PPP close flag is set or not.
             

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_gonulltmri_can_cleanup_netmdl_um_ppp(ps_iface_type *iface_ptr)
{
  if (
      (( ps_iface_phys_link_state(iface_ptr) == PHYS_LINK_UP ) ||
      ( PS_IFACE_GET_GRACEFUL_CLOSE(iface_ptr) == TRUE ) )
      &&
      ( ds707_rmsm_is_netmdl_call_up() == TRUE )
     )
  {
    return (TRUE);
  }
  else
  {
    return (FALSE);
  }

}
#endif /* FEATURE_DEDICATED_C2K_MODE_DATA */

/*===========================================================================
                         EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_GONULLTMRI_START

DESCRIPTION   This is a function to start the ABORT_DORMANT_PPP timer. 
             

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_gonulltmri_start(void)
{

  /* Set to zero so that if we are not on CDMA or HDR then we do not start a timer */
  uint32 gonull_timer = 0;
  ds707_gonull_timer_struct  ds707_gonull_timer_event_report;

  //if (!ds707_pkt_mgr_check_dcm_jcdma_allowed())
  /*Simplified JCDMA:
    5.1.6 Use PPP close timer for socket calls  [D1-No.90]*/
  if(!ds707_roaming_is_curr_mode_jcdma() &&
     !ds707_roaming_is_simplified_jcdma_mode())
  {
    return;
  }

#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
  /*---------------------------------------------------------------------
      If FEATURE_DEDICATED_C2K_MODE is defined then add a check to see if 
      AT is in Hybrid ON mode. If AT is in Hybrid ON mode only then we need 
      to start the GoNull timer.
      Hybrid mode == OFF means AT is in either 1X only or DO only mode.
      In both these cases we will not start the timer.
  ---------------------------------------------------------------------*/
  if (ds707_pkt_mgr_get_current_hybrid_pref() == FALSE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
                    "AT not in Hybrid mode. Do not start Go Null timer");
  }
  else
#endif
  {
    if (ds707_get_gonulltmri_is_running_flag() == FALSE)
    {
      /* Based on KDDI Requirement for Session Close timer,
         we determine what mode we are in(1x or DO). Based on mode
         we start a timer with the required value set. */

      /* When handoff occurs then JCDMA builds will do a PPP resync.*/
      if(ds707_extif_is_pkt_data_sess_on_cdma())
      {
        gonull_timer = ds707_gonulltmr_get_timer(DS707_GONULLTMRI_1X);
      }
      else if(ds707_extif_is_pkt_data_sess_on_hdr())
      {
        gonull_timer = ds707_gonulltmr_get_timer(DS707_GONULLTMRI_DO);
      }
      else 
      {
        /* We should not get here */
        DATA_IS707_MSG0(MSG_LEGACY_ERROR, "The Current Data Session is neither HDR nor 1X");
        gonull_timer = ds707_get_gonulltmri_val();
      }
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Start Close Dormant PPP Timer %d seconds", gonull_timer);

      /* Convert the gonull_timer into milli seconds to set the timer value */
      gonull_timer *= MILLISECS_PER_SECOND;
      if (gonull_timer > 0)
      {
        ds707_set_gonulltmri_is_running_flag(TRUE);
        ds3gi_start_timer(DS3G_TIMER_ABORT_DORMANT_PPP, gonull_timer);
        ds707_gonull_timer_event_report.timer_state = DS707_GONULL_EVENT_TIMER_START;
        ds707_gonull_timer_event_report.timer_value   = (dword)gonull_timer;
        event_report_payload(EVENT_DS_GO_NULL_TIMER, 
                             sizeof(ds707_gonull_timer_struct),
                             (void *)(&ds707_gonull_timer_event_report));
      }
    }
  }
} /* ds707_gonulltmri_start() */

/*===========================================================================
FUNCTION      DS707_GONULLTMRI_STOP

DESCRIPTION   This function stops the ABORT_DORMANT_PPP timer

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_gonulltmri_stop(void)
{
  ds707_gonull_timer_struct  ds707_gonull_timer_event_report;

  /*Simplified JCDMA:
    5.1.6 Use PPP close timer for socket calls  [D1-No.90]*/
  if (!ds707_pkt_mgr_check_dcm_jcdma_allowed() &&
      !ds707_roaming_is_simplified_jcdma_mode())
  {
    return;
  }

  if (ds707_get_gonulltmri_is_running_flag() == TRUE)
  {
    ds707_set_gonulltmri_is_running_flag(FALSE);
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Stop Close Dormant PPP Timer");
    ds3gi_stop_timer(DS3G_TIMER_ABORT_DORMANT_PPP);
    ds707_gonull_timer_event_report.timer_state = DS707_GONULL_EVENT_TIMER_STOP;
    ds707_gonull_timer_event_report.timer_value = 0;
    event_report_payload(EVENT_DS_GO_NULL_TIMER, 
                         sizeof(ds707_gonull_timer_struct),
                        (void *)(&ds707_gonull_timer_event_report));
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Close Dormant PPP Timer already stopped");
  }
} /* ds707_gonulltmri_stop() */

/*===========================================================================
FUNCTION       DS707_GONULLTMRI_EVENT_CBACK

DESCRIPTION    Callback when various phone events occur.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_gonulltmri_event_cback
(
  ds707_cback_event_type               event,
  const ds707_cback_event_info_type   *event_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG2(MSG_LEGACY_HIGH, "ds707_cback_event %d, nest_level  %d", 
                  event, ds707_cback_get_nest_level());


  switch(event)
  {
    case DS707_CBACK_SID_CHANGED_EV:
      ds707_gonulltmri_stop();
      break;

    case DS707_CBACK_PHYS_LINK_UP_EV:
      #ifdef FEATURE_DEDICATED_C2K_MODE_DATA
      ds707_gonulltmr_restore_timer_values();
      #endif

      ds707_gonulltmri_stop();
      break;

    case DS707_CBACK_PHYS_LINK_DOWN_EV:
      if (event_info_ptr->going_dormant == TRUE)
      {
        ds707_gonulltmri_start();
      }
      else /* going NULL */
      {
        ds707_gonulltmri_stop();
      }
      break;

    case DS707_CBACK_ABORT_SESSION_EV:
      ds707_gonulltmri_stop();
      break;

    case DS707_CBACK_NEW_NET_EV:
      if(ds707_pkt_is_handoff_from_1x_to_hdr(event_info_ptr->curr_network, event_info_ptr->new_network) ||
        ds707_pkt_is_handoff_from_hdr_to_1x(event_info_ptr->curr_network, event_info_ptr->new_network))     
      {
        ds707_gonulltmri_stop();
      }
      break;

    default:
      break;
  }

} /* ds707_gonulltmri_event_cback() */

/*===========================================================================
FUNCTION      DS707_GONULLTMRI_INIT

DESCRIPTION   Called at startup

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_gonulltmri_init(void)
{
  uint16                gonulltmri_default_val;
#ifndef FEATURE_DEDICATED_C2K_MODE_DATA
  nv_item_type         local_ds_nv_item;
  nv_stat_enum_type    nv_status;
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
  /* Set the timer to 2 hrs */
  gonulltmri_default_val = 7200;
#else
  /*Simplified JCDMA:
    5.1.6 Use PPP close timer for socket calls  [D1-No.90]*/
  if (ds707_roaming_is_curr_mode_jcdma() ||
      ds707_roaming_is_simplified_jcdma_mode())
  {
    /* Set the timer to 45 mins */
    gonulltmri_default_val = 2700;
  }
  else
  {
    gonulltmri_default_val = 0;
  }
#endif /* FEATURE_DEDICATED_C2K_MODE_DATA */

  ds707_set_gonulltmri_val(gonulltmri_default_val);

#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
  ds707_gonulltmr_set_timer(DS707_GONULLTMRI_DO, gonulltmri_default_val);
  ds707_gonulltmr_set_timer(DS707_GONULLTMRI_1X, 0);
#else
  ds707_gonulltmr_set_timer(DS707_GONULLTMRI_DO, gonulltmri_default_val);
  ds707_gonulltmr_set_timer(DS707_GONULLTMRI_1X, gonulltmri_default_val);

  /* Initialize the go null timer(s) from NV
   */
  /*Simplified JCDMA:
    5.1.6 Use PPP close timer for socket calls  [D1-No.90]*/
  if (ds707_roaming_is_curr_mode_jcdma() ||
      ds707_roaming_is_simplified_jcdma_mode())
  {
    nv_status = ds3gcfgmgr_read_legacy_nv_ex(NV_DS707_GO_NULL_TIMER_1X_I, &local_ds_nv_item, ds707_curr_subs_id());
    if(nv_status == NV_DONE_S)
    {
      ds707_gonulltmr_set_timer(DS707_GONULLTMRI_1X,local_ds_nv_item.ds707_go_null_timer_1x);
    }
    else
    {
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "ds707_gonulltmri_1x_val NOT found in NV, using value %d", 
               ds707_gonulltmr_get_timer(DS707_GONULLTMRI_1X));
    }

    nv_status = ds3gcfgmgr_read_legacy_nv_ex(NV_DS707_GO_NULL_TIMER_DO_I, &local_ds_nv_item, ds707_curr_subs_id());
    if(nv_status == NV_DONE_S)
    {
      ds707_gonulltmr_set_timer(DS707_GONULLTMRI_DO, local_ds_nv_item.ds707_go_null_timer_do);
    }
    else
    {
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "ds707_gonulltmri_do_val NOT found in NV, using value %d", 
              ds707_gonulltmr_get_timer(DS707_GONULLTMRI_DO));
    }
  }
#endif

  (void)ds707_cback_register(
                        DS707_CBACK_SID_CHANGED_EV,
                        ds707_gonulltmri_event_cback
                      );

  (void)ds707_cback_register(
                        DS707_CBACK_PHYS_LINK_UP_EV,
                        ds707_gonulltmri_event_cback
                      );

  (void)ds707_cback_register(
                        DS707_CBACK_PHYS_LINK_DOWN_EV,
                        ds707_gonulltmri_event_cback
                      );

  (void)ds707_cback_register(
                        DS707_CBACK_ABORT_SESSION_EV,
                        ds707_gonulltmri_event_cback
                      );

  (void)ds707_cback_register(
                        DS707_CBACK_NEW_NET_EV,
                        ds707_gonulltmri_event_cback
                      );


} /* ds707_gonulltmri_init() */


/*===========================================================================
FUNCTION      DS707_GONULLTMRI_PROCESS_CMD

DESCRIPTION   Processes the go null timer commands.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_gonulltmri_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
)
{
  ps_iface_type      *iface_ptr;
  
  uint8 curr_iface_index = 0;
  int16              ret_val;
  int16              ps_errno;
  ds707_gonull_timer_struct  ds707_gonull_timer_event_report;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MDM_ARG_NOT_CONST(ds_cmd_ptr);

  if(NULL == ds_cmd_ptr)
  {
    DATA_ERR_FATAL("Invalid ds_cmd_ptr passed");
    return;
  }

  switch(ds_cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_707_ABORT_DORMANT_PPP_TIMER_EXPIRED:
    {
      if ((ds707_gonulltmr_get_timer(DS707_GONULLTMRI_DO) > 0) || 
          (ds707_gonulltmr_get_timer(DS707_GONULLTMRI_1X) > 0) || 
          (ds707_get_gonulltmri_val() > 0))
      {
        ds707_set_gonulltmri_is_running_flag(FALSE);

        DATA_IS707_MSG0(MSG_LEGACY_HIGH, "GO NULL timer expired");
        ds707_gonull_timer_event_report.timer_state = DS707_GONULL_EVENT_TIMER_EXPIRY;
        ds707_gonull_timer_event_report.timer_value = 0;
        event_report_payload(EVENT_DS_GO_NULL_TIMER, 
                             sizeof(ds707_gonull_timer_struct),
                             (void *)(&ds707_gonull_timer_event_report));

#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
        ds707_gonulltmr_restore_timer_values();
#endif /*  FEATURE_DEDICATED_C2K_MODE_DATA */


        for ( curr_iface_index = 0; 
              curr_iface_index < DS707_MAX_IFACES; 
              curr_iface_index++ )
        {      
          iface_ptr = NULL ;
          iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(curr_iface_index);
          #ifdef FEATURE_DEDICATED_C2K_MODE_DATA
          /*-------------------------------------------------------------------
            Check if Phys Link is Down (Dormant)or Abort flag is false and 
            if it is a networkmodel call. If this is TRUE then we need to do 
            ppp_stop. ppp_stop is not needed here in case of relay model calls,
              sockets calls and MIP calls.
          -------------------------------------------------------------------*/
          if (ds707_gonulltmri_can_cleanup_netmdl_um_ppp(iface_ptr))
          {
            if ((DS_FLOW_CTRL_IS_BIT_SET(
                 ps_phys_link_get_flow_mask(PS_IFACE_GET_PHYS_LINK(iface_ptr)),
                 DS_FLOW_NO_SERVICE_MASK)) != TRUE)
            {
              DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Calling PPP stop on UM device");
              (void)ppp_stop(PPP_UM_SN_DEV, PPP_PROTOCOL_LCP);
            }

            else
            {
              DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Calling PPP abort on UM device");
              (void)ppp_abort(PPP_UM_SN_DEV);
            }
          }
          else
          #endif/*FEATURE_DEDICATED_C2K_MODE_DATA*/
          
          if (ps_iface_phys_link_state(iface_ptr) != PHYS_LINK_DOWN) 
          {
            DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Phys link not down, aborting process close dormant PPP timer");
          }
          else
          {
            /*-----------------------------------------------------------------
              When go null timer expires instead of calling tear down command
              multiple times, we will call ps_iface_go_null_cmd. This will
              cause ref count of iface to goto zero and hence the PPP will be
              torn down.
            -----------------------------------------------------------------*/
            ret_val = ps_iface_go_null_cmd ( iface_ptr, &ps_errno, NULL );
            if ((ret_val < 0) && (ps_errno != DS_EWOULDBLOCK))
            {
              DATA_IS707_MSG1(MSG_LEGACY_ERROR, "iface go null fail, errno: %d", ps_errno);
              break;
            }
          }
        }/* for each iface */  
      } 
      break;
    }
    default:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Unknown cmd %d", ds_cmd_ptr->hdr.cmd_id);
      break;
  }
} /* ds707_gonulltmri_process_cmd() */

/*===========================================================================
FUNCTION      DS707_GET_GONULLTMRI_IS_RUNNING_FLAG

DESCRIPTION   Returns value of gonulltmri_is_running flag.

DEPENDENCIES  None

RETURN VALUE  Boolean

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_get_gonulltmri_is_running_flag( void )
{
  return gonulltmri_is_running;
}

/*===========================================================================
FUNCTION      DS707_SET_GONULLTMRI_IS_RUNNING_FLAG

DESCRIPTION   Sets value of gonulltmri_is_running flag.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_set_gonulltmri_is_running_flag(
  boolean gonulltmri_is_running_flag 
)
{
  DATA_IS707_MSG1(MSG_LEGACY_MED, 
                  "Setting value of gonulltmri_is_running to %d", 
                  gonulltmri_is_running_flag);
  gonulltmri_is_running = gonulltmri_is_running_flag;
}

/*===========================================================================
FUNCTION      DS707_GET_GONULLTMRI_VAL

DESCRIPTION   Returns value of ds707_gonulltmri_val

DEPENDENCIES  None

RETURN VALUE  uint32

SIDE EFFECTS  None
===========================================================================*/
uint32 ds707_get_gonulltmri_val( void )
{
  return ds707_gonulltmri_val;
}

/*===========================================================================
FUNCTION      DS707_SET_GONULLTMRI_VAL

DESCRIPTION   Sets value of ds707_gonulltmri_val

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_set_gonulltmri_val(
  uint32 gonulltmri_val
)
{
  DATA_IS707_MSG1(MSG_LEGACY_MED, 
                  "Setting value of ds707_gonulltmri_val to %d", 
                  gonulltmri_val);
  ds707_gonulltmri_val = gonulltmri_val;
}
