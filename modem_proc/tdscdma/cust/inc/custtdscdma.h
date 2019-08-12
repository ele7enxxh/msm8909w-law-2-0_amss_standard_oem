#ifndef CUSTTDSCDMA_H
#define CUSTTDSCDMA_H
/*===========================================================================

                           C U S T    T D S C D M A

DESCRIPTION
  Customer file for TD-SCDMA

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/cust/inc/custtdscdma.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
1/7/14    sarao    Creation and usage of MEDIUM SIZE DSM BUFFER
07/30/14   sarao   Enable FEATURE_TDSCDMA_AM_IGNORE_MAXRST
07/14/14   yufu    Enable wlan coex for DPM
06/16/14   sarao   Enabled the feature FEATURE_TDSCDMA_SILENT_FRAME for THOR
04/14/14   sarao   Enabled the feature FEATURE_TDSCDMA_SILENT_FRAME for DPM
10/16/13   sarao   Disabled segment loading feature as target team is defining it
10/08/13   sarao   Enabled segment loading feature
08/30/13   sarao   Enabled FEATURE_TDSCDMA_SRNS_SMC_FAULT_TOLERANCE
11/15/12   mkh     Disabled FEATURE_TDSCDMA_SAR_COEX_SUPPORT. 
                   Will be enabled only after enough testing.
07/11/12   sarao   Disabled FEATURE_TDSCDMA_MTNET.
07/11/12   sarao   Enabled FEATURE_TDSCDMA_MTNET.
07/07/12   mkh     Disabled FEATURE_TDSCDMA_NO_D2D_FAILURE_REVERT
06/15/12   fp      Merged the LTE for TDS 4.0.0 PW 1.0
04/28/12   fp      Enabled IRAT MEAS
                   disbling FEATURE_NOT_SUPPORT_INTER_RAT_MEAS and
                   enabling GL1_HEADER_FILE_READY
04/28/12   fp      Comment out: #define GL1_HEADER_FILE_READY
04/27/12   fp      #define FEATURE_NOT_SUPPORT_INTER_RAT_MEAS
04/25/12           Enablling RxAGC
04/22/12   mkh     Enabling FEATURE_TDSCDMA_SETUP_PICH_ONE_SUBFRAME_IN_ADVANCE.
                   Merging CL 2363366
04/22/12   fp      Enabled RxAGC feature: FEATURE_TDSCDMA_FW_NEW_LNA_INTF again
                   Disbling FEATURE_NOT_SUPPORT_INTER_RAT_MEAS
                   Reverted #define GL1_HEADER_FILE_READY & #define GL1_HEADER_FILE_READY_TMP
04/20/12   mkh     Reverting rxAGC feature. Will enable in next version
                   Enabling FEATURE_NOT_SUPPORT_INTER_RAT_MEAS
04/20/12   fp      Enabled RxAGC feature: FEATURE_TDSCDMA_FW_NEW_LNA_INTF
04/12/12   mkh     Disabled FEATURE_TDSCDMA_EVENTS_SUPPORTED. Not supported fully.
04/12/12   mkh     Enabled FEATURE_TDSCDMA_EVENTS_SUPPORTED on behalf of Bruce
11/17/11   mkh     Enabled FEATURE_TDSCDMA_DRX
11/09/11   mkh     Feature EIOT enabled
10/12/11   mkh     Enabled FEATURE_TDSCDMA_BYPASS_GERAN_BEFORE_GERAN_READY
09/05/11   its     Enable L1 to use RF message router based interface
08/25/11   mkh     Syncing features from off-target cust file
                   disable FEATURE_TDSCDMA_SUPPORT_IDLE_REQ.
04/08/11   ps      Disbaled FEATURE_TDSCDMA_TEMPERATURE_FC for 9x15 as currently not supported on nikel
04/07/11   ghj     Enabled FEATURE_TDSCDMA_RLC_QXDM_LOGGING FEATURE_TDSCDMA_TEMPERATURE_FC
                           FEATURE_TDSCDMA_USE_RLC_BACKUP_TIMER FEATURE_TDSCDMA_RLC_LW_MUTEX
                           FEATURE_TDSCDMA_MAC_HS_QXDM_LOGGING FEATURE_TDSCDMA_MAC_QXDM_LOGGING
03/08/11   its     Initial

===========================================================================*/

/* Enabling MEDIUM SIZE DSM ITEM to support PDU sizes above 128 bytes as SMALL DSM ITEM cannot handle it. -sarao*/
#define FEATURE_TDSCDMA_MEDIUM_DSM_BUFFER

/* Supporting silent frame feature for MVS callback -sarao*/
/*#if defined(FEATURE_DIMEPM_MODEM) || defined(FEATURE_THOR_MODEM) */ /* Enabling it for all PLs as requested by Audio team: CR 679450 */
#define FEATURE_TDSCDMA_SILENT_FRAME
/*#endif */

/* Enabling FEATURE_TDSCDMA_CFCM for only JO and THOR targets -sarao */
#if defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_THOR_MODEM)
#define FEATURE_TDSCDMA_CFCM
#endif

#define FEATURE_TDSCDMA_SRNS_SMC_FAULT_TOLERANCE


#define FEATURE_TDSCDMA_EVENTS_SUPPORTED

/* Supporting MCS changes fir T2G IRAT */
#define FEATURE_SLPC_TRIPLE_SIM

/* For non-ts0 pruning feature */
#define FEATURE_TDSCDMA_NON_TS0_CELL_PRUNING

//Enabling the MTNET Feature
#define FEATURE_TDSCDMA_MTNET

#ifndef FEATURE_TRITON_MODEM
#define FEATURE_TDSCDMA_60MS_GAP_FOR_BSIC_OPT
#endif

/* for Dime */

#define FEATURE_TDSCDMA_DAL_INT_CONTROLLER
#define FEATURE_TDSCDMA_FW_NEW_LNA_INTF
#define FEATURE_TDSCDMA_CRITICAL_SECTION_CLEANUP
//#define FEATURE_TDSCDMA_DIME_DBG_SB_DETECT_DL_ESTBLISH
#define FEATURE_TDSCDMA_FCCH_OPTIMIZATION
//#define FEATURE_TDSCDMA_DRX_MEAS_SCHE_OPT

#ifdef FEATURE_TRITON_MODEM
#define FEATURE_TDSCDMA_TRITON_UL
#endif

/* TPLT workaround */
#ifdef FEATURE_TDSCDMA_PLT
#define FEATURE_TDSCDMA_PLT_MAC
#define FEATURE_TDSCDMA_PLT_HW_RAM_VALIDATION
//#define FEATURE_TDSCDMA_PLT_BYPASS_MCPM
#define FEATURE_TDSCDMA_UL_MEMPOOL_LOGGING
#define FEATURE_TDSCDMA_EUL_MEMPOOL_LOGGING
#endif

#define FEATURE_TDSCDMA_RLC_QXDM_LOGGING

#define FEATURE_TDSCDMA_IRAT_RF_SCRIPT_PREBUILD
#define FEATURE_TDSCDMA_IRAT_FED_BASED
#define FEATURE_TDSCDMA_REL_RF_USING_SLEEP_API  // change RF API, use wakeup/sleep pair for normal request/release

#define FEATURE_TDSCDMA_USE_RLC_BACKUP_TIMER
#define FEATURE_TDSCDMA_RLC_LW_MUTEX
#define FEATURE_TDSCDMA_MAC_HS_QXDM_LOGGING
#define FEATURE_TDSCDMA_MAC_QXDM_LOGGING

//improve the UE response time for reconfiguration failure within one cell.
#define FEATURE_TDSCDMA_NO_D2D_FAILURE_REVERT

  /* 64QAM support in TDSCDMA. This feature has to come w/ MAC-ehs support in R9 */
#undef FEATURE_TDSCDMA_64QAM

/* A2 DL/UL feature */
#define FEATURE_TDSCDMA_A2_HW_DL

//#define FEATURE_TDSCDMA_UL_CIPHERING_DEBUG_TRACE
#define FEATURE_TDSCDMA_UL_CIPHERING_SW_WORKAROUND

  /* Send a CPHY_SETUP_REQ w/ Drop all chan flag to L1 to put it into SYNC state
   * which will still maintain the sync time of the camped cell.
   * RRC will no longer to send IDLE_REQ to L1 to send it to IDLE mode.
   * The counter part L1 feature is FEATURE_TDSCDMA_SUPPORT_IDLE_REQ.
   */

/*for IRAT QTF test*/
//#define FEATURE_TDSCDMA_IRAT_TEST

/* Enable L1 to use RF message router based interface */
#define FEATURE_TDSCDMA_RF_SUPPORT

#define FEATURE_TDSCDMA_L1_PERFORMANCE_WATCHDOG

/* TODO: remove once full stack bring up complete */
#if !defined(FEATURE_TDSCDMA_PLT) && !defined(TEST_FRAMEWORK)
#define FEATURE_TDSCDMA_FULLSTACK_BRINGUP
#endif

#define FEATURE_TDSRLF_SYSTEM_SEL
#define FEATURE_TDSCDMA_TC_CONSTANT_DELAY

// #define FEATURE_TDSCDMA_EIOT
// #define FEATURE_NOT_SUPPORT_INTER_RAT_MEAS
/* #ifdef FEATURE_TDSCDMA_EIOT
#define FEATURE_NOT_SUPPORT_INTER_FREQ_MEAS
#endif  */

#define FEATURE_TDSCDMA_CU_CNF_FREQINFO_FAULT_TOLERANCE

#define FEATURE_JDS_OPT

#define FEATURE_TDSCDMA_RF_API_REVAMP


#define FEATURE_TDSCDMA_CELL_RESELECT_INTER_F
#define FEATURE_TDSCDMA_CELL_RESELECT_INTER_S
#define FEATURE_TDSCDMA_CELL_FACH_MEAS_OCCASION
#define FEATURE_TDSCDMA_QUALITY_MEAS

/*Define HSUPA support*/
//top level feature removed:
#define FEATURE_TDSCDMA_HSUPA
#define FEATURE_TDSCDMA_HSUPA_PADDING_MAXIMUM
 
/*To support SP6200 test when there is no signalled TFC while all others are marked as computed*/

/* Search TD-SCDMA PLMN only even dual mode search is required by higher layer */


/* L1 features from UMTS */
/*#define FEATURE_TDSCDMA_2G_RANK_Q_OFFSET_OPT*/

#define FEATURE_TDSCDMA_AGGRESSIVE_DORMANT_MODE

#define FEATURE_TDSCDMA_CELL_FACH_RESEL_CHK_THRESH

//MOB #define FEATURE_TDSCDMA_CELL_SELECTION_PERIODIC


/* enable JDS implementation for G2T IRAT */
/*  #define TEMP_T2G_DEDICATE_ONLY   */
#define FEATURE_TDSCDMA_IRAT_JDS


/* L1 DL OLPC feature */
#define FEATURE_TDSCDMA_L1_TB_BASED_OLPC
#define FEATURE_TDSCDMA_L1_TFCI_BASED_OLPC


/*Enabled FEATURE_TDSCDMA_SRNS_MO_CALL_DEFER*/
#define FEATURE_TDSCDMA_SRNS_MO_CALL_DEFER

/* RF FTM Non-Signalling feature*/
/* For DPM 2.0, CR http://prism.qualcomm.com/CR/649895*/
#define FEATURE_TDSCDMA_FTM_NS_NEW

/* L1 SW trace  */
#define FEATURE_TDSCDMA_L1_TRACE

/* Enable RxAGC feature */
#define FEATURE_TDSCDMA_FW_NEW_LNA_INTF

#define FEATURE_TDSCDMA_SETUP_PICH_ONE_SUBFRAME_IN_ADVANCE

/* Enable IRAT feature */
#define FEATURE_TDSCDMA_FW_IRAT_LNA_INTF
#define FEATURE_TDSCDMA_IRAT_BUILD_RF_CMD


#ifdef FEATURE_TRM_V6
#define FEATURE_TDSCDMA_TRM_NEW_API
#endif


/* DSDS & DSDA feature flags */
#if defined(FEATURE_DUAL_SIM) && !defined(FEATURE_TDSCDMA_PLT)
#define FEATURE_TDSCDMA_DSDS
#define FEATURE_TDSCDMA_DSDS_QTA
#define FEATURE_TDSCDMA_DSDS_TUNEAWAY
#define FEATURE_TDSCDMA_G2T_TUNE_AWAY
#ifdef FEATURE_BOLT_MODEM
#define FEATURE_TDSCDMA_DR_DSDS
#define FEATURE_TDSCDMA_DR_DSDS_FALLBACK
#endif
#define FEATURE_TDSCDMA_DSDS_ENH
#define FEATURE_TDSCDMA_DSDA  
#define FEATURE_TDSCDMA_DSDS_ALLOW_QTA_BEFORE_ACT_TIME
#ifdef FEATURE_TDSCDMA_DSDS
#define FEATURE_WWAN_WLAN_ANTENNA_SHARING
#undef FEATURE_TDSCDMA_TRM
#endif /* FEATURE_TDSCDMA_DSDS */
#endif /* FEATURE_DUAL_SIM */
/* DSDS & DSDA feature flags end */

/* Temp disable TRM for bolt */
//#if !defined(FEATURE_TDSCDMA_DSDS) && !defined(FEATURE_TDSCDMA_PLT)
//  #define FEATURE_TDSCDMA_TRM
//#endif
 

#ifdef FEATURE_TDSCDMA_TO_LTE

   /* LTETBD: Defined in custlte.h. Remove after LTE is supported */
   #define FEATURE_LTE_TO_TDSCDMA
#ifdef FEATURE_LTE_REL11
   #define FEATURE_TDSCDMA_SUPPORT_LTE_RLF_REPORT_ENHANCEMENT
#endif
   /* LTDTBD: will change to FEATURE_3GPP_CSFB
    * after the LTE upmerge complete
    */
   #define FEATURE_3GPP_CSFB_TDS

#ifdef FEATURE_TDSCDMA_DSDS
   /* L2TD DSDS/TRM exchange */
   #define FEATURE_LTE_TO_TDSCDMA_DSDS
#endif

   /* From FEATURE_3GPP_CSFB_SKIP_OPT_WCDMA_SIBS */
   #define FEATURE_3GPP_CSFB_SKIP_OPT_TDSCDMA_SIBS

   /* Disabled according to LTE team request */
   /*#define FEATURE_TDSCDMA_L2TD_PS_HANDOVER*/

   /* Turn on FEATURE_INTERRAT_HANDOVER_G2TD for LTE to use the shared items with GeRan*/
   /* #define FEATURE_INTERRAT_HANDOVER_G2TD*/

   #define FEATURE_TDSCDMA_ABSOLUTE_PRIORITY
   /* This is for Rel-8 compliance of sharing dedicated priorities across RATs. 
      In particular sharing of dedicated priroities between G and TDS is complete with this. */
#ifdef FEATURE_DEDICATED_PRIORITY
   #define FEATURE_TDSCDMA_DEDICATED_PRI_INFO
#endif
   #define FEATURE_TDSCDMA_SIB19

   //#define FEATURE_TDSCDMA_TRM   /* disabled for dsds */
   /* Some LTE macro definition under FEATURE_TDSCDMA_REL8, so need opend it */
   #define FEATURE_TDSCDMA_REL8
   /* Add more LTE related features such as L1 only features */

   /* Have to enable it even only for REL8 support */
   #define FEATURE_TDSCDMA_REL9
   //#define FEATURE_TDSCDMA_HSDPA_MAC_EHS
   //#define FEATURE_TDSCDMA_64QAM

   /* LTETBD: this feature comes from FEATURE_WCDMA_CM_LTE_SEARCH
    * It is not completely merged because TDS does not support CM
    * Will check with L1 and see if L1 has to keep CM for LTE as well
    */
   /* #define FEATURE_TDSCDMA_CM_LTE_SEARCH */
#endif /* FEATURE_TDSCDMA_TO_LTE */

/*Define the FreqInfo and CPID workaround for sepcial test cases*/
#define FEATURE_TDSCDMA_SINGLE_FREQ_NO_CPID_HO_SUPPORT

/* TS0 ERSCP Pruning */
#define FEATURE_TDSCDMA_FILT_M
#define FEATURE_LINEAR_FILTER_OPT

#define FEATURE_TDSCDMA_FREE_FLOATING_TASK
/*Define avoid add fach channel when drop N-PCCPCH channel*/

#define FEATURE_TDSCDMA_L1_LOGGING_ENABLED
//#define FEATURE_TDSCDMA_F3_MESSAGE_REDUCTION

/* mMetric Feature */
#define FEATURE_TDSCDMA_IRAT_JDS_OPT

/* Rx Diversity */
#if !defined(FEATURE_TRITON_MODEM) && !defined(FEATURE_JOLOKIA_MODEM) && !defined(FEATURE_TDSCDMA_PLT)
#define FEATURE_TDSCDMA_READ_RXD_NV
#endif
#define TDSCDMA_FEATURE_SERVING_CELL_RSCP_ANT1


/*SAR Coex Support*/
//#define FEATURE_TDSCDMA_SAR_COEX_SUPPORT
#define FEATURE_TDSCDMA_ISCP

#define FEATURE_TDSCDMA_RESEL_OPT


/* per Huichun to disable in order to fix 442562*/
/*add it back in CR458532*/
/*add it back in CR496039*/
#define FEATURE_JDS_OPT2

#ifdef FEATURE_3GPP_FAST_DORMANCY
    #define FEATURE_TDSCDMA_IGNORE_T323
#endif


#define FEATURE_TDSCDMA_RCR_CANCEL_RECFG

/* TDS MCVS FEATURE */ 
#define FEATURE_TDSCDMA_MCVS


#ifdef FEATURE_TABASCO_MODEM           
    #define FEATURE_TDSCDMA_TABASCO_MCVS /*Tabasco*/
#elif defined(FEATURE_JOLOKIA_MODEM)
    #define FEATURE_TDSCDMA_JOLOKIA_MCVS /*Jolokia*/ 
#elif defined(FEATURE_THOR_MODEM) 
    #define FEATURE_THOR_TDS_MCVS     /* Thor */
#elif defined(FEATURE_BOLT_MODEM) 
    #define FEATURE_BOLT_TDS_MCVS     /* Bolt */
#elif defined(FEATURE_TRITON_MODEM)
    #define FEATURE_TRITON_TDS_MCVS   /* Triton */
#elif defined(FEATURE_DIMEPM_MODEM)
    #define FEATURE_DIMEPM_TDS_MCVS   /* DimePM */
#elif defined(FEATURE_DIME_MODEM)
    #define FEATURE_DIME_TDS_MCVS     /* Dime*/
#endif

#ifdef FEATURE_BOLT_MODEM
/* RF wakeup step 0 separation */ 
#define FEATURE_TDSCDMA_RF_WU_STEP0_SEP
#endif

// Adaptive Wakeup Timeline feature, temp for DPM
#if defined(FEATURE_DIMEPM_MODEM) || defined(FEATURE_THOR_MODEM) || defined(FEATURE_JOLOKIA_MODEM)
   #define FEATURE_TDSCDMA_ADAPTIVE_WAKEUP_TIMELINE
#endif 

#define FEATURE_TDSCDMA_SW_F8_DECIPHER   /*eigher triton or dime, we use the SW F8 */

#define FEATURE_TDSCDMA_SMC_DURING_RECFG_OPT
/*To support silent redial*/


#define FEATURE_TDSCDMA_SRB5_SUPPORT
#define FEATURE_TDSCDMA_VOC_AMR_WB


#define FEATURE_TDSCDMA_E4A_OPT
#define FEATURE_TDSCDMA_MTPL_THERMAL_BACKOFF

#define FEATURE_TDSCDMA_UL_RLC_SIZE_CHANGE
#define FEATURE_TDSCDMA_RRC_RB_PRIORITY_OPT

#ifndef FEATURE_THOR_MODEM
#if defined(FEATURE_MODEM_ANTENNA_SWITCH_DIVERSITY) && !defined(FEATURE_TDSCDMA_PLT)
#define FEATURE_TDSCDMA_ANTENNA_SWITCH_DIVERSITY
#ifdef FEATURE_BOLT_MODEM
#define FEATURE_TDSCDMA_ASDIV_DSDA
#endif
#endif
#endif

/*for SRNS relocation enhancement when dl-counter-sync is missing in the OTA*/
#define FEATURE_TDSCDMA_SRNS_WO_DL_COUTER_SYNC_INFO
#define FEATURE_TDSCDMA_RECONFIG_INDICATOR_CU
#define FEATURE_TDSCDMA_NEW_NON_TS0_PRUNING
#define FEATURE_TDSCDMA_RRC_SN_INCREASE_IN_SRNS

/*SRNS enhancement for buffering the DL DT during pending state*/
#define FEATURE_TDSCDMA_SRNS_OLD_FRESH_BUFFER_DDT

#define FEATURE_TDSCDMA_REL9_NO_T2L_CONN_MEAS

#define FEATURE_TDSCDMA_QTUNER

//#define FEATURE_TDSCDMA_ISCP_SCHE_CHECK /*check measure report configuration to decide whether to do ISCP measurement scheduling*/
#define FEATURE_TDSCDMA_ISCP_SCHE_BASED_ON_MEAS_SET /*schedule ISCP measurement based on 'cells_for_measurement', rather than entire cells in intra freq set*/
//#define TDSCDMA_ISCP_OPTIMIZATION_8CELLS_1SLOT

#define FEATURE_TDSCDMA_CHANGE_SRB4_PRIRORITY


/*Change the RRC-MAC interface for ciphering configuration*/
#define FEATURE_TDSCDMA_MAC_CONFIG_UPDATE_CIPHER_ONLY

//Sarao: Porting WCDMA CR 295081 MRAB changes to Avoid RLC reset on PS RB during MRAB call
#define FEATURE_TDSCDMA_AM_IGNORE_MAXRST

#define FEATURE_TDSCDMA_SCAN_SCOPE

/*move from tdscdma_local_define.h*/
#define FEATURE_TDSCDMA_E4B_OPT
/*#define FEATURE_TDSCDMA_UM_CCCH_OPT*/
#define FEATURE_TDSCDMA_SRB_RLC_SDU_RESTORE
#define FEATURE_TDSCDMA_PS_RLC_SDU_RESTORE
#define FEATURE_TDSCDMA_TFC_ROUNDROBIN

/*Below is after feature rename*/


#ifndef FEATURE_TEST_LOOPBACK
#define FEATURE_TEST_LOOPBACK  /*the owner looks like WCDMA*/
#endif

#ifndef FEATURE_TDSCDMA_CPU_BASED_FLOW_CONTROL
#define FEATURE_TDSCDMA_CPU_BASED_FLOW_CONTROL
#endif
#ifndef FEATURE_TDSCDMA_CPU_BASED_FLOW_CONTROL_WITH_QOS
#define FEATURE_TDSCDMA_CPU_BASED_FLOW_CONTROL_WITH_QOS
#endif

#ifndef FEATURE_THOR_MODEM
#ifndef FEATURE_TDSCDMA_TVM_BLOCK
#define FEATURE_TDSCDMA_TVM_BLOCK
#endif
#endif

#ifndef FEATURE_FC_CPU_MONITOR
#define FEATURE_FC_CPU_MONITOR
#endif

/*RRC split features*/
#define FEATURE_TDSCDMA_HSUPA
#define FEATURE_TDSCDMA_MULTIRATE_AMR
#define FEATURE_TDSCDMA_DRX_MANAGER


#define FEATURE_TDSCDMA_NIKEL_SW  /*will be mainlined in future release*/


/*L1 split features*/
#define FEATURE_TDSCDMA_TX_PWR_BACK_OFF
#define FEATURE_TDSCDMA_TCXO_RESOURCE_SUPPORT
#define FEATURE_TDSCDMA_PWR_TFC_ELMN
#define FEATURE_TDSCDMA_OFFLINE_SEARCHER
#define FEATURE_TDSCDMA_OFFLINE_MODEM
#define FEATURE_TDSCDMA_MULTIPLE_L3_FILTERS_SUPPORTED
#define FEATURE_TDSCDMA_L1_RXD_FILTERING_SUPPORTED
#define FEATURE_TDSCDMA_HCS
#define FEATURE_TDSCDMA_GENESIS_F8_BUG_WORKAROUND
#define FEATURE_TDSCDMA_DL_MODEM_ACC
#define FEATURE_TDSCDMA_DL_DATA_PATH_OPT
#define FEATURE_TDSCDMA_DL_DYN_MEM_ALLOCATION
#define FEATURE_FORCE_DROP_ADD_HSDPA

#define FEATURE_MCAL_TDSCDMA

#define FEATURE_HANDLE_CN_PAGE_IN_CONNECTED_STATE
#define FEATURE_TDSCDMA_L1_BSIC_ID_FORBID_TIMER_AFTER_F_RECONF_ABORT
#define FEATURE_TDSCDMA_L1_FORBID_KNOWN_CELL_OFFSET_IN_SAME_ARFCN
#define FEATURE_TDSCDMA_L1_GSM_FORCE_RECONF_ABORT_UNKNOWN_CELL
#define FEATURE_TDSCDMA_L1_INTRAF_MRM_OPT
#define FEATURE_TDSCDMA_OFFLINE_SEARCHER
#define FEATURE_TDSCDMA_SRCHACQ_RETRY
#define FEATURE_TDSCDMA_TX_BETA_GAIN_TABLE_VER2

/*Enable repeat 1g/2a report to max. times and trigger on second strongest cell*/
#define FEATURE_TDSCDMA_REPEAT_TO_SECOND_BEST

#define FEATURE_TDSCDMA_E3A_THRESHOLD_OPT

#define FEATURE_TDSCDMA_SPECIFIC_SIB_READING

#define FEATURE_TDSCDMA_SRB_RESET
#define FEATURE_TDSCDMA_PCH_FET
/*SIR FET enabled for Bolt/Thor/Jolokia/Tabasco*/
#define FEATURE_TDSCDMA_PCH_SIR_FET


/*Enable the features after split with W feature set*/
#define FEATURE_INTERRAT_PCCO_G2TD

#define FEATURE_PARTIAL_MPLMN_DISPLAY

#ifdef FEATURE_TRITON_MODEM /* TR defines this in MM, DI defines this in SYS */
#define FEATURE_USE_MM_MANUAL_SEARCH_TIMER_VALUE_TYPE
#endif

#ifdef FEATURE_BOLT_MODEM
#define FEATURE_MODEM_BOLT
#define FEATURE_SERVICE_SEARCH_PRIORITY_PLMN
#endif

#define FEATURE_TDSCDMA_SDU_LIFETIME_VALIDATION

#if defined(FEATURE_TDSCDMA_TO_LTE) && defined(FEATURE_TDSCDMA_ABSOLUTE_PRIORITY)
#define FEATURE_TDSCDMA_TO_LTE_EVAL_TRESEL_OPT
#define FEATURE_TDSCDMA_PRIORITY_RESELECT_OPT
#endif 

#define FEATURE_TDSCDMA_L1M_PROFILING_LOG_PACKET
#define FEATURE_TDSCDMA_FAST_HO_BY_MTPL

#ifndef FEATURE_THOR_MODEM
#if defined(FEATURE_DIMEPM_MODEM) || defined(FEATURE_BOLT_MODEM)
#define FEATURE_TDSCDMA_LVA_DECODE
#endif
#endif

/*for DPM only shortly */
//#ifdef FEATURE_DIMEPM_MODEM 
#if defined(FEATURE_DIMEPM_MODEM) || defined(FEATURE_BOLT_MODEM) || defined(FEATURE_JOLOKIA_MODEM)
#define FEATURE_TDSCDMA_RF_WKUP_DETECT  /*for mpss4.1 branched out as of DPM2.0 support this feature*/
#endif

#define FEATURE_TDSCDMA_ONE_SIDE_REEST

#ifndef FEATURE_THOR_MODEM
#define FEATURE_TDSCDMA_CELL_SELECTION_INTER_SIB_SLEEP
#endif

#define FEATURE_TDSCDMA_LOW_SIB_PRIORITY_DUR_CELL_SEL

#ifndef FEATURE_TDSCDMA_PLT
#define FEATURE_TDSCDMA_ACQ_ENHANCEMENT
#endif

/* CR664031 depends on NAS CR664032 which is not available for DPM */
#ifdef FEATURE_DIMEPM_MODEM
/* Nothing here */
#else
#define FEATURE_TDSCDMA_PSEUDO_SIB19
#endif

#if defined(FEATURE_BOLT_MODEM) && defined(FEATURE_THOR_MODEM)
/* for Adaptive ReACQ Gamma size */
#define FEATURE_TDSCDMA_ADAPTIVE_REACQ_GAMMA_SIZE
#endif

//For now, the JO target has DPM_MODEM defined. the wlan coex feature is for DPM only, and undef this feature for other targets.
#if defined(FEATURE_DIMEPM_MODEM)
#define FEATURE_TDSCDMA_WLAN_COEX
#endif

#if defined(FEATURE_JOLOKIA_MODEM) && !defined(FEATURE_TDSCDMA_PLT)
#define FEATURE_TDSCDMA_WLAN_COEX
#ifdef FEATURE_TDSCDMA_WLAN_COEX
#define FEATURE_TDSCDMA_DSDA
#endif
#endif


#if defined(FEATURE_THOR_MODEM) || defined(FEATURE_BOLT_MODEM) || defined(FEATURE_TABASCO_MODEM)
#undef FEATURE_TDSCDMA_WLAN_COEX
#endif

#if defined(FEATURE_TRITON_MODEM) || defined(FEATURE_JOLOKIA_MODEM)
#define FEATURE_TDSCDMA_HSUPA_EPUCH_CMD_AFTER_A2_CB
#endif

#if defined(FEATURE_BOLT_MODEM) || defined(FEATURE_THOR_MODEM)
#define FEATURE_TDSCDMA_HSUPA_MK_TX
#endif

#if defined(FEATURE_JOLOKIA_MODEM) || defined(FEATURE_TABASCO_MODEM)
#define FEATURE_TDSCDMA_SAWLESS 
#define FEATURE_TDSCDMA_PLT_JOLOKIA_SERIES
#endif

#define TDSCDMA_T2G_GSM_INIT_MSGR
#define FEATURE_TDSCDMA_NEW_GAP_DETECTION_THRESHOLD

#ifdef  FEATURE_THOR_MODEM
#define FEATURE_TDSCDMA_MCPM_POWER_REPORTING
#endif

#if defined(FEATURE_BOLT_MODEM) || defined(FEATURE_THOR_MODEM) || defined(FEATURE_JOLOKIA_MODEM)
/*enable the Pwr test feature for Bolt and THor and JOLOKIA 
  still it's NV controled*/
#define FEATURE_TDSCDMA_MSS_PWR_TEST
#endif
#ifdef FEATURE_TDSCDMA_MSS_PWR_TEST
/*paging pattern learning feature to complete the power test mode*/
#define FEATURE_TDSCDMA_PAGE_PATTERN_DETECT
#endif

#if defined(FEATURE_BOLT_MODEM) && !defined(FEATURE_THOR_MODEM) && !defined(FEATURE_JOLOKIA_MODEM) && !defined(FEATURE_TABASCO_MODEM)
#define FEATURE_TDSCDMA_MOTION_SENSOR_AID_MEAS_SUPPRESS
#endif

#if !defined(FEATURE_BOLT_MODEM) && !defined(FEATURE_JOLOKIA_MODEM)
#define FEATURE_TDSCDMA_CHECK_GERAN_SUB_ID /*T<->G sub ID not PoRed for BOLT/JO*/
#endif

//Feature flag for FR 25117: T2L enhancement
#if defined(FEATURE_THOR_MODEM) || defined(FEATURE_TABASCO_MODEM)
#define FEATURE_TDSCDMA_LTE_MEAS_TIMING_IMPROVEMENT
#endif

//Feature flag for FR 21713: False Alarm in TDS Acq in LTE signal
#define FEATURE_TDSCDMA_L1_NEW_GAP_DETECT_API
#define FEATURE_TDSCDMA_IRAT_INFO_TO_LTE
#define FEATURE_TDSCDMA_ADAPTIVE_T2G
#endif /* CUSTTDSCDMA_H */
