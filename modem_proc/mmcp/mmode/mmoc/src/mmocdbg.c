/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
               M U L T I M O D E   C O N T R O L L E R ( MMoC )

                     D E B U G      M O D U L E

GENERAL DESCRIPTION
   This module contains the debug related functionality of MMoC.

EXTERNALIZED FUNCTIONS
  mmocdbg_print_message ()
  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The MMoC task should be started before DEBUG APIs are called.


Copyright (c) 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: 

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/05/13   jvo     Added 5 mmocdbg_print_x functions to functionalize usage
                   of MMOC_PRINT_MSG_HIGH under MMOC_STRING_MAPS_ACTIVE
08/28/13   jvo     Cleanup remaining Fusion pieces
09/27/12   vm      Removing DIME featurization and cleanup
03/02/12   rk      Adding support for deativating dormant protocol through
                   MMOC
02/15/12   vk      Replace usage of AEE library function with corresponding 
                   CoreBSP library
01/17/12   aj/cl   SC SVLTE SO 33 changes 
01/11/12   xs      Remove MMOC_TRANS_PWR_SAVE_EXIT transaction state
09/14/11   vk      New MMOC Transaction "MMOC_TRANS_DEACT_1XCSFB_CMD" related
                   changes.
07/22/11   xs      MMOC SC_SVLTE 1.0 merging to Mainline
07/13/11   nk      Updated stubs in UT environment to be in sync with QTF. 
05/20/11   ak      Adding new prot state for blocked RAT on SVLTEII.
05/16/11   xs      Added another stop mode rquest to deactivate NAS in suspend
                   state
05/06/11   rk      Correcting the order in mmocdbg_trans_e_type_string_map
03/09/11   cl      Add support for SVLTE2 MMSS arbitration
02/28/11   aj      Add wait for activation_cnf state in DO to LTE reselection
12/10/10   rk      MO/MT 1XCSFB call successful scenarios support
12/09/10   rk      Updating debug string arrays
11/10/10   am      Changes for MUTE to MOB migration
11/05/10   gm      Changes to wait for session open confirmation
10/27/10   rk      Reverting support for deativating dormant protocol 
10/28/10   gm      Fix to compiler warning.
10/27/10   rk      Adding support for deativating dormant protocol through
                   MMOC
05/24/10   mp      Added support for session close and 
                   MMOC_TRANS_MMGSDI_INFO_IND transaction to avoid nested
                   callbacks
01/28/10   mp      EF_EPRL Implementation
11/21/09   rn      LTE-eHRPD Redirection support
10/16/08   sv      Correcting Featurization for Memory Reduction
09/19/08   jd      Memory reduction changes
04/28/08   fj      Lint cleanup, CR 144389
03/12/08   aj      Edit header to reflect p4 path and revision
03/19/07   ka      Replacing banned apis
01/31/07   ka      Adding new state in MMOC to wait for TMC to ack operating
                   mode change.
08/02/06   ka      Adding additional entry in prot_state string map. 
04/03/06   ka      Changing copyright.
12/20/05   ka      Initial revision

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "mmocdbg.h" /* Includes mmoc debug related info */
#include <string.h>
#include <stringl/stringl.h>

#ifdef FEATURE_MMOC_UNIT_TEST
#error code not present
#endif

/*===========================================================================
=============================================================================
=============================================================================
==================== MMoC DEBUG STRING MAPS =================================
=============================================================================
=============================================================================
===========================================================================*/

#ifndef FEATURE_MMOC_LOW_MEM_TARGET

#ifdef MMOC_STRING_MAPS_ACTIVE

  /* Provide strings for  values mmoc_cmd_e_type */
  const char* mmocdbg_cmd_e_type_string_map [ MMOC_CMD_MAX ] = {
    "SUBSCRIPTION_CHGD",
    "PROT_GEN_CMD",
    "OPRT_MODE_CHGD",
    "WKUP_FROM_PWRSAVE",
    "DUAL_STANDBY_CHGD",
    "PROT_REDIR_IND",
    "PROT_HO_IND",
    "MMGSDI_INFO_IND",
    "DEACT_1XCSFB_PROT",
    "SUSPEND_SS",
    "DEACT_FROM_DORMANT",
    "MCFG_REFRESH"
  };


  /* Provide strings for  values mmoc_rpt_e_type */
const char* mmocdbg_rpt_e_type_string_map [ MMOC_RPT_MAX ] = {
  "PROT_DEACTD_CNF",
  "PROT_AUTO_DEACTD_IND",
  "PH_STAT_CHGD_CNF",
  "PROT_GEN_CMD_CNF",
  "PROT_AUTO_ACTD_IND",
  "MMGSDI_CNF",
  "DS_STAT_CHGD_CNF",
  "ACTIVATION_CNF",
  "PS_DETACH_CNF",
  "1XCSFB_PROT_DEACTD_CNF",
  "IRAT_HOLD_USER_ACT_CNF",
  "UE_MODE_SWITCH_CNF",
  "SUBS_CAP_CHGD_CNF",
  "RPT_UE_MODE_SWITCH_CNF_1X",
  "RPT_UE_MODE_SWITCH_CNF_HDR"
};


    /* Provide strings for  values mmoc_trans_state_e_type */
const char* mmocdbg_trans_state_e_type_string_map [ MMOC_TRANS_STATE_MAX ] = {
  "NULL",
  "PROT_DEACT_ENTER",
  "WAIT_DEACTD_CNF",
  "MMGSDI_READ_ENTER",
  "MMGSDI_READ_CNF",
  "PROT_PH_STAT_ENTER",
  "WAIT_PH_STAT_CNF",
  "GEN_CMD_ENTER",
  "WAIT_GEN_CMD_CNF",
  "WAIT_AUTO_DEACTD_IND",
  "WAIT_AUTO_ACTD_IND",
  "HDR_DEACT_ENTER",
  "WAIT_HDR_DEACTD_CNF",
  "PROT_REDIR_ENTER",
  "WAIT_SESSION_OPEN_CNF",
  "PROT_HO_ENTER",
  "DS_STAT_CHGD_ENTER",
  "WAIT_DS_STAT_CHGD_CNF",
  "GEN_CMD_ACTIVATION_ENTER",
  "WAIT_ACTIVATION_CNF",
  "WAIT_PS_DETACH_CNF",
  "HYBR2_DEACT_ENTER",
  "WAIT_HYBR2_DEACTD_CNF",
  "HYBR3_DEACT_ENTER",
  "WAIT_HYBR3_DEACTD_CNF",
  "WAIT_DEACTD_CNF_GWL",
  "WAIT_1XCSFB_DEACT_CNF",
  "SUSPEND_SS_ENTER",
  "RESUME_SS_ENTER",
  "WAIT_HOLD_USER_ACT_CNF",
  "WAIT_UE_MODE_SWITCH",
  "WAIT_SUBS_CAP_CHGD_ENTER",
  "WAIT_SUBS_CAP_CHGD_CNF",
  "WAIT_UE_MODE_SWITCH_CNF_1X",
  "WAIT_UE_MODE_SWITCH_CNF_HDR"
};

    /* Provide strings for  values mmoc_trans_e_type */
const char* mmocdbg_trans_e_type_string_map [ MMOC_TRANS_MAX ] = {
  "NULL",
  "SUBSC_CHGD",
  "ONLINE",
  "OFFLINE",
  "PROT_GEN_CMD",
  "PWR_DOWN",
  "PWR_SAVE_ENTER",
  "PROT_AUTO_DEACTD_IND",
  "DUAL_STANDBY_CHGD",
  "PROT_REDIR_IND",
  "PROT_HO_IND",
  "MMGSDI_INFO_IND",
  "DEACT_1XCSFB_CMD",
  "SUSPEND_SS",
  "DEACT_FROM_DORMANT",
  "MCFG_REFRESH"
};

/* Provide strings for  values prot_state_e_type */
const char* mmocdbg_prot_state_e_type_string_map [ PROT_STATE_MAX ] = {
  "NULL",
  "ONLINE_CDMA",
  "OFFLINE_CDMA",
  "ONLINE_AMPS",
  "OFFLINE_AMPS",
  "ONLINE_GWL",
  "ONLINE_HDR",
  "OFFLINE",
  "FTM",
  "PWR_SAVE",
  "ONLINE_DED_MEAS",
  "RESET",
  "LPM",
  "PWR_DOWN",
  "GPSONE_MSBASED",
  "DORMANT_GWL",
  "BLOCKED"
};

#endif /* MMOC_STRING_MAPS_ACTIVE */

mmoc_trans_e_type                    dbg_current_trans = MMOC_TRANS_MAX;
mmoc_trans_state_e_type              dbg_current_trans_state = MMOC_TRANS_STATE_MAX;
dword dbg_curr_uptime  = 0;


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== MMoC DEBUG RELATED FUNCTIONS ===========================
=============================================================================
=============================================================================
===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION MMOCDBG_PRINT_MESSAGE

DESCRIPTION

 Behaves as printf() - takes the very same format string and any number of
 parameters and allows to print <stuff> to diagnostic monitor output
 screen.

 The problem is that %s in the format string does not work for
 MSG_HIGH (and other MSG_ macros) plus those macros support only
 maximum of 3 arguments, so this function was provided to remedy both problems.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/

void mmocdbg_print_message
(
     
     const char *string,
       /* Format in which string needs to be printed */
     ...
)
{
  va_list   arg_ptr = NULL;
  char        msg_str[ MMOC_MAX_STRING_LENGTH];

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear the string buffer 
  */
  memset (msg_str, 0, sizeof(msg_str));

  /*lint -save -e64 -e718 -e746 -e63 -e530 */
  va_start( arg_ptr, string );
  /*lint -restore */

  /*  Writing max of MMOC_MAX_STRING_LENGTH-1 characters only, 
  **  so the end null character stays untouched.
  */  
  (void)vsnprintf (msg_str, (MMOC_MAX_STRING_LENGTH - 1), string, arg_ptr);
  va_end( arg_ptr );

  MSG_SPRINTF_1(MSG_SSID_DFLT, MSG_LVL_HIGH, "=MMOC= %s", msg_str);

} /* mmocdbg_print_message() */


/*===========================================================================

FUNCTION mmocdbg_print_protocol_state

DESCRIPTION
  Prints out the current protocol state information

DEPENDENCIES
  mmocdbg_print_message()

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void mmocdbg_print_protocol_state
(
  mmoc_state_info_s_type *mmoc_info_ptr
    /* Pointer to MMoC's state information */
)
{
  prot_state_e_type                    main_stack_prot_state;
  prot_state_e_type                    hybr2_stack_prot_state;
  
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  prot_state_e_type                    hybr3_stack_prot_state;
#endif
  prot_state_e_type                    hybr_hdr_stack_prot_state;

  /* Validate input parameter(s) */
  if ( mmoc_info_ptr == NULL )
  {
    MMOC_ERR_0( "NULL mmoc_info_ptr");
    MMOC_ASSERT(FALSE); /* Fail test if this happens */
    return;
  }

  /* Initialize locals after check for null */
  main_stack_prot_state     = mmoc_info_ptr->prot_state [SD_SS_MAIN];
  hybr2_stack_prot_state    = mmoc_info_ptr->prot_state [SD_SS_HYBR_2];
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  hybr3_stack_prot_state    = mmoc_info_ptr->prot_state [SD_SS_HYBR_3];
#endif
  hybr_hdr_stack_prot_state = mmoc_info_ptr->prot_state [SD_SS_HYBR_HDR];

  /* Validate indices into string array */
  if ( main_stack_prot_state < 0 ||
       main_stack_prot_state >= PROT_STATE_MAX ||
       hybr2_stack_prot_state < 0 ||
       hybr2_stack_prot_state >= PROT_STATE_MAX ||
       
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
       hybr3_stack_prot_state < 0 ||
       hybr3_stack_prot_state >= PROT_STATE_MAX ||
#endif
       hybr_hdr_stack_prot_state < 0 ||
       hybr_hdr_stack_prot_state >= PROT_STATE_MAX )
  {
    MMOC_ERR_3( "Bad prot. state(s)"
                    " [MAIN] %d | [HYBR_GW] %d | [HYBR_HDR] %d",
                    main_stack_prot_state,
                    hybr2_stack_prot_state, 
                    hybr_hdr_stack_prot_state);
    
    #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
    if(mmoc_is_sxlte())
    { 
      MMOC_ERR_3( "Bad prot_state: MAIN %d HYBR2 %d HYBR3 %d",
                    main_stack_prot_state,
                    hybr2_stack_prot_state, 
                    hybr3_stack_prot_state);
    }
      #endif
    
    MMOC_ASSERT(FALSE); /* Fail test if this happens */
    return;
  }

  #ifdef MMOC_STRING_MAPS_ACTIVE
  mmocdbg_print_protocol_state_interpreted(
    mmoc_info_ptr,
    main_stack_prot_state,
    hybr2_stack_prot_state,
    #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
    hybr3_stack_prot_state,
    #endif
    hybr_hdr_stack_prot_state);
  #else
  mmocdbg_print_protocol_state_not_interpreted(
    mmoc_info_ptr,
    main_stack_prot_state,
    hybr2_stack_prot_state,
    #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
    hybr3_stack_prot_state,
    #endif
    hybr_hdr_stack_prot_state);
  #endif /* MMOC_STRING_MAPS_ACTIVE */
  
} /* mmocdbg_print_protocol_state */


#ifdef MMOC_STRING_MAPS_ACTIVE
/*===========================================================================

FUNCTION mmocdbg_print_protocol_state_interpreted

DESCRIPTION
  Prints out the current protocol state information with interpreted strings

DEPENDENCIES
  mmocdbg_print_message()

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void mmocdbg_print_protocol_state_interpreted
(
  mmoc_state_info_s_type *mmoc_info_ptr,
    /* Pointer to MMoC's state information */

  prot_state_e_type       main_stack_prot_state,
    /* main stack's protocol state */

  prot_state_e_type       hybr2_stack_prot_state,
	/* hybr2 stack's protocol state */
 
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  prot_state_e_type       hybr3_stack_prot_state,
    /* hybr3 stack's protocol state */
  #endif

  prot_state_e_type       hybr_hdr_stack_prot_state
	/* HDR stack's protocol state */
)
{
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if( mmoc_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM) && 
      mmoc_info_ptr->subs_feature[0] == SYS_SUBS_FEATURE_MODE_NORMAL &&
      mmoc_info_ptr->subs_feature[1] == SYS_SUBS_FEATURE_MODE_NORMAL &&
      mmoc_is_sub2_map_to_hybr3(mmoc_info_ptr) )
  {
    /* print state of main, HDR, hybr3 */
    MMOC_PRINT_MSG_HIGH(
      "Prot_state MAIN %d(%s), HDR %d(%s), HYBR3 %d(%s)",
      main_stack_prot_state,
      mmocdbg_prot_state_e_type_string_map[main_stack_prot_state],
      hybr_hdr_stack_prot_state,
      mmocdbg_prot_state_e_type_string_map[hybr_hdr_stack_prot_state],
      hybr3_stack_prot_state,
      mmocdbg_prot_state_e_type_string_map[hybr3_stack_prot_state]);
  }
  else
  #endif
  {
  /* Print state with interpreted strings */
     #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
    if(mmoc_is_sxlte())
    { 	  
       MMOC_PRINT_MSG_HIGH(
       "Prot_state [MAIN] %d(%s) -- [HDR] %d(%s) -- [HYBR_GW] %d(%s) -- [HYBR_GW3] %d(%s)",
       main_stack_prot_state,
       mmocdbg_prot_state_e_type_string_map[main_stack_prot_state],
       hybr_hdr_stack_prot_state,
       mmocdbg_prot_state_e_type_string_map[hybr_hdr_stack_prot_state],
       hybr2_stack_prot_state,
       mmocdbg_prot_state_e_type_string_map[hybr2_stack_prot_state],
       hybr3_stack_prot_state,
       mmocdbg_prot_state_e_type_string_map[hybr3_stack_prot_state]);
    }
    else
     #endif
    {
      MMOC_PRINT_MSG_HIGH(
        "Prot_state MAIN %d(%s), HYBR2 %d(%s), HDR %d(%s)",
        main_stack_prot_state,
        mmocdbg_prot_state_e_type_string_map[main_stack_prot_state],
        hybr2_stack_prot_state,
        mmocdbg_prot_state_e_type_string_map[hybr2_stack_prot_state],
        hybr_hdr_stack_prot_state,
        mmocdbg_prot_state_e_type_string_map[hybr_hdr_stack_prot_state]);
      
    }
  }
}
#endif


/*===========================================================================

FUNCTION mmocdbg_print_protocol_state_not_interpreted

DESCRIPTION
  Prints out the current protocol state information without interpreted strings

DEPENDENCIES
  mmocdbg_print_message()

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void mmocdbg_print_protocol_state_not_interpreted
(
  mmoc_state_info_s_type *mmoc_info_ptr,
    /* Pointer to MMoC's state information */

  prot_state_e_type	      main_stack_prot_state,
    /* main stack's protocol state */
  
  prot_state_e_type	      hybr2_stack_prot_state,
    /* hybr2 stack's protocol state */
  
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  prot_state_e_type	      hybr3_stack_prot_state,
    /* hybr3 stack's protocol state */
  #endif
  
  prot_state_e_type	      hybr_hdr_stack_prot_state
    /* HDR stack's protocol state */
)
{
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if( mmoc_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM) && 
      mmoc_info_ptr->subs_feature[0] == SYS_SUBS_FEATURE_MODE_NORMAL &&
      mmoc_info_ptr->subs_feature[1] == SYS_SUBS_FEATURE_MODE_NORMAL &&
      mmoc_is_sub2_map_to_hybr3(mmoc_info_ptr) )
  {
    /* print state of main, HDR, hybr3 */
    MMOC_PRINT_MSG_HIGH(
    "Prot_state MAIN %d HDR %d HYBR3 %d",
    main_stack_prot_state,
    hybr_hdr_stack_prot_state,
    hybr3_stack_prot_state);
  }
  else
  #endif
  {
  /* Print state without interpreted strings */
  MMOC_MSG_HIGH_3(
    "Prot_state [MAIN] %d -- [HYBR_GW] %d -- [HYBR_HDR] %d",
    main_stack_prot_state,
    hybr2_stack_prot_state,
    hybr_hdr_stack_prot_state);
    
    #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
    if(mmoc_is_sxlte())
    {
      MMOC_MSG_HIGH_3(
      "Prot_state [MAIN] %d -- [HYBR_GW] %d -- [HYBR_GW3] %d",
      main_stack_prot_state,
      hybr2_stack_prot_state,
      hybr3_stack_prot_state);
    }
    #endif
  }
}


/*===========================================================================

FUNCTION mmocdbg_print_transaction_state

DESCRIPTION
  Prints out the current transaction state

DEPENDENCIES
  mmocdbg_print_message()

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void mmocdbg_print_transaction_state
(
  mmoc_state_info_s_type *mmoc_info_ptr
    /* Pointer to MMoC's state information */
)
{
  mmoc_trans_e_type                    current_trans;
  mmoc_trans_state_e_type              current_trans_state;

  /* Validate input parameter(s) */
  if ( mmoc_info_ptr == NULL )
  {
    MMOC_ERR_0( "NULL mmoc_info_ptr");
    MMOC_ASSERT(FALSE); /* Fail test if this happens */
    return;
  }

  /* Initialize local after check for null */
  current_trans             = mmoc_info_ptr->curr_trans;
  current_trans_state       = mmoc_info_ptr->trans_state;

  /* Validate index into string array */
  if ( current_trans < 0 || current_trans >= MMOC_TRANS_MAX )
  {
    MMOC_ERR_1( "Bad current_trans %d", current_trans);
    MMOC_ASSERT(FALSE); /* Fail test if this happens */
    return;
  }

  if ( current_trans_state < 0 ||
       current_trans_state >= MMOC_TRANS_STATE_MAX )
  {
    MMOC_ERR_1( "Bad current_trans_state %d", current_trans_state);
    MMOC_ASSERT(FALSE); /* Fail test if this happens */
    return;
  }

  #ifdef MMOC_STRING_MAPS_ACTIVE
  /* Print state with interpreted strings */
  MMOC_PRINT_MSG_HIGH (
    "Curr_trans  %d(%s), Trans_state %d(%s)", 
    current_trans,
    mmocdbg_trans_e_type_string_map [current_trans],
    current_trans_state,
    mmocdbg_trans_state_e_type_string_map [current_trans_state]);

  #else
  /* Print state without interpreted strings */
  MMOC_MSG_HIGH_1(
    "Curr_trans  %d", current_trans);
  MMOC_MSG_HIGH_1(
    "Trans_state %d", current_trans_state);

  #endif /* MMOC_STRING_MAPS_ACTIVE */
  
} /* mmocdbg_print_transaction_state */


/*===========================================================================

FUNCTION mmocdbg_print_report_received

DESCRIPTION
  Prints out the received report

DEPENDENCIES
  mmocdbg_print_message()

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void mmocdbg_print_report_received
(
  mmoc_rpt_msg_s_type *rpt_ptr
    /* The report that was received */
)
{
  mmoc_rpt_e_type                      report_name;

  /* Validate input parameter(s) */
  if ( rpt_ptr == NULL )
  {
    MMOC_ERR_0( "NULL rpt_ptr");
    MMOC_ASSERT(FALSE); /* Fail test if this happens */
    return;
  }

  /* Initialize local after check for null */
  report_name = rpt_ptr->rpt.name;

  /* Validate index into string array */
  if ( report_name < 0 || report_name >= MMOC_RPT_MAX )
  {
    MMOC_ERR_1( "Bad report_name %d", report_name);
    MMOC_ASSERT(FALSE); /* Fail test if this happens */
    return;
  }

  #ifdef MMOC_STRING_MAPS_ACTIVE
  /* Print report with interpreted string */
  MMOC_PRINT_MSG_HIGH( "Recvd report %d(%s)", report_name,
    mmocdbg_rpt_e_type_string_map [report_name]);

  #else
  /* Print report without interpreted string */
  MMOC_MSG_HIGH_1( "Recvd report %d", report_name);

  #endif /* MMOC_STRING_MAPS_ACTIVE */
  
} /* mmocdbg_print_report_received */


/*===========================================================================

FUNCTION mmocdbg_print_command_received

DESCRIPTION
  Prints out the received command

DEPENDENCIES
  mmocdbg_print_message()

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void mmocdbg_print_command_received
(
  mmoc_cmd_msg_s_type *cmd_ptr
    /* The report that was received */
)
{
  mmoc_cmd_e_type                      command_name;

  /* Validate input parameter(s) */
  if ( cmd_ptr == NULL )
  {
    MMOC_ERR_0( "NULL cmd_ptr");
    MMOC_ASSERT(FALSE); /* Fail test if this happens */
    return;
  }

  /* Initialize local after check for null */
  command_name = cmd_ptr->cmd.name;

 
  /* Validate index into string array */
  if ( command_name < 0 || command_name >= MMOC_CMD_MAX )
  {
    MMOC_ERR_1( "Bad command_name %d", command_name);
    MMOC_ASSERT(FALSE); /* Fail test if this happens */
    return;
  }

  #ifdef MMOC_STRING_MAPS_ACTIVE
  /* Print command with interpreted string */
  MMOC_PRINT_MSG_HIGH( "Recvd command %d(%s)", command_name,
    mmocdbg_cmd_e_type_string_map [command_name] );

  #else
  /* Print command without interpreted string */
  MMOC_MSG_HIGH_1( "Recvd command %d", command_name);

  #endif /* MMOC_STRING_MAPS_ACTIVE */
  
} /* mmocdbg_print_command_received */


/*===========================================================================

FUNCTION mmocdbg_print_before_trans

DESCRIPTION
  Prints out the transaction state and protocol state before the transaction

DEPENDENCIES
  mmocdbg_print_message()

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void mmocdbg_print_before_trans
(
  mmoc_state_info_s_type *mmoc_info_ptr
    /* Pointer to MMoC's state information */
)
{
  mmoc_trans_e_type					 current_trans;
  mmoc_trans_state_e_type 			 current_trans_state;
  dword                              curr_uptime = time_get_uptime_secs();
  /* Validate input parameter(s) */
  if ( mmoc_info_ptr == NULL )
  {
    MMOC_ERR_0( "NULL mmoc_info_ptr");
    MMOC_ASSERT(FALSE); /* Fail test if this happens */
    return;
  }
  
  /* Initialize local after check for null */
  current_trans			  = mmoc_info_ptr->curr_trans;
  current_trans_state 	  = mmoc_info_ptr->trans_state;
  
  /* Validate index into string array */
  if ( current_trans < 0 || current_trans >= MMOC_TRANS_MAX )
  {
    MMOC_ERR_1( "Bad current_trans %d", current_trans);
    MMOC_ASSERT(FALSE); /* Fail test if this happens */
    return;
  }
  
  if ( current_trans_state < 0 ||
  	 current_trans_state >= MMOC_TRANS_STATE_MAX )
  {
    MMOC_ERR_1( "Bad current_trans_state %d", current_trans_state);
    MMOC_ASSERT(FALSE); /* Fail test if this happens */
    return;
  }
  
  if(current_trans == dbg_current_trans
     && current_trans_state == dbg_current_trans_state
     && curr_uptime == dbg_curr_uptime)
  {
    /* To avoid too many debugs in same second*/
    return;
  }

  dbg_current_trans = current_trans;
  dbg_current_trans_state = current_trans_state;
  dbg_curr_uptime = curr_uptime;


  #ifdef MMOC_STRING_MAPS_ACTIVE
  /* Print state with interpreted strings */
  MMOC_PRINT_MSG_HIGH (
    "Before event was processed: Curr_trans %d(%s), Trans_state %d(%s)", 
    current_trans,
    mmocdbg_trans_e_type_string_map [current_trans],
    current_trans_state,
    mmocdbg_trans_state_e_type_string_map [current_trans_state]);
  
  #else
  /* Print state without interpreted strings */
  MMOC_MSG_HIGH_2(
    "Before event was processed: Curr_trans %d, Trans_state %d", 
    current_trans, current_trans_state);
  
  #endif /* MMOC_STRING_MAPS_ACTIVE */

  /* print out protocol state */
  mmocdbg_print_protocol_state(mmoc_info_ptr);
}


/*===========================================================================

FUNCTION mmocdbg_print_after_trans

DESCRIPTION
  Prints out the transaction state and protocol state after the transaction

DEPENDENCIES
  mmocdbg_print_message()

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void mmocdbg_print_after_trans
(
  mmoc_state_info_s_type *mmoc_info_ptr
    /* Pointer to MMoC's state information */
)
{
  mmoc_trans_e_type				   current_trans;
  mmoc_trans_state_e_type 		   current_trans_state;
  dword                              curr_uptime = time_get_uptime_secs();

  /* Validate input parameter(s) */
  if ( mmoc_info_ptr == NULL )
  {
    MMOC_ERR_0( "NULL mmoc_info_ptr");
    MMOC_ASSERT(FALSE); /* Fail test if this happens */
    return;
  }
  
  /* Initialize local after check for null */
  current_trans			= mmoc_info_ptr->curr_trans;
  current_trans_state 	= mmoc_info_ptr->trans_state;
  
  /* Validate index into string array */
  if ( current_trans < 0 || current_trans >= MMOC_TRANS_MAX )
  {
    MMOC_ERR_1( "Bad current_trans %d", current_trans);
    MMOC_ASSERT(FALSE); /* Fail test if this happens */
    return;
  }
  
  if ( current_trans_state < 0 ||
     current_trans_state >= MMOC_TRANS_STATE_MAX )
  {
    MMOC_ERR_1( "Bad current_trans_state %d", current_trans_state);
    MMOC_ASSERT(FALSE); /* Fail test if this happens */
    return;
  }
  
  if(current_trans == dbg_current_trans
     && current_trans_state == dbg_current_trans_state
     && curr_uptime == dbg_curr_uptime)
  {
    /* To avoid too many debugs in same second*/
    return;
  }

  dbg_current_trans = current_trans;
  dbg_current_trans_state = current_trans_state;
  dbg_curr_uptime = curr_uptime;

  #ifdef MMOC_STRING_MAPS_ACTIVE
  /* Print state with interpreted strings */
  MMOC_PRINT_MSG_HIGH (
    "After event was processed: Curr_trans %d(%s), Trans_state %d(%s)", 
    current_trans,
    mmocdbg_trans_e_type_string_map [current_trans],
    current_trans_state,
    mmocdbg_trans_state_e_type_string_map [current_trans_state]);
  
  #else
  /* Print state without interpreted strings */
  MMOC_MSG_HIGH_2(
    "After event was processed: Curr_trans %d, Trans_state %d", 
    current_trans, current_trans_state);
  
  #endif /* MMOC_STRING_MAPS_ACTIVE */
  
  /* print out protocol state */
  mmocdbg_print_protocol_state(mmoc_info_ptr);
}


#endif /* !FEATURE_MMOC_LOW_MEM_TARGET */

