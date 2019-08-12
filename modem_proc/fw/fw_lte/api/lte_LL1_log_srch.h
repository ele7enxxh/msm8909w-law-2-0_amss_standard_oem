/*!
  @file
  lte_LL1_log_srch.h

  @brief
  srch log packet data structures

  @detail
  !!! Please do not modify !!!
  This file was auto-generated on Thu Sep 29 09:26:01 2011.
  Command line used: perl w2h.pl log_srch srch 2 1 1 2 5 2
   Note: The params, [2 1 1 2 5 2], correspond to version numbers for
         each log message in the listed order in the ICD

*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/lte_LL1_log_srch.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
#ifndef LTE_LL1_LOG_SRCH_H
#define LTE_LL1_LOG_SRCH_H



/*==========================================================================*/
/*! @brief
 * 1 LTE LL1 PSS Results (Log code ID: 0xB113)
 *
 *  This log packet gives important parameters related to PSS results. The log 
 *  record is generated every 5ms. 
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_SRCH_PSS_RESULTS_VERSION 22
#define LTE_LL1_LOG_SRCH_NUMBER_OF_PSS_RECORDS 20

/*! @brief pss_results number_of_pss_records struct
 */
typedef struct
{
  uint32 pss_peak_value                   : 16;  ///< In linear scale, range from 0 to 0xffff. To be
                                                         ///< displayed as 10log10(x/128) in units of dB,
                                                         ///< where x=PSS Peak Value.
  	                                                     ///< in Dime, this is only available for initacq
  uint32 peak_position                    : 16;  ///< Sample position with 5ms resolution
                                                         ///< (Range: 0 to (9600-1))
  uint32 reserved0                        : 30; 
  uint32 pss_indices                      : 2;  ///< 0-2
}lte_LL1_log_srch_pss_results_pss_records_s;


/*! @brief pss_results main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Version number (Range 0 to 255)
                                                         ///< Version = 2 for the log packet structure
                                                         ///< described below
  uint32 number_of_half_frames            : 3;  ///< In units of 5ms (Range 0 to 7). To be Displayed as 
                                                         ///< x+1, where x= Number of half frames
  uint32 reserved0                        : 5; 
  uint32 sub_frame_number                 : 4;  ///< Sub-frame number (Range 0..9)
  uint32 system_frame_number              : 10;  ///< System frame number (range 0..1023)
  uint32 reserved1                        : 2; 
  uint32 k_value                          : 16;  ///< k_d = k_value/256. A multiplicative factor in
                                                         ///< units of micro Volts/LSB
                                                         ///< Show dBVref per RX Antenna (difference (dB)
                                                         ///< between recommended reference energy at the ADC
                                                         ///< input and the narrowband (0.96MHz) energy at
                                                         ///< ADC input), as calculated below:
                                                         ///< P_ADC_in = (Energy_est/2)*(k_d/10^6)^2,  [Power
                                                         ///< at ADC input in Volts^2 (per I or Q assuming
                                                         ///< equal powers)]
                                                         ///< dBVref = 20*log10(Vref_d)-10*log10(P_ADC_in) [I
                                                         ///< or Q Mean Squared Signal level at ADC input
                                                         ///< relative to Vref
  uint32 vref                             : 9;  ///< Double precision value Vref_d = Vref/256.
                                                         ///< Recommended reference volts at ADC input
  uint32 reserved2                        : 2; 
  uint32 number_of_pss_records            : 5;  ///< Range: 0-20
  lte_LL1_log_srch_pss_results_pss_records_s lte_LL1_log_srch_pss_results_pss_records[LTE_LL1_LOG_SRCH_NUMBER_OF_PSS_RECORDS];
}lte_LL1_log_srch_pss_results_ind_struct;



/*==========================================================================*/
/*! @brief
 * 2 LTE LL1 SSS Results (Log code ID: 0xB115)
 *
 *  This log packet gives important parameters related to SSS results. The log 
 *  record is generated every 5ms. SSS results are based on the most recent 
 *  PSS/SSS ocurrences in the search sample server (not necessarily 
 *  corresponding to the PSS log results) 
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_SRCH_SSS_RESULTS_VERSION 22
#define LTE_LL1_LOG_SRCH_NUMBER_OF_BARRED_CELLS 16
#define LTE_LL1_LOG_SRCH_NUMBER_OF_DETECTED_CELLS 8

/*! @brief sss_results number_of_barred_cells struct
 */
typedef struct
{
  uint16 barred_cell_id                   : 9;  ///< Range: 0..503
  uint16 reserved0                        : 7; 
}lte_LL1_log_srch_sss_results_barred_cells_s;


/*! @brief sss_results number_of_detected_cells struct
 */
typedef struct
{
  uint32 sss_peak_value                   : 16;  ///< Q14 In linear scale, range from 0 to 0xffff. To be
                                                         ///< displayed as 10*log10(x/(1-x)) in units of dB,
                                                         ///< where x=(SSS Peak Value)*(2^-14).
  uint32 cell_id                          : 9;  ///< Range: 0..503
  uint32 cp                               : 1;  ///< Cyclic prefix used
                                                         ///< 0: normal CP
                                                         ///< 1: extended CP
  uint32 half_frame_hypothesis            : 1;  ///< 0: Preamble
                                                ///< 1: Midamble
  uint32 reserved0                        : 5; 
  uint32 frequency_offset                 : 16;  ///< In units of Hz with 1Hz resolution.
                                                         ///< (Range: -15000Hz to +15000Hz)
  uint32 erfcn                            : 16; ///< center frequency
  uint32 rx0_lna_offset;                  ///< Q24 in linear scale, rx0 LNA offset
  uint32 rx1_lna_offset;                  ///< Q24 in linear scale, rx1 LNA offset
  uint32 frame_bndry_lo;                  ///< 64 bit frame boundary RTC counter
  uint32 frame_bndry_hi;                  ///< 64 bit frame boundary RTC counter
  uint32 min_frame_bndry;                 ///< 32 bit min frame boundary
  uint32 max_frame_bndry;                 ///< 32 bit max frame boundary
}lte_LL1_log_srch_sss_results_detected_cells_s;


/*! @brief sss_results main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Version number (Range 0 to 255)
                                                           ///< Version = 1 for the log packet structure
                                                         ///< described below
  uint32 reserved0                        : 19; 
  uint32 number_of_barred_cells           : 5;  ///< Number of barred cells (Range: 0-16)
  lte_LL1_log_srch_sss_results_barred_cells_s lte_LL1_log_srch_sss_results_barred_cells[LTE_LL1_LOG_SRCH_NUMBER_OF_BARRED_CELLS];
  uint32 reserved1                        : 27; 
  uint32 number_of_detected_cells         : 5;  ///< Number of detected cells (Range: 0-16)
  lte_LL1_log_srch_sss_results_detected_cells_s lte_LL1_log_srch_sss_results_detected_cells[LTE_LL1_LOG_SRCH_NUMBER_OF_DETECTED_CELLS];
}lte_LL1_log_srch_sss_results_ind_struct;



/*==========================================================================*/
/*! @brief
 * 4 LTE LL1 Serving Cell Measurement Results (Log code ID: 0xB116)
 *
 *  This log packet gives important parameters related to measurements. The log 
 *  record is generated every serving cell measurement occasion. 
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_SRCH_SERVING_CELL_MEASUREMENT_RESULTS_VERSION 21

/*! @brief serving_cell_measurement_results main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Log packet version. Range: 0...255.
                                                           ///< Version = 0 for the log packet structure described below
  uint32 starting_sfn                     : 16;  ///< Sub/System Frame Number, defined as LSB 3:0 = Sub-frame number (Range
                                                         ///< 0..9)
                                                         ///< LSB 15:4 = System frame number
  uint32 is_idle_mode                     : 1;  ///< 0: Connected mode
                                                         ///< 1: Idle mode
  uint32 measurement_bw                   : 3;  ///< Encoded as:
                                                         ///< 0x0: 1.44 MHz
                                                         ///< 0x1: 3 MHz
                                                         ///< 0x2: 5 MHz
                                                         ///< 0x3: 10 MHz
                                                         ///< 0x4: 15 MHz
                                                         ///< 0x5: 20 MHz
                                                         ///< 0x6-0x7: Reserved
  uint32 carrier_type                     : 4; ///< 0-PCC, 1-SCC 
  uint32 cell_id                          : 9;  ///< Range: 0-504
  uint32 digital_rotator                  : 19;  ///< In units of Hz with 1Hz resolution. Range: -15kHz to +15kHz
                                                         ///< 0x0000: -15kHz ... 0x7531: +15kHz
  uint32 reserved1                        : 4; 
  uint32 timing_offset_rx[2]                          ;  ///< Timing offset per Rx antenna with a resolution of 10ms.
                                                         ///< Range 0-307200
   uint8 threshold_offset_rx[2]                       ;  ///< Weighting factor for threshold Rx0 (Q factor: 7U4)
     uint8 alpha_combining_weight          ;  ///< Measurement results (RSRP, RSSI) are combined across Rx antennas using
                                                         ///< this combining weight (Range: 0~1) (Q-factor: 5U4)
   uint8 reserved2                       ; 
  uint16 start_offset_rx[2]                           ;  ///< Starting offset per Rx antenna in CIR domain of threshold computation
                                                         ///< window for Rx0 energies
  uint16 threshold_rx[2]                              ;  ///< Measurement threshold per Rx antenna
}lte_LL1_log_srch_serving_cell_measurement_results_ind_struct;



/*==========================================================================*/
/*! @brief
 * 5 LTE LL1 Neighbor Cell Measurements and Tracking (Log code ID: 0xB119)
 *
 *  This log packet gives important parameters related to measurements. The log 
 *  record is generated 
 *  every scheduling period (SP) which is 40 ms when the neighbor measurements 
 *  and tracking module is enabled. 
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_SRCH_NEIGHBOR_CELL_MEASUREMENTS_AND_TRACKING_VERSION 3
#define LTE_LL1_LOG_SRCH_NUMBER_OF_NCELL_MTF_RECORDS_FIXED 8

/*! @brief neighbor_cell_measurements_and_tracking number_of_ncell_mtf_records_fixed struct
 */
typedef struct
{
  uint32 cell_id                          : 9;  ///< Range: 0~511
  uint32 cp_type                          : 1;  ///< 0: Normal CP
                                                         ///< 1: Extended CP
  uint32 tier_index                       : 2;  ///< Tier the cell belongs to. Range: 0~2
  uint32 number_of_tx_ant                 : 2;  ///< Number of Tx antennas used for measuing this
                                                         ///< neighbor cell
  uint32 reserved0                        : 18; 
   uint8 sf_numbers_for_measurements[8]               ;  ///< Subframes (#) per averaging period used for
                                                         ///< neighbor cell channel estimation
  uint16 start_offset_rx[2]                           ;  ///< Starting offset of window used for threshold
                                                         ///< computation. Starting position depends on TTL.
  uint16 threshold_rx[2]                              ;  ///< Measurement threshold of cell per Rx antenna
  uint16 ttl_il_disc_rx0[8]                           ;  ///< TTL inner loop discriminant per averaging period
                                                         ///< for Rx antenna 0. Q-factor: 16S8
  uint16 ttl_il_disc_rx1[8]                           ;  ///< TTL inner loop discriminant per averaging period
                                                         ///< for Rx antenna 1. Q-factor: 16S8
  uint16 ttl_ol_disc_rx0[8]                           ;  ///< TTL outer loop discriminant per averaging period
                                                         ///< for Rx antenna 0. Q-factor: 16S8
  uint16 ttl_ol_disc_rx1[8]                           ;  ///< TTL outer loop discriminant per averaging period
                                                         ///< for Rx antenna 1. Q-factor: 16S8
  uint32 ftl_il_disc[8]                               ;  ///< FTL inner loop discriminant per averaging
                                                         ///< period. Q-factor: Q19
  uint32 ftl_ol_disc[8]                               ;  ///< FTL outer loop discriminant per averaging
                                                         ///< period. Q-factor: Q19
  uint32 frame_bndry_ref_time_rx[2]                   ;  ///< Neighbor cell frame boundary per Rx antenna as a
                                                         ///< modulo 10ms offset including neighbor cell TTL
                                                         ///< adjustment.
  uint32 ftl_cumu_offset_dig_rotator     ;  ///< FTL cumulative frequency offset at the digital
                                                         ///< rotator
  uint32 ftl_cumu_offset_vctcxo          ;  ///< FTL cumulative frequency offset at the VCTCXO
                                                         ///< oscillator
}lte_LL1_log_srch_neighbor_cell_measurements_and_tracking_ncell_mtf_records_s;


/*! @brief neighbor_cell_measurements_and_tracking main struct
 */
typedef struct
{
  // WORD0
  uint32 version                          : 8;  ///< Log packet version. Range: 0~255.
                                                   ///< Version = 1 for the log packet structure
                                                   ///< described below
  uint32 total_num_ncells                 : 5;  ///< Total number of neighbor cells. The packet has a
                                                   ///< maximum of 8.
  uint32 sp_cnt_in_mp                     : 3;  ///< Scheduling period count in Measurement Period
  uint32 system_frame_number              : 10; ///< System frame number (range 0..1023)
  uint32 sub_frame_number                 : 4;  ///< Sub-frame number (Range 0..9)
  uint32 mode                             : 2;  ///< 0: Offline DRX
                                                ///< 1: Online DRX
                                                ///< 2: Connected
  // WORD1
  uint32 earfcn                              ;  ///< Channel raster number
  // WORD2
  uint32 duplexing_mode                   : 1;  ///< Duplexing mode
                                                         ///< 0: FDD
                                                         ///< 1: TDD
  uint32 num_swrp_per_mp                  : 3;  ///< Number of software reporting periods per
                                                         ///< measurement period
  uint32 num_cnf_msg_sent                 : 3;  ///< Number of CNF messages sent
  uint32 packet_counter                   : 1;  ///< 0: First log packet (Cells 0 ~ 7)
                                                         ///< 1: Second log packet (Cells 8~ 15)
  uint32 t_avail                          : 6;  ///< Time available for neighbor measurements in the
                                                         ///< schduling period (in ms)
  uint32 num_tiers                        : 2;  ///< Total number of cell tiers. Range: 1~3
  uint32 reserved0                        :16;
  // WORD3
  uint8 iir_coeff_meas_rslts_per_tier[3]     ;  ///< IIR coefficient used to filter measurement
                                                ///< results across SPs. Q-factor: 8U7
  uint8 reserved1                            ;
  // WORD4
  uint8 ap_per_tier[3]                       ;  ///< Averaging period per tier. A neighbor cell's
                                                ///< energy values are averaged every AP
  uint32 number_of_rx_ant                 : 2;  ///< Number of Rx antennas used for neighbor
                                                ///< measurements
  uint32 reserved2                        : 6; 
  lte_LL1_log_srch_neighbor_cell_measurements_and_tracking_ncell_mtf_records_s lte_LL1_log_srch_neighbor_cell_measurements_and_tracking_ncell_mtf_records[LTE_LL1_LOG_SRCH_NUMBER_OF_NCELL_MTF_RECORDS_FIXED];
}lte_LL1_log_srch_neighbor_cell_measurements_and_tracking_ind_struct;



/*==========================================================================*/
/*! @brief
 * 6 LTE LL1 Serving Cell FTL Results (Log code ID: 0xB11B)
 *
 *  This log packet gives important parameters related to the serving cell 
 *  frequency tracking loop (FTL). The log record is generated once every 20 
 *  sub-frames. 
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_SRCH_SERVING_CELL_FTL_RESULTS_VERSION 46
#define LTE_LL1_LOG_SRCH_NUMBER_OF_FTL_RECORDS_FIXED 20

/*! @brief serving_cell_ftl_results number_of_ftl_records_fixed struct
 */
typedef struct
{
  uint32 sfn_sf                          ;  ///< frame and subframe number
  uint32 outer_loop_filter_state         ;  ///< Outer loop filter state (Hz) in 32S12
                                                         ///< Range: -15 KHz to +15 KHz
  uint32 inner_loop_filter_state         ;  ///< Inner loop filter state (Hz)
                                                         ///< 32S12
                                                         ///< Range: -15kHz to +15kHz
  uint32 filtered_freq_error             ;  ///< Filtered frequency error in 32S12.
                                                         ///< Input to RV-AFC outer loop.
  uint32 inner_loop_discriminant         ;  ///< Inner loop discriminant
                                                         ///< Q-factor: 32S19

  uint8  snrw_enabled                    ;  ///< boolean indicate if SNRW is enabled
  uint8  iloop_freeze                    ;  ///< boolean indicate if iloop is frozen
  uint16 reserved0;

  uint32 real_input_to_arctangent        ;  ///< Real input to arctangent in inner loop
  uint32 imaginary_input_to_arctangent   ;  ///< Imaginary input to arctangent in inner loop
  uint32 epir_fe;                        ///< Q-factor:32S19
  uint32 epir_te;                        ///< Q-factor:32S30
  uint32 epir_r_plus_re;                 ///< weighted R plus real
  uint32 epir_r_plus_im;                 ///< weighted R plus imaginary
  uint32 epir_r_minus_re;                ///< weighted R minus real
  uint32 epir_r_minus_im;                ///< weighted R minus imaginary
  uint16 snr_weight_rx0[4]               ;  ///< SNR weights per Tx antenna for Rx0.
                                            ///< Q-factor: 16U16.
  uint16 snr_weight_rx1[4]               ;  ///< SNR weights per Tx antenna for Rx1.
																												 ///< Q-factor: 16U16.
  uint16 mrc_weight_rx0[2];              ///< MRC weights per Tx antenna for Rx0.
                                         ///< Q-factor: 16U16.
  uint16 mrc_weight_rx1[2];              ///< MRC weights per Tx antenna for Rx1.
                                         ///< Q-factor: 16U16.
  uint32 rx_resampler_fcw0               ;  ///< for XO mode only RX resampler control word
  uint32 rx_resampler_fcw1               ;  ///< for XO mode only TX resampler control word
  uint32 tx_resampler_fcw                ;  ///< for xo mode only TX resampler control word
  uint32 mstmr_fcw                       ;  ///< for XO mode only mstmr control word 
}lte_LL1_log_srch_serving_cell_ftl_results_ftl_records_s;


/*! @brief serving_cell_ftl_results main struct
 */
typedef struct
{
  // WORD0
  uint32 version                          : 8;  ///< Log packet version. Range: 0...255.
                                                   ///< Version = 2 for the log packet structure
                                                   ///< described below
  uint32 system_frame_number              : 10;  ///< System frame number (range 0..1023)
  uint32 sub_frame_number                 : 4;  ///< Sub-frame number (Range 0..9)

  uint32 serving_cell_id                  : 9;  ///< Range: 0...503
  uint32 reserved0                        : 1;
  // WORD1
  uint32 carrier_index                                  : 4;  ///< 0-PCC, 1-SCC
  uint32 number_of_records                              : 5;  ///< Range: 1-20
  uint32 ftl_mode                                       : 4;  ///< Bit0: Pull in range (Normal/Extended),
                                                              /// <Bit1: Correlation filtering (FIR/IIR)
  uint32 reserved1                                      :15;
  uint32 num_cell                                       : 4;  ///< 1 serving cell + # of IC cells

  // WORD2,3
  uint16 cell_id_list[1 + LTE_MAX_NUM_CRS_IC_CELLS];          ///< cell ID (0 to 503)
  // WORD4
  uint32 earfcn                                         ;     ///< EARFCN, 0...65535
  // WORD5
  uint16 rvafc_inner_loop_gain;                               ///< Unitless (0 to 1). Q-format: 16U16

  uint16 rvafc_outer_loop_gain;                               ///< Unitless (0 to 1). Q-format: 16U16
  // WORD6
  uint16 rotator_conversion_factor;                           ///< 2^25 / fs in 16U12
  uint16 reserved2;


  lte_LL1_log_srch_serving_cell_ftl_results_ftl_records_s lte_LL1_log_srch_serving_cell_ftl_results_ftl_records[LTE_LL1_LOG_SRCH_NUMBER_OF_FTL_RECORDS_FIXED];
}lte_LL1_log_srch_serving_cell_ftl_results_ind_struct;



/*==========================================================================*/
/*! @brief
 * 7 LTE LL1 Serving Cell TTL Results (Log code ID: 0xB11D)
 *
 *  This log packet gives important parameters related to the serving cell time 
 *   tracking loop (TTL). The log record is generated once every 20 subframes 
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_SRCH_SERVING_CELL_TTL_RESULTS_VERSION 44
#define LTE_LL1_LOG_SRCH_NUMBER_OF_TTL_RECORDS_FIXED 20

/*! @brief serving_cell_ttl_results number_of_ttl_records_fixed struct
 */
typedef struct
{
  uint32 frame_number                              :16;///< provides frame number where TTL results are logged
  uint32 sub_frame_number                          :16;///< provides subframe number where TTL results are logged
  uint32 total_timing_adjustment_rx[2];            ///< Total timing adjustment per Rx antenna in
                                                   ///< sample server samples.
                                                   ///< Q-factor: 16S0
  uint16 inst_timing_adjustment_rx[2];             ///< Instantaneous timing adjustment per Rx antenna
                                                   ///< in sample server samples.
  uint32 inner_loop_discriminant_rx[2];            ///< Inner loop discriminant per Rx antenna.
                                                   ///< Q-factor: 24S16

  uint16 oneshot_ttl_loop_gain;                    ///< One-shot TTL loop gain.
                                                   ///< Q-factor: 16U16
  int16 com_adj;                                   ///< Average COM loop adjustment
                                                   ///< Q-factor: 16S0
  uint16 cell_weight[1 + LTE_MAX_NUM_CRS_IC_CELLS]; ///<  cell weight for SE combining. Temp in Jolokia
}lte_LL1_log_srch_serving_cell_ttl_results_ttl_records_s;


/*! @brief serving_cell_ttl_results main struct
 */
typedef struct
{
  uint32 version                                        : 8;  ///< Log packet version. Range: 0...255.
  uint32 serving_cell_id                                : 9;  ///< Range: 0...503
  uint32 num_cell                                       : 4;  /* 1 serving cell + # of IC cells (internal) */
  uint32 reserved0                                      :11;

  uint16 cell_id_list[1 + LTE_MAX_NUM_CRS_IC_CELLS];          /* cell ID (0 to 503) (internal) */

  uint32 system_frame_number                            :10;  ///< System frame number (range 0..1023)
  uint32 sub_frame_number                               : 4;  ///< Sub-frame number (Range 0..9)
  uint32 carrier_index                                  : 4;  ///< 0-PCC, 1-SCC
  uint32 reserved1                                      : 8;
  uint32 com_loop_enable                                : 1;  ///< indicating if com loop is enabled or not
  uint32 number_of_records                              : 5;  ///< Range: 1-20

  uint16 inner_loop_gain;                                     ///< Unitless (0 to 1) in 16U16
  uint16 inner_loop_step_size;                                ///< Unitless (0 to 1) in 16U16

  lte_LL1_log_srch_serving_cell_ttl_results_ttl_records_s lte_LL1_log_srch_serving_cell_ttl_results_ttl_records[ LTE_LL1_LOG_SRCH_NUMBER_OF_TTL_RECORDS_FIXED ];
}lte_LL1_log_srch_serving_cell_ttl_results_ind_struct;


#endif //LTE_LL1_LOG_SRCH_H
