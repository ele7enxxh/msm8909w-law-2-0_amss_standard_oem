#ifndef MCCDS_V_H
#define MCCDS_V_H

/*===========================================================================

            MAIN CONTROL DATA SERVICE SUPPORT FUNCTIONS

GENERAL DESCRIPTION
  This module contains utilities to handle requests from Data Service Task.

  Copyright (c) 2001 - 2013 Qualcomm Technologies, Inc. 
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
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccds.h_v   1.1.2.0   30 Nov 2001 17:11:44   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mccds_v.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/21/12   ppr     Feature Cleanup
02/04/12   ppr     Feature Cleanup
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
11/15/10   jtm     Initial SU API split for CP.
06/25/09   jj      Made changes to include cai_v.h file.
05/14/09   jj      Split the file as part of CMI phase-II
12/13/07   pg      Fixed featurization problem on FEATURE_IS2000_CHS and
                   FEATURE_IS2000_R_SCH.
05/09/06   TMR     Removed remoting of mccds_reg_chs_notifier()... now being
                   shimmed as callback is called in interrupt context.
02/01/06   pg      RPC related changes.
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
06/26/01   sb      Merged the following from common archive:
           06/03/01   ht      Added $Header line for revision control.
05/11/01   jq      Added mccds_reg_chs_notifier.
05/02/01   fc      Added support of T_ADD_ABORT.
04/10/01   jq      Moved all SCRM related data definitions and function
                   prototypes from mccsch.h to here.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"

#include "err.h"
#include "cai_v.h"
#include "caii_v.h"
#include "mccsrch_i.h"
#include "mccds.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

// Enum type for channel state
typedef enum
{
  MCCDS_CONTROL_HOLD_STATE,
  MCCDS_ACTIVE_STATE
} mccds_ch_state_type;

/* function pointer to hold the DS call back function to set the Control Hold State */
extern void (*mccds_notify_chs) (boolean chs_on);

/*===========================================================================

FUNCTION mccds_init_scrm

DESCRIPTION
  This function initializes SCRM related variables and functions

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccds_init_scrm(void);

/*===========================================================================

FUNCTION mccds_clear_scrm_req_blob

DESCRIPTION
  This function clears SCRM BLOB.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccds_clear_scrm_req_blob(void);

/*===========================================================================

FUNCTION mccds_update_t_mulchan

DESCRIPTION
  This function updates SCRM pilot strength reporting offset.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccds_update_t_mulchan
(
  uint8 t_mulchan
);

/*===========================================================================

FUNCTION mccds_get_t_mulchan

DESCRIPTION
  This function returns SCRM pilot strength reporting offset.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 mccds_get_t_mulchan(void);

/*===========================================================================

FUNCTION mccds_get_scrm_seq_num

DESCRIPTION
  This function returns the last used SCRM sequence number.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 mccds_get_scrm_seq_num(void);

/*===========================================================================

FUNCTION mccds_build_sup_chan_req_msg

DESCRIPTION
  This function builds a Supplemental Channel Request message from DS task's
  SCRM blob info and/or SRCH's task SCRM pilot strength measurement report.

DEPENDENCIES
  mccds_scrm.sz_req_blob and mccds_scrm.req_blob must already setup
  properly.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccds_build_sup_chan_req_msg
(
  caii_tx_msg_type *msg_ptr,
    /* Pointer to Supplemental Channel Request Message */
  mccsrch_rpt_type *srch_rpt_ptr,
    /* SRCH SCRM Pilot Strength Measurements or T_ADD_ABORT report */
  boolean use_scrm_seq_num
    /* Indicator whether to use SCRM sequence number */
);

/*===========================================================================

FUNCTION mccds_process_resource_request

DESCRIPTION
  This function handles resource request from Data Service Task.

  SCRM request: It gets SCRM blob from DS task. If the sise of SCRM blob is not
  zero, it commands SRCH task to send SCRM pilot strength measurement report.
  Otherwise, it sends SCRM right away.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccds_process_resource_request(void);

#endif // MCCDS_V_H
