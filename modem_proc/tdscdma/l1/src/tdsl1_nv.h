#ifndef TDS_L1_NV_H
#define TDS_L1_NV_H

/*============================================================================
              T D S L 1 N V  H E A D E R   F I L E

DESCRIPTION
This files contains all necessary macros and definitions for maintainaing
TDS L1 NV configuration database.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order. 
 
 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsl1_nv.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $
 

when        who     what, where, why
--------    ---     --------------------------------------------------------

                    
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "queue.h"
#include "nv.h"
#include "tdsl1macdata.h"

#define TDSL1_RXD_CONF_FILE "/nv/item_files/conf/tdscdma_l1.conf"
#define TDSL1_CONF_FILE_PERMISSIONS 0777

/* default values */
  #define TDSL1_NUM_ITERATION_DEFAULT                             5
  #define TDSL1_SFT_THRESHOLD_DEFAULT                             0.4
  #define TDSL1_MIM_NOISE_FLOOR_DEFAULT                           0.00125
  #define TDSL1_CODE_COND_FACTOR_DEFAULT                          5
  #define TDSL1_COND_FACTOR_DEFAULT                               10
  #define TDSL1_JD_MODE_SRV_SEL_DEFAULT                           2
  #define TDSL1_JD_INTF_DEFAULT                                   2
  #define TDSL1_SF_DET_ENABLED_DEFAULT                            TRUE
  #define TDSL1_SE_FILTER_ALPHA_DEFAULT                           0.375
  #define TDSL1_RMF_THRESHOLD1_DEFAULT                            2.5
  #define TDSL1_RMF_THRESHOLD2_DEFAULT                            3.5
  #define TDSL1_CORRELATION_THRESHOLD_DEFAULT                     -5.95
  #define TDSL1_SS_SIR_THRESHOLD_DEFAULT                          2
  #define TDSL1_SIR_TPC_HOLD_DEFAULT                              6
  #define TDSL1_TPC_THRESHOLD_UP_DEFAULT                          0
  #define TDSL1_TPC_THRESHOLD_DOWN_DEFAULT                        0
  #define TDSL1_FTL_MIDAMBLE_STEP_SIZE_DEFAULT                    0.2
  #define TDSL1_FTL_MIDAMBLE_THRESHOLD_FILTERED_SINR_DB_DEFAULT   3
  #define TDSL1_FTL_DWPTS_STEP_SIZE_DEFAULT                       0.2
  #define TDSL1_FTL_DWPTS_THRESHOLD_FILTERED_SINR_DB_DEFAULT      3
  #define TDSL1_TTL_MIDAMBLE_STEP_SIZE_DEFAULT                    0.1
  #define TDSL1_TTL_MIDAMBLE_THRESHOLD_FILTERED_SINR_DB_DEFAULT   3
  #define TDSL1_TTL_DWPTS_STEP_SIZE_DEFAULT                       0.1
  #define TDSL1_TTL_DWPTS_THRESHOLD_FILTERED_SINR_DB_DEFAULT      3
  #define TDSL1_IIRCOEFF_PER_SUB_FRAME_DEFAULT                    0.1
  #define TDSL1_IIR_COEFF_MAX_DEFAULT                             0.4
  #define TDSL1_MAX_NUM_ITERATIVE_CELL_TSO_DEFAULT                4
  #define TDSL1_MAX_NUM_ITERATIVE_CELLS_NON_TSO_DEFAULT           4
  #define TDSL1_MAX_NUM_TOTAL_CELLS_TSO_DEFAULT                   40
  #define TDSL1_MAX_NUM_TOTAL_CELLS_NON_TSO_DEFAULT               13
  #define TDSL1_JSD_MIN_VALUE_DBM_DEFAULT                         -32768

  #define TDSL1_JDS_THRESHOLD_TSRSP_ROUND_ROBIN_DEFAULT          -28160
  #define TDSL1_JDS_THRESHOLD_FROM_SEARCH_TSRSP_DEFAULT          -25600
  #define TDSL1_JDS_THRESHOLD_TIMING_SUBFRAME                    6000
  
  #define TDSL1_ENABLE_CQI_OL_DEFAULT                             TRUE
  #define TDSL1_TRACKING_LOOP_FLAG_FECA_DEFAULT                   0

  /* measurement parameters */
  #define TMIN_SP_PRIF  	   10
  #define TMAX_SP_PRIF       40
  #define TMIN_SP_CANF       40
  #define TMAX_SP_CANF       60
  #define TMIN_SP_REMF       40
  #define TMAX_SP_REMF  	   200
  #define TMIN_SP_GSM_RSSI   20
  #define TMAX_SP_GSM_RSSI   160
  #define REX_TIMER          500

  /* handover parameters */
  #define TDSSRCHDCH_EVT_1G_P1 	    6
  #define TDSSRCHDCH_EVT_1G_P2	    15
  #define TDSSRCHINTERF_EVT_2A_P1 	6
  #define TDSSRCHINTERF_EVT_2A_P2	  15
  #define TDSSRCHGSM_EVT_3A_P1  	  6
  #define TDSSRCHGSM_EVT_3A_P2  	  15
  #define TDSSRCHDCH_EVT_1G_P3   	  12
  #define TDSSRCHINTERF_EVT_2A_P3   12
  #define TDSSRCCHGSM_EVT_3A_P3	    12
  #define TDSSRCHINTRAF_EVT_1G_TXPOWER 2
  #define TDSSRCHINTERF_EVT_2A_TXPOWER 2
  #define TDSSRCHGSM_EVT_3A_TXPOWER 2
 

  /* CELL RESEL PARAMETERS */

  #define TINTRA_MEAS_FRESH_INTERVAL 200
  #define TINTER_MEAS_FRESH_INTERVAL 400

  #define QUALMIN_1	3
  #define QUALMIN_2 	0
  #define QUALMIN_4	-12
  #define QIMMEDIATE_CR	-20
  #define TFACH_CELL_EVAL_PERIOD	400

  /* SEARCHER PARAMETERS */
  #define RAW_SCAN_THRESHOLD 	-103
  #define FINE_SCAN_THRESHOLD -103
  #define LIST_FREQ_SCAN_THRESHOLD 	-98
  #define RAW_SCAN_THRESHOLD_BAND_F 	-103
  #define FINE_SCAN_THRESHOLD_BAND_F -103

  /* HSDPA drop NV default value */
  #define HSDPA_DROP_RSCP_THRESHOLD_DEFAULT 	100   /*Threshold default value: -100dbm*/
  #define HSDPA_DROP_PERIOD_DEFAULT 	160        /*Period default value: 160ms */

  /* MTPL power control NV default value */
  #define MTPL_STEP_SIZE_DEFAULT 	        5   /*5 dB*/
  #define MTPL_STEP_PERIOD_DEFAULT 	        10000   /*10000 ms*/
  #define MTPL_TIMER_UP_DEFAULT 	        50   /*50 ms*/
  #define MTPL_TIMER_DOWN_DEFAULT 	        400   /*400 msÅ´*/
  #define MTPL_MAX_REDUCE_DEFAULT 	        255   /*255 dB*/

  /* Fast HO control type 2 NV default value */
  #define TDSSRCHDCH_EVT_1G2A3A_SWITCH_DEFAULT 	        TRUE   /*Boolean switch default value: 1(TRUE)*/
  #define TDSSRCHDCH_EVT_1G_MR_REPEAT_NUMBER_DEFAULT  1
  #define TDSSRCHDCH_EVT_2A_MR_REPEAT_NUMBER_DEFAULT  1
  #define TDSSRCHDCH_EVT_1G2A_DELAY_AFTER_3A_DEFAULT  256
  #define TDSSRCHDCH_EVT_1G_MR_REPEAT_INTERVAL_DEFAULT  600
  #define TDSSRCHDCH_EVT_2A_MR_REPEAT_INTERVAL_DEFAULT  600
  #define TDSSRCHDCH_EVT_1G2A_RSCP_MIN_THRESHOLD_DEFAULT  -104
  #define TDSSRCHDCH_EVT_DELAY_FAST_HO_AFTER_SETUP_RECONFIG_DEFAULT  100

    /* T2L CM MEAS Periodicty  NV default value */
  #define TDSSRCHLTE_CM_SRCH_MIN_PERIODICITY_DEFAULT 	        2   
  #define TDSSRCHLTE_CM_SRCH_G_PRESENT_DEFAULT                  1
  #define TDSSRCHLTE_CM_SRCH_V_FACTOR_DEFAULT                   4
  #define TDSSRCHLTE_CM_SRCH_MIN_PERIODICITY_SGLTE_DEFAULT		1   
  #define TDSSRCHLTE_CM_SRCH_G_PRESENT_SGLTE_DEFAULT		    0
  #define TDSSRCHLTE_CM_SRCH_V_FACTOR_SGLTE_DEFAULT 		    9

  #define TDSSRCHMEAS_TWAIT_DOMAIN_FACOTR_ANY_CS_DEFAULT        20
  #define TDSSRCHMEAS_TWAIT_DOMAIN_FACOTR_PS_ONLY_DEFAULT       4
#ifdef FEATURE_TDSCDMA_REPEAT_TO_SECOND_BEST 
  #define TDSSRCHDCH_EVT_1G_MR_REPEAT_CELL_NUM_DEFAULT  2
  #define TDSSRCHDCH_EVT_2A_MR_REPEAT_CELL_NUM_DEFAULT  2
  #define TDSSRCHDCH_EVT_1G_MR_REPEAT_TIMES  3
  #define TDSSRCHDCH_EVT_2A_MR_REPEAT_TIMES  3
#endif

  /* CELL SELECTION PARAMETERS */
  #define QSELECT_MIN -105

  /*Default RDDS parameters*/
  #define BLER_HIGH_THRESHOLD 2458
  #define BLER_LOW_THRESHOLD 983
  #define BLER_ALPHA 6
  #define TIMER_TRANS 2 
  #define SIR_TARGET_THRESHOLD 12288
  #define SPECIAL_BURST_QUALITY_THRESHOLD_LOW 4096
  #define SPECIAL_BURST_QUALITY_THRESHOLD_HIGH 8192
  #define TIME_SPAN_SPECIAL_BURST_QUAL 2
  #define SIR_TARGET_SET_POINT 2 

  /*RxDPM NV default value*/
  
  #define SCCH_SIR_TARGET 9216  /*16Q10 format: 9db*/
  #define SCCH_ACTIVITY_THRESHOLD 819 /*16Q13 format 10%*/
  #define SCCH_ACTIVITY_THRESHOLD_2 82 /*16Q13 format 1%*/
  #define SCCH_ACTVITY_FILTER_BETA 3
  
  /*ARD NV defaul value*/
  #define ARD_ANT_IMBALANCE_THRESHOLD 15360
  #define ARD_ANT_HIGH_CORR_THRESHOLD 13107
  #define MMSE_PWC_MODE_SEL_THRESHOLD 13107

  
  /*RDDS ARD NV default value*/
  #define ARD_IMBALANCE_THRESH   205  /*0.2 (-7dB) in 16Q10 */
  #define WEAK_SNR_THRESH_RX0    65535  /* disabled */
  #define WEAK_SNR_THRESH_RX1    65535  /* disabled */
  #define SNR_IIR_FILER_COEFF    4256 /*0.12987 IN 16Q15*/ 
  #define SAMPLE_NUM             20
  #define SAMPLE_EXPIRE_TIME     200
  #define T_ARD                  20

  /*IDLE RXD NV default value*/

  /*SIR (linear value in in unsigned 16Q6 format*/
  #define IDLE_SIR_THRESHOLD_LOW   25    /*-4db*/
  #define IDLE_SIR_THRESHOLD_HIGH  101   /*2db*/
  /*RSCP in S16Q8 format: -90dbm and -80 dbm*/
  #define IDLE_RSCP_THRESHOLD_LOW   -23040
  #define IDLE_RSCP_THRESHOLD_HIGH   -20480
  #define IDLE_FILTER_APHA    3
  #define PCH_CRC_TIMER     10

  /* Add modules whose F3s are allowed by default to this mask.
   The allowed mask can also be controlled by EFS file "f3_mask". */ 
  #define TDSL1_MSG_MASK_DEFAULT 0
  /* NV control for clock setting */
  #define TDSL1_NV_CONTROL_LEVEL_DEFAULT 0

/* special settings for test */
#define SPECIAL_SETTING_TESTING_MODE_DEFAULT 0

/* NV control for timeline profiling */
  #define TDSL1_NV_TIMELINE_PROFILE_MODE_DEFAULT 0
#define TDSSRCHGSM_E3A_OPT_TD_THRESHOLD_DEFAULT -100
#define TDSSRCHGSM_E3A_OPT_GSM_THRESHOLD_DEFAULT -90
#define TDSSRCHGSM_E3A_OPT_TD_THRESHOLD_FOR_CS_ONLY_DEFAULT -116
#define TDSSRCHLTE_OPT_Q_RXLEVMIN_EUTRA_DEFAULT -140

/* Default values for adaptive wake up timeline feature*/
#define TDSL1SLEEP_ADAPTIVE_TARGET_PICH_MISS_RATE_DEFAULT 1
#define TDSL1SLEEP_ADAPTIVE_CONVERGE_RATE_FAST_DEFAULT 100
#define TDSL1SLEEP_ADAPTIVE_CONVERGE_RATE_SLOW_DEFAULT 10
#define TDSL1SLEEP_ADAPTIVE_CONVERGE_RATE_SWITCH_THRESHOLD_DEFAULT 100
#define TDSL1SLEEP_ADAPTIVE_STEP_SIZE_IN_INITIAL_MODE_DEFAULT 1000

/* *************option 1************** */

/* Demfront Parameters: FW */
typedef PACKED struct PACKED_POST{ 
  /* Number of CE iterations when number of JDS cells  */
  uint8                                          num_iteration;
  /* Active shift detection threshold */
  int16                                          sft_threshold;
  int16                                          min_noise_floor;
  /* List of ordered indices to acquisition entries */
  uint8                                          code_cond_factor;
  uint8                                          cond_factor;
  uint8                                          jd_mode_srv_sel;
  uint8                                          jd_intf;
  boolean                                        sf_det_enabled;

} tdsl1_nv_fw_demfront_params_type;

/* CQI reporting */
typedef PACKED struct PACKED_POST{ 

  int16                                          se_filter_alpha;
  int16                                          rmf_threshold1;
  /* List of ordered indices to acquisition entries */
  uint8                                          rmf_threshold2;
} tdsl1_nv_fw_cqi_params_type;

/* ehich decoding : FW*/
typedef PACKED struct PACKED_POST{ 

  int16                                          correlation_threshold;
 
} tdsl1_nv_fw_ehich_params_type;

/* ehich decoding : FW*/
typedef PACKED struct PACKED_POST{ 

  int16                                          ss_sir_threshold;
 
} tdsl1_nv_ss_params_type;

/* DL TPC : FW*/
typedef PACKED struct PACKED_POST{ 

  int16                                          sir_tpc_hold;
  int16                                          tpc_threshold_up;
  int16                                          tpc_threshold_down;
 
} tdsl1_nv_fw_dl_tpc_params_type;

/* Tracking loops parameters: FW*/
/* Required 2 for midable and DwPTS.*/
typedef PACKED struct PACKED_POST{ 

  float  step_size;
  int16  threshold_filtered_sinr_db;
} tdsl1_nv_fw_ftl_tracking_loop_params_type;

/* Required 2 for midable and DwPTS.*/
typedef PACKED struct PACKED_POST{ 

  float  step_size;
  int16  threshold_filtered_sinr_db;
} tdsl1_nv_fw_ttl_tracking_loop_params_type;


/* Tracking loops parameters: SW*/
typedef PACKED struct PACKED_POST{ 
  boolean tracking_loop_flag_feca;
} tdsl1_nv_sw_tracking_loop_params_type;

/* CQI reporting for SW */
typedef PACKED struct PACKED_POST{ 
  boolean enable_cqi_ol; 
  uint16 se_filter_reset_threshold;
  int16 sir_min;
  uint16 sir_max;
  uint16 target_bler;
  uint16 down_step_size;
  uint16 reset_bler_threshold_m;
} tdsl1_nv_sw_cqi_params_type;

/* JDS management */  
/* NOTE: NV tools team has this struct */
typedef PACKED struct PACKED_POST{ 
  uint8 version;
  uint8 max_num_iterative_cells_tso;
  uint8 max_num_iterative_cells_non_tso;
  uint8 max_num_total_cells_tso;
  uint8 max_num_total_cells_non_tso;
  int16 min_value_dbm;
  double  iir_coeff_per_subframe;
  double  iir_coeff_max; 
  uint8 reserved[2];

} tdsl1_nv_sw_jds_management_params_type;

typedef PACKED struct PACKED_POST{ 
  uint8 version;
  int16 threshold_tsrsp_round_robin_dbm;
  int16 from_search_tsrsp_dbm;
  uint16 threshold_timing_subframe;
  uint8 reserved[10];
  
} tdsl1_nv_jds_management_threshold_params_type;

typedef PACKED struct PACKED_POST{ 
  uint8 version;
  boolean rxd_enable;
  boolean rxd_rdds_enable;
  uint32 bler_high_threshold;
  uint32 bler_low_threshold;
  uint8 bler_alpha ;
  uint8 timer_trans;
  int16 sir_target_threshold ;
  int16 special_burst_quality_threshold_low;
  int8 sir_target_set_point ;
  uint8 time_span_special_brst_qual ;
  
  int16 special_burst_quality_threshold_high ;
  uint8 reserved ;
  
} tdsl1_nv_rxd_params_type;


typedef PACKED struct PACKED_POST{ 
    uint8 version ;
    uint8  beta_for_scch_rate_filter;
    uint16 HS_SCCH_SIR_Target ;
    uint16 HS_SCCH_low_activity_threshold;
    uint16  ARD_ant_imbalance_threshold;
    uint16  ARD_ant_high_corr_theshold;
    uint16  threshold_for_MMSE_and_PWC_selection;
    int16 idle_sir_threshold_high ;
    int16 idle_sir_threshold_low ;
    int32 idle_rscp_threshold_high ;
    int32 idle_rscp_threshold_low ;
    uint8 idle_filter_alpha ;

    
    uint8 feature_control_bit_mask;
    uint16  HS_SCCH_low_activity_threshold_2;
    uint8   pch_crc_error_timer;
    uint8 reserved;


 
} tdsl1_nv_rxdpm_ard_idleRxD_params_type;


typedef PACKED struct PACKED_POST{ 
  uint8 version;
  uint16 imbalance_threshold;
  uint16 weak_snr_thresh_rx0;
  uint16 weak_snr_thresh_rx1;
  uint16 snr_IIR_filter_coefficient ;
  uint16 sample_num ;
  uint16 sample_expire_time ;
  uint16 timer_ard ;
  uint8 reserved[20] ; 
 
  
} tdsl1_nv_rdds_ard_params_type ;

/* measurement parameters */
typedef PACKED struct PACKED_POST{ 
  uint8 tmin_sp_priF;
  uint8 tmax_sp_priF; 

  uint8 tmin_sp_canF;
  uint8 tmax_sp_canF;

  uint8 tmin_sp_remF;
  uint8 tmax_sp_remF;
  uint8 tmin_sp_gsm_rssi;
  uint8 tmax_sp_gsm_rssi;
  int16 REX_timer;
 
} tdsl1_nv_meas_params_type;

/* searcher parameters */
typedef PACKED struct PACKED_POST{ 
  int16 raw_scan_threshold;
  int16 fine_scan_threshold;
} tdsl1_nv_searcher_params_type;

/* searcher params extent */
typedef PACKED struct PACKED_POST{
/* Freq Scan parameters */
  int8 list_freq_scan_threshold;
  int8 raw_scan_threshold_bandF;
  int8 fine_scan_threshold_bandF;
  int8 reserve[5];
} tdsl1_nv_searcher_params_ext_type;
/* HSDPA force drop parameters */
typedef PACKED struct PACKED_POST{ 
  int16 hsdpa_drop_threshold;
  uint16 hsdpa_drop_period;
  uint8 reserved[4] ;  // to reserve space for future use
} tdsl1_nv_hsdpa_drop_params_type;

/* MTPL power control*/
typedef PACKED struct PACKED_POST{
	uint8 version;
   /*mtpl reduce power step size*/
   uint16 step_size;
   /*the period to change the mtpl reduce power step*/
   uint16 step_period;
   /*timer counting when mtpl at normal level*/
   uint16 timer_up;
   /*timer counting when mtpl at reduce level*/
   uint16 timer_down;
   /*max reduce power of mtpl*/
   uint16 max_reduce_mtpl;

   uint8 reserved[4] ;  // to reserve space for future use
} tdsl1_nv_mtpl_control_params_type;

/* Fast HO control type 2 */
typedef PACKED struct PACKED_POST{
   /*enable/disable RSCP based 1g fast HO triggering. True is enable*/
   boolean tdssrchdch_evt_1G_RSCP_fast_HO_Feature_Switch;
   /*enable/disable TS-RSP based 1g fast HO triggering. True is enable */ 
   boolean tdssrchdch_evt_1G_TSRSP_fast_HO_Feature_Switch;
   /*enable/disable SIRtarget based 1g fast HO triggering. True is enable */ 
   boolean tdssrchdch_evt_1G_SIRTARGET_fast_HO_Feature_Switch;
   /*enable/disable UL Tx power based 1g fast HO triggering. True is enable */
   boolean tdssrchdch_evt_1G_UL_TxPower_fast_HO_Feature_Switch;
   /*enable/disable RSCP based 2a fast HO triggering. True is enable */
   boolean tdssrchdch_evt_2A_RSCP_fast_HO_Feature_Switch;
   /*enable/disable TS-RSP based 2a fast HO triggering. True is enable */
   boolean tdssrchdch_evt_2A_TSRSP_fast_HO_Feature_Switch;
   /*enable/disable SIRtarget based 2a fast HO triggering. True is enable */
   boolean tdssrchdch_evt_2A_SIRTARGET_fast_HO_Feature_Switch;
   /*enable/disable UL Tx power based 2a fast HO triggering. True is enable */
   boolean tdssrchdch_evt_2A_UL_TxPower_fast_HO_Feature_Switch;
   /*enable/disable RSCP based 3a fast HO triggering. True is enable */
   boolean tdssrchdch_evt_3A_RSCP_fast_HO_Feature_Switch;
   /*enable/disable TS-RSP based 3a fast HO triggering. True is enable */
   boolean tdssrchdch_evt_3A_TSRSP_fast_HO_Feature_Switch;
   /*enable/disable SIRtarget based 3a fast HO triggering. True is enable */
   boolean tdssrchdch_evt_3A_SIRTARGET_fast_HO_Feature_Switch;
   /*enable/disable UL Tx power based 3a fast HO triggering. True is enable */
   boolean tdssrchdch_evt_3A_UL_TxPower_fast_HO_Feature_Switch;
   /*enable/disable starting TTT timer earlier feature. True is enable */
   boolean tdssrchdch_evt_1G2A_TTT_earlier_fast_HO_Feature_Switch;
   /*number to repeat 1g MR message */
   int16 tdssrchdch_evt_1G_MR_Repeat_Number;
   /*number to repeat 2a MR message */
   int16 tdssrchdch_evt_2A_MR_Repeat_Number;
   /*the delay time to send 1G2A after send 3a MR */
   int16 tdssrchdch_evt_1G2A_Delay_after_3A;
   /*interval between repeated 1g MR message */
   int16 tdssrchdch_evt_1G_MR_Repeat_Interval;
   /*interval between repeated 2a MR message */
   int16 tdssrchdch_evt_2A_MR_Repeat_Internal;
   /*the Min RSCP threshold for 1g, 2a target cell. 
     If 1g, 2a target cell RSCP is lower than the threshold, 1g, 2a will not be triggered. In dBm. */
   int16 tdssrchdch_evt_1G2A_RSCP_Min_Threshold;
   /*delay time to enable UL Tx power based and 
     SIRtarget based fast HO after in_sync of Setup or Reconfiguration procedure */
   int16 tdssrchdch_evt_Delay_Fast_HO_After_Setup_Reconfig;

#ifdef FEATURE_TDSCDMA_REPEAT_TO_SECOND_BEST 
   /*number of times each cell can repeat 1g MR message */
   uint8 tdssrchdch_evt_1G_MR_Repeat_Times;
   /*number of times each cell can repeat 2a MR message */
   uint8 tdssrchdch_evt_2A_MR_Repeat_Times;
   /*number of cells which can repeat 1g MR message */
   uint8 tdssrchdch_evt_1G_MR_Repeat_Cell_Num;
   /*number of cells which can repeat 2a MR message */
   uint8 tdssrchdch_evt_2A_MR_Repeat_Cell_Num;
#else
   uint8 reserved[4] ;  // to reserve space for future use
#endif
} tdsl1_nv_fast_ho_control_struct_type2;

/* cell reselection parameters */
typedef PACKED struct PACKED_POST{ 
  int16 tintra_meas_fresh_interval;
  int16 tinter_meas_fresh_interval;
  int16 q_qualmin1;
  int16 q_qualmin2;
  int16 q_qualmin4;
  int16 q_immediateCR;
  int16 t_fach_cell_eval_period;

} tdsl1_nv_cell_resel_params_type;

/* cell selection parameters */
typedef PACKED struct PACKED_POST{ 
  int16 q_select_min;

} tdsl1_nv_cell_sel_params_type;

/* HO parameters */
typedef PACKED struct PACKED_POST{ 
  int16 tdssrchdch_evt_1G_P1;
  int16 tdssrchdch_evt_1G_P2;
  int16 tdssrchinterf_evt_2A_P1;
  int16 tdssrchinterf_evt_2A_P2;
  int16 tdssrchgsm_evt_3A_P1; 
  int16 tdssrchgsm_evt_3A_P2; 
  int16 tdssrchdch_evt_1G_P3;   
  int16 tdssrchinterf_evt_2A_P3;
  int16 tdssrchgsm_evt_3A_P3;
  
} tdsl1_nv_HO_params_type;

/* DRX T2G meas control */
typedef PACKED struct PACKED_POST{ 
  int16 sir_interf_thresh;
  int16 rscp_interf_thresh_1;
  int16 rscp_interf_thresh_2;
  int16 rscp_t2g_thresh1;

  uint8 disable_meas;
  uint8 reserved[7] ;  // to reserve space for future use
} tdsl1_nv_drx_t2g_control_struct_type;

/* fast HO */
typedef PACKED struct PACKED_POST{ 
  int16 tds_interf_evt_1g_txpower;
  int16 tds_interf_evt_2a_txpower;
  int16 tds_interf_evt_3a_txpower;
  uint8 tds_interf_evt_1g_sirtarget_offset;
  uint8 tds_interf_evt_2a_sirtarget_offset;
  uint8 tds_interf_evt_3a_sirtarget_offset;
  uint8 tds_rlc_signalingPDU_repeat_num;
  uint8 tds_rlc_signalingPDU_boost_pwr;
  uint8 tds_rlc_signalingPDU_boost_sirtarget;
  boolean internal_3a_enable;
  int16 internal_3a_own_sys_thresh;
  int16 internal_3a_other_sys_thresh;

  uint32 reserved[4] ;  // to reserve space for future use
} tdsl1_nv_fast_ho_control_struct_type;

typedef PACKED struct PACKED_POST{ 
  uint8 version;
  uint8 tdseul_disable_uph_check;  // TRUE:disable UPH check; FALSE:enable UPH check when select E-TFCI
  uint8 tdseul_enable_olpc_opt; // OLPC enable flag
  uint8 tdseul_olpc_type; // 0: fix BLER OLPC; 1: adaptive BLER OLPC
  uint16 tdseul_olpc_opt_bler_tgt_1;
  uint16 tdseul_olpc_opt_bler_tgt_2;
  uint16 tdseul_olpc_tpc_adj_step;
  int16 tdseul_olpc_tpc_adj_min;
  int16 tdseul_olpc_tpc_adj_max;
  int8 tdseul_olpc_pwr_th_1;
  int8 tdseul_olpc_pwr_th_2;
  int8 mtpl_for_uph;
  int8 mtpl_for_etfci_sel;
  uint8 tdseul_enable_pebase_freeze;
  uint8 tdseul_pebase_freeze_low_boundary;
  uint16 tdseul_snpl_opt_alpha;
  uint8 tdseul_enable_snpl_opt;
  uint8 reserved[9];
} tdsl1_nv_hsupa_params_type;

/* L1M NV for test control */
typedef PACKED struct PACKED_POST{ 
  uint8  version;
  uint8  reserved1;
  /*TL1 measure feature bitmap 
    Bit 3: Enable T2G measurement (1: enalbe, 0: disable)
    Bit 2: Enable T2G measurement in FACH (1: enalbe, 0: disable)
    Bit 1: Enable T2G measurement in DCH (1: enalbe, 0: disable)
    Bit 0: Enable T2G measurement in IDLE (1: enalbe, 0: disable)*/
  /*
    Bit 4: Enable T2L measurement in Idle (1: enable, 0: disable)
    Bit 5: Enable T2L measurement in DCH(1: enable, 0: disable)
    */
  uint16 measurement_feature_bitmap;
  uint32 tdsl1m_nv_bit_map;
} tdsl1m_nv_control_type;

/* tdsl1_uppch_tadv_optimization */
typedef PACKED struct PACKED_POST{ 
  uint16 uppch_tadv_step_size;   /*tadv step size in talbe, Range: 8,16,24 Tc*/
  int16  filter_coefficient;   /*IIR filter coefficient ( linear value, signed Q14)*/
  int16  success_rate_init;    /*if this flag is 1, need to init success rate table*/
  int16  success_rate_table[7]; /*Q14, range:0...2^14.  success rate for each TA, For index0, TA=0, For index i, TA= i*uppch_ta_step_size*/
  int16  feature_enable;  /*0:disable;  1:enable*/
  int16  reserved;           /*reserved for future usage*/
} tdsl1_nv_uppch_tadv_opt_params_type;

/* NV control for clock setting */
typedef PACKED struct PACKED_POST{ 
  uint16 level; /* 0x0000 default (power saving mode); 
                   0x0001 high clock for all case (non power saving mode)  
                   0x0002 static mode(disable MCVS Dynamic Power Control)
                   0x0003 static mode(disable MCVS Dynamic Power Control) but high clock for all cases*/
  uint16 reserved_1;
  uint16 reserved_2;
  uint16 reserved_3;
} tdsl1_nv_clock_control_type;

/* special settings for test */
typedef PACKED struct PACKED_POST{ 
  uint8 test_mode;

} tdsl1_nv_special_setting_for_test_type;

/* Antenna Switch */
typedef PACKED struct PACKED_POST{ 
    uint8  version;
	boolean tdssrch_rscp_antenna_switch_enable;
    uint8 band_A_algorithm_type;
    uint8 band_E_algorithm_type;
    uint8 band_F_algorithm_type;
	uint16 T1_probe_time;
	uint8 num_probe_cycles;
	uint16 base_waiting_time;
	int16 tdssrch_rscp_antenna_switch_threshold_db_x256;
	int8  Look_Back_Threshold_dB;
	uint16 look_back_time;
	uint8 MTPL_ratio_threshold;
	int8 preferred_antenna_rscp_threshold;
	int8 idle_safe_condition_rscp;
	int8 traffic_safe_condition_rscp;
	uint8 tds_traffic_crisis_mode_thre;
	uint8 tds_idle_pingpong_mitigation_thre;
	uint8 tds_idle_suspension_time;
	
	int16 reserved;  // to reserve space for future use
} tdsl1_nv_antenna_switch_struct_type;

typedef PACKED struct PACKED_POST{ 
  uint8 version;
  int8 type1_RSCP_threshold;
  uint8 type1_lookback_threshold;
  uint8 Tp;
  uint8 type1_switch_threshold;
  uint8 type1_switchBack_threshold;
  uint8 type1_switch_holding_time;
  int16 reserved[8];  // For future use
} tdsl1_nv_asdiv_type1_algo_struct_type;

/*T2L CM SRCH periodity */
typedef PACKED struct PACKED_POST{ 
  uint8 srch_min_periodicity; 	   
  uint8 g_present;
  uint8 v_factor;  
} tdsl1_nv_t2l_cm_meas_periodicity_params_type;

/*T2L CM SRCH periodity */
typedef PACKED struct PACKED_POST{ 
  uint8 srch_min_periodicity; 	   
  uint8 g_present;
  uint8 v_factor;  
} tdsl1_nv_t2l_cm_meas_periodicity_sglte_params_type;

/*T2L CM gap policy */
typedef PACKED struct PACKED_POST{ 
  uint8 twait_domain_factor_any_cs; 	   
  uint8 twait_domain_factor_ps_only; 
}tdsl1_nv_cm_meas_gap_policy_params_type;

/* NV control for timeline profiling */
typedef PACKED struct PACKED_POST{ 
  uint32 mode_bitmap; 
  /* 0x0000 ñ default: no timeline profiling;
     0x0001 ñ DRX timeline profiling mode;
     from bit1 -bit 31: l1m profiling modules */
} tdsl1_nv_timeline_profile_params_type;

extern tdsl1_nv_timeline_profile_params_type tdsl1_timeline_profile_params;

/* Add NV control for FR 15544: TD-SCDMA UL power control mechanism to assist beam forming */
typedef PACKED struct PACKED_POST{ 
  uint8 enable_tx_pwr_boost;    /* 0: disable, 1: enable */
  uint8 tx_pwr_boost_step_size; /* Step size of tx power boost */
  uint8 snr_diff_threhold;      /* The threshold of SNR difference */
} tdsl1_nv_tx_pwr_boost_params_type;
typedef PACKED struct PACKED_POST{ 
	int8 tdssrchgsm_e3a_opt_td_threshold;
	int8 tdssrchgsm_e3a_opt_gsm_threshold;
   int8 tdssrchgsm_e3a_opt_td_threshold_for_CS_only;
   int8 reserved_2;
} tdsl1_nv_e3a_threshold_opt_params_type;
typedef PACKED struct PACKED_POST{ 
	int16 tdssrchlte_q_rxlevmin_eutra_opt;
	int8 reserved_1;
   int8 reserved_2;
} tdsl1_nv_t2l_threshold_opt_params_type;

/* IFREQ Qtuner dynamic script upate */
typedef PACKED struct PACKED_POST{ 
	uint8 version;
	boolean enable;
	int16 Reserved[4];	// For future use
} tdsl1_ifreq_qtuner_update_params_type;

/*ACQ L1 Cell DB for ACQ Enhancement*/
typedef PACKED struct PACKED_POST{ 
  int8 tdssrchacq_high_threshold_shallow;    
  int8 tdssrchacq_low_threshold_shallow; 
  int8 tdssrchacq_high_threshold_deep;  
  int8 tdssrchacq_low_threshold_deep;      
} tdsl1_acq_cell_db_threshold_params_type;

/* TDSL1 NV power test params */
typedef PACKED struct PACKED_POST{
  uint8 version;
  uint8 enable_pch_det;
  uint8 pccpch_snr_thresh;
  uint8 pccpch_iir_filter;
  uint8 enable_sensor_aid;
  uint8 paging_rate;
  uint8 reserved[10];
} tdsl1_nv_pwr_test_params_type;


/*NV item for DRX mode*/
typedef PACKED struct PACKED_POST{
   // Sleep enabled or not, defualt is TRUE.
   boolean sleep_enabled;
   // The target miss rate, defualt value: 0.001(real value) * 1000
   uint16 target_pich_miss_rate;
   // The fast converge rate in tracking mode, default value: 0.1(real value) * 1000
   uint16 converge_rate_fast;
   // The slow converge rate in tracking mode, default value: 0.01(real value) * 1000
   uint16 converge_rate_slow;
   /* The number of DRX cycles when the converge rate from fast to slow in tracking mode
   default value: 100*/
   uint16 converge_rate_switch_threshold;
   // The number of Chip8 for step size in initial mode
   uint16 step_size_of_initial_mode;
   // Reserve field for future use
   uint32 reserve_1;
   uint32 reserve_2;
   uint32 reserve_3;
}tdsl1_nv_drx_params_type;

/* UL Test Control Parameters */
typedef PACKED struct PACKED_POST{ 
	uint8 version;
  uint8 tx_power_diff_thres;
	int16 Reserved[4];	// For future use
} tdsl1_nv_ul_test_control_params_type;
extern tdsl1_nv_drx_params_type tdsl1_nv_drx_params;

/* NV configuration for F3 messages using EFS files */
extern uint32                             f3_mask_cfg;

extern tdsl1_nv_searcher_params_type tdsl1_searcher_params;
extern tdsl1_nv_searcher_params_ext_type tdsl1_searcher_params_ext;

extern tdsl1_nv_hsdpa_drop_params_type tdsl1_hsdpa_drop_params;

/*===========================================================================
FUNCTION tdsl1_create_nv_item

DESCRIPTION
  This function creates perfomance enhancement related NV items and sets to
  default value
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void tdsl1_create_nv_item(void);


/*===========================================================================

FUNCTION TDSL1_F3_MASK_EFS_CFG

DESCRIPTION
  Reads F3 mask configuration from EFS
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
extern void tdsl1_read_nv_items(void);


/*===========================================================================
FUNCTION tdsl1_get_cqi_params_nv_item

DESCRIPTION
  This function reads the NV items and returns the CQI outer loop parameters
  related configuration.
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern tdsl1_nv_sw_cqi_params_type* tdsl1_get_cqi_params_nv_item(void);

/*===========================================================================
FUNCTION tdsl1_get_jds_nv_item

DESCRIPTION
  This function reads the NV items and returns the JDS management related
  configuration.
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern tdsl1_nv_sw_jds_management_params_type* tdsl1_get_jds_nv_item(void);

/*===========================================================================
FUNCTION tdsl1_get_jds_threshold_nv_item

DESCRIPTION
  This function reads the NV items and returns the JDS management threshold
  configuration.
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern tdsl1_nv_jds_management_threshold_params_type* tdsl1_get_jds_threshold_nv_item(void);


extern tdsl1_nv_rxd_params_type* tdsl1_get_rdds_nv_items(void);
extern boolean tdsl1_get_rxd_status(void);
extern boolean tdsl1_get_rxd_rdds_status(void);

extern tdsl1_nv_rxdpm_ard_idleRxD_params_type* tdsl1_get_rxdpm_ard_idle_nv_params(void) ;
extern tdsl1_nv_hsupa_params_type*  tdsl1_get_hsupa_nv_item(void);
extern tdsl1_nv_rdds_ard_params_type* tdsl1_get_rdds_ard_nv_params(void) ;
/*===========================================================================
FUNCTION tdsl1_get_measurement_nv_item

DESCRIPTION
  This function reads the NV items and returns the  measurement parameter
  configuration.
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
tdsl1_nv_meas_params_type* tdsl1_get_measurement_nv_item(void);

/*===========================================================================
FUNCTION tdsl1_get_searcher_nv_item

DESCRIPTION
  This function reads the NV items and returns the  searcher parameter
  configuration.
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
tdsl1_nv_searcher_params_type* tdsl1_get_searcher_nv_item(void);
/*===========================================================================
FUNCTION tdsl1_get_searcher_ext_nv_item

DESCRIPTION
  This function reads the NV items and returns the  searcher parameter extend
  configuration.
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
tdsl1_nv_searcher_params_ext_type* tdsl1_get_searcher_ext_nv_item(void);


/*===========================================================================
FUNCTION tdsl1_get_hsdpa_drop_nv_item

DESCRIPTION
  This function reads the NV items and returns the HSDPA drop parameter
  configuration.
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
tdsl1_nv_hsdpa_drop_params_type* tdsl1_get_hsdpa_drop_nv_item(void);

/*===========================================================================
FUNCTION tdsl1_get_mtpl_control_nv_item

DESCRIPTION
  This function reads the NV items and returns the mtpl control parameter
  configuration.
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
tdsl1_nv_mtpl_control_params_type* tdsl1_get_mtpl_control_nv_item(void);

/*===========================================================================
FUNCTION tdsl1_get_fast_ho_control_type2_nv_item

DESCRIPTION
  This function reads the NV items and returns the TDS search DCH event 1G, 2A, 3A parameter
  configuration.
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
tdsl1_nv_fast_ho_control_struct_type2* tdsl1_get_fast_ho_control_type2_nv_item(void);

/*===========================================================================
FUNCTION tdsl1_get_cell_sel_nv_item

DESCRIPTION
  This function reads the NV items and returns the  cell selection parameter
  configuration.
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
tdsl1_nv_cell_sel_params_type* tdsl1_get_cell_sel_nv_item(void);



/*===========================================================================
FUNCTION tdsl1_get_cell_resel_nv_item

DESCRIPTION
  This function reads the NV items and returns the  cell reselection parameter
  configuration.
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
tdsl1_nv_cell_resel_params_type* tdsl1_get_cell_resel_nv_item(void);


/*===========================================================================
FUNCTION tdsl1_get_HO_nv_item

DESCRIPTION
  This function reads the NV items and returns the handover parameter
  configuration.
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
tdsl1_nv_HO_params_type* tdsl1_get_HO_nv_item(void);

/*===========================================================================
FUNCTION tdsl1_get_fast_HO_nv_item

DESCRIPTION
  This function reads the NV items and returns the fast handover parameter
  configuration.
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
tdsl1_nv_fast_ho_control_struct_type* tdsl1_get_fast_HO_nv_item(void);

/*===========================================================================
FUNCTION tdsl1_get_uppch_tadv_opt_nv_item

DESCRIPTION
  This function reads the NV items and returns the uppch tadv optimization item
  configuration.
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
tdsl1_nv_uppch_tadv_opt_params_type* tdsl1_get_uppch_tadv_opt_nv_item(void);
/*===========================================================================
FUNCTION tdsl1_save_uppch_tadv_opt_nv_item

DESCRIPTION
  This function saves the NV items for uppch tadv optimization item
  configuration. only save sucess rate table into NV, donot change other values
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1_save_uppch_tadv_opt_nv_item(void);

/*===========================================================================
FUNCTION tdsl1_get_l1m_test_support_nv_item

DESCRIPTION
  This function reads the NV items and returns the L1M test support parameter
  configuration.
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
tdsl1m_nv_control_type* tdsl1_get_l1m_test_support_nv_item(void);

/*===========================================================================
FUNCTION tdsl1_get_tx_pwr_boost_params_nv_item

DESCRIPTION
  This function reads the NV items and returns the tdsl1_tx_pwr_boost_params item
  configuration.
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
tdsl1_nv_tx_pwr_boost_params_type* tdsl1_get_tx_pwr_boost_params_nv_item(void);

/*===========================================================================
FUNCTION tdsl1_get_special_setting_for_test_nv_item

DESCRIPTION
  This function reads the NV items and returns special_setting_for_test
  configuration.
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern tdsl1_nv_special_setting_for_test_type* tdsl1_get_special_setting_for_test_nv_item(void);

/*===========================================================================
FUNCTION tdsl1_get_fast_HO_nv_item

DESCRIPTION
  This function reads the NV items and returns the fast handover parameter
  configuration.
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
tdsl1_nv_fast_ho_control_struct_type* tdsl1_get_fast_HO_nv_item(void);

extern tdsl1_nv_fast_ho_control_struct_type* tdsl1_get_fast_HO_nv_item(void);
/*===========================================================================
FUNCTION tdsl1_get_clock_control_nv_item

DESCRIPTION
  This function reads the NV items and returns clock control
  configuration.
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
tdsl1_nv_clock_control_type* tdsl1_get_clock_control_nv_item(void);

/*===========================================================================
FUNCTION tdsl1_t2l_cm_meas_periodity_nv_item

DESCRIPTION
  This function saves the NV items for t2l cm periodicity item
  configuration. only save sucess rate table into NV, donot change other values
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

tdsl1_nv_t2l_cm_meas_periodicity_params_type* tdsl1_get_t2l_cm_meas_periodicity_nv_item(void);


/*===========================================================================
FUNCTION tdsl1_t2l_cm_meas_periodity_sglte_nv_item

DESCRIPTION
  This function saves the NV items for t2l cm periodicity item
  configuration in the sglte mode. only save sucess rate table into NV, donot change other values
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

tdsl1_nv_t2l_cm_meas_periodicity_sglte_params_type* tdsl1_get_t2l_cm_meas_periodicity_sglte_nv_item(void);

/*===========================================================================
FUNCTION tdsl1_get_cm_meas_gap_policy_nv_item

DESCRIPTION
  This function saves the NV items for connected mode measurement gap policy item
  configuration. only save sucess rate table into NV, donot change other values
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

tdsl1_nv_cm_meas_gap_policy_params_type*  tdsl1_get_cm_meas_gap_policy_nv_item(void);
/*===========================================================================
FUNCTION tdsl1_get_timeline_profile_nv_item

DESCRIPTION
  This function reads the NV items and returns timeline profiling mode
  configuration.
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
tdsl1_nv_timeline_profile_params_type* tdsl1_get_timeline_profile_nv_item(void);

/*===========================================================================
FUNCTION tdsl1_get_e3a_threshold_opt_nv_item

DESCRIPTION
  This function reads the NV items and returns e3a_threshold_opt
  configuration.
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
tdsl1_nv_e3a_threshold_opt_params_type *tdsl1_get_e3a_threshold_opt_nv_item(void);
/*===========================================================================
FUNCTION tdsl1_get_t2l_threshold_opt_nv_item

DESCRIPTION
  This function reads the NV items and returns t2l_threshold_opt
  configuration.
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
tdsl1_nv_t2l_threshold_opt_params_type *tdsl1_get_t2l_threshold_opt_nv_item(void);

/*===========================================================================
FUNCTION tdsl1_get_ifreq_qtuner_update_nv_item

DESCRIPTION
  This function reads the NV items and returns IFREQ Qtuner dynamic update configuration.
 
DEPENDENCIES

RETURN VALUE
  tdsl1_ifreq_qtuner_update_params_type* tdsl1_ifreq_qtuner_update_params

SIDE EFFECTS
  None
===========================================================================*/
tdsl1_ifreq_qtuner_update_params_type* tdsl1_get_ifreq_qtuner_update_nv_item(void);

/*===========================================================================
FUNCTION tdsl1_get_cqi_params_nv_item

DESCRIPTION
  This function reads the NV items and returns the power test related configuration.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern tdsl1_nv_pwr_test_params_type* tdsl1_get_pwr_test_params_nv_item(void);

/*===========================================================================
FUNCTION tdsl1_get_drx_nv_item

DESCRIPTION
  This function reads the NV items and returns drx related configuration.
 
DEPENDENCIES

RETURN VALUE
  tdsl1_nv_drx_params_type* 

SIDE EFFECTS
  None
===========================================================================*/
tdsl1_nv_drx_params_type* tdsl1_get_drx_nv_item(void);

/*===========================================================================
FUNCTION tdsl1_get_ul_test_control_nv_item

DESCRIPTION
  This function reads the NV items and returns ul test contorl nv configuration.
 
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
tdsl1_nv_ul_test_control_params_type* tdsl1_get_ul_test_control_nv_item(void);


#endif /* TDS_L1_NV_H */
