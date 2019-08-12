#ifndef TX_H
#define TX_H

/*===========================================================================

          M A I N   T R A N S M I T   T A S K   D E F I N I T I O N S

DESCRIPTION
  This file contains global declarations and external references
  for the Main Transmit task.

  Copyright (c) 1992 - 2015 Qualcomm Technologies, Inc. 
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
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/tx.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/26/09   jj      Split this file as part of CMI Phase2.
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

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION TX_TASK

DESCRIPTION
  This procedure is the entrance procedure for the Main Transmit task.
  It contains the main processing loop for the Main Transmit task which
  controls the activation of the ACPTX and TXC subtasks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void tx_task (
  dword dummy
    /* Required for REX, ignore */
);

#endif

