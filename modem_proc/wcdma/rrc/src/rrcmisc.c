/*===========================================================================
                  R R C  M I S C  P R O C E D U R E 

DESCRIPTION
  This header file contains function prototypes for the RRC Misc Procedure.
  The RRC Misc Procedure is created for implementation purpose to handle
  processing of unknown or unsupported message type. It also process the 
  RRC_SIM_UPDATE_REQ and RRC_SIM_INSERTED_REQ commands from MM and updates
  suitable RRC Data.


EXTERNALIZED FUNCTIONS

  rrcmisc_init_procedure
    This function initializes everything that the RRC Misc procedure
    needs to have initialized.
  
  rrcmisc_procedure_event_handler

    This procedure is the event handler for the RRC Misc procedure.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2001-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcmisc.c_v   1.4   01 Oct 2001 18:02:20   rjeenaga  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcmisc.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/12/16   sn      Changes for FR30881
05/21/15   bc      Changes to read SIBs across DRX for CGI scan.
01/21/15   sa      Made changes for QTF with CRM
11/10/14   sp      Made changes to save procedure substates in crash debug info
10/27/14   sr      Changes to clear the barred list on receiving SIM inserted request and 
                   when high access class is present
10/15/14   vg      Made chnages to convert Big heap variables to Globals to benefit from  qzip compression gain
09/05/14   as      Made changes to log events for inter freq HHO status
07/15/14   sg      Made changes to remove allocation of dec_ctxt_list from init_dynamic_ptrs for optimization
07/23/14   sp      Made changes to remove F3s as part of FR19999
07/17/14   sr      Changes for NV refresh FR
06/17/14   sp      Made changes to mainline RLC-restablihment redesign changes
06/02/14   sp      Made changes for FR19997 - UL rlc re-establishment redesign
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
03/28/14   gv      Made changes to update RSSI only when it is valid
24/01/13   sg      Made changes to log CU and UU status as external event
01/09/14   sp      Removed code under FEATURE_CS_VOICE_OVER_HSPA feature
01/03/14   bc      Made Changes to read SIBs across DRX cycles.
12/31/13   sg      Made changes to optimize dec_ctxt_list
11/27/13   sp      Made changes to add nv support and to save time ticks for 
                   rr to/from rrc mm to/from rrc and to add msg_type for crash debug ota changes
10/27/13   as      Made changes to backup OTAs for crash debugging
08/13/13   gv      Added support for FEATURE_TRIPLE_SIM
06/05/13   as      Made changes to correct freeing of dec_ctxt_list
05/20/13   sn      Changes to consider W CSG feature as disabled if there is 
                   no whitelist update from NAS
05/17/13   db      Added code to free memory of rrc_ordered_hanging_rb and rrc_current_hanging_rb
                   pointers
05/13/13   sg      Made changes to optimize dec_ctxt_list
05/08/13   db      Multiple mapping support for EFACH
04/24/13   sn      WRRC side of changes to meet CSG ADAPT TCs
04/05/13   sg      Made changes to revert the CR 458942 dec_ctxt_list optimization
04/01/13   sg      Made changes to optimize dec_ctxt_list
02/04/13   ad      Made changes to remove NULL check for rrc_get_int_cmd_buf function
01/03/13   ad      Made changes to reduce the redundant F3s
12/25/12   db      Made changes to requeue transition to disconnected triggered by CU when there is no response
12/07/12   sn      CSG feature changes
10/19/12   sg      Made changes for Rel 9 migration, removing unsupported IEs and ZI reductions. 
06/20/11   kp      Added code to trigger close_session_req for HFN 
                   mismatch recovery.
10/04/12   md      Made changes to detect RLC size change during state transitions via CELL PCH state.
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
05/24/12   sks     Added support for QCHAT specific dynamic DRX.
04/10/12   gv      Made changes to remove unused heap allocation and related unused code
12/16/11   gv      Modified the function name under free floating of tasks
12/12/11   gv      Added code to support free floating of tasks
24/11/11   md      Corrected 'tmsi_present’ element reference while processing 
                   SIM_UPDATE_REQ 
11/3/11    db      Mainlined T_MDM9X00B feature to resolve futex exhaustion problem 
08/25/11   gv      Added code to support FEATURE_UIM_DISABLE_GSDI_INTERFACE
05/03/11   rl      Fixed the lint error
03/03/11   su      Added code to call rrc_sib_proc_events_init in order
                   to free the tempholder pointer before freeing 
                   sib_events_ptr. 
02/15/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.91.50
02/09/11   su      Added code to free the gtow_info.sib16_ptr before calling 
                   mem_deinit_heap during G2W HO failure to avoid Error Fatal.
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
1/22/11    sk      Moved mem deinit before heap free.
01/21/11   su      Made changes to call mem_deinit_heap to 
                   avoid Futex exhaustion.
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/11/10   mrg     MAC DL HS ZI memory optimization changes
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
06/29/10   rm      Made changes for ZI memory optimization
06/28/10   mrg     MAC-R99 ZI memory reduction changes
05/13/10   ss      Fixed compiler warnings.
05/07/10   dm      Fixed Lint warnings
03/29/10   dm      Made changes to not do ERROR FATAL if dynamic pointers are NULL
                   when freeing them.
03/15/10   gkg     Made changes to put changes for "Multiple DRX Cycles" and 
                   "CU wait Timer" under feature R7. 
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
02/01/10   dm      Added code for handling Multiple DRX.
01/22/10   rm      Modified the usage of qchatupk_wcdma.h with rrc_qchatupk.h
                   (at the request of QCHAT team)
01/20/10   gkg     Made changes to correct the unit of CU wait timer.
12/21/09   gkg     Made changes for CSoHSPA under feature FEATURE_CS_VOICE_OVER_HSPA.
12/01/09   gkg     RRC changes required to integrate QChat Porting Kit Optimizations 
                   into AMSS. The changes are under feature FEATURE_QCHAT.
10/15/09   kp      Added support to re-read all the SIB's when UE come back from LPM.
09/24/09   ps      Made changes to stop CU timers if  UE is going to
                   disconnected state. This will avoid cross over scenarios.
06/22/09   kp      Added support to allocate memory using calloc for dynamically allocate 
                   cc/oc/toc/sib_event ptr.
06/08/09   dm      Added support for freeing 100k sib heap when WCDMA stack is 
                   inactive as part of ZI reduction activity. code changes are under
                   FEATURE_WCDMA_RRC_ZI_OPT_PHASE_2.
06/05/09   ps      Made changes to change MODEM_MEM_CL_WCDMA….to 
                   MODEM_MEM_CLIENT_WCDMA_.. according to memory team requirement
06/05/09   ps      Made changes to not start the T305 timer if value is infinity
05/18/09   ps      Changed MODEM_MEM_CL_WCDMA_RRC_GLOBAL_CRITICAL
                     to MODEM_MEM_CL_WCDMA_RRC_GLOBAL_CRIT
05/13/09   kp      Fixed compiler warning.
05/11/09   ps      Feature to dynamically allocate cc/oc/toc/sib_event ptr
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
04/08/09   gkg     In case of Cell_FACH->Cell_DCH transition through Reconfig OTA, 
                   if RB-Mapping for any RB is provided again, TOC will be not be 
                   updated for RB-Mapping.
04/09/09   ps      Made changes to send reconfig ind in CU/or go to idle
                   if RL failure is triggered while l2 ack for reconfig complete
01/19/08   ps      Made changes for ASn1 migration 25.331v7.9  
                   i.e.May 2008 version  (2008-05) 
06/13/08   gkg     Added code for notification of Cell_Id change through OTAs
                   in Cell_DCH state. 
11/21/07   rc/vm   Added support for FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY feature.
09/05/06   vm      Updated the existing function rrc_transition_to_disconnected_state()
                   as it now takes 1 additional argument.
                   Populated the new field tx_to_discon_reason in the command
                   initiate_cell_sel_ind with a default value.                   
05/08/06   segu    Added support for RSSI optimazation
11/09/05   kc      Added fn. to support taking UE to idle through CSP.
11/04/05  da/vr    Made changes to detect UE inactivity and take appropriate actions.
08/17/05   vm      Added support for not barring a cell if SIM has not been
                   read (if SIM is present).
                   Added new local global variable rrc_rcvd_sim_ins_req and
                   definition of new function rrcmisc_rrc_rcvd_sim_insert_req().
10/11/04   sgk     Removed RRCMISC_INVALID_RCVD_MSG_TYPE as it was unused.
08/30/04   sgk     Used rrc_ReceivedMessageType_spare1 instead of 
                   RRCMISC_INVALID_RCVD_MSG_TYPE in function 
                   rrcsend_send_rrc_status_message called in function 
                   rrcmisc_process_downlink_msg to fix lint error
07/15/04   sk      Make ptmsi or tmsi presence as invalid if they are not
                   present in SIM_UPDATE_REQ as they are always read from SIM
08/08/03   kc      Added support to process SIM_UPDATE_REQ in CONNECTING_STATE.
02/28/03   bu      Initialize the lo and hi access classes to invalid values in
                   rrcmisc_init_procedure.
20/01/03   sk      Fixed the following errors reported by lint.
                   Removed the 'void'casting.
                   Corrected the indentations.
10/24/02   rj      Deleted tmc.h from include files.
08/06/02   rj      Added code to process RRC_SIM_INSERTED_REQ and 
                   RRC_SIM_UPDATE_REQ commands and removed the processing of
                   RRC_PAGING_CONTROL_REQ.
10/01/01   rj      Removed nested comments.
09/13/01   rj      Implemented prcessing of RRC_PAGING_CONTROL_REQ command
                   from the MM. Updated heading to reflect paging control
                   request processing.
09/03/01   rj      Removed include file rrcasnutil.h since it is no longer 
                   required after moving send_rrc_status_message to SEND CHAIN.
08/30/01   rj      Renamed rrc_send_rrc_status_message to
                   rrcsend_send_rrc_status_message. Changed 
                   UE_MODE_ACKNOWLEDGED_DATA_PLANE to UE_MODE_ACKNOWLEDGED_DATA.
05/16/01   rj      Created file

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"

#include "rrclcm.h"             /* Logical Channel Manager */
#include "rrcsend.h"              /* For sending a signalling message */
#include "msg.h"
#include "err.h"
#include "rrcasn1.h"
#include "rrcscmgr.h"
#include "rrcsmc.h"
#include "rrcdata_v.h"
#include "rrcllcoc.h"
#include "rrcmisc.h"
#include "rrctmr.h"
#ifdef FEATURE_INACTIVITY_HANDLING
#include "rrcdt.h"
#include "rrcrcr.h"
#include "sm.h"
#include "mm_umts.h"
#include "rlcrrcif_v.h"
#endif /* FEATURE_INTACTIVITY_HANDLING */
#include "rrccu.h"
#include "rrccmd_v.h"
#include "rrcrcr.h"
#include "rrcrbcommon.h"
#include "rrcsibproci.h"
#include "rrcsibdbi.h"
#include "rrcsibproc.h"
#include "rrccui.h"
#include "rrcsibdb.h"

#ifdef FEATURE_QCHAT
#include "rrc_qchatupk.h"
#endif /*FEATURE_QCHAT*/

#include "rrcmeas.h"
#include "rrciho.h"

#include "sys.h"
#include "rrccspi.h"
#include "rrcmcm.h"
#ifdef FEATURE_FEMTO_CSG
#include "rrccsp.h"
#endif
#include "rrcasn1util.h"
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

#define RRCMISC_INVALID_TRANSACTION_ID 0xFF
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */
#ifdef FEATURE_INACTIVITY_HANDLING
rlc_data_activity_type rlc_data_activity_ind;
#endif /* FEATURE_INTACTIVITY_HANDLING */
extern boolean rrc_log_inter_freq_hho_started;
boolean rrc_high_mobility_ind_in_rcr = FALSE;

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
LOCAL boolean rrc_rcvd_sim_ins_req = FALSE;

/* Stores the current state of MISC for Multiple DRX */
rrcmisc_substate_e_type  rrcmisc_multiple_drx_substate;

extern boolean    nv_gcf_test_flag;

#ifdef FEATURE_WCDMA_RRC_HEAP_TO_BSS

ordered_config_type             ordered_config_glob_var;
ordered_config_type             current_config_glob_var;
ordered_config_type             TOC_glob_var;
sib_active_event_database_type  sib_events_glob_var;

#ifdef FEATURE_WCDMA_HS_FACH
rb_mapping_info_type_           rrc_ordered_hanging_rb_glob_var[MAX_HANGING_RB_MAPPING_INFO];
rb_mapping_info_type_           rrc_current_hanging_rb_glob_var[MAX_HANGING_RB_MAPPING_INFO];
#endif /* FEATURE_WCDMA_HS_FACH */

rb_mapping_info_type            rrc_ordered_hanging_rb_mapping_glob_var[MAX_HANGING_RB_MAPPING_INFO];
rb_mapping_info_type            rrc_current_hanging_rb_mapping_glob_var[MAX_HANGING_RB_MAPPING_INFO];

#endif/* FEATURE_WCDMA_RRC_HEAP_TO_BSS */

#ifdef FEATURE_WCDMA_HS_FACH
extern rb_mapping_info_type_ *rrc_ordered_hanging_rb;
extern rb_mapping_info_type_ *rrc_ordered_hanging_rb;
#endif

extern rb_mapping_info_type *rrc_ordered_hanging_rb_mapping;
extern rb_mapping_info_type *rrc_current_hanging_rb_mapping;
extern rrcasn1util_obj_ctxt_list_type *dec_ctxt_list;
/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */
#ifdef FEATURE_WCDMA_REL7_SPECRS 

/***************RRC CO UEC ************************************/
#define N_COUEC_TIMER 2

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* This variable holds internal data for Change of UE Capability Procedure*/

rrccouec_int_data_type rrccouec;

/*This global indicates whether Serving RNC supports IE "supportForChangeOfUE-Capability" or not*/
boolean rrc_rnc_cap_change_support = FALSE;

/*This variable stores the IE "supportForChangeOfUE-Capability" temporarily until procedure is over
    successfully*/
rrc_rnc_cap_change_support_e_type rrc_ordered_rnc_cap_change_support = RNC_CAP_CHANGE_NONE;

/*This structure stores the UE capabilities throughout the RRC Conn Setup.*/
rrc_ue_cap_update_during_rrc_conn_type rrc_ue_cap_during_rrc_conn;

#endif /*FEATURE_WCDMA_REL7_SPECRS*/

boolean rrc_clear_sibdb_for_lpm = FALSE;

/* QCHAT specific DRX coeff which UE should use after a successful LAU/RAU */
sys_drx_cn_coefficient_s1_e_type  rrc_qchat_drx_coeff;

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */


/* =======================================================================
**                            Function Definitions
** ======================================================================= */
#ifdef FEATURE_DUAL_SIM
extern byte reg_sim_per_subs_read_ens_flag(sys_modem_as_id_e_type as_id);
#else
extern byte reg_sim_read_ens_flag( void);
#endif
/*===========================================================================

FUNCTION RRC_TRANSITION_TO_DISCONNECTED_STATE

DESCRIPTION

  This function is used to send initiate cell selection command to CSP
  with destination state as disconnected. CSP on receiving this command
  releases all channels and takes UE to idle 
  
DEPENDENCIES

  None

RETURN VALUE

  SUCCESS:   command successfully sent
  ERR-FATAL: command could not be sent

SIDE EFFECTS

  None

===========================================================================*/
void rrc_transition_to_disconnected_state
(    
  rrc_proc_e_type rrc_proc,
  rrc_tx_to_discon_reason_e_type tx_to_discon_reason
)
{
  rrc_cmd_type *int_cmd_ptr;  /* Pointer RRC Command */
  
  rrccu_log_cu_status(RRC_CU_UU_STATUS_FAILURE, TRUE);

  /* Allocates the buffer to RRC Internal command */
  int_cmd_ptr = rrc_get_int_cmd_buf();
  int_cmd_ptr->cmd_hdr.cmd_id                              = RRC_INITIATE_CELL_SELECTION_IND;
  int_cmd_ptr->cmd.initiate_cell_sel_ind.cause             = RRC_TRANSITION_TO_DISCONNECTED;
  int_cmd_ptr->cmd.initiate_cell_sel_ind.proc              = rrc_proc;
  int_cmd_ptr->cmd.initiate_cell_sel_ind.dest_freq_present = FALSE;
  int_cmd_ptr->cmd.initiate_cell_sel_ind.dest_psc_present  = FALSE;
  int_cmd_ptr->cmd.initiate_cell_sel_ind.dest_state        = RRC_STATE_DISCONNECTED;

  WRRC_MSG1_HIGH("tx to discon reason %d", tx_to_discon_reason);
  int_cmd_ptr->cmd.initiate_cell_sel_ind.tx_to_discon_reason = tx_to_discon_reason;

  rrc_put_int_cmd(int_cmd_ptr); 
  /*Stop CU timers to avoid cross over scenarios while going to disconnected */
  rrccu_stop_cu_timers();
  
  if(rrc_proc == RRC_PROCEDURE_CU)
  {
    WRRC_MSG1_HIGH("RRC_DEBUG:Starting %dms timer to retry incase CSP ignores, incase CSP ignores trans to disconn",RRCCU_TRANS_TO_DISON_TIMER);
    rrctmr_start_timer(RRCTMR_RRCCU_TRANS_TO_DISCON_TIMER,RRCCU_TRANS_TO_DISON_TIMER);
  }
  rrc_log_inter_freq_hho_started = FALSE;
} /* rrcdt_transition_to_disconnected_state */

/*===========================================================================

FUNCTION rrcmisc_process_downlink_msg  

DESCRIPTION
  This function checks whether RRC_DOWNLINK_SDU_IND is for DCCH logical 
  channel. If the message is received on DCCH logical channel, and message type
  unknown or unsupported, then RRC sends RRC STATUS message with error cause 
  "Message Type Invalid or Unsupported".

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/

static void rrcmisc_process_downlink_msg
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to the RRC Command */
)
{

  rlc_lc_id_type lc_id;                      /* Local variable to store 
                                                RLC logical channel Id */
   
  /* Assert the receieved message is of type rrc_DL_CCCH_Message */
  if(RRC_DOWNLINK_SDU_IND == cmd_ptr->cmd_hdr.cmd_id)
  {
    switch(cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type)
    {
      case rrc_DL_DCCH_Message_PDU:

        lc_id = rrclcm_check_ul_rlc_lc_id(    
                UE_LOGCHAN_DCCH,          /* Uplink Logical Channel Type. */
                DCCH_AM_RADIO_BEARER_ID,  /* Radio Bearer Id assoicated with
                                            the above Downlink Logical 
                                            Channel type. */
                UE_MODE_ACKNOWLEDGED_DATA);
                                          /* Mode of transmission for the RLC 
                                             service entity. */
        if(RRCLCM_RLC_LC_ID_NOT_FOUND == lc_id)
        {
          WRRC_MSG0_HIGH("Can't find RLC ID for UL RB 2");
        } 

        /* Set a dummy received Message type to remove compile warning.
         * There are only 0 through 30 valid received message types 
         */
        else /* Valid RLC ID found */
        {
          if(SUCCESS != rrcsend_send_rrc_status_message(
            T_rrc_ProtocolErrorMoreInformation_type1_messageTypeNonexistent,
                                           /* The protocol error cause to be placed in
                                           /  the message */
             RRCMISC_INVALID_TRANSACTION_ID,
                                           /* Received trasactiopn Id */
             rrc_ReceivedMessageType_spare1,
                                           /* Received message identification. These
                                              parameters will not be used to build
                                              RRC STATUS message */
             RRC_PROCEDURE_NONE,           /* RRC procedure */
             lc_id,                        /* Logical Channel Id */
             RRCSEND_L2ACK_NOT_REQUIRED  ) ) 
                                           /* Indicates whether L2 Ack 
                                              required or not */
          {
            WRRC_MSG0_HIGH("Failed to transmit RRC STATUS message");
          } 
        } /* Valid RLC ID found */
        break;

      case rrc_DL_CCCH_Message_PDU:
      case rrc_PCCH_Message_PDU:
      case rrc_BCCH_FACH_Message_PDU:
      case rrc_BCCH_BCH_Message_PDU:
        /* Do nothing. Ignore the message */
        break;

      case rrc_DL_SHCCH_Message_PDU:
        WRRC_MSG0_ERROR("Unsupoorted SDU type");
        break;

      default:
        /* This should never happen */
        ERR_FATAL("Invalid SDU type is forwarded to MISC Proc: %d",
                   cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type,0,0);
        break;
    }     
 
  }
  else
  {
    WRRC_MSG1_HIGH("Invalid Event: %d is forwarded to MISC Procedure",
              cmd_ptr->cmd_hdr.cmd_id);
  }
  
} /* rrcmisc_process_downlink_msg */

#ifdef FEATURE_FEMTO_CSG
/*===========================================================================

FUNCTION rrcmisc_process_white_list_ind  

DESCRIPTION
  This function process the RRC_CSG_WHITE_LIST_UPDATE_IND that received from MM and 
  updates internal white list database

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/

static void rrcmisc_process_white_list_ind
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to the RRC Command */
)
{

  uint32 i=0;
  rrc_cmd_type *cmd;
  rrc_state_e_type  rrc_state = rrc_get_state();

  /* Make sure white list length received in command is not greater than MAX_NO_CSG_LIST */
  is_whitelist_valid = TRUE;
  if(cmd_ptr->cmd.white_list.length > MAX_NO_CSG_LIST)
  {
    WRRC_MSG0_HIGH("CSG: white list length from NAS is greater than expected. Discard");
    return;
  }

  rrc_csg_white_list.length = cmd_ptr->cmd.white_list.length;

  for(i=0; i<rrc_csg_white_list.length; i++)
  {
    rrc_csg_white_list.csg_list[i] = cmd_ptr->cmd.white_list.csg_list[i];
  }

  /*Send unbar request upon whitelist update indication*/
  WRRC_MSG0_HIGH("CSG: Send unbar request upon whitelist update indication");
  rrc_csp_int_data.bar_non_white_csg = TRUE;
  rrccsp_send_cphy_cell_bar_req(RRC_CSP_WCDMA_FREQ_BARRED,0);

  /* Send ASF timer expiry ind immediately so that ASF search can be triggered immediately */
  if(((rrc_state == RRC_STATE_DISCONNECTED) ||
     (rrc_state == RRC_STATE_CELL_PCH) ||
     (rrc_state == RRC_STATE_URA_PCH))
     && (!rrc_csp_int_data.csg_asf_srch_in_progress))
  {
  if( (cmd =  rrc_get_int_cmd_buf()) != NULL)
  {
    WRRC_MSG0_HIGH("CSG: Trigger ASF on whitelist update indication");
    cmd->cmd_hdr.cmd_id = RRC_CSG_ASF_SRCH_TIMER_EXPIRY_IND;
    rrc_put_int_cmd(cmd);

      rrctmr_stop_timer(RRCTMR_CSG_ASF_SRCH_TIMER);
    }
  }
} /* rrcmisc_process_white_list_ind */
#endif

/*===========================================================================

FUNCTION rrcmisc_process_sim_inserted_req  

DESCRIPTION
  This function process the RRC_SIM_INSERTED_REQ that received from MM and 
  updates IMSI, TMSI, PTMSI and Access Class varibales in the RRC DATA module
  and sets enable/ disable these indenties as per the inter-task command.

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/

static void rrcmisc_process_sim_inserted_req
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to the RRC Command */
)
{

  uint32 counter;                      /* Local variable to use as counter */
   
  byte sim_read_ens_flag;

  /* Assert the received command is RRC_SIM_INSERTED_REQ */
  if(RRC_SIM_INSERTED_REQ == cmd_ptr->cmd_hdr.cmd_id)
  {
#ifdef FEATURE_DUAL_SIM
    rrc_is_sim_id_valid = TRUE;
    rrc_sim_id_recvd = cmd_ptr->cmd.rrc_sim_inserted_req.as_id;
#endif

    /* To clear frequencies that have been barred because of AC restriction when SIM is not inserted */
    if((cmd_ptr->cmd.rrc_sim_inserted_req.hi_access_class & RRC_ACCESS_CLASS_11_BIT) ||
        (cmd_ptr->cmd.rrc_sim_inserted_req.hi_access_class & RRC_ACCESS_CLASS_15_BIT))
    {
      wl1_clear_barred_cells();
    }

    if(cmd_ptr->cmd.rrc_sim_inserted_req.imsi_present == TRUE)
    {
      rrc_imsi.num_digits = cmd_ptr->cmd.rrc_sim_inserted_req.imsi.num_digits;
      if(rrc_imsi.num_digits > RRC_MAX_GSMMAP_IMSI_DIGITS)
      {
          WRRC_MSG0_ERROR("Invalid IMSI is rcvd");
      }
      /* Update IMSI digits */
      for(counter = 0; counter < cmd_ptr->cmd.rrc_sim_inserted_req.imsi.num_digits;
          counter++)
      {
        rrc_imsi.value[counter] = cmd_ptr->cmd.rrc_sim_inserted_req.imsi.value[counter];
      }
      /* Set the IMSI to valid */
      rrc_imsi_valid = TRUE;
    }
    if(cmd_ptr->cmd.rrc_sim_inserted_req.ptmsi_present == TRUE)
    {
      /* Copy the PTMSI value from the command to RRC data */
     for(counter = 0; counter < RRC_MAX_GSMMAP_PTMSI_LENGTH; counter++)
     {
       rrc_ptmsi.value[counter] = cmd_ptr->cmd.rrc_sim_inserted_req.ptmsi.value[counter];
     }
     /* Set the PTMSI to valid */
     rrc_ptmsi_valid = TRUE;
    }

    if(cmd_ptr->cmd.rrc_sim_inserted_req.tmsi_present == TRUE)
    {
      /* Copy the TMSI value from the command to RRC Data*/
     for(counter = 0; counter < RRC_MAX_GSMMAP_TMSI_LENGTH; counter++)
     {
       rrc_tmsi.value[counter] = cmd_ptr->cmd.rrc_sim_inserted_req.tmsi.value[counter];
     }
     /* Set the TMSI to valid */
     rrc_tmsi_valid = TRUE;

    }
    /* Copy Low Access Class */
    rrc_lo_access_class = cmd_ptr->cmd.rrc_sim_inserted_req.lo_access_class;

    /* Copy High Access Class */
    rrc_hi_access_class = cmd_ptr->cmd.rrc_sim_inserted_req.hi_access_class;

   /* Update Ciphering Information in the Security Mode Procedure*/
    (void) rrcsmc_init_integrity_ciphering_config (&cmd_ptr->cmd.rrc_sim_inserted_req);

    rrc_rcvd_sim_ins_req = TRUE;

#if defined (FEATURE_UIM_DISABLE_GSDI_INTERFACE) || defined (FEATURE_DUAL_SIM)
    rrc_rcvd_admin_data = cmd_ptr->cmd.rrc_sim_inserted_req.admin_data;
#endif

#ifdef FEATURE_DUAL_SIM
    sim_read_ens_flag = reg_sim_per_subs_read_ens_flag(rrc_get_as_id());
#else
    sim_read_ens_flag = reg_sim_read_ens_flag();
#endif

    if(sim_read_ens_flag || nv_gcf_test_flag || rrc_clear_sibdb_for_lpm)
    {
      rrc_sibdb_clear_cell_header();
    }
  }
  else
  {
    WRRC_MSG1_HIGH("Invalid Event: %d is forwarded to MISC Procedure",
              cmd_ptr->cmd_hdr.cmd_id);
  }
  
} /* rrcmisc_process_sim_inserted_req */
/*===========================================================================

FUNCTION rrcmisc_process_sim_update_req  

DESCRIPTION
  This function process the RRC_SIM_UPDATE_REQ that received from MM and 
  updates  TMSI, PTMSI and Access Class varibales in the RRC DATA module
  and sets enable/ disable these indenties as per the inter-task command.

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/

static void rrcmisc_process_sim_update_req
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to the RRC Command */
)
{

  uint32 counter;                      /* Local variable to use as counter */
   
  /* Assert the received command is RRC_SIM_UPDATE_REQ */
  if(RRC_SIM_UPDATE_REQ == cmd_ptr->cmd_hdr.cmd_id)
  {
  
    MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"RRC_SIM_UPDATE_REQ ptmsi:%d, tmsi:%d, Current status ptmsi:%d, tmsi:%d ",
             cmd_ptr->cmd.rrc_sim_update_req.ptmsi_present,
             cmd_ptr->cmd.rrc_sim_update_req.tmsi_present,
             rrc_ptmsi_valid, 
             rrc_tmsi_valid);

    if(cmd_ptr->cmd.rrc_sim_update_req.ptmsi_present == TRUE)
    {
      /* Copy the PTMSI value from the command to RRC data */
     for(counter = 0; counter < RRC_MAX_GSMMAP_PTMSI_LENGTH; counter++)
     {
       rrc_ptmsi.value[counter] = cmd_ptr->cmd.rrc_sim_update_req.ptmsi.value[counter];
     }
     /* Set the PTMSI to valid */
     rrc_ptmsi_valid = TRUE;
    }
    else
    {
      rrc_ptmsi_valid = FALSE;
    }

    if(cmd_ptr->cmd.rrc_sim_update_req.tmsi_present == TRUE)
    {
      /* Copy the TMSI value from the command to RRC Data*/
     for(counter = 0; counter < RRC_MAX_GSMMAP_TMSI_LENGTH; counter++)
     {
       rrc_tmsi.value[counter] = cmd_ptr->cmd.rrc_sim_update_req.tmsi.value[counter];
     }
     /* Set the TMSI to valid */
     rrc_tmsi_valid = TRUE;

    }
    else
    {
      rrc_tmsi_valid = FALSE;
    }
    /* Copy Low Access Class */
    rrc_lo_access_class = cmd_ptr->cmd.rrc_sim_update_req.lo_access_class;

    /* Copy High Access Class */
    rrc_hi_access_class = cmd_ptr->cmd.rrc_sim_update_req.hi_access_class;

   /* Update Ciphering Information in the Security Mode Procedure*/
     rrcsmc_update_integrity_ciphering_config (&cmd_ptr->cmd.rrc_sim_update_req);
      
  }
  else
  {
    WRRC_MSG1_HIGH("Invalid Event: %d is forwarded to MISC Procedure",
              cmd_ptr->cmd_hdr.cmd_id);
  }
  
} /* rrcmisc_process_sim_update_req */

/*===========================================================================

FUNCTION rrcmisc_rrc_rcvd_sim_insert_req  

DESCRIPTION
  This function is currently called by CSP before a cell bar request is 
  sent to L1. If the SIM has not been read, then cell bar request is not
  sent to L1. The reason being that it is possible that the cell UE attempts
  to camp on is reserved for Operator use. If UE has high access class(es) 
  present then the cell should not be barred else the cell should be barred
  for 1280s (~21mins). Till SIM is read it is not known whether High ACs are 
  present or not and hence such a cell is not barred and in addition UE
  does not camp on this cell (during cell selection processs only and not
  due to cell access restrictions change via SIB modification). 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: SIM has been read
  FALSE: SIM has not been read

===========================================================================*/
boolean rrcmisc_rrc_rcvd_sim_insert_req()
{
  return (rrc_rcvd_sim_ins_req);

} /* rrcmisc_rrc_rcvd_sim_insert_req */

#ifdef FEATURE_INACTIVITY_HANDLING

/*===========================================================================

FUNCTION RRCMISC_CHECK_FOR_PS_RAB_INACTIVITY

DESCRIPTION
  Check for PS RAB Inactivity based on the status returned by RLC
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE -  If Inactivity is detected
  FALSE - If PS RABs are active

===========================================================================*/
boolean rrcmisc_check_for_ps_rab_inactivity()
{
  if(rlc_data_activity_ind == NO_DATA_TRANSFERRED)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
/*===========================================================================

FUNCTION RRCMISC_FORCE_GMMSM_CLOSE_SESSION_REQ

DESCRIPTION
  This function will call SM API to trigger Close Session Req to RRC if free
  floating is not defined else will send message to RABM/SM to do the same
  
DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcmisc_force_gmmsm_close_session_req()
{
#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
  rabm_cmd_type *rabm_ptr;
#endif

#ifndef FEATURE_WCDMA_FREE_FLOATING_TASK
  sm_force_gmmsm_close_session_req();
#else
  if ((rabm_ptr = rabm_get_cmd_buf()) == NULL)
  {
    ERR_FATAL("Can't send RABM cmd - out of memory!", 0, 0, 0);
  }
  rabm_ptr->header.cmd_id = RABMAS_FORCE_CLOSE_SESSION_IND;
  rabm_ptr->header.message_set = MS_RABM_RRC;
#ifdef FEATURE_DUAL_SIM
  rabm_ptr->cmd.rabmas_force_close_session_ind.as_id = rrc_get_as_id();
#endif
  /* Put the command on RABM queue */
  rabm_put_cmd(rabm_ptr);
#endif
}

/*===========================================================================

FUNCTION rrcmisc_inactivity_expiry_handler

DESCRIPTION


DEPENDENCIES
  None.

RETURN VALUE


===========================================================================*/
void rrcmisc_inactivity_expiry_handler
(
  rrc_cmd_type *cmd_ptr                       /* Pointer to RRC command */
)
{
  rrc_state_e_type curr_state = rrc_get_state();

  WRRC_MSG2_HIGH("RRCMISC: command id:%x rcvd curr_state:%d",cmd_ptr->cmd_hdr.cmd_id,curr_state);

  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_STATE_CHANGE_IND:
      if (cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED ||
          cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_CONNECTING ||   //* NOT SURE HERE
          cmd_ptr->cmd.state_change_ind.new_state  == RRC_STATE_CELL_PCH||
          cmd_ptr->cmd.state_change_ind.new_state  == RRC_STATE_URA_PCH)
      {
        rrctmr_stop_timer(RRCTMR_INACTIVITY_TIMER);
      }
      else
      {
        if (rrcscr_get_signalling_connection_status(RRC_PS_DOMAIN_CN_ID)) 
        {
          if (cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_CELL_DCH) 
          {
            rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, INACTIVITY_TIMER_IN_MS);
          }
          else
          {
            MSG_HIGH("Restarting Inactivity timer %d + remain T305 %d actual t305 %d",INACTIVITY_TIMER_IN_MS,
                     rrctmr_get_remaining_time(RRCTMR_T_305_TIMER),rrc_convert_t305_to_ms(RRC_GET_T305()));
            /* Restart incativity handling timer */
            if(rrc_convert_t305_to_ms(RRC_GET_T305()) != RRCTMR_INFINITY_VAL )
            {
              rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, 
                              (unsigned long)(INACTIVITY_TIMER_IN_MS + rrctmr_get_remaining_time(RRCTMR_T_305_TIMER))); 
            }
            else
            {
              /*If T305 timer was infinity then the timer wouldnt have been started . In thats case start
                timer of 5 mins plus 24 hrs*/
              rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, 
                              (unsigned long)(INACTIVITY_TIMER_IN_MS + RRCTMR_INFINITY_VAL)); 
            }
          }
          (void)rrcmisc_handle_ps_rab_inactivity(1);
        }
      }
      break;

    case RRC_INACTIVITY_TIMER_EXPIRED_IND:

      if (rrctmr_get_remaining_time(RRCTMR_INACTIVITY_TIMER) > 0) 
      {
        WRRC_MSG0_HIGH("Do not handle Inactivity timer expiry");
        return;
      }

      if ((!rrcrcr_is_proc_active())) 
      {
        if (rrcscr_get_signalling_connection_status(RRC_CS_DOMAIN_CN_ID))
        {
          if (curr_state == RRC_STATE_CELL_DCH) 
          {
            WRRC_MSG1_HIGH("CS signalling up, restart inact tmr %d",INACTIVITY_TIMER_IN_MS);
            rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, INACTIVITY_TIMER_IN_MS);
          }
          else if (curr_state == RRC_STATE_CELL_FACH)
          {
            MSG_HIGH("CS signalling up. Restart Inact tmr %d + remain T305 %d Actual T305 %d",INACTIVITY_TIMER_IN_MS,
                     rrctmr_get_remaining_time(RRCTMR_T_305_TIMER),rrc_convert_t305_to_ms(RRC_GET_T305()));
            /* Restart incativity handling timer */
            if(rrc_convert_t305_to_ms(RRC_GET_T305()) != RRCTMR_INFINITY_VAL )
            {
              rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, 
                              (unsigned long)(INACTIVITY_TIMER_IN_MS + rrctmr_get_remaining_time(RRCTMR_T_305_TIMER))); 
            }
            else
            {
              /*If T305 timer was infinity then the timer wouldnt have been started . In thats case start
                timer of 5 mins plus 24 hrs*/
              rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, 
                              (unsigned long)(INACTIVITY_TIMER_IN_MS + RRCTMR_INFINITY_VAL)); 
            }
          }
          (void)rrcmisc_handle_ps_rab_inactivity(1);
        }
        else
        {
          WRRC_MSG0_HIGH("CS signalling not up");
          if (rrc_check_rabs_associated_with_cn_domain(RRC_PS_DOMAIN_CN_ID)) 
          {
            if (SUCCESS == rrcmisc_handle_ps_rab_inactivity(2))
            {
              if (rrcmisc_check_for_ps_rab_inactivity()) 
              {
                WRRC_MSG0_HIGH("PS Inactivity detected. Inactive,Forcing close session req");
                rrcmisc_force_gmmsm_close_session_req();
              }
              else
              {
                WRRC_MSG0_HIGH("PS RAB active");
              }
              if (curr_state == RRC_STATE_CELL_DCH) 
              {
                WRRC_MSG1_HIGH("Restarting inact timer %d",INACTIVITY_TIMER_IN_MS);
                rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, INACTIVITY_TIMER_IN_MS);
              }
              else if (curr_state == RRC_STATE_CELL_FACH)
              {
                MSG_HIGH("Restarting Inactivity timer %d + remain T305 %d actual T305 timer",INACTIVITY_TIMER_IN_MS,
                         rrctmr_get_remaining_time(RRCTMR_T_305_TIMER),rrc_convert_t305_to_ms(RRC_GET_T305()));
                /* Restart incativity handling timer */
                if(rrc_convert_t305_to_ms(RRC_GET_T305()) != RRCTMR_INFINITY_VAL )
                {
                  rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, 
                                  (unsigned long)(INACTIVITY_TIMER_IN_MS + rrctmr_get_remaining_time(RRCTMR_T_305_TIMER))); 
                }
                else
                {
                 /*If T305 timer was infinity then the timer wouldnt have been started . In thats case start
                    timer of 5 mins plus 24 hrs*/
                  rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, 
                                  (unsigned long)(INACTIVITY_TIMER_IN_MS + RRCTMR_INFINITY_VAL)); 
                }
              }
              (void)rrcmisc_handle_ps_rab_inactivity(1);
            }
            else
            {
              WRRC_MSG0_ERROR("Inactivity status cannot be determined **********!!!");
              if (curr_state == RRC_STATE_CELL_DCH) 
              {
                WRRC_MSG1_HIGH("Restarting inact timer %d",INACTIVITY_TIMER_IN_MS);
                rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, INACTIVITY_TIMER_IN_MS);
              }
              else if (curr_state == RRC_STATE_CELL_FACH)
              {
                WRRC_MSG2_HIGH("Restarting Inactivity timer %d + remain T305 %d",INACTIVITY_TIMER_IN_MS,
                         rrctmr_get_remaining_time(RRCTMR_T_305_TIMER));
                /* Restart incativity handling timer */
                rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, 
                                   (unsigned long)(INACTIVITY_TIMER_IN_MS + rrctmr_get_remaining_time(RRCTMR_T_305_TIMER)));
              }
              (void)rrcmisc_handle_ps_rab_inactivity(1);
            }
          }
          else
          {
            WRRC_MSG0_HIGH("No PS rabs present, Force gmmsm close session req");
            rrcmisc_force_gmmsm_close_session_req();
            if (curr_state == RRC_STATE_CELL_DCH) 
            {
              WRRC_MSG1_HIGH("Restarting inact timer %d",INACTIVITY_TIMER_IN_MS);
              rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, INACTIVITY_TIMER_IN_MS);
            }
            else if (curr_state == RRC_STATE_CELL_FACH)
            {
              WRRC_MSG2_HIGH("Restarting Inactivity timer %d + remain T305 %d",INACTIVITY_TIMER_IN_MS,
                       rrctmr_get_remaining_time(RRCTMR_T_305_TIMER));
              /* Restart incativity handling timer */
              if(rrc_convert_t305_to_ms(RRC_GET_T305()) != RRCTMR_INFINITY_VAL )
              {
                rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, 
                                  (unsigned long)(INACTIVITY_TIMER_IN_MS + rrctmr_get_remaining_time(RRCTMR_T_305_TIMER))); 
              }
              else
              {
                /*If T305 timer was infinity then the timer wouldnt have been started . In thats case start
                    timer of 5 mins plus 24 hrs*/
                rrctmr_start_timer(RRCTMR_INACTIVITY_TIMER, 
                                  (unsigned long)(INACTIVITY_TIMER_IN_MS + RRCTMR_INFINITY_VAL)); 
              }
            }
            (void)rrcmisc_handle_ps_rab_inactivity(1); 
          }
        }
      }
      else
      {
        WRRC_MSG0_HIGH("SCR or RCR active.  Not handling exp ind");
      }
      break;
  default: 
      /* This should never happen */
      WRRC_MSG0_ERROR("Event is forwarded to MISC :");
      break;

  }
}
#endif /* FEATURE_INTACTIVITY_HANDLING */



/*===========================================================================

FUNCTION rrcmisc_initiate_cell_update

DESCRIPTION
  This procedure initiates the Cell Update procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcmisc_initiate_cell_update(void)
{
  rrc_cmd_type *int_cmd_ptr = NULL;
  if((int_cmd_ptr =  rrc_get_int_cmd_buf()) != NULL)
  {
    int_cmd_ptr->cmd_hdr.cmd_id = RRC_CELL_UPDATE_INITIATE_REQ;
    int_cmd_ptr->cmd.initiate_cell_update.cause =rrccu_get_cu_cause_for_wait_timer();
    int_cmd_ptr->cmd.initiate_cell_update.cu_complete_cnf_required = FALSE;
    int_cmd_ptr->cmd.initiate_cell_update.procedure = RRC_PROCEDURE_MISC;
  
    WRRC_MSG0_HIGH("CELL_UPDATE_INITIATE_REQ sent by RRCMISC due to T320 expiry");
    rrc_put_int_cmd(int_cmd_ptr);
  }
  rrccu_reset_cu_cause_for_wait_timer();
}
/*===========================================================================

FUNCTION rrcmisc_process_T320_expiry

DESCRIPTION
  This procedure handles the Expiry of timer T320.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcmisc_process_T320_expiry(rrc_cmd_type *cmd_ptr)
{
  rrc_state_e_type rrc_state = rrc_get_state();

  if(rrccu_get_cu_cause_for_wait_timer() == RRC_UL_DATA_TRANSMISSION)    
  {
    switch(rrc_state)
    {
       /*even in Ura pch cell update has to intited check:PSR*/
      case RRC_STATE_URA_PCH:
      case RRC_STATE_CELL_PCH:
        rrcmisc_initiate_cell_update();
        break;

      default:
        WRRC_MSG1_ERROR("Error ** T320 expiry in non PCH state. Current RRC State = %d",rrc_state);
        break;
    }
  }
}
/*===========================================================================

FUNCTION rrcmisc_process_multiple_drx

DESCRIPTION
  This function handles the following commands for Multipe DRX feature.

  RRC_T319_EXPIRED_IND
  RRC_BPLMN_SUSPEND_CNF

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
static void rrcmisc_process_multiple_drx(rrc_cmd_type *cmd_ptr)
{
  rrc_state_e_type rrc_state = rrc_get_state();

  switch(rrcmisc_multiple_drx_substate)
  {
    case  RRCMISC_MULTIPLE_DRX_INITIAL:
      if (cmd_ptr->cmd_hdr.cmd_id == RRC_T319_EXPIRED_IND)
      {
         if(((rrc_state == RRC_STATE_CELL_PCH) || 
            (rrc_state == RRC_STATE_URA_PCH)) &&
            (WTOW_SEARCH_STOP != rrc_csp_bplmn_srch_in_progress())
            )
         {
           WRRC_MSG0_HIGH("UTRAN DRX:BPLMN active. Suspend BPLMN.");
           rrc_csp_send_bplmn_suspend_req(RRC_PROCEDURE_MISC,BPLMN_SUSPEND_REASON_OTHER);
           rrcmisc_multiple_drx_substate =  RRCMISC_WAIT_FOR_BPLMN_SUSPEND_CNF;
         }
         else
         {
           rrcllc_update_drx_coef_length();
         }
      }
      else
      {
        WRRC_MSG1_HIGH("Invalid event %d in Multiple DRX initial substate",
                    cmd_ptr->cmd_hdr.cmd_id);
      }
      break;

    case RRCMISC_WAIT_FOR_BPLMN_SUSPEND_CNF:
      rrcmisc_multiple_drx_substate =  RRCMISC_MULTIPLE_DRX_INITIAL;
      if(cmd_ptr->cmd_hdr.cmd_id == RRC_BPLMN_SUSPEND_CNF)
      {
        if(cmd_ptr->cmd.rrc_csp_bplmn_suspend_cnf.status == SUCCESS)
        {
          rrcllc_update_drx_coef_length();
        }
        else
        {
          if(utran_drx_cycle_info.drx_cycle2_timer_value !=0)
          {
            rrctmr_start_timer(RRCTMR_T_319_TIMER,utran_drx_cycle_info.drx_cycle2_timer_value);
            WRRC_MSG0_HIGH("UTRAN DRX Coef Length 2 in use");
          }
          else
          {
            WRRC_MSG1_HIGH("T319 timer = %d, UTRAN DRX Coef Length 1 in use",utran_drx_cycle_info.drx_cycle2_timer_value);
          }
        }
      }
      else
      {
        WRRC_MSG1_HIGH("Invalid event %d in Wait for BPLMN Suspend cnf substate",
                    cmd_ptr->cmd_hdr.cmd_id);
      }
      break;
  }

return;
}

/*===========================================================================

FUNCTION rrcmisc_state_change_handler

DESCRIPTION
  Used to handle state change to/from Cell PCH (or URA PCH)
  
DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
void rrcmisc_state_change_handler
(
  rrc_cmd_type *cmd_ptr
)
{
  WRRC_MSG2_HIGH("State changed to %d from %d",cmd_ptr->cmd.state_change_ind.new_state,
           cmd_ptr->cmd.state_change_ind.previous_state);
    if(cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_CELL_PCH ||
        cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_URA_PCH)
    {
      if(rrccu_get_cu_wait_timer_value() != RRC_T320_INVALID_VALUE)
      {
        rrctmr_start_timer(RRCTMR_T_320_TIMER, 1000*rrccu_get_cu_wait_timer_value());

        /*Now we have used the value of wait timer to start T320. So we need to reset it.*/		
        rrccu_reset_cu_wait_timer_value();
      }
    }
    /*This case handles the scenarios when next state is not PCH and T320 isn't running.
        In that case, we need to reset wait timer value.*/	 
    else if(rrctmr_get_remaining_time(RRCTMR_T_320_TIMER) == 0)
    {
      rrccu_reset_cu_cause_for_wait_timer();
    }

    /*If UE is going to Idle mode, we need to reset wait timer value and cu cause in CU.*/			
    if(cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED)
    {
      rrccu_reset_cu_wait_timer_value(); 
      rrccu_reset_cu_cause_for_wait_timer();
    }


  if(
      (cmd_ptr->cmd.state_change_ind.previous_state == RRC_STATE_CELL_DCH ||
       cmd_ptr->cmd.state_change_ind.previous_state == RRC_STATE_CELL_FACH ) 
       &&
      (cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_CELL_PCH ||
           cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_URA_PCH)
    )
  {
    if(utran_drx_cycle_info.drx_cycle2_timer_value !=0)
    {
      rrctmr_start_timer(RRCTMR_T_319_TIMER, utran_drx_cycle_info.drx_cycle2_timer_value);
      WRRC_MSG0_HIGH("UTRAN DRX Coef Length 2 in use");
    }
    else
    {
      WRRC_MSG1_HIGH("T319 timer = %d, UTRAN DRX Coef Length 1 in use",utran_drx_cycle_info.drx_cycle2_timer_value);
    }
  }
  
  if(
      (cmd_ptr->cmd.state_change_ind.previous_state == RRC_STATE_CELL_PCH || 
        cmd_ptr->cmd.state_change_ind.previous_state == RRC_STATE_URA_PCH) 
        && 
        (cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_CELL_DCH ||
        cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_CELL_FACH)
    )
  {
    if(rrctmr_get_remaining_time(RRCTMR_T_319_TIMER) >0)
    {
      rrctmr_stop_timer(RRCTMR_T_319_TIMER);
  
      rrcllc_update_drx_coef_length();
    }
  }
}


/*===========================================================================

FUNCTION rrcmisc_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for the RRC Misc procedure.
  The following events are handled by this procedure:

    RRC_DOWNLINK_SDU_IND Primitive (received from RLC)
    RRC_SIM_INSERTED_REQ primitive (received from MM)
    RRC_SIM_UPDATE_REQ primitive   (received from MM)


DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcmisc_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                       /* Pointer to RRC command */
)
{
 
#ifdef  FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY
  #error code not present
#else
  l1_rssi_info_struct_type l1_rssi;
#endif
  rrc_state_e_type rrc_state = rrc_get_state();

  WRRC_MSG1_HIGH_OPT("function rrcmisc_procedure_event_handler is called in rrc_state = rrc_state_e_type_value%d",rrc_state);
  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_MISC,RRC_INVALID_PROC_SUBSTATE,cmd_ptr->cmd_hdr.cmd_id);



  /* RSSI Enhancement */
  if (cmd_ptr->cmd_hdr.cmd_id == RRC_STATE_CHANGE_IND)
  {

    if(cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED) 
    {
      rrc_update_reconfig_waiting_for_l2ack_dch(MAX_NUM_OF_PROCEDURES);
    }


#ifdef  FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY
    #error code not present
#else
    l1_get_rssi(&l1_rssi);
    if(l1_rssi.is_valid_rssi)
    {
      rrc_rssi = l1_rssi.rssi_value; 
    }
#endif
    
#ifdef FEATURE_INACTIVITY_HANDLING
    rrcmisc_inactivity_expiry_handler(cmd_ptr);
#endif

    /*If we're going out of Cell_DCH state, we need to invalidate the lac_in_ota.*/
    if((cmd_ptr->cmd.state_change_ind.new_state != RRC_STATE_CELL_DCH) &&
       (cmd_ptr->cmd.state_change_ind.previous_state == RRC_STATE_CELL_DCH))
    {
      lac_in_ota_present = FALSE;
    }
    if(cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED)
    {
      rrc_ordered_state = RRC_STATE_MAX;
    }

    /*In case of a state change indication*/
    rrcmisc_state_change_handler(cmd_ptr);
    
#ifdef FEATURE_QCHAT
    if (qchatupk_wcdma_amssopts_enabled())
    {
      qchatupk_rrc_state_change_handler(cmd_ptr);
      WRRC_MSG0_HIGH("QCHAT is informed about RRC state change");
    }
#endif /*FEATURE_QCHAT*/

    return;
  }

#ifdef FEATURE_INACTIVITY_HANDLING
  if(cmd_ptr->cmd_hdr.cmd_id == RRC_INACTIVITY_TIMER_EXPIRED_IND)
  {
    rrcmisc_inactivity_expiry_handler(cmd_ptr);
    return;
  }
#endif

  if(cmd_ptr->cmd_hdr.cmd_id == RRC_SET_DRX_IND)
  {
    rrc_qchat_drx_coeff = cmd_ptr->cmd.rrc_set_drx_ind.drx_coefficient;
    WRRC_MSG2_HIGH("QCHAT_DRX: SET_DRX_IND recvd with coeff value %d in RRC state:%d", rrc_qchat_drx_coeff, rrc_state);

    /* RRC to send DRX_REQ to L1 with the negotiated PS DRX coeffs. DRX_REQ is sent only in IDLE state, because in connected state 
        PS DRX will not be used when PS session is open. 
       Spec ref: 25.331 - 8.3 Discontinuous Reception for Paging
       The DRX cycle lengths to use for UTRAN connected mode is the shortest of the following:
       - UTRAN DRX cycle length;
       - any of the stored CN domain specific DRX cycle length for the CN domains the UE is only attached to with no
         signalling connection established.
    */
    if((RRC_MODE_ACTIVE == rrcmcm_get_rrc_mode()) && (rrc_state == RRC_STATE_DISCONNECTED))
    {
      /* Send DRX request to L1 */
      rrc_csp_send_l1_drx_req();
    }
    return;
  }
#ifdef FEATURE_QCHAT_HFN_MISMATCH_RECOVERY
  if(cmd_ptr->cmd_hdr.cmd_id == RRC_CRLC_PDU_INVALID_IND)
  {
    if(rrcscr_get_signalling_connection_status(RRC_PS_DOMAIN_CN_ID))
    { 
      sm_force_gmmsm_close_session_req();
      WRRC_MSG0_HIGH("Qchat:HFN mismatch Force gmmsm close session req");
      return;
    }
  }
#endif

#ifdef FEATURE_MODEM_CONFIG_REFRESH
  if(RRC_NV_REFRESH_IND == cmd_ptr->cmd_hdr.cmd_id)
  {
    /* As per the requirement, WCDMA will be put to inactive before receiving the NV refresh callback */  
    if(RRC_MODE_INACTIVE == rrcmcm_get_rrc_mode())
    {
      /* Framework to re-read the required NV/EFS on SIM swapping 
       * More information can be added on need basis
       */
      rrc_read_nv_refresh();
    }
    else
    {
      /* NV refresh indication received when WCDMA is active..Ignore */
    }

    /* NV refresh done indication should be notified to MCFG with the same reference id */
    if(!mcfg_refresh_done(cmd_ptr->cmd.rrc_nv_refresh.reference_id))
    {
      WRRC_MSG0_ERROR("NV refresh done indication failed");
    }
    return;
  }
#endif

  switch (rrc_state)
  {
    case RRC_STATE_DISCONNECTED: 
    case RRC_STATE_CELL_PCH:
    case RRC_STATE_URA_PCH:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_SIM_INSERTED_REQ:
          rrcmisc_process_sim_inserted_req(cmd_ptr);
          break;

        case RRC_SIM_UPDATE_REQ:
          rrcmisc_process_sim_update_req(cmd_ptr);
          break;

        case RRC_DOWNLINK_SDU_IND:     
          /* In these states, if RRC dispatcher failes to identify OTA message
             Type, RRC does not require to send RRC STATUS message.
          */
          WRRC_MSG0_HIGH("Unable to identify the Message Type");
          break;
 
        case RRC_T320_EXPIRED_IND:
          rrcmisc_process_T320_expiry(cmd_ptr);
          break;

        case RRC_T319_EXPIRED_IND:
        case RRC_BPLMN_SUSPEND_CNF:
          rrcmisc_process_multiple_drx(cmd_ptr);
          break;

#ifdef FEATURE_FEMTO_CSG
        case RRC_CSG_WHITE_LIST_UPDATE_IND:
          rrcmisc_process_white_list_ind(cmd_ptr);
          break;
#endif

        default:
          WRRC_MSG1_HIGH("Invalid event %d is forwarded to MISC proc",
                    cmd_ptr->cmd_hdr.cmd_id);
          break;      }
      break;

    case RRC_STATE_CONNECTING:
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_SIM_INSERTED_REQ:
          WRRC_MSG0_ERROR("SIM Inserted rcvd in invalid state");
          break;

        case RRC_SIM_UPDATE_REQ:
          rrcmisc_process_sim_update_req(cmd_ptr);
          break;

#ifdef FEATURE_FEMTO_CSG
        case RRC_CSG_WHITE_LIST_UPDATE_IND:
          rrcmisc_process_white_list_ind(cmd_ptr);
          break;
#endif

        case RRC_DOWNLINK_SDU_IND:
          /* In this state, if RRC dispatcher failes to identify OTA message
             Type, RRC does not require to send RRC STATUS message.
          */
          WRRC_MSG0_HIGH("Unable to identify the Message Type");
          break;

        default:
          WRRC_MSG1_HIGH("Invalid event %d is forwarded to MISC proc",
                    cmd_ptr->cmd_hdr.cmd_id);
          break;

      }
      break;

    case RRC_STATE_CELL_FACH:    
    case RRC_STATE_CELL_DCH:  
      switch(cmd_ptr->cmd_hdr.cmd_id)
      {
        case RRC_SIM_INSERTED_REQ:
          rrcmisc_process_sim_inserted_req(cmd_ptr);
          break;

        case RRC_SIM_UPDATE_REQ:
          rrcmisc_process_sim_update_req(cmd_ptr);
          break;

#ifdef FEATURE_FEMTO_CSG
        case RRC_CSG_WHITE_LIST_UPDATE_IND:
          rrcmisc_process_white_list_ind(cmd_ptr);
          break;
#endif

        case RRC_DOWNLINK_SDU_IND:
          /* In these states,if RRC dispatcher failes to identify OTA message
             type RRC need to send RRC STATUS message if message is received 
             on DCCH.
          */
          rrcmisc_process_downlink_msg(cmd_ptr);
          break; 

        case RRC_T319_EXPIRED_IND:
          rrcllc_update_drx_coef_length();
          break;

        default:     
          WRRC_MSG1_HIGH("Invalid event %d is forwarded to MISC proc",
                    cmd_ptr->cmd_hdr.cmd_id);
          break;
      }
      break;


    default: 
      /* This should never happen */
      ERR_FATAL("Event is forwarded to MISC in an invalid State: %d",
                 rrc_state,0,0);
      break;
   }  
} /* rrcmisc_procedure_event_handler */

/*===========================================================================

FUNCTION rrcmisc_init_procedure

DESCRIPTION
  This function initializes everything that the RRC Misc procedure
  needs to have initialized.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcmisc_init_procedure(void)
{

  rrcmisc_multiple_drx_substate =  RRCMISC_MULTIPLE_DRX_INITIAL;
  /* Initialize the lo and hi access classes to invalid values */
  rrc_lo_access_class = RRC_LO_ACCESS_CLASS_ABSENT;
  rrc_hi_access_class = RRC_HI_ACCESS_CLASS_ABSENT;

#ifdef FEATURE_INACTIVITY_HANDLING

  rrcscmgr_register_for_scn(RRC_PROCEDURE_MISC,
                            RRC_STATE_WILDCARD,
                            RRC_STATE_WILDCARD);
#else /*FEATURE_INTACTIVITY_HANDLING */

  rrcscmgr_register_for_scn(RRC_PROCEDURE_MISC,
                            RRC_STATE_WILDCARD,
                            RRC_STATE_WILDCARD);
#endif

  rrc_qchat_drx_coeff = SYS_DRX_NOT_SPECIFIED_BY_MS;

} /* rrcmisc_init_procedure */

#ifdef FEATURE_INACTIVITY_HANDLING

/*===========================================================================

FUNCTION RRCMISC_HANDLE_PS_RAB_INACTIVITY

DESCRIPTION
  Get/Reset Inactivity flag in RLC
  
DEPENDENCIES
  None.

RETURN VALUE
  Success : If values were successfully set/reset
  Failue  : Future use
  
===========================================================================*/

uecomdef_status_e_type rrcmisc_handle_ps_rab_inactivity(uint8 instance_id)
{
  if(1 == instance_id)
  {
    /* Reset the data activity flag in RLC */
    WRRC_MSG0_HIGH("Resetting RLC activity flag for Sampling Instance 1");
    rlc_reset_activity_flag(); 
  }
  else if(2 == instance_id)
  {
    /* Query for the data activity flag from RLC */
    rlc_data_activity_ind = rlc_query_data_channel_activity();
    WRRC_MSG1_HIGH("Queried RLC for activity flag for Sampling Instance 2.Data activity status:%d",
               rlc_data_activity_ind);           
  }
  return SUCCESS;
}

#endif /* FEATURE_INACTIVITY_HANDLING */


#ifdef FEATURE_WCDMA_REL7_SPECRS 
/***************RRC CO UEC ************************************/

/*===========================================================================

FUNCTION rrccouec_is_proc_active

DESCRIPTION
  This function tells whether RRC COUEC procedure is active or not.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
boolean rrccouec_is_proc_active
(
  void
)
{
  if(rrccouec.substate != RRCCOUEC_INITIAL)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
/*===========================================================================

FUNCTION rrccouec_process_completion_cap_change_with_reconfig

DESCRIPTION
  This function is called from Reconfig procedures to motify that cap change is complete. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrccouec_process_completion_cap_change_with_reconfig
(
  void
)
{
  rrctmr_stop_timer(RRCTMR_COUEC_TIMER);
  
  /*Save the requested UE capabilities into global "rrc_ue_cap_during_rrc_conn"*/
  rrccouec_save_ue_cap();  
  
  rrccouec_clear_procedure();
}

    

/*===========================================================================

FUNCTION rrccouec_process_completion_of_ueci_req

DESCRIPTION
  This function is the procedure event handler of RRC COUEC procedure. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrccouec_process_completion_of_ueci_req
(
  rrccouec_cnf_e_type rrc_ueci_cnf
)
{
  /*To put a safegaurd, check whether COUEC is in RRCCOUEC_WAIT_FOR_UECI_CNF substate.*/
  if(rrccouec.substate ==RRCCOUEC_WAIT_FOR_UECI_CNF)
  {
    if(rrc_ueci_cnf == CHANGE_OF_CAP_ACCEPTED)
    {
      /*Save the requested UE capabilities into global "rrc_ue_cap_during_rrc_conn"*/
      rrccouec_save_ue_cap();
      rrccouec_send_couec_cnf(RRC_UE_CAP_CHANGE_ACCEPTED);
      rrccouec_clear_procedure();
    }
    else if((rrc_ueci_cnf == CHANGE_OF_CAP_WITH_RECONFIG) || (rrc_ueci_cnf == CHANGE_OF_CAP_REFUSED))
    {
      /*Start COUEC timer till Reconfig OTA provides the confirmation of UE Capability change OR the next retry
          of RRC_INITIATE_UE_CAPABILITY_REQ to UECI procedure.*/
      rrctmr_start_timer(RRCTMR_COUEC_TIMER, RRCTMR_COUEC_TIMER_IN_MS);
      rrccouec.substate = RRCCOUEC_WAIT_FOR_COUEC_TIMER_TO_EXPIRE;    
    }
    else
    {
      WRRC_MSG1_ERROR("Invalid UECI CNF type %d",rrc_ueci_cnf);
    }
  }
  else
  {
    WRRC_MSG0_ERROR("Invalid Call in COUEC procedure.");
  }
}

/*===========================================================================
FUNCTION rrccouec_save_ue_cap

DESCRIPTION
  This function saves the requested UE  capabilities. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrccouec_save_ue_cap
(
  void
)
{
  if(rrccouec.ue_cap_change_requested.is_valid_mask & HSDPA_CAT_INCL)
  {
    rrc_ue_cap_during_rrc_conn.hsdpa_category = rrccouec.ue_cap_change_requested.hsdpa_category;
  }
  if(rrccouec.ue_cap_change_requested.is_valid_mask & HSUPA_CAT_INCL)
  {
    rrc_ue_cap_during_rrc_conn.hsupa_category = rrccouec.ue_cap_change_requested.hsupa_category;
  }
}


/*===========================================================================

FUNCTION rrc_save_rnc_cap_change_support

DESCRIPTION
  This function updates global "rrc_rnc_cap_change_support" based on the OTA IE value.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rrc_save_rnc_cap_change_support
(
  void
)
{
  if(rrc_ordered_rnc_cap_change_support != RNC_CAP_CHANGE_NONE)
  {
    if(rrc_ordered_rnc_cap_change_support == RNC_CAP_CHANGE_SUPPORTED)
    {
      rrc_rnc_cap_change_support = TRUE;
    }
    else if(rrc_ordered_rnc_cap_change_support == RNC_CAP_CHANGE_UNSUPPORTED)
    {
      rrc_rnc_cap_change_support = FALSE;
    }
    WRRC_MSG1_HIGH("Support of Change of UE Capability = %d",rrc_rnc_cap_change_support);
  }

  /*Irrespective of the previous value, initialie rrc_ordered_rnc_cap_change_support for the next OTA.*/
  rrc_ordered_rnc_cap_change_support = RNC_CAP_CHANGE_NONE;
  
}


/*===========================================================================

FUNCTION rrccouec_send_couec_cnf

DESCRIPTION
  This function sends  RRC_COUEC_CNF comamnd to the requested procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrccouec_send_couec_cnf
(
  rrc_couec_cnf_e_type couec_cnf
)
{
  rrc_cmd_type *rrc_cmd_ptr;                 /* Pointer to the RRC Command */
  
  if((rrc_cmd_ptr =  rrc_get_int_cmd_buf()) != NULL)
  {
    /* Fill in the command parameters for RRC_COUEC_CNF */
    rrc_cmd_ptr->cmd_hdr.cmd_id = RRC_COUEC_CNF;
    rrc_cmd_ptr->cmd.rrc_couec_cnf.proc_id = rrccouec.proc_id;
    rrc_cmd_ptr->cmd.rrc_couec_cnf.ueci_cnf = couec_cnf;
    /* Send RRC_COUEC_CNF primitive */
    rrc_put_int_cmd(rrc_cmd_ptr);       
  }
}


/*===========================================================================

FUNCTION rrccouec_process_initial_substate

DESCRIPTION
  This function is the substate event handler of COUEC procedure. This substate handler handles following commands.
  1. RRC_COUEC_REQ 
  2. RRC_STATE_CHANGE_IND - It resets the COUEC procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrccouec_process_initial_substate
(
  rrc_cmd_type *cmd_ptr                       /* Pointer to RRC command */
)
{

  rrc_cmd_type *rrc_cmd_ptr;                 /* Pointer to the RRC Command */
  
  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_COUEC_REQ:

      rrccouec.proc_id = cmd_ptr->cmd.rrc_couec_req.proc_id;
      
      /*Check whether RNC supports Change in UE Capability or not*/
      if(rrc_rnc_cap_change_support == FALSE)
      {
        /*If RNC doesn't support Change in UE Capability, send unsupported to requested proc.*/
        rrccouec_send_couec_cnf(RRC_UE_CAP_CHANGE_UNSUPPORTED);
        rrccouec_clear_procedure();
      }
      else if(cmd_ptr->cmd.rrc_couec_req.ue_cap_requested.is_valid_mask)        
      {
        /*To initiate RRC_INITIATE_UE_CAPABILITY_REQ*/
        rrc_cmd_ptr =  rrc_get_int_cmd_buf();
 
        /* Fill in the command parameters for RRC_INITIATE_UE_CAPABILITY_REQ */
        rrc_cmd_ptr->cmd_hdr.cmd_id = RRC_INITIATE_UE_CAPABILITY_REQ;

        rrc_cmd_ptr->cmd.initiate_ue_cap.originator = 
            RRC_UE_CAPABILITY_ORIGINATOR_COUEC;

        rrc_cmd_ptr->cmd.initiate_ue_cap.transaction_id = RRCUECI_INVALID_TRANSACTION_ID; 

        if(cmd_ptr->cmd.rrc_couec_req.ue_cap_requested.is_valid_mask & HSDPA_CAT_INCL)
        {
          rrccouec.ue_cap_change_requested.is_valid_mask |= HSDPA_CAT_INCL;
          rrccouec.ue_cap_change_requested.hsdpa_category = cmd_ptr->cmd.rrc_couec_req.ue_cap_requested.hsdpa_category;
        }
        
        if(cmd_ptr->cmd.rrc_couec_req.ue_cap_requested.is_valid_mask & HSUPA_CAT_INCL)
        {
          rrccouec.ue_cap_change_requested.is_valid_mask |= HSUPA_CAT_INCL;
          rrccouec.ue_cap_change_requested.hsupa_category = cmd_ptr->cmd.rrc_couec_req.ue_cap_requested.hsupa_category;
        }       
        /* Send RRC_INITIATE_UE_CAPABILITY_REQ primitive */
        
        rrc_put_int_cmd(rrc_cmd_ptr);   
        
        rrccouec.substate = RRCCOUEC_WAIT_FOR_UECI_CNF;
        rrccouec.num_of_retries++;          

      }
      else
      {
        /*In this case, the requested procedure hasn't changed any UE capability, so just send NONE.*/
        rrccouec_send_couec_cnf(RRC_UE_CAP_CHANGE_NONE);
        rrccouec_clear_procedure();
      }
      break;
  
    case RRC_STATE_CHANGE_IND: 
      /*Since we are going to disconnected state (Idle mode), we need to
      reset rrc_rnc_cap_change_support to FALSE and reset the COUEC procedure.*/   
      if(cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED)
      {
        rrc_rnc_cap_change_support = FALSE;
        rrc_ordered_rnc_cap_change_support = RNC_CAP_CHANGE_NONE;
        rrccouec_clear_procedure();
      }
      break;

    default:
      WRRC_MSG1_ERROR("Unsupported cmd %d in initial substate of UECI Proxy",cmd_ptr->cmd_hdr.cmd_id);        
  }
}

/*===========================================================================

FUNCTION rrccouec_process_wait_for_timer_couec_to_expire

DESCRIPTION
  This function is the substate event handler of RRC COUEC procedure. This substate handler handles following commands.
  1. RRC_TIMER_COUEC_EXPIRED_IND - It handles the expiry of COUEC timer.
  2. RRC_STATE_CHANGE_IND - It resets the COUEC procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrccouec_process_wait_for_timer_couec_to_expire
(
  rrc_cmd_type *cmd_ptr                       /* Pointer to RRC command */
)
{

  rrc_cmd_type *rrc_cmd_ptr;                 /* Pointer to the RRC Command */
  
  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_TIMER_COUEC_EXPIRED_IND:
        
      if(rrccouec.num_of_retries < N_COUEC_TIMER)
      {
        rrctmr_start_timer(RRCTMR_COUEC_TIMER, RRCTMR_COUEC_TIMER_IN_MS);
        
        /*To initiate RRC_INITIATE_UE_CAPABILITY_REQ*/
        rrc_cmd_ptr =  rrc_get_int_cmd_buf();

        /* Fill in the command parameters for RRC_INITIATE_UE_CAPABILITY_REQ */
        rrc_cmd_ptr->cmd_hdr.cmd_id = RRC_INITIATE_UE_CAPABILITY_REQ;

        rrc_cmd_ptr->cmd.initiate_ue_cap.originator = 
            RRC_UE_CAPABILITY_ORIGINATOR_COUEC;

        rrc_cmd_ptr->cmd.initiate_ue_cap.transaction_id = RRCUECI_INVALID_TRANSACTION_ID; 

        /* Send RRC_INITIATE_UE_CAPABILITY_REQ primitive */
        rrc_put_int_cmd(rrc_cmd_ptr);   
        rrccouec.substate = RRCCOUEC_WAIT_FOR_UECI_CNF;
        rrccouec.num_of_retries++;          

      }
      else
      {
        WRRC_MSG0_HIGH("No. of retries is more than N_COUEC_TIMER.");
        rrccouec_send_couec_cnf(RRC_UE_CAP_CHANGE_REFUSED);
        rrccouec_clear_procedure();
      }
      break;

    case RRC_STATE_CHANGE_IND: 
      /*Since we are going to disconnected state (Idle mode), we need to
      reset rrc_rnc_cap_change_support to FALSE reset the COUEC procedure.*/   
      if(cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED)
      {
        rrc_rnc_cap_change_support = FALSE;
        rrc_ordered_rnc_cap_change_support = RNC_CAP_CHANGE_NONE;
        rrccouec_clear_procedure();
      }
      break;      
      
    default:
      WRRC_MSG1_ERROR("Unexpected cmd %d in COUEC.",cmd_ptr->cmd_hdr.cmd_id);        
  }
}

/*===========================================================================

FUNCTION rrccouec_process_wait_for_ueci_cnf

DESCRIPTION
  This function is the substate event handler of COUEC procedure. This substate handler handles following commands.
  1. RRC_STATE_CHANGE_IND - It resets the COUEC procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrccouec_process_wait_for_ueci_cnf
(
  rrc_cmd_type *cmd_ptr                       /* Pointer to RRC command */
)
{
  switch(cmd_ptr->cmd_hdr.cmd_id)
  {
    case RRC_STATE_CHANGE_IND: 
    /*Since we are going to disconnected state (Idle mode), we need to
    reset rrc_rnc_cap_change_support to FALSE reset the COUEC procedure.*/   
    if(cmd_ptr->cmd.state_change_ind.new_state == RRC_STATE_DISCONNECTED)
    {
      rrc_rnc_cap_change_support = FALSE;
      rrc_ordered_rnc_cap_change_support = RNC_CAP_CHANGE_NONE;
      rrccouec_clear_procedure();
    }
    break;      

    default:
      WRRC_MSG1_ERROR("Unexpected cmd %d in COUEC.",cmd_ptr->cmd_hdr.cmd_id);         
  }
}

/*===========================================================================

FUNCTION rrccouec_procedure_event_handler

DESCRIPTION
  This function is the procedure event handler of COUEC procedure. Currently, it processes the following
  RRC Internal commands.
  1. RRC_COUEC_REQ
  2. RRC_STATE_CHANGE_IND
  3. RRC_TIMER_COUEC_EXPIRED_IND

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrccouec_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                       /* Pointer to RRC command */
)
{
  WRRC_MSG1_HIGH_OPT("function rrccouec_procedure_event_handler is called in rrccouec.substate = rrccouec_substate_e_type_value%d",rrccouec.substate);
  
  /*If RRC Conn Rel is in progress, no need to process any RRC command in UECI Proxy*/
  if (FALSE == rrcrcr_is_proc_active())
  {
    switch(rrccouec.substate)
    {
      case  RRCCOUEC_INITIAL:
        rrccouec_process_initial_substate(cmd_ptr);     
        break;

      case RRCCOUEC_WAIT_FOR_UECI_CNF:
        WRRC_MSG0_HIGH("COUEC is active and waiting for UECI to complete.");       
        break;      
      
      case  RRCCOUEC_WAIT_FOR_COUEC_TIMER_TO_EXPIRE:
        rrccouec_process_wait_for_timer_couec_to_expire(cmd_ptr);
        break;
      
      default:
        WRRC_MSG2_HIGH("Command %d forwarded in Invalid COUEC substate %d",
            cmd_ptr->cmd_hdr.cmd_id,rrccouec.substate);       
    }
  }
  else
  {
    WRRC_MSG1_HIGH("RRC Conn Rel is in progress. COUEC not processing cmd %d",
        cmd_ptr->cmd_hdr.cmd_id);
    rrccouec_clear_procedure();
  }
}

/*===========================================================================

FUNCTION rrccouec_clear_procedure

DESCRIPTION
  This function clears the COUEC  procedure. Essentially, it resets the procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrccouec_clear_procedure
(
  void
)
{
  /*Reset all the globals within COUEC procedure*/
  rrccouec.ue_cap_change_requested.is_valid_mask = 0;
  rrccouec.substate = RRCCOUEC_INITIAL;
  rrccouec.num_of_retries = 0;
  rrccouec.proc_id = RRC_PROCEDURE_NONE;
}

/*===========================================================================

FUNCTION rrccouec_init_procedure

DESCRIPTION
  This function initializes required parameters for Change of UE Capability procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrccouec_init_procedure
(
  void
)
{
  /* Set COUEC procedure state to RRCCOUEC_INITIAL at powerup */
  rrccouec.substate = RRCCOUEC_INITIAL;

  rrccouec.num_of_retries = 0;

  rrccouec.proc_id = RRC_PROCEDURE_NONE;

  /*Set the valid mask to 0 in UE Cap database internal to COUEC */
  rrccouec.ue_cap_change_requested.is_valid_mask = 0;

  /* Register for transition from any state to Disconnected state */
  rrcscmgr_register_for_scn(RRC_PROCEDURE_COUEC,
                            RRC_STATE_WILDCARD,
                            RRC_STATE_DISCONNECTED);
}
/***************RRC CO UEC ************************************/
#endif /*FEATURE_WCDMA_REL7_SPECRS*/ 

/*====================================================================
FUNCTION: rrcllc_initiate_config_ptrs()

DESCRIPTION:
  This function is called to allocate OC, CC and TOC pointers

DEPENDENCIES:
  None.

RETURN VALUE:
  None
  
SIDE EFFECTS:

====================================================================*/
void rrc_init_dynamic_ptrs(
  void
  )
{
  MSG_HIGH("config ptrs %d %d %d", ordered_config_ptr, current_config_ptr, transition_config.toc_ptr);
  if ((ordered_config_ptr != NULL) || (current_config_ptr != NULL) || (transition_config.toc_ptr != NULL))
  {
    ERR_FATAL("Trying to initialize the config pointers that are already initialized 0x%x 0x%x 0x%x", 
              (uint32)ordered_config_ptr, (uint32)current_config_ptr, (uint32)transition_config.toc_ptr);
  }

#ifdef FEATURE_WCDMA_RRC_HEAP_TO_BSS
  current_config_ptr =             &current_config_glob_var;
  ordered_config_ptr =             &ordered_config_glob_var;
  transition_config.toc_ptr =      &TOC_glob_var;
  sib_events_ptr =                 &sib_events_glob_var;

#ifdef FEATURE_WCDMA_HS_FACH
  rrc_ordered_hanging_rb =         &rrc_ordered_hanging_rb_glob_var[0];
  rrc_current_hanging_rb =         &rrc_current_hanging_rb_glob_var[0];
  rrcllc_init_hanging_rb();
#endif /* FEATURE_WCDMA_HS_FACH */
  rrc_ordered_hanging_rb_mapping = &rrc_ordered_hanging_rb_mapping_glob_var[0];
  rrc_current_hanging_rb_mapping = &rrc_current_hanging_rb_mapping_glob_var[0];
#else 
  current_config_ptr =  modem_mem_calloc(1,sizeof(ordered_config_type), MODEM_MEM_CLIENT_WCDMA_RRC_GLOBAL_CRIT);
  ordered_config_ptr =  modem_mem_calloc(1,sizeof(ordered_config_type), MODEM_MEM_CLIENT_WCDMA_RRC_GLOBAL_CRIT);
  transition_config.toc_ptr =  modem_mem_calloc(1,sizeof(ordered_config_type), MODEM_MEM_CLIENT_WCDMA_RRC_GLOBAL_CRIT);
  sib_events_ptr = modem_mem_calloc(1,sizeof(sib_active_event_database_type), MODEM_MEM_CLIENT_WCDMA_RRC_GLOBAL_CRIT);

#ifdef FEATURE_WCDMA_HS_FACH
  rrc_ordered_hanging_rb = modem_mem_calloc(MAX_HANGING_RB_MAPPING_INFO,sizeof(rb_mapping_info_type_), MODEM_MEM_CLIENT_WCDMA_RRC_GLOBAL_CRIT);
  rrc_current_hanging_rb = modem_mem_calloc(MAX_HANGING_RB_MAPPING_INFO,sizeof(rb_mapping_info_type_), MODEM_MEM_CLIENT_WCDMA_RRC_GLOBAL_CRIT);
  WRRC_MSG1_HIGH("RRC_DEBUG: Size allocated for hanging RB %d",MAX_HANGING_RB_MAPPING_INFO*sizeof(rb_mapping_info_type));
  rrcllc_init_hanging_rb();
#endif

  rrc_ordered_hanging_rb_mapping = modem_mem_calloc(MAX_HANGING_RB_MAPPING_INFO,sizeof(rb_mapping_info_type), MODEM_MEM_CLIENT_WCDMA_RRC_GLOBAL_CRIT);
  rrc_current_hanging_rb_mapping = modem_mem_calloc(MAX_HANGING_RB_MAPPING_INFO,sizeof(rb_mapping_info_type), MODEM_MEM_CLIENT_WCDMA_RRC_GLOBAL_CRIT);
  WRRC_MSG1_HIGH("RRC_DEBUG: Size allocated for hanging RB %d",MAX_HANGING_RB_MAPPING_INFO*sizeof(rb_mapping_info_type));
 #endif /*FEATURE_WCDMA_RRC_HEAP_TO_BSS*/
       
  if((ordered_config_ptr == NULL) || (current_config_ptr == NULL) || (transition_config.toc_ptr == NULL)
    ||(sib_events_ptr == NULL) || (rrc_ordered_hanging_rb_mapping == NULL) || 
    (rrc_current_hanging_rb_mapping == NULL))
  {
      ERR_FATAL("Dynamic initialization of config ptrs failed! 0x%x 0x%x 0x%x", 
              (uint32)ordered_config_ptr, (uint32)current_config_ptr, (uint32)transition_config.toc_ptr);
  }
 
  MSG_HIGH("Allocating config ptrs 0x%x 0x%x 0x%x", ordered_config_ptr, current_config_ptr, transition_config.toc_ptr);

  rrcllc_init_config_ptrs();

  rrcmeas_init_dynamic_ptrs();
#ifdef FEATURE_WCDMA_MAC_UL_ZI_REDUCTION
  WRRC_MSG0_HIGH("Allocating dynamic memory - for MAC");
  mac_ul_allocate_mem_ZI_opt();
#endif /* FEATURE_WCDMA_MAC_UL_ZI_REDUCTION */
/* Init the backed up sizes */
  rrcllc_init_old_ul_rlc_size_backup();
}

/*====================================================================
FUNCTION: rrcllc_free_config_ptrs()

DESCRIPTION:
  This function is called to free the allocated OC, CC and TOC pointers

DEPENDENCIES:
  None.

RETURN VALUE:
  None
  
SIDE EFFECTS:

====================================================================*/
void rrc_free_dynamic_ptrs(
  void
  )
{
  int count;
  MSG_HIGH("config ptrs %d %d %d", ordered_config_ptr, current_config_ptr, transition_config.toc_ptr);
  if ((ordered_config_ptr == NULL) || (current_config_ptr == NULL) || (transition_config.toc_ptr == NULL))
  {
    MSG_HIGH("Trying to free the config pointers that are already free! 0x%x 0x%x 0x%x", 
              (uint32)ordered_config_ptr, (uint32)current_config_ptr, (uint32)transition_config.toc_ptr);
  }


#ifdef FEATURE_WCDMA_RRC_HEAP_TO_BSS
 
  ordered_config_ptr =              NULL;
  current_config_ptr =              NULL;
  transition_config.toc_ptr =       NULL;
  rrc_ordered_hanging_rb_mapping =  NULL;
  rrc_current_hanging_rb_mapping =  NULL;

#ifdef FEATURE_WCDMA_HS_FACH
  rrc_ordered_hanging_rb =          NULL;
  rrc_current_hanging_rb =          NULL;
#endif

#else
  if(ordered_config_ptr != NULL)
  {
    modem_mem_free(ordered_config_ptr, MODEM_MEM_CLIENT_WCDMA_RRC_GLOBAL_CRIT );
    ordered_config_ptr = NULL;
  }

  if(current_config_ptr != NULL)
  {
    modem_mem_free(current_config_ptr, MODEM_MEM_CLIENT_WCDMA_RRC_GLOBAL_CRIT );
    current_config_ptr = NULL;
  }

  if(transition_config.toc_ptr != NULL)
  {
    modem_mem_free(transition_config.toc_ptr, MODEM_MEM_CLIENT_WCDMA_RRC_GLOBAL_CRIT );
    transition_config.toc_ptr = NULL;
  }

  if(rrc_ordered_hanging_rb_mapping != NULL)
  {
    modem_mem_free(rrc_ordered_hanging_rb_mapping, MODEM_MEM_CLIENT_WCDMA_RRC_GLOBAL_CRIT );
	rrc_ordered_hanging_rb_mapping = NULL;
  }

  if(rrc_current_hanging_rb_mapping != NULL)
  {
    modem_mem_free(rrc_current_hanging_rb_mapping, MODEM_MEM_CLIENT_WCDMA_RRC_GLOBAL_CRIT );
	rrc_current_hanging_rb_mapping = NULL;
  }

#ifdef FEATURE_WCDMA_HS_FACH
  if(rrc_ordered_hanging_rb != NULL)
  {
    modem_mem_free(rrc_ordered_hanging_rb, MODEM_MEM_CLIENT_WCDMA_RRC_GLOBAL_CRIT );
	rrc_ordered_hanging_rb = NULL;
  }

  if(rrc_current_hanging_rb != NULL)
  {
    modem_mem_free(rrc_current_hanging_rb, MODEM_MEM_CLIENT_WCDMA_RRC_GLOBAL_CRIT );
	rrc_current_hanging_rb = NULL;
  }
#endif
#endif /*FEATURE_WCDMA_RRC_HEAP_TO_BSS*/


  /*Calling rrc_sib_proc_events_init which in turn will call rrcsib_free_tsib
       in order to free the tempholder pointer which is used to hold MIB 
       and SIB3 temporarily before freeing sib_events_ptr.
   */
      
  rrc_sib_proc_events_init();

#ifdef FEATURE_WCDMA_RRC_HEAP_TO_BSS

  sib_events_ptr = NULL;

#else

  if(sib_events_ptr !=NULL)
  {
    modem_mem_free(sib_events_ptr, MODEM_MEM_CLIENT_WCDMA_RRC_GLOBAL_CRIT );  
    sib_events_ptr = NULL;
  }

  MSG_HIGH("Freed config ptrs 0x%x 0x%x 0x%x", ordered_config_ptr, current_config_ptr, transition_config.toc_ptr);

#endif /*FEATURE_WCDMA_RRC_HEAP_TO_BSS*/

  for( count=0; count<=SIBDB_MAX_INDEX; count++ )
  {
    rrc_sibdb_free_all_decodedsibs_for_a_cell(count);
  }

  rrcmeas_free_dynamic_ptrs();

if(!(rrc_disable_crash_debug_info_nv & NV_CRASH_DEBUG_INFO_DISABLE))
{
  rrc_crash_debug_free_ota_msgs();
}


/* L2 (MAC) will de-allocate/free the dynamic memory allocation.*/
  l2_free_mem_ZI_opt();

  rrcsibdb_free_pseudo_sib19();

}

/*====================================================================
FUNCTION: rrc_get_qchat_specific_drx_cycle_lenght()

DESCRIPTION:
  This function is called to get the QCHAT specific DRX cycle length.

DEPENDENCIES:
  None.

RETURN VALUE:
  DRX cycle length
  
SIDE EFFECTS:

====================================================================*/
uint32 rrc_get_qchat_specific_drx_cycle_length( void )
{
  uint32 drx_cycle_length = 0;

  if(rrc_qchat_drx_coeff != SYS_DRX_NOT_SPECIFIED_BY_MS)
  {
    drx_cycle_length = (uint32) (1 << rrc_qchat_drx_coeff);
  }

  WRRC_MSG1_HIGH("QCHAT_DRX: DRX cycle lenght configured by QCHAT: %d", drx_cycle_length);
  return drx_cycle_length;
}
