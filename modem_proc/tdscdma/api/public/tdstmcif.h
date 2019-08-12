#ifndef TDSTMCIF_H
#define TDSTMCIF_H

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/api/public/tdstmcif.h#1 $
$DateTime: 2016/12/13 07:58:19 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/07/09   rmsd    Renamed is_sleep_drv_in_sleep(void) to tds_is_in_sleep(void).
                   Defined it here because it is needed outside modem (by dog.c,dogtest.c)
08/06/09   rm      Mainlined all features as part of CMI Phase 2-defeaturization effort.
03/02/09   rm      Initial Release
===========================================================================*/

#include "comdef.h"
/* --------------------------------- */
/* PUBLIC APIs of machstask.h */
/* --------------------------------- */
#include "queue.h"

/* -------------------------------------- */
/* PUBLIC API of tdsl1mtask.h                 */
/* -------------------------------------- */
/*===========================================================================

FUNCTION    L1_TASK

FILE NAME   l1m.c

DESCRIPTION
  This procedure is the entrance procedure for the L1 task.
  It performs queue initialization and handshake with the Main Control
  task. It then loops forever waiting for commands from the command queues,
  watchdog timer signals, Main Control task signals, and local signals.
  Upon receipt of the signals, it dispatches the command/signal to be
  processed by the appropriate handlers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void tds_l1_task
(
  /* Required for REX, ignore */
  uint32 dummy
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/* -------------------------------------- */
/* PUBLIC API of l2dltask.h               */
/* -------------------------------------- */
/*===========================================================================

FUNCTION        wcdma_l2_dl_task

DESCRIPTION
  This task performs downlink layer 2 processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern  void  tds_l2_dl_task
(
  dword dummy

    /* Required for REX, ignore */
);

/* -------------------------------------- */
/* PUBLIC API of l2ultask.h               */
/* -------------------------------------- */
/*===========================================================================

FUNCTION        wcdma_l2_ul_task

DESCRIPTION
  This task performs uplink layer 2 processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None 

===========================================================================*/
extern  void    tds_l2_ul_task
(
  dword dummy
    /* Required for REX, ignore */
);

/* -------------------------------------- */
/* PUBLIC API of tdsrrctask.h               */
/* -------------------------------------- */
/*===========================================================================

FUNCTION RRC_TASK

DESCRIPTION
  This function is the entry point for RRC Task. It initializes RRC state
  and individual procedures by calling rrc_init(). The ACK signal is sent back
  to TMC and a wait is done on Task Start Signal from TMC. Once a Task Start
  Signal is received, rrc_dispatcher() is called that takes control now onwards.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern  void  tds_rrc_task(dword param);

/* -------------------------------------- */
/* PUBLIC API of machstask.h               */
/* -------------------------------------- */
/*===========================================================================

FUNCTION        wcdma_mac_hs_dl_task

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern  void  tds_mac_hs_dl_task( dword dummy );

/*===========================================================================

FUNCTION WCDMA_IS_IN_SLEEP

DESCRIPTION
    This function returns TRUE or FALSE depending on the last interrupt
    received from the sleep controller. Sleep driver is supposed to be in
    sleep if the last interrupt is SLEEP_START_INT

DEPENDENCIES
    None

RETURN VALUE
    TRUE : if last interrupt is SLEEP_START_INT
    FALSE: all other interrupts other than SLEEP_START_INT

SIDE EFFECTS
    None

===========================================================================*/
extern boolean tds_is_in_sleep(void);

#endif /* TDSTMCIF_H */

