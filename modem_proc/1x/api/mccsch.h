#ifndef MCCSCH_H
#define MCCSCH_H
/*===========================================================================

    S U P P L E M E N T A L   C H A N N E L   ( S C H )   S E R V I C E S

GENERAL DESCRIPTION
  This module contains utilities related to operation of SCH.

  Copyright (c) 2000 - 2013 Qualcomm Technologies, Inc. 
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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccsch.h_v   1.6   25 Feb 2002 11:06:46   randyp  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/mccsch.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
02/24/11   ssh     Fixed compiler warnings.
11/15/10   jtm     Initial SU API split for CP.
10/12/09   jj      Meta-Comments cleanup for API's exported to DS.
03/20/09   jtm     Removed mctrans_v.h include. The file no longer exists.
04/08/07   pg      Featurization and general code cleanup for memory reduction
                   effort.
06/23/06   pg      Added support for RPC.
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
07/13/04   bkm     Changed arg type for mccsch_rsch_assign_cb to match usage.
03/04/04   az      Added support for new transaction engine
02/10/04   az      Changed name from mccsch_r_sch0_extend_trans() to
                   mccsch_begin_r_sch0_extend_trans()
02/22/02   jrp     Merged from common archive:
                   02/07/02   lcc     Added duration argument to the callback mccsch_sch_config_change_cb.
12/07/01   ph      Merged in from MSM_CP.05.00.108
                   11/28/01   fc      Added function prototypes mccsch_is_r_sch_finite_burst,
                                      mccsch_is_use_t_add_abort_changed and
                                      mccsch_update_r_sch_parm_trans.
10/23/01   jrp     Merged the following from common archive (MSM_CP.05.00.99)
                   10/05/01   lcc     Added support for R-SCH burst extension.
05/22/01   lcc     Modified mccsch_compute_start_time to take a reference time
                   as function argument.
05/03/01   jq      Added mccsch_verify_fwd_assignment ()
05/02/01   fc      Added function prototypes mccsch_process_t_add_abort,
                   mccsch_update_use_t_add_abort and
                   mccsch_update_ignore_escam.
04/10/01   jq      Moved all SCRM related data definitions and function
                   prototypes to mccds.h.
01/16/01   jq      Moved/renamed mccsch_retry_delay_type, mccsch_retry_delay to
                   mccdma.h.
01/10/01   lcc     Added data structures for use by mccsch_sch_config_inform
                   function.
09/18/00   jq      Moved mccsch_verify_rate() from mccsch.c to here.
08/28/00   fc      Featurized mccsch_reg_retry_delay() and
                   mccsch_update_retry_delay() under FEATURE_IS2000.
08/23/00   jq      Added mccsch_proc_r_dtx()
08/23/00   lcc     Added function prototype for mccsch_stop_f_sch0_trans_now.
07/25/00   fc      Added the prototype for mccsch_reg_ds_scrm_init().
07/10/00   fc      Changed scrm_blob_generator prototype to support STDSO.
06/19/00   fc      Added support for SCRM.
06/12/00   lcc     Added prototype for mccsch_sch_is_active.
02/23/00   lcc     Initial release.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "memory.h"
#include "comdef.h"

#include "cai.h"
#include "caii.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

// Type for reporting new SCH operation
typedef struct
{
  boolean           changed;  // Indicates if this channel's operation has changed
  boolean           active;   // Indicates if channel on or off
  cai_sch_rate_type sch_rate; // If active, rate of the channel
} mccsch_sch_state_type;

// Type for reporting new SCH operation
typedef struct
{
  mccsch_sch_state_type f_sch_0;
  mccsch_sch_state_type r_sch_0;
} mccsch_sch_info_type;

// Type for SCH callbacks.  Used by DS.
typedef void (* mccsch_sch_config_change_cb_type) (mccsch_sch_info_type * sch_info);


typedef void (* mccsch_rsch_assign_cb_type) (uint8 duration);

typedef void (* mccsch_rsch_extension_cb_type)(void);

/*===========================================================================

FUNCTION mccsch_reg_sch_config_change_cb

DESCRIPTION
  This function binds the following callback function to the SCH manager:
  Function to call when SCH configuration changes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_reg_sch_config_change_cb
(
  mccsch_sch_config_change_cb_type sch_config_change_cb_func
);

/*===========================================================================

FUNCTION mccsch_reg_rsch_assign_cb

DESCRIPTION
  This function binds the following callback function to the SCH manager:
  Function to call when ESCAM including R-SCH assignment (not stop) is received
  (not at the start time but the time the message is received).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_reg_rsch_assign_cb
(
  mccsch_rsch_assign_cb_type rsch_assign_cb_func
);

/*===========================================================================

FUNCTION mccsch_reg_rsch_extension_cb

DESCRIPTION
  This function binds the following callback function to the SCH manager:
  Function to call when there are "n" frames left in the burst, or at beginning
  of burst if burst length is less than n.  "n" is passed in as a function
  argument.  This is used for burst extension.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_reg_rsch_extension_cb
(
  mccsch_rsch_extension_cb_type rsch_extension_cb_func,
  uint8 trigger_time
);

#endif // MCCSCH_H

