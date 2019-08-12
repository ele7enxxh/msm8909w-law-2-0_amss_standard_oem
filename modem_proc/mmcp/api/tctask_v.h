#ifndef TCTASK_V_H
#define TCTASK_V_H
/*===========================================================================
                          TC Interface

DESCRIPTION

 This module defines contains the defines, structures and function 
 prototypes required to send a command to the Test Control (TC) task.
   
FUNCTIONS

  Functions used by all Entities to put TC command :
  --------------------------------------------------
  tc_get_cmd_buf()       Get a TC command buffer.
  tc_put_cmd_buf()       Put a TC message on the TC command queue.

  Function used by TMC controller task to start the TC task.
  ----------------------------------------------------------
  tc_task()              TC task entry
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
 

Copyright (c) 2002 Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/ctest/vcs/tctask.h_v   1.3   06 Feb 2002 18:34:02   madiraju  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/tctask_v.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/31/04   mks     Added support for handling multiple radio bearers during
                   loopback call setup
01/30/02   mks     Added command Ids for for commands sent CM and RRC, 
                   removed ESTABLISH req/confirm/reject commands for 
                   GMM & MM.
09/17/01   KS      Initial Revision.

===========================================================================*/
#include "queue.h"
#include "task.h"
#include "tc_gmm.h"
#include "tc_mm.h"
#include "tc_rabm.h"
#include "tc_rrc.h"
#include "tc_cm.h"


typedef struct lb_proc_dl_rlc_data_ind 
{
  uint32 wm_id;
} lb_proc_dl_rlc_data_ind_T;

/* -----------------------------------------------------------------------
** Constant/Define Declarations
** ----------------------------------------------------------------------- */
#define TC_CMD_Q_SIG         0x1
#define TC_DOGRPT_TIMER_SIG  0x2
                          
/*--------------------------------------------------------*/
/*                 TC COMMAND IDs                         */
/*                                                        */
/*  Command Ids for commands sent from various tasks to   */
/*  TC are defined here.                                  */
/*--------------------------------------------------------*/                          
typedef enum
{
  /*--------------------------------------------------------*/
  /* Command Ids sent by CM entity                          */
  /*--------------------------------------------------------*/
  CMTC_CHANGE_TEST_MODE_RSP = 0x02000000,  /* CM commands start here */
  CMTC_STOP_MODE_REQ,   

  /*--------------------------------------------------------*/
  /* Command Ids sent by RAB entity                         */
  /*--------------------------------------------------------*/
  RABMTC_RAB_SETUP_IND = 0x03000000,  /* RABM commands start here */
  RABMTC_RAB_REL_IND,   

  /*--------------------------------------------------------*/
  /* Command Ids sent by GMM entity                         */
  /*--------------------------------------------------------*/
  GMMTC_ERROR_IND = 0x04000000,  /* GMM commands start here */
  GMMTC_UNITDATA_IND,

  /*--------------------------------------------------------*/
  /* Command Ids sent by MM entity                          */
  /*--------------------------------------------------------*/
  MMTC_UNITDATA_IND = 0x05000000,  /* MM commands start here */
  MMTC_RAB_SETUP_IND,
  MMTC_RAB_RELEASE_IND,
  MMTC_ERROR_IND,

  /*--------------------------------------------------------*/
  /* Command Ids sent by RRC entity                         */
  /*--------------------------------------------------------*/
  RRCTC_TEST_LOOPBACK_CNF = 0x06000000,  /* RRC commands start here */
  RRCTC_TEST_LOOPBACK_REJ,

  /*----------------------------------------------------------*/
  /* Command Ids sent by LOOPBACK  entity (from RLC context) */
  /*----------------------------------------------------------*/
  LB_PROC_DL_RLC_DATA_CMD = 0x07000000,  /* LB commands start here */

  /*--------------------------------------------------------*/
  /* End of TC command ids                                  */
  /*--------------------------------------------------------*/
  TC_MAX_CMDS

} tc_cmd_enum_type;

                                 
/* -----------------------------------------------------------------------
** TC_CMD_HDR_TYPE data structure. All the external entities sending the
** TC commands need to set these fields properly.  
** ----------------------------------------------------------------------- */
typedef struct 
{
  q_link_type  link;        /* Quaue link */
  byte  message_set;        /* Identifies the Entity */
  tc_cmd_enum_type cmd_id;  /* TC Command ID */
} tc_cmd_hdr_type;
                                  
/* -----------------------------------------------------------------------
** Enumeration of all the Commands/primitivies that TC task receives.
** Note: The primitive data types are defined in TC_<entity>.h files.
**       where <entity> is one of the following: GMM,RABM,MM,RRC,CM.
** ----------------------------------------------------------------------- */

typedef struct
{
  tc_cmd_hdr_type      header;

  union 
  {  
    /*--------------------------------------------------------*/
    /* Commands sent from GMM to TC                           */
    /*--------------------------------------------------------*/
    gmmtc_unitdata_ind_T          gmm_unitdata_ind;
    gmmtc_error_ind_T             gmm_error_ind;         

    /*--------------------------------------------------------*/
    /* Commands sent from MM to TC                            */
    /*--------------------------------------------------------*/
    mmtc_unitdata_ind_T          mm_unitdata_ind;
    mmtc_error_ind_T             mm_error_ind;
    mmtc_rab_setup_ind_T         mm_rab_est_ind;
    mmtc_rab_rel_ind_T           mm_rab_rel_ind;

    /*--------------------------------------------------------*/
    /* Commands sent from RABM to TC                          */
    /*--------------------------------------------------------*/
    rabmtc_rab_setup_ind_T      rabm_rab_est_ind;
    rabmtc_rab_rel_ind_T        rabm_rab_rel_ind;    
 
    /*--------------------------------------------------------*/
    /* Commands sent from CM to TC                            */
    /*--------------------------------------------------------*/
    cmtc_change_test_mode_rsp_T  cm_change_test_mode_rsp;
    cmtc_stop_mode_req_T         cm_stop_mode_req;

    /*--------------------------------------------------------*/
    /* Commands sent from TC to TC (internal commands)        */
    /*--------------------------------------------------------*/
    lb_proc_dl_rlc_data_ind_T   lb_proc_dl_rlc_data_ind;
  } cmd;

} tc_cmd_type;

/* =======================================================================
**                         Function Definitions
** ======================================================================= */

/* Function used by other entities to put a command in TC command Queue */
extern void tc_put_cmd(tc_cmd_type *cmd_ptr);

/* Function used by other entities to get TC command type */
extern tc_cmd_type *tc_get_cmd_buf(void);

extern void tc_nas_rab_list_init (byte rab_id);


#endif
