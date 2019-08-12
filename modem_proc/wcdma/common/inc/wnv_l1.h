/*============================================================================
                            W N V _ L 1 . H
DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/common/inc/wnv_l1.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     -------------------------------------------------------- 
10/12/15    svh     CME features NV.
09/10/15    ac      Port MSIM support from TH 2.0
02/09/15    svh     Added NV for MDSP RECOVERY stats.
12/31/14    svh     Added CirCleanThresh field to CIRE NV.
12/19/14    da      Changes to use the lower two bytes of NV 5458 to lock PSC for acq
11/24/14    svh     Added NV for MDSP RECOVERY.
11/21/14    hdk     New member added to WNV_ID_L1_ANT_SWITCH
10/27/14    ac      Fixed type of WNV_ID_L1_RXD_OPTS to be wl1_rxd_opts_nv_struct_type
10/17/14    psr     Added is_is_pso_opt_off in srchzz_idle_mode_opt_nv_struct_type and featured for non-bolt.
10/16/14    skk     Added is_reacq_less_pich_opt_off field in srchzz_idle_mode_opt_nv_struct_type.
10/13/14    ac      Change FET default from 0x47FF to 0x4FFF
09/22/14    amj     Changes for NV for NASTT Power Optimizations and other debug.
09/25/14    rsr     Added Change to ensure that we dont Skip CPC Short Cycle 
09/17/14    ac      Added WNV_CONF_ID_WSRCH_NBREJ and WNV_CONF_ID_QTA_ROTA
08/26/14    svh     Added changes for CIR enhancement NV.
07/15/14    ac      Initial checkin 
===========================================================================*/

#ifndef _WNV_L1_H_
#define _WNV_L1_H_

#include "comdef.h"
#include "wnv.h"
#include "msgcfg.h"

/*************************************************************************** 
              L1 defaults (not specifically called out elsewhere)
****************************************************************************/

/* FET default - DEFAULT_NV_VALUE is applied internally when
   DEFAULT_WRITTEN_NV_VALUE is read from NV */
#define WL1_FET_DED_EDv2_DEFAULT_NV_VALUE                   (0x4FFF)
#define WL1_FET_DED_EDv2_DEFAULT_WRITTEN_NV_VALUE           (0xFFFF)

/* CME defaults */
/* CME opts NV setting */
#define WL1_CME_OPTS_DISABLE_QPM_IN_MRAB_CALL_SRB_MAPPED_TO_HS (0x0001)
#define WL1_CME_OPTS_ENABLE_CRASH_TEST                         (0x0008)
#define WL1_CME_DEFAULT_OPTS (WL1_CME_OPTS_DISABLE_QPM_IN_MRAB_CALL_SRB_MAPPED_TO_HS)
#define WL1_CME_DEFAULT_OPTS_WRITTEN_TO_NV                     (0xFFFF)

/* Default value for minimum value of rssi delta below which the variation calls 
 * for QICE to be disabled: 0.01 -> 10 in Q10
 */
#define WL1_CME_DEFAULT_MIN_AVG_RSSI_DELTA  (10)
/* Default value for minimum retainer value of rssi delta above which the variation calls 
 * for QICE to be enabled: 0.035 -> 36 in Q10
 */
#define WL1_CME_DEFAULT_MIN_AVG_RSSI_RETAIN_DELTA  (36)

/* DL defaults */
typedef struct
{
  uint8  hsonlytpc_enable :1;
  uint8  hsonlytpc_dup_thresh_percentage :7;
  uint8  hsonlytpc_TPC_imbal_thresh_percentage;
}dl_nv_hsonlytpc_struct_type;

typedef PACKED struct PACKED_POST
{
  /* Num fingers on primary and secondary carrier. The 1st nibble will be num pri fingers
       The 2nd nibble will be num sec fingers */
  uint8 wl1_tri_num_fingers_pri_sec;

  /* Num fingers on 3rd and 4th carrier. The 3rd nibble will be num pri fingers
     The 2nd nibble will be num sec fingers */
  uint8 wl1_tri_num_fingers_3rdC_4thC;

  /* To enable/disable EDRX Full sleep
    TRUE = Disable EDRX FS
    FALSE = Enable EDRX FS*/
  boolean disable_EDRX_FS;

  /* NV to control HS only TPC feature */
  dl_nv_hsonlytpc_struct_type hsonlytpc_nv;

  /*
  Bit 0: Enable_Triage_dbg_F3s
  Bit 1: Enable_Idle_Triage_dbg_F3s
  Bit 2: Finger_Buckshot_Algorithm
  Bit 3: Enable_DL_Cell_Cfg_F3s
  Bit 4: Enable_DL_PHCH_F3s
  Bit 5: FDPCH_SIRE_Bias_Compensation
  Bit 6: CPC_Lite_Sleep
  Bit 7: Dynamic_Finger_Switch_DCUPA
  */
  uint8  enable_Triage_dbg_F3s         :1;/*BIT 0*/
  uint8  enable_Idle_Triage_dbg_F3s    :1;/*BIT 1*/
  uint8  finger_Buckshot_Algo          :1;/*BIT 2*/
  uint8  enable_DL_Cell_Cfg_F3s        :1;/*BIT 3*/
  uint8  enable_DL_PHCH_F3s            :1;/*BIT 4*/
  uint8  FDPCH_SIRE_Bias_Comp          :1;/*BIT 5*/
  uint8  CPC_Lite_Sleep                :1;/*BIT 6*/
  uint8  dynamic_fing_switch_dcupa     :1;/*BIT 7*/

  /* Reserved for future use */
  uint16 reserved2;
} dl_nv_struct_type;

/* WL1 UL */
typedef PACKED struct PACKED_POST
{
  int16 wl1_sar_tx_pwr_timer_nv;
  uint8 reserved1;
  uint8 reserved2;
  uint32 reserved3;
} wl1_ul_nv_struct_type;

/* Macro for default timer value */
#define WCDMA_SAR_TX_PWR_FILT_TMR_DFLT_VAL 20

/* L1 SRCH */

/* Structure to read NV item for e1d enhancement*/ 
typedef PACKED struct PACKED_POST
{ 
  boolean enabled; 
  uint8 threshold;
  uint8 nominal_timer;
  uint8 min_timer;
 
  uint8 reserved1;
  uint32 reserved3;
}wl1_srch_e1d_enhancement_vars_struct_type; 

typedef PACKED struct PACKED_POST
{
  /*! NBR tunable parameters through NV item*/
  boolean enabled;
  uint32 rotatorFreq;  /* rotator frequency to be programmed to RXFE_NB_CSR_FREQ_OFFSET_NB0 */
  uint8 accumBpg;     /*! num of bpgs to accumulate for NBEE */
  int8 threshold1;    /* threshold1 - XXX */
  int8 threshold2;    /* threshold1 - XXX */
 
} wsrch_nbrej_nv_config_params;

#define WSRCH_NB_REJ_ROTATOR_KHZ  400
/*NB rejection algo will run on these many samples */
#define WSRCH_NB_REJ_ACCUM_BPG 16
#define WSRCH_NB_REJ_ACCUM_BPG_MAX 32
#define WSRCH_NB_REJ_THRESH1 5
#define WSRCH_NB_REJ_THRESH2 10


/* L1 ANT_SWITCH */
/* Structure of the ant switch efs item */
typedef PACKED struct PACKED_POST
{
  /* the power delta between the Pri and Div antennas*/
  uint16 power_delta;
  /* The rscp threashold at which the probe step 2 will be triggered */
  uint16 RSCP_trigger;
  /* to enable or disable the feature*/
  uint16 ant_switch_enable;
  /* Type2 RSCP switch threshold */
  uint8 Type2_RSCP_switch_threshold;
  /* Type2 MTPL threshold */
  uint8 Type2_MTPL_threshold;
}wl1_ant_switch_nv_struct_type;

/* Structure of the ant switch efs item */
typedef PACKED struct PACKED_POST
{
  uint64   type_1_bands_mask;
  /* Bands supported for type 1 */
  uint64   type_2_bands_mask;
    /* Bands supported for type 2 */
  int16 type2_post_switchBack_suspension_time;
  int16 type2_post_switch_eval_base_thre;
  uint8 type2_RSCP_drop_thre;
  uint8 mtpl_alogrithm_diable;
  uint8 type1_post_switchback_suspension_time;
  /*Traffic crisis mode threshold*/
  uint8 wcdma_traffic_crisis_mode_thr;
  /*Idle mode ping-pong mitigation threshold*/
  uint8 wcdma_idle_pingpong_mitigation_thr;
  /*Idle suspension duration*/
  uint8 wcdma_idle_suspension_duration;

  uint8 reserved_field[6];
}wl1_ant_switch_band_nv_struct_type;


/* The EFS (NV) contains a structure of following RACH parameter. */
/* NV 70282                                                       */
typedef struct
{
  uint16 control;
  uint16 max_preamble;
  uint16 max_noack_crash_cnt;
  uint16 reserved;
} wl1_ul_rach_nv_config_type;


typedef PACKED struct PACKED_POST
{
  /* NV version number */
  uint8 version;
  /* how often atuner should fire its periodic timer */
  uint32 timer_proc_period_ms;
  /* extra space for future versions */
  uint32 reserved;
} wl1_atuner_nv_read_struct_type;

/* this is how often the atuner processing timer will fire */
#define WL1_ATUNER_MIN_PROC_PERIOD_MS 20
#define WL1_ATUNER_MAX_PROC_PERIOD_MS 10000
#define WL1_ATUNER_DEFAULT_PROC_PERIOD_MS 200

typedef PACKED struct PACKED_POST
{
  uint8 version; 
  boolean is_parallel_reacq_on;
  boolean is_offline_pich_on;
  boolean is_nbr_srch_opt_on;
  boolean is_offline_nastt_on;
  boolean is_interf_offline_pn_on;
  boolean is_rf_wakeup_opt_on;
#ifndef FEATURE_BOLT_MODEM
  boolean is_reacq_less_pich_opt_off;
  boolean is_pso_opt_off;
#else
  uint16 reserved1;
#endif
  uint32 reserved2;
}srchzz_idle_mode_opt_nv_struct_type;

/* HARQ buffer flush timer max value */
#define HS_HARQ_BUF_FLUSH_TMR_MAX_VAL 48

/* Support for NV read for DRX delayed timer */
typedef PACKED struct PACKED_POST
{
  uint8  version; 
  uint8  drx_delayed_timer_ms;
  uint8  rlp_nbrs_check_disabled;
  /*
    1st bit - To crash on low PI_SUM (<100 and >-100).
    2nd bit - To print FW raw CPICH/PICH symbols info for RLP.
  */
  uint16 idle_mode_debug_var;
  uint32 reserved1;
  uint16 reserved2;
  uint8  reserved3;
}drx_idle_mode_debug_nv_struct_type;

typedef PACKED struct PACKED_POST
{
  boolean   cirInterpEnable;
  uint8     oneTapCirAlpha;
  boolean   enableDopplerEstimation;
  uint16    doppler         : 10;
  uint16    cirCleanThresh  :  6;
  uint16    sigmaScale;
} wl1_cme_cir_enh_nv_struct_type;

typedef PACKED struct PACKED_POST
{
  uint8 version;

  /* Num fingers on primary and secondary carrier. The 1st nibble will be num pri fingers
       The 2nd nibble will be num sec fingers */
  boolean nastt_connected_mode_pwr_opt_en;

  /* Reserved for future use */
  uint64 reserved1;

  uint32 srch_enhnc_ctrl;

  uint16 reserved3;
} srch_nv_struct_type;

typedef PACKED struct PACKED_POST
{
  uint32   irpmEnable    : 1;
  uint32   vlcEnable     : 1;
  uint32   pcovEnable    : 1;
  uint32   qiceCmEnable  : 1;
  uint32   qiceTMEnable  : 1;
  uint32   reserved      : 27;
} wl1_cme_features_nv_struct_type;

/*************************************************************************** 
                            RxD defaults
****************************************************************************/

/*NV OPTs struct type*/
typedef PACKED struct PACKED_POST
{
  uint8 version;
  uint32 rxd_opts;
  uint32 reserved;
}wl1_rxd_opts_nv_struct_type;

#define WL1_RXD_DEFAULT_OPTS_WRITTEN     0xFFFF

/* RxDPM_V2/ARD_V2 algo thresholds */
#define RXD_MMSE_TAP_ENG_DELTA_DEFAULT    205 /* 0.2 (Q0) */

/* Q5 values */
#define RXD_CPICH_SNR_THRESH_1_DEFAULT    666 /* 20.8 dB */
#define RXD_CPICH_SNR_THRESH_2_DEFAULT    506 /* 15.8 dB */
#define RXD_CPICH_SNR_THRESH_3_DEFAULT    314 /*  9.8 dB */
#define RXD_CPICH_SNR_THRESH_4_DEFAULT    282 /*  8.8 dB */

/* ARD v2 filters settling time - L in frames */
#define RXD_ARD_V2_FILTER_SETTLE_TIME_L_DEFAULT 10

/* ARD v2 minimum dwell time with RxD off - K in frames */
#define RXD_ARD_V2_DWELL_TIME_K_DEFAULT  200

/* Fast Mode Diversity is not being used for now */
/* So setting default value for upper thresh (Thresh_1) < lower thresh (Thresh_2) */
#define RXD_PCH_FILTECIO_THRESH_1 (24) /* -12 dB*/
#define RXD_PCH_FILTECIO_THRESH_2 (22) /* -11 dB*/
#define RXD_PCH_PI_SUM_THRESH (0)
#define RXD_PCH_MIN_NUM_CYCLES_FULL_DIV_CTR (10)
/* Oride Values for RxD in PCH */
/* 0: No Oride
   1: All sleep cycles in No Diversity
   2: All sleep cycles in Full Diversity
   3: All sleep cycles in Fast Diversity
   4: All sleep cycles in Fast Diversity: Always wakeup Diversity after PICH
   5: All sleep cycles in Fast Diversity: Never wakeup Diversity after PICH
*/
#define RXD_PCH_ORIDE_VAL_DEFAULT (0)

/*NV item masks for RxD Opts decision*/
#define WL1_RXD_ENABLE_RXDPM_IN_CPC_MODE 0x1
#define WL1_RXD_SKIP_DRX_SHORT_SLEEP     0x2
#define WL1_RXD_SKIP_CPC_DRX             0x4
#define WL1_RXD_ENABLE_DSDS_QTA          0x8
#define WL1_RXD_ENABLE_DSDA_OMRD         0x10
#define WL1_RXD_DEFAULT_OPTS             (WL1_RXD_ENABLE_RXDPM_IN_CPC_MODE|WL1_RXD_ENABLE_DSDA_OMRD|WL1_RXD_ENABLE_DSDS_QTA)



/*************************************************************************** 
                            QICE defaults
****************************************************************************/
#define WNV_QICE_DEFAULT 0x33 /* Default NV value for external usage */

/* Enum to indicate supported release type */
typedef enum
{
  L1_NV_REL_INDICATOR_R99  = 0,
  L1_NV_REL_INDICATOR_REL5 = 1,
  L1_NV_REL_INDICATOR_REL6 = 2,
  L1_NV_REL_INDICATOR_REL7 = 3,
  L1_NV_REL_INDICATOR_REL8 = 4,
  L1_NV_NUM_REL_INDICATOR  = 5
} l1_nv_rel_ind_enum_type;

/* default NV release indicator value in case NV value is invalid */
#define L1_DEFAULT_NV_REL_IND L1_NV_REL_INDICATOR_REL8

/* CIR Enhancement NV setting */
#define WL1_CME_DEFAULT_CIR_INTERP_EN                  (TRUE)
#define WL1_CME_DEFAULT_CIR_ALPHA                      (0x00)
#define WL1_CME_DEFAULT_CIR_DOPPLER_EN                 (TRUE)
#define WL1_CME_DEFAULT_CIR_DOPPLER                    (0x64)
#define WL1_CME_DEFAULT_CIR_SIGMA_SCALE                (0x00)
#define WL1_CME_DEFAULT_CIR_CLEAN_THRESH               (0x14)

#define WL1_CME_DEFAULT_CIR_ALPHA_WRITTEN_TO_NV        (0xFF)
#define WL1_CME_DEFAULT_CIR_DOPPLER_WRITTEN_TO_NV      (0x3FF)
#define WL1_CME_DEFAULT_CIR_SIGMA_SCALE_WRITTEN_TO_NV  (0xFFFF)
#define WL1_CME_DEFAULT_CIR_CLEAN_THRESH_WRITTEN_TO_NV (0x3F)

/* Default NV value for MDSP Recovery. */
#define WL1M_MDSP_RECOVERY_DEFAULT_VALUE               (1)
/* Default NV value written. */
#define WL1M_MDSP_RECOVERY_DEFAULT_WRITTEN_VALUE       (0xFFFF)

/* Default NV value for MDSP Recovery Tracking. */
#define WL1M_MDSP_RECOVERY_STAT_DEFAULT_VALUE          (0)
/* Number of states tracked for MDSP Recovery stats. */
#define WL1M_MDSP_REC_NUM_STATES_TRACKED               (5)

/* CME Features NV setting. */
#define WL1_CME_DEFAULT_IRPM_EN                     (TRUE)
#define WL1_CME_DEFAULT_VLC_EN                      (TRUE)
#define WL1_CME_DEFAULT_PCOV_EN                     (FALSE)
#define WL1_CME_DEFAULT_QICE_CM_EN                  (TRUE)
#define WL1_CME_DEFAULT_QICE_TM_EN                  (FALSE)

#define WL1_CME_DEFAULT_CME_FEAT_WRITTEN_TO_NV       (0x7FFFFFF)

/*************************************************************************** 
                           Multi-SIM defaults
****************************************************************************/

/* W2G tuneaway configuration structure. */
typedef PACKED struct
{
  uint8   version;                        /* Structure version, starting at 0. */
  uint16  w2g_tuneaway_overhead_msec;     /* Tuneaway overhead time in msec.   */
  uint16  w2g_tuneaway_algorithm_type;    /* Tuneaway algorithm type.          */
  uint16  w2g_tuneaway_data_1;            /* Tuneaway config data #1.          */
  uint16  w2g_tuneaway_data_2;            /* Tuneaway config data #2.          */
  uint16  Reserved[4];                    /* For future use.                   */
} wl1_w2g_tuneaway_config_type;

/* Default W2G tuneaway overhead time, 48 milliseconds. */
#define WL1_W2G_TUNEAWAY_OVERHEAD_DEFAULT_MSEC         48
#define WL1_W2G_TUNEAWAY_OVERHEAD_DEFAULT_TIMETICKS  1573

#define WL1_W2G_TUNEAWAY_ALGORITHM_TYPE_DEFAULT         0
#define WL1_W2G_TUNEAWAY_DATA_1_DEFAULT                 4000  /* Minimum default GSM page repeat time of 4 seconds. */
#define WL1_W2G_TUNEAWAY_DATA_2_DEFAULT                 6000  /* Maximum default GSM page repeat time of 6 seconds. */

/* Same NV is used for ROTA and QBTA. Bit 0 is for ROTA and Bit 2 is for
   QBTA. ROTA and QBTA are allowed if the respective bits are set. Default 
   value is 0x5 as both ROTA and QBTA are allowed by default */
#define QTA_ROTA_DEFAULT_VAL 0x5

/*************************************************************************** 
            Default values (must be in format <NV_ID>_DEFAULT)
****************************************************************************/

/* RxD */
#define WNV_ID_L1_RXD_RSCP_THRESH_DEFAULT                 121 /* Minimum CPICH RSCP, corresponding to -121dBm */
#define WNV_ID_L1_RXD_QUAL_THRESH_DEFAULT                 125 /* 12.5 % which corresponds to (2^14 * 12.5/100)*/
#define WNV_ID_L1_RXD_TPR_THRESH_DEFAULT                  40  /* 40 which corresponds to -10 dBm */
#define WNV_ID_L1_RXD_ARD_THRESH_DEFAULT                  48
#define WNV_ID_L1_RXD_ARD_MMT_TIME_DEFAULT                10 /* subframes */
#define WNV_ID_L1_RXD_ARD_DWELL_TIME_DEFAULT              20 /* frames */
#define WNV_ID_L1_RXD_ARD_V2_DECISION_DELTA_DEFAULT       RXD_MMSE_TAP_ENG_DELTA_DEFAULT
#define WNV_ID_L1_RXD_ARD_V2_CPICH_THRESH1_DEFAULT        RXD_CPICH_SNR_THRESH_1_DEFAULT
#define WNV_ID_L1_RXD_ARD_V2_CPICH_THRESH2_DEFAULT        RXD_CPICH_SNR_THRESH_2_DEFAULT
#define WNV_ID_L1_RXD_ARD_V2_CPICH_THRESH3_DEFAULT        RXD_CPICH_SNR_THRESH_3_DEFAULT
#define WNV_ID_L1_RXD_ARD_V2_CPICH_THRESH4_DEFAULT        RXD_CPICH_SNR_THRESH_4_DEFAULT
#define WNV_ID_L1_RXD_ARD_V2_MIN_DWELL_TIME_K_DEFAULT     RXD_ARD_V2_DWELL_TIME_K_DEFAULT
#define WNV_ID_L1_RXD_ARD_V2_FILTER_SETTLE_TIME_L_DEFAULT RXD_ARD_V2_FILTER_SETTLE_TIME_L_DEFAULT
#define WNV_ID_L1_RXD_OPTS_DEFAULT                        WL1_RXD_DEFAULT_OPTS_WRITTEN
#define WNV_ID_L1_RXD_PCH_FILTECIO_UPPER_THRESH_DEFAULT   RXD_PCH_FILTECIO_THRESH_1
#define WNV_ID_L1_RXD_PCH_FILTECIO_LOWER_THRESH_DEFAULT   RXD_PCH_FILTECIO_THRESH_2
#define WNV_ID_L1_RXD_PCH_PI_SUM_THRESH_DEFAULT           RXD_PCH_PI_SUM_THRESH
#define WNV_ID_L1_RXD_PCH_MIN_FULL_DIV_CYCLES_DEFAULT     RXD_PCH_MIN_NUM_CYCLES_FULL_DIV_CTR
#define WNV_ID_L1_RXD_PCH_ORIDE_VAL_DEFAULT               RXD_PCH_ORIDE_VAL_DEFAULT

/* FET */
#define WNV_ID_L1_FET_CONTROL_DEFAULT                     WL1_FET_DED_EDv2_DEFAULT_WRITTEN_NV_VALUE

/* CME */
#define WNV_ID_L1_CME_OPTS_DEFAULT                        WL1_CME_DEFAULT_OPTS_WRITTEN_TO_NV
#define WNV_ID_L1_CME_QPM_AVG_RSSI_VAR_DEFAULT            WL1_CME_DEFAULT_MIN_AVG_RSSI_DELTA
#define WNV_ID_L1_CME_QPM_AVG_RSSI_VAR_RETAIN_DEFAULT     WL1_CME_DEFAULT_MIN_AVG_RSSI_RETAIN_DELTA

#define WNV_ID_L1_UL_RACH_DEFAULT                         0
#define WNV_ID_L1_ANT_SWITCH_DEFAULT                      0
#define WNV_ID_L1_ANT_SWITCH_BAND_DEFAULT                 0
#define WNV_ID_L1_SRCH_E1D_DEFAULT                        0
#define WNV_ID_L1_DSR_DEFAULT                             2
#define WNV_ID_L1_HARQ_FLUSH_TIMER_MAX_DEFAULT            HS_HARQ_BUF_FLUSH_TMR_MAX_VAL
#define WNV_ID_L1_UL_SAR_DEFAULT                          WCDMA_SAR_TX_PWR_FILT_TMR_DFLT_VAL

/* Legacy NV defaults*/
#define WNV_ID_BTFD_OLPC_FLOOR_DB_I_DEFAULT               0 /* dB */
#define WNV_ID_NON_BTFD_OLPC_FLOOR_DB_I_DEFAULT           0 /* dB */
#define WNV_ID_WCDMA_SPIKE_MITIGATION_ENABLED_I_DEFAULT   FALSE /* Inter-freq measurement optimization disabled */ 
#define WNV_ID_WCDMA_RRC_VERSION_I_FOR_L1_DEFAULT         L1_DEFAULT_NV_REL_IND/* TODO: Shared with L2/RRC */
#define WNV_ID_WCDMA_RX_DIVERSITY_CTRL_I_DEFAULT          0x3   /* External default */
#define WNV_ID_WCDMA_EQUALIZER_CTRL_I_DEFAULT             WNV_QICE_DEFAULT
#define WNV_ID_WCDMA_TEST_NV_1_I_DEFAULT                  0xFFFF
#define WNV_ID_BOARD_CAPABILITIES_I_DEFAULT               0xFF /* MIMO-specific */
#define WNV_ID_PREF_MODE_I_DEFAULT                        0

/* Placeholder for NV types to make memory allocations easier */
typedef union
{
  int16  rscp_thresh;
  int16  qual_thresh;
  int16  tpr_thresh;
  int16  ard_thresh;
  int16  ard_mmt_time;
  int16  ard_dwell_time;
  uint16 ard_v2_mmse_tap_eng_delta;
  int16  ard_v2_cpich_thresh1;
  int16  ard_v2_cpich_thresh2;
  int16  ard_v2_cpich_thresh3;
  int16  ard_v2_cpich_thresh4;
  uint16 ard_v2_min_dwell_time_k;
  uint8  ard_v2_filter_settle_time_l;
  wl1_rxd_opts_nv_struct_type rxd_opts;

  int16  pch_filtecio_upper_thresh;
  int16  pch_filtecio_lower_thresh;
  int16  pch_pi_sum_thresh;
  int16  pch_min_full_div_cycles;
  int16  pch_oride_val;
} wnv_rxd_default;

typedef union
{
  uint16                                    fet_control;
  uint32                                    cme_opts;
  uint32                                    cme_qpm_avg_rssi_var_thresh;
  uint32                                    cme_qpm_avg_rssi_var_retain_thresh;
  uint8                                     dsr_mode;
  boolean                                   dc_w2l_mode;
  uint8                                     harq_flush_timer_max;
  uint8                                     wcdma_qta_rota_nv;
  /* NV for MDSP Recovery. */
  uint16                                    wl1m_mdsp_recovery_nv_value;
  uint32                                    wl1m_mdsp_recovery_stat_per_state[WL1M_MDSP_REC_NUM_STATES_TRACKED];

  dl_nv_struct_type                         dl_nv;
  wl1_ul_nv_struct_type                     ul_sar;
  wl1_srch_e1d_enhancement_vars_struct_type srch_e1d;
  wl1_ant_switch_nv_struct_type             ant_switch;
  wl1_ant_switch_band_nv_struct_type        ant_switch_band;
  wl1_ul_rach_nv_config_type                ul_rach;
  wl1_w2g_tuneaway_config_type              w2g_tuneaway;
  wl1_atuner_nv_read_struct_type            atuner_nv;
  srchzz_idle_mode_opt_nv_struct_type       idle_mode_opt;
  drx_idle_mode_debug_nv_struct_type        idle_debug;
  wl1_cme_cir_enh_nv_struct_type            cire_nv;
  wl1_cme_features_nv_struct_type           cme_features_nv;
  wsrch_nbrej_nv_config_params              nbrej_init_params;
  srch_nv_struct_type                       srch_nv;
} wnv_l1_default;

typedef union
{
  int32   btfd_olpc_floor_db;
  int32   non_btfd_olpc_floor_db;
  uint8   board_capabilities;
  uint8   wcdma_rx_diversity_ctrl;
  uint32  wcdma_test_nv_1;
  uint8   wcdma_spike_mitigation_enabled;
  uint16  wcdma_equalizer_ctrl;
  l1_nv_rel_ind_enum_type wcdma_rrc_version;
} wnv_l1_legacy_default;

/*************************************************************************** 
                              NV default functions
****************************************************************************/
wnv_api_status wnv_default_rxd(wsub_id_e_type wsub_id, wnv_nv_id_enum nv_id, void* out_buffer, uint32 output_len, boolean write_to_file);
wnv_api_status wnv_default_l1(wsub_id_e_type wsub_id, wnv_nv_id_enum nv_id, void* out_buffer, uint32 output_len, boolean write_to_file);
wnv_api_status wnv_default_l1_legacy(wsub_id_e_type wsub_id, wnv_nv_id_enum nv_id, void* out_buffer, uint32 output_len, boolean write_to_file);

#endif /* _WNV_L1_H_*/
