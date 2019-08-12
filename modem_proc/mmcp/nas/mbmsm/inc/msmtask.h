#ifndef MSMTASK_H
#define MSMTASK_H
/*===========================================================================

               MBMS SERVICE MANAGER TASK HEADER FILE (MSMTASK.H)

DESCRIPTION
  This file contains types, function prototypes and data declarations for
  MBMS Service Manager Task

Copyright (c) 2001,2002,2003 Qualcomm Technologies, Incorporated and its licensors.  All
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mbmsm/inc/msmtask.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/28/06   vr      Created initial file
01/11/07   ajt     Added support for MM_MSM_SUPP_IND
03/18/08   rk      Added Support to merge MBMS SM in CB task

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* Include the interface files for CM and RRC */
#include "rrcmsmif.h"
#include "msm_cm.h"
#include "msm_mm.h"
#include "msm_sm.h"
#include "sm_cm.h"
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*
** REX signals used by the MSM task.
*/
#define MSM_CMD_Q_SIG         0x0008
#define MSM_DOGRPT_TIMER_SIG  0x0002

#define MSM_RRC_CMD_ID_BASE 0x01000000
#define MSM_CM_CMD_ID_BASE  0x02000000
#define MSM_MM_CMD_ID_BASE  0x03000000
#define MSM_SM_CMD_ID_BASE  0x04000000

typedef enum
{
  /* Commands from RRC to MSM */
  MSM_RRC_CMD_BASE = MSM_RRC_CMD_ID_BASE,

  /* MBMS Current Cell Capability Indication */
  RRC_MBMS_CELL_CAPABILITY_IND,

  /* MBMS Service Act Cnf */
  RRC_MBMS_SERVICE_ACTIVATED_CNF,

  /* MBMS Service DeAct Cnf */
  RRC_MBMS_SERVICE_DEACTIVATED_CNF,

  /* MBMS Service availablity information */
  RRC_MBMS_SERVICE_IND,

  /* MBMS RAB Setup Ind */
  RRC_MBMS_RAB_SETUP_IND,

  /* MBMS RAB Release Ind */
  RRC_MBMS_RAB_REL_IND,

  /* MBMS RAB Re-establishment Ind */
  RRC_MBMS_RAB_RE_EST_IND,

  /* MBMS New Session Ind */
  RRC_MBMS_NEW_SESSION_IND,

  /* MBMS Service Suspend Cnf */
  RRC_MBMS_SERVICE_SUSPEND_CNF,

  /* MBMS Service Resume Cnf */
  RRC_MBMS_SERVICE_RESUME_CNF,

  /* MBMS Available services list Cnf */
  RRC_MBMS_AVAILABLE_SERVICES_LIST_CNF,

  /* MBMS lower layers ready ind */
  RRC_MBMS_LL_READY_IND,

  /* MAX RRC CMD */
  MSM_RRC_MAX_CMD,

  /* Commands from CM to MSM */
  MSM_CM_CMD_BASE = MSM_CM_CMD_ID_BASE,

  /* MBMS Service activated Req */
  MBMS_SERVICE_ACTIVATED_REQ,

  /* MBMS Service deactivated Req */
  MBMS_SERVICE_DEACTIVATED_REQ,

  /* MBMS Service deactivated Req */
  MBMS_SERVICE_SUSPEND_REQ,

  /* MBMS Service deactivated Req */
  MBMS_SERVICE_RESUME_REQ,

  /* Command from MBMS Apps */
  MBMS_CM_GENERIC_CMD,

   /* MAX CM CMD */
  MSM_CM_MAX_CMD,

  /* Commands from MM to MSM */
  MSM_MM_CMD_BASE = MSM_MM_CMD_ID_BASE,

  /* MBMS Support Ind at RA Level from MM to MSM */
  MSM_MM_MBMS_SUPP_IND,

  /* MAX CM CMD */
  MSM_MM_MAX_CMD,

  /* Commands from SM to MSM */
  MSM_SM_CMD_BASE = MSM_SM_CMD_ID_BASE,

  MSM_SM_ACTIVATION_CNF,

  MSM_SM_ACTIVATION_IND,

  MSM_SM_ACTIVATION_REJ,

  MSM_SM_DEACTIVATION_CNF,

  MSM_SM_DEACTIVATION_IND,

  MSM_SM_ABORT_CNF,

  /* MAX SM CMD */
  MSM_SM_MAX_CMD,

  /* MAX Cmds */
  MSM_MAX_CMDS

}msm_cmd_e_type;

/* Command header type */
typedef struct
{
  q_link_type   link;        /* Queue link     */
  msm_cmd_e_type cmd_id;     /* The command Id */

}msm_cmd_hdr_type;

/* Union of all MSM commands from Upper and lower Layers */
typedef union
{
  /* Commands from RRC */
  rrc_mbms_service_activated_cnf_s_type   rrc_mbms_service_act_cnf;
  rrc_mbms_service_deactivated_cnf_s_type rrc_mbms_service_deact_cnf;
  rrc_mbms_service_ind_s_type             rrc_mbms_service_ind;
  rrc_mbms_cell_capability_ind_s_type     rrc_mbms_cell_capability_ind;
  rrc_mbms_ll_ready_ind_s_type            rrc_mbms_ll_ready_ind;

  /* Commands from CM */
  msm_cm_service_act_req_s_type           cm_mbms_service_act_req;
  msm_cm_service_deact_req_s_type         cm_mbms_service_deact_req;
  msm_cm_mbms_generic_cmd_s_type          cm_mbms_generic_cmd;

  /* Commands from MM */
  mm_msm_supp_ind_s_type                  mm_msm_supp_ind;

  /* Commands from SM */
  smreg_mbms_activate_cnf_T               sm_mbms_act_conf;
  smreg_mbms_activate_ind_T               sm_mbms_act_ind;
  smreg_mbms_activate_rej_T               sm_mbms_act_rej;
  smreg_mbms_deactivate_cnf_T             sm_mbms_deact_conf;
  smreg_mbms_deactivate_ind_T             sm_mbms_deact_ind;
  smreg_mbms_abort_cnf_T                  sm_mbms_abort_cnf; 

} msm_cmd_union_type;

/* Command Structure*/
typedef struct
{
  msm_cmd_hdr_type                 cmd_hdr;
  msm_cmd_union_type               cmd;

} msm_ext_cmd_type;

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION MSM_GET_CMD_BUF

DESCRIPTION

  This function returns a command buffer of type msm_ext_cmd_type
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling msm_free_cmd_buf().

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type msm_ext_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
msm_ext_cmd_type * msm_get_cmd_buf
(
  void
);

/*===========================================================================

FUNCTION MSM_FREE_CMD_BUF

DESCRIPTION

  This function frees a command buffer removed from any MSM
  command queue. Only commands removed from MSM's command queues
  can be passed into this function.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void msm_free_cmd_buf
(
  msm_ext_cmd_type *cmd_buf
);

/*===========================================================================

FUNCTION MSM_PUT_CMD

DESCRIPTION

  This function puts a command buffer into MSM's into
  command queue. Note that the command buffer must have been
  requested through a call to msm_get_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the MSM
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the MSM task and this could cause a context
  switch.

===========================================================================*/
void msm_put_cmd
(
  msm_ext_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
);

/*===========================================================================

FUNCTION MSM_TASK

DESCRIPTION
  This function is the entry point for MSM Task. It initializes MSM state
  and individual procedures by calling msm_init(). The ACK signal is sent back
  to TMC and a wait is done on Task Start Signal from TMC. Once a Task Start
  Signal is received, msm_dispatcher() is called that takes control now onwards.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void msm_task (dword);

#endif
