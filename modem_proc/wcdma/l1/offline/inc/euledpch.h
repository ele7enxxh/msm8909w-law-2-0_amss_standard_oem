#ifndef EULEDPCH_H
#define EULEDPCH_H

/*==========================================================================
             WCDMA L1 HSUPA UL protocol operation related header file

DESCRIPTION
  This file contains definition, declaration required for HSUPA UL protocol
  procedures and algorithms

  Copyright (c) 2006-2007 by Qualcomm Technologies, Inc.
  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/euledpch.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/06/16   sg      Remove support to reuse SS tables, always rebuild.
01/26/15   cjz     QXDM command support for DC-HSUPA activation via fake hs-scch order 
10/06/14   ar      Compiler warnings fixed
10/06/14   ymu     Populate CM gap when UE is in CPC DTX gap
10/01/14   gv      Static to heap: EDPCCH boost global and time profiling
10/01/14   gv      Code cleanup. Removal of stale and unused code. 
08/21/14   cjz     Debugging logic to crash UE if EDL channel recfg delayed in WFW
08/11/14   yw      Re-evaluate T/P when both carriers going through ReTx, bug fixes, precision enhancement, code cleanup
07/28/14   pkg     Code Check-in for 16QAM feature.
07/17/14   ssv     Enhancing 0x422c log packet
06/23/14   pkg     Moving DC-HSUPA specific code under FEATURE_WCDMA_DC_HSUPA flag.
05/31/14   vr      Converting large local to dynamically allocated variable to avoid
                   stack overflow.
05/22/14   at      Code check in for to allow DCUPA CPC with QTA  
05/21/14   raj     Initial code checkin to allow CPC with QTA
05/15/14   ar      Fix for P_non_sg not allocated during no grant state
                   Fix for wrong power allocation for re-tx carrier
05/13/14   ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
05/12/14   gp      BOLT minimising stack allocation in L1 code-path
04/27/14   jkb     Add check for Feature dual sim with feature qta 
03/27/14   ymu     Fix CPC preample tx on Primary carrier when only secondary is Tx EDCH.
02/13/14   gp      EUL FIQ Changes for Bolt : New Macro for BPGs in a Subframe
01/17/14   vr      Remove debug code from CR459020
12/17/13   ash     Removed A2 Done ISR as well as A2 Done Sem Logic
12/17/13   yw      Enabling Rel7 E-DPCCH boost feature and Rel7 MPR
12/11/13   gp      FR:3693 EUL stats API for data services
11/04/13   yw      CM support for DCHSUPA
10/25/13   cjz     Validate EUL NW config to check if ETFCI 0 is supported
09/17/13   jkb     Porting Dual Sim from Triton to Dime
07/22/13   yw      UPH support for DCHSUPA
07/12/13   yw      CPC DTX support for DC-HSUPA
07/11/13   ymu     Support for secondary carrier OOS and recfg.
06/12/13   ymu     Fix KW errors, deadlock crash and 435B crash. 
06/12/13   at      Activating SS table immediately after configuration is finished. 
05/22/13   oh      Adding DC-HSUPA framework support
03/19/13   vr      Turn PA off as soon as EDCH resource release for HSRACH is initiated.
                   QXDM command control is present to NOT execute this.
01/04/13   raj     Changes as a part of ZI memory optimization
12/12/12   ash     Handling of scenario where EUL reconfig comes in 
                   the middle of reverting back after HHO failure and Clearing
                   the e_ul_reuse_ss_tables FLAG, everytime when EUL DROP happens.
09/20/12   pkg     Added support to handle very low MTPL values for UPH calculation
08/31/12   vr      Added HSRACH specific support to handle very low MTPL 
                   values for UPH calculation
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
03/28/12   zr      Added feature definitions for Dime
07/10/12   vr      Added UPH calculation specific to HSRACH
06/08/12   gv      Feature HS RACH Initial code check in
02/15/12   sp      Optimized MPR bin implementation to reduce MIPS01/10/12   sp      Optimized MPR bin implementation to reduce MIPS01/10/12   sp      Optimized MPR bin implementation to reduce MIPS
02/01/12   raj     Nikel Feature cleanup.
02/01/12   gnk     Nikel Feature cleanup.
02/01/12   gnk     Nikel feature cleanup
01/31/12   raj     Nikel feature cleanup. Mainlined the feature FEATURE_WCDMA_A2_HW_UL.
01/26/11   vs      Nikel feature cleanup. Mainlined the feature FEATURE_HSUPA_L1_LOGGING_ENABLED.
11/24/11   gnk     Removed unused features
04/28/11    sj     upmerge from offline/main
04/26/11   gnk     Removed FEATURE_WCDMA_HSUPA_IRQ_DELAY_FLOW_CONTROL feature
11/11/10   amj     Corrected blast include for the semaphore use.
10/26/10   gv/gnk  Validated DTX fixes and code review comments
10/26/10   gnk/gv  EUL IRQ split fix for Tx data stall/crash
10/05/10   ksr     Compiler warnings
09/15/10   ksr     Enhanced A2 logics with ERAM Readback, L1-L2 interace contents
                   dumping, Rd back profiling etc
09/14/10   ksr     Added Semaphore protection between build next TTI function and 
                   A2 DONE ISR Callback logics
09/02/10   gv      Code Checkin for CPC DTX
08/08/10   sj      Added unified PLT support
06/10/10   sa      MPR 7.1 support
04/09/10    ksr     Changes to move logging variables to dynamic memory approach
09/02/09   sa      EDPCCH power boost feature support: Added a structure type
                   for the new Aec semistatic table.
08/28/09   gv      Checkin EUL PP changes protected within the feature
                   DEBUG_FEATURE_WCDMA_PP_HW_UL
06/23/09   oh      Fixed declaration issue for SGP table.
05/26/09   oh      Fixed a merge issue.
05/22/09   oh      Support for Rel-7 EDPDCH power interpolation feature.
04/08/09   ka      Changed code to fill up MDSP MPR bins in
                   euledpch_write_mpr_bins_to_fw
01/30/09   ka      Added Support for Enhanced MPR algorithm
11/04/08   gnk     Added support for EDCH to continue TX over seamless reconfig.
01/16/09   oh      Replacing INTLOCK in EUL ISR with REX_ISR_LOCK.
10/13/08   oh      EUL IRQ f3 messages enabled under a debug flag.
10/03/08   hk      Bringing in 7k Mailine fixes
11/19/07   am      Mainlining athena rev2 changes & removing rev1 hw workarounds
09/26/07   oh      Fix for SGP as per the Spec 25.321 CR 0325.
08/13/07   oh      Added support to exclude DTX slots during CM for DPCCH, UPH meas.
08/13/07   oh      Fix to handle the case where NW sends higher ref (forbidden) ETFCI
02/21/07   am      Added code for applying SG at activation time
02/02/07   oh      Enhanced interface to enable various f3 messages for EUL.
01/30/07   yiz     Fix ETFC PR to use next frame MTPL_no_mpr
01/04/07   gnk     Lint Fixes
12/18/06   am      Add euledpch_uph_meas_update() to fix compiler warning
12/12/06   rgn     Added test_beta_ed_sf2 to the list of beta overrides for plt
12/06/06   am      Fix EUL L1/MAC log packet
11/20/06   yiz     Add runtime detection of HW EUL cipher support
11/15/06   yiz     Add euledpch_save_ss_tables()
11/10/06   am      Added code for enabling/disabling Pwr restrction through QXDM
09/29/06   rgn     Added logic to override the beta_ed calculated for plt
08/14/06   yiz     Mainline HSUPA code
02/28/06   yiz     Initial version

===========================================================================*/


/* ---------------------------- */
/* Define/unde local features   */
/* ---------------------------- */

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"
#include "l1macdata.h"
#include "l1rrcif.h"
#include "eulcfg.h"
#include "eulsg.h"


#include "mdspasync.h"
#include "rlcdsapif.h" 
/* ---------------------------- */
/* Check for necessary features */
/* ---------------------------- */
extern uint16 debug_etfci_value;
/* SLAMMING THE PDU DONE TIME IN Cx8*/
extern uint32 e_ul_pdu_done_time_cx8;

extern uint16 cpc_dtx_mac_inactivity_counter[WL1_MAX_UL_CARR_IDX][E_UL_LOG_DOUBLE_BUF][E_UL_LOG_MAX_SFN_INDEX];
extern boolean cpc_dtx_mac_index;

/*flags to be set through QXDM for dumping out L2 data sent to L1 */
extern boolean eulenc_debug_input_data_dump;

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* Max number of signaled delta E-DPDCH values */
#define EUL_MAX_NUM_DELTA_EDPDCH 30

/* Max number of signaled delta E-DPDCH values + DTX value */
#define EUL_MAX_NUM_DELTA_EDPDCH_WITH_DTX  (EUL_MAX_NUM_DELTA_EDPDCH + 1)

/* Number of beta_ed values for MPR: EUL_MAX_NUM_EDPDCH_VALUES_FOR_MPR will be
** 33 or 31 based on whether FEATURE_WCDMA_REL7P1_MPR is defined or not.*/
#ifdef FEATURE_WCDMA_REL7P1_MPR
  /* Number of beta_ed values for Rel7.1 MPR*/
#define EUL_MAX_NUM_EDPDCH_VALUES_FOR_MPR_B1 33
#endif /*FEATURE_WCDMA_REL7P1_MPR*/
#define EUL_MAX_NUM_EDPDCH_VALUES_FOR_MPR_B0 EUL_MAX_NUM_DELTA_EDPDCH_WITH_DTX

/* value of HARQ ID to indicate it is inactive */
#define  E_UL_HARQ_INACTIVE  0xFFFF

/* MAX number of E-DCH CM factor possibilities after configuration */
#define EUL_MAX_NUM_CM_PATNS_PER_CFG  8

/* Max number of signaled HARQ power offset values */
#define EUL_MAX_NUM_HARQ_PWR_OFFSET 7

#ifdef FEATURE_WCDMA_REL7
/*Max (harq power offset num, max num of CM patterns per config)*/
#define EUL_MAX_OF_HARQ_PO_AND_CM_PATNS_PER_CFG\
           ((EUL_MAX_NUM_HARQ_PWR_OFFSET > EUL_MAX_NUM_CM_PATNS_PER_CFG) ?\
             EUL_MAX_NUM_HARQ_PWR_OFFSET : EUL_MAX_NUM_CM_PATNS_PER_CFG)

#endif/*FEATURE_WCDMA_REL7*/

/* MAX number of E-DCH CM factor possibilities after configuration */
#define EUL_MAX_NUM_CM_PATNS_PER_10MS_CFG  EUL_MAX_NUM_CM_PATNS_PER_CFG

/* MAX number of E-DCH CM factor possibilities after configuration */
#define EUL_MAX_NUM_CM_PATNS_PER_2MS_CFG  3

/* normalization constant for R_j and R_combo, 2^14 */
#define EUL_PWR_TAB_NORM_SHFT 14

/* CM factor normalization constant, 2^14 */
#define EUL_CM_FACTOR_NORM_SHFT 14

/* HARQ Power Offset factor normalization constant, 2^14 */
#define EUL_HARQ_PWR_OFFSET_FACTOR_NORM_SHFT 14

/* Invalid CFN to suspend build_frame_ind()  */
#define EUL_CFN_TO_SUSPEND_BFI_INVALID  ((uint16)-1)

/*if this value is set then EDPCH is dtxed in the next opportunity.So in FIQ,
if this value is encountered then euledpch module starts dtxing the EDPCH*/
#define EUL_CFN_TO_SUSPEND_BFI_NOW  ((uint16)-2)

/* Chipx8 per sub-frame */
#define CHIPX8_PER_SUB_FRAME 61440

#ifdef FEATURE_WCDMA_BOLT_2_0
#define NUM_BPG_PER_SUB_FRAME 30
#endif

/*This is invalid value for DPCCH power or MTPL that FW programs when in CM or CPC DTX*/
#define EUL_INVALID_TXAGC_VAL 0xFBFC

#define EUL_FW_MIN_MTPL_DPCCH_DB10 392

/*Number of values possible for signaled Delta-E-DPDCH */
#define EUL_MAX_A_ED_UNQUAN_IDX 168

#ifdef FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST
/*Number of possible values for signalled delta-T2TP*/
#define EUL_NUM_DELTA_T2TP 7

/*Number of values possible for signaled Delta-E-DPDCH with EDPCCH power boost*/
#define EUL_MAX_NUM_DELTA_EDPDCH_WITH_EDPCCH_PWR_BOOST 32

/*Max ETFCI*/
#define EUL_MAX_ETFCI 127

/*Maximum A_ecx15 with EDPCCH power boost*/
#define EUL_MAX_A_EC_UNQUAN_IDX 239

/*Maximum A_edx15 with EDPCCH power boost*/
#define EUL_MAX_A_ED_UNQUAN_IDX_WITH_EDPCCH_PWR_BOOST 377

/*The value of e_tfci_boost_val signalled by RRC when not given in the OTA*/
#define EUL_INVALID_ETFCI 0XFF

/*The value of delta_t2tp signalled by RRC when not given in the OTA*/
#define EUL_INVALID_T2TP_VAL 0xFF

/* T2TP stored is T2TP << 14 */
#define EUL_T2TP_SHFT 14                  

/* Resultant bit-shift after taking square root of the appropriate pwr Aec table 
** entry. This is actually EUL_PWR_TAB_NORM_SHFT/2 */
#define EUL_A_EC_UNQUAN_SHIFT 7

/* Number of pages in the pwr Aec table double buffer*/
#define EUL_NUM_A_EC_TABLE_PAGES 2

extern boolean apply_boost;
#endif /*FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST*/

#ifdef FEATURE_WCDMA_DC_HSUPA 
/*Used for Power allocation calculations.*/
#define EUL_NORM_SHFT_HIGH_SG 12
//#define IS_DCHSUPA_DC_ACTIVE(carr_idx) ((e_ul_sec_edpch_state == E_UL_SEC_EDPCH_STATE_TX) || (carr_idx == WL1_PRI_UL_CARR_IDX))
#endif

/* Invalid datarate for EUL */
#define EUL_INVALID_DATA_RATE 0xFFFF

#define IS_DCHSUPA_SEC_CARR_ACTIVE() ((e_ul_sec_edpch_state == E_UL_SEC_EDPCH_STATE_TX)||(e_ul_sec_edpch_state == E_UL_SEC_EDPCH_STATE_DISABLED) \
             || (e_ul_sec_edpch_recfg_state == E_UL_SEC_CFG_WHEN_ACTIVE))

#define EUL_NO_GRANT_EITHER_CARR(pri_sg_idx, sec_sg_idx) ((pri_sg_idx >= EUL_MAX_NUM_ACTIVE_SG) && (sec_sg_idx >= EUL_MAX_NUM_ACTIVE_SG))




/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Structure to contain one set of N_e_data for one TTI/CM combination */
typedef struct
{
  /* 10 ms TTI:
   *   MAX number of n_txl (25.212 4.4.5.1) is EUL_MAX_NUM_CM_PATNS_PER_CFG = 8
   *   For TX in CM, or re-trans of E-DCH that was initially TX'ed in CM,
   *     N_e_data shall be scaled by n_txl/15
   * 2 ms TTI:
   *   Only normal mode N_e_data is needed.
   * Indexed by l1_e_max_ch_codes_enum_type
   * Values are bits per TTI
   */
  uint16 n_e_data_tab[L1_NUM_L1_E_MAX_CH_CODES_SF];

} eul_n_e_data_tab_struct_type;

/* Derived L1 EDPCH configuration parameters */
typedef struct
{
  /* Is DPDCH configured ? */
  boolean dpdch_cfged;

  /* MAX channelization code, lower rate of (UTRAN, UE_capability, DPDCH_cfged) */
  l1_e_max_ch_codes_enum_type max_chan_code;

  /* 33 for 2xSF2+2xSF4, or 44 for fewer codes */
  uint32 pl_max;
} eul_max_rate_cfg_param_struct_type;

#define EUL_ETFC_FORBIDDEN   L1_NUM_L1_E_MAX_CH_CODES_SF
/* Semi-static table of E-TFC coding, which depends on the following:
 *   TTI, E-TFC table index, max chan code, puncture limits
 */
typedef struct
{
  /* num of NM/CM possibilities in etfc_coding_tab[] */
  uint32  num_cm_for_sf;

  /* num of supportable E-TFCI for each CM pattern */
  uint8   num_etfci_tab[EUL_MAX_NUM_CM_PATNS_PER_CFG];

  /* indexed by E-TFCI and (15 - num_tx_slots_in_frame)
   * value as l1_e_max_ch_codes_enum_type
   * EUL_ETFC_FORBIDDEN indicates cannot support
   * row major: left most index change means large memory address difference
   */
  l1_e_max_ch_codes_enum_type etfc_coding_tab[EUL_MAX_NUM_CM_PATNS_PER_CFG][L1_UPA_UL_MAX_NUM_ETFC_PER_TAB];

} eul_etfc_coding_tab_struct_type;

/* Semi-static table of CM factors for power gains, which depends on:
 *   TTI, DPCCH normal mode slot format
 */
typedef struct
{
  /* num of NM/CM possibilities in pwr_cm_factor_tab[] */
  uint32  num_pwr_cm_factors;

  /* 10ms TTI:
   *   num_pwr_cm_factors = 8
   *   Indexed by (15 - num_tx_slots_in_frame)
   *
   * 2 ms TTI:
   *   num_pwr_cm_factors = 1 for Normal Mode slot format 1 and 3
   *   num_pwr_cm_factors = 3 for Normal Mode slot format 0 and 2
   */
  uint32 pwr_cm_factor_tab[EUL_MAX_NUM_CM_PATNS_PER_CFG];
} eul_pwr_cm_factor_tab_struct_type;

/* Semi-static tables of SGP
 * Serving Grant Payload calculations as per spec 25.321 new CR 0325
 * Normalization factor = (0x1 << EUL_PWR_TAB_NORM_SHFT)
 */
typedef struct
{
  /* num of NM/CM possibilities in etfc_coding_tab[]
   * Number of CM possibilities for R_j and R_combo are determined by power CM factors */
  uint32  num_pwr_cm_factors;

  /* 10ms TTI:
   *   num_pwr_cm_factors = 8
   *   Indexed by (15 - num_tx_slots_in_frame)
   *
   * 2 ms TTI:
   *   num_pwr_cm_factors = 1 for Normal Mode slot format 1 and 3
   *   num_pwr_cm_factors = 3 for Normal Mode slot format 0 and 2
   */

  /* SG Payload table */
  #ifdef FEATURE_WCDMA_REL7
  /*  While calculating SGP (0..37) using EDPDCH power interpolation, in the SGP SS Table, SGP is
   *  calculated for all possible values of SG, for each reference ETFCI and for each possible
   *  harq power offset (0..7). The CM factor is taken into consideration later during FIQ. 
   *  So we define the first index of array to be max of (harq po num, max num of CM patterns per config)
   *  as either interpolation or extrapolation can be configured when Rel7 is supported. When 
   *  interpolation is configured, SGP table below is used to store results of first part of the formula
   *  (Serving_grant)  * (Ke_ref_2 - Ke_ref_1)/
   *  (  (10 ^ delta_harq/10) ( (L_e_ref_2* A_ed_ref_2x15 * A_ed_ref_2x15) - 
   *     (L_e_ref_1* A_ed_ref_1x15 * A_ed_ref_1x15) )
   *  )  
   * As per spec 25.321 Section  11.8.1.4 
   */
  uint64 SGP[EUL_MAX_OF_HARQ_PO_AND_CM_PATNS_PER_CFG][L1_MAX_REF_E_TFCI_LIST][EUL_MAX_NUM_ACTIVE_SG];
  #else
  uint64 SGP[EUL_MAX_NUM_CM_PATNS_PER_CFG][L1_MAX_REF_E_TFCI_LIST][EUL_MAX_NUM_ACTIVE_SG];
  #endif /*FEATURE_WCDMA_REL7*/
  
} eul_pwr_SGP_val_tab_struct_type;

#ifdef FEATURE_WCDMA_REL7
   /*As per spec 25.321 Section  11.8.1.4, calculating SGP when
     EDPDCH power interpolation is configured
   * Table to store results for implementing second part of formula
   * (L_e_ref_1* A_ed_ref_1x15 * A_ed_ref_1x15) * (Ke_ref_2 - Ke_ref_1)/
   * ( (L_e_ref_2* A_ed_ref_2x15 * A_ed_ref_2x15) - 
   *   (L_e_ref_1* A_ed_ref_1x15 * A_ed_ref_1x15)
   * ) 
   * result of first part of SGP calculations is stored in the above SGP tab.
   */

typedef struct
{
  uint32 SGP_interp_offset_tab[L1_MAX_REF_E_TFCI_LIST];
} eul_pwr_SGP_val_interp_offset_tab_struct_type;

#endif /*FEATURE_WCDMA_REL7*/

#ifdef FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST/* E-DPCCH power boost additions */
/* Semistatic table structure type to hold A_ec,i^2/harq_pwr_offset for each 
** ETFCI and for each CM factor combination and HARQ power offset.*/
typedef struct
{
  /* Indicates if E-DPCCH power boosting is enabled. This flag is updated 
  ** at appropriate tab_index during configuration time. */
  boolean edpcch_power_boost_enabled;

  /* 3-dimensional table to store scaled EDPCCH power*/
  uint32 pwr_A_ec_norm[EUL_MAX_NUM_HARQ_PWR_OFFSET][EUL_MAX_NUM_CM_PATNS_PER_CFG][L1_UPA_UL_MAX_NUM_ETFC_PER_TAB];
}eul_pwr_A_ec_norm_tab_struct_type;

/* double buffer structure type having semi-static tables to store A_ec,i^2/harq_pwr_offset
** A_ec,i - E-DPCCH amplitude ratio with E-DPCCH power boost*/
typedef struct
{
  /* 0 or 1 for double buffer, EULEDPCH_DBL_BUF_NEITHER_ACTIVE for no-active
   buffer */
  uint32  curr_tab_index;

  /* current inactive table index */
  uint32  inactive_tab_index;

  /* buffer index for revert/resume */
  uint32 saved_idx;

  /* Whether need to switch buffers at activation */
  boolean need_to_switch;

  /* Is table built and ready for activation */
  boolean table_ready;

  /* Semi-static table to store A_ec,i^2/harq_pwr_offset for each E-TFCI*/
  eul_pwr_A_ec_norm_tab_struct_type  pwr_A_ec_norm_tabs[EUL_NUM_A_EC_TABLE_PAGES];
} eul_pwr_A_ec_norm_tab_dbl_buf_struct_type;
#endif /*FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST*/

/* Semi-static tables of R_j and R_combo
 * R_j is used in calculating beta_Ed
 * R_combo is used in E-TFC POwer Restriction
 * Normalization factor = (0x1 << EUL_PWR_TAB_NORM_SHFT)
 */
typedef struct
{
  /* num of NM/CM possibilities in etfc_coding_tab[]
   * Number of CM possibilities for R_j and R_combo are determined by power CM factors */
  uint32  num_pwr_cm_factors;

  /* 10ms TTI:
   *   num_pwr_cm_factors = 8
   *   Indexed by (15 - num_tx_slots_in_frame)
   *
   * 2 ms TTI:
   *   num_pwr_cm_factors = 1 for Normal Mode slot format 1 and 3
   *   num_pwr_cm_factors = 3 for Normal Mode slot format 0 and 2
   */

  /* num of supportable E-TFCI for each CM pattern */
  uint8  num_etfci_tab[EUL_MAX_NUM_CM_PATNS_PER_CFG];

  /* R_j table */
  uint32 pwr_R_j[EUL_MAX_NUM_CM_PATNS_PER_CFG][L1_UPA_UL_MAX_NUM_ETFC_PER_TAB];

  /* R_combo table */
  #ifdef FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST
  /*With EDPCCH power boost, Rcombo values can be greater than 2^32.
  ** So, use 64 bit entries*/
  uint64 pwr_R_combo[EUL_MAX_NUM_CM_PATNS_PER_CFG][L1_UPA_UL_MAX_NUM_ETFC_PER_TAB];
  #else
  uint32 pwr_R_combo[EUL_MAX_NUM_CM_PATNS_PER_CFG][L1_UPA_UL_MAX_NUM_ETFC_PER_TAB];
  #endif  /*FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST*/

} eul_pwr_R_val_tab_struct_type;

/*enum to indicate type of Tx during power allocation*/
typedef enum
{
  /* Re Tx on both carriers */
  EUL_EDPCH_RE_TX_BOTH_CARR,

  /* Re Tx on current carrier, New Tx on other carrier*/
  EUL_EDPCH_RETX_CURR_CARR,

  /* New Tx on current carrier, Re Tx on other carrier*/
  EUL_EDPCH_NEW_TX_CURR_CARR,

  /* NewTx on both carriers */
  EUL_EDPCH_NEW_TX_BOTH_CARR

}eul_edpch_new_tx_ind_type;

/* EDCH SRB delay state */
typedef enum
{
  /* initial state before EDPCH config */
  EULEDPCH_SRB_STATE_INIT,

  /* normal TX state to include SRB */
  EULEDPCH_SRB_STATE_TX,

  /* DCH SRB delay period */
  EULEDPCH_SRB_STATE_OMIT,

  /* one frame lagging of E-DCH after DCH in frame building */
  EULEDPCH_SRB_STATE_SWITCHING
} euledpch_srb_state_enum_type;

#ifdef FEATURE_WCDMA_MODEM_TPUT_STATS_API 

/* Default filter co-eff for Datarate */
#define EUL_ALLOWED_THROUGHPUT_DEFAULT_FILTER_COEFF 50

/* Plus flag threshold co-eff */
#define EUL_PLUS_FLAG_PERCENT_THRESHOLD 20

/* Upscale factor for 2ms TTI */
#define EUL_2MS_SCALING_FACTOR_FOR_ALLOWED_DATARATE   500

/* Upscale factore for 10ms TTI */
#define EUL_10MS_SCALING_FACTOR_FOR_ALLOWED_DATARATE  100

/* Bitmask to enable datarate related F3's */
#define WCDMA_STATS_ALLOWED_DATARATE_LOGGING_BMSK  0x0001

/* Bitmask to enable plus flag related F3's */
#define WCDMA_STATS_PLUS_FLAG_LOGGING_BMSK         0x0004

typedef struct {
  /* HARQ Power offset for last TTI */
  uint8 harq_power_offset[WL1_MAX_UL_CARR_IDX];

  /* Max allowed bits according to available power */
  uint32 allowed_power_bits[WL1_MAX_UL_CARR_IDX];

  /* Max allowed bits according to available SG */
  uint32 allowed_sg_bits[WL1_MAX_UL_CARR_IDX];

  /* Filtered Maximum allowed data rate in bits per second */
  uint32 allowed_eul_data_rate;

  /* Indicates if Filtered Allowed data rate need to be accumulated */
  boolean skip_eul_tput_stats_accum;

  /* Indicates if counters needs to updated on TTI basis*/
  boolean update_data_rate_counter_tti;

  /* Counter for number of times L2 indicated that higher data rate is possible 
     as TRUE */
  uint16 increase_data_rate_counter;

  /* Counter for number of times L2 indicated that higher data rate as TRUE 
     or FALSE */
  uint16 new_tx_for_tti_counter;

  /* Plus flag indicated value from L2 per carrier for per TTI computation */
  boolean increase_data_rate_flag_carr[WL1_MAX_UL_CARR_IDX];

  /* ETFCI selected bits by MAC for debug */
  uint32 mac_selected_bits[WL1_MAX_UL_CARR_IDX];

  /* QXDM Control: Set to TRUE if allowed datarate filter needs to be set to different filter
     co-efficient value for debugging */
  boolean is_dbg_datarate_filter_coeff;

  /* QXDM Control: debug EUL data rate filter coeff */  
  uint32 eul_datarate_filter_coeff;

} euledpch_stats_struct_type;

extern euledpch_stats_struct_type eul_stats;
extern rex_crit_sect_type eul_stats_lock;
#endif /* FEATURE_WCDMA_MODEM_TPUT_STATS_API */

#if defined(FEATURE_WCDMA_PLT) || defined(FEATURE_WCDMA_PLT_MODE)
#error code not present
#endif /*  #if defined(FEATURE_WCDMA_PLT) || defined(FEATURE_WCDMA_PLT_MODE) */

/* Index for empr params database. It's a double
 * buffer FW async structure */
extern uint16 empr_param_db_idx;

#ifdef FEATURE_WCDMA_DC_HSUPA 
/*Initialized to 0 as it's toggled before populating */
uint16 empr_dc_fw_idx;
#endif /*FEATURE_WCDMA_DC_HSUPA  */


/* ==========================================================================
** Public Data Declarations
** ========================================================================*/

/* current in use E-TFC table */
extern const l1_upa_ul_etfc_table_struct_type* eul_curr_etfc_tab_ptr;

/* current in use derived L1 EDPCH max_chan_code and pl_max */
extern eul_max_rate_cfg_param_struct_type* eul_curr_max_rate_cfg_param_ptr;

/* current in use semi-static table of E-TFC coding */
extern eul_etfc_coding_tab_struct_type* eul_curr_etfc_coding_tab_ptr;

/* current in use semi-static table of power CM factors */
extern eul_pwr_cm_factor_tab_struct_type* eul_curr_pwr_cm_factor_tab_ptr;

/* current in use semi-static table of R_j and R_combo */
extern eul_pwr_R_val_tab_struct_type* eul_curr_pwr_R_val_tab_ptr;

#if defined(FEATURE_WCDMA_PLT) || defined(FEATURE_WCDMA_PLT_MODE)
#error code not present
#endif /*  #if defined(FEATURE_WCDMA_PLT) || defined(FEATURE_WCDMA_PLT_MODE) */

#if defined(FEATURE_QTA) && defined(FEATURE_WCDMA_CPC_DTX)
extern int16  e_ul_cpc_dtx_qta_block_cnt[WL1_MAX_UL_CARR_IDX];
extern uint16 e_ul_cpc_dtx_qta_block_X;
extern int16  e_ul_cpc_dtx_qta_block_Y;
extern uint16 e_ul_cpc_dtx_qta_block_cfg_X;
extern int16  e_ul_cpc_dtx_qta_block_cfg_Y;
#endif

#if defined(FEATURE_QTA) && defined(FEATURE_WCDMA_CPC_DRX)
extern boolean e_ul_cpc_drx_was_in_qta_gap;
extern uint16 e_ul_cpc_drx_grant_monitor_2ms;
extern uint16 e_ul_cpc_drx_grant_monitor_10ms ;
#endif

extern boolean euledpch_validate_nw_config(e_ul_cfg_info_struct_type* config_param_ptr);

extern uint32 ul_update_pdu_count;

/* Timer to detect if FW is stuck during EDL reconfig */
extern timer_type e_dl_config_timeout_timer;

#ifdef FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST
/* Array to store sum of EDPCCH and EDPDCH powers over which ETFCI 
** restriction due to MP is done. */
extern uint64 *pwr_A_ec_norm_R_comb_sum_tab;
#endif /*FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST*/

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
#if defined(FEATURE_QTA) && defined(FEATURE_WCDMA_CPC_DTX)  
/* Mostly its 42 ms, so have chosen as 5 CFN = 50 ms */
#define CPC_DTX_QTA_PREPARE_TIME 5  
#endif

#define EULEDPCH_NUM_UPH_MEAS_FILTER_FR  10

 typedef struct
 {
   /* filtered uph to report */
   int32  curr_uph;
 
   /* element in circular buffer to update next */
   uint32  update_idx;
 
   /* number of filtered frames in history */
   uint32  num_filtered_fr;
 
   int32  sum_uph_history;
 
   /* history of UPH for each frame in the past */
   uint8   uph_history[EULEDPCH_NUM_UPH_MEAS_FILTER_FR];
 
  #ifdef FEATURE_WCDMA_HS_RACH
   /* Flag to check if first UPH measurement is done */
   boolean uph_first_meas_done;
   /* Flag to monitor only first EUL Tx */
   boolean uph_first_eul_tx_done;
  #endif /* FEATURE_WCDMA_HS_RACH */
 } euledpch_ups_meas_struct_type;
 
 /* Data structure for UPH meas */
extern euledpch_ups_meas_struct_type euledpch_ups_meas_buf;

/* EDCH SRB delay state */
extern euledpch_srb_state_enum_type euledpch_srb_state;

/* history buffer for MTPL_no_mpr for ETFC PR
 * mod 4 is much easier than mod 3
 */
#define EULEDPCH_MTPL_NO_MPR_AND_DIP_CM_DELTA_HIST_MOD_2MS_MASK  0x7
#define EULEDPCH_MTPL_NO_MPR_AND_DIP_CM_DELTA_HIST_MOD_10MS_MASK  0x3
extern uint16 euledpch_mtpl_no_mpr_hist[EUL_MAX_HARQ_RTT];

/* history buffer for DPCCH_Initial_Pwr + CM_delta for ETFC PR and UPH
 * mod 4 is much easier than mod 3
 */
extern int32 euledpch_dip_cm_delta_hist[EUL_HARQ_RTT_10MS];

extern uint16 euledpch_cfn_to_suspend_bfi;

#ifdef FEATURE_WCDMA_DC_HSUPA 
extern uint16 euledpch_sec_ul_cfn_to_suspend_bfi;
#endif /* FEATURE_WCDMA_DC_HSUPA */

extern uint16     eul_cpc_dtx_cache_sgp[WL1_MAX_UL_CARR_IDX];
extern uint8      eul_cpc_dtx_cache_max_etfci_pwr[WL1_MAX_UL_CARR_IDX];

/* Changing the cap values dynamically (Learning algo )
** will be required only for initial estimation. This
** estimation will be mostly a one time activity and
** after that the code may not be exercised.
*/
extern boolean eul_enable_irq_fc_learning_algo;
extern boolean dbg_eul_f3_enable;



#ifdef FEATURE_WCDMA_REL7P1_MPR
extern boolean debug_rel7_mpr_logging;
#endif /*FEATURE_WCDMA_REL7P1_MPR*/


/* double buffer of semi-static tables of R_j and R_combo */
typedef struct
{
  /* 0 or 1 for double buffer, EULEDPCH_DBL_BUF_NEITHER_ACTIVE for no-active
   buffer */
  uint32  curr_tab_index;

  /* current inactive table index */
  uint32  inactive_tab_index;

  /* buffer index for revert/resume */
  uint32 saved_idx;

  /* Whether need to switch buffers at activation */
  boolean need_to_switch;

  /* Is table built and ready for activation */
  boolean table_ready;

  /* double buffer of semi-static tables of R_j and R_combo */
  eul_pwr_R_val_tab_struct_type  pwr_R_val_tabs_buf[2];
} eul_pwr_R_val_tab_dbl_buf_struct_type;

/* double buffer of semi-static table of SGP */
typedef struct
{
  /* 0 or 1 for double buffer, EULEDPCH_DBL_BUF_NEITHER_ACTIVE for no-active
   buffer */
  uint32  curr_tab_index;

  /* current inactive table index */
  uint32  inactive_tab_index;

  /* buffer index for revert/resume */
  uint32 saved_idx;

  /* Whether need to switch buffers at activation */
  boolean need_to_switch;

  /* Is table built and ready for activation */
  boolean table_ready;

  /* double buffer of semi-static tables of R_j and R_combo */
  eul_pwr_SGP_val_tab_struct_type  pwr_SGP_val_tabs_buf[2];
} eul_pwr_SGP_val_tab_dbl_buf_struct_type;

extern eul_pwr_R_val_tab_dbl_buf_struct_type *eul_pwr_R_val_tab_dbl_buf_ptr;
extern eul_pwr_SGP_val_tab_dbl_buf_struct_type *eul_pwr_SGP_val_tab_dbl_buf_ptr;
#ifdef FEATURE_WCDMA_ZI_OPT_PHASE2
extern eul_pwr_A_ec_norm_tab_dbl_buf_struct_type* eul_pwr_A_ec_norm_tab_dbl_buf_ptr;
#endif

#ifdef FEATURE_WCDMA_DC_HSUPA
extern uint8 rumi_ignore_disable_tx;
#endif /* FEATURE_WCDMA_DC_HSUPA */

#ifdef FEATURE_WCDMA_16_QAM
/* Global for 16QAM QXDM command. */
extern uint32 eul_16_qam_qxdm_debug;

/* Bitmasks to check which bit is set in QXDM command. */
#define GET_16_QAM_QXDM_BMSK(x)         (eul_16_qam_qxdm_debug & x)

/* Bitmasks that can be set in above QXDM command. */
#define BMSK00 0x0000007F
#define BMSK01 0x00000080
#define BMSK02 0x00000100
#define BMSK03 0x00000200
#define BMSK04 0x00000400
#define BMSK05 0x00000800
#define BMSK06 0x00001000
//#define BMSK07 0x00002000
//#define BMSK08 0x00004000
//#define BMSK09 0x00008000
//#define BMSK10 0x00010000
//#define BMSK11 0x00020000
//#define BMSK12 0x00040000
//#define BMSK13 0x00080000
//#define BMSK14 0x00100000
//#define BMSK15 0x00200000
//#define BMSK16 0x00400000
//#define BMSK17 0x00800000
//#define BMSK18 0xFF000000

//#define BMSK0405 (BMSK04 | BMSK05)
//#define BMSK0809 (BMSK08 | BMSK09)
//#define BMSK1112 (BMSK11 | BMSK12)
//#define BMSK1617 (BMSK16 | BMSK17)
#endif /* FEATURE_WCDMA_16_QAM */


/* =======================================================================
**                        Function Declarations
** ======================================================================= */
/*==========================================================================
FUNCTION:     EUL_EDPCH_IS_TRANSMITTING()

DESCRIPTION:  This function returns TRUE if the eul state is in TX state otherwise it returns false

DEPENDENCIES: None

RETURN VALUE: TRUE if eul is in Tx state FALSE otherwise

SIDE EFFECTS: None

==========================================================================*/
extern boolean eul_edpch_is_transmitting(void);

/*===========================================================================
FUNCTION  EULEDPCH_INIT

DESCRIPTION
  This function initiliazes HSUPA UL protocol module at power up.

DEPENDENCIES
  Must be called only once at power up.

RETURN VALUE
  none

SIDE EFFECTS
  initilization to make HSUPA UL protocol module usable
===========================================================================*/
void euledpch_init(void);

/*===========================================================================
FUNCTION  EULEDPCH_PROCESS_CFG_DATA

DESCRIPTION
  This function processes E-DPCH cfg data and may build the semi static tables

DEPENDENCIES
  old and new confg params in e_ul_cfg_info_struct_type pointer

RETURN VALUE
  None

SIDE EFFECTS
  May build semi-static tables for E-DPCH
===========================================================================*/
void euledpch_process_cfg_data
(
  e_ul_cfg_info_struct_type *config_param_ptr
);

/*===========================================================================
FUNCTION  EULEDPCH_ACTIVATE_SS_TABLES

DESCRIPTION
  This function makes built Semi-Static tables active

DEPENDENCIES
  Non

RETURN VALUE
  None

SIDE EFFECTS
  As above
===========================================================================*/
void euledpch_activate_ss_tables(void);

/*===========================================================================
FUNCTION  EULEDPCH_PROCESS_EDCH_FOR_NEXT_TTI

DESCRIPTION
  This function is called by Serving Grant module to process E-DCH for next
  TTI's transmission in FIQ.

DEPENDENCIES
  Called by Serving Grant module in FIQ

RETURN VALUE
  void

SIDE EFFECTS
  EUL UL will do E-TFC power restriction, ask MAC/RLC to build data, and ask
  FW and HW to transmit the data for new transmission, or EUL UL will ask
  FW and HW to re-transmit the HARQ
===========================================================================*/
void euledpch_process_edch_for_next_tti
(
  eul_edch_harq_sg_info_struct_type *harq_sg_info_ptr, /* parameters from SG module */
  l1_eul_pwr_offset_req_struct_type *eul_pwr_offset_si_req_info,
  boolean                           *next_tti_dtx
);

/*===========================================================================
FUNCTION  EULEDPCH_UPH_MEAS_RESET

DESCRIPTION
  This function resets UPH measurements

DEPENDENCIES
  Non

RETURN VALUE
  None

SIDE EFFECTS
  resets UPH measurements
===========================================================================*/
void euledpch_uph_meas_reset(void);


/*===========================================================================
FUNCTION  EULEDPCH_TX_DISABLE

DESCRIPTION
  This function is called by ENC module to disable E-DPCH for out-of-sync

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  E-DPCH DTX
===========================================================================*/
void euledpch_tx_disable(void);

/*===========================================================================
FUNCTION    EULEDPCH_TX_DISABLE_DURING_RECFG

DESCRIPTION This function is called by l1msetup/seq module to disable
            E-DPCH at activation time when EDPCH is recfged

DEPENDENCIES This function should be called only at activation time and
             only when EDCH is recfged

RETURN VALUE None

SIDE EFFECTS E-DPCH DTXed
===========================================================================*/
void euledpch_tx_disable_during_recfg(void);
/*===========================================================================
FUNCTION        EULEDPCH_SUSPEND_BFI_ONE_FRAME_BEFORE_ACT_TIME

DESCRIPTION     This function sets cfn corresponding to act_time -1 to suspends
                build_fram_ind one frame before the activation time till eul new cfg is
                activated. The EDPCH is DTXed during this time

DEPENDENCIES    Must be called within WCDMA_INTLOCK

RETURN VALUE    None

SIDE EFFECTS    As said above
==========================================================================*/
void euledpch_suspend_bfi_one_frame_before_act_time(uint16 local_act_time, wl1_ul_carr_id_enum_type carr_idx);


/*===========================================================================
FUNCTION  EULEDPCH_TX_RE_ENABLE

DESCRIPTION
  This function is called by ENC module to re-enable E-DPCH after out-of-sync

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  E-DPCH resumes
===========================================================================*/
void euledpch_tx_re_enable(void);

#ifdef FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST
/*===========================================================================
FUNCTION  EULEDPCH_SET_EDPCCH_POWER_BOOST_ENABLED_FLAG

DESCRIPTION
This function updates "edpcch_power_boost_enabled" flag in the inactive index
of the Aec table double buffer "eul_pwr_A_ec_norm_tab_dbl_buf", indicating 
whether EDPCCH power boost is enabled or not in the new config.
 
DEPENDENCIES
The boolean value to be assigned to "edpcch_power_boost_enabled"
 
RETURN VALUE
  
SIDE EFFECTS
The function also updates "inactive_tab_index" of the Aec table double buffer
===========================================================================*/
void euledpch_set_edpcch_power_boost_enabled_flag(boolean val);
#endif /*FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST*/

#if defined(FEATURE_WCDMA_PLT) || defined(FEATURE_WCDMA_PLT_MODE)
#error code not present
#endif /*  #if defined(FEATURE_WCDMA_PLT) || defined(FEATURE_WCDMA_PLT_MODE) */

/*===========================================================================
FUNCTION  EUL_SET_VAR_THROUGH_QXDM

DESCRIPTION
  This function sets the eul uplink global variables as received
  through diag command. This function is a part of the callback function for
  diag command dispatch table.

DEPENDENCIES
  This function is called when "send_data 75 4 24 0 <val>" is issued from cmd
  line interface from QXDM

RETURN VALUE
  None

SIDE EFFECTS
  The eul pwr restriction can be disabled or enabled based on the val received
===========================================================================*/
extern void eul_set_var_through_qxdm( uint8 debug_switch_type, uint32 val);

/*==========================================================================
FUNCTION     E_UL_FILL_LOGPKTS_IN_NOT_TX_STATE

DESCRIPTION  This function fills the logpkts info depended on euledpch module
             when eul is not in tx state

DEPENDENCIES This function should be called only when eulstate is not in tx

RETURN VALUE None

SIDE EFFECTS logpkts will be filled in non tx state.
===========================================================================*/
void e_ul_fill_logpkts_in_not_tx_state(
                 mdspasync_edpch_param_db_struct_type **next_tti_fw_param_ptr,
                 uint8 enc_pwr_buf_idx,uint8 local_subfn);

/*==========================================================================
FUNCTION     E_UL_GET_MAX_RATE_CFG_PARAMS

DESCRIPTION  This function gets the dpdch cfged info and UE derived max SF
             from the signalled information

DEPENDENCIES This is called only when EUL is added/recfged/dropped

RETURN VALUE TRUE if dpdch is configured else FALSE and
             UE derived max channelization code

SIDE EFFECTS None.
===========================================================================*/
extern void e_ul_get_max_rate_cfg_params(boolean *dpdch_cfged,uint8 *ue_max_sf);

/*===========================================================================
FUNCTION      EULEDPCH_GET_AVG_DPCCH_PWR_IN_DB12

DESCRIPTION   This function is called to get the average DPCCH power in db12
              when eul is setup

DEPENDENCIES  This function should be called only when EUL is setup

RETURN VALUE  int16 Average DPCCH power in db12

SIDE EFFECTS  None
===========================================================================*/
extern int16 euledpch_get_avg_dpcch_pwr_in_db12(void);

/*===========================================================================
FUNCTION    UL_TX_RF_SEND_LOG_PKT_TO_DIAG()

DESCRIPTION This function sends 0x4313 log packet to diag

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
extern void ul_tx_rf_send_log_pkt_to_diag(void);

#ifdef FEATURE_WCDMA_DC_HSUPA 
extern void eul_new_log_send_log_pkt_to_diag (void);
void eul_mpr_log_pkt_submit_local_cmd (void);
void eul_new_mpr_log_pkt_populate(void);
#endif

#ifdef FEATURE_WCDMA_DC_HSUPA  
extern void eul_pow_alloc_send_log_pkt_to_diag (void);
void eul_pow_alloc_log_pkt_submit_local_cmd (void);
uint8 eul_pow_alloc_log_pkt_populate (eul_edch_harq_sg_info_struct_type* harq_sg_info_ptr,
                                              mdspasync_edpch_param_db_struct_type**  next_tti_fw_param_ptr,
                                              l1_eul_serving_grant_update_info_type* edpch_sg_info,
                                              uint32 edpch_a_ecx15,
                                              l1_e_max_ch_codes_enum_type *etfc_coding_sf
                                              );
#endif

/*===========================================================================
FUNCTION  EULEDPCH_UPH_MEAS_UPDATE

DESCRIPTION
  This function updates UPH measurements in UL maintainence event
  UPH is MTPL to DPCCH powe ratio

DEPENDENCIES
  Must be called in UL maintainence event before FIQ

RETURN VALUE
  None

SIDE EFFECTS
  updates UPH measurements
===========================================================================*/
extern void euledpch_uph_meas_update(void);

/*===========================================================================
 FUNCTION  EULEDPCH_CLEANUP_EDCH_CM_INFO

 DESCRIPTION
   This function resets the contents of edch_cm_info & edch_prev_tti_cm_info.
   This would be useful while during EUL cleaup a call to this function would
   ensure that no old information in these structures gets used when EUL is
   started again.

 DEPENDENCIES
   none

 RETURN VALUE
   None

 SIDE EFFECTS
   None
===========================================================================*/
extern void euledpch_cleanup_edch_cm_info(void);

/*===========================================================================
 FUNCTION  EULEDPCH_CPY_LAST_FRAME_CM_INFO

 DESCRIPTION
   This function stores a copy of the current frames CM info
   parameters.

 DEPENDENCIES
   none

 RETURN VALUE
   None

 SIDE EFFECTS
   None
===========================================================================*/
extern void euledpch_cpy_last_frame_cm_info(void);

/*===========================================================================
FUNCTION  EULEDPCH_IS_NEXT_SUBFRAME_IN_CM

DESCRIPTION
  This function tells whether the next sub frame is in CM are not

DEPENDENCIES

RETURN VALUE
  void

SIDE EFFECTS
===========================================================================*/
extern boolean euledpch_is_next_subframe_in_cm(uint16 sub_fr);

/*==========================================================================
FUNCTION:     E_UL_CPC_POPULATE_CM_BITMASK()

DESCRIPTION:  This function will populate CM gap and info FW when UE is in CPC DTX cycle.

DEPENDENCIES: None

RETURN VALUE: None

SIDE EFFECTS: None
==========================================================================*/
extern void e_ul_cpc_populate_cm_bitmask();


#ifdef FEATURE_WCDMA_CPC_DTX

/*===========================================================================
FUNCTION  EULEDPCH_IS_NEXT_FRAME_BURST_OVERLAPS_WTTH_GAP

DESCRIPTION  Calculates whether the preamble+DPCCH burst+postamable overlaps     
             with CM gap for current frame or next frame.
       
DEPENDENCIES NONE

RETURN VALUE boolean : TRUE if there is an overlap. FALSE otherwise

SIDE EFFECTS
===========================================================================*/
extern boolean euledpch_is_next_frame_burst_overlaps_with_gap
                                          (uint16 last_calc_burst1_intial_slots,
                                           boolean burst_info_extending_next_frame,
                                           boolean e_ul_cpc_dtx_cm_nrf_last_two_slots_preamble,
                                           wl1_ul_carr_id_enum_type carr_idx
                                           );
/*===========================================================================
FUNCTION  EULEDPCH_IS_NEXT_FRAME_CM_GAP_EXIST

DESCRIPTION  Calculates whether the next frame has CM gap and if so populate
             DTX CM INFO data structure.        
DEPENDENCIES NONE

RETURN VALUE boolean : TRUE if CM gap exists. FALSE otherwise

SIDE EFFECTS
===========================================================================*/

extern boolean euledpch_is_next_frame_cm_gap_exist(void);

/*===========================================================================
FUNCTION  EULEDPCH_CM_GAP_EXISTS_ALLEAST_CURR_OR_NEXT_TWO_FRAMES

DESCRIPTION  Calculates whether the current or next or next next frame has CM gap. 
 
DEPENDENCIES NONE

RETURN VALUE boolean : TRUE if CM gap exists. FALSE otherwise

SIDE EFFECTS
===========================================================================*/

extern boolean euledpch_cm_gap_exists_atleast_curr_or_next_two_frames(void);

/*===========================================================================
FUNCTION  EULEDPCH_IS_THIS_FRAME_CM_GAP_EXIST

DESCRIPTION  Calculates whether this frame has CM gap.        
DEPENDENCIES NONE

RETURN VALUE boolean : TRUE if CM gap exists. FALSE otherwise

SIDE EFFECTS
===========================================================================*/

extern boolean euledpch_is_this_frame_cm_gap_exist(void);

/*===========================================================================
FUNCTION  EULEDPCH_CPY_LAST_FRAME_DTX_CM_INFO

DESCRIPTION  Copy the cm info of the next frame.        
DEPENDENCIES NONE

RETURN VALUE 

SIDE EFFECTS
===========================================================================*/

extern void euledpch_cpy_last_frame_dtx_cm_info(void);

/*===========================================================================
FUNCTION  EULEDPCH_CLEANUP_EDCH_DTX_CM_INFO

DESCRIPTION  Clear the dtx cm info data structures.        
DEPENDENCIES NONE

RETURN VALUE 

SIDE EFFECTS
===========================================================================*/

extern void euledpch_cleanup_edch_dtx_cm_info(void);

/*===========================================================================
FUNCTION  EULEDPCH_IS_NEXT_EDCH_TX_IN_CM_FOR_CPC_DTX

DESCRIPTION
  This function tells whether the next possible EDCH tx after preamble is txed
  in CPC DTX, going to overlap CM gap.

DEPENDENCIES

RETURN VALUE
  boolean: TRUE if there is an overlap.

SIDE EFFECTS
===========================================================================*/
boolean euledpch_is_next_edch_tx_in_cm_for_cpc_dtx(uint16 sub_fr, uint8 cfn_val);

extern   void e_ul_cpc_dtx_debug_create_cm_gap(void);

#endif /*FEATURE_WCDMA_CPC_DTX*/


/*===========================================================================
FUNCTION      EULEDPCH_DTX_FR_DURING_TTI_RECFG

DESCRIPTION   This function is called by SG module to DTX the frame during
              TTI recfg

DEPENDENCIES  EDL modules should properly set the TTI recfg bit mask for SG module

RETURN VALUE
  void

SIDE EFFECTS  The upcoming frame is DTXed
===========================================================================*/
void euledpch_dtx_fr_during_tti_recfg(void);


/*=========================================================================

FUNCTION     : E_UL_GET_NUM_BITS_UE_CAT

DESCRIPTION  : This function returns max. number of bits as per the
               UE CAT

DEPENDENCIES : None

RETURN VALUE : uint32

SIDE EFFECTS : None


=========================================================================*/
extern uint32 e_ul_get_max_num_bits_for_ue_cat(uint16 ue_cat, e_tti_enum_type tti);

/*=========================================================================

FUNCTION     : E_UL_GET_CHAN_CODE_UE_CAT

DESCRIPTION  : This function returns max. channelisation code as per the
               UE CAT

DEPENDENCIES : None

RETURN VALUE : max_ch_code value 1..6

SIDE EFFECTS : None


=========================================================================*/
extern l1_e_max_ch_codes_enum_type e_ul_get_max_chan_code_for_ue_cat(uint16 ue_cat);




/*===========================================================================

FUNCTION    eul_calc_frnum_or_subfn

DESCRIPTION
  Calculates the subframe number based on TTI and frame number.

DEPENDENCIES
  TTI value should be verified to be 2ms or 10ms before function is called.

RETURN VALUE
  tti = 2 MS  -> Returns Sub-frame number.
  tti = 10 MS -> Returns Frame number.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 eul_calc_frnum_or_subfn ( e_tti_enum_type    tti /* tti 10ms or 2ms */ );

#ifdef FEATURE_WCDMA_REL6
/*===========================================================================
FUNCTION  EULEDPCH_WRITE_MPR_BINS_TO_FW

DESCRIPTION
  This function is called every EUL frame, by euledpch_process_data_for_next_tti()
  and enchs_upload_beta_table_to_mdsp in order to calculate the MPR power backoff
  values for the configuration

DEPENDENCIES
  enc_pwr_params should have been filled with appropriate beta values

RETURN VALUE
  void

SIDE EFFECTS
  enc_pwr_params will be filled with appropriate mtpl values with backoff removed
  FW variables are written.
===========================================================================*/
extern void euledpch_write_mpr_bins_to_fw(
    uint32 edpch_a_ecx15,
    uint16 edpch_a_edx15,
    l1_e_max_ch_codes_enum_type etfc_coding_sf
      ,uint8 enc_pwr_buf_idx
      );
#endif /* FEATURE_WCDMA_REL_6 */

extern void euledpch_dtx_next_tti_due_to_inactive_edpch
(mdspasync_edpch_param_db_struct_type**  next_tti_fw_param_ptr,
  uint8 enc_pwr_buf_idx,uint32  sub_fr);

#ifdef FEATURE_WCDMA_MODEM_TPUT_STATS_API 
/*===========================================================================
FUNCTION  EUL_ACCUMULATE_STATS

DESCRIPTION
  This function updates filtered allowed Data rate and plus_flag 
  at the end of eul data fiq handler. 
  

DEPENDENCIES
  None  

RETURN VALUE
  void

SIDE EFFECTS
  Updates the plus_flag and filtered allowed data rate if required.  
===========================================================================*/
extern void eul_accumulate_stats(void);

/*===========================================================================
FUNCTION  EUL_CALC_MAX_ALLOWED_SG_PO_BITS

DESCRIPTION
  This function computes the max grant allowed bits and max power allowed bits
  when Tx is not scheduled for next TTI. This values are used to estimate the 
  allowed data rate. This function is called at the end of data fiq handler when
  next TTI is DTX.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS  
   Computes allowed bits based on Grant and Power per TTI based on current
   UE condtion.
===========================================================================*/
extern void eul_calc_max_allowed_sg_po_bits(eul_edch_harq_sg_info_struct_type *harq_sg_info_ptr, 
                                            uint8 enc_pwr_buf_idx);

/*===========================================================================
FUNCTION   EUL_STATS_INIT_FOR_TTI 

DESCRIPTION
  This function initializes the internal EUL stats database to collect 
  data per TTI.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
===========================================================================*/
extern void eul_stats_init_for_tti(uint8 curr_cfn); 
#endif /* FEATURE_WCDMA_MODEM_TPUT_STATS_API */

/*===========================================================================
FUNCTION   WL1_EUL_STATS

DESCRIPTION
  This function provides data rate and plus flag to DS.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
extern boolean wl1_eul_stats(wl1_eul_stats_struct_type *ds_eul_stats);

#ifdef FEATURE_WCDMA_DC_HSUPA 
/*===========================================================================
FUNCTION  EULEDPCH_WRITE_DC_MPR_BINS_TO_FW

DESCRIPTION
  This function is called every EUL frame, by euledpch_process_data_for_next_tti()
  in order to get the MPR power backoff values for the configuration. Different from EULEDPCH_WRITE_MPR_BINS_TO_FW, 
  Only use when DC_HSUPA is defined. It will calculate mpr for both carriers.

DEPENDENCIES
  enc_pwr_params should have been filled with appropriate beta values

RETURN VALUE
  void

SIDE EFFECTS
  enc_pwr_params will be filled with appropriate mtpl values with backoff removed
===========================================================================*/

extern void euledpch_write_dc_mpr_bins_to_fw(
    uint16 euledpch_dc_mpr_beta_ec,
    uint16 euledpch_dc_mpr_beta_ed,
    l1_e_max_ch_codes_enum_type etfc_coding_sf,uint8 carr_idx, uint8 enc_pwr_buf_idx);

/*===========================================================================
FUNCTION  euledpch_sec_read_dpcch_mtpl_from_fw

DESCRIPTION
  This function is called from FIQ handler at the begging of  4th BPG to collect MTPL and Sec DPCCH
  in db10 from FW.

DEPENDENCIES
  Can only be called when secondary is in TX state

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void euledpch_sec_read_dpcch_mtpl_from_fw(void);

/*===========================================================================
FUNCTION  euledpch_sec_uph_meas_reset

DESCRIPTION
  This function is called to reset the Secondary UPH circular buffer

DEPENDENCIES
  Can only be called when secondary is getting activated

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void euledpch_sec_uph_meas_reset(void);

/*===========================================================================
FUNCTION  euledpch_sec_uph_meas_update

DESCRIPTION
  This function is called by maint_event_handler once everyframe to update linear power MTPL and DPCCH
  in Secondary carrier UPH circular buffer

DEPENDENCIES
  Can only be called from maint_event_handler and Sec EUL has to be in Tx state

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void euledpch_sec_uph_meas_update(void);

/*===========================================================================
FUNCTION  euledpch_sec_cal_uph

DESCRIPTION
  This function is called by euledpch_sec_uph_meas_update in maint_event_handler 
  once everyframe to calculate the average MTPL/average Sec UL DPCCH ratio over the last 10 frames

DEPENDENCIES
  Can only be called from maint_event_handler and Sec EUL has to be in Tx state

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void euledpch_sec_cal_uph(void);

#endif /*FEATURE_WCDMA_DC_HSUPA  */


/*===========================================================================
FUNCTION  EULEDPCH_SEC_TX_DISABLE

DESCRIPTION
  This function is called by ENC module to disable Sec E-DPCH for out-of-sync

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  E-DPCH DTX
===========================================================================*/
void euledpch_sec_tx_disable(void);

#ifdef FEATURE_WCDMA_DC_HSUPA 
/*===========================================================================
FUNCTION  EULEDPCH_TX_RE_ENABLE

DESCRIPTION
  This function is called by ENC module to re-enable Sec E-DPCH after
   out-of-sync

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  E-DPCH resumes
===========================================================================*/
void euledpch_sec_tx_re_enable_oos(void);

/*===========================================================================
FUNCTION  EULEDPCH_TX_RE_ENABLE

DESCRIPTION
  This function is called by ENC module to re-enable Sec E-DPCH after
  Primary recfg or Secondary recfg
DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  E-DPCH resumes
===========================================================================*/
void euledpch_sec_tx_re_enable_recfg(void);

/*===========================================================================
FUNCTION  EULEDPCH_TX_ENABLE

DESCRIPTION
  This function is called by ENC module to enable Sec E-DPCH after
  PCP on the Sec UL DPCCH is over.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  E-DPCH resumes
===========================================================================*/
extern void euledpch_sec_tx_enable(void);

/*===========================================================================
FUNCTION  EULEDPCH_SEC_TX_DISABLE

DESCRIPTION
  This function is called by ENC module to enable Sec E-DPCH after
  PCP on the Sec UL DPCCH is over.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  E-DPCH resumes
===========================================================================*/
extern void euledpch_sec_tx_deactivate(void);

/*===========================================================================
FUNCTION  E_DL_SEC_TX_ENABLE

DESCRIPTION
  This function is called by EUL ISR to enable Sec E-DL after Sec UL has gone
  active

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  Sec E-DL starts
===========================================================================*/

extern void e_dl_sec_tx_deactivate(void);

/*===========================================================================
FUNCTION  EULEDPCH_SEC_TX_DISABLE_RECFG

DESCRIPTION
  This function is called by ENC module to disable Sec E-DPCH during recfg

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  E-DPCH DTX
===========================================================================*/
extern void euledpch_sec_tx_disable_recfg(void);


/*===========================================================================
FUNCTION  EULEDPCH_SEC_TX_DISABLE_DURING_OOS

DESCRIPTION
  This function is called by ENC module to disable Sec E-DPCH for out-of-sync

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  E-DPCH DTX
===========================================================================*/
extern void euledpch_sec_tx_disable_during_oos(void);


#endif

/*===========================================================================
FUNCTION  eul_process_cm_info

DESCRIPTION
  This function is called in Post FIQ and process the CM info for the next tti
  
DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/

uint32 eul_process_cm_info(uint32 *r_combo_cm_tab_idx, uint32 *cm_tab_idx, boolean *next_tti_dtx, eul_edch_harq_sg_info_struct_type *harq_sg_info_ptr, 
                             mdspasync_edpch_param_db_struct_type** next_tti_fw_param_ptr, 
                             l1_eul_serving_grant_update_info_type* edpch_sg_info_ptr);

/*===========================================================================
FUNCTION  euledpch_call_maint_event

DESCRIPTION
    This function is called in Post FIQ and sets maint event handler parameters.
  
DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void euledpch_call_maint_event(eul_edch_harq_sg_info_struct_type *harq_sg_info_ptr);

/*===========================================================================
FUNCTION  euledpch_check_edpch_state_and_subfn

DESCRIPTION
  This function will check the subframe and and determin if next tti should be dtx or not. 
  it can also return prematurely if edpch state check failed
  
DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
boolean euledpch_check_edpch_state_and_subfn(eul_edch_harq_sg_info_struct_type *harq_sg_info_ptr, boolean *next_tti_dtx, 
                                                           mdspasync_edpch_param_db_struct_type** next_tti_fw_param_ptr, uint8 *enc_pwr_buf_idx);

/*===========================================================================
FUNCTION  euledpch_Aed_quant

DESCRIPTION
  This function Quantilize the Aed and Aed SF2  
  
DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None
===========================================================================*/
void euledpch_calc_a_ed_quant(  l1_e_max_ch_codes_enum_type *etfc_coding_sf, 
                                   mdspasync_edpch_param_db_struct_type** next_tti_fw_param_ptr,
                                   eul_edch_harq_sg_info_struct_type *harq_sg_info_ptr,
                                   uint32 *a_ed_j_harq, uint32 *etfci, wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION  euledpch_prepare_ul_tx_rf_log_pkt

DESCRIPTION
  This function prepare the legacy MPR log packet 
  
DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None
===========================================================================*/

void euledpch_prepare_ul_tx_rf_log_pkt(wl1_ul_carr_id_enum_type carr_idx);

/*===========================================================================
FUNCTION  EULEDPCH_UPDATE_SW_MAINT_DTX_COUNT

DESCRIPTION
  This function is called to update SW maintained DTX count 

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
extern void euledpch_update_sw_maint_dtx_count(wl1_ul_carr_id_enum_type carr_idx);

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)
/*===========================================================================
FUNCTION       WL1_UL_CONVERT_SLOT_TO_SUBFRAME

DESCRIPTION    This funciton will convert the slot number into the 
               subframe number.Not featurizing this intentionally 
               as the HSUPA feature is always enabled
  
DEPENDENCIES   None
 
RETURN VALUE   Will provide the corresponding subframe number for the slot number

SIDE EFFECTS   None
===========================================================================*/

extern uint16 wl1_ul_convert_slot_subframe(uint16 slot_num);
#endif /* FEATURE_QTA */
#endif /* EULEDPCH_H */

