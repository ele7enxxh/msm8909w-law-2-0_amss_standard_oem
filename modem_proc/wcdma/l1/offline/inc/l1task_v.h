#ifndef L1TASK_V_H
#define L1TASK_V_H

/*===========================================================================

          L 1   T A S K   E X T E R N A L  D E C L A R A T I O N S

DESCRIPTION
This file provides an external declaration for the l1 task execution start
point function wcdma_l1_task.  This file is designed to be included by the UE
task controller so that it may start the L1 task.

EXTERNALIZED FUNCTIONS

  wcdma_l1_task
    This procedure is the entrance procedure for the L1 task.
    It performs queue initialization and handshake with the Main Control
    task. It then loops forever waiting for commands from the command queues,
    watchdog timer signals, Main Control task signals, and local signals.
    Upon receipt of the signals, it dispatches the command/signal to be
    processed by the appropriate handlers.

  l1_get_cmd_buf
    This function returns a free command buffer, if available, or NULL if
    no command buffers are available.

  l1_put_cmd
    This function is used by the external clients of the L1 to submit
    commands to the L1. The caller owns the pointer to the command. 
    This function is called in the context of the caller. The function
    puts the command onto the External Command Queue and sets the
    External Command Queue signal of the L1 task.

  l1_cphy_setup_cnf_get_status
    This function checks the operations status for a selected type and
    channel in the command packet of a CPHY_SETUP_CNF indication.
    This function returns the status of the requested operation as
    denoted in the indication packet, additionally the operation type is
    returned as an output parameter.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  The wcdma_l1_task function contains the L1 Manager task.  It must be
  specified in a call to rex_def task and started by a Task Controller.

  Copyright (c) 2000-2009 by Qualcomm Technologies, Inc.
  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/l1task.h_v   1.13   19 May 2002 03:29:12   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/l1task_v.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/25/11   geg     Separated l1task.h into l1task.h and l1task_v.h
12/02/10   geg     Created file  
============================================================================*/

/*===========================================================================

                      INCLUDE FILES

===========================================================================*/

#include "wcdma_variation.h"
#include "l1rrcif.h"
#include "l1macif.h"
#include "l1task.h"

/*===========================================================================
FUNCTION        L1_CPHY_SETUP_CNF_GET_STATUS

FILE NAME       l1m.c

DESCRIPTION     This function checks the operations status for a selected
                type and channel in the command packet of a CPHY_SETUP_CNF
                indication.

DEPENDENCIES    None

RETURN VALUE    the status of the requested operation as denoted in the 
                indication packet, additionally the operation type is
                returned as an output parameter.

SIDE EFFECTS    The ouput parameter *op is updated
===========================================================================*/
boolean l1_cphy_setup_cnf_get_status
(
  /* pointer to the CPHY_SETUP_CNF indication packet */
  l1_setup_cnf_type *cnf,

  /* the type of status, DL_TRCH, DL_PHYCHAN, UL_TRCH, UL_PHYCHAN requested */
  l1_cphy_setup_cnf_stat_type_enum_type stat_type,

  /* the channel type, just cast the associated channel enum to a uint8 */
  uint8 ch,

  /* output parameter for the operation that was performed (including NO OP,
     on the requested channel */
  l1_setup_ops_enum_type *op
);
/*===========================================================================
FUNCTION        L1_PROCESS_IMMEDIATE_MEAS_REQ

FILE NAME       l1mcmd.c

DESCRIPTION     This function processes the immediate measurement request
                from RRC.

DEPENDENCIES    None

RETURN VALUE    None
===========================================================================*/
void l1_process_immediate_meas_req (l1_immediate_meas_type *immed_meas, l1_meas_quan_enum_type meas_quan) ;

/*===========================================================================
FUNCTION        L1_IS_S_CRITERIA_MET

FILE NAME       l1mcmd.c

DESCRIPTION     This function computes S criteria based on Qqualmin and
                Qrxlevmin given by RRC

DEPENDENCIES    None

RETURN VALUE    TRUE if S criteria is met, FALSE otherwise
===========================================================================*/
extern boolean l1_is_s_criteria_met( int16 q_qualmin, int16 q_rxlevmin, int16 ue_max_tx_pwr);

#ifdef FEATURE_WCDMA_INTER_FREQ_RACH_REPORTING
#error code not present
#endif

#endif /* L1TASK_V_H */

