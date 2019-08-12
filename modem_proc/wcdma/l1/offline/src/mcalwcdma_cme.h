#ifndef MCALWCDMA_CME_H
#define MCALWCDMA_CME_H
/*============================================================================*/
/** @file
 * This module has definition and declaration related to MCAL UMTS CME
 * configuration module APIs
 */
/*============================================================================*/

/*============================================================================
Copyright (c) 2008 - 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/mcalwcdma_cme.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who    what, where, why
--------   ---    ---------------------------------------------------------
10/12/15   rsr    Initial Checkin for IRPM Feature.
07/27/15   br     API to get current schic receiver mask.
07/08/15   svh    Not sending cme disable for an already disabled cell.
12/31/14   svh    Added CirCleanThresh field to CIRE NV.
08/26/14   svh    Added changes for CIR enhancement logging and NV.
02/07/14   sm     OMRD changes to make RxD trm compliant
01/06/14   rsr    Added Fix to Take care of Race Conditiion issue with FW in
                  case of SCCH order.
11/18/13   rsr    Added QICE Multi-carrier Support.
10/16/13   qj     Added commonCfgPending from fw
09/12/13   jd     Searcher redesign
07/24/13   jd     Changes for Dual band Dual carrier
06/13/13   rsr    Added Ghost drop counter variable to 
                  mcalwcdma_cme_qset_cfg_params_struct_type
01/29/12   vs     Nikel feature cleanup.
08/31/11   sj     Added query function to get SW config PN position from async intf buffer
08/02/11   vsr    CME bringup
07/05/11   vsr    Mainlining Genesis CME features
10/20/10   vsr    (Genesis) Add QSET Neighbour Cell Support
07/27/10   vsr    (Genesis) Added function to read eq pn pos from status dump
07/14/10   vsr    (Genesis) Changes for CQI based reframing
05/03/10   rvs    Added params to
                  mcalwcdma_cme_process_qice_oride_config_params_cmd().
04/05/10   rvs    Add "sec_cpich_enabled_mimo_mode" and
                  "sec_cpich_ovsf_mimo_mode" fields to structures.
04/01/10   rvs    Add mcalwcdma_cme_set_cqi_and_demod_algo_bmsk().
02/11/10   rvs    Updates to support dual carrier.
09/24/09   rvs    Changes to use locked fingers for window pos calculation.
09/09/09   rvs    Add support for QICE F/Finv log packet.
07/15/09   rvs    Added QICE defines and functions.
04/20/09   rvs    Fixed the definion of mcalwcdma_cme_demod_cfg_cmd_done_cb().
10/23/08   vsr    Reduced QSET size to 1
08/27/08   vsr    Checked in initial version of file

===========================================================================*/

/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "comdef.h"
#include "mcalwcdma_common.h"
#include "srchset.h"
#include "wl1tri.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/** Maximum number of QSET cells that are supported by HW. */
#if WFW_DEMOD_NUM_MAX_EQS_PRI_CARRIER > WFW_DEMOD_NUM_MAX_EQS_SEC_CARRIER
#define MCALWCDMA_CME_MAX_NUM_QSET_CELLS                   (WFW_DEMOD_NUM_MAX_EQS_PRI_CARRIER)
#else
#define MCALWCDMA_CME_MAX_NUM_QSET_CELLS                   (WFW_DEMOD_NUM_MAX_EQS_SEC_CARRIER)
#endif

/** Maximum number of cell ids. */
#define MCALWCDMA_CME_MAX_NUM_CELL_ID                      (WFW_DEMOD_NUM_MAX_CELLS)

/** Number of channel types for which receiver selection has to be done. */
#define MCALWCDMA_CME_NUM_RECEIVER_CHANNEL_TYPES           (3)

/** Index of the R99 receiver in "receiver_selection_bmsk." */
#define MCALWCDMA_CME_RX_IDX_FOR_R99                       (0)

/** Index of the HS receiver in "receiver_selection_bmsk." */
#define MCALWCDMA_CME_RX_IDX_FOR_HS                        (1)

/** Index of the non-master R99 receiver in "receiver_selection_bmsk." */
#define MCALWCDMA_CME_RX_IDX_FOR_NON_MASTER                (2)

#define MCALWCDMA_CME_CMN_RX_DEMOD_UPD_BMSK                (0x1)

#define MCALWCDMA_CME_CMN_OHCIC_UPD_BMSK                   (0x2)

/** Bit used to signal use of Rake in "receiver_selection_bmsk." */
#define MCALWCDMA_CME_RAKE_BIT                             (0x1)


/** Bit used to signal use of QICE in "receiver_selection_bmsk." */
#define MCALWCDMA_CME_QICE_BIT                             (0x4)

/* -----------------------------------------------------------------------
** Enumerations
** ----------------------------------------------------------------------- */
  typedef enum
  {
    MCALWCDMA_CME_INVALID_QSET_TYPE = 0,
    MCALWCDMA_CME_HS_CELL = 1,
    MCALWCDMA_CME_FIRST_INTF = 2,
    MCALWCDMA_CME_SECOND_INTF = 3,
    MCALWCDMA_CME_NUM_QSET_TYPE
  } mcalwcdma_cme_qset_cell_type_enum_type;

/* -----------------------------------------------------------------------
** Structures/Databases
** ----------------------------------------------------------------------- */

/* CME Common Info database */
typedef struct
{
  boolean enable;
  uint8 receiver_selection_bmsk[MCALWCDMA_CME_NUM_RECEIVER_CHANNEL_TYPES];
  uint8 ic_on_receiver_bmsk;
  uint8 ic_on_cell_id;
}mcalwcdma_cme_common_info_struct_type;

typedef struct
{
  uint16 scr_code;
  uint16 freq;
  boolean sttd;
  srch_cell_txd_det_status txd_status;
  uint8 cell_db_idx;
  uint16 filt_ecio;
  boolean aset_cell;
  boolean sec_cpich_enabled;
  uint16 sec_cpich_ovsf;
  boolean sec_cpich_enabled_mimo_mode;
  uint16 sec_cpich_ovsf_mimo_mode;
  uint8 sec_scr_code_index;

  uint8 num_fings;
  dl_tri_fing_pos_and_eng pos_and_eng[MSMHW_MAX_NUM_FINGERS];

  /* positions of the maximum energy paths in 1/8 chip units. */
  uint32  max_pos[ SRCH_MAX_PILOT_PEAKS ];
    
  /* Energies of the maximum energy paths */
  uint32  max_eng[ SRCH_MAX_PILOT_PEAKS ];

  /* positions of the maximum energy paths in 1/8 chip units. */
  uint32  max_pos_div[ SRCH_MAX_PILOT_PEAKS ];

  /* Energies of the maximum energy paths */
  uint32  max_eng_div[ SRCH_MAX_PILOT_PEAKS ];
 
  /* variable to tell triage if it can reliably use results from primary
   * or diversity antenna
   */
  boolean  srch_result_valid[MAX_NUM_ANTENNA];
} mcalwcdma_cme_cell_struct_type;

/* CME QSET Config Info  */
typedef struct
{
  /* Fields used by MCALWCDMA_CME. */

  /** EQ cell index used flag. */
  boolean used;

  /** Initialize flag, set when the cell_idx changes. */
  boolean init;

  /** Primary cell flag, which is set for the HS cell when one is present, or
   *  the strongest cell when one isn't.
   */
  boolean primary_cell;

  /** RxD enable flag. */
  boolean rxd_enable;

  /** QICE enable flag, set when QICE is to be used with this EQ cell, currently
   *  only during HS, but set for all cells, not just the HS cell.
   */
  boolean qice_en;

  /** PSC enable flag. */
  boolean psc_en;

  /** SSC enable flag. */
  boolean ssc_en;

  /** Reframe flag, set when RxD, TxD, or HS state changes. */
  boolean reframe;

  /** Sample server index.  Used for SW debug purposes. */
  uint8 ssid;

  /** Cell db index. */
  uint8 cell_idx;

  /** Center window position of the cell in CHIPx1. */
  uint32 pn_position;

  /** Potential new center window position of the cell in CHIPx1. */
  uint32 new_pn_position;

  /* Fields used by CME. */

  /** The primary scrambling code. */
  uint16 psc;

  /** The secondary scrambling code. */
  uint16 ssc;

  /** The CFN the last time this EQ cell was reframed. */
  uint16 cfn_at_last_reframe;

  /** Signifies of the "pn_position" field was calculated with the most recent
   *  SRCH results.  Reset in wl1_cme_srch_cell_triage_info_cb() and set
   *  whenever wl1_cme_frame_window_calc() is called for that cell.  Used by 
   *  wl1_cme_get_frame_window_pos() so that wl1_cme_frame_window_calc() isn't
   *  called multiple times for a cell on the same search results, since it is
   *  a MIPs-costly function.
   */
  boolean pn_position_recent;

  /** The primary counter of the cell, used to promote a cell to primary if it
   *  is stronger than the current primary cell.  Initialized to 0 and used by
   *  wl1_cme_primary_cell_selection().  If this cell is stronger than the
   *  current primary cell, then increment this counter.  When this counter
   *  exceeds "wl1_cme_oride_qset_primary_ctr_threshold" and there isn't an HS
   *  cell present than this cell is set to be primary.
   */
  uint8 primary_ctr;

  /** The drop counter of the cell, used to drop a cell from the QSET if it is
   *  too weak.  Initialized to 0 and used by wl1_cme_update_qset_drop_counters().
   *  If this cell is weaker than the Ec/Io drop threshold, then increment this
   *  counter.  When this counter exceeds "wl1_cme_oride_qset_drop_ctr_threshold"
   *  and this isn't teh HS cell, then drop it from the QSET.
   */
  uint8 drop_ctr;

   /** Ghost cell Drop Counter */
  uint8 ghost_drop_ctr;

  /** TD active on the cell. */
  boolean cpich_td;

  /** The current filtered Ec/Io of the cell.  This is set whenever a cell is
   *  added to the QSET and in wl1_cme_srch_cell_triage_info_cb().  It is used
   *  throughout the code, for example when update the drop counters,
   *  calculating PSET matrices, and when determining what the weakest QSET
   *  cell is.
   */
  uint16 ecio;
  uint8 num_qice_iterations;

  mcalwcdma_cme_qset_cell_type_enum_type qset_cell_type;

  /** Pointer to cell information, set whenever a cell is added to the QSET and
   *  updated whenever wl1_cme_srch_cell_triage_info_cb() is called. Used
   *  throughout the code.
   */
  mcalwcdma_cme_cell_struct_type * cell_ptr;

  /** Secondary scrambling code index. */
  uint8 sec_scr_code_index;

  /** Secondary CPICH enabled flag. */
  boolean sec_cpich_enabled;
 
  /** Secondary CPICH OVSF. */
  uint16 sec_cpich_ovsf;

  /** MIMO Mode Secondary CPICH enabled flag. */
  boolean sec_cpich_enabled_mimo_mode;

  /** MIMO Mode Secondary CPICH OVSF. */
  uint16 sec_cpich_ovsf_mimo_mode;
} mcalwcdma_cme_qset_cfg_params_struct_type;

typedef struct
{
  mcalwcdma_cme_common_info_struct_type common_info;
  mcalwcdma_cme_qset_cfg_params_struct_type cfg_params[MCALWCDMA_CME_MAX_NUM_QSET_CELLS];
} mcalwcdma_cme_cfg_params_struct_type;

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
typedef WfwDemodEqInfoStruct mcalwcdma_cme_eq_cfg_buffer_mdsp_struct_type;
typedef WfwDemodCommonInfoStruct  mcalwcdma_cme_common_info_mdsp_struct_type;

/* -----------------------------------------------------------------------
** Global Data/pointer Declarations
** ----------------------------------------------------------------------- */
  
/* -----------------------------------------------------------------------
 ** EQ Logging related definitions  
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/*============================================================================*/
/**
  * This function initializes pointers to all the CME async interface buffers
  */
/*============================================================================*/
void mcalwcdma_cme_init(void);

/*============================================================================*/
/**
  * Sets the CQI and Demod Algorithm Bitmasks.  For debug only.
  */
/*============================================================================*/
void mcalwcdma_cme_set_cqi_and_demod_algo_bmsk(
  uint8 cqi_algo_bmsk,
  uint8 demod_algo_bmsk);

void mcalwcdma_cme_update_cme_window(
  uint8 carrier,
  mcalwcdma_cme_cfg_params_struct_type* cme_cfg_params_ptr);

/*============================================================================*/
/**
  * This function updates the CME command interface buffers with parameters for config
  */
/*============================================================================*/
extern void mcalwcdma_cme_update_cme_cfg_params(
  uint8 carrier,
  uint16 eq_cfg_update_bmsk, 
  uint16 cmn_cfg_update_bmsk, 
  mcalwcdma_cme_cfg_params_struct_type* cme_cfg_params_ptr);

/*============================================================================*/
/**
  * This function compares current receiver config with proposed config and 
  * returns a mask indicating changes
  */
/*============================================================================*/
uint8 mcalwcdma_cme_set_receiver_upd_mask(
  uint8 carrier,
  mcalwcdma_cme_common_info_struct_type* cmn_cfg_params);

/*============================================================================*/
/**
  * This function updates the CME command interface buffers with parameters for IRPM config
  */
/*============================================================================*/
extern void mcalwcdma_cme_update_irpm_params(uint8 carrier, uint8 Tm, uint8 Ts, uint8 Td, uint32 delta);

/*============================================================================*/
/**
  * This function returns the current schic receiver config.
  */
/*============================================================================*/
uint8 mcalwcdma_cme_get_schicmask(
  uint8 carrier);

/*============================================================================*/
/**
  * This function sends the sync command to FW
  */
/*============================================================================*/
void mcalwcdma_cme_send_cfg_cmd(
  uint16 eq_cfg_update_bmsk,
  uint16 cmn_cfg_update_bmsk);

/*============================================================================*/
/**
  * This function is the callback for cme_demod_cfg cmd and is defined in whl1
  */
/*============================================================================*/
void mcalwcdma_cme_demod_cfg_cmd_done_cb(uint16 status);

/*============================================================================*/
/**
  * This function sets all CME cells to be disabled
  */
/*============================================================================*/
uint8 mcalwcdma_cme_disable_cme_cells(void);

/*============================================================================*/
/**
  * Configures QICE GEN2AXI masters.
  */
/*============================================================================*/
void mcalwcdma_cme_config_gen2axi_masters(void);

/*============================================================================*/
/**
  * This function is the callback for cme_oride_qice_cfg cmd and is defined in whl1.
  */
/*============================================================================*/
void mcalwcdma_cme_oride_qice_cfg_cmd_done_cb(uint16 status);

/*============================================================================*/
/**
  * This function is called when Signal is Raised by FW indicating that
  * previous CMD has been processed.
  */
/*============================================================================*/
void mcalwcdma_cme_demod_cfg_proc_done_cb(void);

/*============================================================================*/
/**
  * Sets the QICE Override Table Parameters async registers and sends the sync
  * command to the MDSP.
  */
/*============================================================================*/
void mcalwcdma_cme_process_qice_oride_table_params_cmd(
  /* Hyperbolic tangent look-up table. */
  uint32 * tanh_table,
  /* Square-root look-up table part 1. */
  uint32 * sqrt_table_1,
  /* Square-root look-up table part 2. */
  uint32 * sqrt_table_2,
  /* Parameter table. */
  uint32 * param_table);

/*============================================================================*/
/**
  * Sets the QICE Override Config Parameters async registers and sends the sync
  * command to the MDSP.
  */
/*============================================================================*/
void mcalwcdma_cme_process_qice_oride_config_params_cmd(
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

/*============================================================================*/
/**
  * Sets the QICE Override FHT Parameters async registers and sends the sync
  * command to the MDSP.
  */
/*============================================================================*/
void mcalwcdma_cme_process_qice_oride_fht_params_cmd(
  /* EQ index to override the FHT parameters for. */
  uint8 eq_index,
  /* FHT enable/disable for OVSF 256 bitmask. */
  uint32 * fht256,
  /* FHT enable/disable for OVSF 16 bitmask. */
  uint16 fht16);

/*============================================================================*/
/**
  * Sets the QICE Override FW Parameters async registers and sends the sync
  * command to the MDSP.
  */
/*============================================================================*/
void mcalwcdma_cme_process_qice_oride_fw_params_cmd(
  /* EQ index to override the QICE FW parameters for. */
  uint8 eq_index,
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

/*============================================================================*/
/**
  * Sets the QICE Reset async registers and sends the sync command to the MDSP.
  */
/*============================================================================*/
void mcalwcdma_cme_process_qice_reset_cmd(
  /* Bitmask which select which EQ/EQ's SFC calculations are reset. */
  uint8 reset_bmsk);

/*============================================================================*/
/**
  * Sets the QICE Override SFC Log Parameters aync registers and sends the sync
  * command to the MDSP.
  */
/*============================================================================*/
void mcalwcdma_cme_process_qice_sfc_log_oride_cmd(
  /* Number of HW dumps between SFC log entries. */
  uint8 hw_dump_per_log,
  /* Pointer to 3-element array of OVSFs to log for the 3 EQ cells. */
  uint8 * ovsf);

/*============================================================================*/
/**
  * Sets the QICE Override FFINV Log Parameters aync registers and sends the
  * sync command to the MDSP.
  */
/*============================================================================*/
void mcalwcdma_cme_process_qice_ffinv_log_oride_cmd(
  /* Number of WCT dumps between FFINV log entries. */
  uint8 wct_per_log,
  /* Bitmask of EQ indices to log. */
  uint8 eq_index_bmsk);

/*============================================================================*/
/**
  * Sets the QICE Override FHT and FW Parameters to default params.
  */
/*============================================================================*/
void mcalwcdma_cme_set_dflt_fht_and_fw_orides(
  /* Bitmask of EQ indices to set default QICE FHT/FW parameters for. */
  uint8 eq_index_bmsk);

/*============================================================================*/
/**
  * Returns 0 if CME config is not in progress.
  */
/*============================================================================*/
uint32 mcalwcdma_cme_get_config_pending_mask(void);

/*============================================================================*/
/**
  * Returns the rake cqi value last updated by FW
  */
/*============================================================================*/
uint16 mcalwcdma_cme_get_rake_cqi(uint16 carrier);

/*============================================================================*/
/**
  * Returns the eq cqi value last updated by FW
  */
/*============================================================================*/
uint16 mcalwcdma_cme_get_eq_cqi(uint16 carrier);

/*============================================================================*/
/**
  * Returns the qice cqi value last updated by FW
  */
/*============================================================================*/
uint16 mcalwcdma_cme_get_qice_cqi(uint16 carrier);

/*============================================================================*/
/**
  * Returns the eq PN pos last updated by FW for given cell
  */
/*============================================================================*/
uint16 mcalwcdma_cme_get_fw_eq_pn_pos(uint16 cell_db_idx);

/*============================================================================*/
/**
  * Returns the eq PN pos last configured by SW for given cell
  */
/*============================================================================*/
uint16 mcalwcdma_cme_get_qset_cell_configured_pn_pos(uint16 carrier, uint16 qset_idx);

/*============================================================================*/
/**
  * Displays the CME configuration.
  */
/*============================================================================*/
void mcalwcdma_cme_display_config(
  uint32 eq_cfg_update_bmsk,
  uint32 cmn_cfg_update_bmsk
  );

/*============================================================================*/
/**
  * Check if QICE is enabled on the given cell in FW at this time.
  */
/*============================================================================*/
boolean mcalwcdma_is_qice_enabled_on_cell(uint8 carr_idx, uint8 cme_idx);

#ifdef FEATURE_WCDMA_CIR_ENHANCEMENT
/*============================================================================*/
/**
  * Updates FW interface buffer with CIR NV values
  */
/*============================================================================*/
void mcalwcdma_cme_update_cir_nv_values(
  boolean cirInterpEnable,
  uint8 oneTapCirAlpha,
  boolean enableDopplerEstimation,
  uint16 doppler,
  uint16 sigmaScale,
  uint16 cirCleanThresh);
#endif

#endif /* MCALWCDMA_CME_H */

