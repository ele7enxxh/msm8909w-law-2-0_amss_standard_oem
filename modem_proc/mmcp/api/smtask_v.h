#ifndef SMTASK_V_H
#define SMTASK_V_H
/*===========================================================================
                          SM Interface

DESCRIPTION

 This module defines contains the defines, structures and function 
 prototypes required to send a command to the Session Management task.
   
FUNCTIONS

  Functions used by all Entities to put SM command :
  --------------------------------------------------
  sm_get_cmd_buf()       Get a SM command buffer.
  sm_put_cmd_buf()       Put a SM message on the SM command queue.

  Function used by Call Manager to get a connection Id.
  -----------------------------------------------------
  sm_get_connection_id Gets a connection Id.

  Function used by TMC controller task to start the SM task.
  ----------------------------------------------------------
  sm_task()              SM task entry
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
 

Copyright (c) 2000 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/sm/vcs/smtask.h_v   1.7   18 Jul 2002 15:45:48   kishores  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/smtask_v.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/26/12   am      Back off timer code
07/03/06  ss        Added GPRS call control fixes
04/18/06  ss        Added cause value to GMMSM_GPRS_RELEASE_IND
08/22/05  kvk      Added gmmsm_establish_ind_T in sm_cmd_type to handle SR rejects for
                   performing local PDP deactivation.
06/07/05   kvk     Added new primitive SMREG_PDP_ABORT_REQ for LPM support. 
03/10/05   ss      Added support for handling incoming PDP Context Status IE while Rau Accept or Service
                   accept based on Rel-4 Spec CR N1-010325
10/18/04   kvk     MT PDP support added
09/02/04   kvk     Added enums RABMSM_REESTABLISH_IND, GMMSM_ESTABLISH_IND in sm_cmd_tpe_t.
                   Added smreg_pdp_activate_sec_cnf_T primitive for SM->CM SEC pdp
                   activate cnf.
08/27/04   ab      CR39294, added received NPDU list indicator.
06/03/04   kvk     Added timer_expiry_cmd_T in sm_cmd_type to handle timer expiry for 
                   SM timers.
03/18/03   mks     Added gmm_rat_change_req to command union in sm_cmd_type.
02/27/03   mks     Added command identifiers for GMMSM_RAT_CHANGE_REQ and
                   GMMSM_RAT_RESUME_REQ primitives from GMM.
02/24/03   ks      Added new function sm_unreserve_connection_id()
10/21/02   he      Added SNSM_SEQUENCE_RESP to sm_cmd_enum_type
09/30/02   ks      Renamed GMMSM_ERROR_IND with GMMSM_CON_RELEASE_IND and
                   GMMSM_RELEASE_IND with GMMSM_GPRS_RELEASE_IND primitives.
07/18/02   ks      Renamed FEATURE_GPRS with FEATURE_GSM_GPRS
07/01/02   ks      Added GMMSM_TLLI_ASSIGNMENT_IND primitive for GPRS supprt
11/20/01   ks      Added SMREG_PDP_ACTIVATE_REJ_RSP in sm_cmd_enum_type.
08/01/01   ks      Added SM_DOGRPT_TIMER_SIG for watch dog support.
06/29/01   ks      Added GMM_ERROR_IND in the sm_cmd_enum_type. Also added
                   gmm_error_ind_T structure in the sm_cmd_type structure. 
04/30/01   ks      Added pdp_activate_rej_resp data type in the sm_cmd_type.
03/03/01   ks      Added SM command Enums and functions sm_task and
                   sm_get_connection_id().
12/11/00   ts      Initial Revision.

===========================================================================*/

#include "queue.h"
#include "sm_common.h"
#include "sm_gmm.h"
#include "sm_cm.h"
#include "sm_sndcp.h"
#include "sm_rabm.h"
#include "sm_gmm.h"
#include "task.h"
#include "ms_timer.h"


#include "msm_sm.h"


/* -----------------------------------------------------------------------
** Constant/Define Declarations
** ----------------------------------------------------------------------- */
#define SM_CMD_Q_SIG         0x1
#define SM_DOGRPT_TIMER_SIG  0x2
// LTE-ESM signal
#define ESM_CMD_Q_SIG         ((rex_sigs_type)0x00000004)
/* Message posted from MCFG callback to SM Queue */


/*--------------------------------------------------------*/
/*                 SM COMMAND IDs                         */
/*                                                        */
/*  Command Ids for commands sent from various tasks to   */
/*  SM are defined here.                                  */
/*--------------------------------------------------------*/                          
typedef enum
{
  /*--------------------------------------------------------*/
  /* Command Ids sent by CM entity                          */
  /*--------------------------------------------------------*/
  SMREG_PDP_ACTIVATE_REQ = 0x02000000, /* CM commands start here */
  SMREG_PDP_DEACTIVATE_REQ,
  SMREG_PDP_ACTIVATE_REJ_RSP,
  SMREG_PDP_MODIFY_REQ,
  SMREG_PDP_ACTIVATE_SEC_REQ,
  SMREG_PDP_ABORT_REQ,

  SMREG_PS_SIGNALING_REL_REQ,


  SMREG_PDP_GRACEFUL_ABORT_REQ,


  SMREG_PDP_NW_MODIFY_REJ,
  SMREG_PDP_NW_MODIFY_CNF,

  SMREG_BLOCKED_APN_LIST,
  

  /*--------------------------------------------------------*/
  /* Command Ids sent by RAB entity                         */
  /*--------------------------------------------------------*/
  RABMSM_ACTIVATE_RSP = 0x03000000,  /* RABM commands start here */
  RABMSM_DEACTIVATE_RSP,
  RABMSM_DEACTIVATE_REQ,
  RABMSM_MODIFY_RSP,
  RABMSM_STATUS_REQ,
  RABMSM_REESTABLISH_IND,/*To indicate RAB restablishment*/
 
  /*--------------------------------------------------------*/
  /* Command Ids sent by GMM entity                         */
  /*--------------------------------------------------------*/
  GMMSM_ESTABLISH_CNF = 0x04000000,  /* GMM commands start here */
  GMMSM_ESTABLISH_IND,
  GMMSM_ESTABLISH_REJ,
  GMMSM_GPRS_RELEASE_IND,
  GMMSM_UNITDATA_IND,
  GMMSM_CON_RELEASE_IND,

  GMMSM_TLLI_ASSIGNMENT_IND,
  GMMSM_NPDU_LIST_IND,

  GMMSM_RAT_CHANGE_REQ,
  GMMSM_RAT_RESUME_REQ,
  GMMSM_PDP_MBMS_STATUS_IND,
  GMMSM_PS_FOP_IND,

  GMMSM_CON_RELEASE_REJ,
 

  GMMSM_STATUS_IND,


  GMMSM_FAST_DORMANCY_CNF,


  GMMSM_ISR_STATUS_IND,

  GMMSM_UNBLOCK_ALL_APN,

  GMMSM_MULTIMODE_SUBS_CHGD_REQ,

  /*--------------------------------------------------------*/
  /* Command Ids sent by SNDCP entity                       */
  /*--------------------------------------------------------*/
  SNSM_ACTIVATE_RESP = 0x05000000,  /* SNDCP commands start here */
  SNSM_DEACTIVATE_RESP,
  SNSM_MODIFY_RESP,
  SNSM_STATUS_REQ,

  SNSM_SEQUENCE_RESP,



  /*--------------------------------------------------------*/
  /* Command Ids sent by MBMS Service Manager entity        */
  /*--------------------------------------------------------*/
  MSM_SM_ACTIVATION_REQ = 0x06000000,  /* MSM commands start here */
  MSM_SM_ACTIVATE_REJ_RSP,
  MSM_SM_ABORT_REQ,


  /*--------------------------------------------------------*/
  /* End of SM command ids                                  */
  /*--------------------------------------------------------*/
  SM_MAX_CMDS
  
} sm_cmd_enum_type;                                             
                     
                                 
/* -----------------------------------------------------------------------
** SM_CMD_HDR_TYPE data structure. All the external entities sending the
** SM commands need to set these fields properly.  
**
** Note: The connection_id is used only during CM-SM communcation, other
** entities will ignore this field.
** ----------------------------------------------------------------------- */
typedef struct 
{
  q_link_type       link;           /* Quaue link */
  byte              message_set;    /* Identifies the Entity */
  sm_cmd_enum_type  cmd_id;         /* SM Command ID */
  byte              connection_id;  /* Connection ID of the context */
}sm_cmd_hdr_type;
                                  
                                  
/* -----------------------------------------------------------------------
** Enumeration of all the Commands/primitivies that SM task receives.
** Note: The primitive data types are defined in SM_<entity>.h files.
**       where <entity> is one of the following: CM,SNDCP,GMM,RABM.
** ----------------------------------------------------------------------- */

typedef struct
{   
  sm_cmd_hdr_type      header;

  union 
  {  
    /*--------------------------------------------------------*/
    /* Commands sent from GMM to SM                           */
    /*--------------------------------------------------------*/
    gmmsm_establish_rej_T         gmm_establish_rej;
    gmmsm_establish_ind_T        gmm_establish_ind;
    gmmsm_unitdata_ind_T          gmm_unitdata_ind;
    gmmsm_con_release_ind_T       gmm_con_release_ind;
    gmmsm_gprs_release_ind_T     gmm_gprs_rel_ind;

    gmmsm_npdu_list_ind_T         gmm_npdu_list_ind;
    gmmsm_tlli_assignment_ind_T   snsm_tlli_assignment_ind ;

    gmmsm_rat_change_req_T        gmm_rat_change_req;

    gmmsm_rat_resume_req_T        gmm_rat_resume_req;
    gmmsm_con_release_rej_T        gmmsm_con_release_rej;
    gmmsm_establish_cnf_T         gmm_establish_cnf;
    gmmsm_ps_fop_ind_T         gmm_ps_fop_ind;


    gmmsm_pdp_mbms_context_status_T gmm_pdp_mbms_context_status;
    
    gmmsm_status_ind_T  gmm_status_ind;

    gmmsm_fast_dormancy_cnf_T gmm_fast_dormancy_cnf;


    gmmsm_isr_status_ind_t       gmm_isr_status_ind;

    gmmsm_unblock_all_apn_T   gmmsm_unblock_all_apn;

    gmmsm_multimode_subs_chgd_req_T   gmmsm_multimode_subs_chgd_req;

    /*--------------------------------------------------------*/
    /* Commands sent from CM to SM                            */
    /*--------------------------------------------------------*/
    smreg_pdp_activate_req_T      pdp_activate_req;
    smreg_pdp_modify_req_T        pdp_modify_req;
    smreg_pdp_deactivate_req_T    pdp_deactivate_req;
    smreg_pdp_activate_rej_resp_T pdp_activate_rej_resp;
    smreg_pdp_activate_sec_req_T  pdp_sec_activate_req;
    smreg_pdp_activate_sec_cnf_T  pdp_act_sec_cnf;

    smreg_pdp_abort_req_T  pdp_abort_req;

    smreg_ps_signalling_rel_req_T  ps_signalling_rel_req;

    smreg_pdp_graceful_abort_req_T  pdp_graceful_abort_req;

    smreg_pdp_nw_modify_rej_T    pdp_nw_modify_rej;

    smreg_pdp_nw_modify_cnf_T    pdp_nw_modify_cnf;

    smreg_blocked_apn_list_ind_T   blocked_apn_list_ind;

    /*--------------------------------------------------------*/
    /* Commands sent from SNDCP to SM                         */
    /*--------------------------------------------------------*/
    snsm_activate_rsp_T           sndcp_activate_resp;
    snsm_deactivate_rsp_T         sndcp_deactivate_resp;
    snsm_modify_rsp_T             sndcp_modify_resp;
    snsm_sequence_rsp_T           sndcp_sequence_resp;
    snsm_status_req_T             sndcp_status_req;

    /*--------------------------------------------------------*/
    /* Commands sent from RABM to SM                          */
    /* Note: RABM is part of SM task                          */
    /*--------------------------------------------------------*/
    rabmsm_activate_rsp_T         rabm_activate_rsp;
    rabmsm_modify_rsp_T           rabm_modify_rsp;
    rabmsm_deactivate_rsp_T       rabm_deactivate_rsp;
    rabmsm_status_req_T           rabm_status_req;  
    rabmsm_reest_ind_T            rabm_reest_ind;

    /*--------------------------------------------------------*/
    /* Commands sent from MSM to SM                           */
    /*--------------------------------------------------------*/

    smreg_mbms_activate_req_T       mbms_act_req;
    smreg_mbms_activate_rej_resp_T  mbms_act_rej_resp;
    smreg_mbms_abort_req_T          mbms_abort_req;


    /*--------------------------------------------------------*/
    /* Command sent from REX to SM on timer expiry            */
    /*--------------------------------------------------------*/
    timer_expiry_T                timer_expiry;
  } cmd;
    
} sm_cmd_type;

/* =======================================================================
**                         Function Definitions
** ======================================================================= */

/* Function used by other entities to put a command in SM command Queue */
extern void sm_put_cmd(sm_cmd_type *cmd_ptr);

/* Function used by other entities to get SM command type */
extern sm_cmd_type *sm_get_cmd_buf(void);

/* Function called by Call Manager to get the Connectin Id of a PDP context */
extern boolean sm_get_connection_id_int(byte connection_id);

/* Function called by Call Manager to unreserve connectin Id of a PDP context */
extern boolean sm_unreserve_connection_id_int(byte connection_id);


/* -----------------------------------------------------------------------
FUNCTION SM_RESERVE_NSAPI

DESCRIPTION
  This function reserves a NSAPI for a given connection_id. The validity of
  connection_id is not checked.
-------------------------------------------------------------------------*/
extern byte sm_reserve_nsapi ( byte connection_id );

/* -----------------------------------------------------------------------
FUNCTION SM_RELEASE_NSAPI

DESCRIPTION
  This function releases a reserved NSAPI for a given connection_id. The
  validity of connection_id is not checked.
-------------------------------------------------------------------------*/
extern int sm_release_nsapi ( byte connection_id );

/* -----------------------------------------------------------------------
FUNCTION SM_RELEASE_NSAPI_SM

DESCRIPTION
  This function releases a reserved NSAPI for a given connection_id. The
  validity of connection_id is not checked.
  This function should only be called from SM ONLY
-------------------------------------------------------------------------*/
extern int sm_release_nsapi_sm ( byte connection_id );







#endif
