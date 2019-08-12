
/*!
  @file
  lte_ml1_log_ext.h

  @brief
  This file describes the external log interfaces to LTE ML1
*/


/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_ml1_log_ext.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/11/15   dsp     CR 901376: Addded AFC Logging support, log packet 0xB16D
10/25/13   ap      Adding support for DLM log packets
10/16/13   awj     Initial version
===========================================================================*/

#ifndef LTE_ML1_LOG_EXT_H
#define LTE_ML1_LOG_EXT_H
#include "lte_as.h"
#include "lte_cphy_msg.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! Maximum number of neighbor cells per request */
#define LTE_ML1_LOG_MAX_NCELLS_PER_REQ  16

/*! Maximum number of HRPD band config structures */
#define LTE_ML1_SM_LOG_HRPD_BAND_CONFIG_MAX  32

/* Max number of CCs currently supported (including primary)*/
#define LTE_ML1_GM_LOG_MAX_CC                                             3

/*! Maximum number of HRPD pilots */
#define LTE_ML1_SM_LOG_HRPD_PILOT_INFO_MAX   32

#define LTE_ML1_GM_LOG_MAX_PDCCH_RESULTS_PER_TTI                         (8)

#define LTE_ML1_GM_LOG_DCI_INFO_MAX_CNT                                 (20)

#define LTE_ML1_LOG_GM_PUCCH_TX_N_1_PUCCH_NUM                            (4)
/*! Maximum number of HRPD pilots */
#define LTE_ML1_SM_LOG_HRPD_PILOT_INFO_MAX   32

#define LTE_ML1_LOG_MAX_MBSFN_ALLOCATIONS                       (8)
#define LTE_ML1_DLM_LOG_PDSCH_STAT_MAX_CNT                      (25)

#define LTE_ML1_DLM_LOG_BLER_STATS_CARRIER_MAX_CNT  LTE_LL1_CARRIER_COUNT

#define LTE_ML1_DLM_LOG_MAX_CARRIERS LTE_LL1_CARRIER_COUNT
#define LTE_ML1_DLM_LOG_LINK_CAP_MAX_CNT                        (10)
#define LTE_ML1_DLM_LOG_RES_AVAIL_FRAC_MAX_CNT                  (20)

#define LTE_ML1_BOLT_LOG_PACKET_BASE_VERSION                    (16)

/*! @brief Main log structure for all serving cell information */
typedef PACK(struct)
{
  /*! Version */
  uint16  version:8;

    /*! Downlink bandwidth */
  uint16  dl_bandwidth:8;

  /*! System frame number (at ref time) */
  uint16   sfn;

  ///< serving cell earfcn
  uint32  earfcn; 

  ///< serving  cell_ID
  uint16  cell_ID:9;

  ///< decode result: 1 = pass, 0= fail;
  uint16  decode_result:1;

  /*! The phich duration configures lower limit on the size of the control
      region signaled by the PCFICH. */
  uint16 phich_duration:3;

   /*! Parameter for determining the number of PHICH groups */
  uint16 phich_resource:3;

  uint16  reserved0;
  
   ///< serving cell PSS
  uint32  pss_corr_result;

  ///< serving cell PSS
  uint32  sss_corr_result;

  /*------------------------*/
  ///< 10 ms boundary;
  uint32 ref_time[2];

   ///< PBCH payload
  uint32 mib_payload;

   ///< updated frequency offset estimate (in Hz);
  ///< invalid if freq_offset_invalid
  uint16  freq_offset;

  ///< number of antennas for cell: 1, 2, or 4;
  uint16  num_antennas:2;


  uint16  reserved1:14;

} lte_ml1_sm_log_serv_cell_info_pkt_s;


/*! @brief Serving Cell Measurement & Evaluation
*/
typedef struct
{
  /*! Version info */
  uint32  version:8;
  /*! standards version */
  uint32  standards_version:8;

  uint32  reserved0:16;

  /*! E-ARFCN */
  uint32  earfcn;

  /*! Physical cell ID, 0 - 504 */
  uint16  phy_cell_id:9;
  /*! Serving cell priority */
  uint16 serv_layer_prio:4;
  /*! reserved */
  uint32  reserved1:19;

  /*! measured RSRP, -140 ~ -40 dBm*/
  uint32  measured_rsrp:12;
  /*! True measured RSRP, -180 ~ -30 dBm*/
  uint32  true_measured_rsrp:12;
  /*! reserved */
  uint32  reserved2:8;

  /*! averge RSRP, -140 ~ -40 dBm*/
  uint32  avg_rsrp:12;
  /*! True averge RSRP, -180 ~ -30 dBm*/
  uint32  true_avg_rsrp:12;
  /*! reserved */
  uint32  reserved3:8;

  /*! measured RSRQ, -30 ~ 0*/
  uint32  measured_rsrq:10;
  /*! True measured RSRQ, -30 ~ +10 */
  uint32  true_measured_rsrq:10;
  /*! averge RSRQ, -30 ~ 0 */
  uint32  avg_rsrq:10;
  /*! reserved */
  uint32  reserved4:2;

  /*! True averge RSRQ, -30 ~ +10 */
  uint32  true_avg_rsrq:10;
  /*! measured RSSI, -110 ~ -10 */
  uint32  measured_rssi:11;
  /*! reserved */
  uint32  reserved5:11;

  /*! Qrxlevmin */
  uint32  q_rxlevmin:6;
  /*! P_max, unit of db, -30 to 33, 0x00=-30, 0x1=-29, ...0x3F=33,
      0x40=NP for not present */
  uint32  p_max:7;
  /*! max UE Tx Power */
  uint32  max_ue_tx_pwr:6;
  /*! Srxlev */
  uint32  s_rxlev:7;
  /*! num drxs S < 0 */
  uint32  num_drx_S_fail:6;

  /*! S_intra_search*/
  uint32  s_intra_search:6;
  /*! S_non_intra_search */
  uint32  s_non_intr_search:6;
  /*! meas rules updated or not */
  uint32  meas_rules_updated:1;
  /*! Measurement rules */
  uint32  meas_rules:4;
  /*! reserved */
  uint32  reserved6:15;

#ifdef FEATURE_LTE_REL9
  /*! Range -34 to -3 */
  uint32  q_qualmin:7;
  /*! Range -35 to +34 */
  uint32  s_qual:7;
  /*! Range 0 to +31 */
  uint32  s_intra_search_q:6;
  /*! Range 0 to +31 */
  uint32  s_non_intra_search_q:6;
  uint32  reserved7:6;
#endif
} lte_ml1_sm_log_idle_serv_meas_eval_s;


/*! @brief Subpacket struct for serv cell measurement result*/
typedef struct
{
  /*! E-ARFCN, 0 ~ 39649 */
  uint32 earfcn; 
  /*! Physical cell ID, 0 ~ 504*/
  uint16 phy_cell_id:9;
  /*! Serving cell index: 0..7 */
  uint16 serv_cell_index:3;
  uint16 is_serv_cell:1;
  uint16 reserved_0:3;
  uint16 rserved_00;

  /*! System frame time when measurements are made, 0 ~ 1023*/
  uint32 current_sfn:10;
  /*! System subframe time when measurements are made, 0 ~ 9*/
  uint32 current_subfn:4;
  uint32 reserved_1:18;

  /*!Measurement done on restricted subframe , 0 or 1 */
  uint32 is_subfm_restricted:1;
  /*! cell timing for rx antenna 0, 0 ~ 307199*/
  uint32 cell_timing_0:19;
  uint32 reserved_2:12;

  /*! cell timing for rx antenna 1, 0 ~ 307199*/
  uint32 cell_timing_1:19;
  /*! SFN for rx antenna 0 corresponding to cell timing, 0 ~ 1023*/
  uint32 cell_timing_sfn_0:10;
  uint32 reserved_3:3;

  /*! SFN for rx antenna 1 corresponding to cell timing, 0 ~ 1023*/
  uint32 cell_timing_sfn_1:10;
  /*! Inst RSRP value for antenna 0, -140 ~ -40 dBm*/
  uint32 inst_rsrp_rx_0:12;
  uint32 reserved_4:10;

  /*! True Inst RSRP value for antenna 0, -180 ~ -30 dBm*/
  uint32 true_inst_rsrp_rx_0:12;
  /*! Inst RSRP value for antenna 1, -140 ~ -40 dBm*/
  uint32 inst_rsrp_rx_1:12;
  uint32 reserved_5:8;

  /*! True Inst RSRP value for antenna 0, -180 ~ -30 dBm*/
  uint32 true_inst_rsrp_rx_1:12;
  /*! Inst RSRP value combined across tx-rx pairs, -140 ~ -40 dBm*/
  uint32 inst_rsrp:12;
  uint32 reserved_6:8;

  /*! True inst RSRP value combined across tx-rx pairs, -180 ~ -30 dBm*/
  uint32 true_inst_rsrp:12;
  /*! Inst RSRQ value for antenna 0, -30 to 0 dBm*/
  uint32 inst_rsrq_rx_0:10;
  /*! True inst RSRQ value for antenna 0, -30 to +10 dBm*/
  uint32 true_inst_rsrq_rx_0:10;

  /*! Inst RSRQ value for antenna 0, -30 to 0 dBm*/
  uint32 inst_rsrq_rx_1:10;
  /*! True inst RSRQ value for antenna 1, -30 to +10 dBm*/
  uint32 true_inst_rsrq_rx_1:10;
  /*! Inst RSRQ value combined across tx-rx pairs, -30 ~ 0 dBm*/
  uint32 inst_rsrq:10;
  uint32 reserved_7:2;

  /*! True inst RSRQ value combined across tx-rx pairs, -30 ~ +10 dBm*/
  uint32 true_inst_rsrq:10;
  /*! Inst RSSI value for antenna 0, -110 ~ -10 dBm*/
  uint32 inst_rssi_rx_0:11;
  /*! Inst RSSI value for antenna 1, -110 ~ -10 dBm*/
  uint32 inst_rssi_rx_1:11;

  /*! Inst RSRQ value combined across tx-rx pairs, -110 ~ -10 dBm*/
  uint32 inst_rssi:11;
  uint32 reserved_8:21;

  /*! DVGA gain in log domain for antenna 0 and 1*/
  int16  dvga_rx_log_0;
  int16  dvga_rx_log_1;
  /*! LNA gain in log domain for antenna 0 and 1*/
  int16  lna_gain_rx_log_0;
  int16  lna_gain_rx_log_1;

  /*! Wideband CQI */
  uint16 cqi_web;
  /*! Residual frequency error */
  int16  res_freq_error;

  /*! FTL-SNR in linear scale for antenna 0 and 1*/
  uint32 ftl_snr_linear_rx_0;
  uint32 ftl_snr_linear_rx_1;
  /*! FTL-SNR value for antenna 0, -20 to 30 dBm*/
  uint32 ftl_snr_rx_0:9;
  /*! FTL-SNR value for antenna 1, -20 to 30 dBm*/
  uint32 ftl_snr_rx_1:9;
  uint32 reserved_9:14;

  /*! RS-SNR in linear scale for antenna 0 and 1*/
  uint32 rs_snr_linear_rx_0;
  uint32 rs_snr_linear_rx_1;
  /*! RS-SNR value for antenna 0, -20 to 30 dBm*/
  uint32 rs_snr_rx_0:9;
  /*! RS-SNR value for antenna 1, -20 to 30 dBm*/
  uint32 rs_snr_rx_1:9;
  uint32 reserved_10:14;

  /*! Projected SIR in Q_format 4 */
  int32 projected_sir;
  /*! Post IC RSRQ */
  uint32 post_ic_rsrq;

} lte_ml1_sm_log_serv_cell_meas_response_subpkt_s;


/*! @brief Idle cell measurement info struct */
typedef struct
{
  /*! physical cell id, 0 ~ 503*/
  uint32 cell_id:9;
  /*! Cumulative frequency offset, 0 ~ 0xffff*/
  uint32 ftl_cumu_freq_offset:16;
  uint32 reserved_0:7;

  /*! Inst RSRP value for rx antenna 0, -140 ~ -40 dBm*/
  uint32 inst_rsrp_rx_0:12;
  /*! True Inst RSRP value for rx antenna 0, -180 ~ -30 dBm*/
  uint32 true_inst_rsrp_rx_0:12;
  uint32 reserved_1:8;

  /*! Inst RSRP value for rx antenna 1, -140 ~ -40 dBm*/
  uint32 inst_rsrp_rx_1:12;
  /*! True Inst RSRP value for rx antenna 1, -180 ~ -30 dBm*/
  uint32 true_inst_rsrp_rx_1:12;
  uint32 reserved_2:8;

  /*! Inst RSRP value combined across tx-rx pairs, -140 ~ -40 dBm*/
  uint32 inst_rsrp:12;
  /*! True Inst RSRP value combined across tx-rx pairs, -180 ~ -30 dBm*/
  uint32 true_inst_rsrp:12;
  uint32 reserved_3:8;

  /*! Inst RSRQ value for rx antenna 0, -30 ~ 0 dBm*/
  uint32 inst_rsrq_rx_0:10;
  /*! True Inst RSRQ value for rx antenna 0, -30 ~ +10 dBm*/
  uint32 true_inst_rsrq_rx_0:10;
  /*! Inst RSRQ value for rx antenna 1, -30 ~ 0 dBm*/
  uint32 inst_rsrq_rx_1:10;
  uint32 reserved_4:2;

  /*! True Inst RSRQ value for rx antenna 1, -30 ~ +10 dBm*/
  uint32 true_inst_rsrq_rx_1:10;
  /*! Inst RSRQ value combined across tx-rx pairs, -30 ~ 0 dBm*/
  uint32 inst_rsrq:10;
  /*! True Inst RSRQ value combined across tx-rx pairs, -30 ~ 0 dBm*/
  uint32 true_inst_rsrq:10;
  uint32 reserved_5:2;

  /*! Inst RSRQ value for rx antenna 0, -110 ~ -10 dBm*/
  uint32 inst_rssi_rx_0:11;
  uint32 reserved_6:21;

  /*! Inst RSSI value for rx antenna 0, -110 ~ -10 dBm*/
  uint32 inst_rssi_rx_1:11;
  /*! Inst RSSI value combined acros tx-rx pairs, -110 ~ -10 dBm*/
  uint32 inst_rssi:11;
  uint32 reserved_7:10;

  /*! DVGA gain in log domain for antenna 0 and 1*/
  int32  dvga_rx_log_0:16;
  int32  dvga_rx_log_1:16;
  /*! LNA gain in log domain for antenna 0 and 1*/
  int32  lna_gain_rx_log_0:16;
  int32  lna_gain_rx_log_1:16;

  /*! Frame bounrary ref time 0, 0 ~ 0xffff_ffff*/
  uint32 frame_bdry_ref_time_0;
  /*! Frame bounrary ref time 1, 0 ~ 0xffff_ffff*/
  uint32 frame_bdry_ref_time_1;

  /*! Total timing adj to CIR 0, 0 ~ 0xffff*/
  uint32 total_timing_adj_cir_0:16;
  /*! Total timing adj to CIR 1, 0 ~ 0xffff*/
  uint32 total_timing_adj_cir_1:16;

} lte_ml1_sm_log_idle_ncell_meas_response_info_s;


/*! @brief Subpacket struct for idle cell measurement response*/
typedef struct
{
  /*! Cell frequency measured*/
  uint32 earfcn:32;

  /*! Number of cell measurements, 0 ~ 16 */
  uint32 num_cells:5;
  /*! Current count of SWRP in MP, 1 ~ 4*/
  uint32 cnt_swrp:2;
  uint32 duplexing_mode:2;
  /*! Serving cell index: 0..7 */
  uint32 serv_cell_index:3;
  uint32 reserved_0:20;

  /*! Cell measuerment info list */
  lte_ml1_sm_log_idle_ncell_meas_response_info_s
        meas_resp_info_list[LTE_ML1_LOG_MAX_NCELLS_PER_REQ];

} lte_ml1_sm_log_ncell_meas_response_subpkt_s;


/*! @brief Connected neighbor cell meas paramters*/
typedef struct
{
  /*! SW reporting period in ms, 40, 80 or 160*/
  uint16 swrp:8;
  /*! Scheduling period in ms, 40, 80 or 160*/
  uint16 sp:8;

  /*! ncell occasion enum*/
  uint16 occasion:2;
  /*! Number of scheduling periods per measurement period, 1, 2 or 4*/
  uint16 num_sp_per_mp:3;
  /*! Duration of measurement gap in ms, cannot be more than 15 */
  uint16 gap_duration:4;
  /*! One bit per scheduling period.
    0: no gap
    1:Measurement gap in the end of scheduling period */
  uint16 gap_mask:1;
  uint16 reserved:6;

} lte_ml1_sm_log_ncell_conn_s;


/*! @brief Idle neighbor cell measurement request paramters*/
typedef struct
{
  /*! Physical cell id, 0 ~ 503 */
  uint16 cell_id:9;

  /*! cell cyclic prefix type
      0x00 = LTE_L1_CP_MODE_NORMAL
      0x01 = LTE_L1_CP_MODE_EXTENDED
      0x02 = LTE_L1_CP_MODE_EXTENDED_MBSFN
      0x03 = LTE_L1_MAX_NUM_CP_MODES*/
  uint16 cp_type:2;

  /*! Number of tx antenna for the eNb, 1 or 2*/
  uint16 eNb_tx_ant:2;
  uint16 ttl_enable:1;
  uint16 ftl_enable:1;
  uint16 reserved_0:1;

  /*! valid if ftl_enable is true*/
  uint16 ftl_cumu_freq_offset;

  /*! valid if ttl_enable is true*/
  uint32 frame_bdry_ref_time_0;
  uint32 frame_bdry_ref_time_1;

  uint16 total_timing_adj_cir_0;
  uint16 total_timing_adj_cir_1;

} lte_ml1_sm_log_ncell_meas_req_param_s;


/*! @brief Subpacket struct for connected ncell meas request*/
typedef struct
{
  /*! Cell earfcn, 0 ~ 65535 */
  uint32 earfcn:32;

  /*! Number of neighbor cells to measure, 1 ~ 16 */
  uint32 num_ncells:4;
  /*! Number of TX antennas, 1 ~ 2 */
  uint32 num_rx_ant:2;
  /*! Serving cell index: 0..7 */
  uint32 serv_cell_index:3;
  uint32 reserved_0:23;

  lte_ml1_sm_log_ncell_conn_s conn_params;

  lte_ml1_sm_log_ncell_meas_req_param_s
          cell_list[LTE_ML1_LOG_MAX_NCELLS_PER_REQ];

} lte_ml1_sm_log_conn_ncell_meas_req_subpkt_s;


/*! @brief DRX Request
*/
typedef struct
{
  /*! Version info */
  uint32  version:8;
  uint32  reserved0:24;

  /*! E-ARFCN Range: 0-262143*/
  uint32   earfcn;  /* 32 bits */

  /*! Physical cell ID, 0 - 504 */
  uint16  phy_cell_id;

  /*! paging cycle index 0x0 = 320 ms
	0x1 = 640 ms
	0x2 = 1280 ms
	0x3 = 2560 ms
	*/
  uint8   paging_cycle_index;

	/*! Nb 0x0 = fourT 0x1 = twoT 0x2 = oneT 0x3 =  halfT 0x4 = quarterT
	0x5 = oneEightT 0x6 = onSixteenthT 0x7 = oneThirtySecondT*/
  uint8   nb;

  /*! UE ID IMSI mod 1024. Range 0 to 1023 */
  uint16 ueid;

	/*! Paging SFN offset Range 0 to 1023 */
  uint16  paging_occ_sfn_offset;

	/*! Paging sub-FN offset Range 0 to 9*/
  uint8  paging_occ_subfn_offset;

  /*! reserved */
  uint32  reserved1:24;

} lte_ml1_common_log_paging_drx_info_s;


typedef struct
{
  /*! Results pertaining to each cell */
  /* PN position In units of chipx8 Range 0...307200 (38400*8) */
  uint32                                          pn_pos;

  /*!Scrambling code for this peak  Range 0..511*/
  uint16                                          psc;

  /*********************************************************************
  ** Units of the three items below (RSSI, RSCP, Ec/Io) are two times **
  ** the actual dBm value, giving a resolution for each of 0.5 dBm.   **
  *********************************************************************/

  /*!The received wide band power, including thermal noise and
  noise generated in the receiver,within the bandwidth defined
  by the receiver pulse shaping filter Ref 3gpp 25.215*/
  int16                                           cpich_rssi;

  /*!Received Signal Code Power, the received power on one code
  measured on the Primary CPICH. Ref 3gpp 25.215*/
  int16                                           cpich_rscp;

  /*!The received energy per chip divided by the power density
  in the band. The CPICH Ec/No is identical to CPICH RSCP/UTRA
  Carrier RSSI. Measurement shall be performed on the Primary
  CPICH. Ref : 3gpp spec 25.215*/
  int16                                           cpich_ecno;

  /*!Filtered RSCP*/
  int16                                           cpich_rscp_filtered;

  /*!Filtered ECNO*/
  int16                                           cpich_ecno_filtered;

}lte_ml1_sm_log_conn_meas_wcdma_cell_results_s;


/*! CDMA band class configuration structure */
typedef struct
{
  /*! Band class (Range 0...31) */
  uint8 band;
  /*! Cell reselection priority (Range 0...7) */
  uint8 priority;
  /*! Threshold, high-prio layer (Range 0...63) */
  uint8 threshX_high;
  /*! Threshold, low-prio layer (Range 0...63) */
  uint8 threshX_low;

} lte_ml1_sm_log_idle_cdma_meas_config_band_config_s;


/*! CDMA neighbor pilot information structure */
typedef struct
{
  /*! Band class (Range 0...31) */
  uint8 band;
  /*! Reserved */
  uint8  reserved1;
  uint16 reserved2;
  /*! Channel (Range 0...2047) */
  uint16 channel;
  /*! Pilot id (Range 0...511) */
  uint16 pilot;

} lte_ml1_sm_log_idle_cdma_meas_config_pilot_info_s;


/*! CDMA reselection parameters structure */
typedef struct
{
  /*! Treselection in seconds (Range 0...7) */
  uint8 Tresel;
  /*! Medium mobility scaling factor
     0x01 = 0.25, 0x2 = 0.50, 0x3 = 0.75, 0x4 = 1.00 */
  uint8 Tresel_medium;
  /*! High mobility scaling factor
     0x01 = 0.25, 0x2 = 0.50, 0x3 = 0.75, 0x4 = 1.00 */
  uint8 Tresel_high;
  /*! Number of band class configurations */
  uint8 num_bands;
  /*! Number of neighbor pilots */
  uint8 num_pilots;
  /*! Reserved */
  uint8  reserved1;
  uint16 reserved2;
  /*! Band class configuration list */
  lte_ml1_sm_log_idle_cdma_meas_config_band_config_s
    band_list[LTE_ML1_SM_LOG_HRPD_BAND_CONFIG_MAX];
  /*! Neighbor pilot list */
  lte_ml1_sm_log_idle_cdma_meas_config_pilot_info_s
    pilot_list[LTE_ML1_SM_LOG_HRPD_PILOT_INFO_MAX];

} lte_ml1_sm_log_idle_cdma_meas_config_resel_params_s;


/*! LTE ML1 CDMA Idle Meas Config Subpacket */
typedef struct
{
  /*! CDMA window size  (Range 0...15) */
  uint8 window_size;
  /*! Reserved */
  uint8  reserved1;
  uint16 reserved2;
  /*! HRPD reselection parameters */
  lte_ml1_sm_log_idle_cdma_meas_config_resel_params_s hrpd;
  /*! 1x reselection parameters */
  lte_ml1_sm_log_idle_cdma_meas_config_resel_params_s onex;

} lte_ml1_sm_log_idle_cdma_meas_config_subpkt_s;

/*! @brief WCDMA Idle Measurement Configuration subpackets*/
typedef struct
{
  uint16  frequency:       16;
  uint16  cell_resel_prio:  8;
  uint16  thresh_high:      8;

  uint16  thresh_low:       8;
  uint16  Qrxlevmin_act:    8;
  uint16  P_max:            8;
  uint16  Q_qual_min:       8;

} lte_ml1_sm_log_idle_wcdma_meas_config_freq_info_s;


typedef struct
{
  uint8   tresel:         8;
  uint8   sf_med:         8;
  uint8   sf_high:        8;
  uint8   num_wcdma_freq: 8;

  lte_ml1_sm_log_idle_wcdma_meas_config_freq_info_s
      frequency [LTE_CPHY_MEAS_MAX_WCDMA_FREQ];
} lte_ml1_sm_log_idle_wcdma_meas_config_subpkt_s;


typedef struct
{
  uint16 freq;
  uint16 padding;
} lte_ml1_sm_log_idle_gsm_meas_config_freq_s;


typedef struct
{
  uint8  resel_prio:    8;
  uint8  thresh_x_high: 8;
  uint8  thresh_x_low:  8;
  uint8  qrxlev_min:    8;

  uint8  P_max:         8;
  uint8  NCC_permitted: 8;
  uint16 num_freqs:    16;

} lte_ml1_sm_log_idle_gsm_meas_config_freq_group_help_s;


typedef struct
{
  lte_ml1_sm_log_idle_gsm_meas_config_freq_group_help_s info;
  lte_ml1_sm_log_idle_gsm_meas_config_freq_s freqs[LTE_CPHY_MEAS_MAX_GERAN_FREQS_IN_GROUP];
} lte_ml1_sm_log_idle_gsm_meas_config_freq_group_s;


typedef struct
{

  uint8 t_resel: 8;
  uint8 sf_med:  8;
  uint8 sf_high: 8;

  uint8 num_freq_groups: 8;
}lte_ml1_sm_log_idle_gsm_meas_config_subpkt_help_s;


typedef struct
{
  lte_ml1_sm_log_idle_gsm_meas_config_subpkt_help_s info;

  lte_ml1_sm_log_idle_gsm_meas_config_freq_group_s freq_groups[LTE_CPHY_MEAS_MAX_GERAN_FREQ_GROUPS];/*16*/
}lte_ml1_sm_log_idle_gsm_meas_config_subpkt_s;


/*! @brief GM DCI Info Log Record definition
*/
typedef struct
{
  /* First Word */
  uint32  pdcch_timing_sfn:10;
  uint32  pdcch_timing_sub_fn:4;
  uint32  ul_grant_present:1;
  uint32  ul_grant_riv_width:4;
  uint32  ul_grant_riv:13;                       //32 bits

  /* Second Word */
  uint32  ul_grant_hopping_flag:1;
  uint32  ul_grant_mcs_index:5;
  uint32  ul_grant_ndi:1;
  uint32  ul_grant_tpc:2;
  uint32  ul_grant_cyclic_shift_dmrs:3;
  uint32  ul_grant_dci_0_k:3;
  uint32  ul_grant_ul_index_dai:2;
  uint32  reserved1:3;
  uint32  ul_grant_cqi_req:2;
  uint32  ul_grant_srs_req:1;
  uint32  ul_grant_rb_start:7;
  uint32  reserved2:2;                          //32 bits
	
  /* Third Word */
  uint32  ul_grant_num_rbs:7;
  uint32  ul_grant_tbs_index:5;
  uint32  ul_grant_mod_type:2;
  uint32  ul_grant_rv_index:2;
  uint32  harq_id:3;
    uint32  pdcch_order_present:1;
  uint32  tpc_dci_present:1;
  uint32  tpc_dci_format_type:1;
  uint32  reserved3:7;
  uint32  dl_grant_num:3;                      //32 bits

  /* Fourth Word */
  struct
  {
    uint16  dl_grant_present:1;
    uint16  dl_grant_cc_id:3;
    uint16  dl_grant_format_type:4;
    uint16  dl_grant_num_ack_nak_bits:2;
    uint16  dl_grant_tpc_cmd:3;
    uint16  dl_grant_dai:2;
    uint16  reserved:1;
  }dl_grant_info[LTE_ML1_GM_LOG_MAX_CC];

  /* Fifth Word */
  uint32  tpc_dci_rnti_type:4;
  uint32  tpc_dci_tpc_cmd:3;
  uint32  dci_info_reserved:9;                  //32 bits
 
}lte_ml1_gm_log_dci_info_record_s;


/*! @brief GM DCI Info Log packet definition
*/
typedef struct
{
  /* Log header */
  log_hdr_type header;

  /* First Word */
  uint32  version:8;
  uint32  reserved0:15;
  uint32  duplex_mode:1;
  uint32  num_records:8;                        //32 bits
  lte_ml1_gm_log_dci_info_record_s
    dci_info_record [LTE_ML1_GM_LOG_DCI_INFO_MAX_CNT];
} lte_ml1_gm_log_dci_info_s;

/*! @brief GM Tx N_1_pucch record
*/
typedef struct
{
  uint16  n_1_pucch_i:12;
  uint16  reserved:4;
} lte_ml1_gm_log_n_1_pucch_record_s;

/*! @brief GM PUSCH Tx Report Record definition */
typedef struct
{
  /* First word */
  uint32 chan_type:1;
  uint32 sfn:10;
  uint32 sub_fn:4;
  uint32 trblk_size:14;
  uint32 csf_present_flag:1;
  uint32 ack_nak_present_flag:2;                   //32 bits

  /* Second word */
	uint32 ca_mode_enabled:1;
	uint32 ack_reporting_mode:3;
	uint32 ack_nak_to_send_mask:9;
	uint32 ack_nak_to_send_mask_scell_1:9;
	uint32 ack_nak_to_send_mask_scell_2:9;
	uint32 dci_0_present:1;

  /* Third word */
	uint32 w_ul_dai:4;
  uint32 n_bundled:4;
  uint32 end_of_bundling:4;
  uint32 ack_nak_len:3;
  uint32 beta_pusch:16;
	uint32 reserved1:1;															//32 bits

  /* Fourth word */
  uint32 total_tx_power:8;
  uint32 cyclic_shift_dmrs:3;
  uint32 rb_start:7;
  uint32 rv:2;
  uint32 mod_type:2;
  uint32 num_rbs:7;
  uint32 harq_id:3;																//32 bits

  /* Fifth word */
  uint32 retx_index:5;
  uint32 hopping_flag:1;
  uint32 reserved2:2;
  uint32 harq_ack_offset:4;
  uint32 cqi_offset:4;
  uint32 ri_offset:4;
  uint32 hopping_payload:2;
  uint32 srs_present_flag:1;
  uint32 srs_ue_or_cell_specific:1;
  uint32 n_dmrs:3;
  uint32 antenna_num:2;
  uint32 tti_bundl_index:3;												  //32 bits
  
  /* Sixth word */  
  uint32 eib_index:1;
  uint32 mcs_index:5;
  uint32 padding:26;                               //32 bits


  /* Seventh word */
  int32 afc_rx_freq_error;///< AFC Rx Freq Error [Hz] 
}lte_ml1_gm_log_pusch_tx_report_record_s;

/*! @brief GM PUCCH Tx Report Record definition */
typedef struct
{
  /* First word */
  uint32 chan_type:1;
  uint32 sfn:10;
  uint32 sub_fn:4;
  uint32 total_tx_power:8;
  uint32 ack_nak_present_flag:2;
	uint32 ca_mode_enabled:1;
	uint32 ack_reporting_mode:3;
  uint32 ack_nak_len:2;
  uint32 reserved0:1;												  	//32 bits

  /* Second word */
  uint32 ack_nak_to_send_mask:9;
	uint32 ack_nak_to_send_mask_scell_1:9;
	uint32 ack_nak_to_send_mask_scell_2:9;
  uint32 reserved1:5;

	/* Third word */
  uint32 beta_pucch:16;
  uint32 reserved2:1;
  uint32 n_1_pucch:12;
  uint32 n_1_pucch_num:3;                       //32 bits

  /* Fourth and Fifth words */
  lte_ml1_gm_log_n_1_pucch_record_s
    n_1_pucch_records[LTE_ML1_LOG_GM_PUCCH_TX_N_1_PUCCH_NUM];   // 64 bits

  /* Sixth word */
  uint32 n_2_pucch:10;
  uint32 n_3_pucch:10;
  uint32 sr_present_flag:1;
  uint32 trni_ack_flag:1;
  uint32 srs_present_flag:1;
  uint32 srs_ue_or_cell_specific:1;
  uint32 csf_present_flag:1;
  uint32 padding:7;                 	         //32 bits

  /* Seventh word */
  int32 afc_rx_freq_error;///< AFC Rx Freq Error [Hz]
}lte_ml1_gm_log_pucch_tx_report_record_s;

/*! @brief GM Tx Report Log packet union */
typedef union
{
  lte_ml1_gm_log_pucch_tx_report_record_s pucch_tx_report;
  lte_ml1_gm_log_pusch_tx_report_record_s pusch_tx_report;
} lte_ml1_gm_log_tx_report_u;

/*! @brief GM PDCCH PHICH Info Log Record definition
*/
typedef struct
{
  uint32  num_pdcch_results:3;
  uint32  pdcch_timing_sfn:10;
  uint32  pdcch_timing_sub_fn:4;
  uint32  phich_present:1;
  uint32  phich_1_present:1;
  uint32  phich_timing_sfn:10;
  uint32  reserved0:3;                   //32 bits
  uint32  phich_timing_sub_fn:4;
  uint32  phich_val:1;
  uint32  phich_1_val:1;
  uint32  reserved1:26;                  //32 bits

  struct 
  {
    uint32 carr_id:3;
    uint32 rnti_type:4;
    uint32 payload_size:7;
    uint32 aggregation_lvl:2;
    uint32 srch_space:1;
    uint32 sps_grant_type:3;
    uint32 new_dl_tx:1;
    uint32 num_dl_trblks:2;
    uint32 reserved2:9;                 //32 bits
  }pdcch_info[LTE_ML1_GM_LOG_MAX_PDCCH_RESULTS_PER_TTI];
}lte_ml1_gm_log_pdcch_phich_info_record_s;

/*! @brief GM Tx Report Log packet struct */
typedef struct
{
  lte_ml1_gm_log_tx_report_u tx_report;
} lte_ml1_gm_log_tx_report_record_s;

/*! @brief ULM Common config log struct
*/
typedef struct
{
  /* Version */
  uint32 version:8;
  uint32 reserved:24;                                  //32 bits

  /* PUSCH Config */
  uint32 pusch_cfg_present:1;
  uint32 pusch_n_sb:2;
  uint32 pusch_hopping_mode:1;
  uint32 pusch_hopping_offset:7;
  uint32 pusch_enable_64_qam:1;
  uint32 pusch_group_hopping_enabled:1;
  uint32 pusch_group_assignment_pusch:5;
  uint32 pusch_seq_hopping_enabled:1;
  uint32 pusch_cyclic_shift:3;

  /* PUCCH Config */
  uint32 pucch_cfg_present:1;
  uint32 pucch_delta_shift:2;
  uint32 pucch_n_rb_cqi:7;                             //32 bits
  uint32 pucch_n_cs_an:3;
  uint32 pucch_n1_pucch_an:11;

  /* SRS Config */
  uint32 srs_cfg_present:1;
  uint32 srs_enable:1;
  uint32 srs_bw_cfg:3;
  uint32 srs_subframe_config:4;
  uint32 srs_sim_ack_nak_srs:1;
  uint32 srs_max_up_pts:1;
  uint32 reserved1:7;                                  //32 bits

} lte_ml1_ulm_log_common_cfg_s;

/*! @brief ULM Dedicated config log struct
*/
typedef struct
{
  /* Version */
  uint32 version:8;
  uint32 std_ver:8;
  uint32 reserved:16;                                   //32 bits

  /* SRS Config */
  uint32 srs_cfg_present:1;
  uint32 srs_enable:1;
  uint32 srs_bw:2;
  uint32 srs_hopping_bw:2;
  uint32 srs_freq_domain_position:5;
  uint32 srs_duration:1;
  uint32 srs_cfg_index:10;
  uint32 srs_tx_comb:1;
  uint32 srs_cyclic_shift:3;

  /* CQI Config */
  uint32 cqi_cfg_present:1;
  uint32 cqi_enable:1;
  uint32 cqi_reporting_aperiodic_present:1;
  uint32 cqi_reporting_mode_aperiodic:3;                  // 32 bits
  uint32 cqi_reporting_periodic_present:1;
  uint32 cqi_format_indicator:1;
  uint32 cqi_nom_pdsch_rs_epre_offset:4;
  uint32 cqi_pucch_resource_index:11;
  uint32 cqi_ri_cfg_enabled:1;
  uint32 cqi_ri_cfg_index:10;
  uint32 cqi_subband_cqi_k:3;
  uint32 cqi_sim_ack_nak_cqi:1;                           //32 bits
  uint32 cqi_pmi_cfg_index:10;
  uint32 cqi_mask:1;

  /* Antenna Config */
  uint32 antenna_cfg_present:1;
  uint32 antenna_transmission_mode:3;
  uint32 antenna_tx_antenna_sel_enable:1;
  uint32 antenna_tx_antenna_sel_ctrl:1;
  uint32 reserved1:15;                                    //32 bits
  uint32 antenna_codebook_subset_rest[2];                 //64 bits

} lte_ml1_ulm_log_dedicated_cfg_s;

/*! @brief DLM common cfg log structure
    size = 4 + 4 + 1 + [1+1] + 1 + [(4*(0 to 8))] = 44bytes (worst case)
	Packet ID: LOG_LTE_ML1_DLM_COMN_CFG_LOG_C
*/
typedef struct
{
  /*!***************** Version *****************/
  /*! Log packet version */
  uint32  version:8;                                      //32bit word1 start

  /*!***************** TDD cfg *****************/
  /*! tdd cfg: cfg valid if present */
  uint32  tdd_comn_cfg_present:1;
  uint32  tdd_cfg_subfn_assign:4;
  uint32  tdd_cfg_special_subfn_patterns:4;

  /*!***************** UL CP cfg *****************/
  /*! UL CP cfg: cfg valid if present */
  uint32  ul_cp_comn_cfg_present:1;
  uint32  ul_cp_cfg_cp:1;
  uint32  reserved0:13;                                   //32bit word1 end

  /*!***************** UL freq cfg *****************/
  /*! UL freq cfg: cfg valid if present */
  uint32  ul_freq_comn_cfg_present:1;                     //32bit word2 start
  uint32  ul_freq_present:1;
  uint32  ul_freq:16;
  uint32  ul_bw:7;
  uint32  spec_emisson_present:1;
  uint32  spec_emission:6;                                //32bit word2 end

  /*!***************** MIB info *****************/
  /*! MIB info: cfg valid if present */                  //32bit word3 start
  uint8 mib_info_present:1;
  uint8 num_enodeb_tx_ant:3;
  uint8 phich_duration:1;
  uint8 phich_resource:2;

  /*!***************** PDSCH common cfg *****************/
  /*! PDSCH common cfg: cfg valid if present */
  uint8 pdsch_comn_cfg_present:1;
  int8  ref_signal_pwr;
  uint8 pb;

  /*!***************** MBSFN cfg *****************/
  /*! MBSFN cfg: cfg valid if present */
  uint8                 mbsfn_comn_cfg_present:1;
  uint8                 num_mbms_allocations:4;
  uint8                 reserved1:3;                     //32bit word3 end

  /*! MBSFN cfg: VARIABLE SIZE ALLOCATION based on num_mbms allocations */
  struct
  {
    uint32  allocation_offset:3;                         //32bit word4 start
    uint32  allocation_period:3;
    uint32  allocation_type:1;
    uint32  reserved2:1;
    uint32  allocation_mask:24;                          //32bit word4 end
  }mbsfn_cfg[LTE_ML1_LOG_MAX_MBSFN_ALLOCATIONS];

} lte_ml1_log_dlm_comn_cfg_s;

/*! @brief DLM dedicated cfg log structure = 4 + 4 + 4 + 4 + 1 + 1 + 2 = 20bytes
    Packet ID: LOG_LTE_ML1_DLM_DED_CFG_LOG_C
*/
typedef struct
{
  /*!***************** Version *****************/
  uint32  version:8;                                        //32bit word1 start

  /*!***************** PUCCH TPC cfg *****************/
  /*! PUCCH TPC cfg: cfg valid if present */
  uint32  pucch_tpc_ded_cfg_present:1;
  uint32  pucch_enabled:1;
  uint32  pucch_tpc_rnti_val:16;
  uint32  pucch_dci_format:1;
  uint32  pucch_tpc_index:5;                                //32bit word1 end

  /*!***************** PUSCH TPC cfg *****************/
  /*! PUSCH TPC cfg: cfg valid if present */
  uint32  pusch_tpc_ded_cfg_present:1;                      //32bit word2 start
  uint32  pusch_enabled:1;
  uint32  pusch_tpc_rnti_val:16;
  uint32  pusch_dci_format:1;
  uint32  pusch_tpc_index:5;
  uint32  reserved0:8;                                      //32bit word2 end

  /*!***************** Antenna dedicated cfg *****************/
  uint32 codebook_subset_rest_lsw;                          //32bit word3
  uint32 codebook_subset_rest_msw;                          //32bit word4

  /*! cfg valid if present */
  uint8   antenna_ded_cfg_present:1;                        //32bit word5 start
  uint8   transmission_mode:4;
  uint8   tx_antenna_sel_enable:1;
  uint8   tx_antenna_sel_ctrl:1;
  uint8   reserved1:1;

  /*!***************** PDSCH dedicated cfg *****************/
  /*! PDSCH dedicated cfg: cfg valid if present */
  uint8 pdsch_ded_cfg_present:1;
  uint8 pa:4;
  uint8 reserved2:2;

  /*!***************** UE related cfg *****************/
  /*! UE related cfg: cfg valid if present */
  uint8 ue_related_ded_cfg_present:1;
  uint16 c_rnti;                                            //32bit word5 end

} lte_ml1_log_dlm_ded_cfg_s;


/*! @brief DLM pdsch stat record log structure  = 2 + 2 + 2 + 3* [6] + 2 = 26 bytes
*/
typedef struct
{
  /*! ********** PDSCH stat per record **************/

  /* OTA Tx time stamp */
  uint16    subframe_num:4;                   ///< 0 ~9     //32bit word1 start
  uint16    frame_num:12;                     ///< radio frame number 0-1023

  uint16 num_rbs:8;                           ///< number or rbs allocated
  uint16 num_layers:8;                        ///< number of layers
                                              //32bit word1 end

  /*! Num of transport blocks present or valid */
  uint16    num_trbs_present:8;
  uint16    serving_cell_index:3;
  uint16    reserved0:5;

  /*! FIXED SIZE ALLOACTION based on "LTE_LL1_MAX_PDSCH_TRANMISSIONS_PER_TTI" */
  struct                                                //32bit word 2 - 5.5  start
  {
    uint8     harq_id:4;                       ///< (up to 8 for FDD, up to 15 for TDD)
    uint8     rv:2;                            ///< (0, 1, 2, 3)
    uint8     ndi:1;                           ///< (0 or 1)
    uint8     crc_result:1;

    uint8     rnti_type:4;
    uint8     tb_index:1;                      ///< (0 or  1 (MIMO))
    uint8     pdsch_result_discard_retx:1;
    uint8     did_recomb:1;
    uint8     reserved1:1;

    uint16    trblk_size:14;                 /// Size in bytes
    uint16    reserved2:2;                                  //32 bit word end

    uint16    mcs:8;                          ///< ( MCS index 0 - 31)
    uint16    num_rbs:8;                      ///< number of rbs allocated to this tb
  }tb[2];

  uint16      pmch_id:8;     ///< PMCH id: only applicable to pmch decodes
  uint16      area_id:8;     ///< Area id used: only applicable to pmch decodes  //32bit word 2 - 6  end
} lte_ml1_log_dlm_pdsch_stat_record_s;

/*! @brief DLM pdsch stat log records structure = 4 + (1 to 25)*26 = 654 bytes (worst case)
    Packet ID: LOG_LTE_ML1_DLM_PDSCH_STAT_IND_LOG_C
*/
typedef struct
{
  /*!***************** Version *****************/
  uint32     version:8;                                     //32bit word1 start

  /*!***************** Number of PDSCH records available *****************/
  uint32     num_records:8;
  uint32     reserved0:16;                                  //32bit word1 end

  /*!***************** PDSCH records *****************/
  /*! VARIABLE SIZE ALLOACTION based on num of records field */
  lte_ml1_log_dlm_pdsch_stat_record_s    record[LTE_ML1_DLM_LOG_PDSCH_STAT_MAX_CNT];
} lte_ml1_log_dlm_pdsch_stat_records_s;

/*! @brief DLM BLER stats log records structure = 4 + (1 to 2)*12 = 28 bytes (worst case) 
    Packet ID: LOG_LTE_ML1_DLM_BLER_STAT_LOG_C
*/
typedef struct
{
  /*!***************** Version *****************/
  uint32     version:8;                                     //32bit word1 start

  /*!***************** Number of carriers available *****************/
  uint32     num_carriers:8;
  uint32     reserved0:16;                                  //32bit word1 end

  /*!***************** BLER stats *****************/
  /*! VARIABLE SIZE ALLOACTION based on num of carriers field */
  struct
  {
    uint32     total_trblk_rcvd;
    uint32     total_trblk_crc_pass;
    uint32     dl_throughput;
  }carrier[LTE_ML1_DLM_LOG_BLER_STATS_CARRIER_MAX_CNT];
} lte_ml1_log_dlm_bler_stats_s;

typedef enum
{
  ZERO = 0,
  LOW  = 1,
  HIGH = 2,
  INVALID = 0xFF
} confidence_type_e;

/*! @brief DLM throughput estimation Link capacity record log structure  = 
    2 + 4 + 2 + (1 to 3)*4 = 20 bytes
*/
typedef struct
{
  uint16     frame_num:12;                     ///< radio frame number 0-1023
  uint16     subframe_num:4;                   ///< Subframe number 0 ~9

  /*! @brief Instantaneous link capacity in units of Mbps (x16), 
      range >= 0, 0x0000 = 0 Mbps, 0x0001 = 0.0625 Mbps, ...
  */
  uint16     link_cap;
  
  uint32     t_accumulate_expired:8;           ///< Flag indicates if T_accumulate expired or not
  confidence_type_e     confidence:8;          ///< Confidence of link capacity
  uint32     num_carriers:8;                   ///< Number of carriers
  uint32     reserved0:8;                      ///< Reserved

  /*! @brief For carrier[i] where 0 <= i < num_carriers. Size each = 4 bytes */
  struct
  {
    uint16   cqi;                             ///< Wideband CQI
    uint16   efficiency;                      ///< Efficiency from CQI in Mbps
  }carrier[LTE_ML1_DLM_LOG_MAX_CARRIERS];

} lte_ml1_log_dlm_link_cap_record_s;

/*! @brief DLM Throughput estimation link capacity stats log records structure = 
    4 + 20*10 = 204 bytes (worst case) 
    Packet ID: LOG_LTE_ML1_DLM_LINK_CAP_STAT_LOG_C
*/
typedef struct
{
  uint32     version:8;                       ///< Log packet version 0-255
  uint32     num_records:8;                   ///< Number of records max 10
  uint32     reserved0:16;                    ///< Reserved

  /*! @brief For record[i] where 0 <= i < num_records */
  lte_ml1_log_dlm_link_cap_record_s    record[LTE_ML1_DLM_LOG_LINK_CAP_MAX_CNT];
} lte_ml1_log_dlm_link_cap_records_s;

/*! @brief DLM throughput estimation resource availability fraction record log structure  = 
    2 + 2 + 4 + 4 + 2 = 14 bytes
*/
typedef struct
{
  uint16     frame_num:12;                     ///< radio frame number 0-1023
  uint16     subframe_num:4;                   ///< Subframe number 0 ~9

  uint16     t_accumulate_expired:8;           ///< Flag indicates if T_accumulate expired or not
  uint16     dl_frac:8;                        ///< DL fraction 0 <= Range <= 1 (x64), 0x00 = 0, 0x01 = 0.015625, ..., 0x40 = 1  

  /*! @brief Instantaneous resource availability fraction , 0 <= range <= 1 (x64) 
      0x00 = 0, 0x01 = 0.015625, ..., 0x40 = 1 
  */
  uint32 res_avail_frac:8;
  /*! @brief RSRP value for rx antenna 0 in units of dB (x16), 
      range -140 to -40 dB, 0x00 = -140 dB, 0x1 = -139.9375 dB, ...
  */
  uint32 rsrp_rx_0:12;
  /*! @brief RSRP value for rx antenna 1 in units of dB (x16), 
      range -140 to -40 dB, 0x00 = -140 dB, 0x1 = -139.9375 dB, ...
  */
  uint32 rsrp_rx_1:12;

  /*! @brief RSSI value for rx antenna 0 in units of dB (x16), 
      range -110 to -10 dB, 0x00 = -110 dB, 0x1 = 109.9375 dB, ... , 0x640 = -10 dB
  */
  uint32 rssi_rx_0:12;
  /*! @brief RSSI value for rx antenna 0 in units of dB (x16), 
      range -110 to -10 dB, 0x00 = -110 dB, 0x1 = 109.9375 dB, ... , 0x640 = -10 dB
  */
  uint32 rssi_rx_1:12;
  uint32 reserved1:8;                          ///< Reserved

} lte_ml1_log_dlm_res_avail_frac_record_s;

/*! @brief DLM Throughput estimation resource availability fraction stats log records structure = 
    4 + 14*20 = 284 bytes (worst case) 
    Packet ID: LOG_LTE_ML1_DLM_RES_AVAIL_FRAC_STAT_LOG_C
*/
typedef struct
{
  uint32     version:8;                       ///< Log packet version 0-255
  uint32     num_records:8;                   ///< Number of records, max 20
  uint32     reserved0:16;                    ///< Reserved

  /*! @brief For record[i] where 0 <= i < num_records */
  lte_ml1_log_dlm_res_avail_frac_record_s    record[LTE_ML1_DLM_LOG_RES_AVAIL_FRAC_MAX_CNT];
} lte_ml1_log_dlm_res_avail_frac_records_s;

#endif /* LTE_ML1_LOG_EXT_H */
