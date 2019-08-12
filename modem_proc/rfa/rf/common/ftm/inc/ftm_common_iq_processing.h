#ifndef FTM_COMMON_IQ_PROCESSING_H
#define FTM_COMMON_IQ_PROCESSING_H
/*! 
  @file
  ftm_common_iq_processing.h
 
  @brief
  This file defines function prototypes, structures and global variables for IQ
  processing of self-test captures for all technologies
  
  @brief
  This file defines function prototypes, structures and global variables for IQ
  processing of self-test captures for all technologies
  @{
*/

/*==============================================================================

  Copyright (c) 2013 - 2014 QUALCOMM Technologies Inc. All Rights Reserved

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

==============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_common_iq_processing.h#1 $ 
$DateTime: 2016/12/13 07:59:52 $ 
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/14/14   jmf     [selftest] Implement FBRx Noise Floor Cal
10/09/14   dbz     Added spectrum flatness support
08/18/14   jmf     [SelfTest] ACLR meas changes
08/27/13   aka     1x and WCDMA updates
07/30/13   aka     Updates in IQ processing and FFT algo
07/12/13   zhz     Initial version supporting IQ processing for Self Test

===========================================================================*/

/*===========================================================================

                         INTERNAL STRUCTURES

===========================================================================*/

#include "ftm_common_control.h"

typedef struct {
  uint16 carrier_bin_neg;
  uint16 carrier_bin_pos;
  uint16 eutra_start_bin_neg;
  uint16 eutra_end_bin_neg;
  uint16 eutra_start_bin_pos;
  uint16 eutra_end_bin_pos;
  uint16 utra1_start_bin_neg;
  uint16 utra1_end_bin_neg;
  uint16 utra1_start_bin_pos;
  uint16 utra1_end_bin_pos;
  uint16 utra2_start_bin_neg;
  uint16 utra2_end_bin_neg; 
  uint16 utra2_start_bin_pos;
  uint16 utra2_end_bin_pos;
  boolean apply_rrc_filter_carrier;
}ftm_common_aclr_bins_struct_type;

typedef struct {
  uint16 carrier_bin_neg;
  uint16 carrier_bin_pos;
  uint16 adj_start_bin_neg;
  uint16 adj_end_bin_neg;
  uint16 adj_start_bin_pos;
  uint16 adj_end_bin_pos;
  uint16 alt1_start_bin_neg;
  uint16 alt1_end_bin_neg;
  uint16 alt1_start_bin_pos;
  uint16 alt1_end_bin_pos;
  uint16 alt2_start_bin_neg;
  uint16 alt2_end_bin_neg; 
  uint16 alt2_start_bin_pos;
  uint16 alt2_end_bin_pos;
}ftm_common_1x_aclr_bins_struct_type;

/*===========================================================================

                              FUNCTION PROTOTYPES

===========================================================================*/
/*!
  @brief  
  Parse the 32 bit aligned Tx IQ samples for little endian mem
 
  @details
  @param mode
  Input enum of the technology mode
  
  @param samples_array
  Input sample array
  
  @param sample_count
  Input number of samples after parsing
 
  @return 
*/
void ftm_common_parse_tx_IQ(
ftm_rfstate_enum_type mode,
int32* samples_array,
uint16 sample_count
);

/*!
  @brief  
  Parse the PCFL20 format IQ samples
 
  @details
  Take in the PCFL20 format iq samples with enough buffer to expand.
  The macro SHIFT_EXPONENT determines how many bits it expands.
  PCFL20 format expands to max 23 bits I and Q samples.
  Reverse spectrum flag will reverse the Q samples to negative

  @param mode
  Input enum of the technology mode
  
  @param samples_array
  Input sample array
  
  @param sample_count
  Input number of samples after parsing
 
  @return 
*/
void ftm_common_parse_PCFL20_to_IQ(
ftm_rfstate_enum_type mode,
int32* samples_array,
uint16 sample_count
);

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Initialize the required dynamic memories for performing ACLR calculations
 
  @details
  This function initialize all the required memories for different coefficients
  
  @param mode_current
  Input enum for the current technology mode
  
  @param bw_curren
  Input enum for the current LTE Band
  
  @return boolean - Api Status
  If TRUE: All the memories are allocated successfully
  If FALSE: Some parts failed during memory allocations
*/
boolean ftm_common_init_fft_data(
ftm_rfstate_enum_type mode_current,
rfcom_lte_bw_type bw_current
);

/*----------------------------------------------------------------------------*/

/*!
  @brief  
  De-initialize and free memory for the variables used for FFT calculation
 
  @details
  This function de-initialize and free memory for the variables used for FFT calculation
  
  @return 
*/
void ftm_common_deinit_fft_data(void);

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Get the window coefficients
 
  @details
  Get the 1024 window coefficients using the spline interpolation from the pre-calculated hard-coded window coeffs 

  @return 
*/
void ftm_common_get_hanning_coeffs(void);

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Apply the window coefficients to the input samples
 
  @details
  Apply the window coefficients 

  @param samples
  Input samples of size IQ_ARRAY_LENGTH in int32 format and needs to be in multiples of FFT_SIZE
  
  @param window_coeffs
  Input window coefficients in uint16 format in FFT_SIZE
  
  @param window_size
  Input window coefficients size

  @return 
*/
void ftm_common_apply_window(
int32* samples,
uint16* window_coeffs,
uint16 window_size
);

void ftm_common_get_flatness(
  /* const */
  const uint16 tx_bw,
  const uint16 fft_size,

  /* return vals */
  double *flatness_coefficient,
  uint16 *range1_ripple_dB,
  uint16 *range2_ripple_dB
);

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Compute Fast Fourier Transform
 
  @details
  Calculate the Fast Fourier Transform

  @param fft_size
  Input fft size
  
  @param data
  Input data to do FFT
  
  @return 
*/
void ftm_common_compute_fft(uint16 fft_size, int32* data);

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Calls FFT and calculate the average
 
  @details
  Calls the Fast Fourier Transform function and sums the 9 times 1024 FFT and return the values in a input buffer
  This API expects FFT data to be max 30 bit. After sqaure, sum and accumulation, it may overflow 
  64 bit (2*30 + 1 + 4 = 65) and so we accumalted result by 2 bits to keep it within 64 bits

  @param fft_size
  Input fft size
  
  @param samples_iq
  Input iq samples to compute FFT
  
  @return 
*/
void ftm_common_get_samples_fft_avg(uint16 fft_size, int32* samples_iq);

/*!
  @brief  
  Calculate the twiddle factor for fft
 
  @details
  Calculate the twiddle factor for 1024 or smaller Fast Fourier transform using hard-coded sine cosine tables

  @param fft_size
  Input fft size
  
  @return 
*/
void ftm_common_get_twiddle_factor(uint16 fft_size);

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Apply dec filter for FFT
 
  @details
  Apply dec filter for FFT

  @param filter_size
  Input filter size for the decimation filter
  
  @param tech
  Input enum for the technology mode
  
  @param bw
  Input enum for the bandwidth 
 
  @return 
*/
void ftm_common_get_dec_filter_coeffs(
uint16 filter_size,
ftm_rfstate_enum_type tech,
rfcom_lte_bw_type bw);

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Get the root raised cosine coefficients
 
  @details
  Get the root raised cosine coefficients by using the hard-coded values into 129 or 65 length array

  @param bw
  Input enum of the specific LTE Bandwidth
  
  @param rrc_size
  Input size of the coefficients
  
  @return
*/
void ftm_common_get_rrc(ftm_rfstate_enum_type mode_current, rfcom_lte_bw_type bw_current, uint8 rrc_size);

/*----------------------------------------------------------*/
/*!
  @brief  
  Calculate the bins number for integration
 
  @details
  This function integrate the value of the given bin index
  Apply the rrc before integrating depending on given rrc_coeff_flag
  
  @param tech
  Input enum of the specific Technology Mode
  
  @param bw_type
  Input enum of the LTE bandwidth type
  
  @param fft_size
  
  @return
  WCDMA:
                            Bin Number:        Correspond Freq:            Standard Frequency:
    carrier_bin_neg =       960
    carrier_bin_pos =       63
    utra1_start_bin_neg =   764                -4995000                       -5000000
    utra1_end_bin_neg =     921
    utra1_start_bin_pos =   103                 4995000                        5000000
    utra1_end_bin_pos =     230
    utra2_start_bin_neg =   627               -10005000                      -10000000
    utra2_end_bin_neg =     754
    utra2_start_bin_pos =   270                10005000                       10000000
    utra2_end_bin_pos =     397

  LTE1P4
                            Bin Number:        Correspond Freq:            Standard Frequency:
    carrier_bin_neg =       1006
    carrier_bin_pos =       17
    eutra_start_bin_neg =   960               -1395000                       -1400000
    eutra_end_bin_neg =     995
    eutra_start_bin_pos =   29                 1395000                        1400000
    eutra_end_bin_pos =     64
    utra1_start_bin_neg =   854               -3195000                       -3200000
    utra1_end_bin_neg =     981
    utra1_start_bin_pos =   43                 3195000                        3200000
    utra1_end_bin_pos =     170
    utra2_start_bin_neg =   687               -8205000                       -8200000
    utra2_end_bin_neg =     814
    utra2_start_bin_pos =   210                8205000                        8200000
    utra2_end_bin_pos =     337
                                                                                                                
  LTE3
                            Bin Number:        Correspond Freq:            Standard Frequency:
    carrier_bin_neg =       979
    carrier_bin_pos =       44
    eutra_start_bin_neg =   879               -3015000                       -3000000
    eutra_end_bin_neg =     968
    eutra_start_bin_pos =   56                 3015000                        3000000
    eutra_end_bin_pos =     145
    utra1_start_bin_neg =   827               -4005000                       -4000000
    utra1_end_bin_neg =     954
    utra1_start_bin_pos =   70                 4005000                        4000000
    utra1_end_bin_pos =     197
    utra2_start_bin_neg =   660               -9015000                       -9000000
    utra2_end_bin_neg =     787
    utra2_start_bin_pos =   237                9015000                        9000000
    utra2_end_bin_pos =     364
                
  LTE5
                            Bin Number:        Correspond Freq:            Standard Frequency:
    carrier_bin_neg =       949
    carrier_bin_pos =       74
    eutra_start_bin_neg =   783               -4995000                       -5000000
    eutra_end_bin_neg =     932
    eutra_start_bin_pos =   92                 4995000                        5000000
    eutra_end_bin_pos =     241
    utra1_start_bin_neg =   794               -4995000                       -5000000
    utra1_end_bin_neg =     921
    utra1_start_bin_pos =   103                4995000                        5000000
    utra1_end_bin_pos =     230
    utra2_start_bin_neg =   627              -10005000                      -10000000
    utra2_end_bin_neg =     754
    utra2_start_bin_pos =   270               10005000                       10000000
    utra2_end_bin_pos =     397    
                
  LTE10
                            Bin Number:        Correspond Freq:            Standard Frequency:
    carrier_bin_neg =       874
    carrier_bin_pos =       149
    eutra_start_bin_neg =   541               -10005000                     -10000000
    eutra_end_bin_neg =     840
    eutra_start_bin_pos =   184                10005000                      10000000
    eutra_end_bin_pos =     483
    utra1_start_bin_neg =   710               -7515000                       -7500000
    utra1_end_bin_neg =     837
    utra1_start_bin_pos =   187                7515000                        7500000
    utra1_end_bin_pos =     314
    utra2_start_bin_neg =   544              -12495000                      -12500000
    utra2_end_bin_neg =     671
    utra2_start_bin_pos =   353               12495000                       12500000
    utra2_end_bin_pos =     480                    
                
  LTE15
                            Bin Number:        Correspond Freq:            Standard Frequency:
    carrier_bin_neg =       912
    carrier_bin_pos =       112
    eutra_start_bin_neg =   662               -15000000                     -15000000
    eutra_end_bin_neg =     886
    eutra_start_bin_pos =   138                15000000                      15000000
    eutra_end_bin_pos =     362
    utra1_start_bin_neg =   826               -9990000                      -10000000
    utra1_end_bin_neg =     889
    utra1_start_bin_pos =   135                9990000                       10000000
    utra1_end_bin_pos =     198
    utra2_start_bin_neg =   742              -15030000                      -15000000
    utra2_end_bin_neg =     805
    utra2_start_bin_pos =   219               15030000                       15000000
    utra2_end_bin_pos =     282    
                
  LTE20
                            Bin Number:        Correspond Freq:            Standard Frequency:
    carrier_bin_neg =       874
    carrier_bin_pos =       149
    eutra_start_bin_neg =   541               -20010000                     -20000000 
    eutra_end_bin_neg =     840
    eutra_start_bin_pos =   184                20010000                      20000000
    eutra_end_bin_pos =     483
    utra1_start_bin_neg =   784               -12510000                     -12500000
    utra1_end_bin_neg =     847
    utra1_start_bin_pos =   177                12510000                      12500000
    utra1_end_bin_pos =     240
    utra2_start_bin_neg =   701               -17490000                     -17500000
    utra2_end_bin_neg =     764
    utra2_start_bin_pos =   260                17490000                      17500000
    utra2_end_bin_pos =     323    

*/
ftm_common_aclr_bins_struct_type ftm_common_get_aclr_bins_struct
(
  ftm_rfstate_enum_type tech, 
  rfcom_lte_bw_type bw_type, 
  uint16 fft_size, 
  uint32 samp_rate
);

void ftm_common_get_aclr_bins_struct_with_samp_rate
(
  ftm_rfstate_enum_type tech,
  rfcom_lte_bw_type bw_type,
  uint16 fft_size,
  uint32 samp_rate
);
/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Get the Adjacent Channel Leakage Ratio
 
  @details
  This function gets the technology mode, bandwidth with its FFT spectrum and calculates the 1000*log10 of the ratio
  Stores the ACLR Ratio in a struct
  
  @param fft_size
  Input fft size
  
  @param mode
  Input enum of the specific Technology Mode
  
  @param bw
  Input enum of the specific LTE Bandwidth
  
  @param noise_data_to_subtract
  Input struct for noise energy from NV to subtract

  @param aclr_result
  Input struct to save the aclr ratio

  @param spec_inv
  Apply Spectral Inversion (1) or not (0)
  
  @return 
*/
 void ftm_common_get_aclr(
 uint16 fft_size,
 ftm_rfstate_enum_type mode,
 rfcom_lte_bw_type bw,
 ftm_common_power_per_bin_struct_type noise_data_to_subtract,
 ftm_common_aclr_result_struct_type* aclr_result,
 uint8 spec_inv);

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Get the Adjacent Channel Leakage Ratio for 1x
 
  @details
  This function gets CDMA band class and its FFT spectrum and calculates the 1000*log10 of the ratio
  Stores the ACLR Ratio in 1x-specific struct
  
  @param fft_size
  Input fft size
  
  @param cdma_band_class
  Input enum for CDMA band class
  
  @param aclr_result
  Input struct to save the aclr ratio
  
  @return 
*/
 void ftm_common_get_1x_aclr(
 uint16 fft_size,
 rfm_cdma_band_class_type cdma_band_class,
 ftm_common_1x_aclr_result_struct_type* aclr_result
 );

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Integrate the bins
 
  @details
  This function integrate the value of the given bin index
  Apply the rrc before integrating depending on given rrc_coeff_flag
  
  @param start_bin
  Input start bin index for integration
  
  @param end_bin
  Input end bin index for integration
  
  @param acc_num
  Input variable for accumulation
  
  @param rrc_coeff_flag
  Input flag for determine if multiply by rrc
  if rrc_coeff_flag == 1, apply the rrc
  
  @return value of integration
*/
int64 ftm_common_bins_integration(uint16 start_bin, uint16 end_bin, int64 acc_num, boolean apply_rrc_filter);

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Find peak of moving sum power in three channel window in CDMA adjacent and alternate neighbors
 
  @details
  This function finds the peak value of the moving sum power in three channel window
  between the given bin indexes 
  
  @param start_bin
  Input start bin index for moving sum
  
  @param end_bin
  Input end bin index for moving sum
  
  @return value of peak power
*/
int64 ftm_common_1x_find_peak_pwr_in_channel(uint16 start_bin, uint16 end_bin);


/*----------------------------------------------------------------------------*/
/*!
  @brief  
  For given ACLR bin struct, calculate and store binsum power
 
  @details
  This function finds the sum of power in each bin ACLR bin structure for current tech/bw
  and stores in a global structure
  
  @param fft_size,
  Size of FFT
  
  @param mode
  Mode to indicate tech
  
*/

void ftm_common_get_pwr_per_aclr_bin(
 uint16 fft_size,
 ftm_rfstate_enum_type mode
);

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  For given ACLR bin struct, return binsum power
 
  @details
  This function returns sum of power in each bin ACLR bin structure for current tech/bw
  by reading it from a global structure
  
  @param energy_per_bin,
  Pointer to data struct to populate
  
*/

void ftm_common_read_curr_pwr_per_aclr_bin(
   ftm_common_power_per_bin_struct_type * energy_per_bin
);

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  For Noise floor calibration, this function gets proper scaling value
 
  @details
  This function returns the number of bit shifts applied on FFT so that 
  Noise Energy per bin can be normalized to one FFT
  
  @return
  Bit shift applied on accumulated FFT energy per bin
  
*/

uint8 ftm_common_read_accum_bit_shift_num(void);
#endif /* FTM_COMMON_IQ_PROCESSING_H */
