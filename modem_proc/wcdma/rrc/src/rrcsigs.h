#ifndef RRCSIGS_H
#define RRCSIGS_H
/*===========================================================================
                      R R C    S I G N A L S 

DESCRIPTION

  This module defines Rex Signals used by RRC task. This module
	is internal to the RRC task and is not visible to modules
	outside the RRC task.


Copyright (c) 2000-2004, 2007-2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcsigs.h#1 $   

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/07/15   vi      Changes for force DRX and skip idlemode measurements based on QMI request 
09/10/12   gv      Added support for new interface between WRRC and TDS RRC
12/12/11   gv      Added code to support free floating of tasks
12/05/11   pm      Added code to support cell id request by CM in DCH state"
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
05/08/09   ss      Updated Copyright Information
03/06/08   da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
12/21/07   da      Support for FEATURE_MBMS
05/25/07   vg      added support for FEATURE_CGPS and FEATURE_CGPS_UMTS_CP_WCDMA.
12/17/04   vr	   Added support for WCDMA->1x handover feature
02/02/04   vk      Added a new signal for LSM commands under FEATURE_MMGPS
07/25/02   xfg     Added a new signal for RR commands
02/15/02   bu      Added a new signal for NV read/write completion.
01/30/02   xfg     Added a new signal for TC to send a command to RRC
09/14/01   bu      Added signal for watchdog reporting.
11/15/00   ram     Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"



/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */



/*--------------------------------------------------------*/
/*                                                        */
/*                 RRC  SIGNALS                           */
/*                                                        */
/*  All REX SIGNALS for the RRC Task are defined here     */
/*--------------------------------------------------------*/

/* Note that 0x8000 and 0x0001 are reserved for task start*/
/* and task start acknowledgement, respectively           */

/*---------------------------------*/
/*                                 */
/*    Command Queue Signals        */
/*---------------------------------*/

/* Signal for internal RRC commands within the RRC task		*/
#define RRC_INT_CMD_Q_SIG    0x0002

/* Signal for RRC commands sent by L1 task                */
#define RRC_L1_CMD_Q_SIG     0x0004

/* Signal for RRC commands sent by MAC task           		*/
#define RRC_MAC_CMD_Q_SIG    0x0008

/* Signal for RRC commands sent by RLC task               */
#define RRC_RLC_CMD_Q_SIG    0x0010

/* Signal for RRC commands sent by MM task            		*/
#define RRC_MM_CMD_Q_SIG     0x0020

/* Signal to send the watchdog report                           */
#define RRC_RPT_TIMER_SIG    0x0040

/* Signal for RRC commands sent by TC task                      */
#define RRC_TC_CMD_Q_SIG     0x0080

/* Signal for NV read/write completion                          */
#define RRC_NV_SIG           0x0100

/* SIgnal for RRC commands sent by RR task                      */
#define RRC_RR_CMD_Q_SIG     0x0200

#if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
/* Signal for RRC commands sent by LCS Task */
#define RRC_LSM_CMD_Q_SIG     0x0400
#endif

/* Signal for RRC commands sent by CM Task */
#define RRC_CM_CMD_Q_SIG      0x0800

#ifdef FEATURE_DUAL_SIM
#define RRC_WRM_SIG   0x2000

/*Singal to indicate that Preemption is due*/
#define RRC_WRM_PREEPTION_SIG 0x4000
#endif

#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
#define RRC_LTE_RRC_CMD_Q_SIG 0x8000
#endif

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
#define RRC_L1_FF_CMD_Q_SIG   0x10000
#endif

#if defined (FEATURE_TDSCDMA_TO_WCDMA) || defined (FEATURE_WCDMA_TO_TDSCDMA)
#define RRC_TDSRRC_CMD_Q_SIG   0x20000
#endif
#ifndef FEATURE_BOLT_MODEM
#define RRC_QMI_CMD_Q_SIG  0x80000
#endif
/*---------------------------------*/
/*                                 */
/*    Other Signals                */
/*---------------------------------*/



/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */



/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */



/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */




/* =======================================================================
**                          Macro Definitions
** ======================================================================= */




/* =======================================================================
**                        Function Declarations
** ======================================================================= */

#endif /* RRCSIGS_H */

