#ifndef DS707_CTA_H
#define DS707_CTA_H
/*===========================================================================

                         D S 7 0 7 _C T A _ R L P
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

 Copyright (c) 2002 by Qualcomm Technologies Incorporated.  All Rights Reserved.
 Copyright (c) 2004-2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_cta_rlp.h_v   1.1   15 Nov 2002 18:25:26   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_cta_rlp.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/03/13   sd      ATCoP decoupling changes
03/30/07   ac      EMPA: Implemented RLP inactivity timer feature. Let every 
				   RLP instance maintain its own inactivity timer.
11/15/05   atp     Moved ds_process_rlp_frames out to RLP.
02/05/05   rsl     Moved cta timer start and stop on tx side to dsrlp.c from ds707_rmsm.c
02/04/04   ak      Updated to use pkt mgr event mechanisms.
11/15/02   ak      Updated comments in file header.
05/10/02   ak      First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "dsm.h"
#include "ds707.h"

#ifdef FEATURE_IS95B_MDR
  #include "mdrrlp.h"
#else
  #include "rlp.h"
#endif 

#include "dsrlpi.h"
#include "dsat707extctab.h"

/*===========================================================================
                     TYPEDEFS & VARIABLES
===========================================================================*/

#define DS707_CTA_IS_IDLE_MASK  0x0000 /* Traffic channel is idle, idle    */
                                       /* timer running. When a call starts*/
                                       /* this is the default bitmask (so  */
                                       /* idle timer starts running as soon*/ 
                                       /* as call comes up                 */
                                       
#define DS707_CTA_RLP_PHYS_LINK        0x0001 /* TC up or down             */
#define DS707_CTA_RLP_TX               0x0002 /* bitmask for RLP Tx        */
#define DS707_CTA_RLP_RX               0x0004 /* bitmask for RLP Rx        */
#define DS707_CTA_SIO_RX               0x0008 /* bitmask for SIO Rx        */


/*---------------------------------------------------------------------------
  Used in the reset() function to start/stop the idle timer.  
---------------------------------------------------------------------------*/
#define DS707_CTA_START_TIMER   TRUE        /* start the timer             */
#define DS707_CTA_STOP_TIMER    FALSE       /* stop the timer              */


/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
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
);

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
);

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
);

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
);

/*===========================================================================
FUNCTION      DS_RLP_DATA_TXED

DESCRIPTION   This function is called from the rlp layer, after the frames
              transmitted in a 20 msec period have been processed.  If the
              input parameter is FALSE, then no data/ctl/init frames have
              been transmitted in the past 20 msec.  Otherwise at least one
              data frame was transmitted.  If data has been txed, then the
              idle timer should be reset, so that it doesn't expire.
  
              This function should be called from the TX task.

DEPENDENCIES  Called only from RLP 3.  Implies only packet data calls.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_rlp_data_txed
(
  boolean     data_frs_txed      /* tells if any non-idle/fill frames txed */
);

/*===========================================================================
FUNCTION      DS707_CTA_RLP_SYNC_TIMEOUT

DESCRIPTION   Called by RLP when there is a sync timeout.  Should bring
              down the traffic channel.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_cta_rlp_sync_timeout(void);

/*===========================================================================
FUNCTION      DS707_CTA_INIT

DESCRIPTION   Called at powerup.  Registers for phys link events.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_cta_init(void);

/*===========================================================================
FUNCTION      DS707_GET_DEFAULT_CTA_VAL

DESCRIPTION   This is for the multi-processcor Remote Procedure Call interface. The 
              function is needed by RLP.

DEPENDENCIES  None

RETURN VALUE  The global variable ds707_cta_val

SIDE EFFECTS  None
===========================================================================*/
dsat_num_item_type ds707_get_default_cta_val(void);

/*===========================================================================
FUNCTION      DS707_ENHANCED_CTA_GO_DORMANT_HDLR

DESCRIPTION   This is for the multi-processcor Remote Procedure Call interface. The 
              function is needed by RLP, if all RLP are in dormant state.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_enhanced_cta_go_dormant_hdlr(void);
#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_CTA_H        */

