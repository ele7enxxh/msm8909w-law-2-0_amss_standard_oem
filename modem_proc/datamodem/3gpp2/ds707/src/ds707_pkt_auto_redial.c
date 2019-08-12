/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                    P A C K E T   A U T O   R E D I A L

GENERAL DESCRIPTION
                   This file contains functionality that is needed to redial
the call if the data call is preempted by CM before it get to the initializing
of the PPP. If the call is interrupted after PPP start happens, then the
PPP configuration parameters take care of bringing up the call. If the data
call is interrupted before PPP start happens, then this module will automatically
redial the call.
  
EXTERNALIZED FUNCTIONS
 ds707_pkt_auto_redial_init() - Registers for Physical link up/down events.

 ds707_pkt_auto_redial_process_cmd - Processes the auto redial timer
                                       expired command.
 ds707_pkt_auto_redial_process_cmd - Starts the Auto Redial timer when
                                    the data call is interrupted by CM.


  
  Copyright (c) 2008-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_pkt_auto_redial.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/15/11   mg       Global variable cleanup
10/13/10   op       Migrated to MSG 2.0 macros
02/15/09   ls       Initialize epzid in ds3g_pkt_call_info_type
01/22/08   sy       Initial revsion.


===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"
#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
#include "ps_phys_link.h"
#include "ds3gtimer.h"
#include "ds707_cback.h"
#include "ds707_pkt_auto_redial.h"
#include "ds707_pkt_mgri.h"
#include "dstask.h"
#include "data_msg.h"


/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/
static boolean ds707_pkt_auto_redial_timer_started;

/*---------------------------------------------------------------------------
  Packet Data AUTO REDIAL Timer value (in milliseconds).
---------------------------------------------------------------------------*/
static word auto_redial_timeri_val = DS707_AUTO_REDIAL_TIMER_DURATION_MS;
static uint8 auto_redial_retry_attempts = DS707_AUTO_REDIAL_TIMER_ITERATIONS;

/*===========================================================================
                        INTERNAL FUNCTION DEFINTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_AUTO_REDIAL_TIMERI_RESET_VAL

DESCRIPTION   This function resets the auto_redial timer to it's default value.
              
DEPENDENCIES  NONE

RETURN VALUE  NONE

SIDE EFFECTS  None
===========================================================================*/
static __inline void ds707_pkt_auto_redial_timeri_reset_val(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "DCS: Reset  auto redial timer values");
  auto_redial_timeri_val = DS707_AUTO_REDIAL_TIMER_DURATION_MS;
  auto_redial_retry_attempts = DS707_AUTO_REDIAL_TIMER_ITERATIONS;
} /* ds707_auto_redial_timeri_reset_val() */


/*===========================================================================
FUNCTION      DS707_PKT_AUTO_REDIAL_TIMERI_CLEANUP

DESCRIPTION   Stops the auto redial timer if it is already started.
              Timer value and the retry attempts are reset to their
              initialization values.

DEPENDENCIES  NONE

RETURN VALUE  NONE

SIDE EFFECTS  None
===========================================================================*/
static __inline void ds707_pkt_auto_redial_timeri_cleanup(void)
{
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                  "DCM : Stop auto redial timer if it is started = %d", 
                  ds707_pkt_auto_redial_timer_started);

  if (ds707_pkt_auto_redial_timer_started == TRUE)
  {
    ds707_pkt_auto_redial_timer_started = FALSE;
    ds3gi_stop_timer(DS3G_TIMER_PKT_AUTO_REDIAL);
    ds707_pkt_auto_redial_timeri_reset_val();

  }
} /* ds707_pkt_auto_redial_timeri_stop() */



/*===========================================================================
FUNCTION       DS707_AUTO_REDIAL_TIMERI_EVENT_CBACK

DESCRIPTION    The function is called when Physical link goes up/down.
               The timer is stopped and the values are reset to their
               original values.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
static void ds707_pkt_auto_redial_timeri_event_cback
(
  ds707_cback_event_type               event,
  const ds707_cback_event_info_type   *event_info_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_IS707_MSG1(MSG_LEGACY_HIGH, "DCM :Phys link state : %d",event);

  switch(event)
  {
    case DS707_CBACK_PHYS_LINK_UP_EV:
    case DS707_CBACK_PHYS_LINK_DOWN_EV:
      /*
        Incoming call might have brought up the Phys link up.
        So, make sure the timer is stopped. 
      */
      ds707_pkt_auto_redial_timeri_cleanup();
      break;

    default:
      break;
  }

} /* ds707_pkt_auto_redial_timeri_event_cback() */


/*===========================================================================
                         EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_AUTO_REDIAL_TIMER_START

DESCRIPTION   Starts the auto redial timer.If the timer is not started 
              started already and if the retry attempts are not zero,
              the timer will be started again.

DEPENDENCIES  NONE

RETURN VALUE  TRUE: Successfully started the timer.
              FALSE: Failed to start the timer. 

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_pkt_auto_redial_timer_start()
{
 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 if( (ds707_pkt_auto_redial_timer_started == FALSE) &&
     (auto_redial_timeri_val > 0)  && 
     ((auto_redial_retry_attempts--) > 0))
  {
    DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
           "DCM : Start Auto redial timer %d msecs", auto_redial_timeri_val);
    ds3gi_start_timer( DS3G_TIMER_PKT_AUTO_REDIAL,  auto_redial_timeri_val );
    ds707_pkt_auto_redial_timer_started = TRUE;
    return TRUE;
  }
  return FALSE;

} /* ds707_pkt_auto_redial_timer_start() */


/*===========================================================================
FUNCTION      DS707_PKT_AUTO_REDIAL_INIT

DESCRIPTION   Called at startup. This function registers for
              physical link up/down events.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

===========================================================================*/
void ds707_pkt_auto_redial_init(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if (ds707_cback_register(
                            DS707_CBACK_PHYS_LINK_UP_EV,
                            ds707_pkt_auto_redial_timeri_event_cback
                          ) == FALSE)
  {
    ERR_FATAL("Could not reg PHYS_LINK_UP_EV cback",0,0,0);
  }

  if (ds707_cback_register(
                            DS707_CBACK_PHYS_LINK_DOWN_EV,
                            ds707_pkt_auto_redial_timeri_event_cback
                          ) == FALSE)
  {
    ERR_FATAL("Could not reg PHYS_LINK_DOWN_EV cback",0,0,0);
  }

} /* ds707_pkt_auto_redial_timer_init() */


/*===========================================================================
FUNCTION      DS707_PKT_AUTO_REDIAL_PROCESS_CMD

DESCRIPTION   Processes the auto redial timer commands. This function
              will redial the call.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_auto_redial_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
)
{
  ps_phys_link_type           *phys_link_ptr;
  ds3g_pkt_call_info_type     pkt_info;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  phys_link_ptr  = ds707_pkt_get_pri_phys_link_ptr();
  
  DATA_IS707_MSG1(MSG_LEGACY_HIGH, "DCM: Process cmd %d", 
                  ds_cmd_ptr->hdr.cmd_id);

  switch(ds_cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_707_PKT_AUTO_REDIAL_TIMER_EXPIRED:
      pkt_info.drs_bit         = TRUE;
      pkt_info.use_hdown_timer = TRUE;
     /*-------------------------------------------------------------------
        Need to Populate the new_network to be the current data sess 
        network. Otherwise the Orig handler will always set the current
        data session network to zero which is NO_SRVC
     -------------------------------------------------------------------*/
      
      pkt_info.new_network     = ds707_pkt_get_current_data_sess_network();

      /*-------------------------------------------------------------------
       For Non TE Based origination force_cdma_only should always be FALSE
       -------------------------------------------------------------------*/

      pkt_info.force_cdma_only = FALSE; 

      /*-------------------------------------------------------------------
       Set the inc_call flag to FALSE for all Origination attempts
      -------------------------------------------------------------------*/
      pkt_info.inc_call        = FALSE; 

      /*-------------------------------------------------------------------
       Set the epzid flag to FALSE for all Origination attempts
      -------------------------------------------------------------------*/
      pkt_info.epzid           = FALSE;

      phys_link_ptr->phys_link_up_cmd_f_ptr(phys_link_ptr, &pkt_info );
      break;

    default:
      break;
  }
} /* ds707_pkt_auto_redial_timer_process_cmd() */

#endif  /*FEATURE_DEDICATED_C2K_MODE*/




