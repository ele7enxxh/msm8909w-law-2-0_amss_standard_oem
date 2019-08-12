/*===========================================================================
                       T C  C M   S O U R C E   F I L E 

DESCRIPTION

   This file contains the function definitions necessary for communication
   between the Test Controller (TC) and Call Manager (CM) entities.
   
EXTERNALIZED FUNCTIONS   

  tc_process_cm_cmd()  
    This function processes the commands received on CM command queue.

  tc_cm_notify_test_setup_in_progress()    
    This function notifies CM about UE test mode setup in progress.

  tc_cm_notify_normal_mode()    
    This function notifies CM about change of UE mode to NORMAL.

  tc_cm_notify_lb_mode()    
    This function notifies CM about the UE test mode (LB1/LB2)

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/tc/src/tccm.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
 
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/12/12   am       Fixing compilation warnings  
03/28/12   us       Compilation warnings fixed 
08/31/04   mks      Cleaned up LINT warnings
10/24/02   mks      Now notifying CM whether dummy signalling is enabled or
                    not before closing the UE test loop
07/15/02   mks      Included cmwll.h instead of cmll.h
03/29/02   mks      Added tc_cm_notify_lb_mode & 
                    tc_cm_notify_test_setup_in_progress
02/24/02   mks      Changed tc_process_cmd to tc_process_cm_cmd()
02/01/02   mks      Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/
// NPR

#include "mmcp_variation.h"
#include "customer.h"

#ifdef FEATURE_TC

#include "cmwll.h"
#include "tc.h"
#include "tctask.h"
#include "tctask_v.h"


/*===========================================================================
** EXTERNAL FUNCTION DEFINITIONS 
** =========================================================================*/

/* -----------------------------------------------------------------------
** FUNCTION     : TC_ACTIVATE_RB_TEST_MODE()

** DESCRIPTION  : This function activates RB_TEST_MODE 
** ----------------------------------------------------------------------- */
extern void tc_activate_rb_test_mode(void);

/* -----------------------------------------------------------------------
** FUNCTION     : LB_CLEANUP(void)

** DESCRIPTION  : This function performs cleanup for TC entity
** ----------------------------------------------------------------------- */
extern void lb_cleanup(void);


/* ========================================================================
** LOCAL FUNCTION PROTOTYPES.
** These functions are available only in the scope of this file.
** ====================================================================== */

/* -----------------------------------------------------------------------
** FUNCTION     : TC_PROCESS_CHANGE_TEST_MODE_RSP()

** DESCRIPTION  : This function processes change_test_mode response from CM
** ----------------------------------------------------------------------- */
LOCAL void tc_process_change_test_mode_rsp(cmtc_change_test_mode_status_T);

/* -----------------------------------------------------------------------
** FUNCTION     : TC_PROCESS_CMTC_STOP_MODE_REQ()

** DESCRIPTION  : This function processes stop mode request from CM entity
** ----------------------------------------------------------------------- */
LOCAL void tc_process_stop_mode_req(cmtc_stop_mode_type_T);


/* -----------------------------------------------------------------------
** FUNCTION     : TC_CM_NOTIFY_MODE()

** DESCRIPTION  : This is a helper function to notify UE mode to CM entity.
** ----------------------------------------------------------------------- */
LOCAL void tc_cm_notify_mode(cmtc_ue_mode_type_T);

extern void tc_notify_rabm(ue_mode_type_T mode);

/*===========================================================================
FUNCTION : TC_PROCESS_CMD 

DESCRIPTION
  This function processes the commands sent by CM entity.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/ 
void tc_process_cm_cmd(tc_cmd_type* cmd_ptr)
{  
  
  switch (cmd_ptr->header.cmd_id)
  {
    case  CMTC_CHANGE_TEST_MODE_RSP: /* Process change test mode response */  
      MSG_HIGH_DS_1(TC_SUB,"Processing change test mode response from CM with status : %d", cmd_ptr->cmd.cm_change_test_mode_rsp.status ); 
	  tc_process_change_test_mode_rsp(cmd_ptr->cmd.cm_change_test_mode_rsp.status);    
    break;

  case  CMTC_STOP_MODE_REQ:        /* Process stop mode request */
      MSG_HIGH_DS_1(TC_SUB,"Processing stop mode request from CM with reason : %d",cmd_ptr->cmd.cm_stop_mode_req.stop_mode);
	  tc_process_stop_mode_req(cmd_ptr->cmd.cm_stop_mode_req.stop_mode);
      break;  

    default:                         /* Unknown Command from CM */
      ERR("Unknown cmd from CM",0,0,0);
      break;
  }
}

/*===========================================================================
FUNCTION : TC_CM_NOTIFY_TEST_MODE_IN_PROGRESS 

DESCRIPTION
  This function notifies CM about UE test mode in progress

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tc_cm_notify_test_setup_in_progress(void)
{
  tc_cm_notify_mode(CMTC_UE_TEST_SETUP_IN_PROGRESS);
}

/*===========================================================================
FUNCTION : TC_CM_NOTIFY_NORMAL_MODE 

DESCRIPTION
  This function sends a command to CM requesting it to go in to normal mode

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tc_cm_notify_normal_mode(void)
{
  tc_cm_notify_mode(CMTC_UE_NORMAL_MODE);

}


/*===========================================================================
FUNCTION : tc_cm_notify_lb_mode()

DESCRIPTION
   This function notifies the loopback mode (LB1/LB2) & dummy signalling 
   information to CM entity.

DEPENDENCIES
   None

RETURN VALUES
   None  

SIDE EFFECTS
   None
===========================================================================*/
void tc_cm_notify_lb_mode(byte lb_mode, boolean dummy_sig_active)
{
  switch (lb_mode)
  {
     case TC_LOOPBACK_MODE1:
        if (dummy_sig_active)
        {
          tc_cm_notify_mode(CMTC_UE_LB_MODE1_WITH_DUMMY_SIGNALLING);
        }
        else
        {
          tc_cm_notify_mode(CMTC_UE_LB_MODE1);
        }
        break;

     case TC_LOOPBACK_MODE2:
        if (dummy_sig_active)
        {
          tc_cm_notify_mode(CMTC_UE_LB_MODE2_WITH_DUMMY_SIGNALLING);
        }
        else
        {
          tc_cm_notify_mode(CMTC_UE_LB_MODE2);
        }
        break;
#ifdef FEATURE_MBMS
     #error code not present
#endif

     case TC_UNKNOWN_MODE:
     default:
        ERR("Unknown LB Mode: %d",lb_mode,0,0);
        break;
  }
}

/*===========================================================================
FUNCTION : TC_CM_NOTIFY_MODE 

DESCRIPTION
  This function notifies the UE mode to Call Manager (CM) entity 

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tc_cm_notify_mode(cmtc_ue_mode_type_T mode_type)
{
  cm_rpt_type *cm_cmd_ptr;

  if ((cm_cmd_ptr = cm_get_cmd_buf()) != NULL) /* Get CM command */
  {
     // Format the header part of the primitive
     cm_cmd_ptr->hdr.cmd = CM_CHANGE_TEST_MODE_IND;

     // Format the command part of the primitive
     cm_cmd_ptr->cmd.tc_change_test_mode_ind.mode_type = mode_type;     

     MSG_HIGH_DS_1(TC_SUB,"Notify CM : UE Mode :0x%X",mode_type);

     // Send the message to CM task
     cm_put_cmd(cm_cmd_ptr); 
  }
  else /* Allocation failure */
  { 
     ERR( "Failed to allocate buffer for CM_CHANGE_TEST_MODE_IND", 0,0,0);
  }
}

/*===========================================================================
FUNCTION : TC_PROCESS_CHANGE_TEST_MODE_RSP 

DESCRIPTION
  This function processes change of mode response from CM entity. 

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  If Call Manager rejects the change of test mode, current state of TC 
  is set to TC_UNKNOWN_MODE.

===========================================================================*/
void tc_process_change_test_mode_rsp(cmtc_change_test_mode_status_T status)
{ 
  switch (status)
  {
    case CMTC_REJECT:
	  /* Reset TC state machine */
      tc_current_state = TC_UNKNOWN_MODE;
      rb_test_mode     = RB_UNKNOWN_MODE;
      //lb_deregister_wm_rlc (); // TC_PENDING
      break;

    case CMTC_CONFIRM:
	  if (rb_test_mode == RB_TEST_MODE_PENDING_CM_CONFIRM)
      {
        // Initiate RB test mode activation procedure
        tc_activate_rb_test_mode();   
      }
      break;

    default:
      MSG_HIGH_DS_0(TC_SUB,"UE Test Mode Change : Unknown status from CM"); 
      break;
   }
}

/*===========================================================================
FUNCTION : TC_PROCESS_STOP_MODE_REQ 

DESCRIPTION
  This function processes stop mode request from CM entity, resets TC
  state machine and perfroms cleanup for TC task. 

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tc_process_stop_mode_req(cmtc_stop_mode_type_T stop_mode)
{ 
  /* -------------------------------------------------------------------
  ** Process CMTC_CHANGE_TEST_MODE_RSP sent by CM.
  ** -----------------------------------------------------------------*/
  switch(stop_mode)
  {
     case CMTC_UE_POWER_OFF: /* UE Power off */
	   break;
 
     case CMTC_SIM_ERROR:    /* SIM Error */
	   break;

     default: /* Unknown Mode - TC honors stop mode request any way */
       ERR( "Received CMTC_STOP_MODE_REQ with unknown mode from CM", 0,0,0);
       break; 
  }

#ifdef FEATURE_DUAL_SIM
  if (rb_test_mode == RB_TEST_MODE)
  {
    // dont process rest of the instructions, as this would end the Active loopback mode
    return ;
  }
#endif

  /* -------------------------------------------------------------------
  ** Reset the state machine and perform cleanup for TC task
  ** -----------------------------------------------------------------*/
  rb_test_mode = RB_UNKNOWN_MODE;

  tc_current_state = TC_UNKNOWN_MODE;

  if (tc_rat_rab_established_on == SYS_RAT_UMTS_RADIO_ACCESS)
  {
#ifdef FEATURE_WCDMA
    lb_deregister_wm_rlc ();
#endif
  }
#ifdef FEATURE_TDSCDMA
  else if (tc_rat_rab_established_on == SYS_RAT_TDS_RADIO_ACCESS)
  {
    lb_deregister_wm_tdsrlc();
  }
#endif /* FEATURE_TDSCDMA */
  lb_cleanup();

  // Notify RABM about normal mode
  MSG_HIGH_DS_0(TC_SUB,"Notify RABM : UE Normal mode");
  tc_notify_rabm(UE_NORMAL_MODE); 

}  
#endif /* FEATURE_TC */
