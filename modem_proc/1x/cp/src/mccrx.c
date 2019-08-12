/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        C D M A    M A I N    C O N T R O L    S U B T A S K
            R E C E I V E   T A S K  I N T E R F A C E

GENERAL DESCRIPTION
  This module contains the queues used for communication
  from the Receive task to the CDMA Main Control Subtask.

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

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

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccrx.c_v   1.0.2.0   30 Nov 2001 17:13:56   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccrx.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/04/98   pms     Neared the variables to save ROM space.
07/20/92   jai     Created module.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "rex.h"
#include "queue.h"
#include "mccrx.h"


/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Buffers to be placed on the mcc_rx_free_q */
#define                        MCCRX_NUM_RPT_BUFS 3
mccrx_rpt_type mccrx_rpt_pool[ MCCRX_NUM_RPT_BUFS ];

/* queue for free buffers */
q_type mcc_rx_free_q;

/* Queue for reports from RXC to MCC */
q_type          mcc_rx_rpt_q;




/*===========================================================================

FUNCTION MCCRX_INIT

DESCRIPTION
  This function initializes the queues between the Receive task and the CDMA
  Main Control task.  It also places buffers on the free queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccrx_init( void )
{
  word i;
    /* index through array of free buffers */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ( void )q_init( &mcc_rx_rpt_q );
  ( void )q_init( &mcc_rx_free_q );

  /* Fill mcc_rx_free_q */
  for( i=0; i< MCCRX_NUM_RPT_BUFS ; i++ )
  {
    mccrx_rpt_pool[i].hdr.rpt_hdr.done_q_ptr = &mcc_rx_free_q;
    q_put( &mcc_rx_free_q, q_link( &mccrx_rpt_pool[i],
           &mccrx_rpt_pool[i].hdr.rpt_hdr.link ) );
  }

}/* mccrxtx_init */
