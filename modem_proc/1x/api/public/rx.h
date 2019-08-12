#ifndef RX_H
#define RX_H

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
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/rx.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/26/09   jj      Split this file as part of CMI phase2.
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

/* <EJECT> */
/*===========================================================================

                     PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION RX_TASK

DESCRIPTION
  This procedure is the entrance procedure for the Main Receive task.
  It contains the main processing loop for the Main Receive task which
  controls the activation of the ACPRX and RXC subtasks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void rx_task (
  dword dummy
    /* Required for REX, ignore */
);

#endif

