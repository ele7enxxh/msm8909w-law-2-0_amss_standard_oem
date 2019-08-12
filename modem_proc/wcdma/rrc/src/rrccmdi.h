#ifndef RRCCMDI_H
#define RRCCMDI_H
/*===========================================================================
             R R C   C O M M A N D S   I N T E R N A L   H E A D E R

DESCRIPTION

  This module contains the internal header information for the RRC
  Commands module. This file is internal to RRC and should not be included
  by any module outside RRC.


Copyright (c) 2000, 2002-2004, 2007, 2008 Qualcomm Technologies, Inc. All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrccmdi.h#1 $   

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/07/15   vi      Changes for force DRX and skip idlemode measurements based on QMI request 
05/07/14   vi      Made changes to put the rrc_l1_ff_cmd_q usage under critical section to avoid race condition in FF cmds. 
09/10/12   gv      Added support for new interface between WRRC and TDS RRC
12/12/11   gv      Added code to support free floating of tasks
12/05/11   pm      Added code to support cell id request by CM in DCH state"
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
05/08/09   ss      Updated Copyright Information
03/06/08   da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
12/21/07   vr      Added MBMS Service Manager(MSM) command queue
12/17/04   vr      Added CM queue for WCDMA -> 1x handover
07/30/02   xfg     Added extern rrc_rr_cmd_q for supporting inter-rat handover.
02/04/02   xfg     Added extern rrc_tc_cmd_q for loopback test
12/08/00   ram     Moved CMD_BASE_MASK definiton from rrccmd.c
12/06/00   ram     Removed rrc_cmd_e_type definition since it's no longer
                   used. Moved rrc_free_cmd_buf back to rrccmd.h since the
                   function now uses rrc_cmd_type as a parameter.
11/30/00   rj      removed rrccmd.h from include files
11/29/00   ram     Added rrc_cmd_e_type to be used within RRC. Added externalized
                   function rrc_free_cmd_buf that is used by other RRC modules
                   to free commands.
11/17/00   ram     Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "queue.h"


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

#define CMD_BASE_MASK 0xFF000000  /* Mask used to determine base numbers of
                                  RRC command ids */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/*--------------------------------------------------------*/
/*                                                        */
/*      EXTERNALIZED RRC COMMAND QUEUES                   */
/*                                                        */
/*  All Command Queues for the RRC Task are defined here  */
/*--------------------------------------------------------*/

/* Queue for internal RRC commands within the RRC task    */
extern q_type rrc_int_cmd_q;

/* Queue for RRC commands sent by L1 task                 */
extern q_type rrc_l1_cmd_q;

/* Queue for RRC commands sent by MAC task                */
extern q_type rrc_mac_cmd_q;

/* Queue for RRC commands sent by RLC task                */
extern q_type rrc_rlc_cmd_q;

/* Queue for RRC commands sent by MM task                 */
extern q_type rrc_mm_cmd_q;

/* Queue for RRC commands sent by TC task                 */
extern q_type rrc_tc_cmd_q;

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
/*Queue type for WL1 sendign free flaoting cmds to RRC*/
extern q_type rrc_l1_ff_cmd_q;
#endif
/* Queue for RRC commands sent by GSM RR task             */
extern q_type rrc_rr_cmd_q;

/* Queue for RRC commands sent by CM task             */
extern q_type rrc_cm_cmd_q;

#if defined (FEATURE_WCDMA_TO_LTE) || defined (FEATURE_LTE_TO_WCDMA)
/*Queue type for WCDMA RRC cmds from LTE RRC*/
extern q_type rrc_lte_cmd_q;
extern q_type rrc_lte_free_cmd_q;
#endif

#if defined (FEATURE_TDSCDMA_TO_WCDMA) || defined (FEATURE_WCDMA_TO_TDSCDMA)
/* Queue for TDS RRC commands sent by TDSCDMA RRC task          */
extern q_type rrc_tdsrrc_cmd_q;
#endif

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
  extern rex_crit_sect_type rrc_l1_ff_cmd_crit_sect;
#endif

/*Queue type for WCDMA RRC cmds from QMI*/
  extern q_type rrc_qmi_cmd_q;
  extern q_type rrc_qmi_free_cmd_q;



/* =======================================================================
**                          Macro Definitions
** ======================================================================= */




/* =======================================================================
**                        Function Declarations
** ======================================================================= */
/*===========================================================================

FUNCTION RRC_INIT_COMMAND_QUEUES

DESCRIPTION

  This function initializes the RRC command queues. This function needs
  to be called once at the time of the RRC task start-up.
 
DEPENDENCIES

  This function assumes that all REX signals for RRC task have been cleared 
  before this function is called.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.
  
===========================================================================*/
void rrc_init_command_queues( void );


#endif /* RRCCMDI_H */

