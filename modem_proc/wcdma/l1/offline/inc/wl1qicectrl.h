#ifndef WL1_QICE_CTRL_H
#define WL1_QICE_CTRL_H
/*===========================================================================
                 WCDMA L1 QICE Controller

GENERAL DESCRIPTION
  This file contains the code for controlling the operation of the QICE block

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2008-2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wl1qicectrl.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who    what, where, why
--------   ---    ---------------------------------------------------------
02/24/15   ap     Initial check-in for WL1 QSH framework 
10/12/15   rsr    VLC Feature Checkin.
10/12/15   rsr    Added changes to Ensure that IRPM Enable happens 20SF after 
                  first triage cycle.
01/08/15   svh    Fixed compiler warnings/errors.
08/20/14   rsr    Added Fix to Ensure that we immediately Update the NBR CFG
                  BMSK, instead of Posting Local Command for Updation.
08/06/14   sr/rsr Changes to handle nbr QICE cell drop for cell resel to that cell.
06/10/14   qj     Added cfg_done_ngbr_cell_bmsk to wl1_cme_ctrl_cmd_type
03/17/14   mit    Changes to accommodate dynamic cell_db_idx allocation
04/15/14   rsr    Added Fix to Address Race Condition between CME and RxD.
04/10/14   ar     Added QICE suspend client for cell reselection
11/18/13   rsr    Added QICE Multicarrier Support.
11/04/13   rsr    Provided APi to SRCH to avoid calling CME Cb, in case CME 
                  triage is suspended. 
09/12/13   jd      Searcher redesign
08/28/13   sad    Disable SCH-IC if there are multiple cells in ASET
07/24/13   jd      Changes for Dual band Dual carrier
07/23/13   vs     Added support for antenna switch diversity feature.
07/18/13   rsr    MCVS: added support for SVS+, changed prototype
04/09/13   vs     Added support for wcdma periodic statictics log pkt.
04/12/13   vsr    Disable SCHIC for R99 calls with >1 cell in ASET
03/14/13   sj     Changed the query API for MCVS VPE update
03/08/13   sj     MCVS feature enhancement
02/13/13   sj     Dynamic resizing of max QSET size in DC call
                  Eliminating delay in adding cell to QSET
01/24/13   sj     Added function to drop qice neighbor cell to free up cell id
11/28/12          MCVS feature related changes  
07/24/12   geg    Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/21/12   ms     Added API wl1_cme_is_mcpm_relinquish_pending
05/21/12   sj     Added SRCH_DECONFIG_CELLS_CB as CME cmd sender
04/06/12   hk     Added external triggers to enable/disable CME
02/10/12   sj     Added an unified CME local cmd handler.
01/27/12   vs     Nikel feature cleanup.
12/12/11   sj     Added API to check CME pending flag
11/09/11   sj     Changed QICE resource manager to use alternative table 
                  for VPE cycle stressed scenarios
07/05/11   vsr    Mainlining Genesis CME features
01/13/11   vsr    Added support for QICE ASET cell registration
                  Added support for CME triage suspend/resume
11/23/10   vsr    Added API to be called for CME cleanup
10/20/10   vsr    Add QSET Neighbour Cell Support
05/03/10   rvs    Add extra params in
                  wl1_cme_proc_wplt_qice_oride_config_params_cmd().
04/01/10   rvs    Add wl1_cme_proc_wplt_set_cqi_and_demod_algo_bmsk_cmd().
02/11/10   rvs    Updates to support dual carrier.
12/16/09   rvs    Added wl1_cme_proc_wplt_qice_debug_enable_cmd().
09/24/09   rvs    Split header file in wl1qicectrl.h and wl1qicectrli.h.
09/09/09   rvs    Added support for QICE F/Finv and QSET Update log packets.
07/15/09   rvs    Checked in initial version of file.

===========================================================================*/

/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "comdef.h"
#include "customer.h"

#include "l1const.h"
#include "l1def.h"

/* -----------------------------------------------------------------------
** Enumerations/Structures/Typedefs
** ----------------------------------------------------------------------- */

/* List of CME local cmd senders */
typedef enum
{
  WL1_CME_HS_STATE_CHANGE_CB_ENABLE,
  WL1_CME_SRCH_DECONFIG_CELLS_CB_RESUME_HS,
  WL1_CME_SRCH_DECONFIG_CELLS_CLIENT_CB,
  WL1_CME_DEMOD_CFG_CMD_DONE_CB,
  WL1_CME_CFN_NOTIFY_CB,
#ifdef FEATURE_WCDMA_ANT_SWITCH_SUPPORT
  WL1_CME_ANT_SWITCH_CB,
#endif
  WL1_CME_CELL_CFG_DONE_CB,
  WL1_CME_EXT_ENABLE,
  WL1_CME_EXT_DISABLE,
  WL1_CME_SCHIC_DISABLE_CB_RESUME_ASET_UPD,
  WL1_CME_DEMOD_CFG_PROC_DONE_CB,
  WL1_CME_RXD_STATE_UPD,
  WL1_CME_IRPM_ENABLE,
  WL1_CME_VLC_ENABLE,
  WL1_CME_CTRL_CMD_SNDR_MAX
} wl1_cme_ctrl_cmd_sndr_enum_type;

typedef struct 
{
  /*header*/
  l1_local_cmd_hdr_type hdr;
  /*sender of the local command*/
  wl1_cme_ctrl_cmd_sndr_enum_type sender;
  /* RxD State */
  boolean rxd_state;
} wl1_cme_ctrl_cmd_type;

typedef enum
{
  WL1_CME_CARRIER_PRI = 0,
  WL1_CME_CARRIER_SEC,
#ifdef FEATURE_WCDMA_3C_HSDPA
  WL1_CME_CARRIER_TRI,
#endif
  WL1_CME_NUM_CARRIERS
} wl1_cme_carrier_enum_type;

/* Enum for SRCH cycle Mode */
typedef enum
{
  WL1_CME_SRCH_CYCLE_LOW,
  WL1_CME_SRCH_CYCLE_MED,
  WL1_CME_SRCH_CYCLE_HIGH,
  WL1_CME_SRCH_CYCLE_INVALID
} wl1_cme_srch_cycle_factor_type;

typedef struct
{
  /* 1 bit per Carrier 0:C0, 1:C1, 2:C2*/
  uint8 cme_mcvs_txd_status;
  /* 2bits per carrier 0:1 -> C0, 3:2 -> C1 */
  uint8 cme_mcvs_qset_size_status;
  /* 2bits per cell, per carrier 0:1 -> Cell 1, 3:2 -> Cell 2*/
  uint8 cme_mcvs_qset_iteration_status[WL1_CME_NUM_CARRIERS];
} wl1_cme_mcvs_struct_type;

typedef void L1M_NEXT_CMD_HANDLER_CB_FUNC_TYPE(void);

/* -----------------------------------------------------------------------
** Macros
** ----------------------------------------------------------------------- */

#define WL1_CME_SUSPEND_CLIENT_L1M_SETUP                       (0x0002) 
#define WL1_CME_SUSPEND_CLIENT_CME_CLEANUP                     (0x0004) 
#define WL1_CME_SUSPEND_CLIENT_CARRCFG_CLEANUP		       (0x0008)
#define WL1_CME_SUSPEND_CLIENT_CELL_RESEL                      (0x0010)

/*===========================================================================
FUNCTION wl1_cme_hs_ops_needed

DESCRIPTION
API called by HS to see if CME cfg is required for transition to new state

DEPENDENCIES
None

RETURN VALUE
TRUE is CME cfg is required; Else FALSE

SIDE EFFECTS
Return value of TRUE indicates need for break in context
===========================================================================*/
boolean wl1_cme_hs_ops_needed(boolean hs_state);

/*===========================================================================
FUNCTION wl1_cme_cleanup

DESCRIPTION
API to kickoff the cleanup check and sending cleanup confirmation

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
Cleanup confirmation will be sent once CME cleanup is completed
===========================================================================*/
void wl1_cme_cleanup(void);

/*===========================================================================
FUNCTION wl1_cme_suspend_cme_triage

DESCRIPTION
Suspend CME triage

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_suspend_cme_triage(uint16 client_id);

/*===========================================================================
FUNCTION wl1_cme_resume_cme_triage

DESCRIPTION
Resume CME triage

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_resume_cme_triage(uint16 client_id);

/*===========================================================================
FUNCTION wl1_cme_srch_config_cells_cb

DESCRIPTION
Callback called by searcher when requested cells have been configured

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_srch_config_cells_cb(uint16 cell_bmsk);

/*===========================================================================
FUNCTION wl1_cme_is_any_non_primary_cells_configured

DESCRIPTION
checks to see if any non primary configured cells idx present in the 
cell_bmask passed as argument

DEPENDENCIES
None

RETURN VALUE
TRUE : if any non primary cells present in bmask
FALSE: Otherwise

SIDE EFFECTS
None
===========================================================================*/
boolean wl1_cme_is_any_non_primary_cells_configured(uint16 cell_bmsk);

/*===========================================================================
FUNCTION wl1_cme_srch_deconfig_cells_cb

DESCRIPTION
Callback called by searcher when requested cells have been deconfigured

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_srch_deconfig_cells_cb(uint16 cell_bmsk);

/*===========================================================================
FUNCTION wl1_cme_get_qset_neighbours

DESCRIPTION
Function returns a bitmask with cells that have been configured by CME 

DEPENDENCIES
None

RETURN VALUE
Cell Bitmask 

SIDE EFFECTS
None
===========================================================================*/
uint16 wl1_cme_get_qset_neighbours(void);

#ifdef FEATURE_QSH_DUMP
/*===========================================================================
FUNCTION wl1_cme_mini_dump_cb

DESCRIPTION
This function Records Mini Dump Information for Debug

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_mini_dump_cb(void* debug_dump_ptr);
#endif

/*===========================================================================
FUNCTION wl1_cme_proc_wplt_eq_oride_cmd

DESCRIPTION
Overrides CME enable, SCHIC enable, and the HS receivers based on the value
passed in.

DEPENDENCIES
None

RETURN VALUE
FALSE if the command is sent in the wrong state or an incorrect value is sent,
TRUE otherwise.

SIDE EFFECTS
None
===========================================================================*/
boolean wl1_cme_proc_wplt_eq_oride_cmd(
  /* Carrier whose parameters to override. */
  uint32 carrier,
  /* Value that determines the overriden CME enable, SCHIC enable, and HS receivers. */
  uint8 oride_val);

boolean wl1_cme_proc_wplt_qice_oride_num_iter_cmd(
  /* Carrier whose parameters to override. */
  uint32 carrier,
  boolean set_iter,
  uint8 hs_cell_iter, 
  uint8 first_intf_iter,
  uint8 second_intf_iter);

/*===========================================================================
FUNCTION wl1_cme_disable_schic_for_sho

DESCRIPTION
This function is called during an ASU if the ASET count is >1. It will disable
SCHIC if HS is not enabled.

DEPENDENCIES
None

RETURN VALUE
TRUE if ASU can proceed; FALSE if ASU needs to wait for SCHIC disable

SIDE EFFECTS
None
===========================================================================*/
boolean wl1_cme_disable_schic_for_sho(
  L1M_NEXT_CMD_HANDLER_CB_FUNC_TYPE *l1m_next_cmd_handler_cb);

/*===========================================================================
FUNCTION wl1_cme_is_mcpm_relinquish_pending

DESCRIPTION
Query if CME is pending on QICE MCPM relinquish

DEPENDENCIES
None

RETURN VALUE
TRUE if QICE MCPM relinquish, FALSE otherwise

SIDE EFFECTS
None
===========================================================================*/
boolean wl1_cme_is_mcpm_relinquish_pending(void);

/*===========================================================================
FUNCTION wl1_cme_is_cme_pending

DESCRIPTION
Gets the value of cme pending flag.

DEPENDENCIES
None

RETURN VALUE
TRUE if CME pending flag is set, FALSE otherwise

SIDE EFFECTS
None
===========================================================================*/
boolean wl1_cme_is_cme_pending(void);

/*===========================================================================
FUNCTION wl1_cme_proc_wplt_qice_oride_table_params_cmd

DESCRIPTION
Overrides QICE HW tables.  

DEPENDENCIES
None

RETURN VALUE
TRUE if sent while is inactive, FALSE otherwise.

SIDE EFFECTS
None
===========================================================================*/
boolean wl1_cme_proc_wplt_qice_oride_table_params_cmd(
  /* Carrier whose parameters to override. */
  uint32 carrier,
  /* Hyperbolic tangent look-up table. */
  uint32 * tanh_table,
  /* Square-root look-up table part 1. */
  uint32 * sqrt_table_1,
  /* Square-root look-up table part 2. */
  uint32 * sqrt_table_2,
  /* Parameter table. */
  uint32 * param_table);

/*===========================================================================
FUNCTION wl1_cme_proc_wplt_qice_oride_config_params_cmd

DESCRIPTION
Overrides QICE HW config parameters.

DEPENDENCIES
None

RETURN VALUE
TRUE if sent while is inactive, FALSE otherwise.

SIDE EFFECTS
Stores the new parameters to be sent in the next QICE debug info log packet.
===========================================================================*/
boolean wl1_cme_proc_wplt_qice_oride_config_params_cmd(
  /* Carrier whose parameters to override. */
  uint32 carrier,
  /* Commit margin in CHIPx2. */
  uint32 commit_margin_cx2,
  /* Threshold for energy level in P selection. */
  uint32 sfc_energy_thresh,
  /* Alpha value for the FW IIR filter for SFC HW sums. */
  uint16 sfc_hw_sum_alpha,
  /* Alpha value for the FW IIR filter for SFC ratio. */
  uint16 sfc_ratio_alpha,
  /* Threshold for the SFC ratios for determining SF16 or not. */
  uint16 sfc_ratio_thresh,
  /* Interpolation weights across iterations for the diagonal components of the weights used by QICE for equalization. */
  uint8 * mmse_weight_diag,
  /* Interpolation weights across iterations for the off-diagonal components of the weights used by QICE for equalization. */
  uint8 * mmse_weight_cross,
  /* Noise inflation parameter. */
  uint8 noise_inflation,
  /* Number of QICE iterations being programmed. */
  uint8 iterations,
  /* Bank switch delay. */
  uint8 bank_switch_delay,
  /* Number of BPGs that occur between HW dumps. */
  uint8 sfc_bpg_per_dump,
  /* Number of HW dumps before ratios are calculated. */
  uint8 sfc_dump_per_ratio,
  /* Number of ratios calculated before SFC. */
  uint8 sfc_ratio_per_classify,
  /* Excess subframes that a SF16 code is assumed active. */
  uint8 sf16_active_subframes,
  /* Number of consecutive idle windows before turning off QICE clock. */
  uint8 idle_windows);

/*===========================================================================
FUNCTION wl1_cme_proc_wplt_qice_oride_fht_params_cmd

DESCRIPTION
Overrides the QICE HW FHT parameters for a cell.

DEPENDENCIES
None
  
RETURN VALUE
TRUE if the cell is found in the QSET, FALSE otherwise.

SIDE EFFECTS
Stores the new parameters to be sent in the next QICE debug info log packet.
===========================================================================*/
boolean wl1_cme_proc_wplt_qice_oride_fht_params_cmd(
  /* Carrier whose parameters to override. */
  uint32 carrier,
  /* Primary scrambling code of the QSET cell to override the parameters. */
  uint16 psc,
  /* FHT enable/disable for OVSF 256 bitmask. */
  uint32 * fht256,
  /* FHT enable/disable for OVSF 16 bitmask. */
  uint16 fht16);

/*===========================================================================
FUNCTION wl1_cme_proc_wplt_qice_oride_fw_params_cmd

DESCRIPTION
Overrides QICE FW parameters for a cell.

DEPENDENCIES
None

RETURN VALUE
TRUE if the cell is found in the QSET, FALSE otherwise.

SIDE EFFECTS
Stores the new parameters to be sent in the next QICE debug info log packet.
===========================================================================*/
boolean wl1_cme_proc_wplt_qice_oride_fw_params_cmd(
  /* Carrier whose parameters to override. */
  uint32 carrier,
  /* Primary scrambling code of the QSET cell to override the parameters. */
  uint16 psc,
  /* Misc SF16 overrides: forced LMMSE and forced P Matrix settings. */
  uint16 misc_sf16_overrides,
  /* Mask for SF16 OVSFs to override for non-linear mode. */
  uint16 sf16_non_linear_override,
  /* Value for SF16 OVSFs to override for non-linear mode. */
  uint16 sf16_non_linear_select,
  /* Mask for SF16 OVSFs to override for SF16 mode. */
  uint16 sf16_mode_override,
  /* Value for SF16 OVSFs to override for SF16 mode. */
  uint16 sf16_mode_select);

/*===========================================================================
FUNCTION wl1_cme_proc_wplt_qice_oride_log_params_cmd

DESCRIPTION
Overrides the QICE log parameters.

DEPENDENCIES
None

RETURN VALUE
TRUE

SIDE EFFECTS
None
===========================================================================*/
boolean wl1_cme_proc_wplt_qice_oride_log_params_cmd(
  /* Carrier whose parameters to override. */
  uint32 carrier,
  /* Number of HW dumps between log entries. */
  uint8 hw_dump_per_log,
  /* OVSF to log on the primary cell. */
  uint8 primary_ovsf,
  /* OVSF to log on the non-primary cells. */
  uint8 non_primary_ovsf);

/*===========================================================================
FUNCTION wl1_cme_proc_wplt_qice_reset_cmd

DESCRIPTION
QICE reset command.  

DEPENDENCIES
None

RETURN VALUE
TRUE

SIDE EFFECTS
None
===========================================================================*/
boolean wl1_cme_proc_wplt_qice_reset_cmd(
  /* Carrier whose parameters to override. */
  uint32 carrier,
  /* Bitmask which select which EQ/EQ's SFC calculations are reset. */
  uint8 reset_bmsk);

/*===========================================================================
FUNCTION wl1_cme_proc_wplt_cme_oride_params_cmd

DESCRIPTION
Overrides QICE SW parameters.

DEPENDENCIES
None

RETURN VALUE
TRUE

SIDE EFFECTS
None
===========================================================================*/
boolean wl1_cme_proc_wplt_cme_oride_params_cmd(
  /* Carrier whose parameters to override. */
  uint32 carrier,
  /* Maximum number of QSET cells when HS is active and QICE is a possible receiver. */
  uint8  max_num_qset_cells,
  /* Ec/Io add threshold of cells to the QSET in implicitly negative Q1 format. */
  uint8  qset_ecio_add_threshold,
  /* Ec/Io drop threshold of cells to the QSET in implicitly negative Q1 format. */
  uint8  qset_ecio_drop_threshold,
  /* Drop counter value at which a cell will be dropped from the QSET for low Ec/Io. */
  uint8  qset_drop_ctr_threshold,
  /* Metric at which a strong PSET will be promoted into the QSET. */
  uint8  qset_metric_threshold,
  /* Primary counter value at which a cell will become the primary cell, if there is no HS cell. */
  uint8  qset_primary_ctr_threshold,
  /* Minimum value subtracted from a metric when a PSET cell is weaker than a given QSET cell. */
  uint8  pset_leak,
  /* Number of triage cycles over which to avg the number of locked fings. */
  uint8  num_locked_fings_window_size,
  /* Number of triage cycles over which to avg the SCCH success rate. */
  uint8  scch_success_rate_window_size,
  /* Minumum average number of locked fingers when RxD is not present needed to allow QICE to be enabled. */
  uint8  min_avg_num_locked_fings_no_rxd,
  /* Minumum average number of locked fingers when RxD is present needed to allow QICE to be enabled. */
  uint8  min_avg_num_locked_fings_rxd,
  /* Minumum average SCCH decode success rate needed to allow QICE to be enabled. */
  uint8  min_avg_scch_success_rate,
  /* Maximum TX AGC value allowed to allow QICE to be enabled. */
  int16  max_tx_agc_val,
  /* Initial value of the QICE timer after enabling QICE. */
  uint8  disable_qice_timer_init_val,
  /* Initial value of the QICE timer after disabling QICE. */
  uint8  enable_qice_timer_init_val);

/*===========================================================================
FUNCTION wl1_cme_proc_wplt_qice_debug_enable_cmd

DESCRIPTION
Overrides QICE debug enable.

DEPENDENCIES
None

RETURN VALUE
TRUE

SIDE EFFECTS
None
===========================================================================*/
boolean wl1_cme_proc_wplt_qice_debug_enable_cmd(
  /* Carrier whose parameters to override. */
  uint32 carrier,
  /* New value for QICE debug enable flag. */
  uint8  enable);

/*===========================================================================
FUNCTION wl1_cme_proc_wplt_qice_debug_enable_cmd

DESCRIPTION
Overrides QICE debug enable.

DEPENDENCIES
None

RETURN VALUE
TRUE

SIDE EFFECTS
None
===========================================================================*/
boolean wl1_cme_proc_wplt_set_cqi_and_demod_algo_bmsk_cmd(
  /* New value for CQI algorithm bitmask. */
  uint8  cqi_algo_bmsk,
  /* New value for Demod algorithm bitmask. */
  uint8  demod_algo_bmsk);

/*===========================================================================
FUNCTION wl1_cme_ctrl_cmd_handler

DESCRIPTION
Handler for local cmd - L1_CME_CTRL_CMD

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_ctrl_cmd_handler(wl1_cme_ctrl_cmd_type *cmd_ptr);

/*===========================================================================
FUNCTION wl1_cme_trigger_enable

DESCRIPTION
This function posts the control command to enable CME.
  Is useful to switch the execution to task context for the cases where the CME enable 
  can only be triggered from interrupt context.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_trigger_enable(void);

/*===========================================================================
FUNCTION wl1_cme_trigger_disable

DESCRIPTION
This function posts the control command to disable CME.
  Is useful to switch the execution to task context for the cases where the CME disable 
  can only be triggered from interrupt context.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_trigger_disable(void);

/*===========================================================================
FUNCTION wl1_cme_enable

DESCRIPTION
Enables the CME, registers callbacks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void wl1_cme_enable(
  /* Used to identify the caller of this function. */
  uint8 calling_client);

/*===========================================================================
FUNCTION wl1_cme_disable

DESCRIPTION
Disables the CME, clearing out the P/QSET and deregistering all callbacks.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_disable(
  /* Used to identify the caller of this function. */
  uint8 calling_client);

/*===========================================================================
FUNCTION wl1_cme_is_cme_enabled

DESCRIPTION
Gets the state of CME.  

DEPENDENCIES
None

RETURN VALUE
TRUE if CME is enabled, FALSE otherwise.

SIDE EFFECTS
None
===========================================================================*/
boolean wl1_cme_is_cme_enabled(void);

boolean wl1_is_qice_enabled_on_c1(void);
boolean wl1_is_qice_enabled_on_c2(void);

/*===========================================================================
FUNCTION wl1_cme_is_qice_cfg_vpe_high

DESCRIPTION
Get the current QICE request for MCVS VPE update

DEPENDENCIES
None

RETURN VALUE
Boolean

SIDE EFFECTS
None
===========================================================================*/
extern uint8 wl1_cme_is_qice_cfg_vpe_high(void);

/*===========================================================================
FUNCTION wl1_cme_is_triage_suspended

DESCRIPTION
Returns the Current CME traige Status 

DEPENDENCIES
None

RETURN VALUE
Boolean

SIDE EFFECTS
None
===========================================================================*/
boolean wl1_cme_is_triage_suspended(void);

/*===========================================================================
FUNCTION wl1_cme_get_qice_info_for_logging

DESCRIPTION
gives the qice state info per carrier ie whether it is qice off,full qice or qpc.
        00: QICE OFF
        01: FULL QICE ON
        10: invalid
        11: QPC/LEQ ON


DEPENDENCIES
None

RETURN VALUE
uint8 

SIDE EFFECTS
None
===========================================================================*/
uint8 wl1_cme_get_qice_info_for_logging(uint8 carrier);


/*===========================================================================
FUNCTION wl1_cme_get_qice_reason_for_logging

DESCRIPTION
gives the qice reason bitmask per carrier.

DEPENDENCIES
None

RETURN VALUE
uint8

SIDE EFFECTS
None
===========================================================================*/
uint8 wl1_cme_get_qice_reason_for_logging(uint8 carrier);

/*===========================================================================
FUNCTION wl1_cme_get_mcvs_info

DESCRIPTION
Get the current QICE request for MCVS VPE update

DEPENDENCIES
None

RETURN VALUE
Boolean

SIDE EFFECTS
None
===========================================================================*/
void wl1_cme_get_mcvs_info(wl1_cme_mcvs_struct_type *wmcvsdrv_cme_struct);

/*===============================================================================
FUNCTION wl1_cme_update_srch_utilzation_param

DESCRIPTION
Updates the SRCH Utilization Factor for QICE operation

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===================================================================================*/
void wl1_cme_update_srch_cycle_param(wl1_cme_srch_cycle_factor_type cycle_factor);

#endif /* WL1_QICE_CTRL_H */
