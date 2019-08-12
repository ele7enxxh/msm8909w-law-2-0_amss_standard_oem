#ifndef NVRUIM_TASK_H
#define NVRUIM_TASK_H
/*===========================================================================


              N V    R - U I M    S U B S Y S T E M    T A S K


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2013 - 2016 QUALCOMM Technologies, Incorporated and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/nvruim/src/nvruim_task.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
05/18/16   nr      Redirect requests to NV during QCN Restore operation
01/31/15   ar      Checking for task stop before setting the signal for task
01/19/15   stv     Mechanism to provide lock to lower priority task like diag
08/28/14   av      Change nvruim crit sect to mutex-based from pimutex-based
07/14/14   tl      Remove always ON feature FEATURE_MODEM_RCINIT
01/10/14   df      Remove unneccessary include
11/01/13   at      Support for guaranteed response for each NVRUIM request
10/17/13   at      SIM busy support in NVRUIM
09/23/13   vr      qtf-crm MOB migration
05/13/13   vs      Initial revision
=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "task.h"
#include "uim_variation.h"
#include "rcinit.h"
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

#ifdef FEATURE_UIM_USES_TASK_HANDLE
/* Need this as we are referring to UIM task handle */
TASK_EXTERN(uim);
#define UIM_NVRUIM_TCB rcinit_lookup_rextask("nvruim")
#else
#define UIM_NVRUIM_TCB &nvruim_task_tcb
#endif /* FEATURE_UIM_USES_TASK_HANDLE */

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

/*--------------------------------------------------------------------------
                             Signal Masks
--------------------------------------------------------------------------*/
#define NVRUIM_TASK_CMD_Q_SIG                 0x00000001
#define NVRUIM_RPT_TIMER_SIG                  0x00000002
#define NVRUIM_MMGSDI_ACCESS_SIG              0x00000004
#define NVRUIM_SIM_BUSY_SIG                   0x00000008

#define NVRUIM_CLASH_WITH_TASK_OFFLINE_SIG    0x00002000 /* RESERVED SIGNAL */
#define NVRUIM_CLASH_WITH_TASK_STOP_SIG       0x00004000 /* RESERVED SIGNAL */
#define NVRUIM_CLASH_WITH_TASK_START_SIG      0x00008000 /* RESERVED SIGNAL */

#define NVRUIM_TASK_STOP_SIG                  0x00010000
#define NVRUIM_CLIENT_LOCK_SYNC_SIG           0x00020000

#define NVRUIM_CLASH_WITH_ONCRPC_REPLY_SIG    0x00800000 /* RESERVED SIGNAL */

/*******ADDITIONAL RESERVED SIGNALS**************/
#define NVRUIM_CLASH_WITH_CAMERA_SIG          0x04000000 /* RESERVED SIGNAL */
#define NVRUIM_CLASH_WITH_GRP_ACK_SIG         0x08000000 /* RESERVED SIGNAL */
#define NVRUIM_CLASH_WITH_SEC_OP_COMPLETE_SIG 0x10000000 /* RESERVED SIGNAL */
#define NVRUIM_CLASH_WITH_VS_OP_COMPLETE_SIG  0x20000000 /* RESERVED SIGNAL */
#define NVRUIM_CLASH_WITH_FS_OP_COMPLETE_SIG  0x40000000 /* RESERVED SIGNAL */
#define NVRUIM_CLASH_WITH_REX_SLEEP_SIG       0x80000000 /* RESERVED SIGNAL */
/************************************************/

/* Max requests that can be accomodated by NVRUIM queue */
#define NVRUIM_CMD_QUEUE_MAX_SIZE  100

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/
/* ----------------------------------------------------------------------------
   STRUCTURE:      NVRUIM_GENERIC_DATA_TYPE

   DESCRIPTION:
     The structure contains general global data for NVRUIM
-------------------------------------------------------------------------------*/
typedef struct {
  boolean                       task_stopped;
  boolean                       rtre_config_init;
  nv_rtre_control_type          rtre_control_value;
  nv_rtre_polling_control_type  rtre_polling_control_value;
  qurt_mutex_t                  crit_sect;
  rex_crit_sect_type            timer_crit_sect;
  nv_ruim_qcn_restore_status    nvruim_qcn_restore_status;
} nvruim_generic_data_type;

/* Global structure to general info for NVRUIM task */
extern nvruim_generic_data_type *nvruim_generic_data_ptr;

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
   FUNCTION:      NVRUIM_CMD

   DESCRIPTION:
     This function allows putting of command into the NVRUIM command queue.

   DEPENDENCIES:
     None

   LIMITATIONS:
     Parameter check for specific commands are not performed

   RETURN VALUE:
     nv_stat_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
nv_stat_enum_type nvruim_cmd
(
  nv_cmd_type *nv_cmd_ptr
);

/*===========================================================================
FUNCTION NVRUIM_WAIT_FOR_MMGSDI_ACCESS

DESCRIPTION
  Wait routine called when MMGSDI access is in progress

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void nvruim_wait_for_mmgsdi_access (
  void
);

/*===========================================================================
FUNCTION NVRUIM_WAIT

DESCRIPTION
  Main wait routine for the NVRUIM task.

DEPENDENCIES
  None

RETURN VALUE
  A rex_sigs_type value indicating all signals set (the current state
  of all signals). The calling unit can then check for the reason for
  exiting the wait.

SIDE EFFECTS
  None
===========================================================================*/
rex_sigs_type nvruim_wait
(
  rex_sigs_type requested_mask
);


#endif /* NVRUIM_TASK_H */
