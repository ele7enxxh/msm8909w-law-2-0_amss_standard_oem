#ifndef RABMTASK_H
#define RABMTASK_H
/*===========================================================================
                          R A B M task Interface

DESCRIPTION

 This module defines contains the defines, structures and function 
 prototypes required to send a command to the RAB Management sub task.
 
 Note:  RABM runs under SM task context as a sub-task.
   
EXTERNALIZED FUNCTIONS

 rabm_put_cmd_buf() :       Put a RABM message on the SM command queue
 rabm_get_cmd_buf() :       Get a RABM command buffer
   
INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2000 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/rabm/vcs/rabmtask.h_v   1.8   23 Apr 2002 16:29:08   kishores  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/rabmtask.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/22/05   kvk     Now defining rabm_timer_status as external.
06/30/05   kvk     Prototype defined for function RABM_CHECK_RAB_REEST_PEND.
03/31/05   kvk     rabm_init_rabid() function defined to clear the RAB state
11/23/04   kvk     MT PDP support added
09/21/04   kvk     CR 49365: TIME_RAB_EST_PENDING changed to 30s to give window for RRC
                    to indicate RL release.
09/02/04   kvk     Added new function call RAB_CHECK_DORMANT to verify whether a paticular
                   RAB is in dormant or not.
06/07/04   kvk     New RABMSM_DEACTIVATE_IMPLICIT added for implicit RAB deactivation
05/25/04   kvk     Added new enum rabm_timer_status_T for maintaining timer status.
                   New function prototypes defined for new timer interface.
06/23/02   ks      Changed RABM_CMD_Q_SIG from 0x2 to 0x80.
02/27/03   mks     Added support for PS data continuity feature - Added new 
                   primitives RABMSM_RAT_CHANGE_IND & RABMSM_RAT_RESUME_IND
                   in SM-RABM interface.
04/22/02   ks      Added support for RAB reestablish/release procedures during
                   PDP context preservation procedures.
01/30/02   ks      Added support for Test Control (TC) entity
08/01/01   ks      Added a function rabm_rab_established(), for SM to check
                   the current state of the RAB of an associated PDP context.
06/04/01   ks      Changed rrc_est_ind to rrc_establish_ind and rrc_rel_ind
                   to rrc_release_ind in rabm_cmd_type. 
02/11/01   KS      Initial Revision.


===========================================================================*/

#include "rabm_gmm.h"
#include "rabm_rr.h"
#include "rabm_cm.h"
#include "sm_rabm.h"
#include "smtask.h"
#include "smtask_v.h"
#include "ms_timer.h"


#include "tc_rabm.h"
#include "tctask.h"
#include "tctask_v.h"




#include "queue.h"

/* -----------------------------------------------------------------------
** Enumerated Type Declarations
** ----------------------------------------------------------------------- */

                                                                                       
/* -----------------------------------------------------------------------
** Command IDs used by other Entities to send Commands to RABM 
** ---------------------------------------------------------------------*/
typedef enum
{
  /*--------------------------------------------------------*/
  /* Command Ids sent by GMM to RABM                        */
  /*--------------------------------------------------------*/
  GMMRABM_REESTABLISH_RSP = 0x02000000, /* GMM commands start here */
  GMMRABM_REESTABLISH_REJ,

  /*--------------------------------------------------------*/
  /* Command Ids sent by CM to RABM                         */
  /*--------------------------------------------------------*/
  RABM_RAB_REESTABLISH_REQ = 0x03000000, /* CM command to RABM */

  /*--------------------------------------------------------*/
  /* Command Ids sent by RRC (AS) to RABM                   */
  /*--------------------------------------------------------*/
  RABMAS_RAB_ESTABLISH_IND = 0x04000000,  /* RRC commands start here */
  RABMAS_RAB_RELEASE_IND,
  RABMAS_STATUS_IND,
  RABMAS_FORCE_CLOSE_SESSION_IND,

  /*--------------------------------------------------------*/
  /* Command Ids sent by SM  entity                         */
  /*--------------------------------------------------------*/
  RABMSM_ACTIVATE_IND = 0x05000000,  /* SM commands start here */
  RABMSM_DEACTIVATE_IND,
  RABMSM_DEACTIVATE_IMPLICIT_IND,
  RABMSM_MODIFY_IND,
  RABMSM_SIG_CON_RELEASE_IND,
  RABMSM_RAT_CHANGE_IND,
  RABMSM_RAT_RESUME_IND,

  /*--------------------------------------------------------*/
  /* Command Ids sent by TC  entity                         */
  /*--------------------------------------------------------*/
  RABMTC_MODE_CHANGE_IND = 0x06000000,  /* TC commands start here */
 
  /*--------------------------------------------------------*/
  /* End of RABM command ids                                */
  /*--------------------------------------------------------*/
  RABM_MAX_CMDS
  
}rabm_cmd_enum_type;
                                                    
/*--------------------------------------------------------
**  RABM command header, Entities needs to set the fields
**   of this header when they send a command to RABM. 
** Note: Connection Id is set by CM entity only.
**--------------------------------------------------------*/
typedef struct 
{
  q_link_type           link;           /* Quaue link */
  byte                  message_set;    /* Identifies the Entity */
  rabm_cmd_enum_type    cmd_id;         /* RABM Command ID */
  byte                  connection_id;  /* Connection ID of PDP context */
} rabm_cmd_hdr_type;

                                  
/* -----------------------------------------------------------------------
** List of commands RABM receives.
** ---------------------------------------------------------------------*/

typedef struct
{
  rabm_cmd_hdr_type      header;

  union 
  {    
    /*--------------------------------------------------------*/
    /* Commands sent by GMM to RABM                           */
    /*--------------------------------------------------------*/

    gmmrabm_reestablish_rsp_T     gmm_reestablish_rsp;

    gmmrabm_reestablish_rej_T     gmm_reestablish_rej;

    /*--------------------------------------------------------*/
    /* Commands sent by CM to RABM                            */
    /*--------------------------------------------------------*/
    rabm_rab_reestablish_req_T    rabm_reestablish_req;

    /*--------------------------------------------------------*/
    /* Commands sent by RRC to RABM                           */
    /*--------------------------------------------------------*/
    rabmas_rab_establish_ind_T    rrc_establish_ind;
    rabmas_rab_release_ind_T      rrc_release_ind;
    rabmas_status_ind_T           rrc_status_ind;
    rabmas_force_close_session_ind_type rabmas_force_close_session_ind;

    /*--------------------------------------------------------*/
    /* Commands sent by SM to RABM                            */
    /* Note: RABM is a subtask of SM task                     */
    /*--------------------------------------------------------*/
    rabmsm_activate_ind_T         sm_activate_ind;
    rabmsm_deactivate_ind_T       sm_deactivate_ind;
    rabmsm_modify_ind_T           sm_modify_ind;
    rabmsm_release_ind_T          sm_release_ind;
    rabmsm_rat_change_ind_T       sm_rat_change_ind;
    rabmsm_rat_resume_ind_T       sm_rat_resume_ind;

    /*--------------------------------------------------------*/
    /* Command sent from REX to RABM on timer expiry            */
    /*--------------------------------------------------------*/
    timer_expiry_T                timer_expiry;



    /*--------------------------------------------------------*/
    /* Commands sent by Test Control (TC) to RABM             */
    /*--------------------------------------------------------*/
    tc_mode_change_T              tc_mode_change_ind;


    
  } cmd;
} rabm_cmd_type;

/* =======================================================================
**            Function Definitions (Used by other Entities)
** ======================================================================= */

/* Function used by other entities to put a command in RABM command Queue */
extern void rabm_put_cmd(rabm_cmd_type *cmd_ptr);

/* Function used by other entities to get RABM command type */
extern rabm_cmd_type *rabm_get_cmd_buf(void);

/* Function used by RRC to query if RAB_ID is valid, before RAB established */
extern boolean rabm_rabid_validate(rab_query_T query_type, byte Rabid);


#endif

