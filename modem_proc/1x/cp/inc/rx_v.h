#ifndef RX_V_H
#define RX_V_H

/*===========================================================================

          M A I N   R E C E I V E   T A S K   D E F I N I T I O N S

DESCRIPTION
  This file contains global declarations and external references
  for the Main Receive task.

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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/rx.h_v   1.0.2.0   30 Nov 2001 17:47:34   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/rx_v.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/05/13   srk     Replace existing watchdog implementation with the new
                   Dog Heartbeat Mechanism and cleanup unused signals.
08/18/11   adw     Mainline 1x message router support.
11/12/09   vks     Add RXC_MUXMSGR_Q_SIG signal for MSGR/Q6 framework support
05/26/09   jj      Modified the file as part of CMI Phase-II
05/14/09   ag      Split the file as part of CMI phase-II
05/23/06   fc      Used next RXC reserved signal for RXC_PC_FS_TIMER_SIG.
05/17/04   vlc     Used next RXC reserved signal for RXC_PDCH_INT_SIG to support
                   F-PDCH.
02/10/04   az      Added RXC_ACTION_TIME_SIG for action time notification
01/13/02   fc      Fixed compiler warnings.
11/07/02   az      Mainlined FEATURE_IS95B
06/17/99   kmp     Merged in the one change listed below.
06/07/99   ks      Added RXC_AHO_FADE_TIMER timer for T72M timer.
08/31/92   jjw     Added ACPRX Sound command done signal
08/19/92   jjw     Added ACPRX timer signal to check RSSI during ACPRX
                   NULL_STATE so DB (and UI) will also know signal strength
07/08/92   jca     Deleted references to RXCDMA sigs (prefix is now RXC).
07/07/92   arh     Initial check-in for DMSS
04/16/92   jca     Creation

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "modem_1x_defs.h"

#include "rx.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*-------------------------------------------------------------------------*/
/*             CDMA RECEIVE SUBTASK (RXC) SIGNAL DECLARATIONS              */
/*-------------------------------------------------------------------------*/

#define RXC_DOG_RPT_SIG                        0x0001
  /* This signal is set by DOG  Heartbeat mechanism for RX Task */

#define RXC_FADE_TIMER_SIG                     0x0002
  /* This signal is set when the rx_fade_timer expires. */

#define RXC_CMD_Q_SIG                          0x0004
  /* This signal is set when an item is put on the rx_cmd_q. */
  /*  (NOTE! This sig def must differ from the ACPRX cmd queue sig def.)   */

#define RXC_INT_SIG                            0x0008
  /* This signal is used for signalling the Receive Task from the
     Decoder Interrupt Handler */

#define RXC_ISR_CMD_SIG                        0x0010
  /* This signal is specified when issuing a command to the rx interrupt
     handler */

#define RXC_AHO_FADE_TIMER_SIG                 0x0020
  /* This signal is set as a fade timer when T72M timer expires */

#define RXC_ACTION_TIME_SIG                    0x0040
  /* Action time notification for RXC */

#define RXC_TIMER_SIG                          0x0080
  /* RX Task failsafe timer signal */

#define RXC_MUXMSGR_Q_SIG                      0x0100
  /* This signal is set when an item is put in MUXMSGR Q from the MSGR
     framework */

/* Reserved for future use */
#define RXC_RSVD_0200_SIG                      0x0200
#define RXC_RSVD_0400_SIG                      0x0400
#define RXC_RSVD_0800_SIG                      0x0800

/* Signals 0x1000, 0x2000, 0x4000, and 0x8000 are system wide common and
   are defined in TASK.H */
#define RXC_DO_NOT_USE_SYS_RESERVED_1000_SIG   0x1000
#define RXC_DO_NOT_USE_SYS_RESERVED_2000_SIG   0x2000
#define RXC_DO_NOT_USE_SYS_RESERVED_4000_SIG   0x4000
#define RXC_DO_NOT_USE_SYS_RESERVED_8000_SIG   0x8000

/*-------------------------------------------------------------------------*/
/*             MAIN RECEIVE TASK SIGNAL DECLARATIONS                       */
/*-------------------------------------------------------------------------*/

#define RX_CDMA_CMD_Q_SIG   RXC_CMD_Q_SIG
/* Indicates new item on the RXC subtask command queue                     */

/* <EJECT> */
/*===========================================================================

                     PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#endif /* RX_V_H */
