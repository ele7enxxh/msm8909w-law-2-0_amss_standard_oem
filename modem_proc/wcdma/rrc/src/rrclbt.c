/*===========================================================================
         R R C   L O O P B A C K   T E S T   M O D U L E 

DESCRIPTION

  This module performs the actions related to the loopback test. It 
  maintanences the loopback test mode and test type information. It processes 
  the messages from and to the TC. It also performs the channel reconfiguration
  to support the loopback test based on the test type. However, the RB setup 
  and release for the loopback test is done through the RRC RB Setup procedure.
  
  If there is a failure this module sends an Active Set Update failure
  message with the appropriate failure cause.
  
EXTERNALIZED FUNCTIONS

  rrclbt_procedure_event_handler   Event handler for this procedure
  rrclbt_init_procedure            Power-up initialization of the procedure

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Procedure must be initialized at power-up by calling rrcasu_init_procedure. 
  
Copyright (c) 2001-2003, 2007-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrclbt.c_v   1.4   14 May 2002 16:54:24   xgao  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrclbt.c#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/19/14   sp      Made changes for Mav debug snapshot
09/03/14   sp      Made changes to save procedure substates in crash debug info
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
03/20/13   mp      Removed tmc.h include file
01/03/13   ad      Made changes to reduce the redundant F3s
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
06/29/10   as      Removed feature flag WCDMA_UNIT_TEST for rrc stub code
05/24/10   sk      Upper layer if under FEATURE_MBMS
05/06/10   rm      Added feature falg WCDMA_UNIT_TEST for rrc stub code
05/05/10   as      Moved stub functions to rrc test directory
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
03/28/09   rm      CMI Featurisation
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
05/02/08   da      Added API rrclbt_get_lb_mode_and_dummy_sig_enabled() which 
                   returns the loopback mode status
03/06/08   da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
12/12/07   vr      Support for LB3 mode for MBMS
02/03/03   kc      Removed lint errors
11/22/02   xfg     Adding one more case to send reject to TC
11/22/02   xfg     Send reject to TC if a loopback test call is released due
                   to a failure
09/20/02   bu      Changed l1_status to chan_cfg_status in channel config cnf.
05/07/02   xfg     Added support for loopback mode 2 dummy signaling test
04/02/02   xfg     Added support for the subsequent LB call without releasing the 
                   current RB.
02/14/02   xfg     Added the feature tag: FEATURE_TC for loopback feature
02/07/02   xfg     Changed lb_test_mode to an enmu type instead of constants.
01/28/02   xfg     Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"
#include "err.h"
#include "msg.h"
#include "memheap.h"
#include "rrclbt.h"
#include "rrcsend.h"
#include "rrccmd_v.h"
#include "rrcdata_v.h"
#include "rrcintif.h"
#include "rrclcm.h"
#include "rrcllc.h"
#include "rrcscmgr.h"
#include "rrcccm.h"
#include "ms.h"
#include "tctask.h"
#ifdef FEATURE_CMI
#include "tctask_v.h"
#endif
#ifdef TEST_FRAMEWORK
#error code not present
#endif

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */
/* Maximum number of transactions accepted for the ASU message             */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
/* RRC Test Mode                                                           */
byte rrc_test_mode;

/* Loopback Test Type                                                      */
byte rrc_lb_type;

/* Loopback 2 dummy signaling test mode */
boolean rrc_dummy_signaling_test;

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */
/* LB substate */
rrclbt_substates_e_type lbt_substate;

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */


/* =======================================================================
**                            Function Definitions
** ======================================================================= */


/*===========================================================================

FUNCTION  RRCLBT_INIT_DATA

DESCRIPTION

  This function clears all the procedure variables and puts the initial
  values wherever needed.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrclbt_init_data( void )
{                   

  /* Reset variables for each transaction */
  lbt_substate = RRCLBT_INITIAL;
  rrc_test_mode = RRCTC_NORMAL_MODE;
  rrc_dummy_signaling_test = FALSE;
}



/*===========================================================================

FUNCTION  RRCLBT_IS_LB_TEST

DESCRIPTION

  This function returns True if RRC is in the loopback test mode, otherwise,
  returns False.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrclbt_is_lb_on( void )
{                   

  /* Check if RRC is in the Loopback test */
  if ( rrc_test_mode == RRCTC_RB_TEST_MODE ) {
    return ( TRUE );
  }
  else
  {
    return ( FALSE );
  }
}

/*===========================================================================

FUNCTION  RRCLBT_GET_LB_MODE_AND_DUMMY_SIG_ENABLED

DESCRIPTION

  This functions returns TRUE and the LB mode and signalling enabled status.
  If LB is not active, returns false
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrclbt_get_lb_mode_and_dummy_sig_enabled( rrc_lb_test_mode_e_type *lb_mode,
                                                  boolean  *dummy_signaling_enabled)
{                   
  boolean ans = TRUE;
  /* Check if RRC is in the Loopback test */
  if ((lbt_substate == RRCLBT_LB_IN_PROGRESS) &&
      (rrc_test_mode == RRCTC_RB_TEST_MODE)) 
  { 
    /* Set the LP test mode */
    if (rrc_lb_type == RRCTC_LB_MODE1)

    {
      *lb_mode = LB_TEST_MODE_1;
    }
    else if (rrc_lb_type == RRCTC_LB_MODE2)
    {
      *lb_mode = LB_TEST_MODE_2;
    }
    else
    {
      ans = FALSE;
    }
    *dummy_signaling_enabled = rrc_dummy_signaling_test;
  }
  else
  {
    ans = FALSE;
  }

  return ans;
}
/*===========================================================================

FUNCTION   RRCLBT_REQUEST_CHANNEL_RECONFIG

DESCRIPTION

  This function sends the RRC_CHANNEL_CONFIG_REQ command to LLC
  to request a channel reconfiguration for loopback test.
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrclbt_request_channel_reconfig( void )
{
  rrc_cmd_type *cmd_ptr;

  /* Get the command buffer */
  if((cmd_ptr = rrc_get_int_cmd_buf())!=NULL)
  {
    /* Fill in the command id  */
    cmd_ptr->cmd_hdr.cmd_id = RRC_CHANNEL_CONFIG_REQ;

    /* Fill in procedure id    */
    cmd_ptr->cmd.chan_config_req.procedure = RRC_PROCEDURE_LBT;

    /* Indicate that state change is not required,
    since we will be in CELL_DCH only */
    cmd_ptr->cmd.chan_config_req.rrc_state_change_required = FALSE;
    
    /* Indicate that we would like to get a confirmation 
    back from LLC */
    cmd_ptr->cmd.chan_config_req.rrc_channel_config_cnf_required = TRUE;

    /* set the number of RB to config to 0 */
    cmd_ptr->cmd.chan_config_req.num_rb_to_config = 0;

    /* Set the LP test mode */
    if (rrc_lb_type == RRCTC_LB_MODE1)

    {
      cmd_ptr->cmd.chan_config_req.lb_test_mode = LB_TEST_MODE_1;
    }
    else 
    {
      cmd_ptr->cmd.chan_config_req.lb_test_mode = LB_TEST_MODE_2;
    }

    /* Set Dummy signaling test mode */
    cmd_ptr->cmd.chan_config_req.dummy_signaling_enabled = 
                                 rrc_dummy_signaling_test;

    WRRC_MSG0_HIGH(" Sending CHAN_CONFIG_REQ for LBT");
    rrc_put_int_cmd( cmd_ptr );

  }
}/* end function rrclbt_request_channel_reconfig */




/*===========================================================================

FUNCTION  RRCLBT_SEND_LB_CNF

DESCRIPTION

  This function sends a RRCTC_TEST_LOOPBACK_CNF message to TC to indicate 
  LB has successfully been configured.
  
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrclbt_send_lb_cnf( void )
{                   
  tc_cmd_type *cmd_ptr;

  /* get a command TC command buffer */
  if ( (cmd_ptr = tc_get_cmd_buf()) != NULL )
  {
    /* Send LB Request to TC */
    cmd_ptr->header.message_set = MS_RRC_TC;
    cmd_ptr->header.cmd_id = RRCTC_TEST_LOOPBACK_CNF;

    tc_put_cmd(cmd_ptr);
  }
  else
  {
    /* Out of memory!!  */
    ERR_FATAL("Can't allocate LB CNF buf!", 0, 0, 0);
  }
  return;
}



/*===========================================================================

FUNCTION  RRCLBT_SEND_LB_REJ

DESCRIPTION

  This function sends a RRCTC_TEST_LOOPBACK_REJ message to TC to indicate 
  LB can not be configured.
  
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrclbt_send_lb_rej( void )
{                   
  tc_cmd_type *cmd_ptr;

  /* get a command TC command buffer */
  if ( (cmd_ptr = tc_get_cmd_buf()) != NULL )
  {
    /* Send LB Request to TC */
    cmd_ptr->header.message_set = MS_RRC_TC;
    cmd_ptr->header.cmd_id = RRCTC_TEST_LOOPBACK_REJ;

    tc_put_cmd(cmd_ptr);
    WRRC_MSG0_HIGH("LOOPBACK REJ is sent to TC");
  }
  else
  {
    /* Out of memory!!  */
    ERR_FATAL("Can't allocate LB REJ buf!", 0, 0, 0);
  }
  return;
}



/*===========================================================================

FUNCTION   RRCLBT_PROCESS_CHANNEL_CONFIG_CNF

DESCRIPTION

  This function processes the received Channel Config Confirmation
  from LLC. 
          
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrclbt_process_channel_config_cnf
(
  rrc_channel_config_cnf_type *ptr  /* Pointer to the Channel cfg cnf msg */
)
{
  if( ptr->chan_cfg_status != RRCLLC_CHAN_CFG_SUCCESS )
  { 
    /* Channel reconfig failed, clear the LB test */
    WRRC_MSG0_ERROR("Channel config failed for LBT");
    rrclbt_send_lb_rej();
    rrclbt_init_data();
  }
  else
  {
    /* Inform TC that channel reconfig completed */
    rrclbt_send_lb_cnf();

    /* Change LBT state to LP_IN_PROGRESS */
    lbt_substate = RRCLBT_LB_IN_PROGRESS;
  }
} /* end of rrclbt_process_channel_config_cnf */



/*===========================================================================

FUNCTION   RRCLBT_INITIAL_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the LB is in the RRCLBT_INITIAL substate. This function
  looks at the received event and does the appropriate processing
  of the event.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrclbt_initial_substate_event_handler
(
  rrc_cmd_type *cmd_ptr /* Received command                   */
)
{

  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_TC_MODE_CHANGE:
      
      /* Set RRC Test Mode according received command */
      rrc_test_mode=cmd_ptr->cmd.mode_change.mode_type;
      WRRC_MSG1_HIGH("Mode Changed: %d", rrc_test_mode);
      break;

    case RRC_STATE_CHANGE_IND:

      /* If LP test enabled, clear the state */
      if (rrc_test_mode == RRCTC_RB_TEST_MODE) {
        rrclbt_send_lb_rej();
        rrclbt_init_data();
        WRRC_MSG0_HIGH("State to Idle, LP stopped");
      }
      break;

    case RRC_TC_TEST_LOOPBACK_IND:

      /* Store LB test type and perform the channel re-configuration */
      if (rrc_test_mode != RRCTC_RB_TEST_MODE) {
        rrc_test_mode = RRCTC_RB_TEST_MODE;
      }
      rrc_lb_type = cmd_ptr->cmd.test_loopback_ind.lb_mode_type;
      rrc_dummy_signaling_test = 
        cmd_ptr->cmd.test_loopback_ind.dummy_signaling_enabled;

      WRRC_MSG2_HIGH("Mode:%d, Dummy Signaling:%d", 
               rrc_lb_type, rrc_dummy_signaling_test);

      /* Request LLC to perform channel reconfiguration */
      if (rrc_lb_type == RRCTC_LB_MODE1 || 
          rrc_lb_type == RRCTC_LB_MODE2)
      {
        rrclbt_request_channel_reconfig(); 

        /* Change the substate */
        lbt_substate = RRCLBT_WAIT_FOR_CHAN_CFG_CNF;
      }
      #ifdef FEATURE_MBMS
      #error code not present
#endif
      else
      {
        /* send LB reject to TC */
        rrclbt_send_lb_rej();
        WRRC_MSG1_HIGH("Invalid LB Mode: %d", rrc_lb_type);
        rrclbt_init_data();
      }

      break;

    default:

      /* No other events are normal in this
      substate */
      WRRC_MSG2_HIGH("Ignoring LB event %x in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, lbt_substate);
      break;
  }
}



/*===========================================================================

FUNCTION   RRCLBT_WFCHCNF_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the LBT procedure is in the RRCLBT_WAIT_FOR_CHAN_CFG_CNF substate.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrclbt_wfchcnf_substate_event_handler
(
  rrc_cmd_type *cmd_ptr /* Received command                   */
)
{
  /* Switch on the received command. */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_CHANNEL_CONFIG_CNF:
      
      /* Process the channel config confirm command */
      WRRC_MSG0_HIGH("Processing CHANNEL_CONFIG_CNF cmd");
      rrclbt_process_channel_config_cnf
        ( &(cmd_ptr->cmd.chan_config_cnf) );  
      break;

    case RRC_TC_MODE_CHANGE:
      
      /* Check the new test mode in the command */
      if ( cmd_ptr->cmd.mode_change.mode_type == RRCTC_NORMAL_MODE ) {
        /* LB is terminated */
        rrclbt_init_data();
        WRRC_MSG0_HIGH("LP is stopped by TC");
      }
      break;

    case RRC_STATE_CHANGE_IND:

      /* LB is terminated due to state transition to Idle */
      rrclbt_init_data();
      rrclbt_send_lb_rej();
      WRRC_MSG0_HIGH("State to Idle, LP stopped");
      break;

    default:

      /* No other events are normal in this
      substate */
      WRRC_MSG2_HIGH("Ignoring LB event %x in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, lbt_substate);
      break;
  }
}




/*===========================================================================

FUNCTION   RRCLBT_LBINP_SUBSTATE_EVENT_HANDLER

DESCRIPTION

  This function handles the events received by this module when
  the procedure is in the LP Test in progress substate.
      
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
static void rrclbt_lbinp_substate_event_handler
(
  rrc_cmd_type *cmd_ptr  /* Received command                   */
)
{
  /* Switch on the received command. */
  switch( cmd_ptr->cmd_hdr.cmd_id )
  {
    case RRC_TC_MODE_CHANGE:

       WRRC_MSG1_HIGH("RRC_TC_MODE_CHANGE %d",cmd_ptr->cmd.mode_change.mode_type);
      /* If Test mode is normal, test is terminated */
      if (cmd_ptr->cmd.mode_change.mode_type == RRCTC_NORMAL_MODE)
      {
        rrclbt_init_data();
        WRRC_MSG0_HIGH("LB is completed");
      }
      break;

    case RRC_TC_TEST_LOOPBACK_IND:

      /* Store LB test type and perform the channel re-configuration */
      if (rrc_test_mode != RRCTC_RB_TEST_MODE) {
        rrc_test_mode = RRCTC_RB_TEST_MODE;
      }
      WRRC_MSG1_HIGH("RRC_TC_TEST_LOOPBACK_IND %d",cmd_ptr->cmd.test_loopback_ind.lb_mode_type);
      /* Store LB test type and perform the channel re-configuration */
      if (rrc_lb_type == cmd_ptr->cmd.test_loopback_ind.lb_mode_type &&
          rrc_dummy_signaling_test == cmd_ptr->cmd.test_loopback_ind.dummy_signaling_enabled)
      {
        /* no need to perform the re-configuration */
        rrclbt_send_lb_cnf();
        
      }
      else
      {
        /* Request LLC to perform channel reconfiguration */
        if (cmd_ptr->cmd.test_loopback_ind.lb_mode_type == RRCTC_LB_MODE1 ||
            cmd_ptr->cmd.test_loopback_ind.lb_mode_type == RRCTC_LB_MODE2)
        {
          rrc_lb_type = cmd_ptr->cmd.test_loopback_ind.lb_mode_type;
          rrc_dummy_signaling_test = cmd_ptr->cmd.test_loopback_ind.dummy_signaling_enabled;
 
          WRRC_MSG2_HIGH("LB Mode:%d, Dummy Signaling:%d",
                   rrc_lb_type, rrc_dummy_signaling_test);

          rrclbt_request_channel_reconfig();

          /* Change the substate */
          lbt_substate = RRCLBT_WAIT_FOR_CHAN_CFG_CNF;
        }
        #ifdef FEATURE_MBMS
        #error code not present
#endif
        else
        {
          /* send LB reject to TC */
          rrclbt_send_lb_rej();
          WRRC_MSG1_HIGH("Invalid LB Mode: %d", 
                    cmd_ptr->cmd.test_loopback_ind.lb_mode_type);
        }
      }
      break;

    case RRC_STATE_CHANGE_IND:

      /* LP test is terminated due to the RRC state change */
      rrclbt_send_lb_rej();
      rrclbt_init_data();
      WRRC_MSG0_HIGH("LP stopped due to state change");
      break;
      
    default:

      /* No other events are normal in this
      substate */
      WRRC_MSG2_HIGH("Ignoring LB event %x in substate %d",
              cmd_ptr->cmd_hdr.cmd_id, lbt_substate);
      break;
  }
}




/*===========================================================================

FUNCTION RRCLBT_PROCEDURE_EVENT_HANDLER


DESCRIPTION

  This is the main function handling external events received by the
  Active Set Update procedure. This function is called by the
  dispatcher whenever an event is received for this module.  
  
DEPENDENCIES

  None

RETURN VALUE

  None

SIDE EFFECTS

  None
  
===========================================================================*/
void rrclbt_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                      /* Pointer to command */
)
{
  WRRC_MSG1_HIGH_OPT("function rrclbt_procedure_event_handler is called in lbt_substate = rrclbt_substates_e_type_value%d",lbt_substate);
  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_LBT,lbt_substate,cmd_ptr->cmd_hdr.cmd_id);

  /* Check the procedure substate and procedure the event accordingly */  
  switch( lbt_substate )
  {
    case RRCLBT_INITIAL:
      /* Call the event handler for Initial substate */
      rrclbt_initial_substate_event_handler( cmd_ptr );
      break;

    case RRCLBT_WAIT_FOR_CHAN_CFG_CNF:
      /* Call the event handler for waiting for chan cfg cnf substate */
      rrclbt_wfchcnf_substate_event_handler( cmd_ptr );
      break;
  
    case RRCLBT_LB_IN_PROGRESS:
      /* Call the event handler for LB in progress substate */
      rrclbt_lbinp_substate_event_handler( cmd_ptr );
      break;

    default:
      /* Invalid substate - should never happen. Clear
         all procedure vars */
      WRRC_MSG1_ERROR("Invalid LB Substate %d", lbt_substate);
      rrclbt_init_data();
      break;
  }/* end switch */
}





/*===========================================================================

FUNCTION  RRCLBT_INIT_PROCEDURE

DESCRIPTION

  This function initializes the LBT procedure.
    
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrclbt_init_procedure( void )
{
  rrclbt_init_data(); /* Initialize all the procedure variables */

  /* Register for state change  - When state changes from DCH
  to any other state we want to be notified since this procedure
  is active only in the DCH state. */
  rrcscmgr_register_for_scn( RRC_PROCEDURE_LBT,  /* Procedure name */
                             RRC_STATE_WILDCARD, /* From State     */
                             RRC_STATE_DISCONNECTED  /* To State       */
                           );
}

/*===========================================================================

FUNCTION  rrclbt_return_test_mode

DESCRIPTION
returns test mode
    
DEPENDENCIES

  None.
 
RETURN VALUE
  byte
  
SIDE EFFECTS

  None.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
byte rrclbt_return_test_mode(void)
{
  return rrc_test_mode;
}

/*===========================================================================

FUNCTION  rrclbt_return_lbt_substate

DESCRIPTION
returns lbt substate
    
DEPENDENCIES

  None.
 
RETURN VALUE
  rrclbt_substates_e_type
  
SIDE EFFECTS

  None.

===========================================================================*/
#ifdef FEATURE_QSH_DUMP
  __attribute__((section(".uncompressible.text")))
#endif
uint8 rrclbt_return_lbt_substate(void)
{
  return (uint8)lbt_substate;
}
