/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        C D M A    M A I N    C O N T R O L    S U B T A S K
            L A Y E R  2  T A S K  I N T E R F A C E

GENERAL DESCRIPTION
  This module contains variables and queues necessary for communication
  from the Layer 2 task to the CDMA Main Control Subtask.

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

  Copyright (c) 1990 - 2013 Qualcomm Technologies, Inc. 
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

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccrxtx.c_v   1.1   08 Jul 2002 16:23:42   louiel  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccrxtx.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/04/12   srk     Fix compiler warnings.
09/15/12   ppr     FR#2370 - 1x memory optimization changes
02/04/12   ppr     Feature Cleanup
05/14/09   ag      CMI phase 2 changes
07/01/02   yll     Added support for MC Queue Watermark
01/15/99   lcc     Changed MCCRXTX_NUM_BUFS back to 5.  Should never have been
                   20.
01/07/99   dna     Merged increase buffers from 5 to 20
09/04/98   pms     Neared the variables to save ROM space.
07/10/96   gb      Increased # of buffers to handle peak latency.
07/01/92   jai     Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "rex.h"
#include "queue.h"
#include "mccrxtx.h"
#include "task.h"
#include "mc_i.h"
#include "mc_v.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#ifndef FEATURE_1X_CP_MEM_OPT
/* Buffers for MC_RXTX_FREE_Q */
#define                         MCCRXTX_NUM_BUFS 5
mccrxtx_cmd_type mcc_rxtx_bufs[ MCCRXTX_NUM_BUFS ];

q_type mcc_rxtx_free_q;
  /* Queue for free buffers to be placed, when filled, on the
     mcc_rxtx_q queue */
#endif /* !FEATURE_1X_CP_MEM_OPT */

/* Queue definitions */
q_type mcc_rxtx_q;
  /* Queue holding messages from the RXTX task for the Main control task
     to process  */

q_type          mcc_rxtx_ack_q;
  /* Queue holding messages for which MC requested to be signaled when
     they were acknowledged */

/*===========================================================================

FUNCTION MCCRXTX_INIT

DESCRIPTION
  This function initializes the queues between the Layer 2 task and the CDMA
  Main Control task.  It also places buffers on the free queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccrxtx_init( void )
{
#ifndef FEATURE_1X_CP_MEM_OPT
  word i;
    /* index through array of free buffers */
#endif /* !FEATURE_1X_CP_MEM_OPT */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ( void )q_init( &mcc_rxtx_q ) ;
  ( void )q_init( &mcc_rxtx_ack_q );
#ifndef FEATURE_1X_CP_MEM_OPT
  ( void )q_init( &mcc_rxtx_free_q );

  /* Fill mc_rxtx_free_q */
  for( i=0; i< MCCRXTX_NUM_BUFS; i++ )
  {
    mcc_rxtx_bufs[i].hdr.cmd_hdr.done_q_ptr = &mcc_rxtx_free_q;
    q_put( &mcc_rxtx_free_q, q_link( &mcc_rxtx_bufs[i],
                                  &mcc_rxtx_bufs[i].hdr.cmd_hdr.link) );
  }
#endif /* !FEATURE_1X_CP_MEM_OPT */
}/* mccrxtx_init */


/*===========================================================================

FUNCTION MCCRXTX_CMD

DESCRIPTION
  This function sends a command to the mcc subtask from the rxtx task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  mccrxtx_cmd_type is placed on the mcc_rxtx_q.

===========================================================================*/

void mccrxtx_cmd
(
  mccrxtx_cmd_type  *cmd_ptr   /* Pointer to MCCRXTX message buffer */
)
{

  /* Set the request status to busy. initialize and link the command  */
  /* onto the MC command queue, set a signal to the MC task and exit. */

  cmd_ptr->hdr.status = MCCRXTX_BUSY_S;
  (void) q_link(cmd_ptr, &cmd_ptr->hdr.cmd_hdr.link);
  q_put(&mcc_rxtx_q, &cmd_ptr->hdr.cmd_hdr.link);
  (void) rex_set_sigs( MC_TCB_PTR, MCC_Q_SIG );

} /* mccrxtx_cmd */
