/*!
  @file
  lte_LL1_log_rxfe.h

  @brief
  rxfe log packet data structures

  @detail
  !!! Please do not modify !!!
  This file was auto-generated on Mon Nov 14 17:14:23 2011.
  Command line used: perl w2h.pl log_rxfe rxfe 2 4
   Note: The params, [2 4], correspond to version numbers for
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/lte_LL1_log_rxfe.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/
#ifndef LTE_LL1_LOG_RXFE_H
#define LTE_LL1_LOG_RXFE_H


/*==========================================================================*/
/*! @brief
 * 1 AGC (Log code ID: 0xB111)
 *
 *  This log packet gives important parameters related to the receiver 
 *  front end automatic gain control (AGC). The log record is generated 
 *  every 20 records (10ms with 1Rx antenna per record).
 *  In CA scenario, Primary cell and secondary cell are logged in separate log packets.
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_RXFE_AGC_VERSION 21
#define LTE_LL1_LOG_RXFE_NUMBER_OF_RECORDS_FIXED 20

/*! @brief agc number_of_records_fixed struct
 */
typedef struct
{
  uint32 rx_antenna_index                 : 2;  ///< Range 0 to 2 (0: Rx0, 1:Rx1, 2: both Rx)
  uint32 lna_gain_state                   : 3;  ///< LNA Gain State
                                                         ///< Range 0..5
  uint32 sf_type                          : 2;  ///< range 0 to 3,  0 for special subframe
                                                         ///< 1: uplink subframe, 2: downlink subframe
                                                         ///< 3: uplink or downlink subframe
  uint32 reserved0                        : 4;

  uint32 dvga_bias                        : 5;  ///< 5-bit signed DVGA exponent used in HW
                                                         ///< programming. Calculate DVGA in dB as
                                                         ///< 20*log10[x/pow(2,15)*pow(2,y)] where x= DVGA
                                                         ///< scale, y= DVGA bias
  uint32 dvga_scale                       :16;  ///< 16-bit signed DVGA mantissa used in HW
                                                         ///< programming. Calculate in linear scale as
                                                         ///< x/pow(2,15) where x= DVGA scale
  uint32 broadband_rssi_estimate          :12;  ///< 12-bit signed Broadband RSSI estimate value in
                                                         ///< 0.0625 dBm resolution. Range: (-120 dBm to -10
                                                         ///< dBm).
                                                         ///< To be displayed in dBm scale after calculating
                                                         ///< x/16  where
                                                         ///< x = Broadband RSSI estimate
                                                         ///< x=-1 (unsigned raw number is all 1s) is to be
                                                         ///< interpreted as field not populated
  uint32 measured_total_rssi              :10;  ///< 10-bit signed RSSI value for all tones in 0.25
                                                         ///< dBm resolution. Range: (-120 dBm to -10 dBm).
                                                         ///< To be displayed in dBm scale after calculating
                                                         ///< x/4  where
                                                         ///< x = Measured total RSSI
                                                         ///< x=-1 (unsigned raw number is all 1s) is to be
                                                         ///< interpreted as field not populated
  uint32 measured_inband_rssi             :10;  ///< 10-bit signed RSSI value for all tones in 0.25
                                                         ///< dBm resolution. Range: (-120 dBm to -10 dBm).
                                                         ///< To be displayed in dBm scale after calculating
                                                         ///< x/4  where
                                                         ///< x = Measured inband  RSSI
                                                         ///< x=-1 (unsigned raw number is all 1s) is to be
                                                         ///< interpreted as field not populated
}lte_LL1_log_rxfe_agc_records_s;


/*! @brief agc main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Log packet version; range is 0 to 255
                                                         ///< Version - 2 for the log packet structure
                                                         ///< described below
  uint32 number_of_records                : 5;  ///< Range is 1 to 20
  uint32 agc_mode                         : 4;  ///< Range is 0 to 15 (0:Freq scan, 1:Init Acq,
                                                         ///< 2:Traffic, 3:PBCH, 4:Inter-freq, 5:Online-DRX,
                                                         ///< 6:Offline-DRX, 7: X2L idle, 8: X2L_CONN,
                                                         ///< 9: TDD_SRCH_PIPELINE, 10: X2L_TDD_MEAS_LNA,
                                                         ///< 11: X2L_TDD_MEAS_VCELLTIME, 15:OFF)
  uint32 system_frame_number              :10;  ///< System frame number; range is 0 to 1023
  uint32 sub_frame_number                 : 4;  ///< Subframe number; range is 0 to 9
  uint32 reserved0                        : 1; 
  uint16 inner_loop_gain                  ;  ///< Unitless (0 to 1) in 16U16. Calculated as
                                                         ///< x/pow(2,16) where x = Inner loop gain
  uint16 outer_loop_gain                  ;  ///< Unitless (0 to 1) in 16U16. Calculated as x/
                                                         ///< pow(2,16) where x = outer loop gain
  uint32 carrier_index                    : 4; ///< 0-PCC, 1-SCC 
  uint32 reserved1                        : 28; 
  lte_LL1_log_rxfe_agc_records_s lte_LL1_log_rxfe_agc_records[LTE_LL1_LOG_RXFE_NUMBER_OF_RECORDS_FIXED];
}lte_LL1_log_rxfe_agc_ind_struct;



/*==========================================================================*/
/*! @brief
 * 2  AGC INT(Log code ID: 0xB112) -Internal
 *
 *  This internal log packet gives important parameters related to the 
 *  receiver front end automatic gain control (AGC). The log record is 
 *  generated every 1 ms.
 *  In CA scenario, Primary cell and secondary cell are logged in separate log packets.
---------------------------------------------------------------------------*/
#define LTE_LL1_LOG_RXFE_AGC_INT_VERSION 21
//#define LTE_LL1_RXAGC_NIKEL_TARGET_LOG_ID      1
#define LTE_LL1_LOG_RXFE_RS_NUMBER_OF_RECORDS_FIXED 4
#define LTE_LL1_LOG_RXFE_NUMBER_OF_RECORDS 14

/*! @brief agc_int rs_number_of_records_fixed struct
 */
typedef struct
{
  uint32 tx0_crs_ee_raw                  ;  ///< 32S31 for  raw symbol CRS energy estimate;
                                                         ///< calculate in linear scale as x/pow(2,31) where x =
                                                         ///< CRS EE raw
  uint32 tx1_crs_ee_raw                  ;  ///< 32S31 for raw symbol CRS energy estimate; calculate
                                                         ///< in linear scale as x/pow(2,31) where x = CRS EE raw
}lte_LL1_log_rxfe_agc_int_rs_records_s;


/*! @brief agc_int number_of_records struct
 */
typedef struct
{
  uint32 rxfft_scale                      :16;  ///< 16-bit signed RxFFT gain mantissa used in hardware
                                                         ///< programming; calculate in linear scale as
                                                         ///< x/pow(2,15) where x = RxFFT scale
  uint32 rxfft_input_bias                 : 5;  ///< 5-bit signed RxFFT gain exponent used in hardware
                                                         ///< programming;
  uint32 reserved0                        : 11; 
}lte_LL1_log_rxfe_agc_int_records_s;


/*! @brief agc_int main struct
 */
typedef struct
{
  uint32 version                          : 8;  ///< Range is 0 to 255
                                                         ///< Version = 4 for the log packet structure described
                                                         ///< below
  uint32 reserved5                        : 4;  
  uint32 sub_frame_number                 : 4;  ///< Subframe number; range is 0 to 9
  uint32 system_frame_number              :10;  ///< System frame number; range is 0 to 1023
  uint32 rx_antenna_index                 : 1;  ///< Range 0 to1
  uint32 agc_mode                         : 4;  ///< To be displayed as:
                                                         ///< 0: "Freq scan"
                                                         ///< 1: "Init Acq"
                                                         ///< 2: "Traffic"
                                                         ///< 3: "PBCH"
                                                         ///< 4: "Inter-freq"
                                                         ///< 5: "Online-DRX"
                                                         ///< 6: "Offline-DRX"
                                                         ///< 7: "Inter-RAT/Inter-Freq Idle"
                                                         ///< 8: "Inter-RAT/Inter-freq Connected"
                                                         ///< 9: "TDD_SRCH_PIPELINE" 
                                                         ///< 10: "X2L_TDD_MEAS_LNA"
                                                         ///< 11: "X2L_TDD_MEAS_VCELLTIME"
                                                         ///< 15: "OFF"
  uint32 reserved0                        : 1; 

  uint32 carrier_index                    : 4; ///< 0-PCC, 1-SCC 
  uint32 reserved                         : 28; 

  uint16 inner_loop_gain                 ;  ///< 16S15 number. To be displayed in [0,1] range after
                                                         ///< dividing by 2^15.
  uint16 inner_loop_gain_marginal        ;  ///< 16S15 number. To be displayed in [0,1] range after
                                                         ///< dividing by 2^15.
  uint32 outer_loop_gain                 ;  ///< 32S31 number. To be displayed in [0,1] range after
                                                         ///< dividing by 2^31.
  uint32 dc_loop_constant                 : 4;  ///< DC loop constant K. DC loop gain is 2^K.
  uint32 measured_in_band_rssi            :12;  ///< 12-bit signed RSSI value for user-allocated tones
                                                         ///< in 0.0625 dBm resolution; range is -120 dBm to -10
                                                         ///< dBm.
                                                         ///< To be displayed in dBm scale after calculating
                                                         ///< x/16 where
                                                         ///< x = Measured in-band RSSI
  uint32 measured_total_rssi              :12;  ///< 12-bit signed RSSI value for all tones in 0.0625
                                                         ///< dBm resolution. Range: (-120 dBm to -10 dBm).
                                                         ///< To be displayed in dBm scale after calculating
                                                         ///< x/16 where
                                                         ///< x = Measured total RSSI
  uint32 reserved1                        : 4; 
  uint32 broadband_rssi_estimate          :12;  ///< 12-bit signed broadband RSSI estimate in 0.0625 dBm
                                                         ///< resolution. Range: (-120 dBm to -10 dBm).
                                                         ///< To be displayed in dBm scale after calculating
                                                         ///< x/16 where
                                                         ///< x = broadband RSSI
  uint32 curr_total_rssi_to_iir           :12;  ///< 12-bit signed RSSI value, input to iir, in 0.0625
                                                         ///< dBm resolution. Range: (-120 dBm to -10 dBm).
                                                         ///< To be displayed in dBm scale after calculating
                                                         ///< x/16 where
                                                         ///< x = current total RSSI, input to IIR
  uint32 reserved2                        :8; 
  uint32 fft_ee_max                      ;  ///< 32S31 for the max FFT energy estimate per subframe;
                                                         ///< calculate in linear scale as x/pow(2,31) where x =
                                                         ///< FFT EE max
  uint32 crs_ee_avg                      ;  ///< 32S31 for  the average CRS energy estimate per
                                                         ///< subframe; calculate in linear scale as x/pow(2,31)
                                                         ///< where x = CRS EE avg
  uint32 fft_ee_accum                    ;  ///< 32S31 for the accum FFT energy estimate per subframe;
                                                         ///< calculate in linear scale as x/pow(2,31) where x =
                                                         ///< FFT EE accum
  uint32 hybrid_ee                       ;  ///< 32S31 for the hybrid energy estimate per subframe;
                                                         ///< calculate in linear scale as x/pow(2,31) where x =
                                                         ///< hybrid EE
  uint32 bb_ee                           ;  ///< 32u16 for the broadband energy estimate per
                                                         ///< subframe; calculate in linear scale as x/pow(2,16)
                                                         ///< where x = BB EE reading from HW
  uint32 aci_power                       ;  ///< 32S24 for the ACI power per subframe; calculate in
                                                         ///< dB scale as x/pow(2,24) where x = ACI power
  uint32 dc_i_offset                     ;  ///< Current instantaneous DC_i; 32S31, x * 841.1/(2^31)
                                                         ///< in unit(mv)
  uint32 dc_q_offset                     ;  ///< Current instantaneous DC_q; 32S31, x * 841.1/(2^31)
                                                         ///< unit(mv)
  uint32 dc_accum_ee_crs                 ;  ///< 32S31 for accumulated DC energy for CRS symbols.
                                                         ///< calculate in linear scale as x/pow(2,31)  where x =
                                                         ///< DC accum EE
  uint32 inband_accum_ee_crs             ;  ///< 32S31 for accumulated Inband energy for CRS
                                                         ///< symbols; calculate in linear scale as x/pow(2,31)
                                                         ///< where x = inband accum EE
  uint32 outband_accum_ee_crs            ;  ///< 32S31 for accumulated Outband energy for CRS
                                                         ///< symbols; calculate in linear scale as x/pow(2,31)
                                                         ///< where x = outband accum EE
  uint32 dc_max_ee_crs                   ;  ///< 32S31 for max DC energy for CRS symbols; calculate
                                                         ///< in linear scale as x/pow(2,31)  where x = DC max EE
  uint32 inband_max_ee_crs               ;  ///< 32S31 for max Inband energy for CRS symbols;
                                                         ///< calculate in linear scale as x/pow(2,31)  where x =
                                                         ///< inband max EE
  uint32 outband_max_ee_crs              ;  ///< 32S31 for max Outband energy for CRS symbols;
                                                         ///< calculate in linear scale as x/pow(2,31)  where x =
                                                         ///< outband max EE
  uint32 dc_accum_ee_non_crs             ;  ///< 32S31 for accumulated DC energy for  non-CRS
                                                         ///< symbols; calculate in linear scale as x/pow(2,31)
                                                         ///< where x = dc accum EE for non-CRS
  uint32 inband_accum_ee_non_crs         ;  ///< 32S31 for accumulated Inband energy for non-CRS
                                                         ///< symbols; calculate in linear scale as x/pow(2,31)
                                                         ///< where x = inband accum EE for non-CRS
  uint32 outband_accum_ee_non_crs        ;  ///< 32S31 for accumulated Outband energy for non-CRS
                                                         ///< symbols; calculate in linear scale as x/pow(2,31)
                                                         ///< where x = outband accum EE for non-CRS
  uint32 dc_max_ee_non_crs               ;  ///< 32S31 for max DC energy for non-CRS symbols;
                                                         ///< calculate in linear scale as x/pow(2,31)  where x =
                                                         ///< dc max EE for non-CRS
  uint32 inband_max_ee_non_crs           ;  ///< 32S31 for max Inband energy for non-CRS symbols;
                                                         ///< calculate in linear scale as x/pow(2,31)  where x =
                                                         ///< inband max EE for non-CRS
  uint32 outband_max_ee_non_crs          ;  ///< 32S31 for max Outband energy for non-CRS symbols;
                                                         ///< calculate in linear scale as x/pow(2,31)  where x =
                                                         ///< outband max EE for non-CRS
  lte_LL1_log_rxfe_agc_int_rs_records_s lte_LL1_log_rxfe_agc_int_rs_records[LTE_LL1_LOG_RXFE_RS_NUMBER_OF_RECORDS_FIXED];
  uint32 previous_lna_gain_state          : 3;  ///< LNA Gain State for next subframe; range is 0 to 5
  uint32 previous_lna_offset              :12;  ///< 12-bit signed value for previous LNA offset  in
                                                         ///< 0.0625 dB resolution. To be displayed in dB scale
                                                         ///< after calculating  x/16 where
                                                         ///< x =previous LNA offset
                                                         ///<range is -128 to 127
  uint32 reserved3                        :17; 
  uint32 previous_DVGA_dB                ;  ///< 32S24 for previous DVGA; calculate in dB
                                                         ///< scale as x/pow(2,24) where x = previous DVGA.;
                                                         ///< range is -128 dB to 127 dB  uint32 reserved3                        :17; 

  uint32 agc_error                       ;  ///< 32S24 for unfiltered AGC error on current subframe;
                                                         ///< calculate in dB scale as x/pow(2,24) where x = AGC
                                                         ///< error; range is -128 dB to 127 dB
  uint32 agc_error_accum                 ;  ///< 32S24 for  AGC accumulated error; calculate in dB
                                                         ///< scale as x/pow(2,24) where x = AGC error accum.;
                                                         ///< range is -128 dB to 127 dB
  uint32 next_lna_gain_state              : 3;  ///< LNA Gain State for next subframe; range is 0 to 5
  uint32 next_lna_offset                  :12;  ///< 12-bit signed value for next LNA offset  in 0.0625
                                                         ///< dB resolution. To be displayed in dB scale after
                                                         ///< calculating  x/16 where
                                                         ///< x =next LNA offset
                                                         ///<range is -128 to 127
  uint32 aci_detect                       : 1;  ///< 0 - No jammer detected
                                                         ///< 1 - Jammer detected
  uint32 deficit_bias                     : 3;  ///< Integer in [2,7] range representing the deficit
                                                         ///< bias to avoid saturation in FFT
  uint32 number_of_records                : 4;  ///< Integer between [0-15]
  uint32 reserved4                        : 9; 
  lte_LL1_log_rxfe_agc_int_records_s lte_LL1_log_rxfe_agc_int_records[LTE_LL1_LOG_RXFE_NUMBER_OF_RECORDS];
}lte_LL1_log_rxfe_agc_int_ind_struct;



#endif //LTE_LL1_LOG_RXFE_H
