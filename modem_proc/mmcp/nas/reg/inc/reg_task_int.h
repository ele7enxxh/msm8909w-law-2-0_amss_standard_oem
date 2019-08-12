#ifndef REG_TASK_INT_H
#define REG_TASK_INT_H
/*==============================================================================


                   R E G  -  T A S K   H E A D E R   F I L E


                                  DESCRIPTION

  This module defines the types and functions necessary to send a command
  to the REG task.

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_task.h_v   1.1   06 Jun 2002 10:45:00   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/reg/inc/reg_task_int.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
06/20/01   sbk     The command names from CM has changed to include cm_ prefix
08/16/01   ab      Renamed mmi_reg.h references to reg_cm.  Renamed MMI to CM.
10/21/01   sbk     mmr_reg_rej_T, mmr_service_ind_T added to the reg_cmd_type
10/31/01   jca     Added mmr_act_cnf_T to reg_cmd_type.
02/13/02   kwa     Updated to support dual mode requirements.
05/31/02   kwa     Made reg_wait an external function.
09/19/02   kwa     Added cm_act_req to reg_cmd_type.
05/17/04   kwa     Added mmr_camped_ind to reg_cmd_type.
02/22/05   up      Updated for Cingular ENS feature.
                   refresh_fcn_ind command is added to the reg command union.
03/01/05   up      ENS Fix - added sim_refresh_ind in reg command.
09/19/05   up      Added cm_hplmn_search_req in reg_cmd_type to support
                   on demand HPLMN search from user.
11/07/05   ajt     Added msg type mmr_emergency_num_list_ind to reg_cmd_type.                   
04/06/06   sn      Added mmr_cell_service_ind in reg_cmd_type.
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/


#include "mmcp_variation.h"
#include "ms_timer_v.h"
#include "reg_cm_v.h"


#include "reg_sim_v.h"
#ifdef FEATURE_NAS_ECALL
#include "reg_ecall_v.h"
#endif



#include "reg_task_v.h"
/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

/*
** REX signals used by the REG task.
*/
#define REG_CMD_Q_SIG         0x0001
#define REG_DOGRPT_TIMER_SIG  0x0002
#define REG_NVIF_SIG          0x0004
#define REG_RESERVED_SIG      0x1000 /* Used by GSDI library               */

/*==============================================================================

                             FUNCTION DECLARATIONS

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif


/*==============================================================================

FUNCTION NAME

  reg_get_cmd_buf

DESCRIPTION

  Function that allocates memory for a REG command.

RETURN VALUE

  reg_cmd_type* Pointer to the allocated reg_cmd_type.

==============================================================================*/

extern reg_cmd_type* reg_get_cmd_buf
(
  void
);





/*==============================================================================

FUNCTION NAME

  reg_free_cmd_buf

DESCRIPTION

  Function that frees the memory allocated for a REG command.

RETURN VALUE

  None

==============================================================================*/

extern void reg_free_cmd_buf
(
  reg_cmd_type *cmd_buf
);


/*===========================================================================

FUNCTION NAME

  reg_wait

DESCRIPTION

  Main Wait routine for REG task. This routine performs the following functions:
    - Checks REG queue to see if anything is available.  The REG queue
      will be checked to see if anything is available before performing
      a rex_wait().  (There is no need to wait if something is on the queue
      already).  If an item is found on the queue the corresponding bit in
      the returned mask is set.
    - Kicks the watchdog timer while waiting for the specified mask.

RETURN VALUE

  A rex_sigs_type value indicating all signals set (the current state
  of all signals) The calling unit can then check for the reason for
  exiting the wait.

===========================================================================*/

rex_sigs_type reg_wait
(
  rex_sigs_type requested_mask
);


#ifdef __cplusplus
}
#endif

/* Message posted from MCFG callback to REG Queue */
#define REG_MCFG_REFRESH_IND 0x00

#endif
