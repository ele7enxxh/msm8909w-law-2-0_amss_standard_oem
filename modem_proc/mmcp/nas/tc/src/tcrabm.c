/*===========================================================================
                       T C  R A B M   S O U R C E   F I L E 

DESCRIPTION

   This file contains the function definitions between the TC and RABM entities.
   
EXTERNALIZED FUNCTIONS   

  tc_process_rabm_cmd() : This processes a command received on RABM cmd Queue.

  tc_notify_rabm()  :  This function notifies RABM about change in UE mode.
                  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/ctest/vcs/tcrabm.c_v   1.5   25 Mar 2002 16:26:08   madiraju  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/tc/src/tcrabm.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/12/12   am     Fixing compilation warnings
08/03/11  am      Putting check for rab_id. 
07/27/11  am      Adding range check for rab_id. 
08/14/07  rk      removed FEATURE_NAS_KLOCWORK flag
05/23/07  rk    Some of the commands for RABM are handled through Signals and some are direct handled by function calls. 
                        These difference in handling the commands gives raise to process the commands in a order 
                        different than that of order they were sent.
07/11/06   rk      changed include file loopback.h to tc_loopback.h
08/08/05   hj      Now not setting RB Test mode to unknown while processing
                   rab release indication.
10/13/04   mks     Now handling loopback setup with multi-rab configuration
02/06/02   ks      TC state cleanup after RAB release
09/28/01   ks      Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
//NPR
#include "mmcp_variation.h"
#include "customer.h"

#ifdef FEATURE_TC

#include "tc.h"
#include "ms.h"
#include "tc_rabm.h"
#include "tctask.h"
#include "tctask_v.h"
#include "rabmtask_v.h"
#include "tc_loopback.h"
#include "mm.h"

/*===========================================================================
** FUNCTION DEFINITIONS 
** =========================================================================*/
//extern void lb_cleanup(void);

extern void lb_rab_cleanup (byte rab_id);
extern void reset_tc_loop (void);

#ifdef FEATURE_TDSCDMA
static boolean tc_rab_released = FALSE;
#endif
/*===========================================================================
FUNCTION : TC_PROCESS_RABM_CMD 

DESCRIPTION
  This function processes the commands sent by RABM.

DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/ 
void tc_process_rabm_cmd(tc_cmd_type* cmd_ptr)
{  
  

  switch (cmd_ptr->header.cmd_id)
  {
    case  RABMTC_RAB_SETUP_IND:    
      
      tc_process_rab_setup(cmd_ptr->cmd.rabm_rab_est_ind.rab_id);    
#ifdef FEATURE_TDSCDMA
      tc_rab_released = FALSE;
#endif
    break;

    case  RABMTC_RAB_REL_IND:
      /* -------------------------------------------------------------------
      ** Process the RABMTC_RAB_REL_IND sent by RABM.
      ** -----------------------------------------------------------------*/

      tc_process_rab_release(cmd_ptr->cmd.rabm_rab_rel_ind.rab_id);
#ifdef FEATURE_TDSCDMA
      tc_rab_released = TRUE;
#endif
      break;  

      default: 
        ERR("Received unknown cmd from RABM",0,0,0);
      break;
  }
}
/*===========================================================================
FUNCTION : TC_NOTIFY_RABM 

DESCRIPTION
  This function sends a command to the RABM subtask.

DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tc_notify_rabm(ue_mode_type_T mode_type)
{
  rabm_cmd_type *rabm_cmd;
  rabm_cmd = rabm_get_cmd_buf();  /* Get rabm command */

  if (rabm_cmd != NULL)
  {
    // Set the header 
    rabm_cmd->header.cmd_id = RABMTC_MODE_CHANGE_IND;
    rabm_cmd->header.message_set = MS_RABM_TC;

    // Set the command primitive
    rabm_cmd->cmd.tc_mode_change_ind.mode_type = mode_type;
    MSG_HIGH_DS_1(TC_SUB,"Notify RABM : Mode Type :0x%X",mode_type ); 
    rabm_put_cmd(rabm_cmd); 
  }
  else
  {
    ERR( "Unable to get a RABM command buffer",0,0,0);
  }
}

/*===========================================================================
FUNCTION : TC_PROCESS_RAB_SETUP 

DESCRIPTION
  This function processes a RAB establishment indication.

DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tc_process_rab_setup(byte rab_id)
{ 
  /* -------------------------------------------------------------------
  ** Process the RABMTC_RAB_SETUP_IND sent by RABM.
  ** -----------------------------------------------------------------*/
  // RAB is established, set the TC state machine.
  MSG_HIGH_DS_1(TC_SUB,"Processing RABMTC_RAB_SETUP_IND, RAB_ID : %d", rab_id);

#ifdef FEATURE_TDSCDMA  
  /* For the cases of back to back TC loopback, need to reset
  ** the tc flag loop because the RABs have been reinitialized
  */
  if(tc_rab_released)
  {
     reset_tc_loop ();
  }
#endif

  if(rab_id >= MAX_RAB_TO_SETUP)
  {
    ERR( "RAB setup failed: Invalid rab_id %d ", rab_id,0,0);
  }
  else if (rb_test_mode != RB_TEST_MODE)
  {
    ERR( "Wrong RB test mode",0,0,0);
  }
  else if (lb_rab_list[rab_id] != NULL)
  {
    MSG_HIGH_DS_1(TC_SUB,"TC: rabId: %d already established. No Action", rab_id);
  }
  else
  {
    /* ------------------------------------------------------------------- 
    ** Set the LB rab_id as established, ready to register the DL/UL
    ** Water mark queues with RLC for the corresponding Log_chan Id.
    ** -----------------------------------------------------------------*/
#ifdef FEATURE_MODEM_HEAP      
      lb_rab_list[rab_id] = (rab_context_info*) modem_mem_calloc( 1, sizeof(rab_context_info), MODEM_MEM_CLIENT_NAS);
#else
      lb_rab_list[rab_id] = &lb_rab_list_static [rab_id];
#endif
    if (lb_rab_list[rab_id] == NULL)
    {
        MSG_HIGH_DS_1(TC_SUB,"TC: Mem Alloc failed for rabId: %d", rab_id);
        return ;
    }
    tc_nas_rab_list_init (rab_id);
    lb_rab_list[rab_id]->rab_established = TRUE;

    if (is_wcdma_mode())
    {
      MSG_HIGH_DS_0(TC_SUB,"Establish the UL/DL water mark queues with RLC");
      tc_rat_rab_established_on = SYS_RAT_UMTS_RADIO_ACCESS;
      lb_register_rlc_wm ((byte) rab_id);
    }
#ifdef FEATURE_TDSCDMA
    else if (is_tdscdma_mode())
    {
      MSG_HIGH_DS_0(TC_SUB,"Establish the UL/DL water mark queues with TDSRLC");
      tc_rat_rab_established_on = SYS_RAT_TDS_RADIO_ACCESS;
      lb_register_tdsrlc_wm ((byte) rab_id);
    }
#endif /* FEATURE_TDSCDMA */
  }
}

/*===========================================================================
FUNCTION : TC_PROCESS_RAB_RELEASE 

DESCRIPTION
  This function processes a RAB release indication.

DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tc_process_rab_release(byte rab_id)
{
  // RAB is released, reset the TC state machine.
  MSG_HIGH_DS_1(TC_SUB,"Processing RABMTC_RAB_REL_IND, RAB_ID : %d", rab_id);

  if (rab_id < MAX_RAB_TO_SETUP && lb_rab_list[rab_id] != NULL)
  {
    if (tc_rat_rab_established_on == SYS_RAT_UMTS_RADIO_ACCESS)
    {
      lb_deregister_rab_wm_rlc (rab_id);
    }
#ifdef FEATURE_TDSCDMA
    else if (tc_rat_rab_established_on == SYS_RAT_TDS_RADIO_ACCESS)
    {
      lb_deregister_rab_wm_tdsrlc( rab_id );
    }
#endif /* FEATURE_TDSCDMA */
    lb_rab_cleanup (rab_id);
  }

}

#endif /* FEATURE_TC */
