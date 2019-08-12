#ifndef EULCFG_H
#define EULCFG_H

/*============================================================================
             WCDMA L1 HSUPA UL configuration related header file

DESCRIPTION
This files contains all necessary macros and definitions for maintainaing
HSUPA uplink physical channel configuration.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/eulcfg.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/06/16   sg      Remove support to reuse SS tables, always rebuild.
10/06/15   gp      Avoid skiping of CPC STOP command when START command is queued for WFW 
11/19/14   vr      Deprecating and removing 0x4312 log packet code.
10/20/14   ar      Changes for removal of dl drx ctrl sched sequence lock dependency 
                   from EUL ISR
10/01/14   gv      Code cleanup. Removal of stale and unused code. 
08/18/14   vr      Cleanup code for HSRACH 'conventional' seamless recfg.
07/28/14   pkg     Code Check-in for 16QAM feature.
07/15/14   ar      Support for A2 Power request callbacks
07/17/14   ssv     Enhancing 0x422c log packet
06/23/14   pkg     Moving DC-HSUPA specific code under FEATURE_WCDMA_DC_HSUPA flag.
06/19/14   yw      Fix for secondary carrier CPC not able to start if DC is activated during primary enabling delay
06/06/14   gp      Disable EUL FIQ before mod timing for HSRACH seamless reconfig and 
                   re-enable after post callback
05/31/14   vr      Converting large local to dynamically allocated variable to avoid
                   stack overflow.
05/27/14   yw      API to monitor Secondary EUL dirty bit 
05/11/14   yw      MCPM Opts for unification of HSDPA/HSUA Clk requests.
05/19/14   ar      Send correct action CFN to DRX module during TTI reconfig from 10ms to 2ms
05/14/14   cjz     No 0xFF as invalid buffer index: e_ul_cpc_dtx_curr_cfg_db_indx and e_ul_cpc_dtx_ord_cfg_db_indx
05/13/14   ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
05/01/14   gp      BOLT HSRACH L1 SW changes
03/26/14   sks     Reconfig HSUPA clocks through the request_client rather than the relinquish_client.
11/21/13   ash     Added changes to call RF Temp Only in WCDMA L1 Task.
11/20/13   gp      Crash fix for race condition between EUL cleanup and EUL ISR firing in parallel
10/29/13   yw      Fix for race condition causing wrong ETFCI sel when sec EUL becomes active
10/29/13   yw      Fix for EUL and UL DPCCH cfg cmd timing violation during RAB release
10/25/13   cjz     Validate EUL NW config to check if ETFCI 0 is supported
10/14/13   pr/sr   Channel config optimizations
07/12/13   yw      CPC DTX support for DC-HSUPA
07/11/13   ymu     Support for secondary carrier OOS and recfg.
07/02/13   yw      Extend UE cat information to inlcude till Cat 9.
06/25/13   yw      Code cleanup for IDLE REQ, EUL/EDL MODULE_CLEANUP   
06/07/13   at      EUL event reporting to Diag changes for L1M redesign 
05/22/13   oh      Adding DC-HSUPA framework support
05/17/13   vr      Ensure EUL DL is dropped before relinquishing the MCPM
01/11/13   ash     WPLT Changes for EUL and CPC DTX Dime Bringup 
01/04/13   raj     Changes as a part of ZI memory optimization
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/08/12   gv      Feature HS RACH Initial code check in
03/06/12   at      Corrected the hho req mask handling.
02/01/12   raj     Nikel Feature cleanup.
02/01/12   gnk     Nikel Feature cleanup.
01/19/12   zr      Mainlined FEATURE_WCDMA_GENESIS_SW 
01/03/12   at      Complete support for Back up and revert of HS SCCH orders 
                   during HHO failure and revert back.
12/21/11   xl      Merge from MPSS.NI.1.1
11/09/11   sb      Moved MCPM relinquish during EUL Drop from ISR to Task context
10/26/11   oh      Added feature for WPLT Mode.
04/28/11   sj      upmerge from offline/main
04/26/11   gnk     Removed FEATURE_WCDMA_HSUPA_IRQ_DELAY_FLOW_CONTROL feature
04/08/11   rgn     Added function to return dtx config cfn
02/07/11   oh      Ensure that DTX is setup only after sync is established on new cell.
01/31/11   oh      Request high axi clk at the beginning of EUL call when logging is enabled. 
11/24/10   oh      To indicate start of DTX recfg, SW does async write to FW at act. time. 
11/19/10   oh      CPC DTX STOP to be called everytime when an IDLE Request is rcvd. by WL1. 
11/11/10   oh      Ensure 3 CFN diff b/t config CFN and action CFN for a DTX recfg. 
11/02/10   ksr     Moving LOG_STATUS into periodic framework
10/26/10   gv/gnk  Validated DTX fixes and code review comments
09/02/10   gv      Code Checkin for CPC DTX
07/26/10   gv      Aligning the TTI Change in SW with FW. Basically changing
                   to newer TTI only when FW stops the old config.
07/16/10   oh      Replacing INTLOCK by local mutex. 
11/13/09   sa      Release 8 min Beta_ed support: Added an element in EUL signaled
                   config DB to hold beta_ed,k,reduced,min configured by RRC
09/02/09   sa      EDPCCH power boost feature support: Added a structure in EUL
                   config database to hold E-DPCCH boost information
05/22/09   oh      Support for Rel-7 EDPDCH power interpolation feature.
10/03/08   hk      Bringing in 7k Mailine fixes
05/17/07   oh      Support for switching to built SS tables for any UL DPCH reconfig
03/02/07   am      Added support for log on demand for 4303 logpkt
02/21/07   am      Added code for applying SG at activation time
11/15/06   yiz     Add e_ul_cfg_reuse_all_ss_tables()
09/15/06   am      Added code for logging and review comments
08/14/06   yiz     Mainline HSUPA code
03/21/06   am      removed HS info from SS table dependent structure.
02/28/06   am      Initial version

===========================================================================*/

/* <EJECT> */

/*===========================================================================

                      Define/undefine local features   

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"
#include "l1macdata.h"
#include "l1rrcif.h"
#include "eul.h"
#include "eullog.h"

#include "wfw_mod_intf.h"
#include "npa.h"


/*===========================================================================

                     DATA TYPE DEFINITIONS

===========================================================================*/
#ifdef FEATURE_WCDMA_HS_RACH
/* Define for default start and final sub fn for EUL/DL channels for config
** when the channels are added using JUDC*/
#define HSRACH_EUL_START_SUB_FN_CHANNEL_ADD_DFLT_VAL_2MS 1
#define HSRACH_EUL_FINAL_SUB_FN_CHANNEL_ADD_DFLT_VAL_2MS 0

#define HSRACH_EUL_START_SUB_FN_CHANNEL_ADD_DFLT_VAL_10MS 5
#define HSRACH_EUL_FINAL_SUB_FN_CHANNEL_ADD_DFLT_VAL_10MS 0
		
#endif /*FEATURE_WCDMA_HS_RACH*/

/* if IR combining is to be  used. i.e.Indicates whether L1 should use RSN
   based RV index To indicate the redundancy version (RV) of each HARQ 
   transmission and to assist the Node B soft buffer management a two bit
   retransmission sequence number (RSN) is signalled from the UE to the 
   Node B spec 25.212 v6.70 - section 4.9.2.2
*/
#define   E_UL_IR_COMBINING                       1

/* if chase  combining is to be  used.i.e.if RV0 is signalled by higher 
   layers
*/
#define   E_UL_IR_CHASE_COMBINING                 0

/* PL_max for 2xSF2+2xSF4, 25.212-670  4.8.4.1 */
#define   E_UL_PL_MAX_FOUR_CODE                   33

/* PL_max for max code less than 2xSF2+2xSF4 */
#define   E_UL_PL_MAX_NON_FOUR_CODE               44

/*MAX standardised E-TFCI TB size table index-25.331-680. 10.3.6.99*/
#define   E_UL_MAX_E_TFCI_TABLE_INDEX             1

/*MAX ref ETFCI value-25.331-680. 10.3.6.99*/
#define   E_UL_MAX_REF_E_TFCI_VALUE               127

/*number of subframes to DTX in EUL FIQ before EDPCH start subfn*/
#define   E_UL_MAX_NUM_DTX_SUBFR_BF_2MS_EDPCH_START_SUBFR    3

  #define E_UL_GET_FINAL_SUB_FN_TTI_RECFG() e_ul_sw_config_final_subfn_during_tti_reconfig
  #define E_UL_SET_FINAL_SUB_FN_TTI_RECFG(val) e_ul_sw_config_final_subfn_during_tti_reconfig = val


#ifdef FEATURE_WCDMA_HS_RACH
/* A MACRO that is used to set the EUL global variable that holds the CFN value*/
#define EUL_SET_EUL_SW_CURR_CFN(val)  \
           e_ul_curr_cfn_val = val

#define EUL_2MS_SUB_FR0_END_BPG  30

#define EUL_2MS_SUB_FR1_END_BPG 60

#define EUL_2MS_SUB_FR2_END_BPG 90

#define EUL_2MS_SUB_FR3_END_BPG 120

#define EUL_2MS_SUB_FR4_END_BPG 150
#endif /*FEATURE_WCDMA_HS_RACH*/

/* Invalid page index. */
#define  EUL_INVALID_PAGE  0xFF


/*This macro checks if the UE CAT returned by RRC is valid*/
#define EULEDPCH_IS_HSUPA_CATEGORY_VALID(ue_cat) (((ue_cat >=1) && (ue_cat <=9)) ? TRUE : FALSE)

#define EUL_MAX_NUM_UE_CAT 9


// FIXME
#define FIQIRQFREE() WCDMA_INTFREE()
#define FIQIRQLOCK() WCDMA_INTLOCK()
#define FIQIRQLOCK_SAV(sav_var) WCDMA_INTLOCK_SAV(sav_var)
#define FIQIRQFREE_SAV(sav_var) WCDMA_INTFREE_SAV(sav_var)

#ifdef FEATURE_WCDMA_CPC_DTX
#define EUL_RAND_ARRAY_MAX_SIZE 100

/* Generate a random number between low_value and hi_value*/

#define DTX_INVALID_CFN 0xFFFF

#define DTX_INVALID_CFG_DB_PAGE 0xFF

#define DTX_BURST_POS_ARR_SIZE 80
#define DTX_ETFCI_SELECT_ARR_SIZE 40
#define DTX_BURST_ETFCI_SELECT_TAB_SIZE 2
/* Whether ETFCI selection is allowed for a subfn in MAC DTX cycle is stored in an array. Each 32 bit word of the array   
** has info for 32 subfns. Hence we shift by 5 to index in to the array*/
#define BIT_SHIFT_ETFCI_SELECT_MAC_DTX 5
/*If ETFCI selection is allowed for a subfn, store 1 in the array*/
#define BITMASK_ETFCI_SELECT_ALLOWED 1
/* When DPCCH burst starts from a subfn, that info is stored in the array. Each 32 bit word of the array   
** has info for 16 subfns. Hence we shift by 4 to index into the array */
#define BIT_SHIFT_DPCCH_BURST_SUBFN 4
/*If DPCCH burst starts from a subfn*/
#define BITMASK_DPCCH_BURST_START_SUBFN 2
/*If subfn lies in DPCCH burst*/
#define BITMASK_SUBFN_LIES_DPCCH_BURST 1
#define BITMASK_GET_DPCCH_BURST_SUBFN 3

/*Macro definition for 0x422c logging purpose  */
#define E_UL_LOG_DOUBLE_BUF            2
#define E_UL_LOG_MAX_SFN_INDEX         5
#define E_UL_LOG_INVALID_DEFINE    0x3FF

/* Database size for L1 DTX DB */
#define  EUL_SIG_DTX_CFG_DB_COUNT 2

#define EUL_INVALID_FIQ_OFFSET     WFW_TX_EUL_FIQ_DISABLE
#define EUL_PRE_FIQ_INVALID_OFFSET WFW_TX_EUL_FIQ_DISABLE 

/* Special value to signal that the activation time is now and valid*/
  #define EUL_CPC_DTX_ACT_TIME_NOW 0xFFEE

#define EUL_IS_CPC_DTX_ACTIVE(carr_idx)  (e_ul_cpc_dtx_state[carr_idx] != E_UL_CPC_DTX_STATE_INIT)

#define EUL_IS_CPC_DTX_CYCLE1_OR_CYCLE2(carr_idx)  ( (e_ul_cpc_dtx_state[carr_idx] == E_UL_CPC_DTX_STATE_ACTIVATED_CYCLE1) || (e_ul_cpc_dtx_state[carr_idx] == E_UL_CPC_DTX_STATE_ACTIVATED_CYCLE2))

#define EUL_CPC_DTX_SET_RECONFIG_ACTIVE_CFN(cfn) (e_ul_cpc_dtx_state_change_to_reconfig_cfn = cfn)

#define EUL_CPC_DTX_GET_RECONFIG_ACTIVE_CFN() e_ul_cpc_dtx_state_change_to_reconfig_cfn 

#define EUL_CPC_DTX_RECONFIG_SET_SW_FW_INTF_VAR(val) (e_ul_cpc_dtx_reconfig_set_sw_fw_intf_var = val)

#define EUL_CPC_DTX_RECONFIG_GET_SW_FW_INTF_VAR() e_ul_cpc_dtx_reconfig_set_sw_fw_intf_var

#define EUL_CPC_DTX_PRI_CARR_IN_CONFIG_RECONFIG() (e_ul_cpc_dtx_ord_cfg_db_indx_valid == TRUE)

#define EUL_CPC_DTX_PRI_CARR_IN_ENABLING_DELAY_PERIOD() (e_ul_cpc_dtx_cfg_db_info[e_ul_cpc_dtx_ord_cfg_db_indx].start_cfn != DTX_INVALID_CFN)
#endif /* FEATURE_WCDMA_CPC_DTX */

#ifdef FEATURE_WCDMA_DC_HSUPA 
#define IS_SEC_EUL_CFGD() ((e_ul_sec_edpch_state == E_UL_SEC_EDPCH_STATE_CFGD) || (e_ul_sec_edpch_state == E_UL_SEC_EDPCH_STATE_TX))
#endif /*FEATURE_WCDMA_DC_HSUPA */

#ifdef FEATURE_WCDMA_16_QAM
extern boolean wl1_e_ul_16qam_mode_on;

#define WL1_E_UL_16QAM_GET_STATE()  (wl1_e_ul_16qam_mode_on)
#define WL1_E_UL_16QAM_SET_STATE(x) (wl1_e_ul_16qam_mode_on = x)
#define IS_WL1_E_UL_16QAM_MODE_ON() (WL1_E_UL_16QAM_GET_STATE() == TRUE)

/* Check if given SF is 16QAM. */
#define IS_SF4X2_SF2X2_16QAM_PRESENT(sf) \
                                   (sf == L1_E_MAX_CH_CODES_SF4X2_SF2X2_16QAM)

/* AG table index configured by NW for 16QAM.
** AG_TAB_IDX = 0,   when 16QAM not configured, 
** AG_TAB_IDX = 0/1, when 16QAM configured. 
*/
#define E_UL_GET_AG_TAB_IDX() (e_ul_curr_phychan_cfg_db_ptr->ag_tab_idx)

/* SG table index configured by NW for 16QAM.
** SG_TAB_IDX = 0, when 16QAM not configured, 
** SG_TAB_IDX = 1, when 16QAM configured. 
*/
#define E_UL_GET_SG_TAB_IDX(x) ((x == 0) ? e_ul_phychan_cfg_db_info[e_ul_ord_phychan_cfg_db_idx].sg_tab_idx: \
                                 e_ul_curr_phychan_cfg_db_ptr->sg_tab_idx)

/* Under the following configuration conditions, UE L1 shall use the lowest configured 
** SG value in SG Table 2 if the AG value does not map to an entry in SG Table 2. 
**  - AG Table is configured to be Table 16B (AG Table 1): Mapping of AG Value, and
**  - SG Table is configured to be Table 9.2.5.2.1.2 (SG Table 2), and
**  - Network sends AG value 2 [mapped to (7/15)^2].
** For the AG values above 2, when AG Table 1 and SG Table 2 are configured,
** the value maps to "idx - 5" in SG Table 2. Refer to "ag_table[0][]"
*/
#define WL1_E_UL_16QAM_MODIFY_AG_IDX(idx, cidx)       \
        if ((E_UL_GET_AG_TAB_IDX()  == 0) &&          \
            (E_UL_GET_SG_TAB_IDX(1) == 1))            \
        {                                             \
          if (applied_dl_results[cidx].ag_info.ag_val == 2) \
          {                                           \
            idx = 0;                                  \
          }                                           \
          if (applied_dl_results[cidx].ag_info.ag_val > 2)  \
          {                                           \
            idx -= 5;                                 \
          }                                           \
          if (DEBUG_EUL_DL_LOGGING & DEBUG_EUL_DL_LOGGING_SG)           \
          {                                                             \
            MSG_HIGH("16QAM_CFG: SG Idx modified: AGval=%d, newIdx=%d", \
                     applied_dl_results[cidx].ag_info.ag_val, idx, 0);  \
          }                                                             \
        }
#else
#define E_UL_GET_AG_TAB_IDX()  (0)
#define E_UL_GET_SG_TAB_IDX(x) (0)
#define IS_SF4X2_SF2X2_16QAM_PRESENT(sf) (0)
#endif /* FEATURE_WCDMA_16_QAM */

/* Check if given SF has "SF4X2_SF2X2" in it. */
#define IS_SF4X2_SF2X2_PRESENT(sf) ((IS_SF4X2_SF2X2_16QAM_PRESENT(sf))  || \
                                    (sf == L1_E_MAX_CH_CODES_SF4X2_SF2X2))
/* Check if given SF has "SF2X2" in it. */
#define IS_SF2X2_PRESENT(sf)       ((IS_SF4X2_SF2X2_PRESENT(sf))  || \
                                    (sf == L1_E_MAX_CH_CODES_SF2X2))
/* Check if given SF has "SF4X2" in it. */
#define IS_SF4X2_PRESENT(sf)       ((IS_SF4X2_SF2X2_PRESENT(sf))  || \
                                    (sf == L1_E_MAX_CH_CODES_SF4X2))
/* If SF is '16QAM' then use index 1 for BETA tables, else 0. */
#define WL1_E_UL_GET_E_TAB_IDX(sf) (IS_SF4X2_SF2X2_16QAM_PRESENT(sf) ? 1 : 0)

/* Data structure which holds one boolean variable for each log packet
 * In the begining of WL1 task, during every Enable/Disable of PhyCh
 * regular intervals - LOG Status is checked and updated into this variable
 * In all other instances, the following vairable through GET_LOG_STATUS
 * is used to check the LOG Packet status
 */

typedef struct
{
  boolean eul_combined_l1_mac;           //EUL_COMBINED_L1_MAC_LOG_PKT
  boolean eul_l1_mac_stats;              //EUL_L1_MAC_STATS_LOG_PKT
  boolean eul_ul_rate_matching_info;     //EUL_UL_RATE_MATCHING_INFO_LOG_PKT
  boolean eul_ul_etfc_restrict_info;     //EUL_UL_ETFC_RESTRICT_INFO_LOG_PKT
  boolean eul_irq_flow_ctrl_info;        //EUL_IRQ_FLOW_CTRL_INFO_LOG_PKT
  boolean eul_ul_chan_cfg_info;          //EUL_UL_CHAN_CFG_INFO_LOG_PKT
  boolean eul_r_combo_table_info;        //EUL_R_COMBO_TABLE_INFO_LOG_PKT
 
  boolean eul_ul_edpch_info;             //EUL_UL_EDPCH_INFO_LOG_PKT

  
  /* EDL Log packets */
  boolean eul_dl_decode_info;            //WCDMA_EUL_DL_DECODE_INFO_LOG_PACKET
  boolean eul_dl_chan_info;              //WCDMA_EUL_DL_CHAN_INFO_LOG_PACKET
  boolean eul_dl_hich_results;           //WCDMA_EUL_DL_HICH_RESULTS_INFO_LOG_PACKET
  boolean eul_dl_agch_results;           //WCDMA_EUL_DL_AGCH_RESULTS_INFO_LOG_PACKET
  boolean eul_dl_rgch_results;           //WCDMA_EUL_DL_RGCH_RESULTS_INFO_LOG_PACKET
  
  #ifdef FEATURE_WCDMA_CPC_DTX

  /* CPC Log packets */
  boolean cpc_dtx_fw_state_machine;       //CPC_DTX_FW_STATE_MACHINE_LOG_PKT
  boolean eul_dtx_config_reconfig;        //EUL_DTX_CONFIG_RECONFIG_LOG_PKT
  
  #endif /* #ifdef FEATURE_WCDMA_CPC_DTX */
  
  #ifdef FEATURE_WCDMA_DC_HSUPA 
  boolean eul_multi_carr_new_mpr;        //WCDMA_MULTI_CARR_EUL_POWER_ALLOC_LOG_PACKET
  #endif

  #ifdef FEATURE_WCDMA_DC_HSUPA 
  boolean eul_multi_carr_pow_alloc;        //WCDMA_MULTI_CARR_EUL_POWER_ALLOC_LOG_PACKET
  #endif
} eul_log_status_type;

extern eul_log_status_type    eul_log_status_info;

#define UL_GET_EUL_COMBINED_L1_MAC_LOG_STATUS()                (eul_log_status_info.eul_combined_l1_mac)
#define UL_SET_EUL_COMBINED_L1_MAC_LOG_STATUS(value)           (eul_log_status_info.eul_combined_l1_mac = value)

#define UL_GET_EUL_L1_MAC_STATS_LOG_STATUS()                   (eul_log_status_info.eul_l1_mac_stats)
#define UL_SET_EUL_L1_MAC_STATS_LOG_STATUS(value)              (eul_log_status_info.eul_l1_mac_stats = value)

#define UL_GET_EUL_UL_RATE_MATCHING_INFO_LOG_STATUS()          (eul_log_status_info.eul_ul_rate_matching_info)
#define UL_SET_EUL_UL_RATE_MATCHING_INFO_LOG_STATUS(value)     (eul_log_status_info.eul_ul_rate_matching_info = value)

#define UL_GET_EUL_UL_ETFC_RESTRICT_INFO_LOG_STATUS()          (eul_log_status_info.eul_ul_etfc_restrict_info)
#define UL_SET_EUL_UL_ETFC_RESTRICT_INFO_LOG_STATUS(value)     (eul_log_status_info.eul_ul_etfc_restrict_info = value)

#define UL_GET_EUL_IRQ_FLOW_CTRL_INFO_LOG_STATUS()             (eul_log_status_info.eul_irq_flow_ctrl_info)
#define UL_SET_EUL_IRQ_FLOW_CTRL_INFO_LOG_STATUS(value)        (eul_log_status_info.eul_irq_flow_ctrl_info = value)

#define UL_GET_EUL_UL_CHAN_CFG_INFO_LOG_STATUS()               (eul_log_status_info.eul_ul_chan_cfg_info)
#define UL_SET_EUL_UL_CHAN_CFG_INFO_LOG_STATUS(value)          (eul_log_status_info.eul_ul_chan_cfg_info = value)

#define UL_GET_EUL_R_COMBO_TABLE_INFO_LOG_STATUS()             (eul_log_status_info.eul_r_combo_table_info)
#define UL_SET_EUL_R_COMBO_TABLE_INFO_LOG_STATUS(value)        (eul_log_status_info.eul_r_combo_table_info = value)

#define UL_GET_EUL_UL_EDPCH_INFO_LOG_STATUS()                  (eul_log_status_info.eul_ul_edpch_info)
#define UL_SET_EUL_UL_EDPCH_INFO_LOG_STATUS(value)             (eul_log_status_info.eul_ul_edpch_info = value)

#define UL_GET_WCDMA_EUL_DL_DECODE_INFO_LOG_STATUS()            (eul_log_status_info.eul_dl_decode_info)
#define UL_SET_WCDMA_EUL_DL_DECODE_INFO_LOG_STATUS(value)       (eul_log_status_info.eul_dl_decode_info = value)

#define UL_GET_WCDMA_EUL_DL_CHAN_INFO_LOG_STATUS()              (eul_log_status_info.eul_dl_chan_info)
#define UL_SET_WCDMA_EUL_DL_CHAN_INFO_LOG_STATUS(value)         (eul_log_status_info.eul_dl_chan_info = value)

#define UL_GET_WCDMA_EUL_DL_AGCH_RESULTS_INFO_LOG_STATUS()      (eul_log_status_info.eul_dl_agch_results)
#define UL_SET_WCDMA_EUL_DL_AGCH_RESULTS_INFO_LOG_STATUS(value) (eul_log_status_info.eul_dl_agch_results = value)

#define UL_GET_WCDMA_EUL_DL_HICH_RESULTS_INFO_LOG_STATUS()      (eul_log_status_info.eul_dl_hich_results)
#define UL_SET_WCDMA_EUL_DL_HICH_RESULTS_INFO_LOG_STATUS(value) (eul_log_status_info.eul_dl_hich_results = value)

#define UL_GET_WCDMA_EUL_DL_RGCH_RESULTS_INFO_LOG_STATUS()      (eul_log_status_info.eul_dl_rgch_results)
#define UL_SET_WCDMA_EUL_DL_RGCH_RESULTS_INFO_LOG_STATUS(value) (eul_log_status_info.eul_dl_rgch_results = value)

#ifdef FEATURE_WCDMA_CPC_DTX

#define UL_GET_CPC_DTX_FW_STATE_MACHINE_LOG_STATUS()            (eul_log_status_info.cpc_dtx_fw_state_machine)
#define UL_SET_CPC_DTX_FW_STATE_MACHINE_LOG_STATUS(value)       (eul_log_status_info.cpc_dtx_fw_state_machine = value)

#define UL_GET_EUL_DTX_CONFIG_RECONFIG_LOG_STATUS()             (eul_log_status_info.eul_dtx_config_reconfig)
#define UL_SET_EUL_DTX_CONFIG_RECONFIG_LOG_STATUS(value)        (eul_log_status_info.eul_dtx_config_reconfig = value)

#endif /* #ifdef FEATURE_WCDMA_CPC_DTX */

#ifdef FEATURE_WCDMA_DC_HSUPA 
#define UL_GET_EUL_MULTI_CARR_NEW_MPR_LOG_STATUS()              (eul_log_status_info.eul_multi_carr_new_mpr)
#define UL_SET_EUL_MULTI_CARR_NEW_MPR_LOG_STATUS(value)         (eul_log_status_info.eul_multi_carr_new_mpr = value)
#endif

#ifdef FEATURE_WCDMA_DC_HSUPA 
#define UL_GET_EUL_MULTI_CARR_POW_ALLOC_LOG_STATUS()            (eul_log_status_info.eul_multi_carr_pow_alloc)
#define UL_SET_EUL_MULTI_CARR_POW_ALLOC_LOG_STATUS(value)       (eul_log_status_info.eul_multi_carr_pow_alloc = value)
#endif

/*EUL channel action types.These actions are used in mDSP config cmds*/
typedef enum 
{
  /*drop the channel*/
  E_UL_CHAN_DISABLE,
  
  /* add the channel*/
  E_UL_CHAN_ENABLE,
  
  /* reconfig the channel*/
  E_UL_CHAN_RECONFIG
  
} e_ul_chan_action_enum_type;

typedef enum 
{
  /*drop the channel*/
  E_UL_SEC_CHAN_DISABLE,
  
  /* add the channel*/
  E_UL_SEC_CHAN_ENABLE,

  /* reconfig the channel*/
  E_UL_SEC_CHAN_RECONFIG
  
} e_ul_sec_chan_action_enum_type;

/*edpch config db - this contains the edpch channel configuration information
  as per the mDSP interface- FW HLD-2.1.1.4
*/
typedef struct
{
  /*This bit indicates whether IR combining or chase combing is active*/
  boolean                       ir;

  /* TTI of E-DCH */
  e_tti_enum_type               tti;

  /* Action of the E-DPCH Config command*/
  e_ul_chan_action_enum_type    action;

  /*The subframe # where  the EDPCH channel is active with the new 
    configuration. the starting subframe number for 10ms tti is CFN*5+4. 
    The subframe number ranges from 0-1279.The start frame number 
    shall be sent to mDSP in case of start or reconfig
  */
  uint16                        start_sub_fn;
  
  /*the last subframe# that will transmit data on the EDPCH with current
    configuration.the final subframe number for 10ms tti is CFN*5+4. 
    The subframe number ranges from 0-1279.The final frame number 
    shall be sent to mDSP in case of reconfig or stop
  */
  uint16                        final_sub_fn;

  /*Set to TRUE for EUL primary enable/reconfig/disable*/
  boolean                       dirty_bit;

  /* For the sync interface action for Secondary EUL can only be 
  ** enable/disable*/
  e_ul_chan_action_enum_type    action_sec;

  /* The subframe # where  the Sec EDPCH channel is cfgd with the new 
  ** configuration*/
  uint16                        start_sub_fn_sec;

  /* The subframe # where  the Sec EDPCH channel will be disabled */
  uint16                        final_sub_fn_sec;
   
  /*Set to TRUE for EUL secondary config/deconfig*/
  boolean                       dirty_bit_sec;

} e_ul_mdsp_cfg_db_struct_type;

/*The DPCH parameters that effect the building of semi static tables for EDPCH.
  This information is maintained even if E-DCH is not active.These parameters
  are received from uplink DCH module
*/
typedef struct
{
  /* TRUE if UL DPCH TFCS has non-zero data rate. Must be determined by DPCH 
     configuration module even if there is no E-DCH  change 
  */
  boolean                       dpdch_cfged;

  /* DPCCH slot format ( 0, 1, 2, 3 ONLY ) in normal mode as in 25.211-660
     Table 2.Must be determined by DPCH configuration module even if there 
     is no E-DCH  change 
  */
  uint32                        ul_dpcch_slot_fmt_nm;

} e_ul_dpch_cfg_db_struct_type;


/*Struct type for E-DCH config parameters that are received from RRC as part
  of cphy setup req 
*/
typedef struct
{
  /* TTI of E-DCH : 10ms or 2 ms*/
  e_tti_enum_type               tti;

  /* E-DPDCH Puncturing Limit non-max:RRC will convert the received value 
     (11..25)  from NW using calculation (IE value * 4), so range of 
     e_dpdch_pl_non_max is (44..100)  see spec 25.212 - v6.70 section 
     4.8.4.1 for usage
  */
  uint32                        pl_non_max; 

  /* MAX channelization code, lower rate of (UTRAN, UE_capability) */
  l1_e_max_ch_codes_enum_type   max_ch_codes;

  /* 25.331, 10.3.5.7d: If "rv0" is indicated, the UE shall only use 
     E_DCH RV index 0.Default value is "rvtable", in which case the UE 
     shall use an RSN based RV index. 
  */
  l1_harq_rv_config_enum_type   harq_rv_config;

  /* Number of signaled ref E-TFCI from UTRAN */
  uint32                        num_ref_e_tfci;

  /* E-TFCI table index: Indicates which standardised E-TFCI TB size table
     shall be used for a given e_tti : 0 or 1
  */
  uint32                        e_tfci_table_idx; 

  /* E-DPCCH/DPCCH power offset -Delta_E-DPCCH in 25.213 Table 1B, 0..8 */
  uint32                        edpcch_pwr_offset;
    
  /* Ascending sorted reference E-TFCIs */
  l1_ref_e_tfci_struct_type     sorted_ref_e_tfci_list[L1_MAX_REF_E_TFCI_LIST];

  /*EDPDCH power interpolation flag*/
  #ifdef FEATURE_WCDMA_REL7
  boolean edpdch_pwr_interpolation; 
  #endif /*FEATURE_WCDMA_REL7*/
  
  #ifdef FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST
  /* Structure to hold E-DPCCH boost information. */
  l1_e_tfc_boost_info_struct_type e_tfc_boost_info;
  #endif /*FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST*/

  #ifdef FEATURE_WCDMA_REL8
  /* Enum for beta_ed,k,reduced,min */
  l1_e_min_bed_enum_type min_beta_ed;
  #endif /*FEATURE_WCDMA_REL8*/

  /* When 16QAM is configured by RRC ag_tab_idx can be 0 or 1.
  ** If 16QAM is not configured then ag_tab_idx only be 0. 
  ** sg_tab_idx is always 1 when 16QAM is configured, else 0. */
  #ifdef FEATURE_WCDMA_16_QAM
  uint8 ag_tab_idx;
  uint8 sg_tab_idx;
  #endif /* FEATURE_WCDMA_16_QAM */
} e_ul_sig_cfg_db_struct_type;

typedef struct
{
 /* Enum for beta_ed,k,reduced,min */
  l1_e_min_bed_enum_type min_beta_ed_sec;
} e_ul_sec_sig_cfg_db_struct_type;

#if defined(FEATURE_WCDMA_PLT) || defined(FEATURE_WCDMA_PLT_MODE)
#error code not present
#endif /*FEATURE_WCDMA_PLT*/


#ifdef FEATURE_WCDMA_CPC_DTX


/* CPC DTX states */
typedef enum {
  /* DTX Activated - cycle 1 */
  E_UL_CPC_DTX_STATE_ACTIVATED_CYCLE1,

  /* DTX Activated into cycle 2 */
  E_UL_CPC_DTX_STATE_ACTIVATED_CYCLE2,
   
   /* HS-SCCH order disable */
   E_UL_CPC_DTX_STATE_SCCH_ORDER_DISABLE,
   
   /* Reconfiguration going on */
   E_UL_CPC_DTX_STATE_RECONFIG,
     /* DTX INIT */
  E_UL_CPC_DTX_STATE_INIT
} e_ul_cpc_dtx_state_enum_type;

// Enum type for DTX action type to be passed
// to DIAG in a DTX event (as specified in the ICD)
typedef enum
{
  E_UL_CPC_DTX_DIAG_EVENT_START_ACTION = 0,
  E_UL_CPC_DTX_DIAG_EVENT_STOP_ACTION  = 1,
  E_UL_CPC_DTX_DIAG_EVENT_RECFG_ACTION = 3

} e_ul_cpc_dtx_diag_event_action_enum_type;

/* DTX configuration types */
typedef enum
{

   /* stop DTX */
   E_UL_CPC_DTX_ACTION_STOP = 0,

  /* start DTX */
  E_UL_CPC_DTX_ACTION_START = 1,

  /* Reconfig DTX */
  E_UL_CPC_DTX_ACTION_RECONFIG = 2,

  /* UINIT ... INVALID VALUE*/
  E_UL_CPC_DTX_ACTION_UNINIT = 3

} e_ul_cpc_dtx_action_enum_type;

/* Enum to indicate to FW when DTX should be activated.*/
typedef enum {

  /* This enum is set when after reverting to old config, the DTX is being
  ** started but the HS-SCCH order that should be applied is DTX disable.
  ** when this is set, the MDSP will accept the DTX config, but it will activate
  ** only when it receives a next HS-SCCH command from NW to DTX enable*/
  E_UL_ACTIVATE_DTX_HSSCH_ORD = 0,

  /* This is the normal case where the DTX is being started. This will ensure that
  ** the DTX is activated at the action time configured by L1*/
  E_UL_ACTIVATE_DTX_START_ACT_TIME = 1,

  /* This is used during the reconfiguration. For reconfiguration continuous DPCCH
  ** should be enabled at activation time and continued till the action time programmed
  ** by L1 to start the new DTX configuration. When FW gets a command from L1, it start Txing
  ** DPCCH continuously from the upcoming radio frame boundary after the receipt of the config
  ** command containing the below enum*/
  E_UL_ACTIVATE_DTX_RECFG_ACT_TIME = 2,

  E_UL_ACTIVATE_DTX_NOOP,

} e_ul_cpc_dtx_activate_enum_type;



typedef struct
{
  /* Offset of DRX and DTX cycles.
  ** Uplink DPCCH transmission pattern and HS-SCCH reception pattern offset in subframes.*/
  uint16   ue_dtx_drx_offset;

    /* DTX Mode 1 Cycle length */
  uint16   ue_dtx_cycle_1;

  /* DTX Mode 2 Cycle length */
  uint16   ue_dtx_cycle_2;

  /* Uplink DPCCH burst length in Mode 1 */
  uint16   ue_dpcch_burst_1;

  /* Uplink DPCCH burst length in Mode 2 */
  uint16   ue_dpcch_burst_2;

  /* Preamble length to be used for uplink DPCCH when in cycle 2 */
  uint16   ue_dtx_cycle_2_pream_len;

  /* Consecutive TTIs without EDCH Transmission that are required to
   * start cycle 2 */
  uint16  ue_inact_thresh_dtx_cycle_2;

  /* Number of subframes for which CQI transmission takes
  ** priority over DTX after a successful HSSCCH/HSPDSCH reception */
  uint16  cqi_dtx_timer;

}e_ul_mdsp_cpc_dtx_cfg_db_struct_type;

/* DTX database */
typedef struct {

  /* req mask for DTX-DRX configuration */
  uint8  l1_cpc_dtx_req_mask;

  /* Last DTX action
  ** Records DTX action given by RRC
  ** and doesn't record the ones given by
  ** HS-SCCH orders
  */
  e_ul_cpc_dtx_action_enum_type dtx_action;

  /* Populated in FW pre-callback function.
  ** Stores the CFN at which FW will start DTX
  */
  uint32  start_cfn;
  /* Stores the CFN at which FW will start.
  ** A separate member is required to report the start cfn
  ** to DRX module specifically for TTI reconfig, as we clear
  ** start_cfn when we switch to new config, as we want to avoid
  ** unexpected behaviour when HS-SCCH order is received.
  */
  uint32  start_cfn_for_drx;

  /*populated only when secondary carrier is actived through order 
   ** while Primary is already in DTX, this cfn could be different from 
   ** primary start cfn*/
  uint32  dtx_start_cfn_sec;

  /*Offset of the DTX and DRX cycles at the given TTI. Units of
  *subframes, valid range of values are 0 ...159.
  *A value of 0xFF - means dtx_drx_offset has invalid value i.e. its not init.
  *RRC should not use this value when DTX operation is ON
  */
  uint8 dtx_drx_offset;

  /* activation_time + enabling delay = time when DTX starts
  */
  uint8 enabling_delay;

  /* DTX Cycle 1 length in subframes */
  l1_ue_dtx_cycle1_info_enum_type ue_dtx_cycle1;

  /* DTX Cycle 2 length in subframes */
  l1_ue_dtx_cycle2_info_enum_type ue_dtx_cycle2;

  /* Dpcch burst length 1 in Sub-frames */
  l1_ue_dpcch_burst_1_info_enum_type ue_dpcch_burst_1;

  /* Dpcch burst length 2 in Sub-frames */
  l1_ue_dpcch_burst_2_info_enum_type ue_dpcch_burst_2;

  /* Inactive Cycle 2 in terms of E-DCH TTI*/
  l1_inactivity_threshold_for_ue_dtx_cycle2_info_enum_type tinactive_cycle2;

  /* DTX cycle 2 preamble length */
  l1_dtx_long_preamble_length_info_enum_type dtx_long_preamble_length;

  /* cqi timer */
  l1_cqi_dtx_timer_info_enum_type cqi_dtx_timer;

  /* mac dtx cycle */
  l1_mac_dtx_cycle_info_enum_type            l1_mac_dtx_cycle;

  /* mac inactive length */
  l1_mac_inactivity_threshold_info_enum_type l1_mac_inactivity_threshold;

  /*Serving Grant value to be used at the transition in DTX-Cycle-2.
  *(0..37,38) indicates E-DCH serving grant index ; index 38 means zero grant.
  * a Value of 0xFF(L1_SERVING_GRANT_NOT_PRESENT) - means default sg not present
  */
  uint8 default_sg_in_dtx_cycle2;

} e_ul_cpc_dtx_cfg_db_struct_type;

#ifdef FEATURE_CPC_DTX_HS_SCCH_REVERT_HHO_RECOVERY /* RRC Support*/
/* This structure holds information regarding the RRC order regarding the BACKUP, DISCARD
** and restore of the HS SCCH orders received in L1*/
typedef struct{
  /* Store the action which the RRC is asking to perform L1*/
  l1_dtx_drx_hs_scch_order_info_enum_type rrc_hs_scch_ord_action;
  /* When RRC asks L1 to back up, the order from the current is pushed into this
   ** back up*/
  uint16 backup_hs_scch_order;
  /* Current is always the applied HS SCCH order*/
  uint16 current_hs_scch_order;
  
}e_ul_cpc_dtx_scch_ord_bkup_rstr_struct_type;

extern boolean e_ul_cpc_dtx_wait_for_hs_scch_enable[];
#endif


/* Maintains the CPC DTX state. Changes states based on
** - CPHY setup request from RRC.
** - HS-scch orders from network.
** - in-sync and out-of-sync operations.
*/
extern e_ul_cpc_dtx_state_enum_type  e_ul_cpc_dtx_state[WL1_MAX_UL_CARR_IDX];

/*enum type to represent DPCCH burst related info for a subfn*/
typedef enum {

  /* No burst exists in the subfn */
  E_UL_CPC_DTX_NO_BURST,

  /* This subfn lies within a burst*/
  E_UL_CPC_DTX_BURST_OVERLAP,

  /* Burst starting from this subfn */
  E_UL_CPC_DTX_BURST_START,

} e_ul_cpc_dtx_burst_pos_enum_type;

extern e_ul_cpc_dtx_burst_pos_enum_type e_ul_cpc_dtx_subfn_burst_info; 

/*enum type to represent ETFCI select allowed for MAC DTX Cycle*/
typedef enum {

  /* ETFCI Select not possible in the subfn */
  E_UL_CPC_DTX_NO_ETFCI_SELECT,

  /* ETFCI Select allowed in the subfn */
  E_UL_CPC_DTX_ETFCI_SELECT_ALLOW,

} e_ul_cpc_dtx_etfci_select_enum_type;

extern e_ul_cpc_dtx_etfci_select_enum_type e_ul_cpc_dtx_etfci_select_info; 

#ifdef FEATURE_WCDMA_16_QAM
extern e_ul_sig_cfg_db_struct_type e_ul_phychan_cfg_db_info[];
#endif /* FEATURE_WCDMA_16_QAM */


/* This structure defines the arrays that will be used to store information
** needed for DPCCH burst start position and which subfn lies in DPCCH burst, 
** both for cycle1 and cycle2*/ 

typedef struct
{
  /*This array stores the pre-computed values*/
  uint32 e_ul_cpc_dtx_burst_pos[DTX_BURST_ETFCI_SELECT_TAB_SIZE][DTX_BURST_POS_ARR_SIZE];

  /* Store the current page index -- which is being used*/
  uint8 curr_page_idx;

  /* Stores the ord page index, which will be configured*/
  uint8 ord_page_idx;

}e_ul_cpc_dtx_burst_pos_info_struct_type;

/* Table store instances on a per sub frame number basis for ETFCI selection subfn*/

typedef struct
{
  /*This array stores the pre-computed values*/
  uint32 e_ul_cpc_dtx_etfci_select_pos[DTX_BURST_ETFCI_SELECT_TAB_SIZE][DTX_ETFCI_SELECT_ARR_SIZE];

  /* Store the current page index -- which is being used*/
  uint8 curr_page_idx;

  /* Stores the ord page index, which will be configured*/
  uint8 ord_page_idx;
	
}e_ul_cpc_dtx_etfci_sel_info_struct_type;

/* CPC ENUM TYPE that stores the information regarding the CPC DTX CFG command
** sent to FW or if it is being HELD for sync procedure to get done. */

typedef enum
{
  E_UL_CPC_DTX_CFG_CMD_STATE_INVALID,
  E_UL_CPC_DTX_CFG_CMD_START_PENDING_SYNC_PROC,
  E_UL_CPC_DTX_CFG_CMD_RECFG_PENDING_SYNC_PROC,
  /* Represent that START/RECFG command is queued for FW*/
  E_UL_CPC_DTX_START_RECFG_CMD_QUEUED,
  E_UL_CPC_DTX_CFG_CMD_STATE_SENT_POST_SYNC_PROC
}e_ul_cpc_dtx_cfg_cmd_state_enum_type;

extern e_ul_cpc_dtx_cfg_cmd_state_enum_type e_ul_cpc_dtx_cfg_cmd_state;

/*This structure stores the information regarding the start of DPCCH burst and also the overlap
** for cycle 1. There is an internal double buffered array. The instances are stored based on 
** sub FN*/
extern e_ul_cpc_dtx_burst_pos_info_struct_type e_ul_cpc_dtx_burst_cycle1_info;

/* Same as above description but for cycle 2*/
extern e_ul_cpc_dtx_burst_pos_info_struct_type e_ul_cpc_dtx_burst_cycle2_info;

/* Same as the above description but this one stores the ETFCI selection possible sub FNs based
** on MAC DTX cycle configured*/
extern e_ul_cpc_dtx_etfci_sel_info_struct_type e_ul_cpc_dtx_etfci_sel_info;

/* DTX config db*/
extern e_ul_cpc_dtx_cfg_db_struct_type e_ul_cpc_dtx_cfg_db_info[EUL_SIG_DTX_CFG_DB_COUNT];

/* Index to the current DTX config DB */
extern uint32 e_ul_cpc_dtx_curr_cfg_db_indx;
extern boolean e_ul_cpc_dtx_curr_cfg_db_indx_valid;


/* Hssch order sub frame number */
extern uint16 e_ul_cpc_dtx_hsscch_order_subfn;

/* Points to the ordered configuration index */
extern uint32 e_ul_cpc_dtx_ord_cfg_db_indx;
extern boolean e_ul_cpc_dtx_ord_cfg_db_indx_valid;

/* Flag to indicate whether CPC start mdsp sync cmd has been sent*/
/* This will be useful only in the context of DC HSUPA */
extern boolean e_ul_cpc_dtx_mdsp_start_cmd_sent;

/* ul inactivity time for cycle 2 */
extern uint16 e_ul_cpc_dtx_inact_timer_for_cycle_2;

/* MAC inactivity threshold */
extern uint16 e_ul_cpc_dtx_mac_inact_threshold;

/* Pointer to current DTX configuration */
extern e_ul_cpc_dtx_cfg_db_struct_type *e_ul_cpc_dtx_curr_cfg_db_ptr;

/* Variable holding the DPCCH slot format. This is needed for 0x422B logging*/
extern uint8 e_ul_cpc_dtx_copy_ul_dpcch_slt_fmt_for_logging;

/* The global variable below is to store the actication time CFN and this will be used in the EUL ISR to
** change the state of E_UL_CPC_STATE*/
extern uint16 e_ul_cpc_dtx_state_change_to_reconfig_cfn;

/* Variable to track whether the SW FW interface variable to be set*/
extern boolean e_ul_cpc_dtx_reconfig_set_sw_fw_intf_var;

/* Variable to find out whether PA has been turned on 
** before DTX Cfg cmd can be sent */
extern boolean e_ul_cpc_dtx_pa_turned_on_during_cfg;
extern uint8 ul_wait_pcp_cnt;


/* Local Mutex for protection of DTX Config variables used in the EUL ISR as well*/
extern rex_crit_sect_type e_ul_cpc_dtx_cfg_isr_mutex;


/*This Mutext will protect the DTX CFG variables with the R99 UL section*/

#define E_UL_CPC_DTX_CFG_ISR_INTLOCK() REX_ISR_LOCK(&e_ul_cpc_dtx_cfg_isr_mutex)
#define E_UL_CPC_DTX_CFG_ISR_INTFREE() REX_ISR_UNLOCK(&e_ul_cpc_dtx_cfg_isr_mutex)


#define E_UL_CPC_DTX_ACTION_CFN_CURR_CFN_MIN_DIFF 3

extern uint8 e_req_mask_backup_for_dtx_stop;

#ifdef FEATURE_CPC_DTX_HS_SCCH_REVERT_HHO_RECOVERY /* RRC Support Needed*/
extern e_ul_cpc_dtx_scch_ord_bkup_rstr_struct_type e_ul_cpc_dtx_scch_ord_bkup_rstr_info;
#endif

#ifdef FEATURE_WCDMA_CPC_DRX
extern rex_crit_sect_type e_dl_cpc_drx_state_mutex;
#define E_DL_CPC_DRX_STATE_INTLOCK() REX_ISR_LOCK(&e_dl_cpc_drx_state_mutex)
#define E_DL_CPC_DRX_STATE_INTFREE() REX_ISR_UNLOCK(&e_dl_cpc_drx_state_mutex)
#endif
#endif /* FEATURE_WCDMA_CPC_DTX */

#ifdef FEATURE_WCDMA_DC_HSUPA 
extern uint32 debug_eul_dc_hsupa_f3s_enable_bmsk;
#endif /* FEATURE_WCDMA_DC_HSUPA  */

#define EUL_DYNAMIC_MEM_ALLOC_AND_CHK(var, type, size) \
        do {                                                                      \
          var = (type *) wl1_mem_alloc(sizeof(type) * size);                      \
          if(var == NULL)                                                         \
          {                                                                       \
            ERR_FATAL("EUL_ERROR: Mem Alloc failed for EUL CFG params", 0, 0, 0); \
          }                                                                       \
        } while (0)

#define EUL_DYNAMIC_MEM_DEALLOC_AND_CHK(var) \
        do {                   \
          if(var != NULL)      \
          {                    \
            wl1_mem_free(var); \
          }                    \
          var = NULL;          \
        } while (0)

/* This structure contains the pointers to current and ordered config databases
   maintained for EDPCH signalled info , DPCH and HSDPA parameters that effect 
   semistatic tables of EDPCH.if there is no change for the hsdpa params then 
   the  new_hs_db_ptr is null . if there is no change for the dpch params then
   the new_dpch_db_ptr is null .

   if the new pointer of a channel is NULL then that channel information is not
   used in the tables building

   Initially all the current and new pointers are null & all the double buffers
   are filled with zeros
   
   When DPCH is started/recfged one of the DPCH double buffer is filled with the
   signaled params and the new dpch pointer points to the filled buffer 
   
   When EUL is started/recfged, one of the double buffer is filled with the 
   signaled params and the new eul pointer points to the filled buffer
   
   The above operations are done if the validation of the corresponding channel
   is successful.
   
   After the semi static tables are built,inside euledpch_process_cfg_data(),
   the current pointers of each channel are changed to point to the corresponding
   buffers(i.e new pointers if new pointer is not NULL) that are used in the 
   calculations & Make the new pointers of all the channels as NULL
*/
typedef struct
{

  /*pointer to the existing EDPCH active database that is already used in building
   the ss tables
  */
  e_ul_sig_cfg_db_struct_type   *curr_db_ptr;
  
  /*pointer to the newly received EDPCH database that contains the signalled EDPCH
   params
  */
  e_ul_sig_cfg_db_struct_type   *new_db_ptr;
  
  /*pointer to the existing DPCH active database that contains the DPCH params that
   are already used in building the EDPCH semistatic tables
  */
  e_ul_dpch_cfg_db_struct_type  *curr_dpch_db_ptr;
  
  /*pointer to the newly received DPCH database that contains the DPCH params that
    effect the EDPCH semistatic tables and these params should be used in next ss
    table build
  */
  e_ul_dpch_cfg_db_struct_type  *new_dpch_db_ptr;

} e_ul_cfg_info_struct_type;


/*EUL EDPCH states. These states are used only when EUL uplink module is
  in E_UL_EDPCH_TX 
*/
typedef enum
{
  /* edpch default state */
  E_UL_EDPCH_INIT,

  /* EDPCH is in set up or DCH is in PCP */
  E_UL_EDPCH_STATE_SETUP,
  
  /* EDPCH is in normal tx state*/
  E_UL_EDPCH_STATE_TX,
  
  /* EDPCH is disabled due to out of sync */
  E_UL_EDPCH_STATE_DISABLED,
  
  /* ERORR state to recover */
  E_UL_EDPCH_STATE_ERROR
  
} e_ul_edpch_state_enum_type;

/* EUL uplink  states maintained in uplink module*/
typedef enum 
{
  /*un-initialized state.This is the default EUL uplink state before 
    the WCDMA is active
  */
  E_UL_UNINIT,
  
  /*initialized state. EUL uplink enters into this state whenever the 
    WCDMA is started and l1m init is called
  */
  E_UL_INIT,
  
  /*EUL EDPCH active state. EUL uplink enters into this state when  EDPCH 
    is requested and leaves this state when EDPCH is dropped
  */
  E_UL_EDPCH_TX,
  
  /*un-recoverable error state. EUL uplink enters into this state when eul
    uplink module encounter some error condition
  */
  E_UL_ERROR
  
} e_ul_state_enum_type;

/*This enum struct is used during recfg to enable the edch tx .the following 
  explains the state transitions for the variable
===================================================================================
 STATE                       euledpch_tx_re_enable()     e_ul_activate_cfg_params()
===================================================================================
    E_UL_CFG_NO_CHANGE        tx enable                         no_action

    E_UL_CFG_CHANGED          E_UL_CFG_WAIT_FOR_ACTIVATION     E_UL_CFG_WAIT_FOR_PA_ON
                              & don't enable edch tx           & don't enable edch tx

    E_UL_CFG_WAIT_FOR_ACTIVATION   no_action                   E_UL_CFG_NO_CHANGE
                                                               &enable edch tx
                                                                                                           
    E_UL_CFG_WAIT_FOR_PA_ON   E_UL_CFG_NO_CHANGE               no_action
                              &enable edch tx
*/
typedef enum 
{
  /*initial state.This is the default state before the eul uplink recfg takes place.
  when there is no eul uplink recfg then the state is no_change*/
  E_UL_CFG_NO_CHANGE,
  /*When eul uplink info changes then variable state change to this state at 
  activation time.*/
  E_UL_CFG_CHANGED,
  /*during recfg when eul uplink changed then the var changes to this state when
  dch tx is enabled and waiting for eul new cfg to be active */
  E_UL_CFG_WAIT_FOR_ACTIVATION,
  /*during recfg when eul uplink info changed then the var changes to this state 
  when eul new cfg is activated and waiting for dch tx to be enabled*/
  E_UL_CFG_WAIT_FOR_PA_ON
}e_ul_edpch_recfg_state_enum_type;

/*EUL Sec EDPCH states. These states are used only when EUL uplink module is
  in E_UL_EDPCH_TX 
*/
typedef enum
{
  /* sec edpch default state */
  E_UL_SEC_EDPCH_INIT,

  /* Sec EDPCH is in set up or DCH is in PCP */
  E_UL_SEC_EDPCH_STATE_SETUP,

  /* EDPCH is in cfgd state but not active yet*/
  E_UL_SEC_EDPCH_STATE_CFGD,

  /* EDPCH is in normal tx state*/
  E_UL_SEC_EDPCH_STATE_TX,
  
  /* EDPCH is in normal tx state*/
  E_UL_SEC_EDPCH_STATE_RECFG, 
  
  /* EDPCH is disabled due to out of sync */
  E_UL_SEC_EDPCH_STATE_DISABLED,
  
  /* ERORR state to recover */
  E_UL_SEC_EDPCH_STATE_ERROR
  
} e_ul_sec_edpch_state_enum_type;

typedef enum 
{
  /*initial state.This is the default state before the eul or sec eul recfg takes place.
  when there is no eul or sec eul recfg then the state is no_change*/
  E_UL_SEC_CFG_NO_CHANGE,
   /*when recfg happens Sec was in Active state*/
  E_UL_SEC_CFG_WHEN_ACTIVE,
  
}e_ul_sec_edpch_recfg_state_enum_type;


/* Per RL configuration information for 4309/4311 log packet*/
typedef struct{
  /* PSC */
  uint16 psc;
  uint8 tpc_idx;
  uint8 rls_idx;
}eul_logging_edl_per_rl_config_info_struct_type;


/* This information does not change on a per TTI basis. So have this
** as a separate structure so that it is easy to fill at one before committing
** the log packet.*/
typedef struct{
  /*Num TPC sets*/
  uint8 num_tpc_sets[WL1_MAX_UL_CARR_IDX];
  /*Num RLS*/
  uint8 num_rls[WL1_MAX_UL_CARR_IDX];
  /* Num Cells*/
  uint8 num_cells[WL1_MAX_UL_CARR_IDX];
  /* Serving Cell index*/
  uint8 serv_cell_idx[WL1_MAX_UL_CARR_IDX];
  /* Per RL config_info like PSC etc*/
  eul_logging_edl_per_rl_config_info_struct_type per_rl_config_info[WL1_MAX_UL_CARR_IDX][EUL_MAX_RL];
}eul_logging_edl_config_info_struct_type;

/** callback functionm type declaration */
#ifdef FEATURE_WCDMA_DC_HSUPA 
typedef void (*eul_pre_cb)(void);
#endif

/* Enumeration for different stages of A2 power request*/
typedef enum
{
  /* Before Power request has been called state is INIT*/
  EULENC_A2_POWER_REQUEST_INIT,

  /* After Power request is called, but before callback has fired, state is WAITING*/
  EULENC_A2_POWER_REQUEST_WAITING,

  /* After callback has fired state is DONE, in this state EUL Tx can proceed */
  EULENC_A2_POWER_REQUEST_DONE,

  /* Invalid state */
  EULENC_A2_POWER_REQUEST_NUM_STATES
} eulenc_a2_power_request_enum_type;

/*===========================================================================

                     PUBLIC GLOBAL DATA DECLARATIONS

===========================================================================*/
/* index of the  ordered configuration in the double buffer of EDPCH config db.
   The ordered configuration becomes active configuration after successful mdsp
   config cmd
*/
#ifdef FEATURE_WCDMA_HS_RACH
extern uint32 e_ul_ord_phychan_cfg_db_idx;
/* index of current active configuration in double buffer of EDPCH config db*/
extern uint32 e_ul_curr_phychan_cfg_db_idx;

#define EUL_GET_CURR_PAGE() e_ul_curr_phychan_cfg_db_idx
#define EUL_GET_ORD_PAGE() e_ul_ord_phychan_cfg_db_idx
#endif /*FEATURE_WCDMA_HS_RACH*/

/* EDPCH Phychan drop pending flag */
extern boolean e_ul_drop_pending ;

/* current EUL internal UE cat */
extern uint8 eul_curr_ue_cat;

/* Initial EUL state is Un-Initialized */
extern e_ul_state_enum_type e_ul_state;

/* EDPCH state maintained during EUL uplink module is EDPCH TX state. */
extern e_ul_edpch_state_enum_type e_ul_edpch_state;

/* pointer to active E-DCH config parameters double buffer and index are define
 in eulcfg.c 
 */
extern e_ul_sig_cfg_db_struct_type *e_ul_curr_phychan_cfg_db_ptr;

/* indicate whether to activate SS tables */
extern boolean eul_activate_ss_tables_pending;

extern e_ul_edpch_recfg_state_enum_type e_ul_edpch_recfg_state;

extern e_ul_sec_edpch_recfg_state_enum_type e_ul_sec_edpch_recfg_state;

/*number of subframes to DTX in EUL FIQ before EDPCH start subfn*/
extern uint8 e_ul_stop_edch_tx_bf_setup;

/* hho_setup_mask copied from RRC config parameters at EUL validation
 * At this point, L1 global HHO flags are not set yet. */
extern uint16 e_ul_hho_req_mask;

/* This global variable takes a snapshot of the final sub frame number configure
** during TTI reconfiguration. This is need to correctly set the flag that TTI reconfig is
** done to FALSE*/
extern uint16 e_ul_sw_config_final_subfn_during_tti_reconfig;

/* Local Mutex for the protection of EUL cfg params in ISR and task context*/
extern rex_crit_sect_type e_ul_cfg_params_mutex;
extern rex_crit_sect_type e_ul_state_params_mutex;
/* Mutex for avoiding race condition between EUL ISR firing time 
 * reconfiguration during TTI reconfig and Event firing time modification 
 * during 2ms TTI EUL.
 * This is only applicable when WL1 SW fired the EUL ISR
 */
extern rex_crit_sect_type e_ul_isr_firing_time_mutex;


#ifdef FEATURE_WCDMA_DC_HSUPA 
/* Local Mutex for the protection of Sec EUL state params in ISR and task context*/
extern rex_crit_sect_type e_ul_sec_state_params_mutex;
extern rex_crit_sect_type e_ul_sec_cfg_params_mutex;
#endif /*FEATURE_WCDMA_DC_HSUPA */

#define EUL_CFG_INTLOCK() REX_ISR_LOCK(&e_ul_cfg_params_mutex)
#define EUL_CFG_INTFREE() REX_ISR_UNLOCK(&e_ul_cfg_params_mutex)
#define EUL_STATE_INTLOCK() REX_ISR_LOCK(&e_ul_state_params_mutex)
#define EUL_STATE_INTFREE() REX_ISR_UNLOCK(&e_ul_state_params_mutex)

#define EUL_ISR_FIRING_TIME_INTLOCK() REX_ISR_LOCK(&e_ul_isr_firing_time_mutex)
#define EUL_ISR_FIRING_TIME_INTFREE() REX_ISR_UNLOCK(&e_ul_isr_firing_time_mutex)

#ifdef FEATURE_WCDMA_DC_HSUPA 
#define EUL_SEC_CFG_INTLOCK() REX_ISR_LOCK(&e_ul_sec_cfg_params_mutex)
#define EUL_SEC_CFG_INTFREE() REX_ISR_UNLOCK(&e_ul_sec_cfg_params_mutex)
#define EUL_SEC_STATE_INTLOCK() REX_ISR_LOCK(&e_ul_sec_state_params_mutex)
#define EUL_SEC_STATE_INTFREE() REX_ISR_UNLOCK(&e_ul_sec_state_params_mutex)
#endif /*FEATURE_WCDMA_DC_HSUPA */

extern WfwTxEULAsyncWriteStruct *wfw_intf_tx_eul_async_write_ptr[WL1_MAX_UL_CARR_IDX];
extern WfwTxEULAsyncReadStruct  *wfw_intf_tx_eul_async_read_ptr[WL1_MAX_UL_CARR_IDX];
extern WfwTxEULCommonAsyncWriteStruct *wfw_intf_tx_eul_common_async_write_ptr;
extern WfwTxEULCommonAsyncReadStruct *wfw_intf_tx_eul_common_async_read_ptr;
extern WfwTxAsyncWriteStruct        *wfw_intf_tx_async_write_ptr;
extern WfwTxAsyncReadStruct *wfw_intf_tx_async_read_ptr;

#ifdef FEATURE_WCDMA_HS_RACH
/* The below global variable holds the EUL module specific CFN value. With these
** changes, the EUL module will not use the seq_get_cfn() to get the CFN value*/
extern uint16 e_ul_curr_cfn_val;
/* The below global variable holds the EUL sub FN value. This is got from the TLM API*/
extern uint16 e_ul_curr_sub_fn_val;
#endif /*FEATURE_WCDMA_HS_RACH*/

/*EUL client to request for high clk speeds*/
extern npa_client_handle eul_npa_client;
extern boolean eul_npa_registered;

/* Global variable to keep track of whether the client for MCPM has 
** been relinquished or not*/
extern boolean e_ul_release_hsupa_client;

extern e_ul_sec_edpch_state_enum_type e_ul_sec_edpch_state;
extern boolean e_ul_sec_edpch_active_pending;
e_ul_sec_sig_cfg_db_struct_type *e_ul_sec_curr_phychan_cfg_db_ptr;

/* Enum type which saves the current stage of A2 power request */
extern eulenc_a2_power_request_enum_type eulenc_a2_power_request_status;
/*===========================================================================

                     FUNCTION DECLARATIONS

===========================================================================*/

 /*===========================================================================
 FUNCTION        E_UL_GET_CFN_VAL
 
 DESCRIPTION     This function returns the CFN value needed by the caller
                 function. The function gets the CFN value based on whether the feature
                 for maintaining the local CFN value is defined or not. 
 
 DEPENDENCIES    None
 
 RETURN VALUE    Either returns a valid eul SW module CFN variable or the 
                 L1 SW global CFN value using seq_get_cfn().
 
 SIDE EFFECTS    None
 ===========================================================================*/
extern uint16 e_ul_get_cfn_val(void);

extern boolean e_ul_validate_edpch_cfg(l1_e_ul_info_struct_type *e_ul_phychan_ptr);

extern void e_ul_edpch_copy_cfg_data ( l1_e_ul_info_struct_type *e_ul_phychan_ptr);

/*=========================================================================

FUNCTION EUL_SET_LOG_STATUS_PERIODIC

DESCRIPTION
  This function checks the LOG_STATUS for various EUL log codes and updates
  the global structure periodically, which can be used during the regular
  UL logics.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void eul_set_log_status_periodic(void);

#ifdef FEATURE_WCDMA_HS_RACH
/*===========================================================================
FUNCTION        E_UL_SET_EUL_SW_LOCAL_CFN

DESCRIPTION     This function sets the local CFN value, which is local to the
                EUL SW module. This function is ideally to be called in the 4th BPG
                of the frame when the TTI is 2ms. The TLM API will update the CFN
                within this function
                seq_get_cfn().

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void e_ul_set_eul_sw_local_cfn(void);
/*===========================================================================
FUNCTION        E_UL_SET_EUL_SW_LOCAL_SUB_FN

DESCRIPTION     This function  sets the EUL sub frame number. This uses the 
                TLM FRC to get the Tx sys time in cx8. THis is converted to BPG and
                then based on the BPG count, the sub frame number is computed.
                This function updates a EUL global variable that holds the current sub frame
                number.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
void e_ul_set_eul_sw_local_sub_fn(void);

#endif /*FEATURE_WCDMA_HS_RACH*/

/*===========================================================================

FUNCTION E_UL_DYNAMIC_MEM_ALLOC

DESCRIPTION
  Allocates memory for all the variables allocated dynamically during EUL CFG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void e_ul_dynamic_mem_alloc(void);

/*===========================================================================

FUNCTION E_UL_DYNAMIC_MEM_DEALLOC

DESCRIPTION
  Deallocates memory for all the variables that are allocated dynamically during EUL CFG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void e_ul_dynamic_mem_dealloc(void);

/*===========================================================================

FUNCTION EUL_DYNAMIC_MEM_ALLOC

DESCRIPTION
  Allocates memory for all the variables used in FIQ

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void eul_dynamic_mem_alloc(void);

/*===========================================================================

FUNCTION EUL_DYNAMIC_MEM_DEALLOC

DESCRIPTION
  Deallocates memory for all the variables used in FIQ

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void eul_dynamic_mem_dealloc(void);

/*===========================================================================
FUNCTION        E_UL_PHYCHAN_CFG_VALIDATE

DESCRIPTION     This function validates that the operation on  EDPCH channel as
                specified by the parameters referenced by the setup command can
                be carried out successfully, i.e. that the parameters are valid.

DEPENDENCIES    None

RETURN VALUE    TRUE if the command can be carried out, FALSE otherwise

SIDE EFFECTS    None
===========================================================================*/
extern boolean e_ul_phychan_cfg_validate(l1_setup_cmd_type *setup);

/*===========================================================================
FUNCTION        E_UL_PROCESS_CPHY_SETUP_REQ

DESCRIPTION     This function process the operation on EDPCH channel as specified
                by the parameters referenced by the setup command.

DEPENDENCIES    None

RETURN VALUE    TRUE if the command is carried out successfully, FALSE otherwise

SIDE EFFECTS    EDPCH shall be configured/reconfigured/stopped
===========================================================================*/
extern boolean e_ul_process_cphy_setup_req( uint8 e_mask, l1_setup_cmd_type *setup_cmd);

extern void e_ul_process_mc_config
(
 l1_setup_cmd_type *setup_cmd,
 wl1_ul_carr_id_enum_type carr_idx
);

/*===========================================================================
FUNCTION        E_UL_SET_UL_DPCH_INFO

DESCRIPTION     This function passes ul_dpcch_slot_fmt_nm and UL DPDCH presence
                info to EUL UL module at config time

DEPENDENCIES    Called by R99 UL DPCH add or reconfig module

RETURN VALUE    None

SIDE EFFECTS    EUL UL config params will update
===========================================================================*/
extern void e_ul_set_ul_dpch_info(uint32 ul_dpcch_slot_fmt_nm,
                                        boolean dpdch_cfged);

/*===========================================================================
FUNCTION        E_UL_BUILD_SS_TABLES

DESCRIPTION     This function passes the ss table depended cfg information
                maintained and after completion of the table building, the 
                function switches the config pointers

DEPENDENCIES    This function is called after successful validation of cfg
                data for DPCH,HS,EUL channel 

RETURN VALUE    None

SIDE EFFECTS    SS tables are built
===========================================================================*/
extern void e_ul_build_ss_tables(void);

/*===========================================================================
FUNCTION        E_UL_VALIDATE_NETWORK_CFG

DESCRIPTION     This function checks EUL NW config to find if ETCI 0 
                can be supported according the EUL config signalled
                by NW.

DEPENDENCIES    None

RETURN VALUE    Boolean Returns TRUE if ETFCI 0 can be supported under
                given EUL config

SIDE EFFECTS    Validate EUL Config
===========================================================================*/
extern boolean e_ul_validate_network_cfg(void);

/*===========================================================================
FUNCTION        E_UL_SWITCH_SS_TABLE_CFG_PTRS

DESCRIPTION     This function switches the current config pointers to ordered
                config pointers used in the current ss table build

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
extern void e_ul_switch_ss_table_cfg_ptrs(void);


/*===========================================================================

FUNCTION        EUL_DISABLE_FIQ

DESCRIPTION     This function disables the mdsp fiq . Passes NULL address to
                disable FIQ.When isr address is NULL, FIQ shall be disabled 
                by masking it out in the interrupt controller.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    mDSP will not rise FIQ for handling E-DPCH data path

===========================================================================*/
extern void eul_disable_mdsp_fiq(void);

/*===========================================================================

FUNCTION       EUL_CHANGE_FIQ_FIRING_TIME_DURING_TTI_RECFG

DESCRIPTION    This function changes the fiq triggering time during TTI recfg.
               mDSP will.trigger the interrupt based on this time

DEPENDENCIES   This function should be called only during the TTI recfg in EUL

RETURN VALUE   None

SIDE EFFECTS   mDSP will rise FIQ for handling E-DPCH data path based on the new time

===========================================================================*/
void eul_change_fiq_firing_time_during_tti_recfg(e_tti_enum_type e_tti);

/*=========================================================================

FUNCTION        E_UL_CLEANUP

DESCRIPTION     This routine re-initializes the eul encoder driver. It aborts
                all Tx processing in the encoder. It commands the mDSP to stop
                EDPCH channel processing.

DEPENDENCIES    None.

RETURN VALUE    None.

SIDE EFFECTS    None.

=========================================================================*/
extern void e_ul_cleanup( void );

/*=========================================================================

FUNCTION        E_UL_IS_EUL_ACTIVE

DESCRIPTION     This routine checks whether EUL uplink is already active or not

DEPENDENCIES    None.

RETURN VALUE    returns TRUE if EUL uplink module is in DPCH_TX or ERROR
                state else returns FALSE.

SIDE EFFECTS    None.

=========================================================================*/

extern boolean e_ul_is_eul_active(void);

/*=========================================================================

FUNCTION        E_UL_IS_IN_TX_AND_CFG_DB_ACTIVATED()

DESCRIPTION     This routine checks whether the cfg DB is already active 
                and if eul state is in Tx state or not

DEPENDENCIES    None.

RETURN VALUE    Returns TRUE if EUL config DB is activated and if eul 
                is in Tx state else returns FALSE.

SIDE EFFECTS    None.

=========================================================================*/

extern boolean e_ul_is_in_tx_and_cfg_db_activated(void);

/*===========================================================================
FUNCTION        E_UL_ENABLE_SS_TABLE_PENDING_FLAG

DESCRIPTION     This function will enable the pending SS table flag(CR 115236)
                to switch to the new built SS tables for any UL DPCH reconfig
DEPENDENCIES    none
  

RETURN VALUE    none
  

SIDE EFFECTS    none
  
===========================================================================*/

extern void e_ul_enable_ss_table_pending_flag(void);

/*===========================================================================
FUNCTION        E_UL_IS_EDPCH_INFO_CHANGED

DESCRIPTION     This function checks whether there is any specific param info
                that has dependency on SG changed in this recfg cmd. If there 
                is any change in the params it returns the change status so
                that the uplink module will do DTX for the frames till eul 
                uplink new cfg is activated or till tx is enabled by R99 
                procedures(which ever is executed last will result in tx 
                enable) .This function is called only during recfg and 
                after the new cfg is already stored into the ordered page

DEPENDENCIES    This function is called only during recfg and after the new
                cfg is already stored into the ordered page 

RETURN VALUE    returns change if uplink info changes else no_change

SIDE EFFECTS    EDCH shall be DTXed from act time if any param that has 
                dependency on SG changes during recfg 

===========================================================================*/
extern e_ul_edpch_recfg_state_enum_type e_ul_is_edpch_info_changed(void);
/*===========================================================================
FUNCTION        EUL_EDPCCH_PRE_RECONFIG

DESCRIPTION     This funtion is called by SEQ one frame ahead of ACT to
                reconfig EUL EDPCCH for power offset..

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    EUL EDPCCH will change to new config at ACT
===========================================================================*/
void e_ul_edpcch_pre_reconfig(void);

#ifdef FEATURE_L1_LOG_ON_DEMAND
/*===========================================================================
FUNCTION        EUL_PROCESS_UL_CHAN_CFG_LOG_ON_DEMAND

FILE NAME       ulcmd.c

DESCRIPTION     This funtion creates an eul ul chan cfg info log packet and submits it
                to QXDM on demand

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
void e_ul_process_chan_cfg_log_on_demand(void);
#endif /* end of FEATURE_L1_LOG_ON_DEMAND*/

#ifdef FEATURE_WCDMA_CPC_DTX

/*===========================================================================
FUNCTION        E_UL_CHECK_IS_TTI_CHANGED

DESCRIPTION     This checks whether TTI passed is the same as current configuration

DEPENDENCIES    Current EUL config params

RETURN VALUE    TRUE  - TTI is the same as current config
                FALSE - TTI is different

SIDE EFFECTS    None
===========================================================================*/
boolean e_ul_check_is_tti_changed (e_tti_enum_type tti);

/*===========================================================================
FUNCTION        E_UL_CPC_DTX_CFG_EVENT_HANDLER

DESCRIPTION     This function sends mdsp cmd related to START/STOP/RECONF DTX.
                Called from TX timeline event handler.

DEPENDENCIES    The config db information

RETURN VALUE

SIDE EFFECTS    DTX config event is disabled
===========================================================================*/
extern void e_ul_cpc_dtx_cfg_event_handler(void);


/*===========================================================================
FUNCTION       E_UL_CPC_BUILD_BURST_POS_ETFCI_SEL_TABLES

DESCRIPTION     This function is a local command handler for the local command
                posted for the preparation of the arrays that store the start of the DPCCH
                burst position and also the length of the burst. Also this function populates
                the array in which the instances of ETFCI selection allowed will be provided.
                The arrays will be double buffered and a uint32 type. The sub frame number 
                will be used to index into the array based on the mechanism in which they
                are populated.
                

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    Calls the function to update the pre computed arrays for cycle 1, cycle 2
                and etfci selection instances based on syb frame
===========================================================================*/
extern void e_ul_cpc_dtx_build_burst_pos_etfci_sel_tables(void); 

/*===========================================================================
FUNCTION        E_UL_CPC_DTX_UPDATE_STOP_PARAMS

DESCRIPTION     If DCH clean up is called, then DTX should be first stopped before clearing the EUL.
                         This can be even done by calling e_ul_process_dtx_stop_req(), but this is optimized way of stopping DTX

DEPENDENCIES   None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void e_ul_cpc_dtx_update_stop_params(void);

/*===========================================================================
FUNCTION     E_UL_CPC_DTX_BURST_START_SUBFN

DESCRIPTION  This function finds for a given subfn, if DPCCH burst is starting or not.

DEPENDENCIES  NONE

RETURN VALUE  BOOLEAN

SIDE EFFECTS  NONE
===========================================================================*/


extern boolean e_ul_cpc_dtx_burst_start_subfn(uint32,e_ul_cpc_dtx_state_enum_type);


/*===========================================================================
FUNCTION        E_UL_PROCESS_DTX_CFG_REQ

DESCRIPTION     This is the entry function for processing DTX start, stop
                and reconfig operations.

DEPENDENCIES    The command parameters

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void e_ul_process_dtx_cfg_req(l1_setup_cmd_type *setup_cmd_ptr);
/*===========================================================================
FUNCTION        E_UL_PROCESS_DTX_RECONFIG_REQ

DESCRIPTION     Function processes DTX reconfig operation.

DEPENDENCIES    The command parameters

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void e_ul_process_dtx_reconfig_req(l1_setup_cmd_type *setup_ptr);

/*===========================================================================
FUNCTION        E_UL_LOG_CPC_DTX_CFG_RECFG_LOG_PKT

DESCRIPTION     This function logs the Cfg-Recfg log packet 0x422B

DEPENDENCIES    The config db information

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
extern void e_ul_log_cpc_dtx_cfg_recfg_log_pkt(void);

#ifdef FEATURE_CPC_DTX_HS_SCCH_REVERT_HHO_RECOVERY /* RRC Support Needed*/
/*===========================================================================
FUNCTION        E_UL_CPC_DTX_UPDATE_CURR_BACKUP_HS_SCCH_ORD

DESCRIPTION     This function updates the current and the backup hs scch order
                      related variables based on the ord information from RRC

DEPENDENCIES   

RETURN VALUE    BOOLEAN. This function returns TRUE if we are in a case
                     where the RRC action is START and the reverted order that needs to
                     be applied is DTX Disable.

SIDE EFFECTS    Backsup the current order to the backup and the backup should be applied
                       later if requested to be restored.
===========================================================================*/
extern boolean e_ul_cpc_dtx_update_curr_backup_hs_scch_ord(void);

#endif
/*===========================================================================
FUNCTION    E_UL_CPC_DTX_SEND_EVT_TO_DIAG()

DESCRIPTION This function sends DTX events to DIAG

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
extern void e_ul_cpc_dtx_send_evt_to_diag( e_ul_cpc_dtx_action_enum_type dtx_action, 
  uint8 enab_delay, boolean scch_order);

/*===========================================================================
FUNCTION    e_ul_cfg_get_dtx_cfg_cfn()

DESCRIPTION This function returns dtx cfg cfn

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
extern uint16 e_ul_cfg_get_dtx_cfg_cfn(void);
#endif /* FEATURE_WCDMA_CPC_DTX */



/*===========================================================================

FUNCTION        e_ul_get_ul_cat

DESCRIPTION     This function returns the HSUPA CAT. Called from W-MCPM driver

DEPENDENCIES    Unknown

RETURN VALUE    uint16

SIDE EFFECTS    None

===========================================================================*/
extern uint16 e_ul_get_ul_cat(void);



#ifdef FEATURE_WCDMA_MCPM_DRIVER
/*===========================================================================
FUNCTION        E_UL_RELINQUISH_MCPM_CONFIG

DESCRIPTION     This function calls W-MCPM relinquish API

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
void e_ul_relinquish_mcpm_config(uint8 carr_idx);
#endif

#ifdef FEATURE_WCDMA_HS_RACH
/*===========================================================================
FUNCTION   E_UL_PREPARE_CONFIG_FOR_MDSP

DESCRIPTION This function is called when the L1 SW has
                 to send the config command to the FW.

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
extern void e_ul_prepare_config_for_mdsp(wl1_eul_chan_setup_enum_type e_ul_setup_type);

/*===========================================================================
FUNCTION        e_ul_activate_config

DESCRIPTION     This function fills the current config db with the ul params
                received and also program the EUL ISR based on the configured 
                TTI

DEPENDENCIES   None

RETURN VALUE    None

SIDE EFFECTS    Updates the current config to the ordered config.
===========================================================================*/
extern void e_ul_activate_config(void);

/*===========================================================================
FUNCTION        e_ul_set_fiq_offset

DESCRIPTION   This function will program the FIQ offset.

DEPENDENCIES   None

RETURN VALUE    None

SIDE EFFECTS    Updates the current config to the ordered config.
===========================================================================*/
extern void e_ul_set_fiq_offset(e_tti_enum_type e_tti);

/*===========================================================================
FUNCTION   e_ul_int_vars_for_eul_resume

DESCRIPTION This function is called when the channel is 
                 ready for setup.

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
extern void e_ul_int_vars_for_eul_resume(e_tti_enum_type e_tti);
/*===========================================================================
FUNCTION   e_ul_init_vars_for_eul_add

DESCRIPTION This function is called when the channel is 
                 ready for setup.

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
extern void e_ul_init_vars_for_eul_add(void);


#endif /*FEATURE_WCDMA_HS_RACH*/

/*===========================================================================
 
 FUNCTION        e_ul_config_a2_modem_clks
 
 DESCRIPTION     
 
 DEPENDENCIES    Unknown
 
 RETURN VALUE    None
 
 SIDE EFFECTS    Call the clocks and modem pwr drivers to vote
              for higher speeds ad HSUPA is being setup
 
 ===========================================================================*/
extern void e_ul_config_a2_modem_clks(void);

/*===========================================================================
 
 FUNCTION        e_ul_deconfig_a2_modem_clks
 
 DESCRIPTION     
 
 DEPENDENCIES    Unknown
 
 RETURN VALUE    None
 
 SIDE EFFECTS    Call the clocks and modem pwr drivers to relinquish
              for higher speeds ad HSUPA is being setup
 
 ===========================================================================*/

extern void e_ul_deconfig_a2_modem_clks(void);

 /*===========================================================================
 FUNCTION        E_UL_GET_SUB_FN_VAL
 
 DESCRIPTION     This function returns the sub fn value needed to be used in the
                 EUL processing. The function gets the SUB FN value based on whether the feature
                for maintaining the EUL local SUB FN value is defined or not. 
 
 DEPENDENCIES    None
 
 RETURN VALUE    Either returns a valid eul SW module SUB FN variable or the 
                 gets from the legacy EUL SUB FN function
 
 SIDE EFFECTS    None
 ===========================================================================*/
extern uint16 e_ul_get_sub_fn_val(void);

/*===========================================================================
FUNCTION         EUL_DL_UL_LOGGING_EDL_CONFIG_UPDATE

DESCRIPTION     This function updates the EDL config information
                and this function is called just before the log packet is submitted
                as the config information is needed just only once

DEPENDENCIES     None

RETURN VALUE     None

SIDE EFFECTS     A logging structure gets updated

============================================================================*/
extern void e_dl_ul_logging_edl_config_update(eul_logging_edl_config_info_struct_type *e_dl_cfg_info_ptr, wl1_ul_carr_id_enum_type carr_idx);

#ifdef FEATURE_WCDMA_DC_HSUPA

/*===========================================================================
FUNCTION        E_UL_SEC_PHYCHAN_CFG_VALIDATE

DESCRIPTION     This function checks that the operation on  EDPCH channel as
                specified by the parameters received in the setup command can
                be carried out successfully, i.e. the parameters are valid
                for Secondary EUL.

DEPENDENCIES    None

RETURN VALUE    TRUE if the command can be carried out, FALSE otherwise

SIDE EFFECTS    None
===========================================================================*/
boolean e_ul_sec_phychan_cfg_validate(  l1_setup_cmd_type *setup);

/*===========================================================================
FUNCTION        E_UL_SEC_PROCESS_CPHY_SETUP_REQ

DESCRIPTION     This function process the operation on SEC EDPCH channel as
                specified by the parameters referenced by the setup command.

DEPENDENCIES    None

RETURN VALUE    TRUE if the command is carried out successfully,
                FALSE otherwise

SIDE EFFECTS    SEC EDPCH shall be configured/reconfigured/stopped
===========================================================================*/
extern boolean e_ul_sec_process_cphy_setup_req(boolean pri_sec_eul_incl);

/*===========================================================================
FUNCTION        E_UL_SEC_EDPCH_CFG_EVENT_HANDLER

DESCRIPTION     This function sends the mdsp cmd related to Sec EDPCH.
                          In this case specifically Primary EUL is not impacted.

DEPENDENCIES    The config db information

RETURN VALUE

SIDE EFFECTS    The FW Sec EDPCH shadow buffer will be filled.
===========================================================================*/
extern void e_ul_sec_edpch_cfg_event_handler(void);

/*===========================================================================
FUNCTION        E_UL_SEC_EDPCH_SET_STATE

DESCRIPTION     This function Set  EUl state for Sec carrier

DEPENDENCIES    Under Sec SUL mutext lock

RETURN VALUE    None

SIDE EFFECTS  
===========================================================================*/
extern void e_ul_sec_edpch_set_state(e_ul_sec_edpch_state_enum_type next_state);

/*===========================================================================
FUNCTION        E_UL_SEC_EDPCH_SET_ACTION

DESCRIPTION     This function Set EUL action for Sec carrier

DEPENDENCIES    Under Sec SUL mutex lock

RETURN VALUE    None

SIDE EFFECTS  
===========================================================================*/

void e_ul_sec_edpch_set_action(e_ul_sec_chan_action_enum_type next_action);

#endif /* FEATURE_WCDMA_DC_HSUPA */

/*===========================================================================
FUNCTION        E_UL_CPC_DTX_CFG_DONE_EVENT_HANDLER

DESCRIPTION     This function is the respone to mdsp cmd related to START/STOP/RECONF DTX.
                Called from CFG_DONE event handler

DEPENDENCIES    The config db information

RETURN VALUE

SIDE EFFECTS    None
===========================================================================*/
extern void e_ul_cpc_dtx_cfg_done_event_handler(void);

/*===========================================================================

FUNCTION E_UL_DISABLE_CPC_DTX

DESCRIPTION
  This function disables CPC-DTX

DEPENDENCIES
  None

RETURN VALUE
  TRUE if CPC-DTX is not enabled
  FALSE if CPC-DTX is enabled - in order to wait for FW to finish the configuration
    before informing L1M

SIDE EFFECTS
  None

===========================================================================*/
extern boolean e_ul_disable_cpc_dtx(void);

#endif /*end of EULCFG_H*/
