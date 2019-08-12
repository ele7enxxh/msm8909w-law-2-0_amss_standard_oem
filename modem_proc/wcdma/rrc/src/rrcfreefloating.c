/*=========================================================================== RRC  WCDMA  FREE FLOATING MANAGER   FILE
DESCRIPTION

  This file contains RRC FREE FLOATING  procedure

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcfreefloating.c_v   1.25   19 Jun 2002 10:01:34   kpatil  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcfreefloating.c#1 $    $DateTime: 2016/12/13 08:00:12 $ $ $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/05/15   vs      Added local variable to hold return value of function rrcff_l1_get_utran_nmr_info
05/28/14   vi      Made changes to add freq index in function  rrcff_l1_get_additional_intra_freq_msmt_results
05/07/14   vi      Made changes to use Mutex instead of enable/disable preemption under critical section to avoid race condition in FF cmds.
05/06/14   geg     Use safer versions of memcpy() and memmove()
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
01/29/14   sr      Made changes to update bplmn variables properly when RRC is wating for lock and
                   sleep request is receuved from L1
01/29/14   sr      Made changes to handle FF commands from L1 when waiting for lock
01/28/14   vi      Made changes to clear freefloating signal correctly if there are more than 2 freefloating commands in freefloating queue
04/15/13   as      Made changes to pass correct pointer to memcpy
04/09/13   gv      Fixed compilation issue
03/25/13   sn      Changes to replace timer based dog reporting with signal based reporting
02/06/12   gv      Made changes to use the right structure type while doing a memcpy
12/16/11   gv      Fixed compilation issue
12/12/11   gv      Added code for RRC free floating task
===========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "wcdma_variation.h"
#include "comdef.h"

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
#include "rex.h"
#include "msg.h"
#include "err.h"

#include "rrcwrm.h"

#include "rrcsigs.h"
#include "rrctmr.h"

#include "rrcscmgr.h"
#include "rrcdata_v.h"
#include "rrccmdi.h"
#include "rrccspi.h"
#include "rrcmmif.h"
#include "rrcrrif.h"
#include "l1rrcif.h"
#include "rrcfreefloating.h"
#include "l1task.h"
#include "rrcdispatcher.h"

/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */
  rrc_cmd_e_type expected_cmd = RRC_MAX_CMDS;

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
  rex_crit_sect_type rrc_l1_ff_cmd_crit_sect;
#endif

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
extern  rrc_csp_wtog_bplmn_int_data_type  rrc_csp_wtog_bplmn_int_data;

typedef struct
{
  /* Command header common for all commands */
  rrc_cmd_type    *cmd_ptr;

  boolean keep_waiting;
}rrc_ff_wait_type;

/* -----------------------------------------------------------------------
** Fuction Definitions
** ----------------------------------------------------------------------- */

/*===========================================================================

FUNCTION                RRCFF_SET_EXPECTED_CMD_ID

DESCRIPTION
                        This function sets the expected cmd id to be received
                        in response to the requested high priority command

DEPENDENCIES
                        None

RETURN VALUE
                        None

SIDE EFFECTS
                        None
===========================================================================*/
void rrcff_set_expected_cmd_id( rrc_cmd_e_type cmd_id)
{
  expected_cmd = cmd_id;
  WRRC_MSG1_HIGH("RRCFF:Set expected_cmd = 0x%x",expected_cmd);
}

/*===========================================================================

FUNCTION                RRCFF_CHECK_FOR_L1_CMDS

DESCRIPTION
                        This function checks the L1 commands received by RRC (when waiting 
                        for response to high priority command).Process the L1 command and 
                        return the command process status

DEPENDENCIES
                        None

RETURN VALUE
                        Returns the command process status

SIDE EFFECTS
                        None

===========================================================================*/
rrc_ff_wait_type rrcff_check_for_l1_cmds( void)
{

  rrc_cmd_type *cmd_ptr;
  boolean found_exp_cmd = FALSE;
  rrc_ff_wait_type cmd_process_status;

  cmd_process_status.keep_waiting = TRUE;
  cmd_process_status.cmd_ptr = NULL;

  while((cmd_ptr = q_get(&rrc_l1_ff_cmd_q)) != NULL)
  {
    switch(cmd_ptr->cmd_hdr.cmd_id)
    {
      case RRC_CPHY_CHECK_SLEEP_REQ:
        rrcff_check_l1_sleep_req(cmd_ptr);
        break;
      default:
        if(expected_cmd == cmd_ptr->cmd_hdr.cmd_id)
        {
          cmd_process_status.keep_waiting = FALSE;
          cmd_process_status.cmd_ptr = cmd_ptr;
          found_exp_cmd = TRUE;
        }
        else
        {
          ERR_FATAL("Unexpected Message %d",cmd_ptr->cmd_hdr.cmd_id,0,0);
        }
        break;
    }
    if(found_exp_cmd)
    {
      break;
    }
    rrc_free_cmd_buf(cmd_ptr);
  }
  return cmd_process_status;
}

/*===========================================================================

FUNCTION                RRCFF_WAIT_HIGH_PRIORITY_RSP

DESCRIPTION
                        This function is the wait-loop after RRC posts a 
                        high priority request. It continues to wait till the
                        expected response for the requested command is received

DEPENDENCIES
                        None
 
RETURN VALUE
                        Returns the command process status

SIDE EFFECTS
                        None

===========================================================================*/
rrc_ff_wait_type rrcff_wait_high_priority_rsp( void)
{
  rex_sigs_type sigs;             /* hold signals from rex_wait() */

  boolean keep_waiting = TRUE;

  rrc_ff_wait_type cmd_process_status;
  cmd_process_status.keep_waiting = TRUE;
  cmd_process_status.cmd_ptr = NULL;

  do
  {                     /* until we get the specified signal */
    sigs = rex_wait( RRC_L1_FF_CMD_Q_SIG | RRC_RPT_TIMER_SIG);

    /* Need to kick the watchdog */
    if( sigs & RRC_RPT_TIMER_SIG )
    {
      rrc_dog_report_and_clear_sig();
    }
    else if (sigs & RRC_L1_FF_CMD_Q_SIG) 
    {
      cmd_process_status = rrcff_check_for_l1_cmds();
      REX_ISR_LOCK(&rrc_l1_ff_cmd_crit_sect);
      if(rrc_l1_ff_cmd_q.cnt == 0)
      {
        (void) rex_clr_sigs( rex_self(), RRC_L1_FF_CMD_Q_SIG );
      }
      REX_ISR_UNLOCK(&rrc_l1_ff_cmd_crit_sect);
      keep_waiting = cmd_process_status.keep_waiting;
    }

  } while (keep_waiting == TRUE);
  return cmd_process_status;
}

/*===========================================================================

FUNCTION                RRCFF_L1_GET_DATA_FOR_UI_DISPLAY

DESCRIPTION
                        This function sends a command to L1 to get the RSSI, RSCP, Ec/Io
                        for use by the UI and Call Manager subsystems

DEPENDENCIES
                        None

RETURN VALUE
                        None

SIDE EFFECTS
                        None

===========================================================================*/
void rrcff_l1_get_data_for_ui_display(l1_ui_display_info_struct_type *l1_data_for_ui_display)
{
  rrc_ff_wait_type  cmd_process_status;
  l1_ext_cmd_type   *l1_cmd_ptr;

  cmd_process_status.keep_waiting = TRUE;
  cmd_process_status.cmd_ptr = NULL;

  l1_cmd_ptr = l1_get_cmd_buf();

  /* Fill in the Command Header, and indicate to do it Now! */
  l1_cmd_ptr->cmd_hdr.cmd_id = CPHY_DATA_FOR_UI_DISP_REQ;

  l1_put_ff_cmd(l1_cmd_ptr);

  rrcff_set_expected_cmd_id(RRC_CPHY_DATA_FOR_UI_DISP_CNF);

  cmd_process_status = rrcff_wait_high_priority_rsp();

  *l1_data_for_ui_display = cmd_process_status.cmd_ptr->cmd.data_for_ui_disp_cnf;

  rrc_free_cmd_buf(cmd_process_status.cmd_ptr);
}

/*===========================================================================

FUNCTION                RRCFF_L1_GET_RSSI

DESCRIPTION
                        This function sends a command to L1 to get the Received 
                        Signal Strength Indication for use by the UI and Call 
                        Manager subsystems

DEPENDENCIES
                        None

RETURN VALUE
                        None

SIDE EFFECTS
                        None

 ===========================================================================*/
void rrcff_l1_get_rssi(l1_rssi_info_struct_type *rssi_data)
{
  rrc_ff_wait_type  cmd_process_status;
  l1_ext_cmd_type   *l1_cmd_ptr;

  cmd_process_status.keep_waiting = TRUE;
  cmd_process_status.cmd_ptr = NULL;

  l1_cmd_ptr = l1_get_cmd_buf();

  /* Fill in the Command Header, and indicate to do it Now! */
  l1_cmd_ptr->cmd_hdr.cmd_id = CPHY_RSSI_REQ;

  l1_put_ff_cmd(l1_cmd_ptr);

  rrcff_set_expected_cmd_id(RRC_CPHY_RSSI_CNF);

  cmd_process_status = rrcff_wait_high_priority_rsp();

  *rssi_data = cmd_process_status.cmd_ptr->cmd.rssi_cnf;

  rrc_free_cmd_buf(cmd_process_status.cmd_ptr);
}

/*===========================================================================

FUNCTION                RRCFF_L1_IS_S_CRITERIA_MET

DESCRIPTION
                        This function sends a command to L1 to compute S criteria
                        based on the values provided by RRC

DEPENDENCIES
                        None

RETURN VALUE
                        TRUE: If S Criteria is met
                        FALSE: Otherwise

SIDE EFFECTS
                        None

 ===========================================================================*/
boolean rrcff_l1_is_s_criteria_met( int16 q_qualmin, int16 q_rxlevmin, int16 ue_tx_max_pwr)
{
  rrc_ff_wait_type  cmd_process_status;
  l1_ext_cmd_type   *l1_cmd_ptr;
  boolean ret_val;

  cmd_process_status.keep_waiting = TRUE;
  cmd_process_status.cmd_ptr = NULL;

  l1_cmd_ptr = l1_get_cmd_buf();

  l1_cmd_ptr->cmd_hdr.cmd_id = CPHY_CHECK_S_CRIT_REQ;
  l1_cmd_ptr->cmd.s_criteria_req.q_qualmin = q_qualmin;
  l1_cmd_ptr->cmd.s_criteria_req.q_rxlevmin = q_rxlevmin;
  l1_cmd_ptr->cmd.s_criteria_req.ue_tx_max_pwr = ue_tx_max_pwr;

  /* Now send the primitive to L1 */
  l1_put_ff_cmd(l1_cmd_ptr);

  rrcff_set_expected_cmd_id(RRC_CPHY_CHECK_S_CRIT_CNF);

  cmd_process_status = rrcff_wait_high_priority_rsp();

  ret_val = cmd_process_status.cmd_ptr->cmd.s_criteria_cnf.status;

  rrc_free_cmd_buf(cmd_process_status.cmd_ptr);

  return(ret_val);
}

#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION                RRCFF_CHECK_L1_SLEEP_REQ

DESCRIPTION
                        This function processes the sleep request command sent by L1
                        in every DRX cycle to determine if L1 should go to sleep or not

DEPENDENCIES
                        None

RETURN VALUE
                        None

SIDE EFFECTS
                        None

 ===========================================================================*/
void rrcff_fail_l1_sleep_req(rrc_cmd_type *cmd_ptr)
{
  l1_ext_cmd_type   *l1_cmd_ptr;

  l1_cmd_ptr = l1_get_cmd_buf();

   WRRC_MSG0_HIGH("DSIM : can L1 go to sleep called when RRC waiting for lock: function rrcff_fail_l1_sleep_req is called");

  l1_cmd_ptr->cmd_hdr.cmd_id = CPHY_CHECK_SLEEP_RSP;

  l1_cmd_ptr->cmd.l1_sleep_rsp.go_to_sleep = TRUE;

  l1_cmd_ptr->cmd.l1_sleep_rsp.wtog_bplmn_in_progress = FALSE;

  l1_cmd_ptr->cmd.l1_sleep_rsp.wtow_bplmn_in_progress = FALSE;

  /* Checking if bplmn is in progress */
  if(WTOW_SEARCH_RESUME  == rrc_csp_bplmn_srch_in_progress())
  {
    if(!rrc_csp_wtog_bplmn_int_data.wtog_bplmn_srch_active)
    {
      l1_cmd_ptr->cmd.l1_sleep_rsp.wtow_bplmn_in_progress = TRUE;
    }
    else
    {
      l1_cmd_ptr->cmd.l1_sleep_rsp.wtog_bplmn_in_progress = TRUE;
    }
  }
  
  l1_put_ff_cmd(l1_cmd_ptr);
}
#endif


/*===========================================================================

FUNCTION                RRCFF_CHECK_L1_SLEEP_REQ

DESCRIPTION
                        This function processes the sleep request command sent by L1
                        in every DRX cycle to determine if L1 should go to sleep or not

DEPENDENCIES
                        None

RETURN VALUE
                        None

SIDE EFFECTS
                        None

 ===========================================================================*/
void rrcff_check_l1_sleep_req(rrc_cmd_type *cmd_ptr)
{
  uint32 timer_val;
  boolean go_to_sleep;
  boolean wtog_bplmn_in_progress;
#ifdef FEATURE_DUAL_SIM
  boolean wtow_bplmn_in_progress;
#endif

  l1_ext_cmd_type   *l1_cmd_ptr;

  WRRC_MSG0_HIGH_OPT("function rrcff_check_l1_sleep_req is called");

  timer_val = cmd_ptr->cmd.l1_sleep_req.timer_val;

#ifdef FEATURE_DUAL_SIM
  go_to_sleep = rrc_can_l1_go_to_sleep(timer_val,&wtog_bplmn_in_progress,&wtow_bplmn_in_progress);
#else
  go_to_sleep = rrc_can_l1_go_to_sleep(timer_val,&wtog_bplmn_in_progress);
#endif

  l1_cmd_ptr = l1_get_cmd_buf();

  l1_cmd_ptr->cmd_hdr.cmd_id = CPHY_CHECK_SLEEP_RSP;

  l1_cmd_ptr->cmd.l1_sleep_rsp.go_to_sleep = go_to_sleep;

  l1_cmd_ptr->cmd.l1_sleep_rsp.wtog_bplmn_in_progress = wtog_bplmn_in_progress;

#ifdef FEATURE_DUAL_SIM
  l1_cmd_ptr->cmd.l1_sleep_rsp.wtow_bplmn_in_progress = wtow_bplmn_in_progress;
#endif

  l1_put_ff_cmd(l1_cmd_ptr);
}

/*===========================================================================

FUNCTION                RRCFF_L1_GET_PERIODIC_INTER_RAT_MSMT_RESULTS

DESCRIPTION             This function send a high priority cmd to L1 to get the 
                        additonal meas of IRAT type which can be appended to 
                        meas reporting going in UL.

DEPENDENCIES
                        None

RETURN VALUE
                        TRUE: if additional meas result is present
                        FALSE  if not


SIDE EFFECTS
                        None

===========================================================================*/

boolean rrcff_l1_get_periodic_inter_rat_msmt_results
(
  uint32  meas_id,
  l1_inter_sys_meas_result_struct_type  *p_msmtres
)
{
  rrc_ff_wait_type cmd_process_status;
  l1_ext_cmd_type   *l1_cmd_ptr;

  cmd_process_status.keep_waiting = TRUE;
  cmd_process_status.cmd_ptr = NULL;

  l1_cmd_ptr = l1_get_cmd_buf();
  /* Fill in the Command Header, and indicate to do it Now! */
  l1_cmd_ptr->cmd_hdr.cmd_id = CPHY_ADDITIONAL_MEAS_DATA_REQ;

  l1_cmd_ptr->cmd.add_meas_info_req.meas_id =  meas_id;
  l1_cmd_ptr->cmd.add_meas_info_req.meas_type = L1_INTER_SYS;

/* Now send the primitive to L1 */
  l1_put_ff_cmd(l1_cmd_ptr);

  rrcff_set_expected_cmd_id(RRC_CPHY_ADDITIONAL_MEAS_DATA_RSP);

  cmd_process_status = rrcff_wait_high_priority_rsp();
  WCDMA_MEMCPY((void *)p_msmtres, 
               sizeof(l1_inter_sys_meas_result_struct_type),
               (void *)(&cmd_process_status.cmd_ptr->cmd.add_meas_info_rsp.u.inter_sys_rpt.measurements),
               sizeof(l1_inter_sys_meas_result_struct_type));

  rrc_free_cmd_buf(cmd_process_status.cmd_ptr);
  if(p_msmtres->num_msmt_results == 0)
    return FALSE;
  else
    return TRUE;
}

/*===========================================================================

FUNCTION                RRCFF_L1_GET_ADDITIONAL_INTRA_FREQ_MSMT_RESULTS

DESCRIPTION
                        This function send a high priority cmd to L1 to get the 
                        additonal meas of Intra-freq type which can be appended to 
                        meas reporting going in UL.


DEPENDENCIES
                        None

RETURN VALUE
                        TRUE :if additional meas result is present
                        FALSE: if not present


SIDE EFFECTS
                        None

===========================================================================*/

boolean rrcff_l1_get_additional_intra_freq_msmt_results
(
  uint32  meas_id,
  l1_intra_freq_meas_rpt_struct_type  *p_msmtres,
  uint8 freq_idx
)
{
  rrc_ff_wait_type cmd_process_status;
  l1_ext_cmd_type   *l1_cmd_ptr;

  cmd_process_status.keep_waiting = TRUE;
  cmd_process_status.cmd_ptr = NULL;

  l1_cmd_ptr = l1_get_cmd_buf();
  /* Fill in the Command Header, and indicate to do it Now! */
  l1_cmd_ptr->cmd_hdr.cmd_id = CPHY_ADDITIONAL_MEAS_DATA_REQ;

  l1_cmd_ptr->cmd.add_meas_info_req.meas_id =  meas_id;
  l1_cmd_ptr->cmd.add_meas_info_req.meas_type = L1_INTRA_FREQ;
  l1_cmd_ptr->cmd.add_meas_info_req.carr_idx = freq_idx;

/* Now send the primitive to L1 */
  l1_put_ff_cmd(l1_cmd_ptr);

  rrcff_set_expected_cmd_id(RRC_CPHY_ADDITIONAL_MEAS_DATA_RSP);

  cmd_process_status = rrcff_wait_high_priority_rsp();

  WCDMA_MEMCPY((void *)p_msmtres,
               sizeof(l1_intra_freq_meas_rpt_struct_type), 
               (void *)(&cmd_process_status.cmd_ptr->cmd.add_meas_info_rsp.u.intra_f_rpt),
               sizeof(l1_intra_freq_meas_rpt_struct_type));

  rrc_free_cmd_buf(cmd_process_status.cmd_ptr);
  if(p_msmtres->cell_meas_result_incl == FALSE)
    return FALSE;
  else
    return TRUE;
}

/*===========================================================================

FUNCTION                RRCFF_L1_GET_ADDITIONAL_INTERF_MSMT_RESULTS

DESCRIPTION
                        This function send a high priority cmd to L1 to get the 
                        additonal meas of Inter-freq type which can be appended to 
                        meas reporting going in UL.


DEPENDENCIES
                        None

RETURN VALUE
                        TRUE:if additional meas result is present
                        FALSE:if not present


SIDE EFFECTS
                        None

===========================================================================*/

boolean rrcff_l1_get_additional_interf_msmt_results
(
  uint32  meas_id,
  l1_inter_freq_meas_rpt_struct_type  *p_msmtres
)
{
  rrc_ff_wait_type cmd_process_status;
  l1_ext_cmd_type   *l1_cmd_ptr;

  cmd_process_status.keep_waiting = TRUE;
  cmd_process_status.cmd_ptr = NULL;

  l1_cmd_ptr = l1_get_cmd_buf();

  /* Fill in the Command Header, and indicate to do it Now! */
  l1_cmd_ptr->cmd_hdr.cmd_id = CPHY_ADDITIONAL_MEAS_DATA_REQ;

  l1_cmd_ptr->cmd.add_meas_info_req.meas_id =  meas_id;
  l1_cmd_ptr->cmd.add_meas_info_req.meas_type = L1_INTER_FREQ;

/* Now send the primitive to L1 */
  l1_put_ff_cmd(l1_cmd_ptr);

  rrcff_set_expected_cmd_id(RRC_CPHY_ADDITIONAL_MEAS_DATA_RSP);

  cmd_process_status = rrcff_wait_high_priority_rsp();

  WCDMA_MEMCPY((void *)p_msmtres, 
               sizeof(l1_inter_freq_meas_rpt_struct_type),
               (void *)(&cmd_process_status.cmd_ptr->cmd.add_meas_info_rsp.u.inter_f_rpt),
               sizeof(l1_inter_freq_meas_rpt_struct_type));

  rrc_free_cmd_buf(cmd_process_status.cmd_ptr);
  if(p_msmtres->meas_results_included == FALSE)
    return FALSE;
  else
    return TRUE;
}

/*===========================================================================

FUNCTION                RRCFF_L1_GET_ADDITIONAL_INTERNAL_MSMT_RESULTS

DESCRIPTION
                        This function send a high priority cmd to L1 to get the 
                        additonal meas of UE-Internal type which can be appended to 
                        meas reporting going in UL.


DEPENDENCIES
                        None

RETURN VALUE
                        TRUE:if additional meas result is present
                        FALSE: if not present
                        

SIDE EFFECTS
                        None

===========================================================================*/

boolean rrcff_l1_get_additional_internal_msmt_results
(
  uint32  meas_id,
  l1_ue_internal_meas_rpt_struct_type  *p_msmtres
)
{
  rrc_ff_wait_type cmd_process_status;
  l1_ext_cmd_type   *l1_cmd_ptr;

  cmd_process_status.keep_waiting = TRUE;
  cmd_process_status.cmd_ptr = NULL;

  l1_cmd_ptr = l1_get_cmd_buf();

  /* Fill in the Command Header, and indicate to do it Now! */
  l1_cmd_ptr->cmd_hdr.cmd_id = CPHY_ADDITIONAL_MEAS_DATA_REQ;

  l1_cmd_ptr->cmd.add_meas_info_req.meas_id =  meas_id;
  l1_cmd_ptr->cmd.add_meas_info_req.meas_type = L1_UE_INTERNAL;

/* Now send the primitive to L1 */
  l1_put_ff_cmd(l1_cmd_ptr);

  rrcff_set_expected_cmd_id(RRC_CPHY_ADDITIONAL_MEAS_DATA_RSP);

  cmd_process_status = rrcff_wait_high_priority_rsp();
  WCDMA_MEMCPY((void *)p_msmtres, 
               sizeof(l1_ue_internal_meas_rpt_struct_type),
               (void *)(&cmd_process_status.cmd_ptr->cmd.add_meas_info_rsp.u.ue_internal_meas_rpt),
               sizeof(l1_ue_internal_meas_rpt_struct_type));

  rrc_free_cmd_buf(cmd_process_status.cmd_ptr);
  if(p_msmtres->cell_measured_results_included == FALSE)
    return FALSE;
  else
    return TRUE;
}

/*===========================================================================

FUNCTION                RRCFF_L1_GET_UTRAN_NMR_INFO

DESCRIPTION             This function is called to get
                        6best intra/inter-freq/inter-rat cells in response to a
                        NMR request from upper layer.


DEPENDENCIES
                        None

RETURN VALUE
                        TRUE: if NMR info is present.
                        FALSE: if not present


SIDE EFFECTS
                        None

===========================================================================*/

boolean rrcff_l1_get_utran_nmr_info
(
  l1_meas_utran_nmr_info_union_type  *l1_meas_utran_nmr_info,
  l1_rrc_utran_nmr_report_type  report_type
)
{
  boolean status;
  rrc_ff_wait_type cmd_process_status;
  l1_ext_cmd_type   *l1_cmd_ptr;

  cmd_process_status.keep_waiting = TRUE;
  cmd_process_status.cmd_ptr = NULL;

  l1_cmd_ptr = l1_get_cmd_buf();

  /* Fill in the Command Header, and indicate to do it Now! */
  l1_cmd_ptr->cmd_hdr.cmd_id = CPHY_NMR_INFO_REQ;

  l1_cmd_ptr->cmd.utran_nmr_info_req.report_type = report_type;

/* Now send the primitive to L1 */
  l1_put_ff_cmd(l1_cmd_ptr);

  rrcff_set_expected_cmd_id(RRC_CPHY_UTRAN_NMR_INFO_CNF);

  cmd_process_status = rrcff_wait_high_priority_rsp();
  WCDMA_MEMCPY((void *)l1_meas_utran_nmr_info, 
               sizeof(l1_meas_utran_nmr_info_union_type),
               (void *)(cmd_process_status.cmd_ptr->cmd.nmr_cnf.l1_meas_utran_nmr_info),
               sizeof(l1_meas_utran_nmr_info_union_type));

  status = cmd_process_status.cmd_ptr->cmd.nmr_cnf.status;
  rrc_free_cmd_buf(cmd_process_status.cmd_ptr);
  return status;
}

/*===========================================================================

FUNCTION                RRCFF_L1_PROCESS_IMMEDIATE_MEAS_REQ

DESCRIPTION             This function sends a req to L1 to get the 
                        intra-freq measurement report to append in RACH msg


DEPENDENCIES
                        None

RETURN VALUE
                        None

SIDE EFFECTS
                        None

===========================================================================*/

void rrcff_l1_process_immediate_meas_req
(
  l1_immediate_meas_type  *immed_meas,
  l1_meas_quan_enum_type  meas_quan
)
{
  rrc_ff_wait_type cmd_process_status;
  l1_ext_cmd_type   *l1_cmd_ptr;

  cmd_process_status.keep_waiting = TRUE;
  cmd_process_status.cmd_ptr = NULL;

  l1_cmd_ptr = l1_get_cmd_buf();

  /* Fill in the Command Header, and indicate to do it Now! */
  l1_cmd_ptr->cmd_hdr.cmd_id = CPHY_GET_IMM_MEAS_REQ;


  l1_cmd_ptr->cmd.imm_meas_req.meas_quan = meas_quan;
  l1_cmd_ptr->cmd.imm_meas_req.meas_type = L1_INTRA_FREQ;
  

/* Now send the primitive to L1 */
  l1_put_ff_cmd(l1_cmd_ptr);

  rrcff_set_expected_cmd_id(RRC_CPHY_GET_IMM_MEAS_RSP);

  cmd_process_status = rrcff_wait_high_priority_rsp();

  WCDMA_MEMCPY((void *)immed_meas, 
               sizeof(l1_immediate_meas_type),
               (void *)(cmd_process_status.cmd_ptr->cmd.imm_meas_cnf.immed_meas),
               sizeof(l1_immediate_meas_type));

  rrc_free_cmd_buf(cmd_process_status.cmd_ptr);
}

/*===========================================================================

FUNCTION                RRCFF_L1_PROCESS_IMMEDIATE_INTERF_MEAS_REQ

DESCRIPTION             This function sends a req to L1 to get the 
                        inter-freq measurement report to append in RACH msg


DEPENDENCIES
                        None

RETURN VALUE
                        None

SIDE EFFECTS
                        None

===========================================================================*/

void rrcff_l1_process_immediate_interf_meas_req( l1_immediate_meas_type *immed_meas)
{
  rrc_ff_wait_type cmd_process_status;
  l1_ext_cmd_type   *l1_cmd_ptr;

  cmd_process_status.keep_waiting = TRUE;
  cmd_process_status.cmd_ptr = NULL;

  l1_cmd_ptr = l1_get_cmd_buf();

  /* Fill in the Command Header, and indicate to do it Now! */
  l1_cmd_ptr->cmd_hdr.cmd_id = CPHY_GET_IMM_MEAS_REQ;


  l1_cmd_ptr->cmd.imm_meas_req.meas_type = L1_INTER_FREQ;
  

/* Now send the primitive to L1 */
  l1_put_ff_cmd(l1_cmd_ptr);

  rrcff_set_expected_cmd_id(RRC_CPHY_GET_IMM_MEAS_RSP);

  cmd_process_status = rrcff_wait_high_priority_rsp();

  WCDMA_MEMCPY((void *)immed_meas, 
               sizeof(l1_immediate_meas_type),
               (void *)(cmd_process_status.cmd_ptr->cmd.imm_meas_cnf.immed_meas),
               sizeof(l1_immediate_meas_type));

  rrc_free_cmd_buf(cmd_process_status.cmd_ptr);
}

#endif /* FEATURE_WCDMA_FREE_FLOATING_TASK */
