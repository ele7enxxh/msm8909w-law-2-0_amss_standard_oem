#ifndef DRX_H
#define DRX_H
/*===========================================================================

            L 1   D R X   R E L A T E D   D E C L A R A T I O N S

DESCRIPTION

EXTERNALIZED FUNCTIONS

tdsdrx_cmd
  This function is called by L1M when RRC commands L1 to begin DRX
  operation. This function calculates the Paging Occasion and the
  Paging Indicator (PI) value and calls the function to calculate
  the parameters that are needed to send to the mDSP to demodulate
  the PICH.

tdsdrx_rel 
  This function is called to release DRX operation (called when sccpch
  containing pich is released)
  
tdsdrx_sleep_abort
  Aborts Sleep mode if in progress
  
drx_terminate
  This function is called when DRX mode is terminated.
  
drx_process_cmd
  This function processes the DRX sleep local command and implements 
  the DRX state machine
  
tdsdrx_sleep_period_complete
   This function is called to signal that the sleep period is complete and
   its time to wake up.

INTERNALIZED FUNCTIONS
drx_mdsp_parms
  This function calculates the parameters that are needed by the mDSP
  for PICH demodulation:
  piThreshold
  bitsPerPi
  piPnCount
  
  It also calculates the actual SFN and the slot when the mDSP command
  to demodulate the PI bits has to be sent.
  
drx_get_pi_pn_count
  This function calculates the pnCount (in units of 256 chips) in a frame
  where the UE is to begin demodulating the PICH. Range 0...143.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  PICH should be set up before DRX operation.

REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

Copyright (c) (2002-2010) by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/drx.h_v   1.16   11 Jul 2002 21:26:56   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsdrx.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/14/11   weis    Created file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tdscdma_variation.h"
#include "comdef.h"
#include "tdsl1rrcif.h"
#include "tdsdrxcmd.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Convenience symbols to hold various DRX cycle length values in frames */
#define TDSDRX_CYCLE_LENGTH_80MS  8

/* Structure to hold all the parameters necessary for calculating
 * and saving the parameters necessary for PICH demodulation.
 */

/* bplmn srch is supported for a drx cycle length of 32 frames and more */
#define TDSDRX_MIN_DRX_CYCLE_LENGTH_FOR_WTOW_BPLMN 32

/* Minimum number of consequetive DRX Cycles between short-to-long-qxt transitions */
#define TDSDRX_MIN_DXC_FOR_CONSEC_S2L_PULLBACKS 10

/* definition of the states of the DRX/Sleep state machine */
typedef enum 
{
  TDSDRX_IDLE,
  TDSDRX_CONFIGURED,
  TDSDRX_WAITING_ON_SRCH_READY_FOR_SLEEP,
  TDSDRX_WAITING_ON_RF_WARMED_UP,
  TDSDRX_WAITING_ON_BPLMN_COMPLETE,
  TDSDRX_NUM_STATES
} tdsdrx_state_type;

extern tdsdrx_state_type tdsdrx_state;

/* To remember DRX state at the time of a SUSPEND, for use on RESUME. */
extern tdsdrx_state_type tdsdrx_state_at_suspend;

/* drx command saved for use when drx is restarted */
extern tdsl1_drx_cmd_type tdsdrx_cmd_saved;

/*1. max drx cycle length is 5.12s, we may bypass 1 PICH if it's very close. 
     so this timer length should be >= 1 drx cycle length, let's choose 2 
  2. in case of N-BCH decoding, RRC should tear down BCH within 2 s (RRC confirmed)
     because max SIB period is 256 ms . L1 probably bypass PICH as well in this case
     so this time length should be >= 2 s 

  combine 1 and 2, set this timer 10240 ms */
#define TDSDRX_DELAYED_TIMER_VAL_MS (10240)

extern boolean tdsdrx_just_wakeup;
extern boolean tdsdrx_sleep_aborted;


extern boolean tdsdrxmgr_need_reacq_flag;

extern boolean tdsdrx_t2g_bplmn_in_progress;
#ifdef FEATURE_TDSCDMA_DSDS
extern boolean tdsdrx_t2t_bplmn_in_progress;
#endif


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION     DRX_CMD

DESCRIPTION
  This function is called by L1M when RRC commands L1 to begin DRX
  operation. This function calculates the Paging Occasion and the
  Paging Indicator (PI) value and calls the function to calculate
  the parameters that are needed to send to the mDSP to demodulate
  the PICH.

DEPENDENCIES
  None

RETURN VALUE
  True if all went well; False otherwise.

SIDE EFFECTS
  The global variable drx_parms is updated with the computed values.

===========================================================================*/
extern boolean tdsdrx_cmd( tdsl1_drx_cmd_type* cmd );

/*===========================================================================

FUNCTION     DRX_RESTART

DESCRIPTION
  Restarts the DRX state machine with previous DRX command parameters.

DEPENDENCIES
  None

RETURN VALUE
   None

SIDE EFFECTS


===========================================================================*/
extern void tdsdrx_restart(void);

/*===========================================================================

FUNCTION     DRX_REL

DESCRIPTION
  This function is called to stop drx operation

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void tdsdrx_rel(void);

/*===========================================================================

FUNCTION       DRX_LOG_INFO

DESCRIPTION
  This function logs the drx info collected during the last PICH done
  mdsp interrupt. This function is called in task context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsdrx_log_info( void );

/*===========================================================================

FUNCTION     DRX_WAKE_UP

DESCRIPTION
  This function is called to prepare L1 for wakeup

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void tdsdrx_wake_up( void );

/*===========================================================================

FUNCTION       DRX_SLEEP_ABORT
DESCRIPTION
  This function initiates the abort of sleep mode if already in progress

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsdrx_sleep_abort( void );

/*===========================================================================
FUNCTION     DRX_IS_AWAKE_FOR_TCXO_SETTLING

DESCRIPTION
  This function returns whether or not we are intentionally staying awake
  during DRX cycles to allow VCTCXO to settle.  This gives the mDSP a chance
  to adjust trk_lo_adj based upon downlink pilot and drive the VCTCXO
  frequency to a more accurate value.  We do this for approximately 10
  seconds after power up.  This is done to help GSM, in the event that it
  doesn't have rotator support.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE  - We are allowing VCTCXO to track.
  FALSE - Tracking is complete.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean tdsdrx_is_awake_for_tcxo_settling(void);

/*===========================================================================

FUNCTION     DRX_CHK_L1_GO_TO_SLEEP

DESCRIPTION
  This function calls the RRC query function and checks whether a WTOW BPLMN
  scan needs to be intiated.  It also checks whether a WTOG BPLMN search is
  active and monitors the GSM CLK. If the GSM clock is enabled and there are
  no GSM neighbors and the WTOG BPLMN search is not in progress then turn off
  the GSM clock.  The RRC query function returns a FALSE if a WTOW BPLMN
  search can be performed and returns TRUE if no search can be performed or a
  WTOG search can be performed.
  
DEPENDENCIES
  None.

RETURN VALUE
  boolean - TRUE:  Either no WTOW search or WTOG BPLMN search.
          \ FALSE: WTOW BPLMN search can be started.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean tdsdrx_chk_l1_go_to_sleep(uint32 ms_to_sleep);


/*===========================================================================

FUNCTION     DRX_PROCESS_COMMAND

DESCRIPTION
  This function implements the state machine of DRX/Sleep. It processes the
  local commands of type TDS_DRX_SLEEP_CMD.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void tdsdrx_process_command(const tdsdrxcmd_sleep_cmd_type * cmd);

/*===========================================================================

FUNCTION     DRX_BCH_SETUP

DESCRIPTION
  This function is called when a BCH is setup. It sets a flag in the drx module.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void tdsdrx_bch_setup(void);

/*===========================================================================

FUNCTION     DRX_BCH_TEAR_DOWN

DESCRIPTION
  This function is called when BCH is torn down. It resets a flag in the drx module.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void tdsdrx_bch_tear_down(void);

/*===========================================================================

FUNCTION     DRX_SLEEP_PERIOD_COMPLETE

DESCRIPTION
  This function is called when the sleep period is over. It calls 
  a function in the sleep module to wake up.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void tdsdrx_sleep_period_complete(void);

/*===========================================================================

FUNCTION     DRX_BCH_ON

DESCRIPTION
  This function returns true if BCH was setup and FALSE otherwise.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern boolean tdsdrx_bch_on(void);

/*===========================================================================

FUNCTION     DRX_DELAYED

DESCRIPTION
  This function handles the recovery from DRX getting stuck for an extended period.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void tdsdrx_delayed(void);

/*===========================================================================

FUNCTION     tdsdrx_new_drx_cycle

DESCRIPTION
  This function is called to kick in the next DRX cycle

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern void tdsdrx_new_drx_cycle(void);


/*===========================================================================

FUNCTION     SET_DRX_STATE

DESCRIPTION
  This function is called to change the state of the DRX module.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void tdsdrx_set_drx_state(tdsdrx_state_type newState);

/*===========================================================================
FUNCTION     DRX_QXDM_DIPSW_DISABLES_SLEEP

DESCRIPTION
  This function checks the QXDM DIP switch settings to see if the user has
  disabled sleep mode operation.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE if sleep is disabled via QXDM DIP switch setting.  FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean tdsdrx_qxdm_dipsw_disables_sleep(void);

#endif /* DRX_H */
