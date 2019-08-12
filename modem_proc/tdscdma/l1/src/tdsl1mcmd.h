#ifndef TDSL1MCMD_H
#define TDSL1MCMD_H
/*===========================================================================

              L 1 M    C O M M A N D    H A N D L E R S

GENERAL DESCRIPTION
  This file contains function declarations of the L1 Manager external
  command handlers.
  
EXTERNALIZED FUNCTIONS

tdsl1mcmd_cphy_capability_req_cmd
tdsl1mcmd_cphy_idle_req_cmd
tdsl1mcmd_cphy_acq_req_cmd
tdsl1mcmd_cphy_cell_parm_req_cmd
tdsl1mcmd_cphy_hard_handoff_req_cmd
tdsl1mcmd_bch_schedule_cmd
tdsl1mcmd_cphy_drx_req_cmd
tdsl1mcmd_cphy_ciphering_key_req_cmd
tdsl1mcmd_cphy_integrity_key_req_cmd
tdsl1mcmd_cphy_integrity_key_mauc_req_cmd
tdsl1mcmd_cphy_ul_tx_control_req_cmd
tdsl1mcmd_cphy_action_cancel_req_cmd
tdsl1mcmd_cphy_cell_selection_req_cmd
tdsl1mcmd_cphy_cell_reselection_rsp_cmd
tdsl1mcmd_cphy_immediate_reselection_req_cmd
tdsl1mcmd_cphy_measurement_req_cmd
tdsl1mcmd_process_immediate_meas_req
tdsl1mcmd_cphy_immediate_reselection_req_cmd
tdsl1mcmd_cphy_freq_scan_req_cmd
tdsl1mcmd_cphy_deact_req_cmd
tdsl1mcmd_cphy_stop_req_cmd
tdsl1mcmd_cphy_suspend_req_cmd

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS  
  

  Copyright (c) 2000 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/tdsl1mcmd.h_v   1.16   18 Jul 2002 16:43:50   shawnm  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsl1mcmd.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/14/11   cxd     Remove the following L1 external commands
                   TDSL1_CPHY_CAPABILITY_REQ           =   0x0,
                   TDSL1_CPHY_ASET_UPDATE_REQ          =   0xA,
                   TDSL1_CPHY_HARD_HANDOFF_REQ         =   0xB,
                   TDSL1_CPHY_BCH_SCHEDULE_REQ         =   0xC,
                   TDSL1_CPHY_INTEGRITY_KEY_REQ        =   0x13,
                   TDSL1_CPHY_INTEGRITY_KEY_MAUC_REQ   =   0x14,
                   TDSL1_CPHY_IMMEDIATE_RESELECTION_REQ=   0x15,
                   TDSL1_CPHY_CELL_RESELECTION_RSP     =   0x16,
05/03/10   hk      Fixed compiler warnings
01/19/09   stk     Added support for R7 spec CR High Mobility Indicator( spec CR 2948(RP-060712))
10/03/08   hk      Bringing in 7k Mailine fixes
02/20/06   gv      Declared extern enum tdsl1m_global.suspend_cause to hold the cause for
				   which WL1 is suspended.
02/20/06   gv	   Declared extern boolean tdsl1m_global.suspended_w2g_bplmn
				   which is used to indicate whether WL1 was suspended for 
				   W2G BPLMN.
11/08/05  gv/nd    Added code for WtoW BPLMN feature.
07/06/05   vp      Code changes to support new DL sync parms received in a 
                   TDSL1_CPHY_UPD_DL_SYNC_PARMS message. RRC sends this message
                   when it has received a UTRAN Mobility Information message.
05/04/05   sk/sh   Added T-barred Feature support. Support for new cmd CELL_BAR_REQ
02/27/05   scm     Move BMC related prototypes into tdsdlstates.h.
02/17/05   scm     Support commands from BMC (Broadcast Multicast Control).
12/01/04   scm     Fix compiler warning and comment.
11/16/04   vb      Added code to handle hfn rollover update before and after HHO. 
                   Merged the function l1m_compute_suspend_duration to handle 
                   rollover update after HHO function
09/09/03   m       Modifications to support Inter Freq HHO
03/13/03   scm     Move l1m_compute_suspend_duration() from tdsl1msetup.h to here.
11/06/02   scm     Change tdsl1mcmd_cphy_stop_req_cmd() comment for TDSL1M_DEEP_SLEEP support.
10/07/02   scm     SUSPEND with activation time support.
09/09/02   scm     Change tdsl1mcmd_cphy_suspend_req_cmd() to take l1_ext_cmd_type*
                   instead of tdsl1_suspend_cmd_type* for SUSPEND with act time.
08/30/02   scm     More work on SUSPEND/RESUME.
08/21/02   scm     Add tdsl1mcmd_cphy_mac_ul_config_cnf() and tdsl1mcmd_cphy_mac_dl_config_cnf().
08/07/02   scm     Adding l1m_cphy_mac_config_cnf for MAC activation time support.
07/18/02   scm     Initial pass at SUSPEND/RESUME support, partly functional.
05/18/02   sh      Updated documentation for tdsl1mcmd_cphy_drx_req_cmd().
05/06/02   scm     Change FEATURE_WCDMA_L1_START_STOP to
                   FEATURE_CHEETAH_BASELINE, to match RRC.
05/03/02   scm     Implementing support of START/STOP primitives from RRC.
                   Code wrapped in FEATURE_WCDMA_L1_START_STOP define.
02/12/02   sh      Changed proto for tdsl1mcmd_cphy_action_cancel_req_cmd().
                   Added FEATURE_L1_ACT_TIME flag.
10/19/01   sh      Added tdsl1mcmd_cphy_deact_req_cmd() for L1 deactivate cmd.
08/31/01   sh      Added tdsl1mcmd_cphy_freq_scan_req_cmd() to support freq scan.
08/10/01   sh      Removed l1m_cphy_immediate_meas_req_cmd() and
                   l1m_cphy_aset_update_req_cmd().
03/29/01   sh      Removed tdsl1msetup_cphy_cell_transition_req_cmd().
01/03/01   sh      Removed l1m_cphy_immediate_cell_eval_req_cmd(),
                   l1_cphy_sfn_req(), l1m_cphy_set_sfn_req_cmd().
                   l1m_set_cfn_req_cmd().
12/21/00   sh      Added prototype for function
                   tdsl1mcmd_cphy_ul_tx_control_req_cmd()
11/16/00   sh      Moved tdsl1msetup_cphy_setup_req_cmd() to tdsl1msetup.h.
09/25/00   yus     Created file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tdscdma_variation.h"
#include "tdsl1rrcif.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/******************************************************************************
*******************************************************************************
                       EXTERNAL L1M COMMAND HANDLERS
*******************************************************************************
******************************************************************************/
/*===========================================================================
FUNCTION        tdsl1mcmd_cphy_idle_req_cmd

FILE NAME       

DESCRIPTION     This function handles the TDSL1_CPHY_IDLE_REQ primitive

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
extern void tdsl1mcmd_cphy_idle_req_cmd
(
  /* command parameters struct pointer */
  tdsl1_idle_cmd_type *cmd
);

/*===========================================================================
FUNCTION        tdsl1mcmd_cphy_idle_req_cmd_with_timing

FILE NAME       

DESCRIPTION     This function handles the TDSL1_CPHY_IDLE_REQ primitive when 
                L1 has system timing

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
extern void tdsl1mcmd_cphy_idle_req_cmd_with_timing(void);


/*===========================================================================
FUNCTION        tdsl1mcmd_cphy_deact_req_cmd

FILE NAME

DESCRIPTION     This function handles the TDSL1_CPHY_DEACTIVATE_REQ primitive

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void tdsl1mcmd_cphy_deact_req_cmd
(
  /* command parameters struct pointer */
  tdsl1_deactivate_cmd_type *cmd
);

/*===========================================================================
FUNCTION        tdsl1mcmd_cphy_stop_req_cmd

DESCRIPTION
  This function handles the TDSL1_CPHY_STOP_TDSCDMA_MODE_REQ primitive from RRC.
  If WCDMA is no longer the active RAT, L1 goes to TDSL1M_STOPPED state.  If
  service has not been found for some period of time, L1 goes to TDSL1M_DEEP_SLEEP
  state.  In both cases WCDMA L1 ceases all meaningful processing.  However
  in TDSL1M_DEEP_SLEEP the hardware is also configured for power conservation mode.
  For both cases a START command causes initialization and a return to IDLE.

PARAMETERS
  cmd->cause - TDSL1_RAT_CHANGE: Means WCDMA no longer the active RAT.
             \ TDSL1_DEEP_SLEEP: Means no service.  Enter low power mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Changes state to TDSL1M_STOPPED or TDSL1M_DEEP_SLEEP.
===========================================================================*/
extern void tdsl1mcmd_cphy_stop_req_cmd
(
  /* command parameters struct pointer */
  tdsl1_stop_cmd_type *cmd
);

/*===========================================================================
FUNCTION        tdsl1mcmd_handle_hfn_rollover_before_hho

FILE NAME

DESCRIPTION     This function update the current CFN to mac for 
                hfn computation and it store the current sfn, which 
                will be used for computation of duration of th 
                suspension state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsl1mcmd_handle_hfn_rollover_before_hho(void);

/*===========================================================================
FUNCTION        tdsl1mcmd_handle_hfn_rollover_after_hho

FILE NAME

DESCRIPTION     This function processes suspension duration
                and send the rollover hfn to mac if necessary by 
                looking at the input argument.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsl1mcmd_handle_hfn_rollover_after_hho(boolean send_rollover_to_mac);

/*===========================================================================
FUNCTION        tdsl1mcmd_cphy_suspend_req_cmd

DESCRIPTION
  We got the TDSL1_CPHY_SUSPEND_TDSCDMA_MODE_REQ primitive from RRC, meaning we're
  attempting an inter-RAT handover or cell reselection.  Check if it has an
  activation time.  If so, schedule it for later.  If not, suspend now.

PARAMETERS
  tdsl1_ext_cmd_type *cmd_ptr - Pointer to RRC SUSPEND command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  May change state to TDSL1M_SUSPENDED and shut down WCDMA L1, if the
  activation time is now.
===========================================================================*/
extern void tdsl1mcmd_cphy_suspend_req_cmd
(
  /* command parameters struct pointer */
  tdsl1_ext_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION        tdsl1mcmd_cphy_suspend_req_time_adjust

FILE NAME       

DESCRIPTION     If RRC sends SUSPEND_CMD too late(near the frame boundary), L1M would delay SUSPEND processing to next frame, to avoid FW receiving DL CMD after TS6.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
extern void tdsl1mcmd_cphy_suspend_req_time_adjust(void);
/*===========================================================================
FUNCTION        tdsl1mcmd_cphy_trigger_suspend_req_process

FILE NAME       

DESCRIPTION     send a local CMD to trigger SUSPEND_CMD processing

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
extern void tdsl1mcmd_cphy_trigger_suspend_req_process(uint16 subfn, uint32 cfg_db_idx);

/*===========================================================================
FUNCTION        tdsl1mcmd_cphy_acq_req_cmd

FILE NAME       

DESCRIPTION     This function handles the TDSL1_CPHY_ACQ_REQ primitive

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
extern void tdsl1mcmd_cphy_acq_req_cmd
(
  /* command parameters struct pointer */
  // sq_TBD
  tdsl1_acq_cmd_type *cmd
);


/*===========================================================================
FUNCTION        tdsl1mcmd_cphy_acq_req_with_trm_wrapper

FILE NAME       

DESCRIPTION     This function handles the TDSL1_CPHY_ACQ_REQ primitive 
                and try to lock antenna resource before doing real ACQ 

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
extern void tdsl1mcmd_cphy_acq_req_with_trm_wrapper( tdsl1_ext_cmd_type* cmd_ptr );

/*===========================================================================
FUNCTION        tdsl1mcmd_cphy_freq_scan_req_cmd

FILE NAME       

DESCRIPTION     This function handles the TDSL1_CPHY_FREQ_SCAN_REQ primitive.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
extern void tdsl1mcmd_cphy_freq_scan_req_cmd
(
  /* command parameters struct pointer */
  tdsl1_freq_scan_cmd_type *cmd
);

/*===========================================================================
FUNCTION        tdsl1mcmd_cphy_freq_scan_req_with_trm_wrapper

FILE NAME       

DESCRIPTION     This function handles the TDSL1_CPHY_FREQ_SCAN_REQ primitive 
                and try to lock antenna resource before doing real FS.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
extern void tdsl1mcmd_cphy_freq_scan_req_with_trm_wrapper(tdsl1_ext_cmd_type* cmd_ptr);

extern void tdsl1mcmd_cphy_upd_ota_parm_cmd
(
  /*command parameters struct pointer */
  tdsl1_upd_ota_parms_ind_cmd_type *cmd
);
/*===========================================================================
FUNCTION        tdsl1mcmd_cphy_cell_parm_req_cmd

FILE NAME       

DESCRIPTION     This function handles the TDSL1_CPHY_CELL_PARM_REQ primitive

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
extern void tdsl1mcmd_cphy_cell_parm_req_cmd
(
  /* command parameters struct pointer */
  tdsl1_cell_parm_cmd_type *cmd
);    

/*===========================================================================
FUNCTION        tdsl1mcmd_cphy_drx_req_cmd

DESCRIPTION
  This function handles the TDSL1_CPHY_DRX_REQ primitive. This function will
  first send a command to searcher to search SYNC_NSET and ASYNC_NSET
  neighbor cells and sort them. Once the searcher reports search/sort
  done, L1M will proceed to send the DRX command to the DRX module.

DEPENDENCIES    
  None
 
RETURN VALUE    
  None
 
SIDE EFFECTS    
  Search heartbeat is turned off until the nset srch/sort is done.
	DRX Command is saved into l1_pending_drx_cmd.
===========================================================================*/
extern void tdsl1mcmd_cphy_drx_req_cmd
(
  /* command parameters struct pointer */
  tdsl1_drx_cmd_type *cmd
);


/*===========================================================================
FUNCTION        tdsl1mcmd_cphy_ciphering_key_req_cmd

FILE NAME       

DESCRIPTION     This function handles the TDSL1_CPHY_CIPHERING_KEY_REQ primitive.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
void tdsl1mcmd_cphy_ciphering_key_req_cmd
(
  tdsl1_ciphering_key_cmd_type *cmd
);

/*===========================================================================
FUNCTION        tdsl1mcmd_cphy_measurement_req_cmd

FILE NAME       

DESCRIPTION     This function handles the TDSL1_CPHY_MEASUREMENT_REQ primitive

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
extern void tdsl1mcmd_cphy_measurement_req_cmd
(
  /* command parameters struct pointer */
  tdsl1_meas_cmd_type *cmd
);

/*===========================================================================
FUNCTION        tdsl1mcmd_cphy_cell_selection_req_cmd

FILE NAME       

DESCRIPTION     This function handles the TDSL1_CPHY_CELL_SELECTION_REQ primitive

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
extern void tdsl1mcmd_cphy_cell_selection_req_cmd
(
  /* command parameters struct pointer */
  tdsl1_cell_selec_cmd_type *cmd
);

/*===========================================================================
FUNCTION        tdsl1mcmd_cphy_action_cancel_req_cmd

FILE NAME       

DESCRIPTION     This function handles the CPHY_ACTION_CANCEL_REQ primitive

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
extern void tdsl1mcmd_cphy_act_time_cancel_req_cmd
(
  tdsl1_act_time_cancel_cmd_type *cmd
);

/*=========================================================================

FUNCTION tdsl1mcmd_cphy_mac_ul_config_cnf

DESCRIPTION
  This function handles the TDSL1_CPHY_MAC_UL_CONFIG_CNF primitive from MAC,
  providing activation time support to MAC and RRC.  L1 signals MAC at the
  activation time to process a UL config request from RRC.  Once MAC finishes,
  it responds with the TDSL1_CPHY_MAC_UL_CONFIG_CNF primitive, which includes a
  completion status.  L1 then sends the TDSRRC_CPHY_SETUP_CNF primitive to
  RRC, which includes a combined L1 and MAC config status, if both UL and
  DL configurations are complete.

DEPENDENCIES
  None.

PARAMETERS
  l1_mac_config_cnf_type *cmd - Structure containing only the boolean
    "mac_config_status".  TRUE means success.  FALSE means failure.

RETURN VALUE
  None.

SIDE EFFECTS
  Possibly sends TDSRRC_CPHY_SETUP_CNF primitive to RRC.

=========================================================================*/
extern void tdsl1mcmd_cphy_mac_ul_config_cnf
(
  tdsl1_mac_config_cnf_type *cmd
);

/*=========================================================================

FUNCTION tdsl1mcmd_cphy_mac_dl_config_cnf

DESCRIPTION
  This function handles the TDSL1_CPHY_MAC_DL_CONFIG_CNF primitive from MAC,
  providing activation time support to MAC and RRC.  L1 signals MAC at the
  activation time to process a DL config request from RRC.  Once MAC finishes,
  it responds with the TDSL1_CPHY_MAC_DL_CONFIG_CNF primitive, which includes a
  completion status.  L1 then sends the TDSRRC_CPHY_SETUP_CNF primitive to
  RRC, which includes a combined L1 and MAC config status, if both UL and
  DL configurations are complete.

DEPENDENCIES
  None.

PARAMETERS
  l1_mac_config_cnf_type *cmd - Structure containing only the boolean
    "mac_config_status".  TRUE means success.  FALSE means failure.

RETURN VALUE
  None.

SIDE EFFECTS
  Possibly sends TDSRRC_CPHY_SETUP_CNF primitive to RRC.

=========================================================================*/
extern void tdsl1mcmd_cphy_mac_dl_config_cnf
(
  tdsl1_mac_config_cnf_type *cmd
);


/*==============================================================================
FUNCTION        tdsl1mcmd_suspend

DESCRIPTION
  Unconditionally suspends L1

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  May change state to TDSL1M_SUSPENDED
==============================================================================*/
void tdsl1mcmd_suspend(void);

/*==============================================================================
FUNCTION        tdsl1mcmd_stop

DESCRIPTION
  Unconditionally stop L1

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  May change state to TDSL1M_STOP
==============================================================================*/
void tdsl1mcmd_stop(void);

/*==============================================================================
FUNCTION        tdsl1mcmd_cphy_cell_bar_req_cmd

DESCRIPTION
 Handler for CELL_BAR_REQ from RRC

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
==============================================================================*/
extern void tdsl1mcmd_cphy_cell_bar_req_cmd 
(
 tdsl1_cell_bar_cmd_type * cmd
); 

/*===========================================================================
FUNCTION        tdsl1mcmd_cphy_upd_dl_sync_parms_cmd

FILE NAME        l1mcmd.c

DESCRIPTION     This function is the processor for message TDSL1_CPHY_UPD_DL_SYNC_PARMS.
               
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsl1mcmd_cphy_upd_dl_sync_parms_cmd(tdsl1_upd_dl_sync_parms_cmd_type *cmd);


/*===========================================================================
FUNCTION     tdsl1mcmd_bplmn_cphy_setup_req_cmd

DESCRIPTION
  THis function handles the TDSL1_CPHY_SETUP_REQ from RRC when the L1 is in 
  PCH_BPLMN state. This set up request from RRC triggers the L1 to set up 
  a neighbor PCCPCH. RRC currently sends L1 a set up request for S_PCCPCH
  and this command handler internally converts it into a PCCPCH_N.


DEPENDENCIES handle pccpch setup req in BPLMN
  None.

PARAMETERS
  None.

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdsl1mcmd_bplmn_cphy_setup_req_cmd(tdsl1_ext_cmd_type* cmd_ptr);


/*===========================================================================
FUNCTION        tdsl1mcmd_measurement_is_for_internal_meas

FILE NAME       

DESCRIPTION     This function determines if the TDSL1_CPHY_MEASUREMENT_REQ received
                is for internal measurements or not

DEPENDENCIES    

RETURN VALUE    
             boolean
SIDE EFFECTS    
===========================================================================*/
extern boolean tdsl1mcmd_measurement_is_for_internal_meas(tdsl1_meas_cmd_type *cmd);

#ifdef FEATURE_TDSCDMA_QUALITY_MEAS
/*===========================================================================
FUNCTION        tdsl1mcmd_measurement_is_for_quality_meas

FILE NAME       

DESCRIPTION     This function determines if the TDSL1_CPHY_MEASUREMENT_REQ received
                is for quality measurements or not

DEPENDENCIES    

RETURN VALUE    
             boolean
SIDE EFFECTS    
===========================================================================*/
extern boolean tdsl1mcmd_measurement_is_for_quality_meas(tdsl1_meas_cmd_type *cmd);
#endif /* FEATURE_TDSCDMA_QUALITY_MEAS */
#endif /* TDSL1MCMD_H */
