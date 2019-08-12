/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        C D M A    M A I N    C O N T R O L    S U B T A S K
            S E A R C H E R  T A S K  I N T E R F A C E

GENERAL DESCRIPTION
  This module contains the queues used for communication
  from the Searcher task to the CDMA Main Control Subtask.

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

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

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccsrch.c_v   1.0.2.0   30 Nov 2001 19:13:54   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccsrch.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/09/16   ppr     1x2GTA Feature changes
01/23/15   agh     Enhance debugging interface between SRCH and CP
10/04/12   srk     Fix compiler warnings.
09/15/12   ppr     FR#2370 - 1x memory optimization changes
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
11/15/10   jtm     Initial SU API split for CP.
05/14/09   ag      CMI phase II update
10/20/04   an      Include mc.h and task.h because of srchmc.h inclusion in
                   mccsrch.h
08/02/04   yll     Added the function to allow Search to set signals for MC
                   when sending Search reports.
09/04/98   pms     Neared the variables to save ROM space.
10/31/96   rdh     Increased buffer size from 5 to 10 to prevent buffer
                   starvation leading to dropped calls and/or fatal errors.

07/16/92   ptw     Created module.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "rex.h"
#include "queue.h"
#include "qw.h"
#include "mccsrch_i.h"
#include "task.h"
#include "mc_i.h"
#include "mc_v.h"
#include "mccdma_i.h"
#include "m1x_diag.h"

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#ifndef FEATURE_1X_CP_MEM_OPT
#define                        MCCSRCH_NUM_RPTS 10
mccsrch_rpt_type mccsrch_bufs[ MCCSRCH_NUM_RPTS ];
/* Buffers to be placed on the mcc_srch_free_q. Size of this
   buffer must consider the competing pilot situation at call
   origination. In dynamic pilot scenarios, up to 3-4 suppressed
   idle handoffs can generated in system access state, plus the
   number of PSMMs which can be generated (up to 4-5) immediately
   after Search enters Traffic channel state. */

q_type          mcc_srch_free_q;
  /* Queue for free buffers to be placed, when filled, on the
     mcc_srch_q queue */
#endif /* !FEATURE_1X_CP_MEM_OPT */

/* Queue definitions */
q_type mcc_srch_q;
  /* Queue holding messages from the Searcher task for the Main control task
     to process  */

/*===========================================================================

FUNCTION MCCSRCH_INIT

DESCRIPTION
  This function initializes the queues between the Searcher task and the CDMA
  Main Control task.  It also places buffers on the free queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsrch_init( void )
{
#ifndef FEATURE_1X_CP_MEM_OPT
  word i;
    /* index through array of free buffers */
#endif /* !FEATURE_1X_CP_MEM_OPT */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ( void )q_init( &mcc_srch_q );

#ifndef FEATURE_1X_CP_MEM_OPT
  ( void )q_init( &mcc_srch_free_q);

  /* Fill mcc_srch_free_q */
  for( i=0; i< MCCSRCH_NUM_RPTS; i++ )
  {
    mccsrch_bufs[i].hdr.rpt_hdr.done_q_ptr = &mcc_srch_free_q;
    q_put( &mcc_srch_free_q, q_link( &mccsrch_bufs[i],
                                &mccsrch_bufs[i].hdr.rpt_hdr.link ));
  }
#endif /* !FEATURE_1X_CP_MEM_OPT */


}/* mccsrch_init */

/*===========================================================================

FUNCTION MCCSRCH_SET_TA_RESERVATION_TIME

DESCRIPTION
  This function allows Search to inform CP of any changes in TA
  reservation time.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsrch_set_ta_reservation_time ( uint32 sclks )
{
  uint32 current_time_sclks;

  /*----------------------------------------------------------------------*/
  /* Get current time in sclks units */
  current_time_sclks = timetick_get();

  M1X_MSG( DCP, LEGACY_HIGH,
          "1xTA:Change in TA res time, Curr_time:%d, Prev_time:%d, Updated_time:%d",
           current_time_sclks,
           cdma.tc_data_ta.ta_next_reg_time_sclks,
           sclks );
           
  /* Set the TA reservation time in sclks */
  cdma.tc_data_ta.ta_next_reg_time_sclks = sclks;
  
  /* Set signal on MC task after updating cdma global var */
  (void) rex_set_sigs ( MC_TCB_PTR, MCC_TA_RESERVATION_TIMER_SIG );
} /* mccsrch_set_ta_reservation_time */


/*===========================================================================

FUNCTION MCCSRCH_SET_SRCH_RPT_SIGNAL

DESCRIPTION
  This function allows the Search to set a signal for MC when sending a
  Search report.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsrch_set_srch_rpt_signal ( void )
{
  /* Sets the MCC_Q_SIG for MC */
  ( void ) rex_set_sigs ( MC_TCB_PTR, MCC_Q_SIG );
}
