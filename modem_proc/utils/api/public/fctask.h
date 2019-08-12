#ifndef FC_TASK_H
#define FC_TASK_H
/*============================================================================================

          F L O W     C O N T R O L L E R    T A S K    H E A D E R    F I L E

DESCRIPTION
  This file contains all of the definitions necessary to interface with flow controller task.

Copyright (c) 2006-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================================*/

/*============================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/public/fctask.h#1 $ $DateTime: 2016/12/13 07:57:51 $ $Author: mplcsds1 $

when       who    what, where, why
------     ---    ----------------------------------------------------------------------------
05/21/10   pj     Added new signal definitions for handling THERMAL FC events
08/24/09   kvk    Added support for QXDM watermark tracing
06/04/09   kvk      Added support for QXDM Q6 sleep and All wait profiling
11/10/09   pj     Removing "customer.h" inclusion.
11/02/09   ssg    Mainlining "customer.h" inclusion.
10/05/09   pj     Mainlined fc_task() & moved fc_wait() to fctaski.h
05/20/09   pj     Modified signal values of FC_CCM_CPU_SIG & FC_CCM_MEM_SIG to make them 
                  Bit-mapped
12/10/08   sz     Added signals FC_CCM_CPU_SIG and FC_CCM_MEM_SIG.
08/29/06   gsc    Added NV support.
08/09/06   gsc    Initial Revision


============================================================================================*/
#include "comdef.h"

/*============================================================================================
                        FLOW CONTROLLER SIGNAL DEFINITIONS
============================================================================================*/
#define     FC_DOG_RPT_TIMER_SIG    0x0001
#define     FC_SLEEP_RPT_SIG        0x0002
#define     FC_CMD_Q_SIG            0x0004
#define     FC_SUSPEND_SIG          0x0008
#define     FC_INACTIVE_SIG         0x0010
#define     FC_NV_CMD_SIG           0x0020
#define     FC_Q6_PROF_SIG          0x0040
#define     FC_WM_CMD_Q_SIG         0x0080
#define     FC_DSM_CMD_Q_SIG        0x0100
#define     FC_DSM_WM_TRACE_RPT_SIG 0x0200
#define     FC_CCM_CPU_SIG          0x0400 
#define     FC_CCM_MEM_SIG          0x0800 
#define     FC_CCM_THERM_SIG        0x1000



/*============================================================================================
                        FLOW CONTROLLER COMMAND DEFINITIONS
============================================================================================*/



/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION:  fc_task

DESCRIPTION:
  This function handles FC task processing.
  
DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None

===========================================================================*/
extern void fc_task( dword dummy );

#endif /* FC_TASK_H */
