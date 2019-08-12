#ifndef TX_V_H
#define TX_V_H

/*===========================================================================

          M A I N   T R A N S M I T   T A S K   D E F I N I T I O N S

DESCRIPTION
  This file contains global declarations and external references
  for the Main Transmit task.

  Copyright (c) 1992 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/tx.h_v   1.0.2.0   30 Nov 2001 17:46:32   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/tx_v.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/19/14   bph     Make callbacks registered with TRM asynchronous 
01/20/14   srk     ASDIV Cleanup - Remove TXC wait signal used for ASDIV on TC exit
03/05/13   srk     Replace existing watchdog implementation with the new
                   Dog Heartbeat Mechanism and cleanup unused signals.
11/30/12   vks     Add support for 1x asdiv idle mode operation.
08/18/11   adw     Mainline 1x message router support.
11/12/09   vks     Add TXC_MUXMSGR_Q_SIG signal for MSGR/Q6 framework support
05/26/09   jj      Modified the file as part of CMI Phase-II
05/14/09   ag      Split the file as part of CMI phase-II
04/06/09   mca     Added sigs for RF SVDO API
03/15/05   vlc     Mainlined TXC_TX_SHUTDOWN_SIG.
09/01/04   sr      Defined TXC_MDSP_DV_CHAN_TERM_SIG
08/17/04   sr      Defined TXC_MDSP_DV_APP_READY_SIG
05/28/04   bkm/ll  Define TXC_10MS_TICK_SIG.
02/10/04   az      Added TXC_ACTION_TIME_SIG for action time notification
09/15/03   jrp     Added signal TXC_TX_SHUTDOWN_SIG for feature
                   FEATURE_DELAYED_TX_CLOCK_SHUTDOWN.
01/12/02   fc      Fixed compiler warnings.
06/11/93   jca     Added TXC_RF_TIMER_SIG.
07/15/92   jai     Deleted TXC_STOP_SIG (not used).
07/08/92   jca     Deleted references to TXCDMA sigs (prefix is now TXC).
07/07/92   arh     Initial check-in for DMSS
04/16/92   jca     Creation

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "modem_1x_defs.h"

#include "tx.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*-------------------------------------------------------------------------*/
/*             CDMA TRANSMIT SUBTASK (TXC) SIGNAL DECLARATIONS             */
/*-------------------------------------------------------------------------*/

#define TXC_DOG_RPT_SIG                        0x0001
  /* This signal is set by DOG Heartbeat mechanism. */

#define TXC_CMD_Q_SIG                          0x0002
  /* This signal is set when an item is put on the tx_cmd_q. */

#define TXC_INT_SIG                            0x0004
  /* This signal is used for signalling the Transmit Task from the Encoder
     Interrupt Handler */

#define TXC_PN_ROLL_SIG                        0x0008
  /* Lets the transmit task know that a PN roll interrupt has occured - set
     by the Search task and used when the transmit task has to perform a
     timing change before transmitting an access attempt */

#define TXC_INT_CMD_SIG                        0x0010
  /* Lets the transmit ISRs tell the transmit tasks that they have
     performed a command that was given to them. */

#define TXC_RF_TIMER_SIG                       0x0020
  /* Signal associated with timer that allows for delay between turning on
     the transmit portion of the RF hardware and programming the transmit
     synthesizers. */

#define TXC_TX_SHUTDOWN_SIG                    0x0040
  /* Signal to turn off encoder clocks.
  */

#define TXC_ACTION_TIME_SIG                    0x0080
  /* Action time notification for TXC */

#define TXC_TIMER_SIG                          0x0100
  /* TX Task failsafe timer signal */

#define TXC_MUXMSGR_Q_SIG                      0x0200
  /* This signal is set when an item is put in MUXMSGR Q from the MSGR
     framework */

#define TXC_ASDIV_WAIT_SIG                     0x0400
  /* This signal is set when an antenna switch is done */

/* Reserved for future use */
#define TXC_RSVD_0800_SIG                      0x0800

/* Signals 0x1000, 0x2000, 0x4000, and 0x8000 are system wide common and
   are defined in TASK.H */
#define TXC_DO_NOT_USE_SYS_RESERVED_1000_SIG   0x1000
#define TXC_DO_NOT_USE_SYS_RESERVED_2000_SIG   0x2000
#define TXC_DO_NOT_USE_SYS_RESERVED_4000_SIG   0x4000
#define TXC_DO_NOT_USE_SYS_RESERVED_8000_SIG   0x8000

/*-------------------------------------------------------------------------*/
/*             MAIN TRANSMIT TASK SIGNAL DECLARATIONS                      */
/*-------------------------------------------------------------------------*/

/* Subtask Command Queue Signals                                           */

#define TX_CDMA_CMD_Q_SIG   TXC_CMD_Q_SIG
/* Indicates new item on the TXC subtask command queue                     */



/* <EJECT> */
/*===========================================================================

                     PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#endif /* TX_V_H */
