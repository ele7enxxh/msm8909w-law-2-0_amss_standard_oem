#ifndef HDRSRCHSTATES_H
#define HDRSRCHSTATES_H

/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       H D R    S R C H    S T A T E    I N I T I A L I Z A T I O N S

                    Search State Transition Prototypes

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

REGIONAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/srch/api/hdrsrchstates.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ------------------------------------------------------- 
05/04/2015   mbs     Supported QTA using TRM API
02/04/2015   arm     Check if primry chain is settled before sending QTA_END 
                     to TRM
10/22/2014   ljl     Fixed compiler warning. 
10/14/2014   wsh     Changes to support Jolokia bringup
09/09/2014   vke     Redesign fix for connection close during QTA gap
08/22/2014   vke     Delay connection close during QTA gap
08/20/2014   arm     Disable Tx after qta if doing OFS or channel change.
08/12/2014   tnp     Fix compiler warnings
07/29/2014   tnp     FR22375: Fix Medium compiler warnings in HDR
11/14/2013   arm     Merged wakeup manager changes for DSDS.
11/27/2013   dsp     FR17482 for Page blocking management.
09/03/2013   wsh     Replaced FEATURE_DIME/TRITON_BOLT_MODEM with HDR version
08/23/2013   rkc     Moved hdrsrchsleep_init_sleep_state out of featurization. 
08/21/2013   rkc     Added hdrsrchsleep_sleep_now(). 
08/12/2013   rmg     Added fix for FPD being skipped.
05/28/2013   rkc     Added LRE changes: HDRSRCHACQ_ACQ_DIV_ENABLED,
                     HDRSRCHACQ_DIVERSITY_ENABLE_TIME and hdrsrchsync_timeout.
05/02/2013   ljl     Saved scc offset and rate.
04/29/2013   dsp     Added hdrsrchsleep_update_prev_cc_start().
02/26/2013   mbs     Fixed featurization for IRAT code
02/21/2013   smd     Allowed diversity chain tune away first if primary chain 
                     release is done after unlock_by time.
01/25/2013   smd     Fixed HDR not releasing div when tune away is disabled in 
                     TC state.   
09/20/2012   vke    added hdrsrchsleep_get_cur_warmup_time() for logging current 
                              warmup time
10/19/2012   smd     Told fw whether the standby to active transition is for wakeup.
08/24/2012   kss/wsh Fixed TAP mode turning off after tuneaway and OFS
08/15/2012   smd     Supported idle tune away. 
05/25/2012   wsh     Moved ARD logic into it's own state machine 
05/17/2012   smd     Fixed HDR not releasing div when tune away is disabled. 
03/29/2012   smd     Added DIME sleep updates.
12/05/2011   ljl     Added hdrsrchsleep_get_cur_warmup_reacq_time().
11/11/2011   arm     HDR idle and access state diversity implemetation. 
11/11/2011   smd     Added hdrsrchreacq_calc_win_size_fctr(). 
11/08/2011   ljl     Avoided 1x and hdr rf tune conflict.
11/04/2011   ljl/smd Disabled WUTA. 
10/24/2011   ljl     Supported RPM optimized timeline. 
05/31/2011   cnx     Added hdrsrchsleep_get_bcmcs_sleep_cycle().
03/03/2011   arm     Fixed NPA crash due to overlapping NPA calls.   
12/09/2010   pxu     Added function prototypes for dynamic sleep algorithm. 
09/01/2010   vlc     Added support for DO->LTE pilot measurements/reselection.
07/21/2010   smd     Added hdrsrchofs_is_same_channel_ofs().
06/21/2010   cnx     Fixed compiler warnings.
05/11/2010   smd     Exposed hdrsrchsync_send_min_basp_sinr_msg() to all states.
03/24/2010   pxu     Removed FEATURE_HDR_QSM features 
11/20/2009   smd     Added IRAT pilot measurement.
09/15/2009   smd     Added TimeSync state.
07/16/2009   lyl     Supported ccmac MSGR interface.
03/09/2009   smd     Added support for pilot measurement velcro solution.
02/02/2009   smd     Added function hdrsrchsleep_get_sleep_slots().
11/18/2008   grl     Added ARDA functions.
11/13/2008   lyl     Renamed to the more meaningful function name of 
                     hdrsrchsleep_is_qpch_enabled();
10/15/2008   rkc     Added hdrsrchsleep_get_qpch_flag()
10/15/2008   rkc     Added qpch_enabled param to hdrsrchsleep_set_wakeup().
09/18/2008   rkc     Added Intra-SCC sleep params to hdrsrchsleep_set_wakeup().
                     Added hdrsrchsleep_get_intra_scc_valid() and
                     hdrsrchsleep_get_intra_scc_start_slot()
09/16/2008   smd     Added hdrsrchsleep_get_negotiated_sci()
06/16/2008   lyl     Added hdrsrchbc_get_bc_reason()
03/28/2008   grl     Added return value to hdrsrchsleep_adjust_rtc.
11/13/2007   rkc     Removed hdrsrchtc_set_tc_handdown_mode().
10/17/2007   jyw     Merged Rev B changes from branch.
09/20/2007   mt      HDR Adaptive diversity implementation.
07/20/2007   mt      Added hdrsrchofs_notify_nset_update().
06/30/2007   mt      Perform OFS after REACQ failure.
06/02/2007   grl     Added hdrsrchsleep_adjust_rf_warmup_on_tune_done func.
04/30/2007   mt      Enhanced OFS and RUP request changes.
04/28/2007   mt      Added hdrsrchofs_init_ofs_timeline().
03/08/2007   grl     Fixed RCVT compiler warning by changing func prototype.
01/13/2007   grl     Added hdrsrchreacq_last_reacq_succeeded.
10/31/2006   mt      Added hdrsrchsleep_get_last_sleep_sci() prototype.
10/27/2006   kss     Added hdrsrchsleep_adjust_rtc() function.
10/06/2006   grl     Added passing of the negotiated SCI value to hdrsrchsleep.
10/09/2006   mt      Added prototype for hdrsrchreacq_get_fpd_activation_status().
09/18/2006   mt      Added prototype for hdrsrchsleep_get_scc_offset().
08/28/2006   sq      Added support for FEATURE_HDR_QSM
06/30/2006   etv     Supported DSP Handoff algorithm changes.
03/06/2006   sq      Added hdrsrchsleep get/reset wake-up reason
01/04/2006   jyw     Added hdrsrchtc_disable_ofs
12/02/2005   sq      Added hdrsrchreacq_update_reacq_info
10/18/2005   sq      Added FEATURE_HDR_REVA_L1
09/19/2005   sq      Added hdrsrchsleep_set_cc_start
09/10/2005   grl     Added tc mode set and tc reason set functions.
09/06/2005   ds      Added hdrsrchacq_measure_time() prototype definition
07/27/2005   sq      Added support for BCMCS page monitoring
07/12/2005   sq      Added support for enhanced idle state protocol
06/30/2005   sq      Changed OFS API name/Added hdrsrchofs_tc_ofs_reqd
05/31/2005   grl     Added support for TRM and TCXOMgr.
04/27/2005   ds      Added round-robin OFS support
08/30/2004   aaj     Use enum type to represent BCMCS status
07/30/2004   sq      Added support for FEATURE_HDR_BCMCS
06/07/2004   aaj     Replaced hdrsrchidle_init_idle_state with new function
11/11/2003   sq      Added hdrsrchsleep_get_sci
10/06/2003   aaj     Added prototype for hdrsrchtc_aset_mgmt for hdr TC ofs
09/16/2003   sq      Added support for FEATURE_HDR_TC_OFS
03/11/2003   ajn     Added hdrsrchsleep_reset_sci & _adjust_sci prototypes.
                     Removed hdrsrchsleep_alter_sci and _get_sleep_until
03/11/2003   ajn     Updated Copyright notice
09/10/2002   aaj     Added demote_ofreq_cset prototype
04/23/2002   aaj     Delete update_ofs proto
02/16/2002   aaj     added support for slot cycle index enum and functions
02/06/2002   ajn     hdrerrno -> errno
01/17/2002   aaj     Added traffic hybrid suport. Resume connected state
01/16/2002   ajn     Changed ReAcq from IDLE substate to its own state.
10/23/2001   ajn     Created this file

==========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "hdr_variation.h"
#include "hdrsrchtypes.h"
#include "hdrsrchsect.h"
#include "hdrsrchrf.h"
#include "hdrsrchstate.h"
#include "hdrlogi.h"
#include "hdrlmac.h"

#ifdef FEATURE_HDR_WAKEUP_MANAGER
#include "wmgr.h"
#endif /* FEATURE_HDR_WAKEUP_MANAGER */

/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/

#ifdef FEATURE_HDR_DYNAMIC_ACQ_DIV

#define HDRSRCHACQ_ACQ_DIV_ENABLED \
 ( hdrsrch.acq_div.general_div_enabled && hdrsrch.acq_div.acq_div_nv_enable && \
  hdrsrch.acq_div.acq_div_runtime_ctrl )

#define HDRSRCHACQ_DIVERSITY_ENABLE_TIME 10 /*ms*/

#endif /* FEATURE_HDR_DYNAMIC_ACQ_DIV */



/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/


/* EJECT */
/*==========================================================================

                           INACTIVE STATE

==========================================================================*/



/*==========================================================================

FUNCTION
    HDRSRCHST_INIT_INACTIVE_STATE

DESCRIPTION
    Initializes the inactive state processing in HDR mode

DEPENDENCIES
    None

INPUTS
    None

RETURN VALUE
    None

SIDE EFFECTS
    None

==========================================================================*/

void hdrsrchst_init_inactive_state( void );


/* EJECT */
/*==========================================================================

                              ACQ STATE

==========================================================================*/


/*==========================================================================

FUNCTION        HDRSRCHACQ_INIT_ACQ_STATE

DESCRIPTION
  This function switches the HDR Searcher state to Acquisition Mode.

DEPENDENCIES
  Command parameters must be left pending the the ACQUIRE command

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrchacq_init_acq_state( void ); 

/*==========================================================================

FUNCTION     : HDRSRCHACQ_MEASURE_TIME

DESCRIPTION  :
  Measure the time between "start_ms" of specified timer and now.
  This function needs to run with interrupt locked.

DEPENDENCIES : 
  We assume the timer used for this time measurement has been started before
  this function is called, which is true because both "real" and "task" 
  timers are already running when we measure RF warm-up time. And RF warm-up
  time measurement is the first measurement for the ACQ INFO log packet.

RETURN VALUE :
  The measured elapsed time in ms.

SIDE EFFECTS :
  The specified timer will be stopped and re-started.

==========================================================================*/

extern uint16 hdrsrchacq_measure_time( void );


/* EJECT */
/*==========================================================================

                              SYNC STATE

==========================================================================*/



/*==========================================================================

FUNCTION      HDRSRCHSYNC_INIT_SYNC_STATE

DESCRIPTION
  This function is called to transition from Acquisition State to
  Sync State

DEPENDENCIES
  Acquired, fingers in lock

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrchsync_init_sync_state( void );

/*=========================================================================

FUNCTION     : HDRSRCHSYNC_SEND_MIN_BASP_SINR_MSG

DESCRIPTION  : This function sends the message to set minimum SINR input to
               the predictor.

DEPENDENCIES : None

INPUT        : min_basp_sinr - minimum SINR input

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchsync_send_min_basp_sinr_msg
(
  uint16              min_basp_sinr
    /* minimum SINR input to the predictor */
);

/* EJECT */
/*===========================================================================

FUNCTION HDRSRCHSYNC_TIMEOUT

DESCRIPTION
  This function ....

DEPENDENCIES
  TBD

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  TBD

===========================================================================*/

void hdrsrchsync_timeout( void );

/* EJECT */
/*==========================================================================

                              IDLE STATE

==========================================================================*/



/*==========================================================================

FUNCTION      HDRSRCHMON_INIT_IDLE_STATE

DESCRIPTION
  This function is called to transition from Sync, Sleep, or Traffic State
  to Idle Monitor State

DEPENDENCIES
  Acquired, fingers in lock.

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrchmon_init_idle_state( void );


/*===========================================================================

FUNCTION HDRSRCHIDLE_DEFAULT_IDLE_CMD

DESCRIPTION
  This function handles the default idle mode commands. These commands
  are same in active idle and suspended idle modes.

DEPENDENCIES
  None

PARAMETERS
  cmd   - Command to be processed

RETURN VALUE
  E_SUCCESS       - command has been successfully executed
  E_FAILURE       - command has failed
  E_NOT_SUPPORTED - command is not support
  E_IN_PROGRESS   - command has been started, but has not completed
  E_AGAIN         - command cannot be started in present state, but is a
                    valid command.  Execute hdrsrch_retry_cmd( ) at a
                    later point in time to reattempt the command.
                    [hdrsrch_retry_cmd( ) is called at every state change]

SIDE EFFECTS
  If E_IN_PROGRESS or E_AGAIN is returned, command processing is disabled
  until the command buffer is passed to hdrsrch_complete_cmd( ).

===========================================================================*/

errno_enum_type hdrsrchidle_default_idle_cmd
(
  const hdrsrch_cmd_type      * cmd
    /* Command to be processed. */
);

/*===========================================================================

FUNCTION HDRSRCHIDLE_IS_PRI_TA_ENABLED

DESCRIPTION
  This function returns whether tune away is enabled
  
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE: tune away is not disabled
  FALSE: tune aaway is disabled

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrsrchidle_is_pri_ta_enabled( void );


#ifdef FEATURE_HDR_IDLE_TUNE_AWAY
/*===========================================================================

FUNCTION HDRSRCHIDLE_IS_IN_ACCESS

DESCRIPTION
  This function returns whether we are in access substate
  
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE: in access
  FALSE: not in access, we are in monitor substate

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrsrchidle_is_in_access( void );
#endif /* FEATURE_HDR_IDLE_TUNE_AWAY */

/* EJECT */
/*==========================================================================

                              SLEEP STATE

==========================================================================*/



/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCH_INIT_SLEEP_STATE

DESCRIPTION
  This function causes the state machine to begin sleep state processing.

DEPENDENCIES
  Idle state

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  Transitions to the sleep state

===========================================================================*/

void hdrsrchsleep_init_sleep_state( void );

#ifdef FEATURE_HDR_TO_LTE
/*===========================================================================

FUNCTION HDRSRCHSLEEP_FAST_WAKEUP

DESCRIPTION
  Reprograms the sleep controller to wake up as soon as possible.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  Whether the fast wake up has been programmed into sleep controller. 

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrsrchsleep_fast_wakeup( void );
#endif /* FEATURE_HDR_TO_LTE */

/*===========================================================================

FUNCTION HDRSRCHSLEEP_RESET_SCI

DESCRIPTION
  Resets the SCI index to zero.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets hdrsrchsleep.sci to zero

===========================================================================*/

void hdrsrchsleep_reset_sci( void );


/*===========================================================================

FUNCTION HDRSRCHSLEEP_ADJUST_SCI

DESCRIPTION
  Based on the reacquision slew, the reacquisition window size, and the
  sleep duration, compute the SCI for HDRSRCH_SLEEP_UNTIL_MAX_CCCS

DEPENDENCIES
  hdrsrchsleep.sleep_slots

RETURN VALUE
  None

SIDE EFFECTS
  Adjusts hdrsrchsleep.sci

===========================================================================*/

void hdrsrchsleep_adjust_sci
(
  int                             slew,
    /* Reacquisition slew, in half-chips */

  int                             winsize
    /* Reacquisition window, in chips */
);


/*==========================================================================

FUNCTION HDRSRCHSLEEP_ENTER_SLEEP

DESCRIPTION
  This function will cause the HDR SRCH state machine to enter sleep. But,
  it might take a few detours before we actually arrive at the sleep state.
  If required, and sufficient time exists, it may:
    - start off-frequency searching,
  before it finally enters the sleep state.

DEPENDENCIES
  Must be called from IDLE state, and from any "detour" states.

RETURN VALUE
  Whether HDR will actually sleep based on the time to next wakeup.

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrsrchsleep_enter_sleep( void );


#ifdef FEATURE_HDR_BCMCS
/*=========================================================================

FUNCTION      HDRSRCHSLEEP_SET_BCMCS_PAGE_CYCLE

DESCRIPTION
  This command updates the BCMCS page cycle value.

DEPENDENCIES
  None

INPUTS
  page_cycle    - The BCMCS paging channel period which the AT monitors, in
                  units of SCC ( 256 slots ).

RETURN VALUE
  Current SCI value

SIDE EFFECTS
  None

=========================================================================*/

void hdrsrchsleep_set_bcmcs_page_cycle
(
  uint8     page_cycle
    /* BCMCS page cycle */
);


/*=========================================================================

FUNCTION      HDRSRCHSLEEP_SET_BCMCS_SLEEP_CYCLE

DESCRIPTION
  This command updates the BCMCS sleep cycle value.

DEPENDENCIES
  None

INPUTS
  sleep_cycle    - The BCMCS sleep period, in units of SCC ( 256 slots ).

RETURN VALUE
  Current Sleep period

SIDE EFFECTS
  None

=========================================================================*/

void hdrsrchsleep_set_bcmcs_sleep_cycle
(
  uint8     sleep_cycle
    /* BCMCS sleep period */
);

/*=========================================================================

FUNCTION      HDRSRCHSLEEP_GET_BCMCS_SLEEP_CYCLE

DESCRIPTION
  This command returns the current BCMCS sleep cycle value.
 
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  Current BCMCS sleep period

SIDE EFFECTS
  None

=========================================================================*/

uint8 hdrsrchsleep_get_bcmcs_sleep_cycle( void );

#endif /* FEATURE_HDR_BCMCS */


/*==========================================================================

FUNCTION      HDRSRCHSLEEP_SET_CONTROL_CHANNEL_CYCLE

DESCRIPTION
  Set the Control Channel Hash value "R", for "(C+R) mod Nsleep == 0"
  Also sets the maximum HDR sleep cycle
  
DEPENDENCIES
  None

INPUTS
  ccc - Control Channel Hash
  max_hdr_sleep_cycle - maximum HDR sleep cycle
  
RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrchsleep_set_control_channel_cycle
(
  uint8   ccc,
    /* Control Channel Hash value */

  uint8   max_hdr_sleep_cycle
    /* Maximum sleep period       */
);


/* EJECT */
/*==========================================================================

FUNCTION      HDRSRCHSLEEP_GET_CC_START

DESCRIPTION
  This function returns the CC start value in slot.
  
DEPENDENCIES
  None 

INPUTS
  None

RETURN VALUE
  CC start in slot

SIDE EFFECTS
  None

==========================================================================*/

uint8 hdrsrchsleep_get_cc_start( void );


#ifdef FEATURE_HDR_REVA_L1
/* EJECT */
/*==========================================================================

FUNCTION      HDRSRCHSLEEP_SET_CC_START

DESCRIPTION
  This function sets the CC start to indicate where to expect CC/SCC premable.
  
DEPENDENCIES
  This function needs to be called after AT wakes up. 

INPUTS
  cc_start - CC start in slot

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrchsleep_set_cc_start
(
  uint8               cc_start
    /* Time in slot that firmware expects to find CC/SCC premable */
);
#endif /* FEATURE_HDR_REVA_L1 */

/* EJECT */
/*==========================================================================

FUNCTION      HDRSRCHSLEEP_SEND_MSG_UPDATE_CC_START

DESCRIPTION
  This function sends the message to the firmware to update CC start.

DEPENDENCIES
  None 

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrchsleep_send_msg_update_cc_start( void );


/*==========================================================================

FUNCTION HDRSRCHSLEEP_SET_WAKEUP

DESCRIPTION
  This function specifies when to wakeup next (SCC or CCC)

DEPENDENCIES
  None

PARAMETERS
  until                      - Wakeup for the next SCC, or the next CCC.
                               Not used if cc_start_slot_valid is TRUE.
  neg_sci                    - Negotiated SCI which may be different from the
                               next wakeup time
  qpch_enabled               - Whether QPCH is enabled or not for next wake-up
  wakeup_cb                  - Who to inform when we wakeup
  cc_start_slot_valid        - Flag whether cc_start_slot parameter is
                               valid or not.  If TRUE, use cc_start_slot
                               instead SCI to calculate  wake_slot.
  cc_start_slot              - CC start in slots.  This field is used if
                               cc_start_slot_valid is TRUE.
  ornl_cycle                 - Wake up cycle for Other RAT neighbor list message

RETURN VALUE
  None

SIDE EFFECTS
  Wakeup cycle and slot is computed.

==========================================================================*/

void hdrsrchsleep_set_wakeup
(
  uint8                                       until,
    /* When should we wakeup next?  Not used if cc_start_slot_valid is TRUE. */

  uint8                                       neg_sci,
    /* Negotiated SCI which may be different from the next wakeup time */

  boolean                                     qpch_enabled,
    /* Whether QPCH is enabled or not */

  hdrsrch_wakeup_cb_type                      wakeup_cb,
    /* What to do when we wakeup? */

  boolean                                     cc_start_slot_valid,
    /* Flag used to indicate whether cc_start_slot is valid */

  uint16                                      cc_start_slot
    /* CC start in slots. This field is valid if cc_start_slot_valid is TRUE */

#ifdef FEATURE_HDR_TO_LTE
  ,uint8                                      ornl_cycle
    /* Other RAT (Radio Access Technologies) Neighbor List message has its own cycle */
#endif /* FEATURE_HDR_TO_LTE */

#ifdef FEATURE_HDR_PAGE_BLOCKING_MGMT
  ,boolean                                   session_is_open,
    /* If session is open */

   boolean                                   ovhd_is_updated
    /* If ovhd is updated */
#endif /* FEATURE_HDR_PAGE_BLOCKING_MGMT */

);


/*==========================================================================

FUNCTION HDRSRCHSLEEP_RESET_WAKEUP_REASON

DESCRIPTION
  This function sets the wakeup reason to "no wakeup pending".

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
==========================================================================*/

void hdrsrchsleep_reset_wakeup_reason( void );


/*==========================================================================

FUNCTION HDRSRCHSLEEP_GET_WAKEUP_REASON

DESCRIPTION
  This function returns the reason for the next wake-up.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  wake-up reason

SIDE EFFECTS
  None
  
==========================================================================*/

hdrsrch_wakeup_reason_enum_type hdrsrchsleep_get_wakeup_reason( void );


/*=========================================================================

FUNCTION      HDRSRCHSLEEP_GET_SCI

DESCRIPTION
  This function returns the current HDR SCI value. This is provided as an
  access function for HDRSRCH logging.

DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  Current SCI value

SIDE EFFECTS
  None

=========================================================================*/

int8 hdrsrchsleep_get_sci( void );


/*==========================================================================

FUNCTION HDRSRCHSLEEP_GET_INTRA_SCC_VALID

DESCRIPTION
  This function returns intra-SCC sleep is enabled or not.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  The value intra-SCC start slot valid flag, i.e., TRUE is intra-SCC is enabled

SIDE EFFECTS
  None
  
==========================================================================*/

boolean hdrsrchsleep_get_intra_scc_valid( void );


/*==========================================================================

FUNCTION HDRSRCHSLEEP_GET_INTRA_SCC_START_SLOT

DESCRIPTION
  This function returns intra-SCC start slot.  Return 0 if
  hdrsrchsleep.intra_scc_start_slot_valid is FALSE.

DEPENDENCIES

PARAMETERS
  None
  
RETURN VALUE
  Value of intra-SCC start slot

SIDE EFFECTS
  None
  
==========================================================================*/

uint16 hdrsrchsleep_get_intra_scc_start_slot( void );


/* <EJECT> */
/*==========================================================================

                                 REACQ STATE

==========================================================================*/


/*==========================================================================

FUNCTION HDRSRCHREACQ_INIT_REACQ_STATE

DESCRIPTION
  This attempts to reacquire a system after Sleep or Tx/Idle Suspend.

DEPENDENCIES
  Must only be called from Sleep or Tx/Idle Suspend states.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  Search scheduler is restarted.

==========================================================================*/

void hdrsrchreacq_init_reacq_state( void );

#ifdef FEATURE_HDR_DIME_MODEM
/*===========================================================================

FUNCTION HDRSRCHSLEEP_SEND_SLEEP_ADJUST_MSG

DESCRIPTION
  This function sends sleep_adjust message to FW.
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchsleep_send_sleep_adjust_msg( void );

#else  /* FEATURE_HDR_DIME_MODEM */
/*===========================================================================

FUNCTION HDRSRCHSLEEP_ADJUST_RTC

DESCRIPTION
  This function adjusts RTC count after sleep, based on the time
  slept. The function also configures the DSP's initial interrupt
  timing for immediately following RTC on.

DEPENDENCIES
  Must be called during sleep, when RTC is off.

PARAMETERS
  None

RETURN VALUE
  Whether the adjustment was made before the RTC came on.

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrsrchsleep_adjust_rtc( void );
#endif /* FEATURE_HDR_DIME_MODEM */

/*==========================================================================

FUNCTION HDRSRCHREACQ_SET_WAKEUP_REPORT

DESCRIPTION
  This function specifies who to inform about which pilot we reacquired on
  when we wakeup.

DEPENDENCIES
  None

PARAMETERS
  wakeup_cb - Who to inform when we wakeup

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrchreacq_set_wakeup_report
(
  hdrsrch_wakeup_cb_type                      wakeup_cb
    /* What to do when we wakeup? */
);


/*===========================================================================

FUNCTION HDRSRCHREACQ_WAKEUP_REPORT

DESCRIPTION
  This function reports wakeup & reacquisition, with a possible handoff
  to a new pilot.

DEPENDENCIES
  None

PARAMETERS
  sect - Sector to a reacquisition handoff was performed, NULL if old pilot.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void hdrsrchreacq_wakeup_report
(
  hdrsrch_sect_struct_type  *sect
    /* RAHO sector, or NULL if no reacquisition handoff  */
);


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHREACQ_UPDATE_REACQ_INFO

DESCRIPTION
  This function updates REACQ INFO log packet depending on reacq result.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void hdrsrchreacq_update_reacq_info
(
  hdrsrch_sect_struct_type        *sect,
  /* sector that the "RAHOed" or old pilot belongs to   */
  
  log_hdr_reacq_result_enum_type  result
  /* Re-acquisition result              */
);

/*==========================================================================

FUNCTION HDRSRCHREACQ_SET_FAST_ACQ_REPORT

DESCRIPTION
  This function specifies fast acq callback function 

DEPENDENCIES
  None

PARAMETERS
  fast_acq_cb - Who to inform when we wakeup

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrchreacq_set_fast_acq_report
(
  hdrsrch_fast_acq_cb_type                      fast_acq_cb
    /* What to do when we do fast acq */
);


/*==========================================================================

FUNCTION HDRSRCHREACQ_DEFINE_FPD_TIMER

DESCRIPTION
  This function defines timer for scheduling preamble detect. 

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrsrchreacq_define_fpd_timer( void );


/*==========================================================================

FUNCTION HDRSRCHREACQ_START_FPD_TIMER

DESCRIPTION
  This function starts timer for arriving at next SCC slot and programming
  it to FW for FPD, QPCH and DLNA.

DEPENDENCIES
  None

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrchreacq_start_fpd_timer( void );


/* <EJECT> */
/*==========================================================================

                                 OFS STATE

==========================================================================*/

/*===========================================================================

FUNCTION    HDRSRCHOFS_IDLE_OFS_REQUIRED

DESCRIPTION
  Returns whether or not ofreq search is required at this moment

  It may not be necessary to perform a neighbor search on a different
  frequency if our active set very strong or we have lots of good neighbors
  and candidate sectors on the current frequency.


  Check following conditions to decide if OFS is needed

  1. More than 5 seconds has elapsed since last OFS.
  3. All active and candidate set pilots are below -5 dB
  4. Second best pilot in active plus candidate set is below -10 dB
  5. Different frequency candidate or neighbour count is non-zero.

  OFS required = ( 1 && 2 && 3 && 4 )

DEPENDENCIES

PARAMETERS

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS

===========================================================================*/

boolean hdrsrchofs_idle_ofs_reqd( void );


/*===========================================================================

FUNCTION HDRSRCHOFS_IDLE_OFS_STATE_VISIT_REQUIRED

DESCRIPTION
  This function returns the flag that whether 
  we need to visit idle OFS state due to pending RUP 
  search or autonomus OFS search.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrsrchofs_idle_ofs_state_visit_required( void );

/*===========================================================================

FUNCTION HDRSRCHOFS_REGISTER_IDLE_OFS_RETURN_STATE

DESCRIPTION

  This function specifies which state to return to after 
  performing idle OFS.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchofs_register_idle_ofs_return_state
(
  hdrsrch_state_enum_type           return_state
    /* specific state to return to after idle OFS */
);

/*===========================================================================

FUNCTION    HDRSRCHOFS_TC_OFS_REQUIRED

DESCRIPTION
  Returns whether or not ofreq search is required at this moment

  It may not be necessary to perform a neighbor search on a different
  frequency if our active set very strong or we have lots of good neighbors
  and candidate sectors on the current frequency.

  Check following conditions to decide if OFS is needed for traffic state:
  
  1. all the aset and cset pilots are below -5db
  2. different frequency candidate or neighbour count is non-zero.

  OFS required = ( 1 && 2 )

DEPENDENCIES

PARAMETERS

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS

===========================================================================*/

boolean hdrsrchofs_tc_ofs_reqd( void );


/*==========================================================================

FUNCTION HDRSRCH_INIT_OFS_STATE

DESCRIPTION
  This function causes the state machine to begin sleep state processing.

DEPENDENCIES
  Idle state

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  Transitions to the sleep state

==========================================================================*/

void hdrsrchofs_init_ofs_state( void );

/*===========================================================================

FUNCTION HDRSRCHOFS_GET_NEXT_SRCH_IDX

DESCRIPTION
  This function returns the next NSET pilot index  to be considered for 
  round-robin o-freq search.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  Returns the index into the next NSET pilot to be searched

SIDE EFFECTS
  None

===========================================================================*/

uint8 hdrsrchofs_get_next_srch_idx( void );

/*===========================================================================

FUNCTION HDRSRCHOFS_UPDATE_NEXT_SRCH_IDX

DESCRIPTION
  This function updates the search index for o-freq search used in round 
  robin off-frequency search.

DEPENDENCIES
  None

PARAMETERS
  Updated off - frequency search index

RETURN VALUE
  None

SIDE EFFECTS
  If the o-freq next search index has wrapped to zero then this o-freq is 
  marked as having been searched 

===========================================================================*/

void hdrsrchofs_update_next_srch_idx
(
  uint8                   next_srch_idx
    /* Updated off - frequency search index */
);


/* EJECT */
/*==========================================================================

                              CONNECTED STATE

==========================================================================*/



/*==========================================================================

FUNCTION      HDRSRCH_INIT_CONNECTED_STATE

DESCRIPTION
  This function is called to transition from Idle State to
  Connected State

DEPENDENCIES
  Expects a Suspended TCA Command for TCA parameters.

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrchtc_init_connected_state( void );

#ifdef FEATURE_HDR_BCMCS
/* EJECT */
/*==========================================================================

                        IDLE BROADCAST STATE

==========================================================================*/


/*==========================================================================

FUNCTION      HDRSRCHBC_INIT_BC_STATE

DESCRIPTION
  This function is called to transition to Idle Broadcast State

DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrchbc_init_bc_state( void );
                      

/* EJECT */
/*===========================================================================

FUNCTION HDRSRCHBC_GET_BC_REASON

DESCRIPTION
  Retrieves the current reason for BCMCS: 
  high priority BCMCS or regular BCMCS.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  The reason for being in BCMCS. 

SIDE EFFECTS
  None

===========================================================================*/

hdrsrchrf_reason_enum_t hdrsrchbc_get_bc_reason( void );


/* EJECT */
/*==========================================================================

                        IDLE BROADCAST ACCESS STATE

==========================================================================*/


/*==========================================================================

FUNCTION      HDRSRCHBCA_INIT_BCA_STATE

DESCRIPTION
  This function is called to transition from Idle Broadcast State to
  Idle Broadcast Access State

DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrchbca_init_bca_state( void );


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHBC_GET_BC_STATUS

DESCRIPTION
  This function returns BCMCS status ( on/off ).

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  BC_ENABLED
  BC_DISABLED

SIDE EFFECTS
  None.

===========================================================================*/

hdrsrch_bcmcs_enum_type hdrsrchbc_get_bc_status( void );


/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHBC_SET_BC_STATUS

DESCRIPTION
  This function sets BCMCS status ( on/off ).

DEPENDENCIES
  None

PARAMETERS
  BCMCS status

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void hdrsrchbc_set_bc_status( hdrsrch_bcmcs_enum_type bc_status );

#endif /* FEATURE_HDR_BCMCS */

/*===========================================================================

FUNCTION      HDRSRCH_RESUME_CONNECTED_STATE

DESCRIPTION
  This function is called to transition from Idle State to
  Connected State

DEPENDENCIES
  Expects a Suspended TCA Command for TCA parameters.

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  Causes state transition.

===========================================================================*/

void hdrsrchtc_resume_connected_state( void );

/*==========================================================================

FUNCTION HDRSRCHTC_SET_DRC_PARAM

DESCRIPTION
  This function sets the DRC parameters to the "drc_param" format

  Bit pattern which controls the DRC behaviour is:

    Bit7    : if 1 then the remaining bits are valid
    Bit6    : if 1 then enable real variable rate (disable predictor override)
    Bit5    : Reserved (Can be used for Rate Control Loop)
    Bit4    : Reserved
    Bit3-bit0 : DRC Values


DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  Sets the "drc_param" variable

==========================================================================*/

void hdrsrchtc_set_drc_param
(
#ifdef FEATURE_HDR_REVB
  hdrsrch_demod_idx_type demod_idx,
#endif /* FEATURE_HDR_REVB */

  uint8   drc_param
   /* bit pattern for DRC */

);


/*==========================================================================

FUNCTION HDRSRCHTC_DRC_PARAM_CTRL

DESCRIPTION
  This function sets the DRC parameters which are received from the AT command

  Bit pattern which controls the predictor behaviour.

    Bit7    : if 1 then the remaining bits are valid
    Bit6    : if 1 then enable real variable rate (disable predictor override)
    Bit5    : Reserved
    Bit4    : Reserved
    Bit3-bit0 : DRC Values


DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  Changes the DRC rate/mode

==========================================================================*/
#ifdef FEATURE_HDR_REVB
void hdrsrchtc_drc_param_ctrl( uint8 c  );
#else
void hdrsrchtc_drc_param_ctrl( void );
#endif /* FEATURE_HDR_REVB */

/* EJECT */
/*===========================================================================

FUNCTION HDRSRCHTC_DEFAULT_TRAFFIC_CMD

DESCRIPTION
  This function handles the default traffic mode commands. These commands
  are same in active traffic and suspended traffic modes.

DEPENDENCIES
  None

PARAMETERS
  cmd   - Command to be processed

RETURN VALUE
  E_SUCCESS       - command has been successfully executed
  E_FAILURE       - command has failed
  E_NOT_SUPPORTED - command is not support
  E_IN_PROGRESS   - command has been started, but has not completed
  E_AGAIN         - command cannot be started in present state, but is a
                    valid command.  Execute hdrsrch_retry_cmd( ) at a
                    later point in time to reattempt the command.
                    [hdrsrch_retry_cmd( ) is called at every state change]

SIDE EFFECTS
  If E_IN_PROGRESS or E_AGAIN is returned, command processing is disabled
  until the command buffer is passed to hdrsrch_complete_cmd( ).

===========================================================================*/

errno_enum_type hdrsrchtc_default_traffic_cmd
(
  const hdrsrch_cmd_type      * cmd
    /* Command to be processed. */
);

/* EJECT */
/*=========================================================================

FUNCTION     : HDRSRCHTC_LOG_RAB_INFO_RECORD

DESCRIPTION  : Log the reverse activity bit information received in the
               Traffic channel assignment message from the AN

DEPENDENCIES : None

RETURN VALUE : None

SIDE EFFECTS : none

=========================================================================*/
void hdrsrchtc_log_rab_info_record
(
  const hdrsrch_tc_assignment_params_type *params
    /* Traffic Channel Assignment parameters */
);

/*===========================================================================

FUNCTION HDRSRCHTC_SEND_PILOT_REPORT

DESCRIPTION
  This function is originally designed to be used during TC OFS state, to
  send out any route update message when needed.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE when route update is sent out successfully.

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrsrchtc_send_pilot_report( void );


/* EJECT */
/*===========================================================================

FUNCTION HDRSRCHTC_GET_TC_REASON

DESCRIPTION
  Retrieves the current reason for Traffic: BCMCS/Low Latency Traffic/or
  regular traffic.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  The reason for being in traffic. 

SIDE EFFECTS
  None

===========================================================================*/

hdrsrchrf_reason_enum_t hdrsrchtc_get_tc_reason( void );


/* EJECT */
/*===========================================================================

FUNCTION HDRSRCHTC_SET_TC_MODE

DESCRIPTION
  Sets the selected traffic mode which may change the priority for holding
  the RF lock while in traffic and enables/disables simultaneous mode.

  The options currently supported are:

  HDRSRCH_DEFAULT_TC_MODE   - Standard HDR traffic connection.

  HDRSRCH_S1XHDR_HP_TC_MODE - High Priority Traffic Mode where HDR forces 1x
                              paging onto chain 1, and preempts 1x acquisition.


DEPENDENCIES
  None

PARAMETERS
  tc_mode - The traffic mode to be invoked.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchtc_set_tc_mode
(
  hdrsrch_tc_mode_enum_type   tc_mode
    /* The specific HDR traffic mode selected. */
);


/* EJECT */
/*=========================================================================

FUNCTION     : HDRSRCHTC_DISABLE_OFS

DESCRIPTION  : This function is called if any application ( so far FTAP or
               RTAP ) wants to disable TC OFS. TC OFS will be re-enabled
               by default whenever HDRSRCH enters traffic state. 

DEPENDENCIES : None

PARAMETERS   : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrsrchtc_disable_ofs ( void );

/* EJECT */

/*==========================================================================

                              SUSPENDED STATE

==========================================================================*/

/*===========================================================================

FUNCTION      HDRSRCH_INIT_SUSPENDED_STATE

DESCRIPTION

DEPENDENCIES

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  Causes state transition.

===========================================================================*/

void hdrsrchsus_init_suspended_state( void );

/* <EJECT> */
/*===========================================================================

FUNCTION    HDRSRCHOFS_DEMOTE_OFREQ_CSET_PILOTS

DESCRIPTION
  Demotes the off-freq CSET pilots to NSET and sets their Ec/Io to 0

DEPENDENCIES

PARAMETERS

RETURN VALUE
  none

SIDE EFFECTS
  could change nset/cset

===========================================================================*/

void hdrsrchofs_demote_ofreq_cset_pilots( void );

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHTC_ASET_MGMT

DESCRIPTION
  This function performs Active Set management in Connected (Traffic) State.
  Specifically, if any pilot events occur, the upper layer is notified.

DEPENDENCIES
  None

PARAMETERS
  new_rpts - # of new pilot report events (R/N to C Set, C>A, Weak A)

RETURN VALUE
  None.

SIDE EFFECTS
  May call pilot report callback

===========================================================================*/

void hdrsrchtc_aset_mgmt
(
  int new_rpts
    /* Number of new pilot report events (R/N to C Set, C>A, Weak A) */
);


/* EJECT */
/*==========================================================================

FUNCTION        HDRSRCHTC_GET_NUM_CELLS_IN_ASET

DESCRIPTION
  This function is used to get the number of unique cells in an Active Set.
    
DEPENDENCIES
  None

PARAMETERS
  subaset_idx - the subaset index of the subaset requseted.

RETURN VALUE
  Number of unique cells in an Active Set.

SIDE EFFECTS
  None

==========================================================================*/
uint8 hdrsrchtc_get_num_cells_in_aset 
(
#ifdef FEATURE_HDR_REVB
  uint8 subaset_index
#else
  void
#endif /* FEATURE_HDR_REVB */
);

/* EJECT */
/*==========================================================================

FUNCTION HDRSRCHSLEEP_GET_SCC_OFFSET

DESCRIPTION
  This function returns the scc offset saved before going to sleep.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  The value of scc offset saved before going to sleep. 

SIDE EFFECTS
  None
  
==========================================================================*/

uint16 hdrsrchsleep_get_scc_offset( void );

/* EJECT */
/*==========================================================================

FUNCTION HDRSRCHSLEEP_SET_SCC_OFFSET

DESCRIPTION
  This function sets scc offset.

DEPENDENCIES
  None

PARAMETERS
  scc_offset - SCC offset
  
RETURN VALUE
  None 

SIDE EFFECTS
  None
  
==========================================================================*/

void hdrsrchsleep_set_scc_offset
(
  uint16                scc_offset
    /* SCC offset */
);

/* EJECT */
/*==========================================================================

FUNCTION HDRSRCHSLEEP_SET_SCC_INFO

DESCRIPTION
  This function sets scc offset and rate.

DEPENDENCIES
  None

PARAMETERS
  scc_offset - SCC offset
  scc_rate   - SCC rate
RETURN VALUE
  None 

SIDE EFFECTS
  None
  
==========================================================================*/

void hdrsrchsleep_set_scc_info
(
  uint16                scc_offset,
    /* SCC offset */

  uint8                 scc_rate
);


/*==========================================================================

FUNCTION HDRSRCHSLEEP_GET_SCC_RATE

DESCRIPTION
  This function gets scc rate.

DEPENDENCIES
  None

PARAMETERS
  None
RETURN VALUE
  scc rate

SIDE EFFECTS
  None
  
==========================================================================*/

uint8 hdrsrchsleep_get_scc_rate( void );

/* EJECT */
/*==========================================================================

FUNCTION HDRSRCHSLEEP_SEND_MSG_UPDATE_SCC_OFFSET

DESCRIPTION
  This function sends the message to the firmware to update scc offset.

DEPENDENCIES
  None

PARAMETERS
  scc_offset - SCC offset
  
RETURN VALUE
  None 

SIDE EFFECTS
  None
  
==========================================================================*/

void hdrsrchsleep_send_msg_update_scc_offset( void );

#ifdef FEATURE_HDR_FORCE_PREAMBLE_DET

 /* EJECT */
/*===========================================================================
FUNCTION HDRSRCHREACQ_GET_FPD_ACTIVATION_STATUS

DESCRIPTION
  This function returns the flag whether forced preamble detection
  has been activated or not.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
    
RETURN VALUE
  Boolean flag indicating whether forced preamble detection
  has been activated or not.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrsrchreacq_get_fpd_activation_status( void );

 /* EJECT */
/*===========================================================================
FUNCTION HDRSRCHREACQ_RESET_FPD_ACTIVATION_STATUS

DESCRIPTION
  This function resets the flag for forced preamble detection
  activation status.
  
DEPENDENCIES
  None.

PARAMETERS
  None.
    
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrsrchreacq_reset_fpd_activation_status( void );

#endif /* FEATURE_HDR_FORCE_PREAMBLE_DET */


#ifdef FEATURE_HDR_DDARF
/*==========================================================================

FUNCTION HDRSRCHREACQ_LAST_REACQ_SUCCEEDED

DESCRIPTION
  This function returns whether the last reacquisition succeeded.

DEPENDENCIES
  None 

PARAMETERS
  None

RETURN VALUE
  Whether the reacquisition succeeded.

SIDE EFFECTS
  None

==========================================================================*/

boolean hdrsrchreacq_last_reacq_succeeded( void );
#endif /* FEATURE_HDR_DDARF */

/*===========================================================================

FUNCTION HDRSRCHUTIL_CALCULATE_WIN_SIZE_FCTR

DESCRIPTION
  This function caculates the window size factor based on the slew

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE : 
  None

SIDE EFFECTS
  None.

===========================================================================*/

void hdrsrchreacq_calc_win_size_fctr( void );

/*==========================================================================

FUNCTION HDRSRCHSLEEP_RESET_LAST_NORMAL_SLEEP_SCI

DESCRIPTION
  This function sets the LAST_NORMAL_SLEEP_SCI to the dafult 9

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
==========================================================================*/
void hdrsrchsleep_reset_last_normal_sleep_sci( void );
 /* EJECT */
/*==========================================================================

FUNCTION HDRSRCHSLEEP_GET_LAST_SLEEP_SCI

DESCRIPTION
  This function returns the value of the sci used for last sleep interval.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  The value of the sci used for the last sleep interval. 

SIDE EFFECTS
  None
  
==========================================================================*/

uint8 hdrsrchsleep_get_last_sleep_sci( void );

/*==========================================================================

FUNCTION HDRSRCHSLEEP_GET_NEGOTIATED_SCI

DESCRIPTION
  This function returns the value of negotiated sci 

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  The value of negotiated sci. 

SIDE EFFECTS
  None
  
==========================================================================*/

uint8 hdrsrchsleep_get_negotiated_sci( void );

/*==========================================================================

FUNCTION HDRSRCHSLEEP_GET_SLEEP_SLOTS

DESCRIPTION
  This function returns the value of sleep slots. 

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  The value of sleep slots. 

SIDE EFFECTS
  None
  
==========================================================================*/

uint32 hdrsrchsleep_get_sleep_slots( void );
  
 /* EJECT */
/*========================================================================== 
  
FUNCTION HDRSRCHSLEEP_IS_QPCH_ENABLED 
  
DESCRIPTION 
  This function returns TRUE if QPCH is enabled, FALSE otherwise.
  
DEPENDENCIES 
  None 
  
PARAMETERS 
  None 
   
RETURN VALUE 
  TRUE  - if AT should monitor QPCH at next wake-up; 
  FALSE - if AT is not supposed to monitor QPCH at next wake-up. 
  
SIDE EFFECTS 
  None 
   
==========================================================================*/ 
  
boolean hdrsrchsleep_is_qpch_enabled( void ); 
  
  
 /* EJECT */
/*===========================================================================

FUNCTION HDRSRCHOFS_GET_RUPSET_SRCH_INDEX

DESCRIPTION
  This function returns the index into the RUP set 
  to indicate where in RUP set to start adding sectors to search.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  Index into the RUP set.

SIDE EFFECTS
  None

===========================================================================*/

uint16 hdrsrchofs_get_rupset_srch_index( void );

 /* EJECT */
/*===========================================================================

FUNCTION HDRSRCHOFS_SET_RUPSET_SRCH_INDEX

DESCRIPTION
  This function sets the index into the RUP set 
  to indicate where in RUP set to start adding sectors to search.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  If the rupset search index has wrapped to zero then we have completed 
  searching the desired RUP sectors for this round of OFS visit. 

===========================================================================*/

void hdrsrchofs_set_rupset_srch_index
(
  uint16                  next_srch_index
    /* Updated RUPset search index */
);

/*===========================================================================

FUNCTION HDRSRCHOFS_CLEAR_RUP_REQ

DESCRIPTION
  This function cancels any pending RUP request with OFS state.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchofs_clear_rup_req( void );

/*===========================================================================

FUNCTION HDRSRCHOFS_RUP_PENDING

DESCRIPTION
  This function returns the flag that whether
  there is a RUP request pending.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  Flag whether there is RUP request pending.

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrsrchofs_rup_pending( void );

/*===========================================================================

FUNCTION HDRSRCHOFS_RUP_REPORT_PENDING

DESCRIPTION
  This function returns the flag that whether
  there is a RUP search report pending.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  Flag whether there is RUP search report pending.

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrsrchofs_rup_report_pending( void );

/*===========================================================================

FUNCTION HDRSRCHOFS_RUP_REPORT_PENDING

DESCRIPTION
  This function resets the flag that whether
  there is a RUP search report pending.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchofs_reset_rup_report_pending( void );

/*===========================================================================

FUNCTION HDRSRCHOFS_REGISTER_RUP_REQUEST

DESCRIPTION
  This function registers pending RUP request with OFS state.

DEPENDENCIES
  None

PARAMETERS
  chan            - channel of the sector
  system_type     - system type of the channel
  is_pilot_meas   - whether the RUP search is for pilot measurement request
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchofs_register_rup_req
(
  sys_channel_type                chan,
    /* Channel of the sector */
    
  hdrsrch_system_enum_type        system_type,
    /* system type for the current channel */

  boolean                         is_pilot_meas
    /* whether the RUP is for pilot measurement request */
);

/*===========================================================================

FUNCTION HDRSRCHOFS_CANCEL_RUP_REQ

DESCRIPTION
  This function cancels any pending RUP request with OFS state.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchofs_cancel_rup_req( void );

/*===========================================================================

FUNCTION HDRSRCHOFS_PILOT_MEAS_PENDING

DESCRIPTION
  This function returns the flag that whether
  there is a pilot measurement request pending.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  Whether there is pending pilot measurement request

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrsrchofs_pilot_meas_pending( void );


/*===========================================================================

FUNCTION HDRSRCHOFS_CLEAR_PILOT_MEAS_REQ

DESCRIPTION
  This function cancels any pending pilot meas request with OFS state.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchofs_clear_pilot_meas_req( void );

/*===========================================================================

FUNCTION HDRSRCHOFS_IS_OFS_CHANNEL_IN_ASET

DESCRIPTION
  This function returns the flag that indicates whether
  the searched channel in OFS is the same as aset channel.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  Whether it is a same channel OFS

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrsrchofs_is_ofs_channel_in_aset( void );

#ifdef FEATURE_HDR_LTE_TO_EHRPD_IRAT
/*===========================================================================

FUNCTION HDRSRCHOFS_MEAS_CONFIG_PARAMS_STORE

DESCRIPTION
  Store parameters related to IRAT Measurement configuration

DEPENDENCIES
  None 

PARAMETERS

  
RETURN VALUE
  None

SIDE EFFECTS
  

===========================================================================*/

void hdrsrchofs_meas_store_config_params
(
    const hdrsrch_lte_pilot_meas_params_type *params 
);
#endif /* FEATURE_HDR_LTE_TO_EHRPD_IRAT */

/* EJECT */
/*==========================================================================

FUNCTION        HDRSRCHTC_REGISTER_VOIP_OUTAGE_CB

DESCRIPTION
  This registers VOIP outage notification function with
  traffic state.
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrsrchtc_register_voip_outage_cb
( 
  hdrsrch_voip_outage_notify_cb_type    voip_outage_cb
);

/* EJECT */
/*==========================================================================

FUNCTION        HDRSRCHTC_CLEAR_VOIP_OUTAGE_CB

DESCRIPTION
  This clears VOIP outage notification function pointer.
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrsrchtc_clear_voip_outage_cb( void );

/* EJECT */
/*===========================================================================

FUNCTION HDRSRCHOFS_REPETITION_REQUIRED

DESCRIPTION
  This function returns the flag that indicates 
  whether repetitions for RUP or OFS search are pending.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  Returns the flag whether repetitions for current RUP or OFS search are pending

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrsrchofs_repetition_required( void );

/* EJECT */
/*===========================================================================

FUNCTION HDRSRCHOFS_RESET_REPEAT_COUNT

DESCRIPTION
  This function reset repetition count for OFS and RUP searches.
    
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchofs_reset_repeat_count( void );

#ifdef FEATURE_HDR_DDARF

/* EJECT */
/*===========================================================================

FUNCTION HDRSRCHOFS_REQUEST_IDLE_OFS

DESCRIPTION

  This function provides an interface to register an idle OFS request 
  to OFS state. So that an idle OFS can be performed regardless of 
  whether signal conditions for idle OFS is satisfied or not.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchofs_request_idle_ofs( void );

#endif /* FEATURE_HDR_DDARF */

/* EJECT */
/*===========================================================================

FUNCTION HDRSRCHOFS_RESET_IDLE_OFS_TIME

DESCRIPTION

  This function initializes the last idle ofs time stamp to 
  HDRSRCHOFS_IDLE_OFS_INTERVAL_MS before the current time 
  so that an OFS search is not skipped due to OFS 
  interarrival duration limitation.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchofs_reset_idle_ofs_time( void );

/* EJECT */
/*===========================================================================

FUNCTION HDRSRCHOFS_INIT_OFS_TIMELINE

DESCRIPTION

  This function initializes the last idle ofs time stamp to 
  HDRSRCHOFS_IDLE_OFS_INTERVAL_MS before the current time 
  so that an OFS search is not skipped due to OFS 
  interarrival duration limitation.
  
  It also resets state flags related to OFS and RUP search.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchofs_init_ofs_timeline( void );

/* EJECT */
/*==========================================================================

FUNCTION        HDRSRCHTC_UPDATE_TC_OFS_TIME

DESCRIPTION
  This function records the last OFS start time.
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrsrchtc_update_tc_ofs_time
(
  timetick_type          sclks_now
    /* current sclks  count */
);

/* EJECT */
/*==========================================================================

FUNCTION        HDRSRCHTC_UPDATE_TC_OFS_GROUP_TIME

DESCRIPTION
  This function records the last OFS search group start time.
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrsrchtc_update_tc_ofs_group_time
(
  timetick_type          sclks_now
    /* current sclks  count */
);

/* EJECT */
/*==========================================================================

FUNCTION        HDRSRCHTC_UPDATE_TC_OFS_CHECK_TIME

DESCRIPTION
  This function records the time for last TC OFS condition evaluation.
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrsrchtc_update_tc_ofs_check_time
(
  timetick_type          sclks_now
    /* current sclks  count */
);

#ifdef FEATURE_HDR_WUTA
/* EJECT */
/*===========================================================================

FUNCTION HDRSRCHSLEEP_ADJUST_RF_WARMUP_ON_TUNE_DONE

DESCRIPTION
  This function is called when the RF tune is completed. This call can only 
  increase the warmup time if it notices the RTC is already on.  

DEPENDENCIES
  hdrsrchsleep_adjust_rf_warmup_on_rtc_on must be called when the RTC on 
  interrupt fires.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates rf_warmup structure to reflect current timing of the tune operation
  completing.

===========================================================================*/

extern void hdrsrchsleep_adjust_rf_warmup_on_tune_done( void );
#endif /* FEATURE_HDR_WUTA */

/* <EJECT> */
/*===========================================================================

FUNCTION    HDRSRCHOFS_NOTIFY_NSET_UPDATE

DESCRIPTION
  This function notifies OFS state that NSET has been updated. 
  So that OFS state can update the list of o-freq neighbor pilots 
  when the new nlist has been received from the AT.

DEPENDENCIES
  Should be called after the nset is updated with the new nlist

PARAMETERS

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void hdrsrchofs_notify_nset_update( void );


/*==========================================================================

FUNCTION        HDRSRCHTIMESYNC_INIT_TIMESYNC_STATE

DESCRIPTION
  This function switches the HDR Searcher state to TimeSync.

DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrchtimesync_init_timesync_state( void ); 

/*==========================================================================

FUNCTION        HDRSRCHTIMESYNC_GET_TIME_SNAPSHOT

DESCRIPTION
  This function returns the snapshot of CDMA time and LTE time.

DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrchtimesync_get_time_snapshot( hdrsrch_cdma_and_lte_time_struct * );

/*==========================================================================

FUNCTION        HDRSRCHTIMESYNC_GET_RTC_OFFSET

DESCRIPTION
  This function returns the rtc offset after time transfer is done

DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  RTC offset

SIDE EFFECTS
  None

==========================================================================*/
uint32 hdrsrchtimesync_get_rtc_offset( void );

/*==========================================================================

FUNCTION        HDRSRCHTIMESYNC_SET_CB

DESCRIPTION
  This function set the call back for time transfer command

DEPENDENCIES
  None

INPUTS
  cb_func  - call back function

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrsrchtimesync_set_cb( hdrsrch_tran_sys_time_cb_type );

/*===========================================================================

FUNCTION HDRSRCHOFS_MEAS_AGC_LIST_INIT

DESCRIPTION
  This function initializes the agc accum list.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrsrchofs_meas_agc_list_init( void );

/* EJECT */
/*=============================================================================

FUNCTION HDRSRCHSLEEP_INCREMENT_WAKEUP_SCI

DESCRIPTION
  Increment the SCI for sleep slot.

DEPENDENCIES
  NONE

RETURN VALUE
  TRUE if sleep SCI is incremented. FALSE otherwise.

SIDE EFFECTS
  None

=============================================================================*/
boolean hdrsrchsleep_increment_wakeup_sci( void );


/* EJECT */
/*=============================================================================

FUNCTION HDRSRCHSLEEP_DECREMENT_WAKEUP_SCI

DESCRIPTION
  Decrement the SCI for sleep slot.

DEPENDENCIES
  NONE

RETURN VALUE
  TRUE if sleep SCI is decremented. FALSE otherwise.

SIDE EFFECTS
  None

=============================================================================*/
boolean hdrsrchsleep_decrement_wakeup_sci( void );


/*==========================================================================

FUNCTION HDRSRCHSLEEP_RESET_NEW_WAKEUP_SCI

DESCRIPTION
  This function sets the new wakeup SCI to the dafult 9.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
==========================================================================*/
void hdrsrchsleep_reset_new_wakeup_sci( void );

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHREACQ_RESET_WINSIZEFACTOR

DESCRIPTION
  This function sets the winsizefactor field in hdrsrchreaq to 1.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/	 	 
void hdrsrchreacq_reset_winsize_fctr( void );

/* <EJECT> */
/*===========================================================================

FUNCTION HDRSRCHREACQ_CALC_MAX_WIN_FACTOR

DESCRIPTION
  This function caculates the factor required to maximize a window size
  and saves it to the max win size fac field in hdrsrchreacq.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/	 	 
void hdrsrchreacq_calc_max_win_factor
( 
   hdrsrch_winsize_enum_type winsize 
    /* window size enum */
);

/*==========================================================================

FUNCTION HDRSRCHSLEEP_GET_SLEEP_SUB_STATE

DESCRIPTION
  This function returns the value of the current sleep sub state.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  The value of current sleep sub state (hdrsrch_sleep_sub_state_enum_type). 

SIDE EFFECTS
  None
  
==========================================================================*/

hdrsrch_sleep_sub_state_enum_type hdrsrchsleep_get_sleep_sub_state( void );


/*==========================================================================

FUNCTION HDRSRCHSLEEP_SET_SLEEP_SUB_STATE

DESCRIPTION
  This function sets the value of the current sleep sub state.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  None 

SIDE EFFECTS
  None
  
==========================================================================*/

void hdrsrchsleep_set_sleep_sub_state 
(
    hdrsrch_sleep_sub_state_enum_type sleep_sub_state
);


/*==========================================================================

FUNCTION HDRSRCHSLEEP_SET_SLEEP_TIMELINE_DEFAULT

DESCRIPTION
  This function sets the sleep timeline optmized or extended.

DEPENDENCIES
  None

PARAMETERS
  None
  
RETURN VALUE
  TRUE:  optimized timeline
  FALSE: extended timeline

SIDE EFFECTS
  None
  
==========================================================================*/
void hdrsrchsleep_set_sleep_timeline_default
(
  boolean            optimized
    /* Whether to use optimized sleep timeline */
);


/*===========================================================================

FUNCTION HDRSRCHSLEEP_CHECK_SLACK_ON_TUNE_DONE

DESCRIPTION
  This function checks how much warmup overuun.  

DEPENDENCIES
  None
PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrsrchsleep_check_slack_on_tune_done( void );


/*==========================================================================

FUNCTION HDRSRCHSLEEP_TIME_UNTIL_TUNE_COMPLETE

DESCRIPTION
  If the HDR tune is happening within 1x tune duration, return the time that tune will be 
  completed. Otherwise, return 0ms.

DEPENDENCIES
  None

PARAMETERS
 tune_duration_1x: 1x tune duration in usec

RETURN VALUE
  0: There is no conflict between 1x and HDR RF tune.
  >0 : There is conflict and the time in usec until the HDR tune completes 

SIDE EFFECTS
  None

==========================================================================*/
uint32 hdrsrchsleep_time_until_tune_complete
(
  uint32 tune_duration_1x_us
    /* 1x tune duration in usec*/
);

#ifdef FEATURE_HDR_DYNAMIC_IDLE_DIV
/*==========================================================================

FUNCTION      HDRSRCHSLEEP_DEACTIVATE_SCC_DECODE_IND

DESCRIPTION
  This function sends the message to the firmware to deactivate scc decode
  indication.
  
DEPENDENCIES
  None 

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrchsleep_deactivate_scc_decode_ind( void );

/*==========================================================================

FUNCTION      HDRSRCHSLEEP_ACTIVATE_SCC_DECODE_IND

DESCRIPTION
  This function sends the message to the firmware to send
  scc decode indication for m slots.
  
DEPENDENCIES
  None 

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

void hdrsrchsleep_activate_scc_decode_ind ( void );
#endif /* FEATURE_HDR_DYNAMIC_IDLE_DIV */


/*==========================================================================

FUNCTION HDRSRCHSLEEP_GET_CUR_WARMUP_REACQ_TIME

DESCRIPTION
  This function returns the sum of the current warmup time and reacq time.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  warmup + reacq time in us 

SIDE EFFECTS
  None

==========================================================================*/
uint32 hdrsrchsleep_get_cur_warmup_reacq_time( void );

#ifdef FEATURE_HDR_FL_DATA_ADAPTIVE_RXD
/*===========================================================================

FUNCTION HDRSRCHTC_IS_FIXED_DRC

DESCRIPTION
  This function checks if DRC is being overriden by uP.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  True  - if DRC is set to a fixed value
  False - otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrsrchtc_is_fixed_drc( void );
#endif /* FEATURE_HDR_FL_DATA_ADAPTIVE_RXD */

/*==========================================================================

FUNCTION        HDRSRCHTC_OFS_ALLOWED

DESCRIPTION
  This function checks if OFS is allowed (set by TAP)
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
boolean hdrsrchtc_ofs_allowed( void );

/*==========================================================================

FUNCTION hdrsrchsleep_get_cur_warmup_time

DESCRIPTION
  This function returns the current warmup time.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  warmup in us 

SIDE EFFECTS
  None

==========================================================================*/
uint32 hdrsrchsleep_get_cur_warmup_time( void );

/*==========================================================================

FUNCTION        HDRSRCHTC_TUNE_AWAY_ALLOWED

DESCRIPTION
  This function checks if tune away is allowed
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
boolean hdrsrchtc_tune_away_allowed( void );


/*==========================================================================

FUNCTION        HDRSRCHTC_GET_PRIMARY_TA_TIME

DESCRIPTION
  This function return the primary chain tune away time (in sclk).

DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  Primary chain tune away time (end_time_sclk) .

SIDE EFFECTS
  None

==========================================================================*/
uint32 hdrsrchtc_get_primary_ta_time( void );


/*==========================================================================

FUNCTION HDRSRCHSLEEP_UPDATE_PREV_CC_START

DESCRIPTION
  This function updates prev_cc_start with cc_start

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None 

SIDE EFFECTS
  None

==========================================================================*/
void hdrsrchsleep_update_prev_cc_start(void);


/*=============================================================================

FUNCTION HDRSRCHSLEEP_SLEEP_NOW

DESCRIPTION
  Check if it is necessary to sleep now.

DEPENDENCIES
  NONE

RETURN VALUE
  TRUE if sleep SCI is incremented.

SIDE EFFECTS
  None

=============================================================================*/
boolean hdrsrchsleep_sleep_now( void );

#ifdef FEATURE_HDR_WAKEUP_MANAGER
/*=========================================================================

FUNCTION    HDRSRCHSLEEP_WAKEUP_MANAGER_CB

DESCRIPTION
  Call back to inform HDR about any collision during the next wakeup.

DEPENDENCIES
  None

INPUTS  
  rat - RAT ID.
  result -The enum with the result for wakeup.
  transition - Layer1 transitions.
  reserved -  Reserved for enhancements.
  user_data- User defined data if any. 

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrsrchsleep_wakeup_manager_cb
(
  wmgr_client_enum_t rat,
    /* RAT ID */
  
  wmgr_result_type result,
    /* The enum with the result */
  
  uint32 transition,
    /* Layer1 transitions */
  
  uint32 reserved,
    /* Reserved for enhancements */
  
  void *user_data
    /* User defined data */

);
#endif /* FEATURE_HDR_WAKEUP_MANAGER */

#ifdef FEATURE_HDR_PAGE_BLOCKING_MGMT
/*============================================================================

FUNCTION HDRSRCHSLEEP_RESERVE_TRM_AT

DESCRIPTION
  Specifies that the caller needs the given RF resource at the given
  time, for the given duration, for the supplied reason.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the caller currently holds an RF chain, the chain is released.

============================================================================*/
void hdrsrchsleep_reserve_trm_at 
(
  hdrsrchrf_logical_chain_enum_t        chain_id,
    /* The logical chain ID associated with the request */

  hdrsrchrf_resource_enum_t             resource,
    /* The RF resource which is being requested */

  hdrsrchrf_time_t                      when,
    /* When the resource will be needed (sclks timestamp) */

  hdrsrchrf_duration_t                  duration
    /* How long the resource will be needed for (in sclks) */
);


/*============================================================================

FUNCTION HDRSRCHSLEEP_IS_WAKEUP_AT_NEG_SCI_BOUNDARY

DESCRIPTION
  Checks if the wakeup is at a SCI boundary.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE if the wakeup is at a SCI boundary else FALSE.

SIDE EFFECTS
  None 
============================================================================*/
boolean hdrsrchsleep_is_wakeup_at_neg_sci_boundary( void );

#endif /* FEATURE_HDR_PAGE_BLOCKING_MGMT */

#ifdef FEATURE_HDR_QTA
/*==========================================================================

FUNCTION        HDRSRCHTC_IS_QTA_ENABLED

DESCRIPTION
  This function returns whether we are in QTA currently.

DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  TRUE: in QTA
  FALSE: in LTA

SIDE EFFECTS
  None

==========================================================================*/
boolean hdrsrchtc_is_qta_enabled( void );

/*==========================================================================

FUNCTION        HDRSRCHTC_SET_QTA_NV

DESCRIPTION
  This function sets QTA NV status.

DEPENDENCIES
  None

INPUTS
  If QTA is enabled via NV or not.

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrsrchtc_set_qta_nv( boolean is_enabled );

/*==========================================================================

FUNCTION        HDRSRCHTC_SET_QTA_END

DESCRIPTION
  This function sets QTA end.

DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrsrchtc_set_qta_end( void );

#endif /* FEATURE_HDR_QTA */

/*============================================================================
FUNCTION HDRSRCHSLEEP_GET_UNIQUE_WAKEUP_ID

DESCRIPTION 
  Return a unique id for every paging cycle to TRM 

DEPENDENCIES 
  None

RETURN VALUE
  Return unqiue wakeup id for every paging cycle

SIDE EFFECTS


============================================================================*/

uint64 hdrsrchsleep_get_unique_wakeup_id( void );

#ifdef FEATURE_HDR_QTA
/*===========================================================================

FUNCTION      HDRSRCHSUS_PROCESS_RXAGC_TRK_RSP

DESCRIPTION
  Process RX AGC track response received from FW during QTA tuneback.

DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void hdrsrchsus_process_rxagc_trk_rsp ( void );

#ifdef FEATURE_HDR_QTA_THRU_TRM
/*===========================================================================

FUNCTION      HDRSRCHSUS_PROCESS_QUERY_RSP

DESCRIPTION
  Process query response received from FW when L1 queries FW for
  status of the previous gap.

DEPENDENCIES
  None

INPUTS
  QTA status indicating if the previous gap was an empty gap
  or a gap completely used by GSM for page decode

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void hdrsrchsus_process_query_rsp ( hdrfw_qta_status_rsp_t qta_status );
#endif /* FEATURE_HDR_QTA_THRU_TRM */

/*===========================================================================

FUNCTION      HDRSRCHSUS_IS_PRI_CHAIN_SETTLED

DESCRIPTION
  This function returns whether DO L1 received RX AGC track response
  from FW during QTA tuneback.

DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  TRUE: Yes response has been received.
  FALSE: Still waiting on response.

SIDE EFFECTS
  None.

===========================================================================*/

boolean hdrsrchsus_is_pri_chain_settled ( void );

/*==========================================================================

FUNCTION        HDRSRCHTC_GET_QTA_RXLM_BUF_INDEX

DESCRIPTION
 This function returns RXLM buffer index which is allocated during QTA

DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  RXLM Buffer index allocated during QTA

SIDE EFFECTS
  None

==========================================================================*/
uint32 hdrsrchtc_get_qta_rxlm_buf_index( void );


/*===========================================================================

FUNCTION HDRSRCHTC_IS_TA_STARTED

DESCRIPTION
  This function returns tune away status in traffic. This will be helpful in 
  stopping connection close processing while tuning away in traffic.

DEPENDENCIES 
  None. 

PARAMETERS
  None.

RETURN VALUE
  TRUE  - if tune away processing has started.
  FALSE - otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean hdrsrchtc_is_ta_started( void );


/*===========================================================================

FUNCTION HDRSRCHTC_CHANGE_SUBSTATE

DESCRIPTION
  This function changes the substate of hdrsrchtc

DEPENDENCIES 
  None. 

PARAMETERS
  Sub-State

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/

void hdrsrchtc_change_substate( boolean connection_close_state );


/*==========================================================================
 
FUNCTION        HDRSRCHTC_DISABLE_TX_QTA

DESCRIPTION
  This function disables Tx after QTA.

DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrsrchtc_disable_tx_qta( void );

#endif /* FEATURE_HDR_QTA */

#endif /* HDRSRCHSTATES_H */
