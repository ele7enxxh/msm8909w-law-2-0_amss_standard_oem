/*===========================================================================
                       T C  R R C   S O U R C E   F I L E
DESCRIPTION

   This  file  contains the  function definitions necessary for communication
   between the Test Controller (TC) and Radio Resource Control (RRC) entities.

EXTERNALIZED FUNCTIONS   
  tc_process_rrc_cmd()  
    This function processes the commands received on RRC command queue.

  tc_rrc_notify_test_mode()    
    This function notifies RRC entity about mode change to TEST.

  tc_rrc_notify_normal_mode()    
    This function notifies RRC entity about mode change to NORMAL.

  tc_rrc_notify_lb_mode()    
    This function notifies RRC entity about the loopback mode.

INITIALIZATION AND SEQUENCING REQUIREMENTS  
None

Copyright (c) 2002 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*==========================================================================
                        EDIT HISTORY FOR MODULE
This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/tc/src/tcrrc.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
 
when       who     what, where, why
--------   ---     --------------------------------------------------------- 
03/28/12   us       Compilation warnings fixed 
08/31/04   mks      Added support for handling multiple radio bearers during
                    loopback call setup
11/27/02   mks      Now calling tc_handle_rrc_lb_rej () to cleanup the TC
                    state, UL/DL variables/data structures associated with
                    the loopback call and transition UE to normal mode. 
06/26/02   mks      tc_process_rrc_cmd() now calls tc_close_ue_test_loop_complete() 
                    to confirm the test loop completion 
05/03/02   mks      Updated tc_notify_lb_mode & tc_rrc_notify_lb_mode to pass 
                    the dummy signaling flag to RRC
02/24/02   mks      Changed RRCTC_MODE_CHANGE to RRC_TC_MODE_CHANGE
02/01/02   mks      Created file.

===========================================================================*/
// NPR
#include "mmcp_variation.h"
#include "customer.h"

#ifdef FEATURE_TC

#include "tc.h"
#include "tctask.h"
#include "tctask_v.h"
#include "rrccmd.h"
#include "mm.h"
#ifdef FEATURE_TDSCDMA
#include "tdsrrccmd.h"
#endif /* FEATURE_TDSCDMA */

/*==========================================================================
** EXTERNAL FUNCTION DEFINITIONS
** ========================================================================*/
/* -----------------------------------------------------------------------

** FUNCTION     : TC_CLOSE_UE_TEST_LOOP_COMPLETE()

** DESCRIPTION  : This function sends CLOSE_UE_TEST_LOOP_COMPLETE to network
** ----------------------------------------------------------------------- */
void tc_close_ue_test_loop_complete(void);

/* -----------------------------------------------------------------------

** FUNCTION     : TC_HANDLE_RRC_LB_REJ()

** DESCRIPTION  : This function handles test loopback reject message 
** (RRCTC_TEST_LOOPBACK_REJ) from RRC. It transitions TC from loopback 
** test mode to normal mode, deregisters RLC queues, cleans up the 
** UL/DL data structures & variables associated with the loopback call,
** notifies RABM & CM about transition of UE mode to NORMAL.
** ----------------------------------------------------------------------- */
void tc_handle_rrc_lb_rej(void);

/* ========================================================================
** LOCAL FUNCTION PROTOTYPES.
** These functions are available only in the scope of this file.
** ====================================================================== */

/* -----------------------------------------------------------------------
** FUNCTION     : TC_NOTIFY_RRC()

** DESCRIPTION  : This is a helper function to notify RB mode to RRC entity.
** ----------------------------------------------------------------------- */
LOCAL boolean tc_notify_rrc(byte);


/*===========================================================================
FUNCTION : tc_process_rrc_cmd()

DESCRIPTION
   This function processes the commands sent by RRC entity.

DEPENDENCIES
   None

RETURN VALUES
   None

SIDE EFFECTS
   None
===========================================================================*/
void tc_process_rrc_cmd(tc_cmd_type *cmd_ptr)
{
   MSG_HIGH_DS_1(TC_SUB,"Processing RRC command CMD_ID :0x%X",cmd_ptr->header.cmd_id);
   
   switch (cmd_ptr->header.cmd_id)
   {
      case RRCTC_TEST_LOOPBACK_CNF: /* Test loopback confirmation */
         MSG_HIGH_DS_0(TC_SUB,"Received RRCTC_TEST_LOOPBACK_CNF from RRC");
         MSG_HIGH_DS_0(TC_SUB,"Sending CLOSE_UE_TEST_LOOP_COMPLETE to Network");
         tc_close_ue_test_loop_complete();
         break;

      case RRCTC_TEST_LOOPBACK_REJ: /* Test loopback reject */
         MSG_HIGH_DS_0(TC_SUB,"Received RRCTC_TEST_LOOPBACK_REJ from RRC");
         /* RRC rejected loopback mode - Initiate cleanup */
         tc_handle_rrc_lb_rej();
         break;

      default: /* Unknown command from RRC */
         ERR("Received unknown command from RRC",0,0,0);
         break;
   }
}

/*===========================================================================
FUNCTION : tc_rrc_notify_test_mode()

DESCRIPTION
   This function notifies RRC entity of mode change to TEST.

DEPENDENCIES
   None

RETURN VALUES
   boolean     TRUE  -> Successfully notified RRC about mode change
               FALSE -> Failed to notify mode change to RRC 

SIDE EFFECTS
   None
===========================================================================*/
boolean tc_rrc_notify_test_mode(void)
{
  return (tc_notify_rrc(RRCTC_RB_TEST_MODE));
}

/*===========================================================================
FUNCTION : tc_rrc_notify_normal_mode()

DESCRIPTION
   This function notifies RRC entity of mode change to NORMAL.

DEPENDENCIES
   None

RETURN VALUES
   boolean     TRUE  -> Successfully notified RRC about mode change
               FALSE -> Failed to notify mode change to RRC 

SIDE EFFECTS
   None
===========================================================================*/
boolean tc_rrc_notify_normal_mode(void)
{
  return (tc_notify_rrc(RRCTC_NORMAL_MODE));
}

/*===========================================================================
FUNCTION : tc_notify_rrc()

DESCRIPTION
   This function notifies RRC entity of mode change.

DEPENDENCIES
   None

RETURN VALUES
   boolean     TRUE  -> Successfully notified RRC about mode change
               FALSE -> Failed to notify mode change to RRC 

SIDE EFFECTS
   None
===========================================================================*/
boolean tc_notify_rrc(byte mode_type)
{
   boolean sent_flag = TRUE;
   rrc_cmd_type *rrc_cmd_ptr = NULL;
#ifdef FEATURE_SEGMENT_LOADING
   interface_t *pi_TC_TASK_W = NULL;
   interface_t *pi_TC_TASK_T = NULL;
   
   pi_TC_TASK_T = get_tdscdma_interface();
   pi_TC_TASK_W = get_wcdma_interface();
#endif

#ifdef FEATURE_SEGMENT_LOADING
   if (is_wcdma_mode())
   {
#ifdef FEATURE_WCDMA
     rrc_cmd_ptr = IWCDMA_rrc_get_tc_cmd_buf(pi_TC_TASK_W);
#endif
   }
   else if (is_tdscdma_mode()) 
   {
#ifdef FEATURE_TDSCDMA
     rrc_cmd_ptr = (rrc_cmd_type *)ITDSCDMA_tdsrrc_get_tc_cmd_buf(pi_TC_TASK_T);
#endif
   }
   else
   {
       MSG_HIGH_DS_0(TC_SUB,"Wrong Segment");
       return FALSE;
   }
#else
  if (is_wcdma_mode())
   {
#ifdef FEATURE_WCDMA
     rrc_cmd_ptr = rrc_get_tc_cmd_buf();
#endif 
   }
#ifdef FEATURE_TDSCDMA
   else if (is_tdscdma_mode())
   {
     rrc_cmd_ptr = (rrc_cmd_type *)tdsrrc_get_tc_cmd_buf();
   }
#endif
#endif

   if (rrc_cmd_ptr != NULL)
   {
      // Format the header part of the primitive 
      rrc_cmd_ptr->cmd_hdr.cmd_id = RRC_TC_MODE_CHANGE;

      // Format the command part of the primitive

      // Set the loopback mode type
      rrc_cmd_ptr->cmd.mode_change.mode_type = mode_type;

#ifdef FEATURE_SEGMENT_LOADING
      // Send the command to RRC task
      if (is_wcdma_mode())
      {
        IWCDMA_rrc_put_tc_cmd(pi_TC_TASK_W, rrc_cmd_ptr);
      }
      else if (is_tdscdma_mode())
      {
        ITDSCDMA_tdsrrc_put_tc_cmd(pi_TC_TASK_T, ( tdsrrc_tc_cmd_type *)rrc_cmd_ptr);
      }
      else
      {
          MSG_HIGH_DS_0(TC_SUB,"Wrong Segment");
          return FALSE;
      }
#else
      // Send the command to RRC task
      if (is_wcdma_mode())
      {
#ifdef FEATURE_WCDMA
        rrc_put_tc_cmd(rrc_cmd_ptr);
#endif
      }
#ifdef FEATURE_TDSCDMA
      else if (is_tdscdma_mode())
      {
        tdsrrc_put_tc_cmd(( tdsrrc_tc_cmd_type *)rrc_cmd_ptr);
      }
#endif /* FEATURE_TDSCDMA */
#endif
   }
   else /* Allocation Failure */
   {
      ERR("Failed to allocate buffer for RRC_TC_MODE_CHANGE",0,0,0);
      sent_flag = FALSE;      
   }
   
   return sent_flag;
}

/*===========================================================================
FUNCTION : tc_notify_lb_mode()

DESCRIPTION
   This function notifies the loopback mode to RRC entity.

DEPENDENCIES
   None

RETURN VALUES
   boolean     TRUE  -> Successfully notified RRC about mode change
               FALSE -> Failed to notify mode change to RRC 

SIDE EFFECTS
   None
===========================================================================*/
boolean tc_notify_lb_mode(byte lb_mode, boolean dummy_signaling_flag)
{
   boolean sent_flag = TRUE;
   rrc_cmd_type *rrc_cmd_ptr = NULL;
#ifdef FEATURE_SEGMENT_LOADING
   interface_t *pi_TC_TASK_W = NULL;
   interface_t *pi_TC_TASK_T = NULL;

   pi_TC_TASK_T = get_tdscdma_interface();
   pi_TC_TASK_W = get_wcdma_interface();
#endif
   
#ifdef FEATURE_SEGMENT_LOADING
   if (is_wcdma_mode())
   {
#ifdef FEATURE_WCDMA
     rrc_cmd_ptr = IWCDMA_rrc_get_tc_cmd_buf(pi_TC_TASK_W);
#endif
   }
   else if (is_tdscdma_mode())
   {
#ifdef FEATURE_TDSCDMA
     rrc_cmd_ptr = (rrc_cmd_type *)ITDSCDMA_tdsrrc_get_tc_cmd_buf(pi_TC_TASK_T);
#endif
   }
    else
   {
       MSG_HIGH_DS_0(TC_SUB,"Wrong Segment");
       return FALSE;
   }
#else
if (is_wcdma_mode())
   {
#ifdef FEATURE_WCDMA
     rrc_cmd_ptr = rrc_get_tc_cmd_buf();
#endif
   }
#ifdef FEATURE_TDSCDMA
   else if (is_tdscdma_mode())
   {
     rrc_cmd_ptr = (rrc_cmd_type *)tdsrrc_get_tc_cmd_buf();
   }
#endif /* FEATURE_TDSCDMA */
#endif
   if (rrc_cmd_ptr != NULL)
   {
      // Format the header part of the primitive 
      rrc_cmd_ptr->cmd_hdr.cmd_id = RRC_TC_TEST_LOOPBACK_IND;

      // Format the command part of the primitive
      rrc_cmd_ptr->cmd.test_loopback_ind.lb_mode_type = lb_mode;

      // Indicate to RRC whether dummy signaling is enabled or not
      rrc_cmd_ptr->cmd.test_loopback_ind.dummy_signaling_enabled = dummy_signaling_flag;

      // Send the command to RRC task
      if (is_wcdma_mode())
      {
#ifdef FEATURE_WCDMA
#ifdef FEATURE_SEGMENT_LOADING
        IWCDMA_rrc_put_tc_cmd(pi_TC_TASK_W, rrc_cmd_ptr);
#else
        rrc_put_tc_cmd(rrc_cmd_ptr);
#endif
#endif
      }
#ifdef FEATURE_TDSCDMA
      else if (is_tdscdma_mode())
      {
#ifdef FEATURE_SEGMENT_LOADING
        ITDSCDMA_tdsrrc_put_tc_cmd(pi_TC_TASK_T, (tdsrrc_tc_cmd_type *)rrc_cmd_ptr);
#else
        tdsrrc_put_tc_cmd((tdsrrc_tc_cmd_type *)rrc_cmd_ptr);
#endif
      }
#endif /* FEATURE_TDSCDMA */
   }
   else /* Allocation Failure */
   {
      ERR("Failed to allocate buffer for RRC_TC_TEST_LOOPBACK_IND",0,0,0);
      sent_flag = FALSE;      
   }
   
   return sent_flag;
}

/*===========================================================================
FUNCTION : tc_rrc_notify_lb_mode()

DESCRIPTION
   This function notifies the loopback mode to RRC entity.

DEPENDENCIES
   None

RETURN VALUES
   boolean     TRUE  -> Successfully notified RRC about mode change
               FALSE -> Failed to notify mode change to RRC 

SIDE EFFECTS
   None
===========================================================================*/
boolean tc_rrc_notify_lb_mode(byte lb_mode, boolean dummy_signaling_flag)
{
  switch (lb_mode)
  {
     case TC_LOOPBACK_MODE1:
        return(tc_notify_lb_mode(RRCTC_LB_MODE1, dummy_signaling_flag));

     case TC_LOOPBACK_MODE2:
        return(tc_notify_lb_mode(RRCTC_LB_MODE2, dummy_signaling_flag));

#ifdef FEATURE_MBMS
     #error code not present
#endif

     default:
        ERR("Unknown LB Mode: %d",lb_mode,0,0);
        return(FALSE);
  }
}
      
#endif /* FEATURE_TC */
