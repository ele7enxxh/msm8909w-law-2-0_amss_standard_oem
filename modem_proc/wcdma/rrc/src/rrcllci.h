#ifndef RRCLLCI_H
#define RRCLLCI_H
/*===========================================================================

                    L3 - RRC Lower Layer Controller Interface
                    For use only by rrcllc functions.

DESCRIPTION
  This file contains function prototypes and local RRC LLC databases.

Copyright (c) 2000-2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcllci.h_v   1.25   13 Jun 2002 16:06:52   vnagpal  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcllci.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/2715    as      Made changes to trigger cphy act cancel only if cphy setup is in progress for procedure which set OC
11/10/14   sp      Made changes to save procedure substates in crash debug info
09/05/14   sp      Made changes to detect UL size change for fach to pch transition before sending 
                   RB complete to facilitate inclusion of start value
06/17/14   sp      Made changes to mainline RLC-restablihment redesign changes
06/02/14   sp      Made changes for FR19997 - UL rlc re-establishment redesign
05/20/14   sp      Set maximum limit on the number of commands in LLC command queue
05/14/14   ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
03/13/14   sp      Removed code under FEATURE_WCDMA_HS_PCH feature flag
05/22/13   sa      Added code changes to support FEATURE_WCDMA_DC_HSUPA
09/22/12   ad      Corrected featurization for RRCLLC_CHAN_CFG_EPCH_HRNTI_ONLY cause
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/06/12   ad      Changes for HS-RACH feature
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/20/11   as      Made changes to handle cell update confirm with dhrnti and epch state
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
10/04/10   prk     Merging latest e-fach fixes.
09/20/10   prk/kp  Added code to support FEATURE_WCDMA_HS_FACH
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
09/03/09   ps      Made changes as part of CMI to include pdcp.h
                   instead of pdpcrrcif.h
05/08/09   ss      Updated Copyright Information
09/12/08   rm/ps   Added code for REl7 under FEATURE_WCDMA_REL7
04/10/08   rm      Moved LLC_BYPASS_ENGINE out of feature flag
03/06/08   da      Added MBMS phase 2C changes.  MBMS support in CELL_DCH.  
                   Includes mobility and dynamic resource management.
03/06/08   da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
12/21/07   da      Support for FEATURE_MBMS
08/08/07   rm      Added support for feature cell id reading in DCH state, 
                   under feature flag FEATURE_WCDMA_GPS_CELL_ID_READING.
02/26/07   rm      Added LLC_BYPASS_ENGINE to enum rrcllc_cmd_engine_e_type
02/09/07   ps      Added enum type rrcllc_reestab_config_e_type
08/28/06   da      Added support for OOS area handling during transition from DCH.
04/25/06   da      Added boolean sccpch_selec_reqd to rrcllc_cmd_under_process
03/10/06   da      Added Initial support for PDCP under FEATURE_UMTS_PDCP
05/05/05   svk     Added LLC_PHYCHAN_DROP_AND_ADD to rrcllc_phy_chan_op_type.
04/13/05   sk      Added enum rrcllc_unmap_rb_e_type
04/06/05   da      Added Initial support for BMC.  Handling of CTCH  and
                   a second SCCPCH in IDLE, CELL_PCH and URA_PCH states
                   are now supported.  CTCH is not present in CONNECTING, CELL_FACH
                   and CELL_DCH states.
03/11/05   vk      Merged HSDPA code in Mainline from RRC_MSM6275 branch under FEATURE HSDPA
12/23/04   sk      Added update_mac_with_rnti variable to cmd_under_process structure.
07/15/04   jh      Changed name of is_ifhho_in_dch flag to is_hho_in_dch.
07/14/04   sk      Added channel_config_reason field to chan_config structure
03/15/04   bu      Removed the use of FEATURE DUALMODE BASELINE. This feature is
                   always defined (even for singlemode builds).
03/04/04   bu      Removed the err_fatal_recovery_in_progress from under the
                   FEATURE_RRC_L1_ERROR_RECOVERY_P1.
02/20/04   bu      Added err_fatal_recovery_in_progress to
                   rrcllc_rrc_cmd_process_data_type.
02/12/04   bu      If ERR_FATAL recovery has been enabled change the delay
                   timer to 1 second.
11/21/03   bu      Added a boolean is_ifhho_in_dch to indicate if the command
                   under process is an Inter-freq HHO in DCH.
11/17/03   jh      Added new enums and data for SRNS relocation.
09/11/03   vn      Added new enums rrcllc_action_on_config_req_e_type and
                   rrcllc_mac_sync_with_l1_e_type for solving Interlayer
                   sync issues
09/09/03   jh      Added new_freq flag in rrcllc_rrc_cmd_process_data_type for
                   interfrequency handover.
09/03/03   bu      Added a new #define for RRCTMR_DELAY_ERR_FATAL_TIMER_IN_MS
08/11/03   vn      Added rnti_update_cnf_needed to rrcllc_cmd_process_data_type
07/11/03   vn      Changed name of LLC_WCDMA_MODE_DEACT_ENGINE to
                   LLC_STOP_WCDMA_MODE_ENGINE.
06/23/03   vn      Added a new #define for RRCTMR_L1_DEADLOCK_DETECT_TIMER_IN_MS
04/21/03   vn      Added clear_oc_pending in rrcllc_cmd_under_process_data.
04/10/03   bu      Added sys_stop_mode_reason_e_type to
                   rrcllc_rrc_cmd_process_data_type.
11/08/02   bu      Added the l1_stop_cause_enum_type to
                   rrcllc_rrc_cmd_process_data_type.
09/24/02   vn      Support for DCH->FACH transitions:
                   Added LLC_RELEASE_TR_PHY_CHAN_ENGINE to release transport
                   and physical channels before starting cell selection after
                   transitioning to CELL_FACH from CELL_DCH, added
                   rrcllc_toc_usage_e_type to indicate the state of TOC.
09/20/02   vn      Added chan_cfg_status in rrcllc_rrc_cmd_process_data_type.
08/09/02   vn      Added a field inside rrcllc_rrc_cmd_process_data_type to
                   indicate if a Confirm for Channel Config Request is needed.
06/11/02   vn      Merged in dummy signalling support.
05/16/02   bu      Fixed a merge mismatch.
05/15/02   vn      Merged the following from cell reselection branch:
          bu,vn    Added Cell Reselection Info in rrcllc_cmd_process_data_type.
                   Added n_pccpch action in phy_chan_action_list_type, added
                   LLC_CELL_RESELECTION_ENGINE in rrcllc_cmd_engine_e_type.
05/13/02   bu      Added a new LLC_WCDMA_MODE_DEACT_ENGINE to
                   rrcllc_cmd_engine_e_type for WCDMA mode deactivation support.
                   The changes have been added with FEATURE DUALMODE BASELINE tag.
03/04/02   vn      Moved rrcllc_dl_phy_chan_data_type, rrcllc_ul_phy_chan_
                   data_type and rrcllc_semi_permanent_data_type out of this
                   file as these structures now are part of ordered_config_type
02/07/02   vn      Added LLC_PHYCHAN_SET_LBT_MODE to rrcllc_phy_chan_op_type
                   for Loopback test mode support. Added a new field Loopback
                   mode to rrcllc_rrc_cmd_process_data_type.
11/09/01   vn      Merged the following from branch -
           vn      Support for Active Set Update. Added a flag aset_update in
                   rrcllc_phy_chan_action_list_type to indicate Active Set Update
                   case.
10/26/01   vn      Added a new LLC_DEACT_ENGINE to rrcllc_cmd_engine_e_type
                   for deactivation support.
10/22/01   vn      Support for Out of service area - added flags in semi-permanent
                   data to indicate current status of physical channels.
08/22/01   vn      Increased LLC_MAX_L1_CMDS to 6 to support single operation per
                   CPHY_SETUP_REQ.
07/26/01   vn      Added a new boolean - pch_included to indicate if selected
                   SCCPCH has a PCH - multiple SCCPCH support.
07/06/01   vn      Added a new flag - channel_est_ind_rcvd to
                   rrcllc_rrc_cmd_process_data_type. This indicates if a Channel
                   Establishment Indication was received while waiting for a Setup Cnf.
06/29/01   vn      Fixed a warning related to signed/unsigned mismatch.
06/13/01   vn      Added rrcllc_semi_permanent_data_type. Restructured
                   rrcllc_rrc_cmd_process_data_type for better understanding.
06/06/01   vn      Added l1_pccpch_parms_struct_type to rrcllc_dl_phy_chan_data_type
                   to store PCCPCH parameters for serving cell.
06/05/01   kmp     Fixed a typo.
06/01/01   vn      Added LLC_PHYCHAN_TRCH_EN_DIS to rrcllc_phy_chan_op_type.
05/29/01   vn      Added prototype rrcllc_ul_phy_chan_data_type.
05/24/01   vn      Added various structure prototypes for new LLC Architecture.
                   These include: rrcllc_phy_chan_action_data_type,
                   rrcllc_phy_chan_action_list_type, rrcllc_l1_req_list_type
                   rrcllc_rlc_config_req_list_type,
                   rrcllc_rrc_cmd_process_data_type, rrcllc_dl_cctrch_info_data_type
                   and rrcllc_dl_phy_chan_data_type. Removed others.
05/09/01   kmp     Removed Ordered config information, and AM config prototype.
04/02/01   kmp     Updated to add uplink power data structure to Ordered
                   Config.
03/30/01   rj      Changed dl_sdu_num to uint32 from uint8 to be compatible
                   with latest LED based rrcasn1.h
03/29/01   kmp     Initial Release.

===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "rrccmd_v.h"
#include "uecomdef.h"
#include "sys_plmn_selection.h"

#ifdef FEATURE_UMTS_PDCP
#ifdef FEATURE_CMI
#include "pdcp.h"
#else/*FEATURE_CMI*/
#include "pdcprrcif.h"
#endif/*FEATURE_CMI*/
#endif /* FEATURE_UMTS_PDCP*/

#define  LLC_MAX_L1_CMDS_PER_RRC_INTERN_CMD  6

#define MAX_LLC_COMMANDS_TO_QUEUE 50

/*-------------------------------------------------------------------
This is to define locally (since no one else should care) the CCTrCH
ID error condition.
--------------------------------------------------------------------*/
#define  RRCLLC_INVALID_CCTRCH_ID            0xFF

#define  RRCTMR_L1_DEADLOCK_DETECT_TIMER_IN_MS 6000UL

/* If ERR_FATAL recovery has been enabled change the delay timer to 1 second */
#ifdef FEATURE_RRC_L1_ERROR_RECOVERY_P1
   #define  RRCTMR_DELAY_ERR_FATAL_TIMER_IN_MS 1000UL
#else
   #define  RRCTMR_DELAY_ERR_FATAL_TIMER_IN_MS 6000UL
#endif
#if defined FEATURE_WCDMA_HS_FACH
/*-------------------------------------------------------------------
This macro adds the passed on CCTrCH ID to the list of to-be-released
CCTrCH IDs.
--------------------------------------------------------------------*/
#define RRCLLC_ADD_CCTRCH_ID_TO_RELEASE_LIST(cctrch_id)   \
{                                                         \
  rrcllc_cmd_under_process.chan_config.cctrch_id_release_list.        \
  id[rrcllc_cmd_under_process.chan_config.cctrch_id_release_list.num_ids] =  \
      (cctrch_id);                                               \
  rrcllc_cmd_under_process.chan_config.cctrch_id_release_list.num_ids ++;    \
}
#endif 
/*------------------------------------------------------------------
This enumerates all possibilities of actions on Physical channels.
-------------------------------------------------------------------*/
typedef enum
{
  LLC_PHYCHAN_NO_OP,
  LLC_PHYCHAN_SETUP,
  LLC_PHYCHAN_RELEASE,
  LLC_PHYCHAN_RECONFIG,
  LLC_PHYCHAN_TRCH_EN_DIS,
  LLC_PHYCHAN_DROP_AND_ADD,
  LLC_PHYCHAN_SET_LBT_MODE
}rrcllc_phy_chan_op_type;

/*------------------------------------------------------------------
This enumerates all possibile LLC command engines.
-------------------------------------------------------------------*/
typedef enum
{
  /* Command engine for a Channel Config command that does not
   * result in a Release all channel scenario.
   */
  LLC_CHAN_CONFIG_ENGINE,
  /* Command engine for a Release all Channel scenario. This
   * could happen as a result of a Channel Config Req.
   */
  LLC_RELEASE_ALL_CHAN_ENGINE,

  /* Command engine for lower layer deactivation */
  LLC_DEACT_ENGINE,

  /* Command engine for cell reselection */
  LLC_CELL_RESELECTION_ENGINE,

  /* Command engine for WCDMA Mode deactivation */
  LLC_STOP_WCDMA_MODE_ENGINE,

  /* Command engine for releasing only Physical and Transport
   * channels
   */
  LLC_RELEASE_TR_PHY_CHAN_ENGINE,

  /* Command engine for re-establishing RLC connections */
  LLC_RE_EST_RLC_ENGINE,
  LLC_BYPASS_ENGINE,
  /* Indicates that no command engine is active at present */
  LLC_NO_ENGINE
}rrcllc_cmd_engine_e_type;


/*-------------------------------------------------------------------
ENUM: rrcllc_mac_operation_e_type

This enum defines the possible operations on MAC UL
--------------------------------------------------------------------*/
typedef enum
{
  ONLY_SWAP_PTR,
  RECONFIG_MAC
}rrcllc_mac_ul_operation_e_type;


/*-------------------------------------------------------------------
ENUM: rrcllc_mac_operation_e_type

This enum defines the possible synch options between MAC and L1
--------------------------------------------------------------------*/
typedef enum
{
  SYNC_WITH_L1,
  NO_SYNC_WITH_L1
}rrcllc_mac_sync_with_l1_e_type;

typedef enum 
{
  RRC_CELL_CAP_INACTIVE,
  RRC_CELL_CAP_LEGACY,
  RRC_CELL_CAP_HSRACH
}rrc_cell_capability_type;

/*------------------------------------------------------------------
This enumerates the config db to be used for unmapping RB info.
-------------------------------------------------------------------*/
typedef enum
{
  OC_ONLY,
  TOC_ONLY,
  OC_AND_TOC
#ifdef FEATURE_WCDMA_HS_FACH
  ,SKIP_HANG_RB
#endif
}rrcllc_unmap_rb_e_type;
/*------------------------------------------------------------------
This enumerates all possibile actions on a Channel Config Request
-------------------------------------------------------------------*/
typedef enum
{
  TRASH_CONFIG_REQ,
  PROCESS_CONFIG_REQ
}rrcllc_action_on_config_req_e_type;

typedef enum
{
 ORDERED_CONFIG_ONLY,
 CURRENT_CONFIG_ONLY,
 ORDERED_AND_CURRENT_CONFIG
}rrcllc_reestab_config_e_type;

/*------------------------------------------------------------------
This contains a list of CCTrCH IDs.
-------------------------------------------------------------------*/
typedef struct
{
  byte  num_ids;
  cctrch_id_type  id[UE_MAX_CCTRCH - 1];
}rrcllc_cctrch_id_list_type;


/*-------------------------------------------------------------------
This structure holds a list of possible physical channels and the
corresponding actions on them.
TBD: At this point since there can not be multiple Physical channels
of any one type, there is only one occurance of a physical channel
of each physical channel type. This will have to be modified later to
accomodate multiple physical channels of each type.
--------------------------------------------------------------------*/
typedef struct
{
  rrcllc_phy_chan_op_type  pccpch_action;
  rrcllc_phy_chan_op_type  n_pccpch_action;
  rrcllc_phy_chan_op_type  sccpch_action;
  rrcllc_phy_chan_op_type  sccpch_for_ctch_action;
  rrcllc_phy_chan_op_type  prach_action;
  rrcllc_phy_chan_op_type  ul_dpch_action;
  rrcllc_phy_chan_op_type  dl_dpch_action;
  boolean                  aset_update;
#ifdef FEATURE_WCDMA_DC_HSUPA
//  rrcllc_phy_chan_op_type  sec_dl_dpch_action;
  boolean                  sec_eul_aset_update;
#endif /* FEATURE_WCDMA_DC_HSUPA */
}rrcllc_phy_chan_action_list_type;


/*-------------------------------------------------------------------
This structure holds a list of RLC commands for configuring
Uplink and downlink UM/TM and AM logical channel entities.
--------------------------------------------------------------------*/
typedef struct {
 /* UL TM logical channel config request */
 rlc_ul_tm_config_type  ul_tm;
 /* UL TM logical channel config request */
 rlc_dl_tm_config_type  dl_tm;
 /* UL TM logical channel config request */
 rlc_ul_um_config_type  ul_um;
 /* UL TM logical channel config request */
 rlc_dl_um_config_type  dl_um;
 /* UL TM logical channel config request */
 rlc_am_config_req_type am;
} rrcllc_rlc_config_req_list_type;


#ifdef FEATURE_UMTS_PDCP

/*-------------------------------------------------------------------
This structure holds a list of PDCP commands for configuring
PDCP entities.
--------------------------------------------------------------------*/
typedef struct {
  uint32 num_rbs;
  pdcp_cfg_req_data_type pdcp_cfg_req[MAX_RAB_TO_SETUP];
} rrcllc_pdcp_config_req_list_type;

#endif /* FEATURE_UMTS_PDCP */
/*-------------------------------------------------------------------
This structure holds a list of L1 commands. This list stores all L1
commands that need to be sent for any one RRC internal command.
--------------------------------------------------------------------*/
typedef struct
{
  int32 num_l1_req;
  int32 current_l1_req;
  l1_ext_cmd_type  l1_req[LLC_MAX_L1_CMDS_PER_RRC_INTERN_CMD];
}rrcllc_l1_req_list_type;

/*-------------------------------------------------------------------
This structure holds data pertaining to the current RRC internal
command under process.
--------------------------------------------------------------------*/
typedef struct
{
  /* Indicates if data in this structure is valid */
  boolean  valid;

  /* Procedure that sent the currently-being-processed RRC internal command.
   * A procedure ID of MAX_NUM_OF_PROCEDURES indicates the command was
   * internally generated by LLC
   */
  rrc_proc_e_type  procedure;

  /* RRC state to be transitioned to after processing the command. */
  uint32                chan_config_transaction_id;

  /* RRC state to be transitioned to after processing the command. */
  rrc_state_e_type  next_rrc_state;

  /* Currently processed RRC internal command ID. This is valid only if
   * the procedure ID is a valid procedure (other than LLC).
   */
  rrc_cmd_e_type  rrc_current_cmd_id;

  /* This boolean indicates if a "Clear Ordered Config" operation is
   * poending.
   */
  boolean  clear_oc_pending;

  struct
  {
    /*store the reason for channel configuration */
    rrcllc_chan_cfg_reason_e_type       chan_config_reason;
    /* Indicates if Channel Establishment Indication has been received from L1 */
    boolean   chan_est_ind_rcvd;
    /* LLC Command engine active at present */
    rrcllc_cmd_engine_e_type  current_cmd_engine;

    /* List of RLC Configure requests */
    rrcllc_rlc_config_req_list_type  rlc_config_req_list;

    /* List of physical channels and the corresponding actions on them */
    rrcllc_phy_chan_action_list_type  phy_chan_action_list;

    /* Boolean indicates if this is a solo BCH operation */
    boolean       solo_bch_operation;

    /* Loopback mode - applicable only when DL DPCH action is "Set Loopback mode" */
    rrc_lb_test_mode_e_type  lb_test_mode;
    /* Indicates if dummy signalling for loopback mode 2 is enabled */
    boolean       dummy_signaling_enabled;

    /* Frequency information for Inter-frequency HHO */
    boolean       new_freq;
    boolean       hho_ciph_update_required;


  /* HSDPA Active/Inactive Parameters */

    boolean hsdpa_inactive_to_active;
    boolean hsdpa_active_to_inactive;
    boolean hsdpa_ehs_to_hs;
    boolean hsdpa_hs_to_ehs;
    /* List of L1 request commands */
    rrcllc_l1_req_list_type  l1_req_list;

    /* List of CCTrCH IDs that need to be released once all CPHY_SETUP_REQs are
     * sent and corresponding CNF received.
     */
    rrcllc_cctrch_id_list_type  cctrch_id_release_list;

    /* Indicates if a Confirm is needed */
    boolean  cnf_needed;

    /* Status of the Channel Config command */
    rrcllc_chan_cfg_status_e_type  chan_cfg_status;

#ifdef FEATURE_UMTS_PDCP
    rrcllc_pdcp_config_req_list_type   pdcp_config_req_list;
#endif /* FEAUTURE_UMTS_PDCP */

#ifdef FEATURE_WCDMA_DC_HSUPA
  boolean       sec_eul_new_freq;

#endif /* FEATURE_WCDMA_DC_HSUPA */

  }chan_config;

  /* This structure will store the cell reselection information. It will be updated whenever LLC
   * receives a cell reselection request from CCM */
  struct
  {
    /* Freq. of the cell to be reselected */
    rrc_freq_type  freq;

    /* Scrambling Code of the Cell */
    rrc_scr_code_type   scr_code;

    /* The PLMN ID  */
    rrc_plmn_identity_type      plmn_id;

    /* Cell ID */
    rrc_int_cell_id_type        cell_id;

  }rrcllc_cell_resel_info;

  l1_stop_cause_enum_type       wcdma_stop_cause;

  sys_stop_mode_reason_e_type   wcdma_deact_reason;

  boolean                       rnti_update_cnf_needed;

  rrc_rlc_re_est_for_srns_rb_e_type rlc_re_est_rbs;
  boolean            update_mac_with_rnti;
  /* boolean used durin RNTI_UPDATE_REQ to 
   * indicate that SCCPCH selection is 
   * required
   */
  boolean            sccpch_selec_reqd;

  /* boolean indicating if the command under process is a Inter-frequency
   * Hard Handover within DCH
   */
  boolean       is_hho_in_dch;

  boolean       err_recovery_in_progress;

#ifdef FEATURE_DUAL_SIM
    boolean send_reset_l1_cnf;
#endif

#ifdef FEATURE_WCDMA_DC_HSUPA
boolean       is_hho_in_dch_for_sec_eul;
#endif /* FEATURE_WCDMA_DC_HSUPA */


}rrcllc_rrc_cmd_process_data_type;


/*===========================================================================

FUNCTION: rrcllc_update_hsdpa_mac_action_l1_ops_for_old_config

DESCRIPTION:
  This  function updates the HS and EHS operations for L1 and MAC in ordered config

DEPENDENCIES:
  None.

RETURN VALUE:
  None.

SIDE EFFECTS:
  None
===========================================================================*/
void rrcllc_update_hsdpa_mac_action_l1_ops_for_old_config
(
  hsdpa_setup_ops_enum_type hsdpa_l1_ops
  ,mac_hs_transition_e_type hs_transition

);

/*===========================================================================

FUNCTION: rrcllc_get_log_chan_action_for_rb_action

DESCRIPTION:
  This  function returns the RLC action based on RB Action

DEPENDENCIES:
  None.

RETURN VALUE:
  RLC Action

SIDE EFFECTS:
  None
===========================================================================*/
rlc_channel_action_e_type rrcllc_get_log_chan_action_for_rb_action
(
  rrc_rb_config_type * rb_config
    , uint16 *ul_cfg_mask,
     uint16 * dl_cfg_mask
);
/*===========================================================================

FUNCTION: rrcllc_check_and_take_backup_rlc_parms

DESCRIPTION:
  This  function checks and takes back up of UL RLC size

DEPENDENCIES:
  None.

RETURN VALUE:
None

SIDE EFFECTS:
  None
===========================================================================*/
void rrcllc_check_and_take_backup_rlc_parms
(
  void
);
/*===========================================================================

FUNCTION: rrcllc_check_and_reestablish_ul_side

DESCRIPTION:
  This  function checks if conditions are satisfied for reestablishing and then
  sets the reestablish entity. It also adds the RB to channel config request if the rb is
  not part of channel config request 

DEPENDENCIES:
  None.

RETURN VALUE:
None

SIDE EFFECTS:
  None
===========================================================================*/
void rrcllc_check_and_reestablish_ul_side
(
  rrc_cmd_type *cmd_ptr
);
/*===========================================================================

FUNCTION: rrcllc_set_reestablish_entity_for_ul

DESCRIPTION:
  This  function gets the new rlc info 

DEPENDENCIES:
  None.

RETURN VALUE:
  RLC reestablish action

SIDE EFFECTS:
  None
===========================================================================*/
uecomdef_status_e_type rrcllc_set_reestablish_entity_for_ul
(
  uint8 rb_id,
  boolean is_ul_reestab_required
);

/*====================================================================
FUNCTION: rrcllc_check_and_reestablish_ul_during_rnti_update_req()

DESCRIPTION:
  This function checks if RLC re-establishment needs to be done. If
  yes, then it updates the RLC Re-establishment command buffer and
  returns a TRUE value.

DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE: RLC Re-establishment needs to be done.
  FALSE: Otherwise

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_and_reestablish_ul_during_rnti_update_req
(
  void
);
/*===========================================================================

FUNCTION: rrcllc_init_old_ul_rlc_size

DESCRIPTION:
  This  function clears the old stored size for all entries 

DEPENDENCIES:
  None.

RETURN VALUE:
None

SIDE EFFECTS:
  None
===========================================================================*/
 void rrcllc_init_old_ul_rlc_size_backup(void);
/*====================================================================
FUNCTION: rrcllc_store_last_active_cell_capability()

DESCRIPTION:
  This function stores the capability of the old cell

DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE: RLC Re-establishment needs to be done.
  FALSE: Otherwise

SIDE EFFECTS:
  None.
====================================================================*/
void rrcllc_store_last_active_cell_capability(void);
/*===========================================================================

FUNCTION: rrcllc_clear_old_ul_rlc_size

DESCRIPTION:
  This  function clears the old stored size for a particular RB ID

DEPENDENCIES:
  None.

RETURN VALUE:
None

SIDE EFFECTS:
  None
===========================================================================*/
void rrcllc_clear_old_ul_rlc_size
(
  rb_id_type  rb_id
);
/*====================================================================
FUNCTION: rrcllc_check_if_hsrach_to_legacy_cell_change_happened()

DESCRIPTION:
  This function checks if hsrach to legacy cell change happened or not
DEPENDENCIES:
  None.

RETURN VALUE:
rrc_cell_capability_type

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_if_hsrach_to_legacy_cell_change_happened(void);
/*====================================================================
FUNCTION: rrcllc_get_old_cell_capability()

DESCRIPTION:
  This function gets the the capability of the old cell

DEPENDENCIES:
  None.

RETURN VALUE:
rrc_cell_capability_type

SIDE EFFECTS:
  None.
====================================================================*/
rrc_cell_capability_type rrcllc_get_old_cell_capability(void);
/*===========================================================================
  FUNCTION rrcllc_get_current_substate()
  
  DESCRIPTION
    This function will return current llc
    procedure's sub state.
  DEPENDENCIES
    None
  
  RETURN VALUE
    return current llc sub state.
  SIDE EFFECTS
  none
  ===========================================================================*/
  uint8  rrcllc_get_current_substate(void);

/*===========================================================================

FUNCTION: rrcllc_get_current_chan_conf_under_process_procedure

DESCRIPTION:This function returns the procedure which is currently processing channel config.

DEPENDENCIES:
  None.

RETURN VALUE:
  rrc_proc_e_type

SIDE EFFECTS:
  None
===========================================================================*/
rrc_proc_e_type rrcllc_get_current_chan_conf_under_process_procedure(void);

#endif /* RRCLLCI_H */
