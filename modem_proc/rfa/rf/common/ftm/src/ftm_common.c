/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         F T M  C O M M O N

GENERAL DESCRIPTION
  This is the FTM common file that contains common FTM variables and functions

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2006 - 2015 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_common.c#1 $
$DateTime: 2016/12/13 07:59:52 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/23/15   br      Changed LTE BW from 10MHz to 9MHz for sensitivity calculation
05/14/15    br     Added support for DL power level input by user
02/26/15   am      Fixed Compiler Warnings
02/04/15   sd      Added support for FTM WCDMA Sensitivity Estimation
01/14/15   bar     Removed unnecessary LTE header files
01/13/15   bar     Added embedded support for Sensitivity/CNR calculation
11/17/06   vm/jfc  Created for initial version of FTM for MSM7600

===========================================================================*/


/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/

#include "rfa_variation.h"
#include "customer.h"
#include "comdef.h"
#include "ftm_common_iq_processing.h"
#include "ftm_common.h"
#include <math.h>
#include "modem_mem.h"
#include "q6protos.h"
#include "ftm_task.h"
#include "rf_test_task_v.h"

#define PI acos(-1.0) //acos(-1.0) = 3.141592653589793...

float ftm_common_get_cnr_bw_limits(ftm_rf_technology_type tech);
float ftm_common_get_tech_Eb_No(ftm_rf_technology_type tech);
boolean ftm_common_setup_fft_data(uint16 fft_size);
boolean ftm_common_fft_buffer_setup(int32 **sample_buffer, uint16 fft_size);
void ftm_common_get_hanning_coeffs2(uint16 fft_size);
void ftm_common_get_sensitivity(int32 *sample_buffer, 
                                uint16 fft_size,
                                ftm_rf_technology_type tech,
                                uint32 samp_freq,
                                int16 dl_pwr_lvl);
void ftm_common_perform_windowing(int32* samples,
                                 uint16* window_coeffs,
                                  uint16 window_size);
void ftm_common_get_fft_pwr(uint16 fft_size, int32* samples_iq);
void ftm_common_convert_16sc_to_s16(uint16* buf_ptr_16sc, 
                                    int32* sample_buffer,
                                    uint16 fft_size);

void ftm_common_convert_wcdma_16sc_to_s32(uint16* buf_ptr_16sc, 
                                    int32* sample_buffer,
                                    uint16 fft_size);

void ftm_common_compute_cnr_sensitivity(float bw_limit_kHz, 
                                        float Eb_No, 
                                        uint16 fft_size,
                                        uint32 samp_freq,
                                        int16 dl_pwr_lvl);
void ftm_common_deinit_buffer_setup(void);
void ftm_common_compute_twiddle_factor(uint16 fft_size);
void ftm_common_calc_fft(uint16 fft_size, int32* data);

//const Eb/No for each tech
const float Eb_No_CDMA = -2.0;
const float Eb_No_WCDMA = -9.5;
const float Eb_No_GSM = 4.75;
const float Eb_No_LTE_Prx = -1.0;
const float Eb_No_LTE_Prx_Drx = -4.0;
const float Eb_No_TDSCDMA = -6.8;

//const channel limit bandwidth
const float limit_bw_kHz_CDMA = 615.0;
const float limit_bw_kHz_WCDMA = 1920.0;
const float limit_bw_kHz_GSM = 135.0;
const float limit_bw_kHz_LTE = 4500.0;
const float limit_bw_kHz_TDSCDMA = 640.0;

extern int32 cos_value_tbl[14];
extern int32 sin_value_tbl[14];

/*  For FFT calculation. CNR,Snesitivity        */
typedef struct{
  ftm_rfstate_enum_type mode;
  rfcom_lte_bw_type     bw;
  uint16* hanning_coeffs;
  int32*  twiddle_real;
  int32*  twiddle_imag;
  uint64*  samples_fft_pwr;
  uint16  accum_count;
  uint64  max_pwr_val;
  uint64  total_pwr;
  uint64  sig_pwr;
  uint16  max_pwr_indx;
}ftm_common_fft_data_struct_type;

/*Initializing the Global Struct Variable*/
static ftm_common_fft_data_struct_type ftm_common_fft_data_struct;

/*   Global struct to store the result              */
static ftm_common_sens_est_result_type ftm_common_sens_est_result;

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftm_common.h"
uint16 ftm_tx_rx_freq_cal_channels[FTM_TX_RX_FREQ_CAL_MAX_FREQS];
uint8  ftm_tx_rx_freq_cal_pa_ranges[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ];
uint16 ftm_tx_rx_freq_cal_pdm_list[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ];
uint8  ftm_tx_rx_freq_cal_hdet_read_list[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ];
uint8  ftm_tx_rx_freq_cal_rx_actions[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ];
int16  ftm_tx_rx_freq_cal_exp_agc[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ];
int16  ftm_tx_rx_freq_cal_rx_result[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ*FTM_TX_RX_FREQ_CAL_MAX_FREQS];
uint16 ftm_tx_rx_freq_cal_tx_result[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ*FTM_TX_RX_FREQ_CAL_MAX_FREQS];
uint8  ftm_tx_rx_freq_cal_rx_lpm_list[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ];
#endif /* FEATURE_FACTORY_TEST_MODE */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Main API that initalizes all data structs and caluclates Sensitivity and CNR 

  @param 	
  iq_buf_ptr pointer to buffer with I and Q samples each in 16SC format
 
  @param 	
  
 
  @details
  This function is called from common dispatch to calculate Sensitivity and CNR
  
  @return	
  
  
===============================================================================*/

void ftm_common_analyze_iq_samples(ftm_common_sens_est_result_type *sens_est_result_ptr, 
                                       uint16 *iq_buf_ptr,
                                       uint32 sample_size,
                                       uint32 samp_freq,
                                       ftm_rf_technology_type rf_tech,
                                       int16 dl_pwr_lvl)
{
   int32 *sample_buffer      = NULL;
   uint16 fft_size   = sample_size;
   
   boolean api_status = TRUE,
           iq_success = FALSE;
   
   ftm_rf_technology_type tech = rf_tech; 

   if ((iq_buf_ptr == NULL) || (sample_size == 0) || (samp_freq == 0) )
   {
      MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, "IQ buff pointer is NULL");
      iq_success = FALSE;
   }
   else
   {
      api_status = ftm_common_fft_buffer_setup(&sample_buffer, fft_size);

      if (sample_buffer == NULL )
      {
         MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, " sample_buffer is NULL ");
      }
      else
      {
         if (api_status)
         {
           switch (tech)
           {
             case FTM_RF_TECH_WCDMA:

			 /* convert SC8 to signed 32-bit for WCDMA           */
             ftm_common_convert_wcdma_16sc_to_s32((uint16*) iq_buf_ptr, sample_buffer,fft_size); 

             break;

			 case FTM_RF_TECH_LTE:
			 	
            /* convert SC16 to signed 16-bit                 */
             ftm_common_convert_16sc_to_s16((uint16*) iq_buf_ptr, sample_buffer,fft_size); 
			 break;

	         default:
	         MSG( MSG_SSID_RF, MSG_LEGACY_ERROR, "Incorrect Tech passed");
	         break;
           }
		   
 
            ftm_common_get_sensitivity(sample_buffer, 
                                       fft_size,
                                       tech,
                                       samp_freq,
                                       dl_pwr_lvl);

            sens_est_result_ptr->cnr_db100 = ftm_common_sens_est_result.cnr_db100;
            sens_est_result_ptr->sens_db100 = ftm_common_sens_est_result.sens_db100;
            sens_est_result_ptr->peak_freq = ftm_common_sens_est_result.peak_freq;
         }

         ftm_free(sample_buffer);
      }  

      ftm_common_deinit_buffer_setup();
   }
   
   MSG( MSG_SSID_RF, MSG_LEGACY_MED, " ftm_common_analyze_iq_samples is completed");
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Funtion to get tech specific BW to be used in Sensitivity calculation

  @param 	
  
 
  @param 	
  
 
  @details
  
  
  @return	
  
  
===============================================================================*/
float ftm_common_get_cnr_bw_limits(ftm_rf_technology_type tech)
{
   float limit_bw_kHz = 0.0;
   
   switch(tech)
   {
      case FTM_RF_TECH_CDMA:
         limit_bw_kHz = limit_bw_kHz_CDMA;
         break;
      case FTM_RF_TECH_WCDMA:
         limit_bw_kHz = limit_bw_kHz_WCDMA;
         break;
      case FTM_RF_TECH_GSM:
         limit_bw_kHz = limit_bw_kHz_GSM;
         break;
      case FTM_RF_TECH_LTE:
         limit_bw_kHz = limit_bw_kHz_LTE;
         break;
      case FTM_RF_TECH_TDSCDMA:
         limit_bw_kHz = limit_bw_kHz_TDSCDMA;
         break;
      default:
         limit_bw_kHz = limit_bw_kHz_LTE;
         break;
   }

   return limit_bw_kHz;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Funtion to get tech specific Eb_No to be used in Sensitivity calculation

  @param 	
  
 
  @param 	
  
 
  @details
  
  
  @return	
  
  
===============================================================================*/
float ftm_common_get_tech_Eb_No(ftm_rf_technology_type tech)
{
   float eb_no_dB = 0.0;

   switch(tech)
   {
      case FTM_RF_TECH_CDMA:
         eb_no_dB = Eb_No_CDMA;
         break;
      case FTM_RF_TECH_WCDMA:
         eb_no_dB = Eb_No_WCDMA;
         break;
      case FTM_RF_TECH_GSM:
         eb_no_dB = Eb_No_GSM;
         break;
      case FTM_RF_TECH_LTE:
         eb_no_dB = Eb_No_LTE_Prx;
         break;
      case FTM_RF_TECH_TDSCDMA:
         eb_no_dB = Eb_No_TDSCDMA;
         break;
      default:
         eb_no_dB = Eb_No_LTE_Prx;
         break;
   }
   return eb_no_dB;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Funtion to initalize output sample buffer which stores signed data and to
  initialize all other required data

  @param 	
  
 
  @param 	
  
 
  @details
  
  
  @return	
  
  
===============================================================================*/
boolean ftm_common_fft_buffer_setup(int32 **sample_buffer, uint16 fft_size)
{
   boolean api_status = TRUE;
   
   *sample_buffer = (int32*)ftm_malloc(fft_size * 2 * 4);

   /* If pointer is NULL even after allocating memory, then declare it as failure*/
   if(*sample_buffer == NULL)
   {
     MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "Sample buffer ptr is NULL");
     return api_status = FALSE;
   }
   else
   {
      /* init FFT struct - required before calling FFT API */
      api_status = ftm_common_setup_fft_data(fft_size); 
   }
   return api_status; 
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initalize all data required to perform FFT

  @param 	
  
 
  @param 	
  
 
  @details
  
  
  @return	
  
  
===============================================================================*/
boolean ftm_common_setup_fft_data(uint16 fft_size)
{
   boolean api_status = TRUE;
   ftm_common_fft_data_struct.accum_count = 0;

   /* ************  Null pointer check before allocating space **********  */
  if (ftm_common_fft_data_struct.hanning_coeffs == NULL) 
  {
    ftm_common_fft_data_struct.hanning_coeffs = (uint16*)ftm_malloc(fft_size*sizeof(uint16));

	 /*Null pointer check after allocating space*/
    if (ftm_common_fft_data_struct.hanning_coeffs == NULL)
    {
       MSG_ERROR("LTE IQ capture Hanning coefficient pointer is NULL", 0, 0, 0);
       return api_status = FALSE; 
    }
    
    /*Get the Hanning Coefficients*/
    //rflte_msm_iq_capture_get_hanning_coeffs();
    ftm_common_get_hanning_coeffs2(fft_size);
  }

   /* ******* Null pointer check before allocating space ************ */
  if ((ftm_common_fft_data_struct.twiddle_real == NULL) && 
      (ftm_common_fft_data_struct.twiddle_imag == NULL))
  {
    ftm_common_fft_data_struct.twiddle_real = (int32*)ftm_malloc((fft_size-1)*sizeof(int32));
    if (ftm_common_fft_data_struct.twiddle_real == NULL)
    {
      MSG_ERROR("LTE IQ capture Twiddle real pointer is NULL", 0, 0, 0);
      return api_status = FALSE;
    }

    ftm_common_fft_data_struct.twiddle_imag = (int32 *)ftm_malloc((fft_size-1)*sizeof(int32)); 
    if (ftm_common_fft_data_struct.twiddle_imag == NULL)
    {
      MSG_ERROR("LTE IQ capture Twiddle imag pointer is NULL", 0, 0, 0);
      return api_status = FALSE;
    }
	/*Get the Twiddle Factor Coefficients*/
    ftm_common_compute_twiddle_factor(fft_size); 
  }

  /* ********* Null pointer check before allocating space ***************** */
  if (ftm_common_fft_data_struct.samples_fft_pwr == NULL)
  {
    ftm_common_fft_data_struct.samples_fft_pwr = (uint64*)ftm_malloc(fft_size*sizeof(uint64));
    if (ftm_common_fft_data_struct.samples_fft_pwr != NULL)
    {
       memset(ftm_common_fft_data_struct.samples_fft_pwr, 0, fft_size*sizeof(uint64)); 
    }
    else
    {
       MSG_ERROR("samples_fft_pwr pointer is NULL", 0, 0, 0);
       return api_status = FALSE;
    }
  }

  ftm_common_fft_data_struct.total_pwr = 0;
  ftm_common_fft_data_struct.max_pwr_val = 0;
  ftm_common_fft_data_struct.sig_pwr = 0;

  return api_status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get hanning window coefficients 

  @param 	
  
 
  @param 
  
 
  @details
  
  
  @return	
  
  
===============================================================================*/
void ftm_common_get_hanning_coeffs2(uint16 fft_size)
{
   uint16 coeff16_bit, indx;
   uint16 scale_factor = 32767;  // 2^15-1
   float hannCoeff;

   for( indx = 0; indx < fft_size; indx++)
   {
      hannCoeff = 0.5 - 0.5 * cos( (float)( (2.0 * PI * indx) / ( fft_size * 1.0 ) ) );//result in radians
      coeff16_bit = (uint16)(hannCoeff*scale_factor);
      ftm_common_fft_data_struct.hanning_coeffs[indx] = (uint16)(coeff16_bit);
          
   }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Wrapper funtion to compute sensitivity and to do all required processing

  @param 	
  
 
  @param sample_buffer	
  
 
  @details
  
  
  @return	
  
  
===============================================================================*/
void ftm_common_get_sensitivity(int32 *sample_buffer, 
                                uint16 fft_size,
                                ftm_rf_technology_type tech,
                                uint32 samp_freq,
                                int16 dl_pwr_lvl)
{
   float bw_limit_kHz = 0.0,
         Eb_No = 0.0;
   
   if (sample_buffer != NULL)
   {
      /*Apply hanning window*/
      ftm_common_perform_windowing(sample_buffer, 
                                    ftm_common_fft_data_struct.hanning_coeffs, 
                                    fft_size);
    
      /*compute fft*/
      ftm_common_calc_fft(fft_size, sample_buffer);
      
      /* Compute FFT pwr (I^2 + Q^2)*/
      ftm_common_get_fft_pwr(fft_size, sample_buffer);
      
      /* **************************************************************   */
      bw_limit_kHz = ftm_common_get_cnr_bw_limits(tech);
      Eb_No = ftm_common_get_tech_Eb_No(tech);
      ftm_common_compute_cnr_sensitivity(bw_limit_kHz, Eb_No, fft_size, samp_freq, dl_pwr_lvl);
      /* **************************************************************   */
   }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Convert SC16 to signed 16-bit data to be used for FFT computation

  @param buf_ptr_16sc	
  ptr to SC16 bit data
 
  @param sample_buffer	
  ptr to output buffer to store signed 16 bit data
 
  @details
  
  
  @return	
  
  
===============================================================================*/
void ftm_common_convert_16sc_to_s16(uint16* buf_ptr_16sc, 
                                    int32* sample_buffer,
                                    uint16 fft_size)
{
   int16 nsamples = 0;
   int32 I_sample, I_sample_temp;
   int32 Q_sample, Q_sample_temp;
   int32 ICount=0, QCount=1;  //I's located in even, Q's located in odds
   uint16 indx = 0;
   
   for (nsamples = 0; nsamples < fft_size; nsamples++)
   {
      sample_buffer[ICount] = 0;
      I_sample = buf_ptr_16sc[ICount];
      I_sample = (I_sample ^ 0x8000);
      I_sample_temp = (int32) (I_sample - 32767);
      sample_buffer[ICount] = (int32) I_sample_temp;

      sample_buffer[QCount] = 0;
      Q_sample = buf_ptr_16sc[QCount];
      Q_sample = (Q_sample ^ 0x8000);
      Q_sample_temp = (int32) (Q_sample - 32767);
      sample_buffer[QCount] = (int32) Q_sample_temp;

      ICount += 2;
      QCount += 2;
   }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Convert SC16 to signed 32-bit data to be used for FFT computation

  @param buf_ptr_16sc	
  ptr to SC16 bit data
 
  @param sample_buffer	
  ptr to output buffer to store signed 16 bit data
 
  @details
  This function converts WCDMA 16SC (I 8bit +Q 8bit) samples to signed 32 bit (I 32 bit + Q 32 bit) samples.
  
  @return	
  
  
===============================================================================*/
void ftm_common_convert_wcdma_16sc_to_s32(uint16* buf_ptr_16sc, 
                                    int32* sample_buffer,
                                    uint16 fft_size)
{
   int sample_count;
   uint8 * iq_buf_temp; 

   int16 nsamples = 0;
   int32 I_sample, I_sample_temp;
   int32 Q_sample, Q_sample_temp;
   int32 ICount=0, QCount=1;  //I's located in even, Q's located in odds
   uint16 indx = 0;
    
   /*Allocate memory to iq_buf_temp, 2 * sample size*/

   iq_buf_temp = (uint8*)ftm_malloc(fft_size * 2 * sizeof(uint8));

   if(iq_buf_temp == NULL)
   {
     FTM_MSG_ERROR("WCDMA IQ Capture: Unable to allocate memory for iq_buf_temp",0,0,0);
	 return;
   }

   /* Separate the 8bit I and Q samples */
  for(sample_count = 0; sample_count < fft_size; sample_count++)
  {
     iq_buf_temp[2*sample_count]= (uint8)((buf_ptr_16sc[sample_count] & 0xFF00) >> 8);
	 iq_buf_temp[2*sample_count + 1]= (uint8)(buf_ptr_16sc[sample_count] & 0x00FF);
  }
  		   
    /* Convert Samples to signed 32 bit*/
   for (nsamples = 0; nsamples < fft_size; nsamples++)
   {
      sample_buffer[ICount] = 0;
      I_sample = iq_buf_temp[ICount];
      I_sample = (I_sample ^ 0x80);
      I_sample_temp = (int32) (I_sample - 127);
      sample_buffer[ICount] = (int32) I_sample_temp;

      sample_buffer[QCount] = 0;
      Q_sample = iq_buf_temp[QCount];
      Q_sample = (Q_sample ^ 0x80);
      Q_sample_temp = (int32) (Q_sample - 127);
      sample_buffer[QCount] = (int32) Q_sample_temp;

      ICount += 2;
      QCount += 2;
   }
MSG( MSG_SSID_RF, MSG_LEGACY_MED, " ftm_common: convert WCDMA IQ samples successful ");
}



/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Apply the window coefficients to the input samples
 
  @details
  Apply the window coefficients 

  @param samples
  Input samples of size window_size 
  
  @param window_coeffs
  Input window coefficients in uint16 format of size window_size
  
  @param window_size
  Input window coefficients size

  @return 
*/
void ftm_common_perform_windowing(int32* samples,
                                  uint16* window_coeffs,
                                  uint16 window_size)
{
  uint16 index, sample_index;
  uint32 hann_coeff_scale = 16;
  
  for (index = 0; index < window_size; index++)
  {
    sample_index = index << 1;
    samples[sample_index]   = (int32)(Q6_P_asr_PI(Q6_P_mpy_RR(samples[sample_index], 
                                                      (int32)window_coeffs[index]), hann_coeff_scale));
    samples[sample_index+1] = (int32)(Q6_P_asr_PI(Q6_P_mpy_RR(samples[sample_index+1], 
                                                      (int32)window_coeffs[index]), hann_coeff_scale));
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Compute power by I^2 + Q^2
  
 
  @details
  

  @param 
  
  
  @param 
  

  @return 
*/
void ftm_common_get_fft_pwr(uint16 fft_size, int32* samples_iq)
{
  /* FFT data is 30 bit, so after sqaure, sum and accumulation, it may overflow 64 bit (2*30 + 1 + 4 = 65)*/
  /*shift accumalted result by 2 bits to keep it within 64 bits*/   
  uint8 accumlated_bit_shift_num = 2;
  uint16 fft_index;
  
  ftm_common_fft_data_struct.max_pwr_val = 0;
  ftm_common_fft_data_struct.max_pwr_indx = 0;
  ftm_common_fft_data_struct.total_pwr = 0;
  
  for (fft_index = 0; fft_index < fft_size; fft_index++)
  {
    /* accumlate the I^2 + Q^2*/
    ftm_common_fft_data_struct.samples_fft_pwr[fft_index] = Q6_P_mpyacc_RR(Q6_P_mpy_RR(samples_iq[fft_index<<1], samples_iq[fft_index<<1]),
                                                 samples_iq[(fft_index<<1)+1],
                                                 samples_iq[(fft_index<<1)+1]) >> accumlated_bit_shift_num;   

    /*  DC pwr lies in first FFT sample            */
    if ((fft_index > 0) && (fft_index < fft_size))
    {
       // Total power without DC (witout BW limitation)
       ftm_common_fft_data_struct.total_pwr += ftm_common_fft_data_struct.samples_fft_pwr[fft_index];

       // Carrier Peak search
       if (ftm_common_fft_data_struct.samples_fft_pwr[fft_index] > ftm_common_fft_data_struct.max_pwr_val)
       {
          ftm_common_fft_data_struct.max_pwr_val = ftm_common_fft_data_struct.samples_fft_pwr[fft_index];
          ftm_common_fft_data_struct.max_pwr_indx = fft_index;
       }
    }
  }
  
  /* MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH,"FFT_Pwr Max_value = %d   Max_indx = %d  ", 
        ftm_common_fft_data_struct.max_pwr_val, 
        ftm_common_fft_data_struct.max_pwr_indx); */
}

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Compute CNR and sensitivity
 
  @details
  

  @param 
  
  
  @param 
  

  @return 
*/
void ftm_common_compute_cnr_sensitivity(float bw_limit_kHz, 
                                        float Eb_No, 
                                        uint16 fft_size,
                                        uint32 samp_freq, 
                                        int16 dl_pwr_lvl)
{
   uint32   idxLeft = 0, 
            idxRight = 0, 
            i = 0, 
            k = 0;
   uint32   left_pwr_indx = 0, 
            right_pwr_indx = 0, 
            sig_indx = 0;
   int32  peak_freq = 0;
   int32  nyquist_freq = 0;
   uint16 max_pwr_indx = ftm_common_fft_data_struct.max_pwr_indx; 
   uint64 noise_pwr_out_band = 0;
   uint64 noise_pwr_in_band = 0;
   uint64 signal_pwr_in_band = 0;

   float cnr = 0.0, 
         cnr_db = 0.0, 
         total_pwr_db = 0.0,
         delta_f = 0.0, 
         sig_pwr_db = 0.0,
         sensitivity_db = 0.0,
         Si_pwr = -85.0;

   if((dl_pwr_lvl < -300) && (dl_pwr_lvl > -1150))
   {
      Si_pwr = (float) dl_pwr_lvl/10;
   }

   MSG_2(MSG_SSID_RF, MSG_LEGACY_MED,"DL power level from user is = %d and DL pwr used is = %d", dl_pwr_lvl, (int16)Si_pwr);

   if (fft_size > 0)
   {
      delta_f = samp_freq/fft_size;
      peak_freq = (int32)(max_pwr_indx*delta_f);
      nyquist_freq = samp_freq/2;

      if (peak_freq > nyquist_freq)
      {
         peak_freq = samp_freq - peak_freq;
      }
      ftm_common_sens_est_result.peak_freq = peak_freq;
   }
   else
   {
      MSG_ERROR("FFT size is szero",0,0,0);
   }

   idxLeft = (uint32)(bw_limit_kHz*1000*fft_size/samp_freq);
   idxRight = fft_size - idxLeft ;

   /* MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH,"Left_indx = %u,   Right_indx = %u ", 
        idxLeft, idxRight); */

   /*  add conditon on indices for exceeding the BW/Max signal indx        */
   for(i = idxLeft; i < idxRight ; i++)
   {
      noise_pwr_out_band += ftm_common_fft_data_struct.samples_fft_pwr[i] ;
   }

   // compute indixes for which pwr > 0.1% of the carrier power
   for (k = 1; k < 20 ; k++)
   {
      if (((max_pwr_indx - k) > 0) && ((max_pwr_indx + k) < fft_size))
      {
         if (ftm_common_fft_data_struct.samples_fft_pwr[max_pwr_indx + k] > ( (uint32) ftm_common_fft_data_struct.max_pwr_val*0.0005)) 
         {
            right_pwr_indx++;
         }

         if (ftm_common_fft_data_struct.samples_fft_pwr[max_pwr_indx - k] > ( (uint32) ftm_common_fft_data_struct.max_pwr_val*0.0005))
         {
            left_pwr_indx++;
         }
      }
   }

   sig_indx = max_pwr_indx - left_pwr_indx;

   // compute signal power using indices found above
   for (; sig_indx <= (right_pwr_indx + max_pwr_indx); sig_indx++)
   {
      ftm_common_fft_data_struct.sig_pwr += ftm_common_fft_data_struct.samples_fft_pwr[sig_indx];
   }

   // Compute inband noise
   noise_pwr_in_band = ftm_common_fft_data_struct.total_pwr - ftm_common_fft_data_struct.sig_pwr - noise_pwr_out_band;
   
   if (noise_pwr_in_band != 0)
   {
      cnr = (float) ftm_common_fft_data_struct.sig_pwr/noise_pwr_in_band;
   }
   else
   {
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"noise_pwr_in_band is = ZERO");
   }

   /* calculate pwr's in db scale */
   if (cnr != 0.0 )
   {
      cnr_db = 10*log10(cnr);

      /* Sensitivity (Si,Eb/No Tech,B)dBm = Si(dBm) + Eb/No<Tech(dB)> - C/N(dB)   */
      sensitivity_db = Si_pwr + Eb_No - cnr_db;

      ftm_common_sens_est_result.cnr_db100 = (int32)(cnr_db*100);
      ftm_common_sens_est_result.sens_db100 = (int32)(sensitivity_db*100);
   }
   else
   {
      cnr_db = -999.0;
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"CNR cannot be calculated", 0);
   }
}
/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Free memory if allocated during sensitivity calculation
 
  @details
  
  @param 
  
  @param 
  
  @return 
*/

void ftm_common_deinit_buffer_setup(void)
{

  if (ftm_common_fft_data_struct.hanning_coeffs != NULL)
  {
    ftm_free(ftm_common_fft_data_struct.hanning_coeffs);
    ftm_common_fft_data_struct.hanning_coeffs = NULL;
  }

  if (ftm_common_fft_data_struct.twiddle_real != NULL)
  {
    ftm_free(ftm_common_fft_data_struct.twiddle_real);
	ftm_common_fft_data_struct.twiddle_real = NULL;
  }

  if (ftm_common_fft_data_struct.twiddle_imag != NULL)
  {
    ftm_free(ftm_common_fft_data_struct.twiddle_imag);
	ftm_common_fft_data_struct.twiddle_imag = NULL;
  }
  
 if (ftm_common_fft_data_struct.samples_fft_pwr != NULL)
  {
    ftm_free(ftm_common_fft_data_struct.samples_fft_pwr);
    ftm_common_fft_data_struct.samples_fft_pwr = NULL;
  }

  MSG(MSG_SSID_RF, MSG_LEGACY_MED, "ftm_common_deinit_buffer_setup all varaibles deinitialized ");
  
}

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Calculate the twiddle factor for fft
 
  @details
  Calculate the twiddle factor for 1024 or smaller Fast Fourier transform using hard-coded sine cosine tables

  @param fft_size
  Input fft size
  
  @return 
*/
void ftm_common_compute_twiddle_factor(uint16 fft_size)
{

  uint16 iq_size, sub_fft_size, sub_fft_index, sin_cos_index, istep, twiddle_index;
  int32 temp_twiddle_real, temp_twiddle_imag, temp_twiddle_fac;
  uint32 twiddle_scale_factor = 30;

  iq_size = fft_size << 1;
  sub_fft_size = 2;
  sin_cos_index = 0;
  twiddle_index = 0;

  while (iq_size > sub_fft_size) 
  {
    istep = sub_fft_size << 1;
    temp_twiddle_real = - cos_value_tbl[0];
    temp_twiddle_imag = 0;
    ftm_common_fft_data_struct.twiddle_real[twiddle_index] = temp_twiddle_real;
    ftm_common_fft_data_struct.twiddle_imag[twiddle_index] = temp_twiddle_imag;
    twiddle_index++;

    for (sub_fft_index = 3; sub_fft_index < sub_fft_size; sub_fft_index += 2)       
    {
      temp_twiddle_fac = temp_twiddle_real;

      temp_twiddle_real = (int32)Q6_P_asr_PI(Q6_P_mpynac_RR(Q6_P_mpy_RR(temp_twiddle_real, cos_value_tbl[sin_cos_index]), 
                                                            temp_twiddle_imag, sin_value_tbl[sin_cos_index]), twiddle_scale_factor);
      temp_twiddle_imag = (int32)Q6_P_asr_PI(Q6_P_mpyacc_RR(Q6_P_mpy_RR(temp_twiddle_imag, cos_value_tbl[sin_cos_index]), 
                                                            temp_twiddle_fac,  sin_value_tbl[sin_cos_index]), twiddle_scale_factor);

      ftm_common_fft_data_struct.twiddle_real[twiddle_index] = temp_twiddle_real;
      ftm_common_fft_data_struct.twiddle_imag[twiddle_index] = temp_twiddle_imag;
      twiddle_index++;
    }
    sin_cos_index++;
    sub_fft_size = istep;
  }
}

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
void ftm_common_calc_fft(uint16 fft_size, int32* data)
{
  uint16 iq_size, sub_fft_index, sub_fft_size, twiddle_index, i, j, istep;
  int32 temp_data;
  int64 temp_real, temp_imag;
  uint32 twiddle_scale_factor = 30;

  iq_size = fft_size << 1;
  j = 1;

  for (i = 1; i < iq_size; i += 2) 
  {
    if (j > i) 
    {
      temp_data = data[j-1]; 
      data[j-1] = data[i-1]; 
      data[i-1] = temp_data;  
      temp_data = data[j]; 
      data[j] = data[i]; 
      data[i] = temp_data;          
    }
    sub_fft_index = fft_size;
    while (sub_fft_index >= 2 && j > sub_fft_index) 
    {
      j -= sub_fft_index;
      sub_fft_index >>= 1;
    }
    j += sub_fft_index;
  }

  sub_fft_size = 2;
  twiddle_index = 0;

  while (iq_size > sub_fft_size) 
  {
    istep = sub_fft_size << 1;

    for (sub_fft_index = 1; sub_fft_index < sub_fft_size; sub_fft_index += 2)       
    {
      for (i = sub_fft_index; i <= iq_size; i += istep)  
      {
        j = i + sub_fft_size;   

        temp_real = Q6_P_mpynac_RR(Q6_P_mpy_RR(data[j-1], ftm_common_fft_data_struct.twiddle_real[twiddle_index]),
                                               data[j],   ftm_common_fft_data_struct.twiddle_imag[twiddle_index]);
        temp_imag = Q6_P_mpyacc_RR(Q6_P_mpy_RR(data[j],   ftm_common_fft_data_struct.twiddle_real[twiddle_index]), 
                                               data[j-1], ftm_common_fft_data_struct.twiddle_imag[twiddle_index]);

        data[j-1] = (int32)Q6_P_asr_PI(Q6_P_sub_PP(Q6_P_asl_PI((int64)data[i-1], twiddle_scale_factor), temp_real), twiddle_scale_factor);
        data[j]   = (int32)Q6_P_asr_PI(Q6_P_sub_PP(Q6_P_asl_PI((int64)data[i], twiddle_scale_factor), temp_imag), twiddle_scale_factor);

        data[i-1] = (int32)Q6_P_asr_PI(Q6_P_add_PP(Q6_P_asl_PI((int64)data[i-1], twiddle_scale_factor), temp_real), twiddle_scale_factor);
        data[i]   = (int32)Q6_P_asr_PI(Q6_P_add_PP(Q6_P_asl_PI((int64)data[i], twiddle_scale_factor), temp_imag), twiddle_scale_factor);        
      }
      twiddle_index++;
    }
    sub_fft_size = istep;
  }
}
