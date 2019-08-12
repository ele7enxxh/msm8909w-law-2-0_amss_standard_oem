
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                         D A T A   S E R V I C E S

               C T A  (IDLE TIMEOUT) and related RLP functions

GENERAL DESCRIPTION
  This file contains functions used to to handle idle timeout processing.
  A bitmask is maintained.  If the bitmask is all zeroes, then the idle timer
  is decremented.  If the bitmask is non-zero, then the idle timer is
  stopped.
  
EXTERNALIZED FUNCTIONS
  DS707_CTA_RESET_IDLE_TIMER
    Called at the beginning of a packet data call.  Resets the idle timer 
    bitmasks, etc.  For QNC calls, dormancy is not allowed.  In that case
    the bitmask is set so that the bitmask is never all zeroes.
    
  D707_CTA_SET_IDLE_BIT
    Called by when there is active traffic on the channel.  Do not go
    idle.  
    
  DS707_CTA_CLEAR_IDLE_BIT
    Called when there is no active traffic on the channel.  Allows mobile
    to go idle.
  
  DS_RLP_DATA_RXED
    Called by RLP 3 every 20 msecs.  RLP passes a param indicating if data
    was received or not.
    
  DS707_CTA_RLP_SYNC_TIMEOUT
    Called by RLP if the SYNC handshake takes too long.  Results in the
    call being torn down.

  DS707_CTA_RLP_INIT
    Called at power-up.  Registers this module with pkt mgr for various call
    events.
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  Must call ds707_cta_rlp_init() at power-up.

  Copyright (c) 1999-2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_cta_rlp.c_v   1.1   15 Nov 2002 18:25:18   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_cta_rlp.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/03/13   sd      ATCoP decoupling changes
03/15/11   mg      Global variable cleanup
11/22/10   sn      Replaced REX premption macros with critical section for 
                   mutual exclusion.
10/13/10   op      Migrated to MSG 2.0 macros
09/13/10   op      Changed DS707_CBACK_MSG macro to use MSG 2.0 format
01/30/09   ls      In ds707_cta_event_cback(), it stops both 1x and HDR timers
                   when CDSN is in NO_SERVICE     
04/22/08   ac      Fix the error that the CTA timer is not set according to 
                   "AT + CTA" 
04/02/08   sn      Fixed featurization issue.
03/30/07   ac      EMPA: Implemented RLP inactivity timer feature. Let every 
				   RLP instance maintain its own inactivity timer.
11/15/05   atp     Moved ds_process_rlp_frames out to RLP.
10/25/05   as/vrk  replaced ERROR_FATAL at rlp sync timeout with MSG_ERROR 
                   since the FATAL is not a valid action (no calls active)
04/11/05   vrk     Add check for CTA timer (non zero) before starting
02/05/05   rsl     Moved cta timer start and stop on tx side to dsrlp.c 
				   from ds707_rmsm.c
04/22/04   ak      Lint changes.
04/09/04   ak      Make event callback take a const.
02/13/04   ak      Use phys link for ds707_pkt information.
02/04/04   ak      Updated to use pkt mgr event mechanisms.
11/15/02   ak      Updated comments in file header.
07/29/02   ak      On ps_iface commands, check for DS_EWOULDBLOCK.
07/17/02   ak      On ds_process_rlp_frames, do not assume only for pkt
                   calls.
07/03/02   ak      Change errno's to ps_iface funcs from uint16 -> int16.

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "ds3gtimer.h"
#include "ds707.h"
#include "ds707_cta_rlp.h"
#include "ds707_pkt_mgr.h"
#include "ds707_pkt_mgri.h"
#include "ds707_pkt_mgr_iface.h"
#include "ds707_async_mgr.h"
#include "ds707_so_async.h"
#include "ds707_cback.h"
#include "dsat707extctab.h"
#include "dserrno.h"
#include "dsm.h"
#include "err.h"
#include "msg.h"
#include "data_msg.h"
#include "ps_iface.h"
#include "ps_phys_link.h"
#include "hdrrlp.h"
#include "dsutil.h"

#ifdef FEATURE_IS95B_MDR
  #include "mdrrlp.h"
#else
  #include "rlp.h"
#endif 

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/
LOCAL uint32 dsi_dormant_bitmask = DS707_CTA_IS_IDLE_MASK; /* dorm bitmask */

/*===========================================================================
                        INTERNAL FUNCTION DEFINTIONS
===========================================================================*/
/*===========================================================================
FUNCTION       DS707_CTA_EVENT_CBACK

DESCRIPTION    Callback when phys link changes.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_cta_event_cback
(
  ds707_cback_event_type               event,
  const ds707_cback_event_info_type   *event_info_ptr
)
{
  sys_sys_mode_e_type curr_data_session_network = SYS_SYS_MODE_NO_SRV;
  uint32              cta_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  curr_data_session_network = ds707_pkt_get_current_data_sess_network();
  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_CTA_VAL,
                                          (void *)&cta_val,DS707_NON_ARRAY);

  DATA_IS707_MSG3(MSG_LEGACY_HIGH, 
              "phy link event %d, curr_data_session_network %d, nest_level %d",
              event, curr_data_session_network, ds707_cback_get_nest_level());

  switch(event)
  {
    case DS707_CBACK_PHYS_LINK_DOWN_EV:
#ifdef FEATURE_HDR
      if (curr_data_session_network == SYS_SYS_MODE_HDR)
      {
        hdrrlp_phy_link_down_inactivity_timer_hdlr();
      }
      else
#endif
      if (curr_data_session_network == SYS_SYS_MODE_CDMA)
      {
        ds707_cta_reset_timer(DS707_CTA_STOP_TIMER, DS707_INVALID_SO);
      }
      else
      {
        ds707_cta_reset_timer(DS707_CTA_STOP_TIMER, DS707_INVALID_SO);
#ifdef FEATURE_HDR
        hdrrlp_phy_link_down_inactivity_timer_hdlr();
#endif
      }
      break;

    case DS707_CBACK_PHYS_LINK_UP_EV:
#ifdef FEATURE_HDR
      if (curr_data_session_network == SYS_SYS_MODE_HDR)
      {
        hdrrlp_copy_default_cta_val(cta_val);
        hdrrlp_phy_link_up_inactivity_timer_hdlr();
      }
      else
#endif
      {
        ds707_cta_reset_timer(DS707_CTA_START_TIMER, event_info_ptr->so);
      }
      break;

#ifdef FEATURE_DATA_OPTHO
    case DS707_CBACK_TUN_PHYS_LINK_UP_EV:
      hdrrlp_copy_default_cta_val(cta_val);
      hdrrlp_phy_link_up_inactivity_timer_hdlr();
      break;

    case DS707_CBACK_TUN_PHYS_LINK_DOWN_EV:
      hdrrlp_phy_link_down_inactivity_timer_hdlr();
      break;

#endif /* FEATURE_DATA_OPTHO */

    default:
      break;
  }
} /* ds707_cta_event_cback() */


/*===========================================================================
                        EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_CTA_RESET_IDLE_TIMER

DESCRIPTION   Called when the traffic channel comes up or goes down  If the
              conditions are correct, clears the idle timer mask and starts
              the idle timer.  
    
              Otherwise disables the idle timer by setting a reserved bit.
              Note that the reserved bit is only set in this function.  By
              doing this, we don't need to check for AT+CTA, QNC, or phys
              link in the set_/clear_idle_bit functions.
  
              The boolean can be either:
                DS707_CTA_START_TIMER - start the idle timer
                DS707_CTA_STOP_TIMER  - stop the idle timer

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_cta_reset_timer
(
  boolean           start_stop,           /* start or stop timer?          */
  ds707_so_type     so                    /* service option being used     */
)
{
  uint32  cta_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_CTA_VAL,
                                          (void *)&cta_val,DS707_NON_ARRAY);

  if ((start_stop == DS707_CTA_START_TIMER)     &&
#ifdef FEATURE_DATA_IS707_ASYNC  	
      (ds707_so_async_so_is_async(so) == FALSE) && 
#endif /* FEATURE_DATA_IS707_ASYNC */      
      (cta_val > 0)
     )
  {
    dsi_dormant_bitmask = DS707_CTA_IS_IDLE_MASK;
    ds3gi_start_timer( DS3G_TIMER_CTA, (cta_val * 1000));
    DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                    "Enhanced_cta start old CTA timer: %d sec",cta_val);
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Stop CTA timer");
    dsi_dormant_bitmask = DS707_CTA_RLP_PHYS_LINK;
    ds3gi_stop_timer( DS3G_TIMER_CTA);
  }
} /* ds707_reset_timer() */

/*===========================================================================
FUNCTION      DS707_CTA_SET_IDLE_BIT

DESCRIPTION   This function takes a bitmask as a parameter, and then sets the
              appropriate bit in the dsi_dormant_bitmask.  If needed, it will
              also cancel the idle channel timer.
              
              Call this function when the channel is *not* IDLE; i.e., has 
              real data going back and forth.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_cta_set_idle_bit
(
  uint32   idle_bitmask                                 /* Dormant Bitmask */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Enter critical section to protect dormant parameters.
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&ds707_global_crit_sect);

  if (dsi_dormant_bitmask == DS707_CTA_IS_IDLE_MASK)
  {
    ds3gi_stop_timer( DS3G_TIMER_CTA);
  }
  dsi_dormant_bitmask = dsi_dormant_bitmask | idle_bitmask;

  /*-------------------------------------------------------------------------
    Leave critical section.
  -------------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION(&ds707_global_crit_sect);
} /* ds707_cta_set_idle_bit() */


/*===========================================================================
FUNCTION      DS707_CTA_CLEAR_IDLE_BIT

DESCRIPTION   This function takes a bitmask as a parameter, and then clears
              the appropriate bit in the dsi_dormant_bitmask.  If needed, it
              will also start the idle channel timer.

              Call this function when the channel is IDLE; i.e., NO 
              real data going back and forth.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_cta_clear_idle_bit
(
  uint32   idle_bitmask                                 /* Dormant Bitmask */
)
{
  uint32  cta_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_CTA_VAL,
                                          (void *)&cta_val,DS707_NON_ARRAY);
  /*-------------------------------------------------------------------------
    Enter critical section to protect dormant parameters.
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&ds707_global_crit_sect);

  if ( (dsi_dormant_bitmask == idle_bitmask)  && (cta_val > 0) )
       /* only this bit is set   and timer val is non zero   */
  {
    ds3gi_start_timer( DS3G_TIMER_CTA, (cta_val * 1000));
    DATA_IS707_MSG1(MSG_LEGACY_HIGH, 
                    "Enhanced_cta start old CTA timer %d sec", cta_val);
  }
  dsi_dormant_bitmask = dsi_dormant_bitmask & ~idle_bitmask;

  /*-------------------------------------------------------------------------
    Leave critical section.
  -------------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION(&ds707_global_crit_sect);
} /* ds707_cta_clear_idle_bit() */

#ifndef MSM5000_IRAM_FWD
/*===========================================================================
FUNCTION      DS_RLP_DATA_RXED

DESCRIPTION   This function is called from the rlp layer, after the frames
              received in a 20 msec period have been processed.  If the
              input parameter is FALSE, then no data/ctl/init frames have
              been received in the past 20 msec.  Otherwise at least one
              data frame was received.  If data has been received, then the
              idle timer should be reset, so that it doesn't expire.
  
              This function should be called from the RX task.

DEPENDENCIES  Called only from RLP 3.  Implies only packet data calls.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_rlp_data_rxed
(
  boolean     data_frs_rxed      /* tells if any non-idle/fill frames rxed */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( data_frs_rxed == TRUE)
  {
    ds707_cta_set_idle_bit(DS707_CTA_RLP_RX);
  }
  else  /* no data rxed in RLP */
  {
    ds707_cta_clear_idle_bit(DS707_CTA_RLP_RX);
  }
} /* ds_rlp_data_rxed() */

/*===========================================================================
FUNCTION      DS_RLP_DATA_TXED

DESCRIPTION   This function is called from the rlp layer, after the frames
              received in a 20 msec period have been processed.  If the
              input parameter is FALSE, then no data/ctl/init frames have
              been received in the past 20 msec.  Otherwise at least one
              data frame was received.  If data has been received, then the
              idle timer should be reset, so that it doesn't expire.
  
DEPENDENCIES  Called only from RLP 3.  Implies only packet data calls.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_rlp_data_txed
(
  boolean     data_frs_txed      /* tells if any non-idle/fill frames rxed */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( data_frs_txed == TRUE)
  {
    ds707_cta_set_idle_bit(DS707_CTA_RLP_TX);
  }
  else  /* no data rxed in RLP */
  {
    ds707_cta_clear_idle_bit(DS707_CTA_RLP_TX);
  }
} /* ds_rlp_data_txed() */
#endif /* MSM5000_IRAM_FWD */


/*===========================================================================
FUNCTION      DS707_CTA_RLP_SYNC_TIMEOUT

DESCRIPTION   Called by RLP when there is a sync timeout.  Should bring
              down the traffic channel.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_cta_rlp_sync_timeout(void)
{
  ps_phys_link_type         *phys_link_ptr;
  int16                      err_num;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  phys_link_ptr     = ds707_pkt_get_pri_phys_link_ptr();

  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "RLP failed to SYNC, Ending Call");
  if (( ps_phys_link_down_cmd(phys_link_ptr, &err_num, NULL) < 0) &&
      (err_num != DS_EWOULDBLOCK))
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR, 
         "Cannot bring down call on sync timeout. phy_link_ptr:%x, err_num:%d",
         phys_link_ptr, err_num);
  }
} /* ds707_cta_rlp_sync_timeout() */


/*===========================================================================
FUNCTION      DS707_CTA_INIT

DESCRIPTION   Called at powerup.  Registers for phys link events.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_cta_init(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds707_cback_register(
                            DS707_CBACK_PHYS_LINK_DOWN_EV,
                            ds707_cta_event_cback
                          ) == FALSE)
  {
    ERR_FATAL("Could not reg PHYS_LINK_DOWN_EV cback",0,0,0);
  }

  if (ds707_cback_register(
                            DS707_CBACK_PHYS_LINK_UP_EV,
                            ds707_cta_event_cback
                          ) == FALSE)
  {
    ERR_FATAL("Could not reg PHYS_LINK_UP_EV cback",0,0,0);
  }

#ifdef FEATURE_DATA_OPTHO
  if (ds707_cback_register(
                            DS707_CBACK_TUN_PHYS_LINK_DOWN_EV,
                            ds707_cta_event_cback
                          ) == FALSE)
  {
    ERR_FATAL("Could not reg TUN_PHYS_LINK_DOWN_EV cback",0,0,0);
  }

  if (ds707_cback_register(
                            DS707_CBACK_TUN_PHYS_LINK_UP_EV,
                            ds707_cta_event_cback
                          ) == FALSE)
  {
    ERR_FATAL("Could not reg TUN_PHYS_LINK_UP_EV cback",0,0,0);
  }
#endif /* FEATURE_DATA_OPTHO */

} /* ds707_cta_init() */

/*===========================================================================
FUNCTION      DS707_GET_DEFAULT_CTA_VAL

DESCRIPTION   This is for the multi-processcor Remote Procedure Call interface. The 
              function is needed by RLP.

DEPENDENCIES  None

RETURN VALUE  The global variable ds707_cta_val

SIDE EFFECTS  None
===========================================================================*/
dsat_num_item_type ds707_get_default_cta_val(void)
{
  uint32  cta_val = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - -*/
  
  (void) ds707_pkt_mgr_get_1x_profile_val(DS707_CTA_VAL,
                                          (void *)&cta_val,DS707_NON_ARRAY);

  return (dsat_num_item_type)cta_val;
}

/*===========================================================================
FUNCTION      DS707_ENHANCED_CTA_GO_DORMANT_HDLR

DESCRIPTION   This is for the multi-processcor Remote Procedure Call interface. The 
              function is needed by RLP, if all RLP are in dormant state.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_enhanced_cta_go_dormant_hdlr
(
  void
)
{
  ds_cmd_type  *cmd_ptr; /* Pointer to command */
  ps_phys_link_type                *phys_link_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  phys_link_ptr     = ds707_pkt_get_pri_phys_link_ptr();
  DATA_IS707_MSG0(MSG_LEGACY_HIGH, 
            "Enhanced_cta last busy RLP goes Idle, go dormant, if in traffic");
  if (ds707_pkt_is_dormant(phys_link_ptr) == FALSE)
  {
    if((cmd_ptr = ds_allocate_cmd_buf(DS_CMD_PAYLOAD_SIZE_ZERO)) == NULL)
    {
      ASSERT(0);
      return;
    }

    cmd_ptr->hdr.cmd_id = DS_CMD_707_PKT_CTA_TIMER_EXPIRED;
    ds_put_cmd( cmd_ptr );
  }
} /* ds707_enhanced_cta_go_dormant_hdlr() */
#endif /* FEATURE_DATA_IS707 */
 
