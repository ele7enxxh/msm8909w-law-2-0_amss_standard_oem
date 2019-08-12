#ifndef CUSTWCDMA_H
#define CUSTWCDMA_H
/*=========================================================================== 

                           C U S T    W C D M A 

DESCRIPTION
  Customer file for WCDMA

  Copyright (c) 2002 to 2012 by Qualcomm Technologies, Inc. All Rights Reserved.
===========================================================================*/


/*===========================================================================
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/cust/inc/custwcdma.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/07/15   vnr	   Enabled FEATURE_WCDMA_STEP1_PEAK_PRUNER
12/07/14   geg     Enabled FEATURE_WCDMA_MDSP_RECOVERY  
11/28/14   vnr	   Enabled FEATURE_WCDMA_CSFB_CALL_OPT for JO.1.0 and TA.1.0
11/24/14   nsb     Moved features under FEATURE_WCDMA_JOLOKIA_MODEM for JO.1.0 and TA.1.0 
11/06/14   nsb     Enabled FEATURE_WCDMA_ENABLING_QICE_WITH_CM for JO.1.0 and TA.1.0 
11/06/14   nsb     Enabled FEATURE_WCDMA_RRC_HEAP_TO_BSS for JO.1.0 and TA.1.0 
10/22/14   jd      Added FEATURE_WCDMA_PN_PACKET_VER4 for including uarfcn in PN log packet. 
10/17/14   psr     Mainlined FEATURE_WCDMA_ENH_PN_SCHEDULING and FEATURE_WCDMA_PANIC_SRCH_OPT for Jolokia and Thor.
10/15/14   skk     Added FEATURE_WCDMA_REACQLESS_PICH, will be enabled only for Jolokia.
10/09/14   mk      Encapsulate Jolokia and Tabasco specific W flags under FEATURE_JOLOKIA_MODEM and FEATURE_TABASCO_MODEM 
10/02/14   rkmk    Enable FEATURE_WCDMA_L1_SPLIT_ACQ_ACROSS_BPLMN_CYCLES under FEATURE_JOLOKIA_MODEM 
10/01/14   rkmk    Enable FEATURE_WCDMA_ZI_OPT_PHASE2 under FEATURE_JOLOKIA_MODEM
09/23/14   rkmk    Added condition to disbale FEATURE_WCDMA_REL10 when FEATURE_JOLOKIA_MODEM is defined
09/22/14   rkmk    Enable FEATURE_WCDMA_MAC_UL_ZI_REDUCTION under FEATURE_JOLOKIA_MODEM
09/02/14   rkmk    Feature cleanup (CR 540388)
08/21/14   rkmk    Disabled FEATURE_WCDMA_NPA_THERMAL and FEATURE_WCDMA_MTPL_THERMAL_BACKOFF if FEATURE_CUST_1 is defined
08/13/14   rkmk    Enable FEATURE_WCDMA_CIR_ENHANCEMENT for only Jo under FEATURE_JOLOKIA_MODEM 
07/23/14   ar      Enable FEATURE_WCDMA_ENH_OOS_RXD_PWC only when FEATURE_CUST_1 is not defined
06/30/14   as      Enabled FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH and FEATURE_WCDMA_TO_LTE_FACH 
06/30/14   as      Removed FEATURE_DEBUG_STMR_TEST 
06/23/14   as      Enabled FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST
                           FEATURE_WCDMA_REL7P1_MPR 
06/06/14   as      Adding FEATURE_WCDMA_INTERF_OFFLINE_PN and FEATURE_WCDMA_W2L_FORCED_DET_ON_RESEL_FAIL outside #ifdef FEATURE_CUST_1
05/16/14   as      Enabled FEATURE_WCDMA_HS_ONLY_TPC 
04/28/14   as      Replace FEATURE_PARTIAL_QBTA with FEATURE_QTA 
04/23/14   as      Move FEATURE_WCDMA_DEDICATED_PRI_INFO under target level feature FEATURE_DEDICATED_PRIORITY 
03/17/14   mk      Enable FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR 
02/26/14   ar      Enabled FEATURE_WCDMA_W2L_FORCED_DET_ON_RESEL_FAIL under FEATURE_DIME_2_1
02/21/14   as      Enabled common DIME featues and undefined Multi freq band indicator feature for BO.2.0 compilation; Also undefined 2 fach features
02/19/14   as      Added : FEATURE_WCDMA_BOLT_2_0 
02/14/14   bj      Reverting UARFCN print changes in 0x4179 WCDMA PN Search log packet version-2
02/07/14   bj      Enabled FEATURE_WCDMA_PN_PACKET_VER2 for uarfcn info in SRCH_PN_LOG_PACKET
02/03/14   geg     Enabled FEATURE_WCDMA_SAR_TX_PWR_FILTER 
01/30/14   geg     Enabled FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR  
01/24/14   geg     Enabled FEATURE_WCDMA_COEXISTENCE_SW 
				           FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW 
12/12/13   scv     Enabled FEATURE_WCDMA_MODEM_TPUT_STATS_API 
11/27/13   scv     Enabled FEATURE_WCDMA_UL_COMPR
11/20/13   geg     Enabled FEATURE_WCDMA_SRB_RLC_SDU_RESTORE 
10/16/13   ar      Enabled FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH and FEATURE_WCDMA_TO_LTE_FACH
10/10/13   geg     Enabled FEATURE_WCDMA_MODEM_COEXISTENCE_SW
10/07/13   geg     FEATURE_WCDMA_DEFERRED_SIB11_12_READING is an independent feature and should not be tied to FEATURE_3GPP_CSFB being enabled
07/26/13   rkmk    Removed FEATURE_WCDMA_DB_DC_HSDPA , it will be enabled at target level
07/24/13   jd      Changes for Dual band Dual carrier
07/23/13   vs      Defined the feature FEATURE_WCDMA_ANT_SWITCH_SUPPORT
07/09/13   rkmk    Enabled FEATURE_WCDMA_CUST_RXDPM_THRESHOLD under FEATURE_DIME_2_1
07/04/13   mk      Removed target level rumi emulation feature condition
06/27/13   rkmk    Removed WCDMA_RRC_INDICATE_CODEC_TO_CM - Enabled at Target level 
06/25/13   rkmk    Enabled FEATURE_WCDMA_DEFAULT_CFG_CELLFACH  
05/30/13   rkmk    Moved FEATURE_RLC_MISSING_PDU_REPEAT_STATUS and FEATURE_WCDMA_SRB_PRIO_READJ under FEATURE_DIME_2_1  
05/15/13   rkmk    Enabled FEATURE_WCDMA_AM_IGNORE_MAXRST under FEATURE_DIME_2_1  
05/09/13   mk      Enabling RUMI specific feature for BOLT Modem
05/08/13   rkmk    Replaced Target level FEATURE_DIME_1_1 with FEATURE_DIME_2_1
05/02/13   rkmk    Enabled FEATURE_WCDMA_REL9
04/18/13   rkmk    Enabled FEATURE_WCDMA_INTERF_OFFLINE_PN under FEATURE_DIME_1_1 
04/09/13   geg 	   Enabled FEATURE_WCDMA_FREE_FLOATING_TASK  
04/05/13   geg 	   Enabled FEATURE_WCDMA_UNBAR_OPRTR_RSVD_FREQ 
03/20/13   rkmk    Enabled FEATURE_WCDMA_DIME_TX_STATE_REPORTING  
03/14/13   geg     Enabled FEATURE_WCDMA_RXD_LOCKMODE_CHANGE_FOR_ANT_COUPLING under FEATURE_DIME_1_1    
03/07/13   rkmk    Enabled FEATURE_WCDMA_MRAB_TVM_MAC_OPT under FEATURE_DIME_1_1 
01/18/13   geg     Enabled FEATURE_WCDMA_MCVS   
12/12/12   geg     Enabled FEATURE_WCDMA_HS_FACH_DRX  
12/12/12   geg     Enabled FEATURE_WCDMA_HS_RACH 
10/22/12   geg     Feature clean up and also enabled FEATURE_UM_PDU_LOGGING and FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH
10/01/12   geg     Enabled FEATURE_WCDMA_DEDICATED_PRI_INFO 
09/24/12   mk      moved FEATURE_WCDMA_TO_LTE from tbc_modem_wcdma.builds
09/19/12   dp      Finished mainlining FEATURE_WCDMA_QICE_CONTROLLER
09/13/12   xl      define FEATURE_WCDMA_TRITON_MCAL_API for Triton 
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
05/22/12   pr      STMR changes for Dime
03/28/12   zr      Adding feature definitions for Dime
07/03/12   mk      Enabled FEATURE_WTOL_TDD_CM_SUPPORT 
06/20/12   mk      Enabled FEATURE_WCDMA_HS_RACH 
                            FEATURE_MAC_I
04/18/12   geg     Enabled WCDMA_RRC_INDICATE_CODEC_TO_CM 
                           FEATURE_WTOL_PS_HANDOVER  
03/28/12   mk      Enabled below features under FEATURE_WCDMA_MIMO 
                          FEATURE_MIMO_DEMOD_LOG_PKT_V2
                          FEATURE_MIMO_CHAN_LOG_PKT_V2
                          FEATURE_WCDMA_SCPICH_MIMO_MODE_PRESENT
                          FEATURE_HSPA_PLUS_CAT20
                          FEATURE_WCDMA_COMBINED_MIMO_AND_64QAM
                          FEATURE_WCDMA_MIMO_CODEBOOK_RESTRICTION
                          FEATURE_WCDMA_TX_DIVERSITY_DL_FOR_MIMO
                          FEATURE_WCDMA_QICE_PLUS_MIMO
03/27/12   geg     Enabled FEATURE_WCDMA_MIMO 
                           FEATURE_WCDMA_HS_FACH_DRX 
03/21/12   geg     Enabled FEATURE_WCDMA_HS_FACH 
                           FEATURE_WCDMA_NEW_L2_INTERFACE 
03/16/12   geg     Enabled FEATURE_WCDMA_TRIGGER_BASED_RACH
01/25/12   geg     Enabled FEATURE_EUTRA_DETECT 
01/25/12   geg     Enabled FEATURE_CPC_DTX_HS_SCCH_REVERT_HHO_RECOVERY 
01/18/12   geg     Enabled FEATURE_WCDMA_HSUPA_IRQ_SPLIT 
					       FEATURE_WCDMA_CPC_DTX
						   FEATURE_WCDMA_CPC_DTX_CM
 						   FEATURE_WCDMA_CPC_DTX_LOW_GRANT_OPT
						   FEATURE_WCDMA_CPC_DRX
01/10/12   geg     Enabled FEATURE_WCDMA_OPTIMIZED_BAND_SCAN 
12/12/11   mk      Enabled FEATURE_WCDMA_RXD_PCH 
12/12/11   mk      Enabled FEATURE_IDLEMODE_FILTER
12/08/11   mk      Enabled FEATURE_WCDMA_EARLY_RF_SLEEP 
12/08/11   mk      Enabled FEATURE_WCDMA_FATPATH_IN_DCH
12/05/11   mk      Enabled FEATURE_WCDMA_DSR_SUPPORT
12/02/11   mk      Enabled FEATURE_WCDMA_SUSPEND_BMC_DURING_BPLMN 
11/30/11   mk      Enabled FEATURE_RRC_INTEGRITY_SW_ENGINE. 
11/30/11   mk      Enabled FEATURE_WCDMA_A2_POWER_COLLAPSE 
11/08/11   mk      Enabled FEATURE_WCDMA_RXD_SUPPORT_SRB_NON_TRAFFIC 
11/04/11   mk      Removed FEATURE_WCDMA_REPLACE_LTE_CELL_MEAS_AS_DETECT
11/03/11   mk      Enabled FEATURE_RLC_MISSING_PDU_REPEAT_STATUS 
10/21/11   mk      Enabled FEATURE_WCDMA_UL_CM_INTLV_WORKAROUND
10/19/11   mk      Enabled FEATURE_WCDMA_FAST_RETURN_TO_LTE_AFTER_CSFB
10/17/11   mk      Enabled FEATURE_WCDMA_ALLOW_ECAL_AC_BARRED_CELL 
10/04/11   mk      Enabled FEATURE_WCDMA_REPLACE_LTE_CELL_MEAS_AS_DETECT 
10/04/11   mk      Enabled FEATURE_WCDMA_BC19 
09/23/11   mk      Enabled FEATURE_WCDMA_NPA_THERMAL 
09/23/11   mk      Enabled FEATURE_WCDMA_MTPL_THERMAL_BACKOFF 
09/23/11   mk      Enabled FEATURE_TEMPERATURE_FC 
09/20/11   mk      Removed FEATURE_WCDMA_DO_NOT_SLAM_GSTMR_UPON_WAKEUP 
09/14/11   mk      Enabled FEATURE_WCDMA_CONTINUE_MPLMN
09/07/11   mk      Disabled FEATURE_WCDMA_CONTINUE_MPLMN
09/07/11   mk      Enabled FEATURE_WCDMA_SRB_PRIO_READJ
09/07/11   mk      Enabled FEATURE_WCDMA_CONTINUE_MPLMN
08/30/11   mk      Enabled FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
08/17/11   mk      Removed FEATURE_WCDMA_RACH_ABORT_FW_WORKAROUND 
08/17/11   mk      Enable FEATURE_WCDMA_RACH_ABORT_PROCEDURE                
08/09/11   mk      Removed FEATURE_WCDMA_RACH_RTR8600
08/04/11   mk      Enabled FEATURE_WCDMA_VOICE_SCHIC_DISABLE
07/20/11   mk      Enabled FEATURE_WCDMA_CONCURRENT_R99_EUL_TX_ENCODE 
07/19/11   mk      Enabled FEATURE_WCDMA_NIKEL_ERAMA_ACCESS_FIX
07/13/11   mk      Moved feature FEATURE_WCDMA_SEND_SS_FREEZE_UNFREEZE_TO_FW 
                   definition from mdspsync.h to custwcdma.h
07/08/11   mk      FEATURE_WCDMA_HSDPA_LLR_LOG_PKT_V6
06/09/11   mc      Disabled FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
05/19/11   geg     Enabled FEATURE_WCDMA_USE_MODEM_HEAP
05/03/11   sb      Disabled FEATURE_WCDMA_CME_QICE_FFINV_LOGGING
04/28/11   mc      Enabled FEATURE_XO.
04/26/11   mc      Enabled FEATURE_W2L_IDLE_FW_RF_TUNE
04/21/11   mc      Updated feature set for NikeL 1.0
04/04/11   stk     Enabled FEATURE_WCDMA_MCPM_DRIVER if feature_MCPM is defined. 
04/01/11   sj      Enabled FEATURE_WCDMA_RUMI_SKIP_CME
03/17/11   stk     Mainlined FEATURE_6275_COMPILE for NikeL 
01/20/11   stk     Removed FEATURE_WCDMA_GENESEIS_UL, since the UL code is featurized under FEATURE_WCDMA_GENESIS_SW
01/07/11   stk     Enabled FEATURE_WCDMA_DC_HSDPA_DIFF_TXD_MODE and FEATURE_WCDMA_NIKEL_SW
                   Removed features which are dependant on dem.h.
10/21/10   sup     Enabled WPLT features
10/20/10   sup     Enabled FEATURE_WCDMA_FULL_SRCH_IN_DC
                           FEATURE_WCDMA_CME_NGHBR_SUPPORT 
						   FEATURE_WCDMA_OUTAGE_OLPC
						   FEATURE_WCDMA_L1_RXD_FILTERING_SUPPORTED
10/11/10   sup     Enabled FEATURE_WCDMA_OFFLINE_PICH_OPT
09/20/10   sup     Enabled FEATURE_WCDMA_L1_WDOG_DYNAMIC
                           FEATURE_WCDMA_L2_WDOG_DYNAMIC
                           FEATURE_WCDMA_RRC_WDOG_DYNAMIC
                           FEATURE_WCDMA_ENH_HS_CFG_LOG
09/01/10   sb      Enabled FEATURE_DISABLE_LTE_IRAT_BPLMN
08/24/10   sb      Enabled FEATURE_WCDMA_DC_QICE_RECEIVER
08/11/10   sb      Enabled FEATURE_3GPP_FAST_DORMANCY
08/06/10   sb      Enabled FEATURE_WCDMA_RXD_SUPPORT_CM and 
							FEATURE_RLC_TIMER_OPT
08/03/10   sup     Enabled FEATURE_WCDMA_RACH_RTR8600 
                           FEATURE_HSPA_PLUS_CAT24
	                       FEATURE_WCDMA_BAND_SPEC_DC_HSDPA
07/20/10   sup     Enabled FEATURE_WCDMA_DEM_PA_CONTROL
07/16/10   sup     Added all the LTE features under FEATURE_WCDMA_TO_LTE
07/16/10   sup     Enabled FEATURE_WRLF_SYSTEM_SEL
07/07/10   sup     Enabled FEATURE_WCDMA_HSUPA_BETA_EC_Q8_FORMAT
07/07/10   sup     Enabled FEATURE_HACK_LTE_CELL_MEAS_AS_DETECT
06/24/10   sup     Enabled FEATURE_WCDMA_ABSOLUTE_PRIORITY
06/21/10   sup     Enabled FEATURE_WCDMA_IRAT_RF_BUF_SUPPORT
                           FEATURE_WCDMA_OFFLINE_MODEM_W2G
                   Disabled FEATURE_WCDMA_EQ_DISABLED_BY_DEFAULT
06/10/10   sup     Enabled FEATURE_TEMPERATURE_FC
06/03/10   sup     Enabled FEATURE_WCDMA_FDPCH_STTD
05/25/10   sup     Enabled FEATURE_WCDMA_EQ_DISABLED_BY_DEFAULT
                           FEATURE_WCDMA_FDPCH
                           FEATURE_WCDMA_BC11
 		           FEATURE_WCDMA_DEFAULT_RXDIV_ENABLED_FOR_DATA_CALLS
 		           FEATURE_WCDMA_SIB19
04/26/10   sup     Enabled FEATURE_WCDMA_TO_LTE
                           FEATURE_WTOG_REDIRECTION_AFTER_CONN_RELEASE
04/20/10   sup     Enabled FEATURE_LFS_COOS
04/15/10   sup     Disabled FEATURE_WCDMA_EQ_DISABLED_BY_DEFAULT
04/14/10   sup     Disabled FEATURE_WCDMA_FDPCH
04/09/10   sup     Enabled FEATURE_WCDMA_L1_ZI_OPT under FEATURE_MODEM_HEAP
04/08/10   sup     Disabled FEATURE_PRINT_MDSP_PC_ON_STATUS_DUMP_FAILURES 
04/08/10   sup     Enabled FEATURE_PRIORITIZE_850_BAND_FOR_FULL_SCAN
                           FEATURE_WCDMA_900
                           FEATURE_WCDMA_EFDPCH
                           FEATURE_WCDMA_FDPCH
03/22/10   sup     Removed FEATURE_WCDMA_INTER_FREQ_NOT_SUPPORTED
                   Disabled FEATURE_HSPA_PLUS_CAT18
                             FEATURE_WCDMA_MIMO
02/23/10   sup     Enabled #define FEATURE_WCDMA_QICE_USE_CMI_DEM
02/18/19   sup     Disabled FEATURE_WCDMA_DISABLE_SLEEP_AT_BRING_UP
02/18/10   sup     Enabled FEATURE_WCDMA_EQ_DISABLED_BY_DEFAULT
02/16/10   sup     Enabled FEATURE_WCDMA_QICE_CONTROLLER
                           FEATURE_WCDMA_QICE_DC_CONTROLLER
                           RRC_PRACH_DB_OPTIMISATION_ENH_PH2
                           (If FEATURE_MODEM_HEAP is defined) then
                   Enabled FEATURE_WCDMA_RLC_ZI_REDUCTION, 
                           FEATURE_WCDMA_RLC_DL_ZI_REDUCTION
                           FEATURE_WCDMA_RRC_ZI_OPT_PHASE_2
                            (If FEATURE_MODEM_HEAP is NOT defined) then
                    Enabled  FEATURE_RRC_HEAP
                            FEATURE_RRC_STATIC_BUFFER_ALLOC
02/10/10   sup     Enabled FEATURE_DL_PATH_PROFILING
02/09/10   sup     Enabled FEATURE_WCDMA_DISABLE_SLEEP_AT_BRING_UP
                   Disabled FEATURE_WCDMA_SCPICH_MIMO_MODE_PRESENT
01/14/09   sup     Enabled FEATURE_WCDMA_INTER_FREQ_NOT_SUPPORTED
12/14/09   rmsd    Define FEATURE_RRC_LED only on Windows environment.
12/10/09    sv     Disabled FEATURE_WCDMA_HSUPA_PSEUDO_FIQ
                            FEATURE_WCDMA_PP_HW_UL
                   Enabled  FEATURE_WCDMA_UL_MODEM_ACC
                            FEATURE_DC_HSDPA_LOGGING
12/03/09    sv     Enabled FEATURE_RRC_OPTIMIZED_SIB_HEAP
11/28/09    vb     Undefined FEATURE_DC_HSDPA_LOGGING
11/23/09    sv     Enabled FEATURE_DC_HSDPA_LOGGING
                   Enabled FEATURE_WCDMA_TXAGC_DB10
11/22/09    kvk    undefined FEATURE_WCDMA_DEFAULT_RXDIV_ENABLED_FOR_DATA_CALLS
11/19/09    sv     Enabled FEATURE_WCDMA_GENESIS_F8_BUG_WORKAROUND
11/13/09    sv     Enabled FEATURE_WCDMA_GENESIS_FWS
11/12/09    kvk    Enabled RUMI_8220_BRINGUP
                   FEATURE_WCDMA_DL_MODEM_ACC
                   FEATURE_WCDMA_A2_HW_DL
                   FEATURE_WCDMA_GENESEIS_UL
                   FEATURE_WCDMA_GENESIS_SW
                   FEATURE_WCDMA_GENESIS_SW_FW
                   FEATURE_WCDMA_A2_HW_UL
                   FEATURE_WCDMA_HSUPA_PSEUDO_FIQ
                   FEATURE_MCAL_SRCH_CARR_SEL_SYNC_PARAMS
                   undefined FEATURE_WCDMA_OFFLINE_MODEM_IRAT_PHASE1
                   undefined FEATURE_WCDMA_OFFLINE_MODEM_W2G
                   undefined FEATURE_WCDMA_PP_HW_DL
10/05/09    sv     Enabled  FEATURE_WCDMA_DC_HSDPA
                            FEATURE_WCDMA_DEBUG_ATTEMPT_FULL_SRCH
                            FEATURE_WCDMA_REL8
8/21/09     sv     Disabled FEATURE_WCDMA_OFFLINE_MODEM_IRAT_PHASE2
	                    FEATURE_WCDMA_QICE_CONTROLLER
	                    FEATURE_WCDMA_CME_QICE_LOGGING
                            FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
                            FEATURE_LFS_COOS
            sv     Enabled  RUMI_8220_FW_SW
                            FEATURE_8220_WCDMA_SW_FW
                            FEATURE_8220_WCDMA_SW
	                    FEATURE_8220_WCDMA_BUILD_HACK
10/08/09    sv     Enabled FEATURE_RLC_MULTI_CLIENT_FLOW_CONTROL
                           FEATURE_DUP_ITEM_FC
09/21/09    sv     Disabled FEATURE_WCDMA_OFFLINE_MODEM_IRAT_INITIAL_DEV
09/09/09    sv     Enabled FEATURE_SMALL_ITEM_FC
8/21/09     sv     Enabled FEATURE_STATUS_OPT_BITMAP
7/28/09     sv     Enabled FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
7/21/09     sv     Enabled FEATURE_PESSIMISTIC_CQI_LOGGING
                   Disabled FEATURE_WCDMA_OFFLINE_MODEM_IRAT_PHASE1
                   Enabled FEATURE_WCDMA_OFFLINE_MODEM_IRAT_PHASE2
                   Disabled FEATURE_WCDMA_R99_EUL_MUTEX
7/20/09     sv     Enabled FEATURE_WCDMA_SCPICH_MIMO_MODE_PRESENT
                   Enabled FEATURE_LFS_COOS
7/15/09     sv     Enabled FEATURE_WCDMA_QICE_CONTROLLER 
                   Enabled FEATURE_WCDMA_CME_QICE_LOGGING
                   Enabled FEATURE_WCDMA_CME_EQ_RECFG_LOGGING  
                   Disabled FEATURE_WCDMA_CME_CONTROLLER
                   Disabled FEATURE_WCDMA_CME_CONTROLLER_BRINGUP
7/9/09      sv     Enabled FEATURE_TC_CONSTANT_DELAY_ENH 
7/06/09     sv     Enabled FEATURE_TIMER_BASED_SDU_DISCARD 
            sv     Enabled FEATURE_LARGE_ITEM_FC
6/11/09     sv     Enabled FEATURE_WCDMA_SNOW3G_SECURITY 
5/26/09     sv     Enabled FEATURE_WCDMA_R99_EUL_MUTEX 
5/12/09     sv     Disabled FEATURE_WCDMA_REL7_HACK_CHG_TAU_DPCH
                   Enabled FEATURE_WCDMA_DO_NOT_SLAM_GSTMR_UPON_WAKEUP
5/11/09     sv     Enabled FEATURE_CLTD_LOGGING
5/11/09     sv     Enabled FEATURE_WCDMA_STORE_AM_RLC_SIZE_FROM_OTA
3/30/09     sv     Enabled FEATURE_HSPA_PLUS_CAT18
                           FEATURE_WCDMA_MIMO 
3/24/09     sv     Enabled FEATURE_WCDMA_RLC_LW_MUTEX
3/20/09     sv     Disabled FEATURE_WCDMA_RXD_SUPPORT_CM
9/3/09      sv     Enabled FEATURE_WCDMA_HSUPA_CM
                           FEATURE_WCDMA_SRCHACQ_RETRY
                           FEATURE_LIMIT_TFCS_TO_ERAM_SIZE
                           FEATURE_WCDMA_OFFLINE_MODEM_IRAT_PHASE1
                           FEATURE_WCDMA_OFFLINE_MODEM_IRAT_INITIAL_DEV
                           FEATURE_WCDMA_OFFLINE_MODEM_W2G        
4/3/09      sv     Disabled FEATURE_WCDMA_SRCHACQ_RETRY
3/3/09      sv     Enabled FEATURE_RLC_ALTERNATE_E_BIT
2/27/09     sv     Disabled FEATURE_TIMER_BASED_SDU_DISCARD
                   Enabled  FEATURE_WCDMA_CLEAR_TRK_LO_POLARITY 
                            FEATURE_ADAPTIVE_STEP_SIZE_OLPC
                            FEATURE_DISABLE_384K_ON_20MS_TTI_UE_CAPABILITY    
                            FEATURE_RRC_DEFAULT_BAND_PRIORITY
                            FEATURE_WCDMA_BAND_PRIORITIZATION
                            FEATURE_BPLMN_SEARCH_320MS
                             FEATURE_WCDMA_SRCHACQ_RETRY
2/24/09     sv     Enabled FEATURE_ENABLE_TD_ON_NASTT_SEARCHES
                   Enabled FEATURE_WCDMA_REL6_ENH_MPR and FEATURE_WCDMA_REL6_ENH_MPR_TABLES
                   Disabled FEATURE_WCDMA_HSUPA_NV_DEFAULT_CATEGORY_5
2/17/09     sv     Enabled FEATURE_WCDMA_L1_INTRAF_MRM_OPT
                   Disabled FEATURE_WCDMA_REL7_DISABLE_UL_HE_FIELD 
2/3/09      sv     Enabled FEATURE_DEQUEUE_SDU_TO_PROCESS
                   Enabled FEATURE_WCDMA_REL7_DISABLE_UL_HE_FIELD 
                   Enabled FEATURE_TM_LB
                   Enabled FEATURE_RRC_CALLOC
                   Enabled FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU
                   Enabled FEATURE_STAY_CAMPED_ON_ACCESS_CLASS_BARRED_CELL
                   Enabled FEATURE_SUPPORT_FOR_PARTIAL_ROAMING
                   Enabled FEATURE_ABORT_ESTB_ON_RA_LA_MISMATCH
                   Enabled FEATURE_TVM_IND_IN_CU
                   Enabled FEATURE_SMC_APPEND_ACTIVE_CN_DOMAIN_START_IN_CU
                   Enabled FEATURE_HSDPA_MAC_HS_DL_RESTORE_TSN
                   Enabled FEATURE_TFCC_ENHNC
                   Enabled FEATURE_MEAS_TRID_ENHNC
                   Enabled FEATURE_RRC_BAND_TYPE_DETECTION_FOR_BPLMN
                   Enabled FEATURE_RLC_REMOVE_DUPLICATE_NAKS
                   Enabled FEATURE_POLL_OPTIMIZATION
                   Enabled FEATURE_AVOID_OUT_OF_WINDOW_RETX
                   Enabled FEATURE_MULTIPLE_SCCPCH_SUPPORTED
                   Enabled FEATURE_CELL_SELECTION_PERIODIC
                   Enabled FEATURE_ERR_FATAL_ON_MDSP_SYNC_TIMEOUT
1/28/09    sv      Enabled FEATURE_MULTIPLE_RLC_OPERATIONS
1/26/09    sv      Enabled FEATURE_CCCH_IE_ENH
1/13/09    sv      Enabled FEATURE_WCDMA_HSUPA_2MS_TTI
                   Enabled FEATURE_WCDMA_HSUPA_ENH
		   Enabled FEATURE_WCDMA_HSUPA_INVALID_SUBFN_FIX
                   Enabled FEATURE_WCDMA_HSUPA_NV_DEFAULT_CATEGORY_5
1/06/09    sv      Enabled FEATURE_TC_HSPA_OPTIMIZATION 
                   Enabled FEATURE_DISCONNECT_ON_UNSUPPORTED_IRAT_REDIRECTION
12/22/08   sv      Enabled FEATURE_WCDMA_DEFAULT_RXDIV_ENABLED_FOR_DATA_CALLS
12/19/08   sv      Defined FEATURE_WCDMA_REL7_HACK
12/17/08   sv	   Defined FEATURE_Q6_MT and UNDEFINED FEATURE_L2_MUTEX
                   Defined FEATURE_WCDMA_REL7_HACK_CHG_TAU_DPCH
12/11/08   stk     Undefined FEATURE_WCDMA_INTERP_PHASE_TRACKING
12/10/08   sv      Enabled FEATURE_WCDMA_OFFLINE_MODEM
12/10/08   sv      Disabled FEATURE_WCDMA_HSUPA_ENH
                   Disabled FEATURE_CLTD_LOGGING
12/09/08   sv      Disabled FEATURE_WCDMA_HSUPA_2MS_TTI
12/08/08   sv      Enabled FEATURE_HSUPA_NEW_ETFCI_ALGORITHM 
12/08/08   sv      Enabled FEATURE_WCDMA_UL_DATA_PATH_OPT
                   Enabled FEATURE_WCDMA_PP_HW_UL
                   Enabled FEATURE_L2_MUTEX
                   Enabled FEATURE_WCDMA_HSUPA_INVALID_SUBFN_FIX
                   Enabled FEATURE_NV_WCDMA_HSUPA_CATEGORY
                   Enabled FEATURE_SUPPORT_FOR_PARTIAL_ROAMING
11/21/08   mg      Enable FEATURE_WCDMA_HS_DECODE_LOG_CODE_V4 
11/21/08   mg      Enable FEATURE_ENABLE_INTERFREQ_MEAS_ONLY 
11/18/08   mg      Enable FEATURE_WCDMA_HSUPA_2MS_TTI and FEATURE_WCDMA_HSUPA_ENH
11/18/08   mg      Enable FEATURE_WCDMA_CME_SCHIC_LOGGING and FEATURE_WCDMA_CME_SCHIC_LOGGING
11/14/08   m       Enabled FEATURE_WCDMADIAG_INIT_IN_RRC
11/14/08   sup     Enabled FEATURE_WL1_NEW_SLEEP_API
11/8/08    sup     Enaled FEATURE_WCDMA_DEMOD_FINGER_LOCK_THRESH_27
11/06/08   sup	   Disabled FEATURE_WCDMA_DEFAULT_RXDIV_ENABLED_FOR_DATA_CALLS
10/28/08   vsr     Enabled FEATURE_WCDMA_CME_LOGGING
10/15/08   m       EUL now uses real FIQ
10/15/08   mg      Un define FEATURE_WCDMA_DISABLE_SLEEP_AT_BRING_UP
10/14/08   m       Define HDET related L1/RF features
10/09/08   vsr     Disabled FEATURE_WCDMA_EQ_CONTROLLER
                   Enabled FEATURE_WCDMA_CME_CONTROLLER
10/08/08   hk      Defined TCXOMGR30 Feature
10/08/08   sv      Enable feature FEATURE_MCAL_SRCH_ANTSEL_SYNC_PARAMS. 
09/03/08   mg      Disable FEATURE_WCDMA_SNOW_3G_ALGO_SUPPORT
09/02/08   mg      Enable FEATURE_FW_SW_SYNC
08/23/08   mg      Disable FEATURE_WCDMA_MIMO
07/16/08   asr     Enabled FEATURE_T_BARRED_SUPPORT
07/15/08   ms      Disabled FEATURE_WCDMA_DL_DATA_PATH_OPT_DEBUG
01/30/08   stk     Enabled New DL Features
                           FEATURE_WCDMA_DL_ENHANCED
                           FEATURE_MCAL_WCDMA
                           FEATURE_WCDMA_DRX_MANAGER
                           FEATURE_MCAL_WCDMA_SEQ
                           FEATURE_ENHANCED_CELL_RESELECTION
                   Disabled FEATURE_INTER_SIB_SLEEP     
01/23/08   stk     Enabled FEATURE_STTD_OPTIMIZATION
                           FEATURE_WCDMA_CM_GFR_DBG
01/22/08   bd      Temp removed FEATURE_STTD_OPTIMIZATION for MSM7601
01/22/08   stk     Removed FEATURE_ADAPTIVE_STEP_SIZE_OLPC
01/15/08   stk     Enabled FEATURE_STTD_OPTIMIZATION (for CR 135262)
01/08/08   stk     Enabled FEATURE_WCDMA_MPR_BASED_BETA_BACKOFF
12/18/07   stk     Enabled FEATURE_WL1_NEW_SLEEP_API
12/11/07   stk     Enabled FEATURE_STMR_FRAME_REF_COUNTER_CONFIG_2
12/03/07   stk     Enabled FEATURE_ABORT_ESTB_ON_RA_LA_MISMATCH
11/26/07   stk     Enabled FEATURE_ADAPTIVE_STEP_SIZE_OLPC
11/19/07   stk     Enabled FEATURE_BUFFER_PREV_CFN_VALUES
11/14/07   stk     Enabled FEATURE_SUPPORT_FOR_PARTIAL_ROAMING
11/12/07   stk     Cleanup + Included all sub-features under the main feature flag
15/10/07   stk     Enabled FEATURE_WCDMA_RACH_DEBUG_LOGGING
                           FEATURE_WCDMA_SRCHACQ_RETRY
                           FEATURE_STAY_CAMPED_ON_ACCESS_CLASS_BARRED_CELL
08/10/07   stk	   Enabled FEATURE_WCDMA_FS_FALSE_PEAK_ELIM_OPT
                           FEATURE_LIMIT_TFCS_TO_ERAM_SIZE
                   Disabled 
                           FEATURE_WCDMA_EARLY_REL6_UE_IND
09/24/07   stk     Enabled FEATURE_WCDMA_HSUPA_CM 
09/17/07   stk     Removed FEATURE_WCDMA_TEMP_TX_TIMING_MSG (part of L1 VU 07)
                   Enabled FEATURE_RRC_DEFAULT_BAND_PRIORITY
                           FEATURE_RLC_ENH_BUFFER_STATUS
09/12/07   stk     Enabled FEATURE_FFE_CGC_WORKAROUND (part of L1 VU 06.01)
                   Removed FEATURE_FAIL_TB_ON_MULT_CRC_PASS (part of L1 VU 06.01)
                   Enabled FEATURE_RRC_CALLOC (part of RRC VU 72)
                           FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU (part of RRC VU 72)
09/04/07   stk     Enabled FEATURE_WCDMA_EQ_REFRAMING_INTVL
                   FEATURE_DISABLE_384K_ON_20MS_TTI_UE_CAPABILITY
08/30/07   stk     Enabled FEATURE_HSDPA_ASU_REPOINTING
08/30/07   bd      Disabled FEATURE_WCDMA_HSUPA_CM
08/13/07   stk     Enabled FEATURE_WCDMA_TEMP_TX_TIMING_MSG
07/24/07   bd      Enabled FEATURE_WCDMA_INCREASED_ACQ_STEP1_THRESH                  
                           FEATURE_RRC_TSIB_OPTIMIZATION
                           FEATURE_WCDMA_L1_DEC_BURST_READ
08/03/07   yh      Enabled FEATURE_WPLT_DSTS_IQ
07/16/07   stk     Enabled FEATURE_WCDMA_RX_DIVERSITY_PHASE_2
                   Disabled FEATURE_WCDMADIAG_INIT_IN_RRC
07/12/07   stk     Enabled FEATURE_WCDMADIAG_INIT_IN_RRC
07/01/07   stk     Enabled FEATURE_WCDMA_HSUPA_CM 
                           FEATURE_DEFAULT_CONFIG_IN_RB_SETUP
06/25/07   stk     Enabled FEATURE_WCDMA_RF_TUNE_TIME_REDUCTION
06/18/07   bs      Enabled FEATURE_MAC_HS_UL_LOW_GRANT_OPT
06/14/07   bd      In sync with #15 again
06/14/07   bd      Removed undef FEATURE_RF_TX_PWR_LIM_REV on top of rev#14
06/11/07   stk     Enabled FEATURE_RRC_ERROR_DETECT
                   Disabled FEATURE_WCDMA_ADAPT_PILOT_FILT
06/05/04   stk     Enabled FEATURE_WCDMA_USES_SHARED_SLP_CTLR_DRIVER
                           FEATURE_RRC_ADDL_CHNL_SCAN_OPTIMIZATION  
                           FEATURE_MANUAL_SEARCH_FREQ_SCAN_SKIP_TIMER
05/14/07   stk     Enabled FEATURE_WCDMA_HSUPA_SG_ACT_TIME
                           FEATURE_WCDMA_L1_TB_BASED_OLPC_FLOOR
                   + changed comment for FEATURE_WCDMA_HSDPA_LLR_LOG_PKT_V3
05/09/07   bd      Enabled CPU based flow control features
05/03/07   bd      Enabled FEATURE_WCDMA_HSDPA_LLR_LOG_PKT_V3
                           FEATURE_WCDMA_EQ_PLUS_RXD
                           FEATURE_WCDMA_EQ_DIV_PILOT
                           FEATURE_WCDMA_DPCH_BETA_SCALING
                           FEATURE_WCDMA_ADAPT_PILOT_FILT
                           FEATURE_WCDMA_L1_PSC_SCANNER
                           FEATURE_TFCS_SANITY_CHECK
                           FEATURE_RECONFIG_INDICATOR_CU
                           FEATURE_BYPASS_SYNCA_POST_VERI
                           FEATURE_RRC_STATIC_BUFFER_ALLOC
                   Removed FEATURE_WCDMA_HSDPA_LLR_LOG_PKT_V2
04/26/07   pr      Enabled FEATURE_WCDMA_RX_DIVERSITY_PHASE_1
04/17/07   pr      Enabled FEATURE_SKIP_SIBS_FOR_FPLMN
04/12/07   pr      Enabled FEATURE_WCDMA_CNT_C_SFN_DIFF
                   Disabled FEATURE_MDSP_AT_67MHZ        
04/06/07   bd      Enabled FEATURE_WCDMA_MULTIPLE_L3_FILTERS_SUPPORTED
                           FEATURE_FLEX_FREQ_BIN
                           FEATURE_MAC_HS_UL_QXDM_LOGGING
                           FEATURE_RRC_NO_MIB_FOUND_BAR_CELL
                           FEATURE_RRC_USES_LIBSTD
                           FEATURE_RRC_HEAP
                           FEATURE_WCDMA_EARLY_REL6_UE_IND
                           FEATURE_UPDATE_SIB7_IN_FACH
03/07/07   pr      Enabled FEATURE_IRAT_CELL_INFO_IND
                           FEATURE_STANDALONE_RBMAPPING 
02/21/07   pr      Enabled FEATURE_WCDMA_INTERF_MEAS_RES_OPT
                           FEATURE_INTER_FREQ_EM_CALL_REDIRECTION
02/08/07   bd      Disabled RxD feature
02/07/07   bd      In sync with MSM7200_UMTS_AS.00.00.12
12/08/06   bd      Enabled FEATURE_PRINT_MDSP_PC_ON_STATUS_DUMP_FAILURES
                           FEATURE_6280_HSDPA_PHASE2_CAPABILITY
                           FEATURE_RLC_ONE_SIDE_RE_ESTABLISHMENT
                           FEATURE_WL1_PE_BUFFERING
                           FEATURE_CLOSE_CN_DOMAIN_ON_RE_EST_TIMER_EXPIRY
                           FEATURE_RRC_HARD_ABORT
                   Removed FEATURE_FORCE_CS_RELEASE_T314_EXPIRY
                   Removed/Mainlined
                           FEATURE_WCDMA_HSUPA_ETFCI_0
                           FEATURE_WCDMA_HSUPA_NO_CIPHER
                           FEATURE_WCDMA_HSUPA_TURBO_CODE_SEG_FIX
                           FEATURE_WCDMA_INTRAF_SUPPORT_IN_FACH
                           FEATURE_WCDMA_INTRAF_SUPPORT_IN_CM
                           FEATURE_STTD_CAPABLE
                           FEATURE_STTD_ENABLED_BY_DEFAULT
                           FEATURE_WCDMA_NO_MEAS_RESET_DURING_SHO
                           FEATURE_INTRA_FREQUENCY_HARD_HANDOVER
11/29/06   bd      Enabled FEATURE_SRCHCM_DM_DELAY_CHECK again (cm ifreq meas issue
                   was related to dmov header file)
11/16/06   bd      Enabled FEATURE_WCDMA_UL_TFC_PL_REV_2
                   Disabled FEATURE_SRCHCM_DM_DELAY_CHECK for now (on 3.x)
11/07/06   bd      Enabled FEATURE_WL1_WITH_CHARGER_WAKE_UP_SOONER
                   Removed/Mainlined FEATURE_WCDMA_PATHLOSS
11/02/06   bd      Enabled FEATURE_WCDMA_EQ_SPD_ADAPT
                           FEATURE_SRCHCM_DM_DELAY_CHECK
                   Removed FEATURE_PMIC
11/01/06   bd      Enabled FEATURE_WCDMA_EQ_PLUS_CLTD
                           FEATURE_WCDMA_EQ_CONTROLLER
                           FEATURE_WCDMA_DEFAULT_RXDIV_ENABLED_FOR_DATA_CALLS
                           FEATURE_WCDMA_RXD_SUPPORT_CM
10/20/06   rt      uncomment FEATURE_RF_TX_PWR_LIM_REV
10/12/06   bd      Enabled FEATURE_UI_RF_ENABLED_BANDS_SUPPORT
                           FEATURE_FORCE_CS_RELEASE_T314_EXPIRY
                           FEATURE_CM_GSM_UNLOCK_BSIC_ID_CELL
                           FEATURE_CM_GSM_RSSI_REPORT_TIMER
10/05/06   bd      In sync with //depot/asic/msm7200/2.x/products/7200/build/cust/custwcdma.h#4
09/25/06   bd      Enabled FEATURE_WL1_ENHANCED_LOCAL_CMD_BUF_SIZE
                   Enabled new log packet features
08/31/06   bd      Enabled EUL related features - for the EUL team
08/31/06   bd      Commented out RxDiv/Eq related featurs for now
08/28/06   bd      Enabled RxDiv/Eq related features
08/16/06   bd      Enabled FEATURE_WCDMA_HS_NO_REF_FING_ALLOWED
                           FEATURE_WCDMA_L1_FACH_FORBID_KNOWN_CELL_OFFSET_IN_SAME_ARFCN
                           FEATURE_REL6_HCS_RESEL_USE_H
08/03/06   stk     Removed FEATURE_3SLICE_REACQ_SEARCH 
07/25/06   bbd     Sync to //depot/asic/msm6280/4.0E/build/cust/custwcdma.h#15
07/06/06   bd      Sync to //depot/asic/msm6280/build/cust/custwcdma.h#36
11/03/05   ttl     Enable FEATURE_RRC_CU_IDT_ESTCAUSE.
                   Enable FEATURE_BUCKSHOT_SCCPCH_FINGERS
                   Enable FEATURE_2G_RANK_Q_OFFSET_OPT
                   Enable FEATURE_IGNORE_TRESEL_ON_REACQ_FAIL
                   Enable FEATURE_FACH_IGNORE_S_INTRA_FOR_RESEL
                   Enable FEATURE_IGNORE_PENALTY_AT_LOW_CPICH_ECNO
                   Enable FEATURE_IGNORE_HCS_ON_S_OR_REACQ_FAIL
                   Move FEATURE_RELEASE_5_RLC under FEATURE_REL5.
10/14/05   ttl     Merge from Patch 3.0 release.
                   Enable FEATURE_TIMER_BASED_SDU_DISCARD.
10/12/05   ttl     Enable FEATURE_ENABLE_12_FINGERS (for 3.2 only)
                   Enable FEATURE_FING_MGR (for 3.2 only)
                   Enable SRCHCM_INTERF_ABORT_RECONF_SRCH_AT_CLEANUP.
                   Enable FEATURE_WCDMA_L1_TA_INFO_LOG_CODE_4006.
10/06/05   ttl     Disable FEATURE_WCDMA_PCCPCH_RESLAM.
09/27/05   ttl     Enable FEATURE_WCDMA_LOG_FILTERING under FEATURE_REL5.
                   Remove FEATURE_HW_FING_MERGE_PROTECTION_EN
                   Enable FEATURE_WCDMA_PCCPCH_RESLAM
                   Enable FEATURE_CIPHERING_SUPPORT_FOR_CR2272
                   Enable FEATURE_PESSIMISTIC_CQI.
09/21/05   ttl     Enable FEATURE_MODEM_SLEEP_CONTROLLER_INSTANCE_1
                   Remove FEATURE_ENABLE_8_FINGERS (mainlined)
09/14/05   ttl     Enable FEATURE_DL_OLPC_FAST_UP_STEP.
                   Enable FEATURE_WCDMA_L1_DETECTED_CELLS_DCH.
                   Enable FEATURE_L1_DETECTED_CELLS_IDLE.
                   Remove FEATURE_WCDMA_L1_IGNORE_CM_ACT_TIME_ADJ. (mainlined)
09/09/05   ttl     Disable FEATURE_PESSIMISTIC_CQI.
                   Enable FEATURE_PIGGYBACK_RESEL.
09/01/05   ttl     Enable FEATURE_PESSIMISTIC_CQI
08/31/05   ttl     Enable FEATURE_RRC_ENABLE_CM_HSDPA.
                   Enable FEATURE_HSDPA_PLUS_CM.
08/29/05   ttl     Enable FEATURE_CLTD_LOGGING.
                   Enable FEATURE_CM_GSM_SEARCH_GAP_USE_OPTIMIZE_STATISTICS.
08/22/05   ttl     Enable FEATURE_CM_GSM_SEARCH_GAP_USE_OPTIMIZE.
08/15/05   ttl     Remove FEATURE_WCDMA_L1_CM_SUSPEND_RESUME_SUPPORTED for this
                   is an L1 feature only.
                   Remove FEATURE_RF_DISABLE_LDO_IN_SLEEP for defined in custrf.h.
                   Enable FEATURE_RF_WAKE_UP_OPTIMIZED. 
08/12/05   ttl     Move FEATURE_REL5 under FEATURE_HSDPA_ACCESS_STRATUM
08/09/05   ttl     Disable FEATURE_HSDPA_PLUS_CM.
                   Disable FEATURE_WCDMA_HFN_UPDATE.
                   Enable FEATURE_LIST_LOG_413F.
08/01/05   ttl     Enable FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER.
                   Enable FEATURE_HSDPA_PLUS_CM. 
                   Enable FEATURE_FREQ_ERROR_AVERAGING
                   Enable FEATURE_WCDMA_L1_MANUAL_CQI_UPD
                   Enable FEATURE_REL5
07/25/05   ttl     Enable FEATURE_RLC_REPORT_VR_H_ACK_SN.
                   Enable FEATURE_TMC_TCXOMGR.
07/21/05   ttl     Fix errors.
07/20/05   ttl     Enable FEATURE_HHO_FAILURE_HANDLING.
07/19/05   ttl     Obsolete FEATURE_SRCH_GRANULARITY_WORKAROUND.
                   Enable FEATURE_OUT_OF_SERVICE_ENHANCEMENT.
07/18/05   ttl     Enable FEATURE_FFE_CGC_WORKAROUND.
                   Enable FEATURE_MIN_INIT_TX_PWR.
                   Enable FEATURE_UMTS_BMC_SLEEP_OPT.
                   Remove FEATURE_DISABLE_TRIAGE_MERGE_DEASSIGN .
                   Enable FEATURE_SRCH_GRANULARITY_WORKAROUND.
07/11/05   ttl     Disable FEATURE_RLC_REPORT_VR_H_ACK_SN.
07/08/05   ttl     Enable FEATURE_SRCH_TIMING_ADJ.
                   Obsolete FEATURE_SRCH_GRANULARITY_WORKAROUND.
07/07/05   ttl     Enable FEATURE_FACH_FREQ_REDIRECTION_IN_CONN_SETUP.
07/06/05   ttl     Enable FEATURE_RLC_REPORT_VR_H_ACK_SN, 
                   Enable FEATURE_RELEASE_5_RLC when FEATURE_HSDPA is on.
                   Remove FEATURE_TWO_PARAM_ASYNC_RX_METRICS since feature name
                   chnaged.
                   Remove dup of FEATURE_DISABLE_TRIAGE_MERGE_DEASSIGN and correct
                   its comments.
07/01/05   ttl     Define FEATURE_L1_POWER_CTR_PKT_WINDUP_MODE.
06/16/05   ttl     Enable FEATURE_WCDMA_NO_MEAS_RESET_DURING_SHO, 
                   FEATURE_RF_TX_PWR_LIM_REV, FEATURE_FILTER_INTERF_MEASUREMENTS,
                   FEATURE_DISABLE_TRIAGE_MERGE_DEASSIGN, 
                   FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING.
06/13/05   ttl     Enable FEATURE_DISABLE_TRIAGE_MERGE_DEASSIGN.
06/10/05   ttl     Enable FEATURE_FTL_INIT_ON_FIRST_FINGER_ONLY.
06/08/05   ttl     Enable FEATURE_UMTS_BMC, FEATURE_UMTS_BMC_DRX_SUPPORTED,
                   FEATURE_MULTIPLE_SCCPCH_SUPPORTED for BMC.
                   Enable FEATURE_SPEC_CR2393.
06/06/06   ttl     Back off FEATURE_HHO_FAILURE_HANDLING.
06/06/05   ttl     Enable FEATURE_ROLLBACK_SECURITY_CONFIGURATION. Enable
                   FEATURE_HHO_FAILURE_HANDLING.
06/01/05   sgk     Enable FEATURE_WCDMA_EXTEND_REX_TICKS_WITH_ONLY_TCXO
                   Disable FEATURE_CELL_RESELECTION_DUAL_SYS_LOG
                   FEATURE_SRCH_HB_IN_DRX
                   FEATURE_MULT_NBR_FING_TRIAGE
                   FEATURE_MULTIPLE_NGHBR_FINGS
                   FEATURE_FINGER_REUSE
                   FEATURE_FING_MERGE_PROTECTION
                   FEATURE_FING_STANDOFF_DETECTION
                   FEATURE_REF_FINGER_PROTECTION
05/25/05   sgk     Enable FEATURE_PRIORITIZE_STEP1_PEAKS_OUTSIDE_256_CHIPS
                          FEATURE_MAINTAIN_GTIME_THROUGH_WSLEEP
                   Disable FEATURE_CLTD_LOGGING
05/25/05   src     Enable FEATURE_WCDMA_AGC_LOG_PKT_0x4165 for rev-2 AGC log.
05/24/05   src/gs  Enable FEATURE_WCDMA_TX_BETA_GAIN_TABLE_VER2.
05/14/05   ttl     Enable FEATURE_WCDMA_PWR_TFC_ELMN for FEATURE_6275_COMPILE.
05/13/05   ttl     Enable FEATURE_WCDMA_TX_PWR_BACK_OFF
                   Enable FEATURE_WCDMA_UL_TFC_PL_REV_6275.
05/13/05   ttl     Enable FEATURE_QXT, FEATURE_NCP, FEATURE_3SLICE_REACQ_SEARCH.
                   Enable FEATURE_RRC_RAVEN_F9DRIVER under FEATURE_6275_COMPILE.
                   Enable FEATURE_CLTD_DPCH_EST_CRITERIA_BYPASS,
                   FEATURE_CLTD_LOGGING, FEATURE_FAIL_TB_ON_MULT_CRC_PASS for 
                   6275 for CLTD.
05/09/05   ttl     Disable FEATURE_WCDMA_L1_MDSP_CMD_LOG
05/06/05   ttl     Enable FEATURE_WCDMA_L1_MDSP_CMD_LOG
                   Enable FEATURE_T_BARRED_SUPPORT
05/04/05   ttl     Enable FEATURE_OPTIMIZE_SEARCH_FOR_HHO 
                   Enable FEATURE_WCDMA_HFN_UPDATE        
05/02/05   ttl     Define FEATURE_HS_LOG_DECODE_STATUS_W_DATA.
                   Define FEATURE_CELL_SELECTION_PERIODIC.
                   Remove FEATURE_L1_CELL_SELECTION.
04/28/05   ttl     Define FEATURE_FINGER_NUDGING under FEATURE_6275_COMPILE.
04/22/05   ttl     Fix a typo.
04/22/05   ttl     Enable FEATURE_ENABLE_8_FINGERS, 
                   remove FEATURE_FING_PERF_ENHANCE, 
                   undefine FEATURE_FULL_STTD_SUPPORT for Raven and
                   enable FEATURE_TRANSMIT_DIVERSITY_SUPPORT.
04/18/05   ttl     Define FEATURE_WCDMA_HCS, FEATURE_RRC_RELEASE_OPT,
                   FEATURE_INTER_FREQ_REDIRECTION, FEATURE_SIB18,
                   FEATURE_WCDMA_QUALITY_MEAS.
04/01/05   ttl     Remove FEATURE_WCDMA_INCLUDE_SERV_FREQ_IN_INTERF_MRM.
03/31/05   ttl     Define couple features.
03/22/05   ttl     Define FEATURE_RRC_ASN1_REL5.
03/17/05   sr      Enabled FEATURE_WSLEEP_TRIAGE_PICH_GAP_REDUCTION and 
                   FEATURE_WSLEEP_TRIAGE_PICH_GAP_REDUCTION_TCXO_SHUTDOWN
03/09/05   src     Replaced instances of the feature MSMHW_SLEEP_CTRL_REV_6275
                   with feature FEATURE_MODEM_SLEEP_CONTROLLER_INSTANCE_1.
02/19/05   src     Removed FEATURE_RF_DISABLE_LDO_IN_SLEEP.
02/19/05   src     Enabled FEATURE_TWO_PARAM_ASYNC_RX_METRICS.
02/19/05   src     Enabled FEATURE_SRCH_GRANULARITY_WORKAROUND.
02/09/05   src     Enabled hardware-based transparent-mode deciphering and
                   brought in additional inter-RAT features.
01/25/05   src     Added 6.0 features and undefined FEATURE_WCDMA_PWR_TFC_ELMN.
01/21/05   ttl     Removed FEATURE_RRC_INTEGRITY_SW_ENGINE.
01/21/05   src     Deleted FEATURE_HSDPA_UL_TEST_MODE_ENABLED enabling.
01/10/05   src     Enabled FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING for non-HS
                   cases, subject to future modification.
01/10/05   ttl     Enabled Multi-Fingers feature. 
01/08/05   src     Temporarily enabled FEATURE_NO_MULTIPLE_FINGERS for the new
                   target feature FEATURE_HSDPA_ACCESS_STRATUM. Also, mainlined
                   all other HSDPA-related features by putting them under this
                   new target feature.
01/05/05   src     Disabled FEATURE_NO_MULTIPLE_FINGERS.
12/14/04   src     Enabled FEATURE_DEBUG_UNRESPONSIVE_MDSP.
12/14/04   src     Enabled FEATURE_MDSP_CMD_TRACE_DEBUG.
12/03/04   src     Restricted operation to single fingers.
12/03/04   src     Enabled eight-finger operaton on a trial basis.
11/05/04   src     Added MSM6275-specific features.
09/23/04   bu      Defined FEATURE_STEP1_PEAK_ELIMINATION. 
09/22/04   bu      Undefined FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING.
09/22/04   bu      Defined FEATURE_OUT_OF_SERVICE_AREA.
09/14/04   bu      Defined FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING.
09/07/04   bu      Defined FEATURE_PCCPCH_SETUP_DECODE_ENHANCEMENT and
                   FEATURE_IDLE_MODE_PCCPCH_SETUP_DECODE_ENHANCEMENT. 
08/31/04   bu      Defined FEATURE_3SLICE_REACQ_SEARCH and FEATURE_SUB_10_AWAKE_TIME.
08/27/04   bu      Defined FEATURE_TRANSMIT_DIVERSITY_SUPPORT. Undefined
                   FEATURE_FULL_STTD_SUPPORT. Defined FEATURE_CELL_RESELECTION_RANK_OPT.
08/25/04   bu      Defined FEATURE_FREQ_INFO_IN_CONNECTION_RELEASE and
                   FEATURE_FULL_STTD_SUPPORT.
08/23/04   bu      Defined FEATURE_WCDMA_PATHLOSS.
08/20/04   bu      Defined FEATURE_SIB_SCHEDULE and FEATURE_WCDMA_RX_TX.
08/20/04   bu      Defined DEBUG_RACH_RAM_LOG. 
08/19/04   bu      Undefined FEATURE_MSM6250_ERAMA_HW_BUG_SW_WORKAROUND.
08/09/04   ttl     Define FEATURE_REF_FINGER_PROTECTION and FEATURE_FING_STANDOFF_DETECTION
07/23/04   ttl     Added FEATURE_WCDMA_L1_BSIC_ID_FORBID_TIMER_AFTER_F_RECONF_ABORT.
07/22/04   ttl     Added FEATURE_WCDMA_RESUME_CIPHERING_SUPPORT.
07/20/04   ttl     Added FEATURE_DL_OLPC_ACQ_MODE_NO_DOWN_STEP_0_BLK_SZ.
07/19/04   ttl     Added FEATURE_INTRA_FREQUENCY_HARD_HANDOVER, 
                   FEATURE_TFCS_CONTROL_ENABLE.
07/16/04   ttl     Define FEATURE_NO_UL_SLAM_ON_RESUME.
06/20/04   src     Defined FEATURE_MDSP_HAS_AWAKE_STATE_VAR.
06/14/04   ttl     Removed FEATURE_CM_GSM_RSSI_MEAS_PER_GAP_LIMIT.
06/10/04   ttl     Added FEATURE_CAGC, FEATURE_CELL_FACH_RESEL_CHK_THRESH
06/10/04   ttl     Added FEATURE_SET_FING_TTL_BETA, FEATURE_FREQ_SCAN_OPTIMIZATION.
06/08/04   ttl     Added FEATURE_INTER_FREQUENCY_CELL_FACH_RE_SELECTION,
                   FEATURE_STTD_CAPABLE, FEATURE_STTD_ENABLED_BY_DEFAULT, 
                   FEATURE_BUTTERFLY_CORRELATION, FEATURE_SET_HW_CPP_PF_GAINS          
                   FEATURE_HW_FING_MERGE_PROTECTION_EN,
                   FEATURE_ENABLE_8_FINGERS.
06/07/04   ttl     Define FEATURE_RRC_SIB_HEAP.
06/06/04   ttl     Added FEATURE_REDUCE_TRI_PI_GAP_IN_GOOD_DL.
06/04/04   ttl     Added FEATURE_WCDMA_PWR_TFC_ELMN.
06/01/04   ttl     Added FACH_DCH_TRANSITION_DEBUG.
05/19/04   mks     Enabled FEATURE_WCDMA_L1_FORCE_RECONF_ABORT_SKIP_FIRST_BSIC_ID_CELL
05/14/04   ttl     Remove FEATURE_INTER_FREQUENCY_CELL_FACH_RE_SELECTION.
05/13/04   ttl     define FEATURE_INTER_FREQUENCY_CELL_FACH_RE_SELECTION.
05/12/04   ttl     Define FEATURE_RRC_STATIC_AMR, undef FEATURE_MVS_RRC,
                   and undef FEATURE_L1_CM_TGL2_NO_USE_FOR_GSM.
05/03/04   ttl     Added FEATURE_QUICK_BCH_HYP_TESTING, FEATURE_SRCH_HB_IN_DRX,
                   FEATURE_MULTIPLE_NGHBR_FINGS, FEATURE_MULT_NBR_FING_TRIAGE.
04/28/04   ttl     Undefine FEATURE_L1_SUBSTITUTE_EVENT_3C_FOR_EVENT_3A
04/26/04   ttl     Moved all WCDMA related feature definitions from custwcdma.h
===========================================================================*/

  #define FEATURE_WCDMA_DAL_INT_CONTROLLER

  /* FR2018: Modem Clock Voltage Scaling */
  #define FEATURE_WCDMA_MCVS


#ifdef FEATURE_6280_COMPILE
  #define FEATURE_WCDMA_MDSP_INTERFACE_REV_6280
  #define MSMHW_DEC_REV_6280
#endif

#define FEATURE_WCDMA_MDSP_INTERFACE_REV_6275
#define MSMHW_DEC_REV_6275


/*---------------------------------------------------------------------------
                              BOLT RUMI FLAGS
---------------------------------------------------------------------------*/
#ifdef FEATURE_BOLT_MODEM
   #define FEATURE_WCDMA_BOLT_RUMI_HWIO_UPLINK
   #define FEATURE_WCDMA_BOLT_RUMI_HWIO_IDLE_MODE
   #define FEATURE_WCDMA_BOLT_RUMI_HWIO_DEMOD
   #define FEATURE_WCDMA_BOLT_RUMI_HWIO_DATAPATH
   #define FEATURE_WCDMA_BOLT_RUMI_HWIO_IRAT
   #define FEATURE_WCDMA_BOLT_RUMI_HWIO_SEARCHER
   #define FEATURE_WCDMA_BOLT_RUMI_HWIO_PLT
#endif /*FEATURE_BOLT_MODEM*/

/* Feature to support UL Compression at the RLC SDU level - Proprietary feature - Anju */
#define FEATURE_WCDMA_UL_COMPR

/*---------------------------------------------------------------------------
                              WCDMA FLAGS
---------------------------------------------------------------------------*/
/* Enable logging for AMR UL,DL Vocoder packets */
#define FEATURE_GSM_WCDMA_AMR_PKT_LOG_DSP

/* Bolt2.0 Specific W flag */
#define FEATURE_WCDMA_BOLT_2_0 

/* Jolokia and Tabasco Specific W flag */
#ifdef FEATURE_JOLOKIA_MODEM
  #define FEATURE_WCDMA_JOLOKIA_MODEM
#endif

#ifdef FEATURE_TABASCO_MODEM
  #define FEATURE_WCDMA_JOLOKIA_MODEM
  #define FEATURE_WCDMA_TABASCO_MODEM
#endif


/*---------------------------------------------------------------------------
                              WCDMA L1 FLAGS
---------------------------------------------------------------------------*/

#define FEATURE_WCDMA_RRC_HEAP_TO_BSS 
#define FEATURE_WCDMA_ENABLING_QICE_WITH_CM 

// --- WCDMA ---
#undef FEATURE_SPIKE_MITIGATION

/* forces "timing init"  even if "timing maintained" is signalled */
#undef FEATURE_FORCE_IFHHO_TIMING_INIT


/* Used to hardcode uplink power parameters because there is not RF calibration. */
#undef FEATURE_USE_INTERNAL_RACH_PARMS

/* Enable InterRAT Cell Reselection */

/* Enable to freeze TPC accumulation upon reconfig to mitigate TX power spike
   This feature should be turned on for DPCH drop and add only when 
   FEATURE_WCDMA_IGNORE_DCH_CHAN_ESTAB_DCH_DROP_ADD is enabled. */
#undef FEATURE_UL_TPC_ON_DPCH_RECONFIG

/* Enabling feature ignores phychan establishment procedure during reconfig */
#undef FEATURE_WCDMA_IGNORE_DCH_CHAN_ESTAB_DCH_DROP_ADD


#define FEATURE_SHO_TPC_THRESH

/* Support for FTM run-time mode change */
#undef FEATURE_WCDMA_SUPPORT_DEACT_REASON

/* Enable support for compressed mode by HLS */

/* Enable UL TFCS restriction if and only if compressed mode by HLS is enabled */
  #define FEATURE_TFCS_CM_BY_HLS

/* Enable UE from getting into powersave/deepsleep mode when FTM accesses are in progress. */
#undef  FEATURE_MODE_CHANGE_FOR_FTM_FIX
                
#undef FEATURE_MSM6250_ERAMA_HW_BUG_SW_WORKAROUND
#undef FEATURE_L1_SUBSTITUTE_EVENT_3C_FOR_EVENT_3A
#undef FEATURE_WCDMA_L1_OLPC_FLOOR_UPDATE_BTFD_CM
#undef FEATURE_LIMIT_BSIC_ID_TO_FIRST_CELL_IN_LIST
#undef FEATURE_WCDMA_L1_GSM_ALLOW_FLEX_UNCERTAINTY_WIN
#undef FEATURE_L1_CM_TGL2_NO_USE_FOR_GSM
#undef FEATURE_SET_HW_CPP_PF_GAINS

#define FEATURE_CELL_FACH_RESEL_CHK_THRESH

/* Full STTD support */
#undef FEATURE_FULL_STTD_SUPPORT

#define FEATURE_NCP
/* For GCF - read NV item at L1 task bringup to decide whether UE should 
   enable/disable field performance optimizations. */
#define FEATURE_WL1_FLD_PERF_OPT_FROM_NV
  
/* Revised CLTD log packet contents for MAP-AV algorithm */
#define FEATURE_WCDMA_CLTD_LOG_CODE_VER2

/* HSDPA related features */
#ifdef FEATURE_HSDPA_ACCESS_STRATUM
  #define FEATURE_WCDMA_MDSP_INTERFACE_REV_6275
  #define MSMHW_DEC_REV_6275
#endif /* #ifdef FEATURE_HSDPA_ACCESS_STRATUM */

/* Enables finger FEE CGC in code */
#undef FEATURE_FFE_CGC_WORKAROUND   // should be taken off in 7600

/* Enables sleep optimization during BMC */
#define FEATURE_UMTS_BMC_SLEEP_OPT


/* For Rel-5 SCCPCH enhancements backfilled into Saber per Systems recommendations */
#define FEATURE_BUCKSHOT_SCCPCH_FINGERS

/* In low serving cell conditions, ignore RACH and perform inter-RAT/freq searches 
   using FACH measurement occasions */
#define FEATURE_PRIORITIZE_FMO_OVER_RACH_IN_LOW_S_RXAGC

/* Implementation of Rel 5 CR 130R1 and CR 135R1 */
#define FEATURE_REL5_25304_135R1


/* Rx Diversity features for Phase 1*/

/* Rx Diversity features for Phase 2 */
  /* Measurement optimizations for RX-D */
  #define FEATURE_WCDMA_RXD_MEAS_OPT 
  /* MMSE Algorithm support for RX-D */

/* Equalizer related features (must follow RxD section) */

/* Fixes Rel-6 CR-133 and 146. Also backward compatible. When UE is in high mobility UE should 
   use H criteria and Select the correct priority layer for evaluating cells for reselection. */
#define FEATURE_REL6_HCS_RESEL_USE_H

/* Support for PSC scan search during DRX */
#undef FEATURE_WCDMA_L1_PSC_SCANNER

/* Added support for calculating, updating and reporting the count_C_SFN_high 
   value in the MRM.*/
#undef FEATURE_WCDMA_CNT_C_SFN_DIFF

/* New DL features */
#ifdef FEATURE_WCDMA_DL_ENHANCED
  #define FEATURE_MCAL_WCDMA_SEQ
#endif

#undef FEATURE_MODEM_MBMS
#ifdef FEATURE_MODEM_MBMS
  #error code not present
#endif

/* To enable sw fw sync */
#define FEATURE_WCDMA_DEMOD_FINGER_LOCK_THRESH_27
#define FEATURE_WCDMA_SYSTEM_TIMER_REV2
/** TCXOMGR version 3.0 **/
/** TCXO manager 3.0 Combines 1x, GSM, WCDMA, SRM and TCXO manager into a single controlling entity **/


/* Feature to enable the latest SW-FW searcher i\f on Mustang where
   PN antenna sel progamming is moved from ASYNC to SYNC params */
#define FEATURE_MCAL_SRCH_ANTSEL_SYNC_PARAMS
#undef FEATURE_TPC_IN_SHO
#define FEATURE_HSDPA_UL_TEST_MODE_ENABLED
/* This feature when enabled do registration and deregistration of HSDPA as
   a resource for ARM clock management. */   
/* Not required for MSM6280 */
#undef FEATURE_SRCH_TIMING_ADJ

/* This feature supports the modification of some global variables (DPCCH Init Pwr Agc,
   enable/disable HDET, enable/disable UL Log Packets etc) through QXDM in R99. */

#define RUMI_8220_BRINGUP

#define FEATURE_WCDMA_DEBUG_ATTEMPT_FULL_SRCH
#undef FEATURE_WCDMA_OFFLINE_MODEM_IRAT_PHASE1

/* QICE related features */
#define FEATURE_WCDMA_QICE_DC_CONTROLLER


/* Dynamic Memory allocation from Modem Heap */
#ifdef FEATURE_MODEM_HEAP
  #define FEATURE_WCDMA_L1_ZI_OPT
#endif


#define FEATURE_WCDMA_FULL_SRCH_IN_DC

#ifdef FEATURE_MCPM
  #define FEATURE_WCDMA_MCPM_DRIVER
#endif  /* FEATURE_MCPM */

/* Diag Support for querying Network cell info via QXDM */
#define FEATURE_WCDMA_DIAG_QUERY_CELL_INFO

/* Fix for HW data corruption */

/* Fix for simultaneous R99 UL- EUL HW encoding issue */
#define FEATURE_WCDMA_CONCURRENT_R99_EUL_TX_ENCODE

/* Workaround for nikel ul compressed mode  */
#define FEATURE_WCDMA_UL_CM_INTLV_WORKAROUND

/* RXD support during non traffic */

/* DSR Support*/

#define FEATURE_WCDMA_EARLY_RF_SLEEP




#define FEATURE_WCDMA_CPC_DTX
#define FEATURE_WCDMA_CPC_DRX
#define FEATURE_CPC_DTX_HS_SCCH_REVERT_HHO_RECOVERY

#ifdef FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY
  #define FEATURE_WCDMA_ANT_SWITCH_SUPPORT
#endif

/* As part of this feature L2 will indicate to L1 when to start/stop rach procedure. 
   This will avoid L1 to query L2 in every frame if there is data to be sent on PRACH,
   thus improving performance. */
#define FEATURE_WCDMA_TRIGGER_BASED_RACH

#define FEATURE_WCDMA_HS_FACH

#ifdef FEATURE_WCDMA_MIMO
  #define FEATURE_WCDMA_SCPICH_MIMO_MODE_PRESENT
  #define FEATURE_WCDMA_MIMO_CODEBOOK_RESTRICTION
  #define FEATURE_WCDMA_TX_DIVERSITY_DL_FOR_MIMO
  #define FEATURE_WCDMA_QICE_PLUS_MIMO
#endif

/* This feature enables Rel-8 HS-RACH */
#define FEATURE_WCDMA_HS_RACH

#define FEATURE_WCDMA_HS_FACH_DRX

/* Support for inter frequency PN searches on frozen/offline samples */
#define FEATURE_WCDMA_INTERF_OFFLINE_PN

/* W2L reselection failure because of LTE cell search not happening in W after a W2L reselection failure */
#define FEATURE_WCDMA_W2L_FORCED_DET_ON_RESEL_FAIL

#ifdef FEATURE_CUST_1
  /* Decoder Feature */
  #define FEATURE_WCDMA_RXD_LOCKMODE_CHANGE_FOR_ANT_COUPLING
  #define FEATURE_WCDMA_CUST_RXDPM_THRESHOLD
#else
  /* Idle mode Feature */
  #define FEATURE_WCDMA_ENH_OOS_RXD_PWC
  /* Uplink Features */
  #define FEATURE_WCDMA_MTPL_THERMAL_BACKOFF
  #define FEATURE_TEMPERATURE_FC
#endif


/* Enable the UE to camp on the cells which are not reserved for operator use in the same frequency as the operator reserved cell.*/
#define FEATURE_WCDMA_UNBAR_OPRTR_RSVD_FREQ

#ifndef TEST_FRAMEWORK
/* Support free floating task with WCDMA AS */
#define FEATURE_WCDMA_FREE_FLOATING_TASK
#else 
#error code not present
#endif

/* API to expose EUL throughput stats to DataServices */
#define FEATURE_WCDMA_MODEM_TPUT_STATS_API

/* FR2334: DSDA Co-Existence Manager */
#if (defined(FEATURE_MODEM_COEXISTENCE_SW) && defined(FEATURE_MODEM_COEXISTENCE_FW)) && (defined(FEATURE_QTA) || defined(FEATURE_DUAL_ACTIVE))
#define FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW
#endif

/* Freq reporting to MCS for DSDA, DDR and SAR FR's */
#if defined (FEATURE_MODEM_COEXISTENCE_SW) && ( \
     defined(FEATURE_WCDMA_MULTISIM_COEXISTENCE_SW) || \
     defined(FEATURE_WCDMA_DIME_TX_STATE_REPORTING) || \
     defined(FEATURE_WCDMA_SAR_TX_PWR_FILTER) || \
     defined(FEATURE_MODEM_DDR_MGMT) \
    )
#define FEATURE_WCDMA_COEXISTENCE_SW
#endif

#define FEATURE_WCDMA_SAR_TX_PWR_FILTER

/* HS only TPC is a feature targeted to improve HS throughput when HS cell imbalance is detected */
#define FEATURE_WCDMA_HS_ONLY_TPC

/* Support for excluding deteceted cells for measurement when in Idle state */
#define FEATURE_WCDMA_EXCLUDE_USET_CELLS_FOR_MEAS

/* A Release 7 feature, it allows higher E-DPCCH gains for E-TFCIs above a certain value, in order to provide enhanced phase reference for larger E-TFCIs */
#define FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST

/* A Release 7 feature together with FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST because the Release 6 MPR will not suitable for FEATURE_WCDMA_REL7_EDPCCH_POWER_BOOST */
#define FEATURE_WCDMA_REL7P1_MPR

/* CIR interpolation, Estimation enhancements and MAC UL ZI Reduction enabled only on JO */

#define FEATURE_WCDMA_CIR_ENHANCEMENT
#define FEATURE_WCDMA_MAC_UL_ZI_REDUCTION
#define FEATURE_WCDMA_ZI_OPT_PHASE2
#define FEATURE_WCDMA_L1_SPLIT_ACQ_ACROSS_BPLMN_CYCLES
#define FEATURE_WCDMA_REACQLESS_PICH
#define FEATURE_WCDMA_PANIC_SRCH_OPT

#define FEATURE_WCDMA_MDSP_RECOVERY

#define FEATURE_WCDMA_STEP1_PEAK_PRUNER

/*---------------------------------------------------------------------------
                              WCDMA L2 FLAGS
---------------------------------------------------------------------------*/

// --- WCDMA ---
/* Define this feature to enable RLC logging. */
#define FEATURE_RLC_QXDM_LOGGING

/* ASC selection is used to identify an appropriate Acess Service Class.(ASC)
   L1 will use the PRACH partition associated with this ASC to transmit data. */

/* HSDPA related features */
#ifdef FEATURE_HSDPA_ACCESS_STRATUM
#define FEATURE_MAC_HS_QXDM_LOGGING
#endif

/* DSM usage optimization by removeing the DSM DUP item using in the UL AM PDU building */

#ifdef FEATURE_MODEM_HEAP
#endif

#if (defined(FEATURE_WCDMA_DL_DATA_PATH_OPT) && defined(FEATURE_WCDMA_UL_DATA_PATH_OPT))
  #define FEATURE_WCDMA_RLC_TIMER_OPT
#endif

#define FEATURE_MAC_I

#define FEATURE_UM_PDU_LOGGING

#ifdef FEATURE_CUST_1 
  #define FEATURE_WCDMA_MRAB_TVM_MAC_OPT
  #define FEATURE_WCDMA_AM_IGNORE_MAXRST
  #define FEATURE_WCDMA_SRB_PRIO_READJ
  #define FEATURE_RLC_MISSING_PDU_REPEAT_STATUS
#endif

/* Enqueue the SDU back into Watermark during RESET in case of SRB, to improve the call performance */
#define FEATURE_WCDMA_SRB_RLC_SDU_RESTORE

/*---------------------------------------------------------------------------
                              RRC FLAGS
---------------------------------------------------------------------------*/

// --- WCDMA ---
/* Define this feature when linking with the LED (lean encoder/decoder) OSS
** library. Undefine to link with the SOED (space-optimized encoder/decoder)
** library.
*/
#ifndef WIN32
#endif

/* Define the OSS_USE_FULL_SOED_ONLY when not linking with LED
*/
  #undef OSS_USE_FULL_SOED_ONLY

/* Enable the 5 second delay prior to transmitting complete messages
   on new Radio links to allow the Anritsu 8480 + PTS time to reconfig. */
#undef FEATURE_RRC_DELAY_TX_ON_NEW_CFG

/* Definition to enable SIngle Operation for each CPHY_SETUP_REQ */
#undef FEATURE_RRC_SINGLE_OP_PER_SETUP_REQ

/* Define this to enable RRC Activation Time. */

/* Define this to enable UE to drop and add Uplink DPCH instead of reconfiguring. */
#undef FEATURE_DROP_ADD_FOR_PHYCHL_RECONFIG

#undef FEATURE_MVS_RRC

#ifndef FEATURE_MODEM_HEAP
  /* Changes have been made for allocating memory, from the static buffer for 
     the internal RRC buffer commands and the LLC commands in RRC */
  #define FEATURE_RRC_STATIC_BUFFER_ALLOC
#endif

#ifndef FEATURE_MODEM_HEAP
  /* When this feature is enabled, RRC will NOT allocate memory from TMC heap. RRC will allocate 
     memory from its own new heap - RRC heap. There are 2 heaps: big and small. Big heap is 60,000 
     bytes and small heap is 40,000 bytes */
  #define FEATURE_RRC_HEAP
#endif

/* Support has been added in UE to keep the SIB7 values updated in RRC CELL_FACH state. 
   UE will try to refresh the sib 7 parameters at a minimum of every 320 ms. */
#define FEATURE_UPDATE_SIB7_IN_FACH

/* Made changes to set Traffic volume indicator in cell update message */

/* Added mechanism in RRC\MAC for reverting back to old TSNs after an HS reset.
   Used in going back to old configuration cases after HHO with MAC HS reset failure. */
#define FEATURE_HSDPA_MAC_HS_DL_RESTORE_TSN

/* With this feature, If NV_WCDMA_CS_VOICE_OVER_HSPA_ENABLED_I is set , UE will prioritize 850 band for full scan */

/* For W to LTE support */
#ifdef FEATURE_LTE
  #define FEATURE_WCDMA_TO_LTE
#endif

#ifdef FEATURE_WCDMA_TO_LTE
  #define FEATURE_WCDMA_ABSOLUTE_PRIORITY
  /* This is for Rel-8 compliance of sharing dedicated priorities across RATs. 
    In particular sharing of dedicated priroities between G and W is complete with this. */
#ifdef FEATURE_DEDICATED_PRIORITY
  #define FEATURE_WCDMA_DEDICATED_PRI_INFO
#endif
  #define FEATURE_WCDMA_SIB19
  #define FEATURE_WCDMA_CM_LTE_SEARCH
  #define FEATURE_WCDMA_ANT1_FW_CTRL
  #define FEATURE_W2L_IDLE_FW_RF_TUNE
  #define FEATURE_WTOL_PS_HANDOVER
  #define FEATURE_WTOL_TDD_CM_SUPPORT
  /* If the UE detects LTE cells, then it should display a ICON*/
  #define FEATURE_EUTRA_DETECT
  #define FEATURE_WCDMA_TO_LTE_FACH
  #define FEATURE_WCDMA_ABSOLUTE_PRIORITY_FACH
  #define FEATURE_WCDMA_CSFB_CALL_OPT
#endif


#define FEATURE_3GPP_FAST_DORMANCY_ENHANCEMENT

#define FEATURE_UMTS_BMC_ENH_SCHEDULE
#define FEATURE_BMC_EXTENDED_SFN_SUPPORT
#define FEATURE_WCDMA_SET_HS_CAT
#define FEATURE_WCDMA_SUSPEND_BMC_DURING_BPLMN

#define FEATURE_WCDMA_OPTIMIZED_BAND_SCAN 

#define FEATURE_PPAC

#define FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH

/* This feature is about providing extra events during codec change and during 
   handovers to CM so that CM can update apps processor with these events. 
   This is expected to help in quick change of codec at AP.  This is a short 
   burst FR. Feature Removed from here and enabled at Target level */


/* FR 3517 Rel-9 Signaling */
#define FEATURE_WCDMA_REL9

/* FR 16755: Rel10 ASN.1 Migration */
#ifdef FEATURE_WCDMA_TABASCO_MODEM
  #define FEATURE_WCDMA_REL10
#endif

/* FR1398 UE to support default configuration in FACH state*/
#define FEATURE_WCDMA_DEFAULT_CFG_CELLFACH 

/* UMTS to LTE reselection with MFBI */

#define FEATURE_WCDMA_MULTI_FREQ_BAND_INDICATOR

/* FR 27891 - With this feature LTAs will be allowed when reconfiguration is in progress and during activation time */
#define FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME

/*---------------------------------------------------------------------------
                     Features that need Rel 6 feature defined
---------------------------------------------------------------------------*/
/* HSDPA cell re-pointing through ASU. This feature is only defined when FEATURE_HSDPA & 
   FEATURE_WCDMA_REL6 is defined. */

/* Added support for "RB Information To Change " IE in Rb-Setup msg. With the support of 
   “Info to Change IE”,the RBs can be released or the existing RBs can be reconfigured to use 
   the default config RB parameters. Also moved the chnages made for Defualt config sent through 
   Rb-Setup msg under the feature flag FEATURE_DEFAULT_CONFIG_IN_RB_SETUP. */
#define FEATURE_DEFAULT_CONFIG_IN_RB_SETUP

#define FEATURE_WCDMA_DEFERRED_SIB11_12_READING

/*---------------------------------------------------------------------------
                              HSUPA
---------------------------------------------------------------------------*/
#ifdef FEATURE_WCDMA_HSUPA
  /* MAC log packets log codes 0x4321, 0x4322, 0x4323 will be logged. These log packets 
     will log the configuration, the accumulated MAC-es headers and the accumulated ETFCI 
     statistics information over a period of time. */
  #define FEATURE_MAC_HS_UL_QXDM_LOGGING
  /* Added changes for UE CAT support for HSUPA. */
  #define FEATURE_NV_WCDMA_HSUPA_CATEGORY 
  /*  This feature will enable the new E-TFCI selection behavior with the latest changes 
      accepted as part of TF 25.321 V6.14.0 to E-TFCI selection procedure in subclause 
      11.8.1.4 and Annex C pseudo-code for E-TFC selection */
  #define FEATURE_HSUPA_NEW_ETFCI_ALGORITHM
#endif

/*---------------------------------------------------------------------------
                            7200A Optimizations
---------------------------------------------------------------------------*/
#ifdef FEATURE_WCDMA_DL_DATA_PATH_OPT
  #define FEATURE_DATA_PS_HDLC_DSM_OPTIMIZED
#endif

/************************ REL 7 defines **********************************/
#ifdef FEATURE_WCDMA_REL7
  #undef FEATURE_DEFAULT_CONFIG_IN_RB_SETUP
  #undef FEATURE_WCDMA_PP_HW_DL
  #undef FEATURE_WCDMA_PLT
#endif

/* This only for WPLT for now */
#ifdef FEATURE_WPLT
  #define FEATURE_HSDPA_HARQ_PREAMBLE
#endif

#ifdef FEATURE_WDOG_DYNAMIC
  #define FEATURE_WCDMA_L1_WDOG_DYNAMIC
  #define FEATURE_WCDMA_L2_WDOG_DYNAMIC
#endif

#ifdef FEATURE_WCDMA_PLT_MODE
  #error code not present
#endif

#ifdef T_RUMI_EMULATION
  #define FEATURE_WCDMA_RUMI_SUPPORT
  #define RUMI_BRING_UP
  #define FEATURE_WCDMA_RUMI_SKIP_CME
#endif

#define FEATURE_WCDMA_SYSTEM_TIMER_REV3

#define FEATURE_TRM_DIME

#define FEATURE_WCDMA_PN_PACKET_VER4

#endif /* CUSTWCDMA_H */
