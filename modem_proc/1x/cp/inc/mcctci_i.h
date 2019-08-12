#ifndef MCCTC_I_H
#define MCCTC_I_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           C D M A   T R A F F I C   C H A N N E L   S T A T E
                       H E A D E R   F I L E

GENERAL DESCRIPTION
  This module externalizes data items needed by the support routines
  for Mobile Station Control on the Traffic Channel state.

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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mcctci.h_v   1.1.2.0   30 Nov 2001 17:23:58   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mcctci_i.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/25/12   ssh     Cleared the TC exit reason during tunnel mode.
02/16/12   srk     Replaced FEATURE_LTE_TO_1X by FEATURE_MODEM_1X_IRAT_LTO1X.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
12/05/10   jj      CSFB: Added Support for Rel8/Rel9 CSFB.
11/15/10   jtm     Initial SU API split for CP.
11/04/10   ssh     Added processing of SMCM with use_time as TRUE.
07/22/10   ag      Support for processing GEM and RCPM in TC release state.
04/08/07   pg      Featurization and general code cleanup for memory reduction
                   effort.
09/29/05   sb      Removed tc_value.
05/19/05   sb      Removed tc_private_lcm_active.
06/02/03   bkm     Lint cleanup.
06/14/01   va      Added prototype for tc_handle_ccim_event
10/27/00   jq      Merged T_AUTH to FEATURE_AUTH.
07/09/99   ry      Removed the extern definitions for the handoff state
                   indicators mcc_frame_offset_ho_pending and
                   mcc_hard_handoff_in_progress
05/30/96   dna     Created header.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"
#include "mccccim.h"
#include "modem_1x_defs.h"

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
#include "mcccsfbsup.h"
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

/* <EJECT> */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Variables associated with Start and Stop Continous DTMF processing      */

extern boolean tc_stop;
  /* Indicates that when mcc_dtmf_timer expires a DTMF_STOP order must
     be transmitted */
extern boolean tc_ack_wait;
  /* Indicator that we are waiting for an ack to a Start or Stop Continous
     DTMF order or a Burst DTMF message */
extern byte tc_cont_dtmf;
  /* Pending continous DTMF */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
extern word mar_rate;
    /* Markov rate to send to Markov Service Option */

/* Lookup table for Forward Power Control pwr_rep_frames parameter */
extern const word pwr_rep_frames [];

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Buffer for message with requested action time                           */
typedef union
{
  caii_gen_tc_type   gen_tc;
  caii_ftc_ord_type  tc_ord;
  #ifdef FEATURE_IS2000_REL_A
  caii_security_mode_cmd_type  tcsmcm;
  #endif /* FEATURE_IS2000_REL_A */
} tc_action_msg_type;

extern tc_action_msg_type tc_action_msg;

/* Buffer for Service Option Control Order with requested action time      */

extern caii_ftc_ord_type tc_action_so_ctl_order;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Power Control Reporting state indicators                                */

/* Periodic power control reporting state  */
extern boolean mcc_rx_periodic_rpt_on;

/* Threshold power control reporting state */
extern boolean mcc_rx_threshold_rpt_on;

/* Exit reason */
extern mcc_exit_name_type tc_exit;


/*===========================================================================

FUNCTION TC_REL_MSG

DESCRIPTION
  This function processes messages received during the Release substate.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

extern void tc_rel_msg
(
  mccrxtx_msg_type *msg_ptr
    /* Pointer to message received from the base station */
);

/*===========================================================================

FUNCTION TC_TC_MSG

DESCRIPTION
  This function processes messages received during the Mobile Station Control
  on the Traffic Channel "Traffic Channel" substate.

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

extern void tc_tc_msg
(
  mccrxtx_msg_type *msg_ptr
    /* Pointer to message received from the base station */
);

/*===========================================================================

FUNCTION TC_HANDLE_CCIM_EVENTS

DESCRIPTION
  This function processes the primitives coming from the Call Control
  Instance Manager

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  This can result in more primitives being sent back to CCIM or messages
  to the BS.
===========================================================================*/

extern void tc_handle_ccim_event( mccccim_tc_event_type * evt_ptr);
/* Prototype for function that handles primitives from MCCCCIM */

#endif /* MCCTC_I_H */

