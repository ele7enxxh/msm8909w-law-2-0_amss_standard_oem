#ifndef HSCFG_H 
#define HSCFG_H

/*==========================================================================
             WCDMA L1 HSDPA configuration header file

DESCRIPTION
  This file contains definition, declaration required for HSDPA configuration
  and maintainance code.

  Copyright (c) 2004 - 2013 by Qualcomm Technologies, Inc.
  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/hscfg.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who    what, where, why
-------   ---     ----------------------------------------------------------
05/19/16   svh    FET update changes.
02/24/15   ap     Initial check-in for WL1 QSH framework 
12/29/15   ms     Move HS-CM registration to task context
07/21/14   hdk    Moving logging func prototype to hslog.h
07/15/14   ac     NV reorganization
06/27/14   rc/jhl HS ASAP Start optimization and recfg removal    
05/11/14   vs     MCPM Opts for unification of HSDPA/HSUA Clk requests.
05/13/14   ymu    Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
03/28/14   bs     NV support to dynamically set max HARQ flush interval by FW.
03/18/13   vs     hs_update_activation_status_in_cur_db API is introduced.
03/10/14   rsr    Added Fix to obtain Sec. Carr. SCCH Status after CME is 
                  enabled.
03/11/14   hdk    Renaming hs_read_commit_pending_log_pkts function
02/05/14   sad    L1-RRC interface change and compaction cphy request
02/03/14   vs     0x4222 redesign to account for the new circular buffer logging.
01/14/14   jhl    Fixed compilation warnings
12/3/13    hdk    Defined macro HS_INFO_TABLE_START_SEC for both 3C and DC feature.
11/19/13   vs     new fiels in the ul cfg db.
11/18/13   rsr    Added QICE 3C Support.
11/12/13   hdk    0x4222 HS Deocode log pkt support for 3C.
10/18/13   hk     Corrected STMR Action Time Calculation
10/17/13   bs     Preliminary 3C Support
09/18/13   vs     HS redesign
08/11/13   kcm    Changes to hold cphy until FET is disabled during HS start/recfg
06/07/13   vs     HSDPA event reporting changes for L1M redesign
05/24/13   jhl    added support for rel9 SCCH order handling, remember SCCH order after
                  W2G, HHO failure, and cell reconfig without repointing
05/21/13   sad    Added DC HSUPA code changes under FEATURE_WCDMA_DC_HSUPA 
05/20/13   sad    Support for Enhanced Layer-1 Manager
02/20/13   hk     Mainlined FEATURE_WCDMA_DL_ENHANCED
02/11/13   ms     Added support for HSDPA Thin Layer and HS-FACH fast-disable
02/01/13   bs     Moving HS CM ctrl table update and logging from UL to DL Context
07/20/12  stk/mc  Added API to return TxD state of HS-PDSCH channel (for SVS feature)
07/31/12   pr     Ensure that cell in ordered set is not deregistered for DC config.
08/14/12   mc     Added code to log the context in which HS+CM tables are updated
07/24/12   geg    Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/21/12   ms     Added API hs_query_hs_mcpm_relinquish_pending
06/08/12   gv      Feature HS RACH Initial code check in
05/03/12   vsr    FET/DED/ED-V2 Feature checkin
04/04/12   zr     Added a function which returns the txd values for the single
                  and the dual carrier HS radio links.
03/16/12   ms     Added support for FEATURE_WCDMA_HS_FACH
02/08/12   ms     Added support for DC-HSDPA + SSC
01/26/12   vs     Nikel feature cleanup.
11/30/11   rc     Added support for FEATURE_WCDMA_DSR_SUPPORT
11/7/11    ms     Added prototype for hs_get_q_id_mapping_for_lc_id
10/21/11   ms     Increased the length of array to hold HS CM param info
10/20/11   ms     Updated HS config cmd confirmation timer to 3 frames
10/04/11   ms     Added support to maintain history of HS CM operations in SW
08/03/11   rgn    Post a local command to deconfigure Dual carrier
07/27/11   ks     Added prototype for hscfg_get_cpu_speed_from_nv
07/05/11   vsr    Mainlining Genesis CME features
05/31/11   geg    Changes for RRC and L2 api files clean-up wcdma dependencies
04/28/11   sj     upmerge from offline/main
04/25/11   mc     SW decoupling
03/31/11   amj    Added code for returning the RL TTO.
03/14/11   ms     Mainlined FEATURE_WCDMA_MDSP_INTERFACE_REV_6280
03/02/11   ms     Moved extern of hs_get_dc_state and hs_get_mimo_state from
                  hscfg.h to l1rrcif.h for RRC to be able to invoke these API
03/02/11   ms     Fixed the mechanism of querying PSC info for obtaining
                  cell_db_idx to query TD information
02/15/11   ms     Change in DL switch config to reschedule 1 frame
                  later if FW is not ready with HS start/reslam
10/25/10   ms     Corrected feature name FEATURE_WCDMA_COMBINED_MIMO_AND_64QAM
10/20/10   ms     Removed prototype for hs_generate_hspa_plus_cfg_evt
10/20/10   vsr    Add QSET Neighbour Cell Support
10/13/10   mc     Add support for FEATURE_WCMDA_COMBINED_MIMO_AND_64QAM
10/05/10   mc     Added support for different TXD interface for 
                  secondary carrier
09/22/10   vb     To allow DC carrier spacing to 5.2MHz
09/15/10   ms     Removed duplicate prototype for 
                  hscfg_clk_regime_deregister_client
09/09/10   mc     Added support for bumping FW Q6 speed with MIMO
08/26/10   mc     Added support for DC-HSDPA client registration with clkregime
08/20/10   mc     Compare freq and not channel number for DC carrier separation
08/03/10   ms     Corrected HS_INFO_TABLE_LEN_W32 macro value.
07/23/10   ms     Added code to catch condition of F/W not responding to HS
                  disable command.
07/18/10   ms     Updated 0x4225 version to 4 for Genesis
06/21/10   ms     Fixed compiler warnings
06/02/10   mc     Add validation for DC frequency
06/01/10   mc     Corrected call configuration in HS decode status logging
05/21/10   mc     Added support for DPCH+MIMO SCPICH combo phase reference
05/14/10   rvs    Add hs_get_mimo_scpich_state() declaration.
04/19/10   mc     Added support for MIMO codebook restriction
03/30/10   rvs    Remove FEATURE_WCDMA_CME_CONTROLLER.
03/25/10   mc     Adding NV control for MIMO 2beam mode.
03/16/10   mr     Changed the data type of s_cpich_po from uint8 to int8 
02/05/10   ms     Taxis/Genesis code merge
01/08/10   hrk/mc Consolidated configuration of misc cell params interface
01/08/10   mc     Added API for hs_get_unfrozen_dc_hs_cell_idx
12/21/09   hk     Added the declaration of hs_chan_deactivate_done
12/18/09   hk     Deactivate DC during DCH cleanup
11/18/09   ms     Added support to handle HS-SCCH orders.
11/13/09   ms     Added new API to query DC state.
11/06/09   mr     Handle MIMO S_CPICH power Imbalance from RRC
10/23/09   mr     Support for MIMO S-CPICH power Imbalance
07/15/09   rvs    Edited feature flags to include
                  FEATURE_WCDMA_QICE_CONTROLLER.
06/22/09   mr     Support for new HSPA+ events and Featurization Changes
05/12/09   rvs    Fixed compiler warnings.
04/24/09   mr     Fixed Compiler Warnings
04/20/09   rvs    Added defitions for the CME reg/dereg functions.
04/02/09   ms     Added macro HS_CFG_INTLOCK.
03/06/09   ms     Added prototypes for hs_get_64qam_state and
                  hs_get_mac_hs_or_ehs APIs.
02/20/09   vc     Introduced logging mutex to avoid concurrency.                 
02/09/09   mc     Drop HS if RL dropped as part of ASU. Check for HS RL to
                  be valid after ASU.
10/29/08   vc     Renamed cqi_mimo_delta to delta_cqi_mimo.
10/28/08   mc     Reduce severity of few debug messages
10/22/08   vc     Added support for mimo delta cqi.
10/11/08   mc     Removed the HS_NUM_DATA_STREAMS definition.
10/11/08   ms     Added prototype for hs_get_tbsize_table and
                  hs_is_64qam_configured.
10/03/08   hk     Bringing in 7k Mailine fixes
10/01/08   mc     Added support for MIMO UL.
09/30/08   ms     HSDPA Action type enum is based on firmware enum define.
09/30/08   vc     Added support to set MIMO parmaeters from QXDM commands.
09/24/08   mc     Changed HS_CONFIG_WINDOW_START_TICK to 3rd slot.
09/02/08   mc     Added support for CQI bler alignment
08/27/08   vsr    Added CME capability under feature FEATURE_WCDMA_CME_CONTROLLER
                  Also added cell_db_idx to cfg db
07/30/08   mc     Added support for MIMO configuration.
06/19/08   ms     Featurized RRC-L1 interface with FEATURE_HSDPA_MAC_EHS.
06/18/08   ms     Removed all occurences of FEATURE_WCDMA_HSPA_DEC and did
                  more code cleanup.
06/10/08   ms     Code cleanup
01/30/08   mc     Hold HS configuration request if EQ config is in progress. 
12/21/07   vb     Added protype hs_handle_hs_aset_update
11/15/07   ms     Added prototype for hs_get_serv_cell_psc
05/05/07   mc     Removed prototype for hscfg_set_dde_scaling_params
04/20/07   ms     Added prototype for enabling DDE scaling
04/16/07   ms     HARQ Preamble/Postamble changes
03/16/07   am     4312 logpkt changes
03/02/07   ms     Removed prototype for hs_cm_logging_init.
02/16/07   ms     Added support for Harq Preamble/Postamble.
01/03/07   ms     Changes to support the re-org of per RL 
                  triage for enhanced DL
11/22/06   ms     Added function prototype for retrieving HSDPA UE category
                  from NV.
10/10/06   ms     Added structure definition 
                  l1_hs_module_cleanup_after_hs_teardown_cmd_type.
09/28/06   mc     Added MCAL decoder support under feature 
                  FEATURE_MCAL_WCDMA
08/09/06   mc     Corrected HS normalize subframe macro syntax.
05/18/06   mc     Added code to cleanup HS state machine and logging after
                  L1 goes to IDLE.
05/18/06   ms     Added structure definition hs_fing_update_cb_struct_type 
                  that holds the HS finger update callback function pointers.
05/18/06   mc     Added support for HS+SSC. Removed HS enable bit field
                  and added HS SSC info in bits 9:12 in finger common config
04/20/06   mc     Added support for HS performance optimization.
04/11/06   bd     Mainlined the file from .../l1_msm6275/hscfg.h#20
03/29/06   ms     Added function prototype for querying UE category.
03/16/06   mc     Added support for Equalizer under the feature flag
                  FEATURE_WCDMA_EQ_CONTROLLER
02/27/06   mc     Changed HS reference finger bitmask in comb cfg db to 
                  enable more than 8 fingers.
02/22/06   mc     For supporting change in UE's category based on a NV 
                  variable, added virtual IR buffer size macros for diff
                  categories. Logic to choose the buf size moved to hscfg.c
01/27/06   gs     For MSM6280, if phase 1 or 2 feature is not defined it
                  defaults to virtual IR buffer size of cat 12
12/12/05   mc     Externalized the variable which keeps track of the CFN at
                  which HS CQI override is to be disabled.
10/17/05   gs     Added maximum virtual IR buffer size based on UE cat feature
09/14/05   gs     Defined macro HS_PDSCH_MAX_VIR_IR_BUF_SZ with value based
                  on UE category
                  Adjusted bit mask for DRM buffer address and virtual IR
                  buffer size in HSDPA info table for 6280
10/17/05   mc     Changed the UL beta table switching event tick to 10 bpgs.
                  This is done to avoid the race condition between beta table
                  switching and HS config window posting an event in case of
                  window miss.
08/16/05   mc     Replaced 'hs_cm_reconfig_cfn' by 'hs_cm_last_cfn_analyzed'.
08/08/05   mc     Added macro for HS beta table switch tick.
07/08/05   mc     Added enumeration to indicate the type of HS repointing
                  and prototype of hs_generate_serv_cell_change_evt.
06/09/05   mc     Added support for FEATURE_HSDPA_PLUS_CM logging. 
05/25/05   mc     Added FEATURE_HSDPA_PLUS_CM to disable mod/demod of
                  the HS subframes when CM gap is present.
04/27/05   mc     Changed the macro definition of HS_PO_DSCH_TO_Q7(x). Instead
                  of left shifting 6 times, x is multiplied by 64. Casting x
                  to uint8 for shift operation was destroying the sign bit.
04/07/05   mc     Added macros to get parameter values from an appropriate
                  location in the buffer.
03/17/05   mc     Removed lint warnings and errors.
03/04/05   gs     Added reference finger Tau DPCH bitmask, shift for HS
                  combiner channel DB
02/26/05   gs     Changed minimum offset between last DSCH and new SCCH and
                  HS-DPCCH at reconfig to 4 subframes from 2.
                  Added HS cell index related macros.
                  Updated function declaration of hs_ref_finger_change_handler
                  to add new parameter.
                  Externalized hs_get_cur_dl_cfg_ptr
                  Externalized HS cell index management related new functions.
12/27/04   gs     Corrected length of combiner config DB to 8 for CQI
                  Added macros for MAC-hs reset and CQI adjust in combiner DB.
                  Added definition of HS_CHANNEL_CFG_DONE_POST_CB_FUNC_TYPE
                  Added new members cctrch_id and dsch_to_cpich_offset in
                  structure hs_dl_channel_cfg_db_struct_type
                  Externalized variable hs_channel_active
                  Updated declaration of hs_channel_config_cmd
                  Declared function hs_deactivate_hsdpa_chan
10/11/04   gs     Created this module
===========================================================================*/

#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "l1const.h"
#include "l1def.h"
#include "l1rrcif.h"
#include "mcalwcdma_dl.h"
#include "wl1api.h"
#include "l1dlcctrchcfg.h"
#include "l1dlphychancfg.h"
#include "wl1fetctrl.h"
#if defined (FEATURE_QSH_DUMP) || defined(FEATURE_QSH_EVENT_METRIC)
#include "wl1_qsh_ext.h"
#endif /* defined (FEATURE_QSH_DUMP) || defined(FEATURE_QSH_EVENT_METRIC) */


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* HSDPA information buffers for ordered and current config */
#define HS_INFO_COUNT 2

/* Maximum value of HS-SCCH channelization code */
#define HS_SCCH_MAX_CH_CODE_VAL 127

/* Various min and max values related to HS-PDSCH */
/* ---------------------------------------------- */

/* 25.306 Section 5 UE radio access capability */
#define HS_PDSCH_VIR_IR_BUF_SZ_CAT6  67200  /* For Cat 6  */
#define HS_PDSCH_VIR_IR_BUF_SZ_CAT8  134400 /* For Cat 8  */
#define HS_PDSCH_VIR_IR_BUF_SZ_CAT9  172800 /* For Cat 9  */
#define HS_PDSCH_VIR_IR_BUF_SZ_CAT10 172800 /* For Cat 10 */
#define HS_PDSCH_VIR_IR_BUF_SZ_CAT12 28800  /* For Cat 12 */
#define HS_PDSCH_VIR_IR_BUF_SZ_CAT14 259200 /* For Cat 14 */
#define HS_PDSCH_VIR_IR_BUF_SZ_CAT16 345600 /* For Cat 16 */
#define HS_PDSCH_VIR_IR_BUF_SZ_CAT18 259200 /* For Cat 18 - 64QAM */
#define HS_PDSCH_VIR_IR_BUF_SZ_CAT20 518400 /* For Cat 20 - MIMO + 64QAM */
#ifdef FEATURE_WCDMA_DC_HSDPA
#define HS_PDSCH_VIR_IR_BUF_SZ_CAT24 518400 /* For Cat 24 - DC-HSDPA */
#endif
#define HS_PDSCH_VIR_IR_BUF_SZ_CAT29 777600 /* For Cat 29 - 3C-HSDPA */
#define HS_PDSCH_MAX_VIR_IR_BUF_SZ hs_pdsch_max_vir_ir_buf_sz

/* Various min and max values related to HS-DPCCH */
/* ---------------------------------------------- */

/* HS PDSCH and CPICH power offset */
#define HS_DPCCH_PO_MIN -12 /* -6 dB */
#define HS_DPCCH_PO_MAX  26 /* 13 dB */

/* CQI feedback cycle */
#define HS_DPCCH_CQI_FEEDBACK_CYCLE_MS_MIN 0
#define HS_DPCCH_CQI_FEEDBACK_CYCLE_MS_MAX 160

/* CQI repetition factor */
#define HS_DPCCH_REP_FACTOR_MIN 1
#define HS_DPCCH_REP_FACTOR_MAX 4

/* delta ACK/NACk/CQI min and max values */
#define HS_DPCCH_DELTA_VAL_MIN 0
#define HS_DPCCH_DELTA_VAL_MAX 8

#ifdef FEATURE_WCDMA_DC_HSDPA
#define HS_DC_FREQ_KHZ_MIN_DIFF   3800
#define HS_DC_FREQ_KHZ_MAX_DIFF   5200
#endif

#define HS_CFG_LOG_INVALID_HSDPA_TABLE_INFO_TYPE 0xFF
#define HS_CFG_LOG_INVALID_HSDPA_CFG_INFO_PARAM 0xFF

/* various macros related to mDSP interface */
/* ---------------------------------------- */
#if defined(FEATURE_WCDMA_3C_HSDPA)
/* There are 6 HSDPA information table. 2 for primary carrier
   and 2 for secondary carrier and 2 for 3rd carrier */
#define HS_INFO_TABLE_COUNT 6
#elif defined(FEATURE_WCDMA_DC_HSDPA)
/* There are 4 HSDPA information table. 2 for primary carrier
   and 2 for secondary carrier */
#define HS_INFO_TABLE_COUNT 4
#else
/* mDSP has 2 HSDPA information table */
#define HS_INFO_TABLE_COUNT 2
#endif

#if ( defined(FEATURE_WCDMA_3C_HSDPA) || defined(FEATURE_WCDMA_DC_HSDPA) )
/* Table 2 and 3 are used for secondary cell */
#define HS_INFO_TABLE_START_SEC 2
#endif
/* invalid HS table index value */
#define HS_INFO_TABLE_INVALID_VAL 0xFF

/* HSDPA information table fields */
/* ------------------------------ */

#define HS_INFO_TABLE_UPDATE_REQD(hs_info) \
  (CPHY_HS_CFG_REQ_IS_CFG_INCL((hs_info)->hs_req_mask, DSCH) || \
   CPHY_HS_CFG_REQ_IS_CFG_INCL((hs_info)->hs_req_mask, SCCH) || \
   CPHY_HS_CFG_REQ_IS_CFG_INCL((hs_info)->hs_req_mask, DPCCH))

#ifdef FEATURE_WCDMA_HS_FACH
/* Extract the HRNTI type from bitmask */
#define HS_DSCH_BCCH_HRNTI_INCL(h_rnti_bmsk) \
          (h_rnti_bmsk & L1_BCCH_H_RNTI_VALID)

#define HS_DSCH_COMMON_HRNTI_INCL(h_rnti_bmsk) \
          (h_rnti_bmsk & L1_COMMON_H_RNTI_VALID)

#define HS_DSCH_DEDICATED_HRNTI_INCL(h_rnti_bmsk) \
          (h_rnti_bmsk & L1_DEDICATED_H_RNTI_VALID)
#endif /* FEATURE_WCDMA_HS_FACH */

/* Number of WORD32 entries in each HSDPA information table */
#define HS_INFO_TABLE_LEN_W32 (sizeof(WfwDembackHsInfoTableStruct)/4)

/* HSDPA Compressed Mode Control Table fields */
/* ------------------------------------------ */

/* Each HSDPA CM control table has 16 entries */
#define HS_CM_CTRL_CFG_PARAM_DB_LEN_W16 16

#define HS_CM_DB_SET_PARAM(buffer, offset, param, val) \
 HS_SET_BUF_VAL(buffer, \
                offset, \
                HS_CM_CFG_DB_##param##_SHFT, \
                HS_CM_CFG_DB_##param##_BMSK, \
                val)

/* Control for HS-SCCH/PDSCH demodulation */
#define HS_CM_CFG_DB_DL_SHFT 0
#define HS_CM_CFG_DB_DL_BMSK 0x0001

/* Control for ACK/NACK transmission */
#define HS_CM_CFG_DB_UL_ACK_NACK_SHFT 1
#define HS_CM_CFG_DB_UL_ACK_NACK_BMSK 0x0002

/* Control for CQI transmission */
#define HS_CM_CFG_DB_UL_CQI_SHFT 2
#define HS_CM_CFG_DB_UL_CQI_BMSK 0x0004

/* Controls CQI value */
#define HS_CM_CFG_DB_CQI_DTX_SHFT 3
#define HS_CM_CFG_DB_CQI_DTX_BMSK 0x0008

/* The mask for the three UL CM control bits 
   shifted to the least significant three bits */
#define HS_CM_CFG_DB_UL_CQI_SHFTED_BMASK        \
           ((HS_CM_CFG_DB_UL_ACK_NACK_BMSK |    \
             HS_CM_CFG_DB_UL_CQI_BMSK      |    \
             HS_CM_CFG_DB_CQI_DTX_BMSK) >> HS_CM_CFG_DB_UL_ACK_NACK_SHFT)

/* HSDPA combiner config param database fields */
/* ------------------------------------------- */

/* Each HSDPA combiner DB table has 8 WORD32 entries */
#define HS_COMB_CFG_PARAM_DB_LEN_W16 8

#define HS_COMB_CFG_DB_SET_PARAM(buffer, param, val) \
  HS_SET_BUF_VAL(buffer, \
                 HS_COMB_CFG_DB_##param##_REG, \
                 HS_COMB_CFG_DB_##param##_SHFT, \
                 HS_COMB_CFG_DB_##param##_BMSK, \
                 val)

/* HS comb cfg action */
#define HS_COMB_CFG_DB_ACTION_BMSK 0x0003
#define HS_COMB_CFG_DB_ACTION_REG  0
#define HS_COMB_CFG_DB_ACTION_SHFT 0

/* HS comb cfg reference finger */
#define HS_COMB_CFG_DB_REF_FING_BMSK 0x000F
#define HS_COMB_CFG_DB_REF_FING_REG  1
#define HS_COMB_CFG_DB_REF_FING_SHFT 0

/* HS comb cfg reference finger tau-DPCH */
#define HS_COMB_CFG_DB_REF_FING_TAU_DPCH_BMSK 0xFF00
#define HS_COMB_CFG_DB_REF_FING_TAU_DPCH_REG  1
#define HS_COMB_CFG_DB_REF_FING_TAU_DPCH_SHFT 8

/* HS comb cfg DPCH hardware channel number */
#define HS_COMB_CFG_DB_DPCH_CH_BMSK 0x0003
#define HS_COMB_CFG_DB_DPCH_CH_REG  2
#define HS_COMB_CFG_DB_DPCH_CH_SHFT 0

/* HS comb cfg DPCH hardware channel number */
#define HS_COMB_CFG_DB_SSC_BMSK 0xF000
#define HS_COMB_CFG_DB_SSC_REG  2
#define HS_COMB_CFG_DB_SSC_SHFT 12

/* HS comb cfg DPCH to SCCH offset */
#define HS_COMB_CFG_DB_DPCH_SCCH_OFFSET_BMSK 0x00FF
#define HS_COMB_CFG_DB_DPCH_SCCH_OFFSET_REG  3
#define HS_COMB_CFG_DB_DPCH_SCCH_OFFSET_SHFT 0

/* HS comb cfg start global sub frame number */
#define HS_COMB_CFG_DB_START_GSUB_FN_BMSK 0x07FF
#define HS_COMB_CFG_DB_START_GSUB_FN_REG  4
#define HS_COMB_CFG_DB_START_GSUB_FN_SHFT 0

/* HS comb cfg stop global sub frame number */
#define HS_COMB_CFG_DB_STOP_GSUB_FN_BMSK 0x07FF
#define HS_COMB_CFG_DB_STOP_GSUB_FN_REG  5
#define HS_COMB_CFG_DB_STOP_GSUB_FN_SHFT 0

/* HS comb cfg MAC-hs reset indicator */
#define HS_COMB_CFG_DB_MAC_HS_RESET_IND_BMSK 0x0001
#define HS_COMB_CFG_DB_MAC_HS_RESET_IND_REG  6
#define HS_COMB_CFG_DB_MAC_HS_RESET_IND_SHFT 0

/* HS comb cfg CQI adjustment value */
#define HS_COMB_CFG_DB_CQI_ADJUST_BMSK 0xFFFF
#define HS_COMB_CFG_DB_CQI_ADJUST_REG  7
#define HS_COMB_CFG_DB_CQI_ADJUST_SHFT 0

 /* note that po_dsch is already scaled by 2 when received OTA */
#define HS_PO_DSCH_TO_Q10(x) ((x) * 512)

/* For now use the values inited in global variable */
#define HS_SPREADING_FAIN_FOR_CQI_Q7    hs_spreading_fain_for_cqi_q7
#define HS_SNR_TO_CQI_OFFSET_FOR_CQI_Q7 hs_snr_to_cqi_offset_for_cqi_q7

/* UL HS DPCCH config param database fields */
/* ---------------------------------------- */

/* Each UL HS-DPCCH DB table has 7 WORD32 entries */
#define HS_UL_CFG_PARAM_DB_LEN_W16 7

#define HS_DPCCH_CFG_DB_SET_PARAM(buffer, param, val) \
  HS_SET_BUF_VAL(buffer, \
                 HS_DPCCH_CFG_DB_##param##_REG, \
                 HS_DPCCH_CFG_DB_##param##_SHFT, \
                 HS_DPCCH_CFG_DB_##param##_BMSK, \
                 val)

/* HS-DPCCH cfg action */
#define HS_DPCCH_CFG_DB_ACTION_BMSK 0x0003
#define HS_DPCCH_CFG_DB_ACTION_REG  0
#define HS_DPCCH_CFG_DB_ACTION_SHFT 0

/* CQI feed back cycle. It is in units of 2 seconds or HS Sub-frame.
   Range of parameter is 0 to 80 (Actual is 0 to 160 ms) */
#define HS_DPCCH_CFG_DB_CQI_FB_CYCLE_LEN_BMSK 0x00FF
#define HS_DPCCH_CFG_DB_CQI_FB_CYCLE_LEN_REG  1
#define HS_DPCCH_CFG_DB_CQI_FB_CYCLE_LEN_SHFT 0

/* CQI repetition factor */
#define HS_DPCCH_CFG_DB_CQI_REP_BMSK 0x0007
#define HS_DPCCH_CFG_DB_CQI_REP_REG  2
#define HS_DPCCH_CFG_DB_CQI_REP_SHFT 0

/* ACK/NACK repetition factor */
#define HS_DPCCH_CFG_DB_ACK_NACK_REP_BMSK 0x0007
#define HS_DPCCH_CFG_DB_ACK_NACK_REP_REG  3
#define HS_DPCCH_CFG_DB_ACK_NACK_REP_SHFT 0

#ifdef FEATURE_HSDPA_HARQ_PREAMBLE
#define HS_DPCCH_CFG_DB_HARQ_PMBL_MODE_BMSK 0x8000
#define HS_DPCCH_CFG_DB_HARQ_PMBL_MODE_REG 0
#define HS_DPCCH_CFG_DB_HARQ_PMBL_MODE_SHFT 15
#endif

/* DPCH to HS-DPCCH offset */
#define HS_DPCCH_CFG_DB_HS_DPCCH_OFFSET_BMSK 0x00FF
#define HS_DPCCH_CFG_DB_HS_DPCCH_OFFSET_REG  4
#define HS_DPCCH_CFG_DB_HS_DPCCH_OFFSET_SHFT 0

/* HS comb cfg start global sub frame number */
#define HS_DPCCH_CFG_DB_START_GSUB_FN_BMSK 0x07FF
#define HS_DPCCH_CFG_DB_START_GSUB_FN_REG  5
#define HS_DPCCH_CFG_DB_START_GSUB_FN_SHFT 0

/* HS comb cfg stop global sub frame number */
#define HS_DPCCH_CFG_DB_STOP_GSUB_FN_BMSK 0x07FF
#define HS_DPCCH_CFG_DB_STOP_GSUB_FN_REG  6
#define HS_DPCCH_CFG_DB_STOP_GSUB_FN_SHFT 0

/* SCCH part 1 scrambler related macros */
/* ------------------------------------ */

/* Convolutional 1/2 coding polynomials.
   Values in octal units */
#define HS_CONV_1_2_POLY_0 0561
#define HS_CONV_1_2_POLY_1 0753

/* Convolutional ending constraint length */
#define HS_CONV_COSTRAINT_LEN 9
/* Number of tail bits */
#define HS_CONV_TAIL_BIT_LEN  8

/* Number of bits in H-RNTI */
#define HS_HRNTI_NUM_BITS 16

/* Total encoded length of encoded stream */
#define HS_CONV_ENCODED_SCRB_LEN \
  ((HS_HRNTI_NUM_BITS + HS_CONV_TAIL_BIT_LEN) * 2)

/* Final scrambler is split in 2 part s high 8 bits and low 32 bits */
#define HS_SCCH_P1_SCRB_HI_LEN 8
#define HS_SCCH_P1_SCRB_LO_LEN 32
/* Total length of scrambler after puncturing encoded stream */
#define HS_SCCH_P1_SCRB_LEN (HS_SCCH_P1_SCRB_HI_LEN + HS_SCCH_P1_SCRB_LO_LEN)

/* HS DEMOD/MOD control table related macros */
/* ----------------------------------------- */

#define HS_NUM_CTRL_TABLE_ENTRIES 16

/* HS channel timing offset related macros */
/* --------------------------------------- */

/* Global sub frame number maximum value */
#define HS_MAX_GLOBAL_SUB_FRAME_COUNT 1280
#define HS_NORMALIZE_GLOBAL_SUB_FN(x) \
  normalize((int32) (x), HS_MAX_GLOBAL_SUB_FRAME_COUNT);

/* HS-SCCH to HS-DSCH offset is 2 slots */
#define HS_SCCH_TO_DSCH_OFFSET_UNIT_256_CHIP (2 * NUM_256_CHIP_UNITS_PER_SLOT)
/* HS sub frame length is 3 slots */
#define HS_SUB_FRAME_LEN_UNIT_256_CHIP       (3 * NUM_256_CHIP_UNITS_PER_SLOT)
/* Number of HS sub frame per radio length 5 */
#define HS_NUM_SUB_FR_PER_RADIO_FR \
  (NUM_256_CHIP_UNITS_PER_FRAME/HS_SUB_FRAME_LEN_UNIT_256_CHIP)
#define HS_NORMALIZE_SUB_FR_LEN(x) \
  normalize((int32) (x), HS_SUB_FRAME_LEN_UNIT_256_CHIP)

/* Minimum offset between stop and start at recinfiguration is 20 bpg
   (256 chip units) */
#define HS_MIN_OFFSET_DSCH_SCCH_AT_RECFG_UNIT_256_CHIP \
  (4 * NUM_256_CHIP_UNITS_PER_SLOT)
#define HS_MIN_OFFSET_DPCCH_AT_RECFG_UNIT_256_CHIP \
  (4 * NUM_256_CHIP_UNITS_PER_SLOT)

/* Frame boundary tick is offset by 4 ticks (256 chip units) */
#define HS_CHAN_CFG_FRM_BDRY_TICK 4

/* The UL beta table is switched later than HS_CHAN_CFG_FRM_BDRY_TICK.
   By this time mDSP has finished latching all value at R99 DPCH boundary*/
#define HS_SWITCH_BETA_TABLE_TICK 10

/* HS combiner channel is reffered to as PhCh 4 for reference
   finger and time tracking purposes */
#define HS_DL_COMB_PHCH_NUMBER 4

/* HS cell index 0 is reserved for all non-ASET RLs so very first
   usable HS cell index start from 1 */
#define HS_CFG_FIRST_HS_CELL_IDX 1
/* Total HS cell index used by mDSP */
#define HS_CFG_MAX_HS_CELL_IDX   8
/* Invalid HS cell index and PSC */
#define HS_CFG_INVALID_PSC_HS_CELL_IDX 0xFFFF
#define HS_CFG_INVALID_HS_CELL_IDX 0xFF

#define HS_MIN_SEC_SCR_CODE_VAL 1
#define HS_MAX_SEC_SCR_CODE_VAL 15

/* Number of tries after which SW will stop waiting
   for HS config cmd confirmation from FW. SW polls  
   the phychan pending mask every 3 slots. So give FW  
   3 radio frames to finish working on the HS config.
 */
#define HS_MAX_CFG_CMD_PEND_CNT 15

/* Indicating that no CM transmission gap exist in the subframe */
#define HS_NO_CM_GAP 0xFF
/* 30 bpg for the length of the subframe + 20 bpg for SCCH to PDSCH offset */
#define HS_CM_SCCH_TO_PDSCH_SUBFRAME_SPAN \
  (HS_SUB_FRAME_LEN_UNIT_256_CHIP + HS_SCCH_TO_DSCH_OFFSET_UNIT_256_CHIP)

/* Number of entries in HS CM param info buffer */
#define HS_CM_PARAM_BUF_LEN 32

/* for HS ASAP start calculation delay current dl tick by 40 BPG*/
#define HS_ASAP_START_DL_TICK_DELAY_OFFSET 40
/* for HS ASAP start - last dl tick to start in current frame*/
#define HS_ASAP_START_LAST_DL_TICK_IN_CURRENT_FRAME 80
/* for HS ASAP start - first dl tick to start in next frame*/
#define HS_ASAP_START_FIRST_DL_TICK_IN_NEXT_FRAME 81
/* for HS ASAP start ul calcaulation - if current dl tick is later than 120
   then delay start by 1 subframe to allow enough time for FW*/
#define HS_ASAP_START_UL_CMD_LATEST_TICK 120
/* DL tick range [0,149], max value is 149 */
#define HS_ASAP_START_MAX_DL_TICK_VALUE 149
/* Max CFN difference between activation time and HS start CFN is 2 */
#define HS_ASAP_START_MAX_DIFF_TO_ACTIVATION_TIME 2
/* In one radio frame there are five sub-frames */
#define HS_NUM_SUB_FRAME_IN_ONE_FRAME 5
/* hs operation max gSFN number is 1280 */
#define HS_ASAP_START_MAX_GSFN_VALUE 1280

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Call back function type declaration */
/* ----------------------------------- */

/* HS event call back function type */
typedef void HS_CHAN_EVENT_FUNC_TYPE(void);

/* HS configuration done post call back function type */
typedef void HS_CHANNEL_CFG_DONE_POST_CB_FUNC_TYPE(boolean status);

/* HS configuration done post call back function type */
typedef void WL1_DSR_HS_TRANS_IND_CB_FUNC_TYPE(boolean hs_active);

/* Callback prototype of function that is called by CME to resume HS config */
typedef void HS_RESUME_CONFIG_CME_DONE_CB_FUNC_TYPE(void);

/* Structure that is sent to CME with HS state change callback */
typedef struct
{
  /* State of HS. FALSE if HS CME cell is to be dropped */
  boolean hs_status;

  /* number of configured sec carriers*/
 uint8 num_sec_carriers;

  /* After CME is done processing, it will call this callback */
  HS_RESUME_CONFIG_CME_DONE_CB_FUNC_TYPE *hs_resume_cb;
} hs_cme_state_change_cb_struct_type;

/* Callback registered by CME with HSDPA */
typedef void HS_CME_STATE_CHANGE_CB_FUNC_TYPE(
  hs_cme_state_change_cb_struct_type *hs_cme_ptr);

/* Enum type declaration */
/* --------------------- */
/* HSDPA channel configuration action types used by mDSP */
typedef WfwDemodPhChActionTypeEnum hs_channel_cfg_mdsp_action_enum_type;

/* Specifies Mac-hs or Mac-ehs */
typedef mac_hs_e_type hs_mac_ver_enum_type;

/* Specifies if HS channel config is initiated by RRC or internally
   by other modules */
typedef enum
{
  HSDPA_SETUP_CMD_EXTERNAL,
  HSDPA_SETUP_CMD_INTERNAL
} hsdpa_setup_cmd_enum_type;

#ifdef FEATURE_WCDMA_HS_FACH
/* HSDPA ops transition scenarios */
typedef enum
{
  HS_OPS_INVALID_TRANSITION,

  /* HS Start transitions */
  HS_OPS_START_TO_E_FACH_TRANSITION,
  HS_OPS_START_TO_E_PCH_TRANSITION,
  HS_OPS_START_TO_DCH_TRANSITION,

  /* Reconfiguration within HS */
  HS_OPS_E_FACH_TO_DCH_TRANSITION,
  HS_OPS_E_FACH_TO_E_PCH_TRANSITION,
  HS_OPS_E_FACH_TO_E_FACH_TRANSITION,
  HS_OPS_E_PCH_TO_E_FACH_TRANSITION,
  HS_OPS_E_PCH_TO_E_PCH_TRANSITION,
  HS_OPS_DCH_TO_DCH_TRANSITION,

  /* HS Stop transitions */
  HS_OPS_E_FACH_TO_STOP_TRANSITION,
  HS_OPS_E_PCH_TO_STOP_TRANSITION,
  HS_OPS_DCH_TO_STOP_TRANSITION

} hs_ops_hs_state_transition_enum_type;
#endif /* FEATURE_WCDMA_HS_FACH */

/* Structure type declaration */
/* -------------------------- */

typedef mcalwcdma_dec_hs_mac_d_pdu_sz_info_struct_type  hs_dl_mac_d_pdu_sz_info_struct_type;

/* Information of MAC-d PDU size Q */
typedef mcalwcdma_dec_hs_mac_d_pdu_sz_q_info_struct_type  hs_dl_mac_d_pdu_sz_q_info_struct_type;

/* Parameters that are specific to each HSDPA carrier */
typedef struct
{
 #ifdef FEATURE_HSDPA_MAC_EHS
  /* Indicates if the HS-DSCH TB table to be chosen is octet aligned or not */
  l1_dl_hs_dsch_tb_size_alignment_enum_type hsdsch_tb_size_alignment_type;
 #endif

  /* primary scrambling code of HS serving RL */
  uint16 psc;

  /* frequency */
  uint16 freq;

  /* cell db index for the given psc*/
  uint8 cell_db_idx;

  /* Transmit diversity mode for HS-PDSCH */
  mcalwcdma_demod_tx_div_mode_enum_type tx_div_mode;

 #ifdef FEATURE_WCDMA_64QAM
  boolean hs_64_qam_configured;
 #endif

  /* configuration related params */
  /* ---------------------------- */

  /* HSDPA information table index */
  uint8 hs_info_table_idx;

  /* HSDPA channel configuration action used by mDSP */
  hs_channel_cfg_mdsp_action_enum_type action;

  /* Reference finger for HSDPA DL channel */
  uint8 ref_finger;

 #ifdef  FEATURE_WCDMA_HS_FACH
  /* Store the common/dedicated H-RNTI */
  uint32 h_rnti ;

  /* Store the BCCH H-RNTI */
  uint32 bcch_h_rnti;
 #endif 

  /* DSCH to CPICH offset. MPO */
  uint16 dsch_to_cpich_offset;

  /* Carrier ACTIVATION status, TRUE: ACTIVATE, FALSE: DEACTIVATE.
     NA for Primary Carrier */
  boolean activation_status;

  /* L1 DL HS SCCH physical channel info */
  l1_dl_hs_scch_info_struct_type  scch_info; 
} hs_dl_channel_per_carrier_struct_type;

/* HS DL combiner information structure */
typedef struct
{
  /*            Common fields          */
  /* ***********************************/
  /* indicate if this configuration is valid or not */
  boolean valid;

 #ifdef FEATURE_HSDPA_MAC_EHS
  /* indicate if this configuration is for MAC-hs or for MAC-ehs */
  hs_mac_ver_enum_type hs_or_ehs;
 #endif

  /* HS channel info variable */
  /* ------------------------ */

  /* CCTrCh Id for this configuration */
  uint8 cctrch_id;

  /* indicate info for MAC-d PDU size Qs
     Q Id goes with index of this array */
  hs_dl_mac_d_pdu_sz_q_info_struct_type
    mac_d_pdu_sz_q_info[L1_DL_MAC_HS_Q_MAX_COUNT];

 #ifdef FEATURE_HSDPA_MAC_EHS
  uint8 mac_ehs_dlchan_q_info[L1_DL_MAX_LOGICAL_CHANNEL];
 #endif

  /* HS serving radio link DPCH tau-DPCH */
  uint8 hs_serving_rl_tau_dpch;

  /* DPCH to HS-PDSCH offset */
  uint8 dpch_to_pdsch_offset;

  /* HS configuration related variable */
  /* --------------------------------- */

  /* configuration CFN */
  uint8 config_cfn;

  /* Start CFN for Enable or Reconfig */
  uint8 start_cfn;

  /* DPCH hardware channel number */
  uint8 dpch_hwch_num;

  /* Request mask indicating type of HS channel */
  uint32 hs_req_mask;

 #ifdef  FEATURE_WCDMA_HS_FACH
  /* store the HRNTI bit mask info */
  uint8 efach_h_rnti_bmsk;

  /* offset of enable HS-SCCH relative to 
     CPICH frame boundary (in bpg) */
  uint8 hs_offset;

  /* only HRNTI is updated in HS DSCH */
  boolean hrnti_update;
 #endif

  /* DPCH to starting HS-SCCH offset */
  uint8 dpch_to_scch_offset;

  /* DPCH to starting HS-SCCH offset for HS+CM logging*/
  uint8 dpch_to_scch_offset_for_hs_cm;

  /* Starting global sub frame number for first HS-SCCH sub frame */
  uint16 start_global_sub_fr_num;

  /* Last HS-PDSCH global sub frame number, Used only for Disable/reslam */
  uint16 stop_global_sub_fr_num;

  /* The offset between CFNx5 and global subframe number */
  uint8 cfnx5_sub_fr_offset;

 #ifdef FEATURE_WCDMA_DC_HSDPA
  /* Flag to indicate state of each carrier */
  boolean mc_configured[L1_HSDPA_MAX_CARRIERS];
 #endif /* FEATURE_WCDMA_DC_HSDPA */
  
  /* Number of secondary carriers configured */
  uint8 num_sec_carr_configured;
 /*           Carrier-specific fields            */
 /* ******************************************** */

  hs_dl_channel_per_carrier_struct_type carr_info[L1_HSDPA_MAX_CARRIERS];
} hs_dl_channel_cfg_db_struct_type;

/* UL HS-DPCCH information structure */
typedef struct
{
  /* indicate if this configuration is valid or not */
  boolean valid;

  /* HS DPCCH info variable */
  /* ------------------------ */

  /* HS DPCCH configuration related variable */
  /* --------------------------------------- */
  /* indicate if HS-DPCCH is active */
  boolean active;

  /* configuration CFN */
  uint8 config_cfn;

  /* HSDPA channel configuration action used by mDSP */
  hs_channel_cfg_mdsp_action_enum_type action;

  /* HS-DPCCH CQI feedback cycle length in units of Sub frame */
  uint8 cqi_feed_back_cycle_len;

  /* CQI repetition factor */
  uint8 cqi_rep_factor;

  /* ACK/NACK repetition factor */
  uint8 ack_nack_rep_factor;

  /* various gain deltas */

  /* delta CQI (0..8) */
  uint8 delta_cqi;
  /* delta ACK (0..8) */
  uint8 delta_ack;
  /* delta NACK (0..8) */
  uint8 delta_nack;
 #if defined(FEATURE_WCDMA_DC_HSDPA)
  /* delta ACK_ACK (0..8) */
  uint8 delta_ack_ack;
  /* delta ACK_NACK (0..8) */
  uint8 delta_ack_nack;
  /* delta NACK_ACK (0..8) */
  uint8 delta_nack_ack;
  /* delta NACK_NACK (0..8) */
  uint8 delta_nack_nack;
  /* delta cqi mimo */
  uint8 delta_cqi_mimo;
 #endif

  /* DPCH to starting HS-DPCCH offset */
  uint8 dpch_to_dpcch_offset;

  /* Starting global sub frame number for first HS-SCCH sub frame */
  uint16 start_global_sub_fr_num;

  /* Last HS-PDSCH global sub frame number, Used only for Disable/reslam */
  uint16 stop_global_sub_fr_num;

  /* The offset between CFNx5 and global subframe number */
  uint8 cfnx5_sub_fr_offset;

 #ifdef FEATURE_HSDPA_HARQ_PREAMBLE
  /* HARQ Preamble Mode (on/off) */
  boolean harq_pmbl_mode;
 #endif

  /* ul cfn at which response was received from FW for the HS-DPCCH cfg cmd*/
 uint8 active_cfn;

 /* ul tick at which SW received the respone from FW */
 uint8 active_tick;
} hs_ul_channel_cfg_db_struct_type;

/*hs call inforamtion structure*/
typedef struct
{
  /*num of carriers*/
  uint8 num_carr;

  /* Number of HARQs configured */
  uint8 num_harq_configured;

  /*MAC HS or EHS */
  hs_mac_ver_enum_type hs_or_ehs;
  /* Indicates if the HS-DSCh TB table to be chosen is octet aligned or not*/
  l1_dl_hs_dsch_tb_size_alignment_enum_type hsdsch_tb_size_alignment_type[L1_HSDPA_MAX_CARRIERS];
  /* 64QAM configured */
  boolean hs_64qam_configured[L1_HSDPA_MAX_CARRIERS];
}dec_hs_call_info_struct_type;

/* Local command structure for HS channel config command done */
typedef struct
{
  /* local command header type */
  l1_local_cmd_hdr_type           hdr;

  /* status of HS channel config */
  boolean status;
} l1_hs_chan_cfg_done_cmd_type;

/* Local command structure for HS channel config command done */
typedef struct
{
  /* local command header type */
  l1_local_cmd_hdr_type           hdr;
} l1_hs_log_submit_cmd_type;

/* Local command structure for DL channel cleanup after HS teardown is done */
typedef struct
{
  /* local command header type */
  l1_local_cmd_hdr_type           hdr;
} l1_hs_module_cleanup_after_hs_teardown_cmd_type;

/* Enumerating the different slot types in a DPCCH subframe */
typedef enum
{
  HS_DPCCH_SLOT_TYPE_ACKNACK,
  HS_DPCCH_SLOT_TYPE_CQI
}hs_ul_dpcch_slot_enum_type;

/* This structure stores the UL compressed mode parameters
   as well as the start and end of the gaps in units of bpg
   for a single frame */
typedef struct
{
  l1_cm_ul_param_struct_type    param;
  uint8                         start_bpg;
  uint8                         end_bpg;
}hs_cm_param_struct_type;

/* The structure used to hold the compressed mode parameters
   in the three consecutive DPCH frames */
typedef struct
{
  hs_cm_param_struct_type prev;
  hs_cm_param_struct_type cur;
  hs_cm_param_struct_type next;
}hs_cm_three_frame_param_struct_type;

/* This enumerates the various types of HS cell repointing */
typedef enum
{
  HS_SOFT_REPOINT,
  HS_SOFTER_REPOINT,
  HS_HARD_REPOINT
}hs_repoint_enum_type;

/* This enumerates the various types of HS CM info */
typedef enum
{
  HS_CM_PARAM_INFO_SET_PREV_CUR,
  HS_CM_PARAM_INFO_STOP_HS,
  HS_CM_PARAM_INFO_STOP_DL,
} hs_cm_param_info_enum_type;

/* Enumerations of the context in which HS+CM control tables are called */
typedef enum
{
  CHAN_CFG,
  VERIFY_DL_WDW,
  VERIFY_UL_WDW,
  SWITCH_CFG,
  CMMGR,
  DL_MAINT_EVT
} hs_cm_ctrl_context_enum_type;

/* This structure contains the history of HS CM operations in SW
   to trace the HS CM info given to FW
   */

typedef struct
{
  uint8 comb_cfn;
  uint8 dl_tick;
  hs_cm_ctrl_context_enum_type context;
  hs_cm_param_info_enum_type param_info_type;
  union
  {
    /* HS_CM_PARAM_INFO_SET_PREV_CUR */
    struct
    {
      uint8 prev_cfn;
      uint8 prev_start_bpg;
      uint8 prev_end_bpg;
      uint8 cur_start_bpg;
      uint8 cur_end_bpg;
    } set_prev_cur_info;

    /* HS_CM_PARAM_INFO_STOP_HS */
    struct
    {
      uint8 next_cfn;
      uint8 next_start_bpg;
      uint8 next_end_bpg;
    } set_next_info;

    /* HS_CM_PARAM_INFO_STOP_DL */
    struct
    {
      uint8 hs_dl_cfg_db_idx;
      uint16 first_subfr;
      uint8 valid[HS_NUM_SUB_FR_PER_RADIO_FR];
    } set_valid_info;
  } hs_cm_info;
} hs_cm_param_buf_struct_type;

typedef struct
{
  uint8 index;
  hs_cm_param_buf_struct_type 
    hs_cm_param_buf[HS_CM_PARAM_BUF_LEN];
} hs_cm_param_info_struct_type;

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* This variable indicate that HSDPA channel is active or not.
   At initialization it is reset to FALSE */
extern boolean hs_channel_active;
/* availability status of HSDPA information tables */
extern boolean hs_info_table_avail_status[HS_INFO_TABLE_COUNT];
/* HS DL config DB index corresponding to HSDPA information
   tables indices */
extern uint8 hs_dl_cfg_db_idx_for_info_table[HS_INFO_TABLE_COUNT];

/* Currently used DL/UL HSDPA information table index */
extern uint8 hs_cur_dl_info_idx;
extern uint8 hs_cur_ul_info_idx;

/* This is the CFN at which CQI override disable command is sent to MDSP */
extern uint16 hs_disable_cqi_override_cfn;

/* HSDPA DL/UL information table index to use in HS+CM subframe
   calculation when HSDPA config is pending */
extern uint8 hs_cm_cfg_dl_info_idx;
extern uint8 hs_cm_cfg_ul_info_idx;

/* The last CFN for which HS mod/demod control table are updated */
extern uint16 hs_cm_last_cfn_analyzed;

/* This indicate that MAC HS config indication is sent to MAC one frame
   after old config has expired. This is applicable only for reconfig
   action */
extern boolean hs_mac_cfg_indication;

/* Debug flag to reduce logging severity */
extern uint8 hs_log_debug;

/* Mutex for all HS configuration */
extern rex_crit_sect_type hs_cfg_crit_sec;

#define HS_CFG_INTLOCK()  REX_ISR_LOCK(&hs_cfg_crit_sec)
#define HS_CFG_INTFREE()  REX_ISR_UNLOCK(&hs_cfg_crit_sec)

/* Mutex for all HS logging */
extern rex_crit_sect_type hs_logging_crit_sec;

#define HS_LOG_INTLOCK()  REX_ISR_LOCK(&hs_logging_crit_sec)
#define HS_LOG_INTFREE()  REX_ISR_UNLOCK(&hs_logging_crit_sec)

extern l1_hsdpa_info_struct_type *hs_saved_info;

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================
FUNCTION hs_get_ue_category_from_nv

DESCRIPTION
  This function retrieves the UE category from NV.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_get_ue_category_from_nv(void);

/*===========================================================================
FUNCTION wl1_harq_buff_flush_read_nv

DESCRIPTION
  Reads the timer value to flush HARQ buffers at FW from NV.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void wl1_harq_buff_flush_read_nv(void);

/*===========================================================================
FUNCTION hs_harq_buff_flush_nv_read

DESCRIPTION
  Creates nv and reads the timer value to flush HARQ buffers at FW.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_harq_buff_flush_nv_read(void);

/*===========================================================================
FUNCTION hs_register_cme_hs_state_change_cb
         hs_deregister_cme_hs_state_change_cb

DESCRIPTION
  This function registers/deregisters a callback with CME to inform of the 
  HS state change.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  HS CME callback variables are updated.
===========================================================================*/

extern void hs_register_cme_hs_state_change_cb(
  /* Callback function pointer */
  HS_CME_STATE_CHANGE_CB_FUNC_TYPE *hs_cme_cb);

extern void hs_deregister_cme_hs_state_change_cb(void);

/*===========================================================================
FUNCTION hs_is_hs_dpcch_configured

DESCRIPTION
  This function returns the state of HS-DPCCH whether it is configured by RRC or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if HSDPA channel is active and HS-DPCCH is configured by RRC.
  FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean hs_is_hs_dpcch_configured(void);

/*===========================================================================
FUNCTION hs_is_hs_dpcch_active

DESCRIPTION
  This function returns the state of HS-DPCCH whether it is active or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if HSDPA channel is active and HS-DPCCH is also active.
  FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean hs_is_hs_dpcch_active(void);

/*===========================================================================
FUNCTION hs_get_ue_category

DESCRIPTION
  This function returns the UE category retrieved from NV.

DEPENDENCIES
  None. Assumes that the NV read has already happened.

RETURN VALUE
  UE category

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 hs_get_ue_category(void);

/*===========================================================================
FUNCTION hs_get_cell_db_idx

DESCRIPTION
  This function returns the HS cell DB index if HS is active else returns
  NO_CELL

DEPENDENCIES
  None

RETURN VALUE
  HS cell DB index if HS active
  NO_CELL otherwise

SIDE EFFECTS
  None
===========================================================================*/

extern  uint8 hs_get_cell_db_idx(l1_hsdpa_carrier_enum_type carrier);

/*===========================================================================
FUNCTION hs_get_serv_cell_psc

DESCRIPTION
  This function gets the HS serving cell PSC from the HS Current DL Config DB.

DEPENDENCIES
  None

RETURN VALUE
  If HS channel active, return the HS serving cell PSC
  If HS channel inactive, return HS_CFG_INVALID_PSC_HS_CELL_IDX (0xFFFF) 

SIDE EFFECTS
  
===========================================================================*/

extern uint16 hs_get_serv_cell_psc(void);

#ifdef FEATURE_WCDMA_DC_HSDPA
/*===========================================================================
FUNCTION hs_get_ordered_mc_psc

DESCRIPTION
  This function returns the DC PSC after querying from ordered and current
  configuration. 

DEPENDENCIES
  None

RETURN VALUE
  uint16 PSC

SIDE EFFECTS
  None
===========================================================================*/
extern uint16 hs_get_ordered_mc_psc(
  /* CPHY_SETUP_REQ command pointer */
  l1_setup_cmd_type *setup,
  l1_hsdpa_carrier_enum_type carrier);

/*===========================================================================
FUNCTION hs_get_ordered_dc_freq

DESCRIPTION
  This function returns the DC frequency after querying from ordered and current
  configuration. 

DEPENDENCIES
  None

RETURN VALUE
  uint16 PSC

SIDE EFFECTS
  None
===========================================================================*/
extern uint16 hs_get_ordered_dc_freq(
  /* CPHY_SETUP_REQ command pointer */
  l1_setup_cmd_type *setup,
  l1_hsdpa_carrier_enum_type carrier);
#endif /* FEATURE_WCDMA_DC_HSDPA */

#ifdef FEATURE_WCDMA_64QAM
/*===========================================================================
FUNCTION hs_get_64qam_state

DESCRIPTION
  This function returns the current state of mimo configuration.

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None
===========================================================================*/
extern boolean hs_get_64qam_state(l1_hsdpa_carrier_enum_type carrier);
#endif

#ifdef FEATURE_HSDPA_MAC_EHS
/*===========================================================================
FUNCTION hs_get_tbsize_table

DESCRIPTION
  This function returns TB-Size table configured.

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None
===========================================================================*/
extern l1_dl_hs_dsch_tb_size_alignment_enum_type hs_get_tbsize_table(l1_hsdpa_carrier_enum_type carrier);

/*===========================================================================
FUNCTION hs_get_mac_hs_or_ehs

DESCRIPTION
  This function returns whether MAC hs or e-hs is configured.

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None
===========================================================================*/
extern hs_mac_ver_enum_type hs_get_mac_hs_or_ehs(void);
#endif

#ifdef FEATURE_WCDMA_HS_FACH
/*===========================================================================
FUNCTION hs_query_efach_state

DESCRIPTION
  This function indicates whether UE is in E-FACH state

DEPENDENCIES
  None

RETURN VALUE
  TRUE if UE is in FACH state and HS is enabled
  FALSE if UE is not in E-FACH state

SIDE EFFECTS
  
===========================================================================*/
extern boolean hs_query_e_fach_state(void);

/*===========================================================================
FUNCTION hs_query_efach_state_for_db_idx

DESCRIPTION
  This function indicates whether UE is in E-FACH state after querying
  the specified Config DB database

DEPENDENCIES
  None

RETURN VALUE
  TRUE if UE is in FACH state and HS is enabled
  FALSE if UE is not in E-FACH state

SIDE EFFECTS
  
===========================================================================*/
extern boolean hs_query_e_fach_state_for_db_idx(uint8 hs_dl_cfg_db_idx);

/*===========================================================================
FUNCTION hs_query_dhrnti

DESCRIPTION
  This function returns Dedicated H-RNTI value if configured

DEPENDENCIES
  None

RETURN VALUE
  TRUE & Dedicated H-RNTI value if configured
  FALSE if Dedicated H-RNTI is not configured

SIDE EFFECTS
  
===========================================================================*/
extern boolean hs_query_dhrnti (uint16 *h_rnti);

#endif /* FEATURE_WCDMA_HS_FACH */

/*===========================================================================
FUNCTION hs_get_intf_buf_idx

DESCRIPTION
  This function returns the value of the HS interface buffer 
  index variable.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 hs_get_intf_buf_idx(void);

/*===========================================================================
FUNCTION hs_get_num_harq_configured

DESCRIPTION
  This function returns the valid harq ids.

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None
===========================================================================*/
extern uint8 hs_get_num_harq_configured(void);

/*===========================================================================
FUNCTION hs_get_ordered_psc

DESCRIPTION
  This function returns the HS PSC after querying from ordered and current
  configuration. It returns HS current PSC if HS is getting stopped.


DEPENDENCIES
  None

RETURN VALUE
  uint16 PSC
  
SIDE EFFECTS
  None
===========================================================================*/
extern uint16 hs_get_ordered_psc(
  /* CPHY_SETUP_REQ command pointer */
  l1_setup_cmd_type *setup);

/*===========================================================================
FUNCTION hs_get_call_info

DESCRIPTION
  This function maps info table index to config db index and then queries HS
  call type information. This will be used to do dec status logging.

DEPENDENCIES
  Assumed to be called after HS has indicated to start logging to DECHS

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_get_call_info(
  /* Pointer to HS call type */
  dec_hs_call_info_struct_type *call_info, 
  /* HS info index */
  uint8 info_table_idx);

/*===========================================================================
FUNCTION hs_get_txd_state

DESCRIPTION
  This function returns the TxD state of HS-PDSCH channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern mcalwcdma_demod_tx_div_mode_enum_type hs_get_txd_state(l1_hsdpa_carrier_enum_type carrier);

/*===========================================================================

FUNCTION hscfg_idle_init

DESCRIPTION
  This function initialize HS configuration and maintainance related
  variables when L1 goes to IDLE state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hscfg_idle_init(void);

/*===========================================================================
FUNCTION hscfg_init

DESCRIPTION
  This function initialize HS configuration and maintainance related
  variables. Following are the initializations done in this function.
  - Init HS active state to FALSE
  - Set HSDPA information table availability to TRUE
  - Clear HSDPA information shadow

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hscfg_init(void);

/*===========================================================================
FUNCTION hs_dsch_validate_config

DESCRIPTION
  This function validate HS-DSCH channel configuration parameters.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on validation result

SIDE EFFECTS
  None
===========================================================================*/

extern boolean hs_dsch_validate_config(
    /* HS configuration information structure pointer */
  l1_hsdpa_info_struct_type *hs_info);

/*===========================================================================
FUNCTION hs_scch_validate_config

DESCRIPTION
  This function validate HS-SCCH channel configuration parameters.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on validation result

SIDE EFFECTS
  None
===========================================================================*/

extern boolean hs_scch_validate_config(
  /* HS configuration information structure pointer */
  l1_hsdpa_info_struct_type *hs_info,
  /* HS-SCCH configurationm information structure pointer */
  l1_dl_hs_scch_info_struct_type *hs_scch_info);

/*===========================================================================
FUNCTION hs_dpcch_validate_config

DESCRIPTION
  This function validate HS-DPCCH channel configuration parameters.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE based on validation result

SIDE EFFECTS
  None
===========================================================================*/

extern boolean hs_dpcch_validate_config(
  /* HS-DPCCH configurationm information structure pointer */
  l1_ul_hs_dpcch_info_struct_type *hs_dpcch_info);

/*===========================================================================
FUNCTION hs_validate_config

DESCRIPTION
  This function validate HS channel configuration.

DEPENDENCIES
  This function is called only when HS operation type is not NOOP

RETURN VALUE
  TRUE or FALSE based on validation result

SIDE EFFECTS
  None
===========================================================================*/

extern boolean hs_validate_config(
  /* CPHY_SETUP_REQ command pointer */
  l1_setup_cmd_type *setup);

#ifdef FEATURE_WCDMA_HS_FACH
/*===========================================================================
FUNCTION hs_cell_trans_cmd_validate_config

DESCRIPTION
  This function validate HS channel configuration invoked at the time of
  CPHY_CELL_TRANS_REQ.

DEPENDENCIES
  This function is called only when HS operation type is not NOOP

RETURN VALUE
  TRUE or FALSE based on validation result

SIDE EFFECTS
  None
===========================================================================*/
extern boolean hs_cell_trans_cmd_validate_config(
  /* holds pointer to current L1 CPHY_CELL_TRANSITION_REQ cmd */
  l1_setup_cmd_type *cell_trans_cmd);
#endif /* FEATURE_WCDMA_HS_FACH */

/*===========================================================================
FUNCTION hs_calulate_scch_part1_scrambler

DESCRIPTION
  This function computes SCCH part 1 scrambler from H-RNTI.
  Calulation involve encoding of H-RNTI with 1/2 rate convolutional encoding
  and puncturing to get fina; 40 bit number.

DEPENDENCIES
  None

RETURN VALUE
  Returns SCCH part 1 scrambler in array. Array pointer is passed as function
  argument. Higher 8 bits are in lower 8 bits of first word and rest of lower
  order bits are in second word

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_calulate_scch_part1_scrambler(
  /* H-RNTI value */
  uint16 h_rnti,
  /* SCCH part 1 srcambler is returned in this array */
  uint32* scch_part1_scrambler_array_ptr);

#if 0
/*===========================================================================
FUNCTION hs_update_info_table

DESCRIPTION
  This function updates HSDPA information table. It does following for
  updating HSDPA information table.
  - It finds HSDPA information table that is available (not in use).
  - Populates various fields in table shadow.
  - Computes SCCH part 1 scrambler, DRM buffer size and starting address
    for each HARQ process.

DEPENDENCIES
  None

RETURN VALUE
  HSDPA information table index

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 hs_update_info_table(
  /* HSDPA channel configuration information structure pointer */
  l1_hsdpa_info_struct_type *hs_info,
  uint8 hs_info_table_idx[L1_HSDPA_MAX_CARRIERS]);
#endif

/*===========================================================================
FUNCTION hs_update_dl_channel_cfg_db

DESCRIPTION
  This function prepares DL HS combiner channel configuration database
  and upload in to mDSP database table corresponding to HSDPA information
  table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_update_dl_channel_cfg_db(
  /* HSDPA configuration operation type */
  hsdpa_setup_ops_enum_type hs_ops,
  /* HSDPA channel configuration information structure pointer */
  l1_hsdpa_info_struct_type *hs_info,
  /* HSDPA information table index */
  uint8 hs_info_table_idx[L1_HSDPA_MAX_CARRIERS]);

/*===========================================================================
FUNCTION hs_query_ul_chan_cfg_reqd

DESCRIPTION
  This function tells if HS UL channel configuration is required based on
  HS configuration information and operation type

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None
===========================================================================*/

extern boolean hs_query_ul_chan_cfg_reqd(
  /* HSDPA configuration operation type */
  hsdpa_setup_ops_enum_type hs_ops,
  /* HSDPA channel configuration information structure pointer */
  l1_hsdpa_info_struct_type *hs_info);

/*===========================================================================
FUNCTION hs_update_ul_channel_cfg_db

DESCRIPTION
  This function prepares UL HS DPCCH channel configuration database
  and upload in to mDSP memory. UL database table has no relation to
  HSDPA information table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_update_ul_channel_cfg_db(
  /* HSDPA configuration operation type */
  hsdpa_setup_ops_enum_type hs_ops,
  /* HSDPA channel configuration information structure pointer */
  l1_hsdpa_info_struct_type *hs_info);

/*===========================================================================
FUNCTION hs_set_suspended_state

DESCRIPTION
  This function updates the HS channel state if it is Suspended or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_set_suspended_state(boolean state);

/*===========================================================================
FUNCTION hsdpa_process_config_cmd

DESCRIPTION
  This function is invoked by all modules for channel activation/deactivation/
  reconfiguration. If a HS config is already in progress, it saves the
  command and invokes it after the current config is done.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hsdpa_process_config_cmd(
  /* Type of HS setup command - internal or external */
  hsdpa_setup_cmd_enum_type hs_setup_cmd,
  /* HSDPA configuration operation type */
  hsdpa_setup_ops_enum_type hs_ops,
  /* HSDPA channel configuration information structure pointer */
  l1_hsdpa_info_struct_type *hs_info,
  /* Function to be called when HS configuration has been completed */
  HS_CHANNEL_CFG_DONE_POST_CB_FUNC_TYPE *cfg_done_cb_func);

/*===========================================================================
FUNCTION hs_generate_serv_cell_change_evt

DESCRIPTION
  This function gets the UARCFN (freq), PSC, TPC index and H-RNTI for the ordered
  HS cell and then compares it with the current values. If the PSC or the 
  UARCFN of the HS serving cell has changed, then it generates an event and 
  posts it to the DM.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If HS serving cell changed, then it generates an EVENT_SERVING_CELL_CHANGE
  event, else it updates the current UARCFN, TPC index and H-RNTI variables.
===========================================================================*/

extern void hs_generate_serv_cell_change_evt(
  /* HSDPA configuration operation type */
  hsdpa_setup_ops_enum_type hs_ops);

/*===========================================================================
FUNCTION hs_verify_combiner_cfg_cmd_window

DESCRIPTION
  This function is pre call back function for HS combiner channel mDSP
  command. It is called just before submitting command to mDSP. It verifies
  the following:
  - Current time is with in config window or not
  - Current frame has CFN same as ordered config CFN

  If there is CFN number mismatch then it compares current CFN with ordered
  config CFN. It is expected in case of mismatch, current CFN is over run by
  one frame only as compared to ordered config CFN. If it is not then it is
  treated as abnormal condition and is not intented to be handled.
  If CFN is mismatched by 1 then start and stop global sub frame number (which
  ever applicable) is adjust by number of sub frames in one WCDMA radio frames.

  If current time is not in config window then DL HS combiner chanel event is
  scheduled.
  If current time is before config window start then event is posted at current
  tick offset by 2 slots.
  If current time is after config window end then event is posted at next frame
  config window start tick and it is ensured that event time is atleast 2 slots
  away from current tick.

  This function is called with ISR locked. Current time with resolution of
  ticks or 256 chip units can extracted from combiner counter running is H/W.
  but that may not give accurate picture of current CFN and correposnding tick
  time. It is required to have a source that provide tick and CFN latched at
  same instances.
  There is source in Raven H/W that maintain CFN for all combiner channels
  but it is not latched at STMR status dump.
  Therefore help is taken from UL time that latches Tx time inchipx8 and lower
  3 bits of frame/CFN counter. DL tick and CFN is extrapolated from UL time.
  DL time is ahead of UL time by 1024 chips or 4 ticks (256 chip units)

  There are 2 ranges of UL time.
  - Tick 0 to 145 or less than 150-4, UL CFN mod 8 is same as DL CFN mod 8
  - Tick 146 to 149 UL CFN mod 8 lags one behind DL CFN mod 8

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_verify_combiner_cfg_cmd_window(void);

/*===========================================================================
FUNCTION hs_dl_post_config_handler

DESCRIPTION
  This function is HS DL combiner config done call back function. It posts
  frame boundary event using DL combiner event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_dl_post_config_handler(
  /* status of mDSP command */
  uint16 status);

/*===========================================================================
FUNCTION hs_dl_comb_chan_evt_unsuspend_cfg_cmd

DESCRIPTION
  This function is HS combiner channel cfg event call back function. It is
  called when HS combiner chanel config event fires. It unsuspend HS combiner
  channel config mDSP command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_dl_comb_chan_evt_unsuspend_cfg_cmd(void);

/*===========================================================================
FUNCTION hs_flush_hs_info_mac_hs_reset

DESCRIPTION
  This function performs the MAC-hs reset as a part of the HS reconfiguration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Possible PDUs being dropped at L1 as a part of MAC-hs reset.
===========================================================================*/

extern void hs_flush_hs_info_mac_hs_reset(void);

/*===========================================================================
FUNCTION hs_dl_switch_config

DESCRIPTION
  This function is HS combiner channel frame boundary event call back function.
  It is called when HS combiner chanel frame boundary event fires DPCH frame
  boundary. It switches ordered config to current config. Old config is marked
  as invalid and ordered config index is set to invalid too.
  
  It there is no pending UL ordered config then traige is released if it was
  delayed by HS config module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_dl_switch_config(void);

/*===========================================================================
FUNCTION hs_enable_dl_comb_chan_event_handler

DESCRIPTION
  This function enables DL combiner channel event at its firing time. It
  saves the call back function and number of events required to call that
  call back function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_enable_dl_comb_chan_event_handler(
  /* Call back function to register */
  HS_CHAN_EVENT_FUNC_TYPE *event_handler_cb_func,
  /* number of events after which this CB function will be called */
  uint8 num_event_count,
  /* event firing tick */
  uint8 firing_time);

/*===========================================================================
FUNCTION hs_dl_comb_chan_event_handler

DESCRIPTION
  This function is HS DL combiner event handler used by DL timline module.
  It disables event and calls HS DL combiner event that is configured.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_dl_comb_chan_event_handler(void);

/*===========================================================================
FUNCTION hs_verify_ul_channel_cfg_cmd_window

DESCRIPTION
  This function is pre call back function for HS UL channel mDSP command.
  It is called just before submitting command to mDSP. It verifies
  the following:
  - Current time is with in UL config window or not
  - Current frame has UL CFN same as ordered config CFN

  Ordered config CFN is set based on DL sequecer CFN. While comparing UL
  CFN mode 8 with status dump, there are 2 possibilities in case of micmatch.

  - Ran over frame: In this case UL CFN will be one more than ordered
   config CFN. Adjust ordered config CFN and update global sub frame number

  - At the begning of DL frame, UL CFN is one less than DL CFN.
   Just schedule event at next UL CFN (the real intended ordered config CFN)
   config window start on UL DPCH timeline

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_verify_ul_channel_cfg_cmd_window(void);

/*===========================================================================
FUNCTION hs_ul_post_config_handler

DESCRIPTION
  This function is HS UL channel config done call back function. It posts
  frame boundary event using UL channel event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_ul_post_config_handler(
  /* status of mDSP command */
  uint16 status);

/*===========================================================================
FUNCTION hs_ul_chan_evt_unsuspend_cfg_cmd

DESCRIPTION
  This function is HS UL channel cfg event call back function. It is called
  when HS UL chanel config event fires. It unsuspend HS UL channel config
  mDSP command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_ul_chan_evt_unsuspend_cfg_cmd(void);

/*===========================================================================
FUNCTION hs_ul_switch_config

DESCRIPTION
  This function is HS UL channel frame boundary event call back function.
  It is called when HS UL chanel frame boundary event fires DPCH frame
  boundary. It switches ordered config to current config. Old config is marked
  as invalid and ordered config index is set to invalid too.
  
  It there is no pending DL ordered config then traige is released if it was
  delayed by HS config module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_ul_switch_config(void);

/*===========================================================================
FUNCTION hs_ul_switch_beta_table

DESCRIPTION
  This function switches beta gain table index and enable event for switching
  UL configuration at next frame.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_ul_switch_beta_table(void);

/*===========================================================================
FUNCTION hs_enable_ul_chan_event_handler

DESCRIPTION
  This function enables UL channel event at its firing time. It saves the
  call back function and number of events required to call that call back
  function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_enable_ul_chan_event_handler(
  /* Call back function to register */
  HS_CHAN_EVENT_FUNC_TYPE *event_handler_cb_func,
  /* number of events after which this CB function will be called */
  uint8 num_event_count,
  /* event firing tick */
  uint8 firing_time);

/*===========================================================================
FUNCTION hs_ul_chan_event_handler

DESCRIPTION
  This function is HS UL event handler used by UL timline module. It disables
  event and calls HS UL event that is configured.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_ul_chan_event_handler(void);

/*===========================================================================
FUNCTION hs_update_chan_active_state

DESCRIPTION
  This function updates state of HS channel active variable based on action
  type. It it find variables in incorrect state based on action then it
  prints MSG_ERROR and corrects the state.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates variable hs_channel_active
===========================================================================*/

extern void hs_update_chan_active_state(
  /* action type based of which to update HS channel active state */
  hs_channel_cfg_mdsp_action_enum_type action_type);

/*===========================================================================
FUNCTION hs_chan_cfg_done_send_cmd

DESCRIPTION
  This function sends local command to L1 manager for HS channel config
  command done.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_chan_cfg_done_send_cmd(boolean status);

/*===========================================================================
FUNCTION hs_deactivate_hsdpa_chan

DESCRIPTION
  This function deactivates HSDPA channels. It is intended to be called at
  transition out of DCH state or when HS serving RL is dropped without
  repointing it before. It registers deactivate compete post call back function
  with HS channel configuration function and it it is not NULL then it will
  be called to mark end of deactivation process.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_deactivate_hsdpa_chan(
  /* Function to be called when HS channel deactvivation has been completed */
  HS_CHANNEL_CFG_DONE_POST_CB_FUNC_TYPE *cfg_done_cb_func);

/*===========================================================================
FUNCTION hscfg_clk_regime_deregister_client

DESCRIPTION
  This function is the response to the local command posted to deregister
  HS and related clients from clock regime.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  HS and related clock regime clients are deregistered.
===========================================================================*/

extern void hscfg_clk_regime_deregister_client(void);
/*===========================================================================
FUNCTION hs_handle_hs_cfg_log_pkt

DESCRIPTION
  This function handles HS configuration log packet submission in task
  context.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_handle_hs_cfg_log_pkt(void);


/*===========================================================================
FUNCTION hs_enable_finger_update_handler

DESCRIPTION
  This function is call back function for enabling or disabling HS on new
  finger assign or existing finger getting deassigned during per RL triage
  for HS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_enable_finger_update_handler(
  /* cell db index */
  uint8 cell_db_idx,
  /* fingher index that need HS enable or disable */
  uint8 fing_index,
  /* finger state (un-assigned or not) */
  boolean is_assigned);

/*===========================================================================
FUNCTION hs_ref_finger_change_handler

DESCRIPTION
  This function is call back function for change in reference finger update.
  It is called by per RL triage for HS when reference finger changes.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_ref_finger_change_handler(
  /* cell db index */
  uint8 cell_db_idx,
  /* old reference finger indxe */
  uint8 old_fing_index,
  /* new reference finger index */
  uint8 new_fing_index);

/*===========================================================================
FUNCTION hs_ref_finger_change_handler

DESCRIPTION
  This function is call back function for change in lock status for HS
  reference finger. It is called by per RL triage for HS when reference finger
  is not available ot there is change in lock status of reference finger.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_ref_fing_lock_status_update_handler(
  /* reference finger index */
  uint8 fing_index,
  /* lock status of reference finger */
  boolean lock_status);

/*===========================================================================
FUNCTION hs_get_cur_dl_cfg_ptr
         hs_get_cur_ul_cfg_ptr

DESCRIPTION
  This function returns pointer to current HS DL/UL configuration information
  structure pointer.
  If current information index is not valid, it returns NULL. Possible only
  if HSDPA has not yet been activated since dedicated mode has started

DEPENDENCIES
  None

RETURN VALUE
  Pointer to structure type hs_dl_channel_cfg_db_struct_type or 
  hs_ul_channel_cfg_db_struct_type or NULL

SIDE EFFECTS
  None
===========================================================================*/

extern hs_dl_channel_cfg_db_struct_type* hs_get_cur_dl_cfg_ptr(void);
extern hs_ul_channel_cfg_db_struct_type* hs_get_cur_ul_cfg_ptr(void);

#ifdef FEATURE_WCDMA_DC_HSDPA
/*===========================================================================
FUNCTION hs_handle_scch_order

DESCRIPTION
  This function handles HS-SCCH orders indicated by firmware.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void hs_handle_scch_order(void);

/*===========================================================================
FUNCTION hs_initiate_dc_drop_operation

DESCRIPTION
  This function is the local command handler for dc hs channel drop. this will
  initiate drop of the second carrier.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void hs_initiate_dc_drop_operation(
    /** cb func to be called after dc drop */
    HS_CHANNEL_CFG_DONE_POST_CB_FUNC_TYPE *post_dc_cfg_hs_cb);
#endif /* FEATURE_WCDMA_DC_HSDPA */


/*===========================================================================
FUNCTION hs_send_submit_log_cmd

DESCRIPTION
  This function send HS submit log packet local command to L1M.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_send_submit_log_cmd(void);

/*===========================================================================
FUNCTION hs_do_log_submit

DESCRIPTION
  This function is called as result of submit HS log local command. It calls
  all possible candidates in HS that do accumulated logging.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_do_log_submit(void);

/*===========================================================================
FUNCTION hs_read_commit_left_over_log_pkts

DESCRIPTION
This function is called during HS tear down in HS Unified call back 
before deactivating HS channels. It reads and commits pending log packets
before HS channel is diabled.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Logging adds overhead cycles for Hs tear down in HS unified call back.
===========================================================================*/

extern void hs_read_commit_pending_log_pkts(void);

/*===========================================================================
FUNCTION hs_init_hs_cell_idx

DESCRIPTION
  This function reset all HS cell index in use to FALSE and init PSC for all
  HS cell index to invalid value.
  It should be called at start of DCH state before HS cell index are
  allocated to RLs in active set.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  HS cell index management related variables are reset
===========================================================================*/

extern void hs_init_hs_cell_idx(void);

/*===========================================================================
FUNCTION hs_allocate_hs_cell_idx_for_psc

DESCRIPTION
  This function alllocate HS cell index for PSC. It scans thro' already
  allocated HS cell index. If requested PSC is already allocated HS cell
  index, it returns that HS cell index. Otherwise if allocates first HS cell
  index not in use, save PSC corresponding to it and returns its value.
  
  In case there is no HS cell index available then it returns invalid HS
  cell index value 0xFF.

DEPENDENCIES
  None

RETURN VALUE
  Returns HS cell index value if available otherwise return invalid HS
  cell index as error indication.

SIDE EFFECTS
  HS cell index if available is marked in use and PSC is stored in
  corresponding location.
===========================================================================*/

extern uint8 hs_allocate_hs_cell_idx_for_psc(
  /* Primary scrambling code for which HS cell index need to be allocated */
  uint16 pri_scr_code);

/*===========================================================================
FUNCTION hs_remove_hs_cell_idx_for_psc

DESCRIPTION
  This function removes HS cell index for PSC. It scans thro' allocated
  HS cell index. If requested PSC is exists it marks that HS cell index 
  in use flag to FALSE and inits PSC value for that HS cell index to
  invalid PSC value.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  HS cell index if in use by this PSC is marked available and PSC is reset
===========================================================================*/

extern void hs_remove_hs_cell_idx_for_psc(
  /* Primary scrambling code for which HS cell index need to be removed */
  uint16 pri_scr_code);

/*===========================================================================
FUNCTION hs_query_hs_cell_idx_for_psc

DESCRIPTION
  This function returns HS cell index for PSC. It scans thro' allocated
  HS cell index. If requested PSC is exists it returns that value otherwise
  returns invalid value as error indication.
  
DEPENDENCIES
  None

RETURN VALUE
  HS cell index of PSC if available otherwise invalid value 0xFF.

SIDE EFFECTS
  HS cell index if in use by this PSC is marked available and PSC is reset
===========================================================================*/

extern uint8 hs_query_hs_cell_idx_for_psc(
  /* Primary scrambling code for which HS cell index need to be found */
  uint16 pri_scr_code);

/*===========================================================================
FUNCTION hs_query_hs_mcpm_relinquish_pending

DESCRIPTION
  This function returns the status of flag that indicates whether 
  HS MCPM relinquish has been pended.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: HS MCPM relinquish has been pended 
  FALSE: otherwise 

SIDE EFFECTS
  None
===========================================================================*/

extern boolean hs_query_hs_mcpm_relinquish_pending(void);


/*===========================================================================
FUNCTION hs_cm_init

DESCRIPTION
  This function resets the state of HS+CM related variables.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_cm_init(void);

/*===========================================================================
FUNCTION hs_is_hs_config_pending

DESCRIPTION
  This function returns the state of HS config.
  
DEPENDENCIES
  None

RETURN VALUE
  Boolean indicating if the HS configuration is pending.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean hs_is_hs_config_pending(void);

/*===========================================================================
FUNCTION hs_update_cm_ctrl_context

DESCRIPTION
  This function updates the context in which HS+CM control tables are being 
  updated. This is needed for debugging purpose only. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_update_cm_ctrl_context(
  /* The context in which HS+CM table updation is called */
  hs_cm_ctrl_context_enum_type context);

/*===========================================================================
FUNCTION hs_set_prev_cur_cm_params

DESCRIPTION
  This function sets the previous and current CM transmission gap information
  into the hs_cm_params structure. These field will be used to compute HS+CM
  for the frame marked CUR. The CFN passed in as the parameter is considered
  to be the PREV frame CFN.
  This function is intended to be called only from HS config channel related
  functions
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The PREV and CUR CM gap information updated.
  hs_cm_reconfig_cfn is also set to current CFN
===========================================================================*/

extern void hs_set_prev_cur_cm_params(uint8 prev_cfn);

/*===========================================================================
FUNCTION hs_get_start_end_cm_bpg

DESCRIPTION
  This function calculates the start and the end position for compressed mode
  in the desired frame. The position markers are calculated in the units of bpg.
  It is assumed that the CM information from UL CM manager is already computed.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The compressed mode gaps position is returned in bpg.
===========================================================================*/

extern void hs_get_start_end_cm_gaps(
  /* Pointer to the struct storing CM info for the frame in consideration */
  hs_cm_param_struct_type* frame);

/*===========================================================================
FUNCTION hs_stop_dl_during_cm_gaps

DESCRIPTION
  This function calculates the subframes in which the HS DL transmissions
  should not be demodulated as either HS-SCCH or HS-PDSCH overlaps with
  a downlink transmission gaps.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The subframes to be ignored are updated in the CM control table shadow.
===========================================================================*/

extern void hs_stop_dl_during_cm_gaps(
  /* The combiner config database index to use */
  uint8 hs_dl_cfg_db_idx,
  /* The start subframe no. of the SCCH falling completely with the DPCH frame*/
  uint16 hs_pdsch_dl_start_global_sub_fr_num);

/*===========================================================================
FUNCTION hs_stop_ul_during_cm_gaps

DESCRIPTION
  This function calculates the subframes in which the HS UL ACK/NACK or CQI
  should not be transmitted as the corresponding HS-DPCCH overlaps with
  an uplink DPCH transmission gaps.

  The function takes an argument indicating whether ACK/NACK or CQI subframes
  are handled.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The subframes to be ignored are updated in the shadow.
===========================================================================*/

extern void hs_stop_ul_during_cm_gaps(
  /* slot type can be wither ACK/NACK or CQI */
  uint8 slot_type, 
  /* The HS-DPCCH config database index to use */  
  uint8 hs_ul_cfg_db_idx,
  /* The UL start global subframe num within DPCH frame*/
  uint16 hs_pdsch_ul_start_global_sub_fr_num);

/*===========================================================================
FUNCTION hs_stop_cqi_during_cm_gaps

DESCRIPTION
  This function calculates the subframes in which the HS UL CQI
  should be DTXed as the corresponding 3 slot measurement overlaps
  with the downlink transmission gap.

 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The subframes to be ignored are updated in the shadow.
===========================================================================*/

extern void hs_stop_cqi_during_cm_gaps(
  /* The HS-DPCCH config database index to use */
  uint8 hs_ul_cfg_db_idx,
  /* The UL start global subframe number */
  uint16 hs_pdsch_ul_start_global_sub_fr_num);

/*===========================================================================
FUNCTION hs_stop_hs_during_cm_gaps

DESCRIPTION
  This function calculates the subframes in which HSDPA transmission should 
  not be demodulated as well as HSDPA UL transmission should be DTXed. It 
  comptes the target CFN for which analysis has to be done and gets the CM 
  information for that CFN. It then calls the above appropriate functions
  to decide if HS operation has to be ignored or not 
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The subframes to be ignored are written to the mDSP.
===========================================================================*/

extern void hs_stop_hs_during_cm_gaps(
  /* The combiner config db index to use */
  uint8 hs_dl_cfg_db_idx, 
  /* the HS-DPCCH config db index to use */
  uint8 hs_ul_cfg_db_idx, 
  /* The CFN for which HS demod/mod is to be skipped */
  uint8 target_cfn);


/*===========================================================================
FUNCTION hs_cm_dl_update_logging

DESCRIPTION
  This function is called periodically every frame. It logs the status of
  the subframes within the current frame with respect to whether the HS DL
  transmission has to be ignored or not. It maintains logging state of the
  log packet as well.
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_cm_dl_update_logging(void);

/*===========================================================================
FUNCTION hs_cm_dl_send_log_submit_cmd

DESCRIPTION
  This function is called to post HS downlink CM control log submit command.
  This function checks if a previous log is pending or not. If pending, then 
  the current buffer to log is discarded. If no DL CM control log is pending,
  then it calls function to issue a L1 local command to submit the log.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If a log buffer pending to be submitted, the current buffer is discarded.
===========================================================================*/

extern void hs_cm_dl_send_log_submit_cmd(
  /* index of the log buffer to be submitted */
  uint8 buffer_idx);

/*===========================================================================
FUNCTION hs_cm_demod_ctrl_submit_log_buffer

DESCRIPTION
  This function is submits the HS+CM DL control log buffer. After successful 
  submission, it releases the resources held up by the log submit operation.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The log buffer availability status as well as the submit pending flags are
  cleared upon successful log submit.
===========================================================================*/

extern void hs_cm_demod_ctrl_submit_log_buffer(void);

/*===========================================================================
FUNCTION hs_cm_ul_update_logging

DESCRIPTION
  This function is called periodically every frame. It logs the status of
  the subframes within the current frame with respect to whether the HS UL
  transmission has to be ignored or not. It maintains logging state of the
  log packet as well.
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_cm_ul_update_logging(void);

/*===========================================================================
FUNCTION hs_cm_ul_send_log_submit_cmd

DESCRIPTION
  This function is called to post HS uplink CM control log submit command.
  This function checks if a previous log is pending or not. If pending, then 
  the current buffer to log is discarded. If no UL CM control log is pending,
  then it calls function to issue a L1 local command to submit the log.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If a log buffer pending to be submitted, the current buffer is discarded.
===========================================================================*/

extern void hs_cm_ul_send_log_submit_cmd(
  /* index of the log buffer to be submitted */
  uint8 buffer_idx);


/*===========================================================================
FUNCTION     HS_GET_DPCH_OFFSET

DESCRIPTION  This function returns the DPCH to starting HS-DPCCH offset 

DEPENDENCIES This function can only be called in DCH state by ENC module 
             during maint  event .

RETURN VALUE returns DPCH to starting HS-DPCCH offset if hs is cfged 
             else returns invalid value(31)

SIDE EFFECTS None
===========================================================================*/
extern uint8 hs_get_dpch_offset(void);

/*===========================================================================
FUNCTION hs_get_cctrch_id_for_cfg_db_idx

DESCRIPTION
  This function returns the CCTrCh id in the DL cfg DB.

DEPENDENCIES
  None

RETURN VALUE
  CCTrCh ID.

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 hs_get_cctrch_id_for_cfg_db_idx(uint8 hs_cfg_db_idx);

/*===========================================================================
FUNCTION hs_get_mac_hs_or_ehs_for_cfg_db_idx

DESCRIPTION
  This function returns whether MAC-hs or MAC-ehs is configured.

DEPENDENCIES
  None

RETURN VALUE
  MAC-hs or MAC-ehs enum.

SIDE EFFECTS
  None
===========================================================================*/

extern hs_mac_ver_enum_type hs_get_mac_hs_or_ehs_for_cfg_db_idx(uint8 hs_cfg_db_idx);

/*===========================================================================
FUNCTION hs_get_q_id_mapping_for_lc_id

DESCRIPTION
  This function returns the Q-ID mapping for the LC-ID and Config DB idx 
  specified.

DEPENDENCIES
  None

RETURN VALUE
  Q-ID value.

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 hs_get_q_id_mapping_for_lc_id(uint8 lc_id, uint8 hs_cfg_db_idx);

#ifdef FEATURE_WCDMA_HS_FACH
/*===========================================================================
FUNCTION hs_get_efach_h_rnti_bmsk_for_cfg_db_idx

DESCRIPTION
  This function returns E-FACH H-RNTI bitmask value.

DEPENDENCIES
  None

RETURN VALUE
  H-RNTI bitmask value.

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 hs_get_efach_h_rnti_bmsk_for_cfg_db_idx(uint8 hs_cfg_db_idx);
#endif /* FEATURE_WCDMA_HS_FACH */

/*===========================================================================
FUNCTION        hs_handle_hs_aset_update

FILE NAME       hscfg.c

DESCRIPTION     This function manages HS cell index when active set is changes.
                HS cell index is allocated and updated for any new RL added.
                HS cell index is removed for any PSC that is removed for
                avtive set. If removed RL PSC is the current HS serving cell
                PSC then l1 setup command is checked if there is already
                existing removal or repointing of removed RL to another RL.
                If this is not the case then it checks if HS STOP action
                can be set to setup command. It STOP action can be set then it
                set that action other wise it is unhandled situation and results
                in ERR_FATAL

DEPENDENCIES    Assumes HS cell index set is already initialized and current
                L1 state is L1M_DCH.

RETURN VALUE    None

SIDE EFFECTS    Allocation and deallocation of HS cell index for newly added
                or removed RLs. It can (not always) result in finger config
                command to update HS cell index.
===========================================================================*/

extern void hs_handle_hs_aset_update(
  /* CPHY_SETUP_REQ command pointer */
  l1_setup_cmd_type *setup);

/*===========================================================================
FUNCTION hs_chan_deactivate_done

DESCRIPTION
  This function handles the post HS chan deactivation and posts the local 
  command to proceed with the DL DCH cleanup
  initiate drop of the second carrier.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void hs_chan_deactivate_done(boolean status);


/*===========================================================================
FUNCTION     hscfg_get_hs_rl_tto

DESCRIPTION  This function returns the hs rl tto

DEPENDENCIES None 

RETURN VALUE hs rl tto

SIDE EFFECTS None
===========================================================================*/
extern uint8 hscfg_get_hs_rl_tto(void);

/*===========================================================================
FUNCTION     hscfg_register_dsr_callback

DESCRIPTION  

DEPENDENCIES None 

RETURN VALUE 

SIDE EFFECTS None
===========================================================================*/
extern void hscfg_register_dsr_callback(WL1_DSR_HS_TRANS_IND_CB_FUNC_TYPE *dsr_cb);
/*===========================================================================
FUNCTION     hscfg_deregister_dsr_callback

DESCRIPTION  

DEPENDENCIES None 

RETURN VALUE 

SIDE EFFECTS None
===========================================================================*/
extern void hscfg_deregister_dsr_callback(void);

/*===========================================================================
FUNCTION hs_update_dsr_cb

DESCRIPTION
  Inform DSR controller that HS state is changing
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void hs_update_dsr_cb(boolean hs_enabled);

/*===========================================================================
FUNCTION     hs_get_ordered_ssc_state

DESCRIPTION  This function returns the SSC for HSDPA

DEPENDENCIES None 

RETURN VALUE Non-zero SSC if HSDPA is active and SSC is configured
             Zero otherwise

SIDE EFFECTS None
===========================================================================*/
extern uint8 hs_get_ordered_ssc_state(
  /* CPHY_SETUP_REQ command pointer */
  l1_setup_cmd_type *setup);

/*===========================================================================
FUNCTION hscfg_get_txdiv_for_hs_rl

DESCRIPTION
  This function returns the txd values for both single and dual carrier HS RLs

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void hscfg_get_txdiv_for_hs_rl(
  /* Pointers to primary and secondary carrier TxD */
  mcalwcdma_demod_tx_div_mode_enum_type *prim_carrier_txdiv, 
  mcalwcdma_demod_tx_div_mode_enum_type *sec_carrier_txdiv);

/*===========================================================================
FUNCTION hs_fast_hs_dpcch_setup

DESCRIPTION
  This function is invoked once Collision Resolution is complete.
  Since the gap between Collision Resolution and the start of the HS-DPCCH
  transmission is about 6 to 7 slots, we need this function to be very
  optimized in sending the HS-DPCCH start command to FW. FW needs 1 slot
  to go active, so L1 has only 5 to 6 slots typically to prepare the HS-DPCCH
  config command.
  1. In the HS-DPCCH config DB, only the following need to be updated
     a. UL DPCCH to HS-DPCCH offset
     d. Start global subframe number

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void hs_fast_hs_dpcch_setup (void);

/*===========================================================================
FUNCTION hs_deactivate_hs_dpcch

DESCRIPTION
  This function is invoked to internally teardown HS-DPCCH channel from
  HS-RACH module
  1. In the HS-DPCCH config DB, only the following need to be updated
     a. action
     b. Stop global subframe number

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void hs_deactivate_hs_dpcch (void);

/*===========================================================================
FUNCTION hs_channel_implicit_drop_indication

DESCRIPTION
    Indicates whether need to drop HS channel as part of HHO

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern boolean hs_channel_implicit_drop_indication (l1_setup_cmd_type *cmd);

/*===========================================================================
FUNCTION hs_get_ord_serv_psc

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern uint16 hs_get_ord_serv_psc(hsdpa_setup_ops_enum_type hs_ops, l1_hsdpa_info_struct_type *hs_info);

/*===========================================================================
FUNCTION hs_submit_stop_events_to_dm

DESCRIPTION
  This API flushes all the HS events to the diag based on the bitmask which was set.
  The API is called by the L1M module after drop sequence is processed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_submit_stop_events_to_dm(void);

/*===========================================================================
FUNCTION hs_submit_start_recfg_events_to_dm

DESCRIPTION
  This API flushes all the HS events to the diag based on the bitmask which was set.
  The API is called by the L1M module at CPHY_SETUP_DONE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_submit_start_recfg_events_to_dm(void);

/*===========================================================================
FUNCTION hs_get_start_gsfn_for_cfn

DESCRIPTION
  This function will compute the starting HS-DSCH global subframe
  number in target CFN

DEPENDENCIES
  None

RETURN VALUE
  Starting Global Subframe Number if HS is active (or config on-going).
  0xFFFF otherwise.

SIDE EFFECTS
  None
===========================================================================*/
extern uint16 hs_get_start_gsfn_for_cfn (uint8 target_cfn);

/*===========================================================================
FUNCTION hs_get_activation_status

DESCRIPTION
  This API provides the HS Activation Status for specific carrier.

ASSUMPTION
  The activation status in the HS config DB is based on RRC config and 
  is not updated dynamically when the SCCH order is received.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if Carrier is Active, else FALSE

SIDE EFFECTS
  None
===========================================================================*/
extern boolean hs_get_activation_status(l1_hsdpa_carrier_enum_type carrier_idx);

/*===========================================================================
FUNCTION hs_update_activation_status_in_cur_db

DESCRIPTION
  This function is called by the carr config module to update the HS activation status in the 
cur db.

DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None
===========================================================================*/
extern void hs_update_activation_status_in_cur_db(uint8 carrier_idx, boolean activation_status_value);

/*===========================================================================
FUNCTION hs_get_category_for_mcpm

DESCRIPTION
  This function is called by the carr config module to update MCPM for the correct UE cat at any instance.
  This is part of the MCPM - carr config optimization

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 hs_get_category_for_mcpm();

/*===========================================================================
FUNCTION hs_cm_ul_send_log_submit_cmd

DESCRIPTION
  This function is called to post HS uplink CM control log submit command.
  This function checks if a previous log is pending or not. If pending, then
  the current buffer to log is discarded. If no UL CM control log is pending,
  then it calls function to issue a L1 local command to submit the log.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If a log buffer pending to be submitted, the current buffer is discarded.
===========================================================================*/

extern void hs_cm_ul_send_log_submit_cmd(uint8 buffer_idx);

/*===========================================================================
FUNCTION hs_stop_hs_during_cm_gaps

DESCRIPTION
  This function calculates the subframes in which HSDPA transmission should
  not be demodulated as well as HSDPA UL transmission should be DTXed. It
  computes the target CFN for which analysis has to be done and gets the CM
  information for that CFN. It then calls the above appropriate functions
  to decide if HS operation has to be ignored or not

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The subframes to be ignored are written to the mDSP.
===========================================================================*/

extern void hs_stop_hs_during_cm_gaps(
  /* The combiner config db index to use */
  uint8 hs_dl_cfg_db_idx,
  /* the HS-DPCCH config db index to use */
  uint8 hs_ul_cfg_db_idx,
  /* The CFN for which HS demod/mod is to be skipped */
  uint8 target_cfn);

/*===========================================================================
FUNCTION hs_stop_cqi_during_cm_gaps

DESCRIPTION
  This function calculates the subframes in which the HS UL CQI
  should be DTXed as the corresponding 3 slot measurement overlaps
  with the downlink transmission gap.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The subframes to be ignored are updated in the shadow.
===========================================================================*/

extern void hs_stop_cqi_during_cm_gaps(
  /* The HS-DPCCH config database index to use */
  uint8 hs_ul_cfg_db_idx,
  /* The UL start global subframe number */
  uint16 hs_pdsch_ul_start_global_sub_fr_num);

/*===========================================================================
FUNCTION hs_stop_ul_during_cm_gaps

DESCRIPTION
  This function calculates the subframes in which the HS UL ACK/NACK or CQI
  should not be transmitted as the corresponding HS-DPCCH overlaps with
  an uplink DPCH transmission gaps.

  The function takes an argument indicating whether ACK/NACK or CQI subframes
  are handled.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The subframes to be ignored are updated in the shadow.
===========================================================================*/

extern void hs_stop_ul_during_cm_gaps(
  /* slot type can be wither ACK/NACK or CQI */
  uint8 slot_type,
  /* The HS-DPCCH config database index to use */
  uint8 hs_ul_cfg_db_idx,
  /* The UL start global subframe num within DPCH frame*/
  uint16 hs_pdsch_ul_start_global_sub_fr_num);

/*===========================================================================
FUNCTION hs_cm_ul_update_logging

DESCRIPTION
  This function is called periodically every frame. It logs the status of
  the subframes within the current frame with respect to whether the HS UL
  transmission has to be ignored or not. It maintains logging state of the
  log packet as well.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void hs_cm_ul_update_logging();

/*===========================================================================
FUNCTION hs_cm_dl_logging_init

DESCRIPTION
  This function initializes all the global variables that are needed for
  HS+CM Demod logging.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  All the global variables for HS+CM Demod logging are reset.
===========================================================================*/

extern void hs_cm_dl_logging_init();

/*===========================================================================
FUNCTION hs_cm_ul_logging_init

DESCRIPTION
  This function initializes all the global variables that are needed for
  HS+CM Mod logging.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  All the global variables for HS+CM Mod logging are reset.
===========================================================================*/
extern void hs_cm_ul_logging_init();

/*===========================================================================
FUNCTION hs_get_ul_channel_cfg_db

DESCRIPTION
  Return a pointer to hs maintained ul channel config db

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern hs_ul_channel_cfg_db_struct_type const* hs_get_ul_channel_cfg_db(void);
/*===========================================================================
FUNCTION hs_get_dl_channel_cfg_db

DESCRIPTION
  Return a pointer to hs maintained dl channel config db

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern hs_dl_channel_cfg_db_struct_type const* hs_get_dl_channel_cfg_db(void);

/*===========================================================================
FUNCTION hs_cm_info_upd

DESCRIPTION
  Update CQI bias info based on current CM state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void hs_cm_info_upd(void);

#ifdef FEATURE_QSH_DUMP
/*===========================================================================
FUNCTION wl1_hs_debug_dump

DESCRIPTION
  This function dumps local variables into wl1_datapath_mini_debug_info_struct_type

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void wl1_hs_debug_dump(wl1_datapath_mini_debug_info_struct_type *dump_ptr);
#endif /* FEATURE_QSH_DUMP */

#endif /* HSCFG_H */
