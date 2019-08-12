#ifndef TDSRRCCMD_H
#define TDSRRCCMD_H




/*===========================================================================
                      R R C    C O M M A N D S
 
                      MODIFIED for EXTERNAL INTERFACE ONLY

DESCRIPTION

  This module contains external definitions for the
  command queues used by RRC task. This file should be
  included by any external module sending commands to RRC's
  command queues.


Copyright (c) 2000-2009 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrccmd.h_v   1.58   18 Jul 2002 19:55:04   vkhanna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/api/tdsrrccmd.h#1 $    $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/16/12   hx      Merged to VU_MODEM_WCDMA_NIKEL_3_0_MHCI.00.00.00.00.08.04

===========================================================================*/


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "comdef.h"
#include "queue.h"
#include "tdsrrcmmif.h"
/*#ifdef FEATURE_TC*/
#include "tc_rrc.h"
/*#endif*/



/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/*--------------------------------------------------------*/
/*                 RRC COMMAND IDs                        */
/*                                                        */
/*  Command Ids for commands sent from various tasks to   */
/*  RRC are defined here. All command ids, regardless of  */
/*  which command queue is used to queue them are defined */
/*  in a single enum type given below.                    */
/*                                                        */
/*  NOTE! When you add a new command ID please make sure  */
/*  the corrosponding command structure is added to the   */
/*  structure tdsrrc_cmd_type that contains all the commands.*/
/*                                                        */
/*--------------------------------------------------------*/

/* RRC and RR command identifier base */
#define RRC_RR_CMD_ID_BASE 0x07000000 /* NOTE: This value should not be changed.
                                         The same value is also used in rrccmd.h */

typedef enum
{
  /* NOTE: When adding new commands to this enum type
  please add the command to the appropriate group based
  on the command type. Each group has been given a specific
  base number in order to assist debugging the command
  queues. */

/*--------------------------------------------------------*/
/* Command Ids of Internal RRC commands                   */
/*--------------------------------------------------------*/

  TDSRRC_INT_CMD_BASE = 0x01000000,  /* Internal commands start here */

  /* 0x01000001: Command posted by LLC to indicate that RLC downlink SDU has been received */
  TDSRRC_DOWNLINK_SDU_IND = 0x01000001,

  /* 0x01000002: Command posted from any RRC module to SIB to get all SIBs for new srv cell */
  TDSRRC_GET_ALL_SIBS_REQ = 0x01000002,

  /* 0x01000003: Command posted by LLC to indicate that RLC downlink L2 ACK has been received*/
  TDSRRC_DOWNLINK_L2ACK_IND = 0x01000003,

  /* 0x01000004: Command posted from any RRC module to LLC to request a reset of L1. In other
   * words, no physical channels are active, and L1 is waiting for an
   * TDSL1_CPHY_ACQ_REQ.
   */
  TDSRRC_RESET_L1_REQ = 0x01000004,

  /* 0x01000005: */
  TDSRRC_RESET_L1_CNF = 0x01000005,

  /* 0x01000006: Command posted from SIB module to another RRC moduel to confirm a
   * TDSRRC_GET_ALL_SIBS_REQ command.
   */
  TDSRRC_GET_ALL_SIBS_CNF = 0x01000006,

  /* 0x01000007: Command to get specific SIBs from the serving cell or a neighbor cell. */
  TDSRRC_GET_SPECIFIC_SIBS_REQ = 0x01000007,

  /* 0x01000008: Command posted from SIB module to another RRC module to confirm a
   * TDSRRC_GET_SPECIFIC_SIBS_REQ command.
   */
  TDSRRC_GET_SPECIFIC_SIBS_CNF = 0x01000008,

  /* 0x01000009: Command posted by timer when TIMER_TO_CAMP_ON is expired */
  TDSRRC_TIMER_TO_CAMP_ON_EXPIRED_IND = 0x01000009,

  /* 0x0100000A: Command posted by timer when WAIT TIME Timer is expired */
  TDSRRC_WAIT_TIMER_EXPIRED_IND = 0x0100000A,

  /* 0x0100000B: Command used to indicate that Phy Chan Failure timer is expired */
  TDSRRC_PHY_CHAN_FAILURE_TIMER_EXPIRED_IND = 0x0100000B,

  /* 0x0100000C: Command posted by timer when RRC Connection Delay Timer is expired */
  TDSRRC_RRC_CON_DELAY_TIMER_EXPIRED_IND = 0x0100000C,

  /* 0x0100000D: Command posted by timer when Timer T300 is  expired */
  TDSRRC_T300_EXPIRED_IND = 0x0100000D,

  /* : Command posted by timer when Timer T308 is  expired */
  TDSRRC_T308_EXPIRED_IND = 0x0100000E,

  /* : Command posted by timer when Timer T304 is  expired */
  TDSRRC_T304_EXPIRED_IND = 0x0100000F,

  /* : Command posted by timer when Timer T302 is  expired */
  TDSRRC_T302_EXPIRED_IND = 0x01000010,

  /* : Command posted by timer when Timer T305 is  expired */
  TDSRRC_T305_EXPIRED_IND = 0x01000011,

  /* : Command posted by timer when Timer T316 is  expired */
  TDSRRC_T316_EXPIRED_IND = 0x01000012,

  /* : Command posted by timer when Timer T317 is  expired */
  TDSRRC_T317_EXPIRED_IND = 0x01000013,

  /* : Command posted by timer when Timer T307 is  expired */
  TDSRRC_T307_EXPIRED_IND = 0x01000014,

  /* : Command posted by timer when Timer T314 is  expired */
  TDSRRC_T314_EXPIRED_IND = 0x01000015,

  /* : Command posted by timer when Timer T315 is  expired */
  TDSRRC_T315_EXPIRED_IND = 0x01000016,

  /* : Command posted by timer when RCE Timer for L2 ACK  is  expired */
  TDSRRC_TIMER_FOR_RCE_L2ACK_EXPIRED_IND = 0x01000017,

  /* : Command posted by timer when RCR Timer for L2 ACK  is  expired */
  TDSRRC_TIMER_FOR_RCR_L2ACK_EXPIRED_IND = 0x01000018,

  /* : Command posted by RCE to CSP with either frequency or System redirection */
  TDSRRC_REDIRECT_REQ = 0x01000019,

  /* : Command posted by CSP to RCE after CSP camped on to redirected frequency or
   * system
   */
  TDSRRC_REDIRECT_CNF = 0x0100001A,

  /* : Command posted to State Change Manager when RRC state changes. */
  TDSRRC_NEW_STATE_IND = 0x0100001B,

  /* : Command poster by State Change Manager to other RRC procedures
   * to notify them of the state transitions.
   */
  TDSRRC_STATE_CHANGE_IND = 0x0100001C,

  /* : Command posted to Cell Change manager requesting a Cell Change */
  TDSRRC_CELL_CHANGE_REQ = 0x0100001D,

  /* : Command posted by Cell Change Manager in response to Cell Change Req.
   * This command is posted to the procedure that requested Cell Change.
   */
  TDSRRC_CELL_CHANGE_CNF = 0x0100001E,

  /* : Command posted by Cell Change manager to inform procedures of a change
   * in cell.
   */
  TDSRRC_NEW_CELL_IND = 0x0100001F,

  /* : Command posted from any RRC module to LLC to setup/reconfig/release
   * connected mode channels.
   */
  TDSRRC_CHANNEL_CONFIG_REQ = 0x01000020,

  /* : Command posted from LLC to any RRC module to confirm a setup/reconfig/release
   * of connected mode channels.
   */
  TDSRRC_CHANNEL_CONFIG_CNF = 0x01000021,

  /* : Command used to request any one of a number of L1 or L2 commands as
   * specified in rrc_llc_cmd_type
   */
  TDSRRC_LLC_CMD_REQ = 0x01000022,

  /* 0x01000023: Command used to confirm any one of a number of L1 or L2 commands as
   * specified in rrc_llc_cmd_type
   */
  TDSRRC_LLC_CMD_CNF = 0x01000023,

  /* : Command used to confirm that CELL UPDATE procedure is completed. This will
   * be in response to RRC_CELL_UPDATE_INITAITE_REQ
   */
  TDSRRC_CELL_UPDATE_COMPLETE_CNF = 0x01000024,

  /* : Command used to initiate CELL UPDATE procedure. It includes the reason to for
   * CELL UPDATE Procedure initiation
   */
  TDSRRC_CELL_UPDATE_INITIATE_REQ = 0x01000025,

  /* : Command used to inform SIB procedure that SIBs modified. */
  TDSRRC_UPDATE_SIBS_IND = 0x01000026,

  /* : Command used to initiate UE Capability Information procedure */
  TDSRRC_INITIATE_UE_CAPABILITY_REQ = 0x01000027,

  /* : Command used by the RCE procedure to indicate CSP to exculde previous frequency
   * from the cell reselection for the specified Wait Time
   */
  TDSRRC_DISABLE_PREV_FREQ_IND = 0x01000028,

  /* : Command posted by cell selection procedure to measurement procedure
   * to read SIB11/12 and then inform L1 to start measurements
   */
  TDSRRC_CELL_SELECTED_MEAS_REQ = 0x01000029,

  /* : Command used to perform the periodically RSSI&BER report to the CM */
  TDSRRC_TIMER_FOR_RSSI_BER_EXPIRED_IND = 0x0100002A,

  /* : Command used to trigger cell selection */
  TDSRRC_INITIATE_CELL_SELECTION_IND = 0x0100002B,

  /* : Command to request deactivation of lower layers */
  TDSRRC_LOWER_LAYER_DEACT_REQ = 0x0100002C,

  /* : Confirmation for lower layer deactivation */
  TDSRRC_LOWER_LAYER_DEACT_CNF = 0x0100002D,

  /* : Command used to inform the SIB procedure that the SIB wait time has
   * expired
   */
  TDSRRC_SIB_WAIT_TIMER_EXPIRED_IND = 0x0100002E,

  /* : Command posted by SIB procedure to other procedures when
   * a SIB in the serving cell has been modified by the UTRAN
   */
  TDSRRC_SIB_CHANGE_IND = 0x0100002F,

  /* : Command to RRC LLC update C-RNTI and/or U-RNTI */
  TDSRRC_RNTI_UPDATE_REQ = 0x01000030,

  /* : Command to Confirm the updatation of C-RNTI and/or U-RNTI */
  TDSRRC_RNTI_UPDATE_CNF = 0x01000031,

  /* : Command to stop TDS mode in the lower layers */
  TDSRRC_STOP_LOWER_LAYER_REQ = 0x01000032,

  /* : Command to Confirm the stop of TDS mode */
  TDSRRC_STOP_LOWER_LAYER_CNF = 0x01000033,

  /* : Command to notify a RRC mode transition */
  TDSRRC_MODE_CHANGE_IND = 0x01000034,

  /* : Command to RRC LLC for cell reselection */
  TDSRRC_CELL_RESELECTION_REQ = 0x01000035,

  /* : Command to confirm the reselection request */
  TDSRRC_CELL_RESELECTION_CNF = 0x01000036,

  /* : Command to give the SIB7 update status. Posted by
   * the SIB procedure to other RRC procedures
   */
  TDSRRC_SIB7_UPDATE_STATUS_IND = 0x01000037,

  /* : Command to indicate that the SIB7 expiration
   * timer has expired.
   */
  TDSRRC_SIB7_EXP_TIMER_EXPIRED_IND = 0x01000038,

  /* : Command from LLC to indcate end of reconfig procedure,
   * and SMC can continue security configuration
   */
  TDSRRC_RECONFIG_COMPLETE_IND = 0x01000039,

  /* : Command from LLC to indcate configuration of lower layers
   * with ciphering information passed in ciph_config_req
   */
  TDSRRC_CIPH_CONFIG_REQ = 0x0100003A,

  /* : Command from LLC to indcate configuration of lower layers
   * with ciphering information passed in ciph_config_req
   */
  TDSRRC_CIPH_CONFIG_CNF = 0x0100003B,

  /* : Command to be send from SMC to LLC to abort ongoing
   * ciphering configuraiton at RLC and revert to old set of keys.
   * NOTE: There is no CNF associated with this command.
   */
  TDSRRC_ABORT_CIPH_CONFIG_REQ = 0x0100003C,

  /* : Command from Timer to indicate that the timer to ignore
   * BCCH Modification info has expired
   */
  TDSRRC_IGNORE_BCCH_MOD_TIMER_EXPIRED_IND = 0x0100003D,

  /* : Command from Timer to indicate that the timer for cell
   * selection going from DCH->FACH has expired
   */
  TDSRRC_DCH_FACH_CELL_SELECTION_TIMER_EXPIRED_IND = 0x0100003E,

  /* : Command from Procedure to SCRR to release Signalling Connections */
  TDSRRC_SIG_CONN_RELEASE_REQ = 0x0100003F,

  /* : Command from SCRR to Procedure confirming that Signalling Connections
   * are released
   */
  TDSRRC_SIG_CONN_RELEASE_CNF = 0x01000040,

  /* : */
  TDSRRC_CHANGE_MODE_REQ = 0x01000041,

  /* : */
  TDSRRC_CHANGE_MODE_CNF = 0x01000042,

  /* : */
  TDSRRC_NAS_DATA_TRANSFER_IND = 0x01000043,

  /* : */
  TDSRRC_REVERT_TO_UTRAN_CONFIG_REQ = 0x01000044,

  /* : */
  TDSRRC_REVERT_TO_UTRAN_CONFIG_CNF = 0x01000045,

  /* : */
  TDSRRC_DELETE_MEAS_REQ = 0x01000046,

  /* : */
  TDSRRC_COMPRESSED_MODE_IND = 0x01000047,

  /* : */
  TDSRRC_CSP_ACQ_REQ = 0x01000048,

  /* : */
  TDSRRC_CSP_ACQ_CNF = 0x01000049,

  /* : */
  TDSRRC_HO_CIPH_REQ = 0x0100004A,

  /* : */
  TDSRRC_HO_CIPH_CNF = 0x0100004B,

  /* : Command from Timer to indicate that the timer for detection
   * of L1 deadlock condition has expired.
   */
  TDSRRCTMR_L1_DEADLOCK_DETECT_TIMER_EXPIRED_IND = 0x0100004C,

  /* : Command from Timer to indicate that the timer to delay ERR_FATAL
   * for CPHY_SETUP_CNF or CPHY_CELL_TRANSITION_CNF has expired
   */
  TDSRRCTMR_DELAY_ERR_FATAL_TIMER_EXPIRED_IND = 0x0100004D,

  /* : IND to inform UDT procedure that SMC has gotten L2 ACK and it is safe to
   * transmit Uplink Messages
   */
  TDSRRC_SMC_DDT_L2_ACK_IND = 0x0100004E,

  /* : Command from TMR to indicated that the timer for cell
   * selection from DCH to CELL_PCH or URA_PCH has expired
   */
  TDSRRC_DCH_PCH_CELL_SELECTION_TIMER_EXPIRED_IND = 0x0100004F,

  /* : Req/CNF between Reconfig-Procedures and SMC to trigger re-establishment
   * of RLC radio bearers
   */
  TDSRRC_RE_EST_RLC_FOR_SRNS_REQ = 0x01000050,

  /* : */
  TDSRRC_RE_EST_RLC_FOR_SRNS_CNF = 0x01000051,

  /* : Req/CNF between SMC-LLC to re-establish RLC radio bearers
   */
  TDSRRC_LLC_RE_EST_RLC_REQ = 0x01000052,

  /* : */
  TDSRRC_LLC_RE_EST_RLC_CNF = 0x01000053,

  /* : */
  TDSRRC_ERR_FATAL_IND = 0x01000054,

  /* : */
  TDSRRC_T3174_EXPIRED_IND = 0x01000055,

  /* : */
  TDSRRC_CSP_CAMP_REQ = 0x01000056,

  /* : */
  TDSRRC_CSP_CAMP_CNF = 0x01000057,

  /* : */
  TDSRRC_CONN_SETUP_IND = 0x01000058,

  /* : Command used to initiate URA UPDATE procedure. It includes the reason to for
   * URA UPDATE Procedure initiation
   */
  TDSRRC_URA_UPDATE_INITIATE_REQ = 0x01000059,

  /* : Command used to confirm that URA UPDATE procedure is completed. This will
   * be in response to RRC_URA_UPDATE_INITAITE_REQ
   */
  TDSRRC_URA_UPDATE_COMPLETE_CNF = 0x0100005A,

  /* : Command used to indicate to CSP that full frequency scan can be initiated */
  TDSRRC_FREQ_SCAN_TIMER_EXPIRED_IND = 0x0100005B,

  /* : Command used to indicate that OOS timer is expired */
  TDSRRC_OOS_TIMER_EXPIRED_IND = 0x0100005C,

  /* : Command used to indicate that Deep Sleep no service timer has expired */
  TDSRRC_DEEP_SLEEP_NO_SVC_TIMER_EXPIRED_IND = 0x0100005D,

  /* : Command used to indicate to CSP that TDS has been resumed (by CHO) */
  RRC_CSP_TDS_RESUMED_IND = 0x0100005E,

  /* : Command used to indicate that BPLMN 2G->3G search timer has expired */
  TDSRRC_BPLMN_SRCH_TIMER_EXPIRED_IND = 0x0100005F,

  /* : Command used to indicate that BPLMN 2G->3G full scan timer has expired */
  TDSRRC_BPLMN_FREQ_SCAN_TIMER_EXPIRED_IND = 0x01000060,

  /* : Command used to indicate that the wait timer started for the last camped
   * frequency during inter frequency redirection has expired.
   */
  TDSRRC_FREQ_REDIRECT_WAIT_TIMER_EXPIRED_IND = 0x01000061,

  /* : Command used to abort interrat operation when a higher priority request
   * needs to be processed
   */
  TDSRRC_IRAT_ABORT_IND = 0x01000062,

  /* : Command used to indicate that the 30s specific PLMN search timer when
   * UE is OOS in Connected Mode has expired
   */
  TDSRRC_CONN_MODE_OOS_TIMER_EXPIRED_IND = 0x01000063,

  /* : Command used to indicate that the inactivity timer when
   * has expired
   */
   TDSRRC_INACTIVITY_TIMER_EXPIRED_IND = 0x01000064,

  /* : Command used for indicating the completion of IDT procedure for a particular domain
   * This is used by release procedure
   */
  TDSRRC_IDT_COMPLETED_FOR_DOMAIN = 0x01000065,

  /* : Command used to indicate that BPLMN 3G->3G search timer has expired */
  RRC_TD2TD_BPLMN_DRX_SRCH_TIMER_EXPIRED_IND = 0x01000066,

  /* : Command used to indicate that MAX BPLMN 3G->3G search timer allowed has expired */
  TDSRRC_BPLMN_GUARD_SRCH_TIMER_EXPIRED_IND = 0x01000067,

  /* : Command used to indicate that BPLMN 3G->3G = ,2G search can be started */
  TDSRRC_BPLMN_SEARCH_RESUME_IND = 0x01000068,

  /* : Command sent by other procedures to CSP to suspend the BPLMN search */
  TDSRRC_BPLMN_SUSPEND_REQ = 0x01000069,

  /* : Command sent by CSP to the procedure that sent suspend request for suspending BPLMN search */
  TDSRRC_BPLMN_SUSPEND_CNF = 0x0100006A,

  /* : command sent from MCM to Meas and UECI on TDSRRC_CHANGE_MODE_IND */
  TDSRRC_MODE_STATUS_IND = 0x0100006B,

  /* : Command used to indicate for updating six hrs sib timer*/
  TDSRRC_SIX_HRS_SIB_TIMER_UPDATE_IND = 0x0100006C,

  /* : Command used to indicate that Uniform OOS search timer has expired */
  TDSRRC_UNIFORM_OOS_SEARCH_TIMER_EXPIRED_IND = 0x0100006D,

  /* : Command used to put UE into deep sleep when UE is OOS in Connected Mode
   */
  TDSRRC_CONN_MODE_DEEP_SLEEP_REQ = 0x0100006E,

  /* : Command used to wakeup UE from deep sleep when UE is OOS in Connected Mode
   */
  TDSRRC_CONN_MODE_DEEP_SLEEP_WAKEUP_REQ = 0x0100006F,

  /* : Command used to indicate that the initial Tmins no deep sleep
   * timer has expired. The timer is started when UE goes OOS in Connected Mode.
   */
  TDSRRC_CONN_MODE_OOS_INITIAL_NO_DS_TIMER_EXPIRED_IND = 0x01000070,

  /* : Command used to indicate that the deep sleep timer has expired and UE
   * should now wake up and search for service again
   */
  TDSRRC_CONN_MODE_OOS_DS_TIMER_EXPIRED_IND = 0x01000071,

  /* :  */
  TDSRRC_RB_PROC_COMPL_IND = 0x01000072,

  /* : Command used to indicate SIB change Gaurd timer expired*/
  TDSRRC_SIB_CHANGE_GUARD_TIMER_EXPIRED_IND = 0x01000073,

  /* : this command is sent by LLC to CSP after receiving START_CNF
   * from L1 in Connected Mode
   */
  TDSRRC_CONN_MODE_DEEP_SLEEP_WAKEUP_CNF = 0x01000074,

  /*: Command used to indicate BCCH Modification timer expiry*/
  TDSRRC_BCCH_MODIFICATION_TIMER_EXPIRED_IND = 0x01000075,

  /* : Command used to indicate that  Manual PLMN search full scan timer has expired */
  TDSRRC_MANUAL_PLMN_FREQ_SCAN_TIMER_EXPIRED_IND = 0x01000076,

  /* : Command from timer proc to indicate that the timer for cell
   * selection going from FACH->FACH/PCH has expired
   */
  TDSRRC_FACH_TO_FACH_PCH_CELL_SELECTION_TIMER_EXPIRED_IND = 0x01000077,

  /* : Command posted by RRC when GSTK queries RRC for
   * Network Measurement information
   */
  TDSRRC_GSTK_UTRAN_NMR_INFO_REQ = 0x01000080,

  /* : Command from timer proc to indicate that the timer for cell
   * selection on GtoW redirection has expired.
   */
  RRC_G2TD_REDIRECTION_WAIT_TIMER_EXPIRED_IND = 0x01000081,

  /* : Command from timer proc to indicate the guard timer for 
   * abort W2G BPLMN has expired.
   */
  RRC_TD2G_ABORT_GUARD_TIMER_EXPIRED_IND = 0x01000082,

  /* : Command from CSP proc to indicate MCM can go ahead with 
   * stop tds request
   */
  TDSRRC_CONTINUE_STOP_MODE_REQ = 0x01000083,

  /*0x01000084: Command for expiry of timer T320. T320 is the wait timer for CU. 
        UTRAN may use this timer to queue the UE in Cell_PCH or URA_PCH state.*/
  TDSRRC_T320_EXPIRED_IND = 0x01000084,

  /*0x01000085: Command for requesting "Change of UE Capability" thru COUEC proc*/
  TDSRRC_COUEC_REQ = 0x01000085,
  
  /*0x01000086: Command for confirmation of "Change of UE Cap." request*/
  TDSRRC_COUEC_CNF = 0x01000086,

  /*0x01000087: Command for expiry of COUEC timer*/
  TDSRRC_TIMER_COUEC_EXPIRED_IND = 0x01000087,

  /*0x01000088 : Command used to indicate T319 expiry*/
  TDSRRC_T319_EXPIRED_IND= 0x01000088,

  /*0x01000089: Command used to indicate that Dedicated priority validity timer expired*/
  TDSRRC_DED_PRI_VALDITY_TIMER_EXPIRED_IND = 0x01000089,

  /*0x01000090: Command used to indicate that Dedicated priority info has changed*/
  TDSRRC_DED_PRI_CHANGE_IND = 0x0100008A,

  TDSRRC_WRM_PREEMPT_IND = 0x0100008B,

  TDSRRC_NO_RESOURCE_AVAILABLE_FROM_L1 = 0x0100008C,

  /*0x0100008B : Command used to indicate T323 expiry*/
  TDSRRC_T323_EXPIRED_IND = 0x0100008D,
  
  /* Command used to indicate that EOOS full scan timer has expired */
  TDSRRC_EOOS_FULL_SCAN_TIMER_EXPIRED_IND = 0x0100008E,

  /* Command used to indicate WRM Unlock_By timer expired */
  TDSRRC_DUAL_SIM_WRM_UNLOCK_BY_TIMER_EXP_IND  = 0x0100008F,

  TDSRRC_QCHAT_FA_RRC_TIMER_EXPIRED_IND = 0x01000090,

  TDSRRC_IHO_OPEN_DOMAIN_IND = 0x01000091,

  TDSRRC_LTE_TO_TDSCDMA_SRCH_TIMER_EXPIRY_IND = 0x01000092,
  
  TDSRRC_INTERNAL_CSFB_ABORT_REQ = 0x01000093,

  // CR 352866 begin
  /* 0x01000094: Command posted by timer when TDSRRCTMR_RCE_WAIT_RESELCT_TIMER is expired */
  TDSRRC_TIMER_TO_RCE_WAIT_RESELCT_EXPIRED_IND = 0x01000094,
  // CR 352866 end

  RRC_G2TD_REDIRECTION_LFS_TIMER_EXPIRED_IND = 0x01000095,

  /* CR 383108. This command doesn't contain payload */
  TDSRRC_CELL_RESELECTION_FAILURE_IND = 0x01000096,

  // CR 430034 RAU Reject workaround
  TDSRRC_DELAY_OOS_RLF_RPT_TIMER_EXPIRED_IND = 0x01000097,

  /* CR 416785 optimization:
  *  IND to inform DDT procedure that SMC has cleared SRNS RLC re-est
  *  and it is safe to pass DDT to upper layer
  *  and tx the corresponding Uplink Messages */
  TDSRRC_SMC_SRNS_RLC_RE_EST_CMP = 0x01000098,

  /*When SCRI cannot be sent in time*/
  TDSRRC_SOFT_ABORT_GUARD_TIMER_EXPIRED_IND = 0x01000099,

  /*Timer to report partial PLMN LIST to NAS during Manual PLMN Search*/ 
  TDSRRC_BPLMN_PARTIAL_REPORT_TIMER_EXPIRED_IND = 0x0100009A,
  TDSRRC_BLOCK_CS_RESEL_NAS_API_TIMER_EXPIRED_IND = 0X0100009B,
  TDSRRC_BLOCK_PS_RESEL_NAS_API_TIMER_EXPIRED_IND = 0X0100009C,
  TDSRRC_DA_BACKOFF_TIMER_EXPIRED_IND = 0X0100009D,

  TDSRRCTMR_IGNORE_NON_EXIST_MCM_TIMER_EXPIRED_IND = 0X0100009E,

  TDSRRCTMR_ABORT_RESEL_REQ_TO_GRR_TIMER_EXPIRED_IND = 0X0100009F,

  /* : Command used to indicate that MAX TD2TD manual BPLMN 3G->3G search timer allowed has expired */
  TDSRRC_TD2TD_MANUAL_BPLMN_GUARD_SRCH_TIMER_EXPIRED_IND = 0x010000A0,
#ifdef FEATURE_TDSCDMA_G2T_TUNE_AWAY
  TDSRRC_SIB_STOP_PARTITION_TMR_EXPIRED_IND = 0x010000A1,

  TDSRRC_SIB_MASK_INVERT_TMR_EXPIRED_IND = 0x010000A2,

  TDSRRC_SIB_EMPTY_MASK_TMR_EXPIRED_IND = 0x010000A3,
#endif

  /* internal cmd when API tdsrrc_mcfg_refresh_cb is called for MCFG refresh event */
#ifdef FEATURE_MODEM_CONFIG_REFRESH
  TDSRRC_MCFG_REFRESH_IND = 0x010000A4,
#endif

  TDSRRC_HOLD_PS_TIMER_EXPIRED_IND = 0x010000A5,

  TDSRRC_QSH_CB_IND = 0x010000A6,

#ifdef FEATURE_TDSCDMA_FREE_FLOATING_TASK
/*--------------------------------------------------------*/
/* Common command for the Free floating                   */
/*                                                        */
/* Please keep in mind that the command may be put into   */
/* the L1 FF queue for fast process if needed             */
/*--------------------------------------------------------*/

  TDSRRC_FF_INTERNAL_QUEUE_BASE = 0x01E00000, /*Internal CMD posted by RRC for FF*/
  TDSRRC_RLC_UL_DATA_IND = 0x01E00001,        /*Internal CMD posted by RLC call back function*/
  TDSRRC_REG_SRVING_CELL_CHG_IND = 0x01E00002, /*FF equivalent for tdsrrc_register_serving_cell_change_ind_cb*/
  TDSRRC_DEREG_SRVING_CELL_CHG_IND = 0x01E00003, /*FF equivalent for tdsrrc_deregister_serving_cell_change_ind_cb*/
    
#endif


  /* For QTF test */
  TDSRRC_QTF_CMD_BASE = 0x01F00000,
  TDSRRC_QTF_RRCINT_REQ = 0x01F00001,
  TDSRRC_QTF_RRCDATA_REQ = 0x01F00002,



/*--------------------------------------------------------*/
/* Command Ids of RRC commands Sent by L1                */
/*--------------------------------------------------------*/

  TDSRRC_L1_CMD_BASE = 0x02000000,  /* L1 commands start here */
  TDSRRC_CMD_ID_L1_PLACEHLDER_BEGIN = TDSRRC_L1_CMD_BASE,
  /*  */
  TDSRRC_CPHY_ACQ_CNF = 0x02000001,

  /*  */
  TDSRRC_CPHY_SETUP_CNF = 0x02000002,

  /*  */
  TDSRRC_CPHY_ERROR_IND = 0x02000003,

  /*  */
  TDSRRC_CPHY_IDLE_CNF = 0x02000004,

  /*  */
  TDSRRC_CPHY_CELL_SELECTION_CNF = 0x02000005,

  /*  */
  TDSRRC_CPHY_CELL_RESELECTION_IND = 0x02000006,

  /*  */
  TDSRRC_CPHY_MEASUREMENT_IND = 0x02000008,

  /*  */
  TDSRRC_CPHY_PHYCHAN_ESTABLISHED_IND = 0x0200000B,

  /*  */
  TDSRRC_CPHY_IN_SYNC_IND = 0x0200000C,

  /*  */
  TDSRRC_CPHY_OUT_OF_SYNC_IND = 0x0200000D,

  /*  */
  TDSRRC_CPHY_RL_FAILURE_IND = 0x0200000E,

  /*  */
  TDSRRC_CPHY_CELL_TRANSITION_CNF = 0x02000011,

  /*  */
  TDSRRC_CPHY_FREQ_SCAN_CNF = 0x02000012,

  /*  */
  TDSRRC_CPHY_DRX_CNF = 0x02000013,

  /*  */
  TDSRRC_CPHY_OUT_OF_SERVICE_AREA_IND = 0x02000014,

  /*  */
  TDSRRC_CPHY_DEACTIVATE_CNF = 0x02000015,

  /*  */
  TDSRRC_CPHY_MEAS_CTRL_CNF = 0x02000016,

  /*  */
  TDSRRC_CPHY_ACT_CANCEL_CNF = 0x02000017,

  /*  */
  RRC_CPHY_START_TDS_MODE_CNF = 0x02000018,

  /*  */
  RRC_CPHY_STOP_TDS_MODE_CNF = 0x02000019,

  /*  */
  RRC_CPHY_SUSPEND_TDS_MODE_CNF = 0x0200001A,

  /*  */
  TDSRRC_CPHY_RESUME_TDS_MODE_CNF = 0x0200001B,

  /*  */
  TDSRRC_CPHY_COMPRESSED_MODE_IND = 0x0200001C,

  /*  */
  TDSRRC_CPHY_DL_WEAK_IND = 0x0200001D,

  /* PLEASE ADD ALL NEW L1 Commands HERE */
  TDSRRC_CPHY_ACTIVATION_TIME_IND = 0x0200001E,
  
  
#ifdef FEATURE_TDSCDMA_FREE_FLOATING_TASK
/*--------------------------------------------------------*/
/* Common command for all the Free floating sent by L1    */   
/* May need to be moved to tdsl1rrcif.h TBD*/
/*--------------------------------------------------------*/

  /*0x0200001F reserved*/

  TDSRRC_CPHY_DATA_FOR_UI_DISP_CNF = 0x02000020,

  TDSRRC_CPHY_RSSI_CNF = 0x02000021,

  TDSRRC_CPHY_CHECK_S_CRIT_CNF = 0x02000022,

  TDSRRC_CPHY_CHECK_SLEEP_REQ = 0x02000023,

  TDSRRC_CPHY_GET_IMM_MEAS_RSP = 0x02000024,

  TDSRRC_CPHY_UTRAN_NMR_INFO_CNF = 0x02000025,

  TDSRRC_CPHY_ADDITIONAL_MEAS_DATA_RSP = 0x02000026,

  TDSRRC_CPHY_NUDGE_FOR_SFN_MISS_MATCH_REQ = 0x02000027,

  TDSRRC_CPHY_MODEM_STATISTICS_RSP = 0x02000028,

//  TDSCPHY_RSSI_CNF = 0x02000029,
#endif
  
#ifdef FEATURE_TDSCDMA_DSDS_QTA /*FEATURE_QTA*/
  /* TDSL1 has started QTA*/
  TDSRRC_CPHY_START_QTA_CNF = 0x02000030,
  /* TDSL1 has come out of QTA*/
  TDSRRC_CPHY_STOP_QTA_CNF = 0x02000031,
#endif
#ifdef FEATURE_TDSCDMA_FATAL_RECOVERY
  TDSRRC_CPHY_HW_ERROR_IND = 0x02000032,
#endif
#ifdef FEATURE_TDSCDMA_G2T_TUNE_AWAY
  TDSRRC_CPHY_G2T_MODE_IND = 0x02000033,
#endif

  TDSRRC_L1_CMD_MAX,    /* added for DSDS */

  TDSRRC_CMD_ID_L1_PLACEHLDER_END = 0x02FFFFFF,


/*--------------------------------------------------------*/
/* Command Ids of RRC commands Sent by MAC                */
/*--------------------------------------------------------*/

  TDSRRC_MAC_CMD_BASE = 0x03000000,  /* MAC commands start here */
  TDSRRC_CMD_ID_MAC_PLACEHLDER_BEGIN = TDSRRC_MAC_CMD_BASE,
  TDSRRC_CMAC_UL_TRAFFIC_IND = 0x03000001,
  TDSRRC_CMAC_STATUS_IND = 0x03000002,
  TDSRRC_CMAC_MEAS_CTRL_CNF = 0x03000003,
  TDSRRC_CMAC_UL_TFC_SUBSET_CFG_CNF = 0x03000004,
  /* PLEASE ADD ALL NEW MAC Commands HERE */
  TDSRRC_CMD_ID_MAC_PLACEHLDER_END = 0x03FFFFFF,


/*--------------------------------------------------------*/
/* Command Ids of RRC commands Sent by RLC                */
/*--------------------------------------------------------*/

  TDSRRC_RLC_CMD_BASE = 0x04000000,  /* RLC commands start here */
  TDSRRC_CMD_ID_RLC_PLACEHLDER_BEGIN = TDSRRC_RLC_CMD_BASE,
  TDSRRC_CRLC_STATUS_IND= 0x04000001,
  TDSRRC_CRLC_DL_TM_CONFIG_CNF= 0x04000002,
  TDSRRC_CRLC_UL_TM_CONFIG_CNF= 0x04000003,
  TDSRRC_CRLC_DL_UM_CONFIG_CNF= 0x04000004,
  TDSRRC_CRLC_UL_UM_CONFIG_CNF= 0x04000005,
  TDSRRC_CRLC_AM_CONFIG_CNF= 0x04000006,
  TDSRRC_CRLC_RELEASE_ALL_CNF= 0x04000007,
  /* PLEASE ADD ALL NEW RLC Commands HERE */
  TDSRRC_CMD_ID_RLC_PLACEHLDER_END = 0x04FFFFFF,


/*--------------------------------------------------------*/
/* Command Ids of RRC commands Sent by MM, TC and RR      */
/* MM and TC Commands are in tdsrrccmd.h                  */
/* RR Commands are in tdsrrc_rr_if.h                      */
/*--------------------------------------------------------*/
/* 0x05000000 ~ 0x06FFFFFF Reserved for MM and TC Command */
/* 0x07000000 ~ 0x07FFFFFF Reserved for RR Command */


/*--------------------------------------------------------*/
/* Command Ids of RRC commands Sent by MM                 */
/*--------------------------------------------------------*/

  TDSRRC_MM_CMD_BASE = 0x05000000,  /* MM commands start here */
  TDSRRC_CMD_ID_MM_PLACEHLDER_BEGIN = TDSRRC_MM_CMD_BASE,

  TDSRRC_SERVICE_REQ = 0x05000001,

  TDSRRC_EST_REQ = 0x05000002,

  TDSRRC_DATA_REQ = 0x05000003,

  TDSRRC_OPEN_SESSION_REQ =0x05000004,

  TDSRRC_CLOSE_SESSION_REQ = 0x05000005,

  TDSRRC_ABORT_REQ = 0x05000006,

  TDSRRC_PLMN_LIST_REQ = 0x05000007,

  TDSRRC_ACT_REQ = 0x05000008,

  TDSRRC_DEACT_REQ = 0x05000009,

  RRC_STOP_TDS_MODE_REQ = 0x0500000A,

  TDSRRC_FORBIDDEN_LAI_LIST_UPDATE_REQ = 0x0500000B,

  TDSRRC_INVALIDATE_SIM_DATA_REQ = 0x0500000C,

  TDSRRC_SIM_INSERTED_REQ = 0x0500000D,

  TDSRRC_SIM_UPDATE_REQ = 0x0500000E,

  TDSRRC_ACTIVATION_RSP = 0x0500000F,

  TDSRRC_CHANGE_MODE_IND = 0x05000010,

  TDSRRC_MODE_CHANGE_REQ = 0x05000011,

  TDSRRC_EQ_PLMN_LIST_CHANGE_IND = 0x05000012,

  TDSRRC_NW_SEL_MODE_RESET_IND = 0x05000013,

  TDSRRC_BPLMN_SEARCH_ABORT_REQ = 0x05000014,    /* Added for WTOW/WTOG BPLMN SEARCH ABORT */

  TDSRRC_EXIT_FROM_PWR_SAVE = 0x05000015,

  TDSRRC_DS_STATUS_CHANGE_IND = 0x05000016,

  TDSRRC_FAST_DORMANCY_REQ = 0x05000017,       /*Added for fast dormancy request trigger*/
  /* 0x05000018 */
  TDSRRC_DS_TUNEAWAY_STATUS_CHANGE_IND = 0x05000018,

  TDSRRC_CSFB_CALL_STATUS_IND = 0x05000019, /*CSFB call status indication*/

  TDSRRC_LTE_RESELECTION_ALLOWED_STATUS_IND = 0x0500001A,

  TDSRRC_LAI_REJECT_LIST_IND = 0x0500001B,
 
  TDSRRC_TUNEAWAY_ALLOWED_IND = 0x0500001C,

  TDSRRC_UE_MODE_IND = 0x0500001D,

  TDSRRC_MODE_PREF_CHANGE_IND = 0x0500001E,

  TDSRRC_MULTIMODE_SUBS_CHGD_IND = 0x0500001F,

  TDSRRC_PSEUDO_LTE_LIST_UPDATE_IND = 0x05000020,
#ifdef FEATURE_CMCC_SECURE_CALL  
  TDSRRC_SECURE_CALL_IND = 0x05000021,
#endif  
  /* PLEASE ADD ALL NEW MM Commands HERE */

/*--------------------------------------------------------*/
/* Command Ids of RRC commands Sent by TC                 */
/*--------------------------------------------------------*/

  TDSRRC_TC_CMD_BASE = 0x06000000,  /* TC commands start here */
  TDSRRC_TC_MODE_CHANGE = 0x06000001,
  TDSRRC_TC_TEST_LOOPBACK_IND = 0x06000002,
  /* PLEASE ADD ALL NEW TC Commands HERE */


    /**************************************************************/
  /******** RR Procedure ID's received by TDS RRC ***************/
  /**************************************************************/

/**********************************************************************/
  /* RR to RRC command base. Note: starting value is RRC_RR_CMD_ID_BASE */
  TDSRR_TO_RRC_BASE = RRC_RR_CMD_ID_BASE,
  /* Add new RR to RRC commands below */
  /**********************************************************************/

  /****** TD -> RR RESELECTION ************/
  TDSRR_INTERRAT_RESELECTION_REJ,
  TDSRR_INTERRAT_RESELECTION_ABORT_CNF,

  /****** TD -> RR HANDOVER ************/
  TDSRR_INTERRAT_HANDOVER_CNF,
  TDSRR_INTERRAT_HANDOVER_ABORT_CNF,

  /****** TD -> RR CC ORDER ************/
  TDSRR_INTERRAT_CC_ORDER_REJ,
  TDSRR_INTERRAT_CC_ORDER_CNF,
  TDSRR_INTERRAT_CC_ORDER_ABORT_CNF,
  TDSRRC_GRR_INTERRAT_PROC_ABORT_IND,

  /****** TD -> RR PLMN SEARCH ************/
  TDSRR_INTERRAT_PLMN_SRCH_CNF,
  TDSRR_INTERRAT_PLMN_SRCH_ABORT_CNF,

  /****** TD -> RR REDIRECTION ************/
  TDSRR_INTERRAT_REDIRECT_REJ,
  TDSRR_INTERRAT_REDIRECT_ABORT_CNF,

  /****** TD -> RR Stop Mode ************/
  TDSRR_STOP_GSM_CNF,

  /**************************************************************/
  /******** TDS RRC Procedure ID's received by TDS RRC **********/
  /**************************************************************/

  /****** RR -> TD RESELECTION ************/
  TDSRRC_INTERRAT_RESELECTION_REQ,
  TDSRRC_INTERRAT_RESELECTION_ABORT_REQ,

  /****** RR -> TD HANDOVER ************/  
  TDSRRC_INTERRAT_HANDOVER_REQ,
  TDSRRC_INTERRAT_HANDOVER_ABORT_REQ,
  TDSRRC_NAS_DATA_IND,

  /****** RR -> TD CC ORDER ************/ 
  TDSRRC_INTERRAT_CC_ORDER_REQ,
  TDSRRC_INTERRAT_CC_ORDER_ABORT_REQ,

  /****** RR -> TD PLMN SEARCH ************/
  TDSRRC_INTERRAT_PLMN_SRCH_REQ,
  TDSRRC_INTERRAT_PLMN_SRCH_ABORT_REQ,

  /****** RR -> TD REDIRECTION ************/
  TDSRRC_INTERRAT_REDIRECT_REQ,
  TDSRRC_INTERRAT_REDIRECT_ABORT_REQ,

  /****** RR -> TD Stop Mode ************/
  TDSRRC_STOP_TDS_REQ,

  /****** RR -> TD Cipher Sync ************/
  TDSRR_CIPHER_SYNC,

  /****** RR -> TD Dedicated Priorities ************/
  TDSRRC_DEDICATED_PRIORITIES_REQ,
  TDSRRC_CLEAR_DEDICATED_PRIORITIES_IND,

  /****** TD -> RR Dedicated Priorities ************/
  TDSRR_DEDICATED_PRIORITIES_RSP,

  TDSRRC_CMD_ID_RR_PLACEHLDER_END = 0x07FFFFFF,


/*--------------------------------------------------------*/
/* Command Ids of RRC commands Sent by LSM */
/*--------------------------------------------------------*/

  TDSRRC_LCS_CMD_BASE = 0x08000000,  /* LCS commands start here */
  TDSRRC_MEASUREMENT_IND, /*0x08000001  A-GPS Meas Report */
#ifdef FEATURE_TDSCDMA_GPS_CELL_ID_READING
  TDSRRC_GPS_CELL_ID_REQUEST, /*0x08000002*/
#endif /*FEATURE_TDSCDMA_GPS_CELL_ID_READING*/



/*--------------------------------------------------------*/
/* Common command for all the UMIDs sent via MSGR         */
/*--------------------------------------------------------*/
  TDSRRC_MSGR_CMD = 0x0C000000,
  

/*--------------------------------------------------------*/
/* Command Ids range of RRC commands Sent by WCDMA RRC    */
/* Definitions are in tdsrrc_rrc_if.h                     */
/*--------------------------------------------------------*/

  TDSRRC_CMD_ID_WCDMA_RRC_PLACEHLDER_BEGIN = 0x0E000000,
  TDSRRC_CMD_ID_WCDMA_RRC_PLACEHLDER_END = 0x0EFFFFFF,


/*--------------------------------------------------------*/
/* End of RRC command ids                                 */
/*--------------------------------------------------------*/
  TDSRRC_MAX_CMDS
}tdsrrc_cmd_e_type;

/*All current Command IDs will be shared with WCDMA in rrccmd.h*/

/*
typedef enum          
{                     
                      
}tdsrrc_mm_cmd_e_type;
*/

/*
typedef enum          
{                     
                      
}tdsrrc_tc_cmd_e_type;
*/



/*--------------------------------------------------------*/
/*                 MM COMMAND BUFFER                     */
/*                                                        */
/*  The Command buffer used for all MM command queues is */
/*  defined here. The command buffer contains a MM       */
/*  command header and a union of all possible commands   */
/*  for any RRC command queue                             */
/*--------------------------------------------------------*/


/*The following tdsrrc_mm_cmd_data_u_type is a for 
  tdscdma specific interface to MM*/

/*All types defined here should be also included in the 
  tdsrrc_cmd_data_u_type*/

/*For MM using such types, RRC needs convert the type from 
  tdsrrc_mm_cmd_type to tdsrrc_cmd_type*/
typedef union
{

  /* Union of individual command structures */
  /*--------------------------------------------------------*/
  /* Command Structure for RRC commands Sent by MM          */
  /*--------------------------------------------------------*/
  rrc_est_req_type           rrc_est_req;
  rrc_service_req_type       rrc_service_req;
  rrc_sim_inserted_req_type  rrc_sim_inserted_req;
  rrc_sim_update_req_type    rrc_sim_update_req;
  rrc_activation_rsp_type    activation_rsp;
  rrc_data_req_type          data_req;
  rrc_open_session_req_type  open_sess_req;
  rrc_close_session_req_type close_sess_req;
  rrc_plmn_list_req_type     plmn_list_req;
  rrc_forbid_lai_list_update_req_type forbid_lai_list_update_req;
  rrc_stop_wcdma_mode_req_type stop_wcdma_mode_req;

  rrc_change_mode_ind_type   rrc_change_mode_ind;
  rrc_abort_req_type         rrc_abort_req;
  rrc_deact_req              deact_req;
  rrc_nw_sel_mode_reset_ind_type    rrc_nw_sel_mode_reset_ind;

  /*Mainline the DUAL_SIM*/
/*#ifdef FEATURE_DUAL_SIM*/
  rrc_ds_status_change_ind_type  rrc_ds_status_change_ind;

  rrc_subscription_id_type rrc_subscription_id;
  rrc_ds_tuneaway_status_change_ind_type rrc_ds_tuneaway_status_change_ind;
  rrc_multimode_subs_chgd_ind_type rrc_multimode_subs_chgd_ind;
/*#endif*/

/*temporary mainined */
  rrc_fast_dormancy_req_type  rrc_fast_dormancy_req;

  rrc_csfb_call_status_ind_type rrc_csfb_call_status_ind;
  rrc_lte_reselection_allowed_status_ind_type  rrc_lte_resel_allowed_status_ind;
  rrc_lai_reject_list_ind_type      rrc_lai_reject_list_ind;
  rrc_sm_tuneaway_allowed_ind_type  rrc_sm_tuneaway_allowed_ind;

/*#ifdef FEATURE_SGLTE*/
  rrc_ue_mode_ind_type   rrc_ue_mode_ind;
/*#endif*/

  rrc_pseudo_lte_list_update_ind_type rrc_pseudo_lte_avail_update_ind;
#ifdef FEATURE_CMCC_SECURE_CALL
  tdsrrc_secure_call_ind_type  rrc_secure_call_ind;
#endif
 /* This dummy field has been added , so that both external and the 
 * RRC internal cmds have the same size for the cmd union. 
 * As the size of the command unoin is determined by the highest data type
 * present in the structure in ARM.*/
  /*Currently disabled for Q6 build*/
  /*uint64 dummyPadding;*/

}tdsrrc_mm_cmd_data_u_type;


/*For NAS using such types, RRC needs covert the type from 
  tdsrrc_tc_cmd_type to tdsrrc_cmd_type*/
typedef union
{
  /*--------------------------------------------------------*/
  /* Command Structure for RRC commands Sent by TC          */
  /*--------------------------------------------------------*/
/*#ifdef FEATURE_TC*/
  rrctc_mode_change_T        mode_change;
  rrctc_test_loopback_ind_T  test_loopback_ind;
/*#endif*/


 /* This dummy field has been added , so that both external and the 
 * RRC internal cmds have the same size for the cmd union. 
 * As the size of the command unoin is determined by the highest data type
 * present in the structure in ARM.*/
  /*Currently disabled for Q6 build*/
  /*uint64 dummyPadding;*/


}tdsrrc_tc_cmd_data_u_type;

typedef struct
{
  q_link_type   link;        
  tdsrrc_cmd_e_type cmd_id;     
  uint8         seq_num;
}tdsrrc_mm_cmd_hdr_type;

typedef struct
{
  q_link_type   link;        
  tdsrrc_cmd_e_type cmd_id;     
  uint8         seq_num;
}tdsrrc_tc_cmd_hdr_type;

typedef struct
{
  /* Command header common for MM commands*/
  tdsrrc_mm_cmd_hdr_type    cmd_hdr;  

  tdsrrc_mm_cmd_data_u_type cmd;

}tdsrrc_mm_cmd_type;

typedef struct
{
  /* Command header common for TC commands*/
  tdsrrc_tc_cmd_hdr_type    cmd_hdr;  

  tdsrrc_tc_cmd_data_u_type cmd;

}tdsrrc_tc_cmd_type;


/* =======================================================================
**                        Function Declarations
** ======================================================================= */



/*===========================================================================

FUNCTION RRC_GET_MM_CMD_BUF

DESCRIPTION

  This function returns a MM command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling tdsrrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type tdsrrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
/*tdsrrc_cmd_type *tdsrrc_get_mm_cmd_buf( void );*/
tdsrrc_mm_cmd_type *tdsrrc_get_mm_cmd_buf( void );



/*===========================================================================

FUNCTION RRC_PUT_MM_CMD

DESCRIPTION

  This function puts a command buffer into RRC's mm
  command queue. Note that the command buffer must have been
  requested through a call to get_rrc_mm_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
void tdsrrc_put_mm_cmd
(
  tdsrrc_mm_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
);



/*===========================================================================

FUNCTION RRC_PUT_TC_CMD

DESCRIPTION

  This function puts a command buffer into RRC's mm
  command queue. Note that the command buffer must have been
  requested through a call to get_rrc_tc_cmd_buf(). Also,
  The calling function must have filled all the necessary
  data including the command id before passing the buffer
  to this function.

  This function also sets the appropriate signal to the RRC
  task to indicate that a command has been placed in the
  command queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  A signal is set for the RRC task and this could cause a context
  switch.

===========================================================================*/
void tdsrrc_put_tc_cmd
(
  tdsrrc_tc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
);
/*===========================================================================

FUNCTION RRC_GET_TC_CMD_BUF

DESCRIPTION

  This function returns a TC command buffer to the calling function.
  This function dynamically allocates memory for the necessary buffer.
  Note that this buffer MUST be freed by calling tdsrrc_free_cmd_buf().

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type tdsrrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
tdsrrc_tc_cmd_type *tdsrrc_get_tc_cmd_buf( void );

/*===========================================================================

FUNCTION RRC_FREE_TC_CMD_BUF

DESCRIPTION

  This function frees a TC command buffer removed from RRC's TC
  command queue. Only commands removed from RRC's TC command queue
  can be passed into this function.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void tdsrrc_free_tc_cmd_buf
(
  tdsrrc_tc_cmd_type *cmd_buf    /* Command Buffer */
);
#endif /* RRCMD.H */

