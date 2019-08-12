#ifndef MCCSCH_I_H
#define MCCSCH_I_H
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
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mccsch_i.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/09/16   ppr     1x2GTA Feature changes
02/21/12   ppr     Feature Cleanup
02/04/12   ppr     Feature Cleanup
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
#include "target.h"
#include "customer.h"

#include "cai.h"
#include "caii.h"
#include "mccsrch_i.h"
#include "mctrans_i.h"
#include "mccsch.h"
#include "mccsch_i.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
// Type to specify forward or reverse
typedef enum
{
  MCCSCH_FORWARD,
  MCCSCH_REVERSE
} mccsch_ch_dir_type;

typedef struct
{
  mccsch_sch_config_change_cb_type sch_config_change_cb;
    // Function to inform change of F/R-SCH
  mccsch_rsch_assign_cb_type rsch_assign_cb;
    // Function to inform R-SCH assignment received
  mccsch_rsch_extension_cb_type rsch_extension_cb;
    // Function to inform time for extension of R-SCH
  uint8 trigger_time;
    // Time in number of frame before end of burst to execute the extension callback
} mccsch_sch_callback_type;

// Type to store SCH assignment information
typedef struct
{
  uint8 start_time_unit;        // Multiples of 20ms, as defined in the Standard
  cai_fpc_mode_type fpc_mode_sch; // FPC_MODE for use with SCH assignment
  boolean for_sch_fer_rep;
  struct
  {
    // This information here is used when the start time arrives (explicit or not)
    // upon which a transaction is created to stop the F-SCH after for_sch_duration.
    boolean active;
    cai_sch_rate_type cur_rate; // If active, current rate of the channel
    uint8   for_sch_duration;
    uint8   sccl_index;
    qword   stop_time;          // Used to store the stop time for encapsulation
    qword   start_time;         // Used to store the start time for encapsulation
    uint8   start_time_shift;   // Shift start time (when ESCAM is too late)
  } forward[CAI_MAX_NUM_FOR_SCH];
  struct
  {
    // This information here is used when the start time arrives (explicit or not)
    // upon which a transaction is created to stop the R-SCH after rev_sch_duration.
    boolean active;
    cai_sch_rate_type cur_rate; // If active, current rate of the channel
    uint8   rev_sch_duration;
    cai_sch_rate_type new_rate; // Rate to use at start time
    qword   stop_time;          // Used to store the stop time for encapsulation
    qword   start_time;         // Used to store the start time for encapsulation
  } reverse[CAI_MAX_NUM_REV_SCH];
  struct
  {
    byte    rev_sch_dtx_duration;
    boolean use_t_add_abort;     /* Reverse use T_ADD abort indicator */
    boolean ignore_escam;        /* Ignore ESCAM indicator */
    boolean current_use_t_add_abort;
      /* Currently used reverse use T_ADD_ABORT indicator */
  } rev_parms;
} sch_assign_type;

// Contains SCH assignment information
extern sch_assign_type sch_assign;

#define INFINITE_DURATION   0xf

/*===========================================================================

FUNCTION mccsch_initialize

DESCRIPTION


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_initialize( void );

/*===========================================================================

FUNCTION mccsch_stop_f_sch0_trans

DESCRIPTION
  Function to generate a transaction to stop the SCH 0 processing.  This is
  call after the completion of a start transaction and is called from the
  transaction client's (MC) context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_stop_f_sch0_trans( void );

/*===========================================================================

FUNCTION mccsch_stop_f_sch0_trans_now

DESCRIPTION
  Function to generate a transaction to stop the SCH 0 processing immediately.
  This is used after a SHO which drops the last leg on F-SCH (shouldn't have
  happened though).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_stop_f_sch0_trans_now( void );

/*===========================================================================

FUNCTION mccsch_is_r_sch_finite_burst

DESCRIPTION
  Function to check whether specified sch_id reverse supplemental assignment
  is a finite burst.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccsch_is_r_sch_finite_burst ( uint8 sch_id );

/*===========================================================================

FUNCTION mccsch_is_use_t_add_abort_changed

DESCRIPTION
  Function to check whether we need to inform Searcher task of the value
  update of USE_T_ADD_ABORT.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccsch_is_use_t_add_abort_changed ( mctrans_transaction_type trans_type );

/*===========================================================================

FUNCTION mccsch_stop_r_sch0_trans

DESCRIPTION
  Function to generate a transaction to stop the R-SCH 0 processing.  This is
  call after the completion of a start transaction and is called from the
  transaction client's (MC) context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_stop_r_sch0_trans( void );
/*===========================================================================

FUNCTION mccsch_r_sch0_extend_trans

DESCRIPTION
  Function to generate a transaction to notify DS that it's time to issue a
  burst extension if necessary (more data to send).  This is called after the
  completion of a start transaction and is called from the transaction
  client's (MC) context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_begin_r_sch0_extend_trans( void );

/*===========================================================================

FUNCTION mccsch_update_r_sch_parm_trans

DESCRIPTION
  Function to generate a transaction to notify Searcher to update
  USE_T_ADD_ABORT. This is called after the completion of a start transaction
  and is called from the transaction client's (MC) context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_update_r_sch_parm_trans ( void );

/*===========================================================================

FUNCTION mccsch_start_trans

DESCRIPTION


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_begin_trans
(
  mctrans_transaction_type  trans_type,
  mctrans_pending_action_msg_type* msg
);

/*===========================================================================

FUNCTION mccsch_update_sch_assign_with_uhdm

DESCRIPTION
  Pre-processor for "stop f-sch 0" transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_update_sch_assign_with_uhdm
(
  caii_ho_msg_type* uhdm
);

/*===========================================================================

FUNCTION mccsch_compute_start_time

DESCRIPTION
  Computes the start time of a start/stop SCH transaction.  "start_time"
  is as specified in the Standard which is in unit of "start_time_unit".

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_compute_start_time
(
  qword               ref_time,
  uint8               start_time,
  uint8               sch_id,
  mccsch_ch_dir_type  dir
);

/*===========================================================================

FUNCTION mccsch_compute_stop_time

DESCRIPTION
  Computes the stop time of a SCH burst assignment based on "duration" value,
  as defined in the Standard.


DEPENDENCIES
  Start time must have been computed before.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_compute_stop_time
(
  uint8               duration,
  uint8               sch_id,
  mccsch_ch_dir_type  dir
);

/*===========================================================================

FUNCTION mccsch_process_escam

DESCRIPTION
  Process message ESCAM received in various states in traffic channel.  It
  first validates the message and sends reject code if not valid.  Then it
  will go ahead a actually process the content of the message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_process_escam
(
  caii_escam_type* escam
);

/*===========================================================================

FUNCTION mccsch_sch_is_active

DESCRIPTION
   This function checks whether the specified SCH is active.  Note the SCH_ID is
   assumed to be valid.  If invalid SCH_ID is specified, return value is
   unpredictable.

DEPENDENCIES
  None.

RETURN VALUE
  True if SCH is active, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccsch_sch_is_active
(
  mccsch_ch_dir_type  dir,
  uint8               sch_id
);

/*===========================================================================

FUNCTION mccsch_update_t_mulchan

DESCRIPTION
  This function updates SCRM pilot strength reporting offset.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_update_t_mulchan
(
  uint8 t_mulchan
);

/*===========================================================================

FUNCTION mccsch_update_ignore_escam

DESCRIPTION
  This function updates ignore ESCAM indicator.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_update_ignore_escam
(
  boolean ignore_escam
);

/*===========================================================================

FUNCTION mccsch_reg_sch_callback

DESCRIPTION
  This function binds the following 3 callback functions to the SCH manager:
  1. Function to call when SCH configuration changes.
  2. Function to call when ESCAM including R-SCH assignment (not stop) is received
     (not at the start time but the time the message is received).
  3. Function to call when there are "n" frames left in the burst, or at beginning
     of burst if burst length is less than n.  "n" is passed in as a function
     argument.  This is used for burst extension.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_reg_sch_callback
(
  mccsch_sch_callback_type* sch_cb
);

/*===========================================================================

FUNCTION mccsch_proc_r_dtx

DESCRIPTION
  This function is invoked when DTX is detected. It tears down the RSCH and
  sends a SCRM to the BS.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_proc_r_dtx ( void );

/*===========================================================================

FUNCTION mccsch_process_t_add_abort

DESCRIPTION
  This function dispatches transaction to stop R-SCH at the end of the
  current 20 ms frame, set ignore ESCAM indicator and send SCRM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_process_t_add_abort ( mccsrch_rpt_type * );


/*===========================================================================

FUNCTION mccsch_verify_rate

DESCRIPTION
  Function to verify rate versus SCH mux option

DEPENDENCIES
  None.

RETURN VALUE
  TURE if verification is successful.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccsch_verify_rate
(
  uint16            sch_mux,
  cai_sch_rate_type sch_rate
);

/*===========================================================================

FUNCTION mccsch_stop_sch0_trans

DESCRIPTION
  Function to generate two transactions to stop the Fwd and Rev SCH 0
  processing at the action time. It is called from the transaction client's
  (MC) context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_stop_sch0_trans( qword action_time );

/*===========================================================================

FUNCTION mccsch_verify_fwd_assigment

DESCRIPTION
  Function verify the sch_assign fwd entries after a HO is completed.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: if the assignment is still valid
  FALSE:  ..              .. not valid

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccsch_verify_fwd_assignment(void);
#endif // MCCSCH_I_H

