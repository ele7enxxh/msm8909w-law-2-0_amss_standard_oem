/*!
  @file
  ftm_enh_internal_device_calibration_droop.c

  @brief

*/
/*==============================================================================

  Copyright (c) 2013 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_enh_internal_device_calibration_droop.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/10/16   rs      Compiler warning fix for C2K disabled builds
02/10/15    br     Send tech,band,bandwidth and fbrx gain state as inputs to stg enable and disable apis
11/17/14    vb     LTE band and bandwidth specific Droop cal support
10/08/14    ka     Replace rfc with device api for retrieving device pointer
09/09/14   jmf     Fix Compiler warnings
08/30/14   jmf     Add STG tone power return from FBRx Droop Cal FTM API
08/20/14   jmf     Fix KW warnings
08/20/14   jmf     Use TDS/1x/W Static Filter for convolution instead of default LTE5
08/20/14   jmf     Remove hard-coded samp_rate for C2k/TDS
08/18/14   jmf    New functions to compute Complex FBRx Droop Filter
05/29/14   brath   Updates to provide STG frequency for device cal APIs
04/21/14   jmf     Updates to Enable All Tech-BW combinations to go through
04/14/14   jmf     Updates to Coeff Generating Matrix
04/08/14   jmf     Updates to accommodate error code returns / Droop Filter Processing
03/14/14   jmf     Updates to account for NV structure changes
03/07/14    aa     Latest updates for droop cal
02/06/14   jmf     STG-based Droop Cal capture sequence implementation, no processing yet
02/05/14    aa     Updated params for droop cal data
01/21/14   jmf     Add support to update global NV structure for Droop Filter
12/15/13   jmf     Initial Revision

==============================================================================*/
#ifndef FEATURE_GNSS_ONLY_NO_WWAN

#include "rfa_variation.h"
#include "comdef.h"
#include "ftm_common_timer.h"
#include "ftm_common_control.h"
#include "ftm_common_data.h"
#include "rfcommon_math.h"
#include "modem_mem.h"
#include "string.h"
#include "stringl.h" /* for memscpy */
#include "ftm_msg.h"
#include "ftm_common_enh_internal_device_cal.h"
//#include "rfm_internal.h"
//#include "DALSys.h" 
#include "rfcommon_time_profile.h" 
//#include "zlib.h"
#include "ftm_enh_internal_device_cal_droop.h"
#include "rfdevice_cmn_intf.h"
#include "rfcommon_core_xpt_buffer.h"
#include "rflm_api_fbrx.h"
#include "math.h"
#include "rfcommon_core_utils.h"
#include "rfcommon_mc.h"

#ifdef FEATURE_LTE
#include "rflte_util.h"
#include "rflte_mc.h"
#include "rflte_ftm_mc.h"
#endif

#ifdef FEATURE_WCDMA
#include "rfwcdma_mc.h"
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_CDMA1X
#include "rf_cdma_data.h"
#include "rf_cdma_utils_freq.h"
#endif

#ifdef FEATURE_TDSCDMA
#include "rf_tdscdma_core_util.h"
#include "rf_tdscdma_mc.h"
#endif

#ifdef FEATURE_WCDMA
extern rfwcdma_mc_state_type rfwcdma_mc_state;
#endif

#ifdef FEATURE_TDSCDMA
extern rf_tdscdma_mc_state_type rf_tdscdma_mc_state;
#endif

extern uint8 rfcommon_fbrx_msm_convert_tx_cfg_to_mode( rfcommon_fbrx_tx_cfg_type cfg);

/*******************************************************/
/*       HARD-CODED PARAMS FOR PROCESSING ALG          */
/*    Ref:  FBRxDroopEqualization_V2_20140403.m        */
/*                                                     */
/*******************************************************/
#define KERNEL_FRAC_RES 14
#define PI 3.14159265358979323846
#define EQ_LEN 10
#define ENABLE_DEBUG_KERNEL_DUMPS 0
#define ENABLE_DEBUG_MULTRES_DUMPS 0
/*******************************************************/
/*       MAGIC  MATRICES FOR COEFF CALCULATION         */
/*    Ref:  FBRxDroopEqualization_V2_20140403.m        */
/*                                                     */
/*******************************************************/

// With assumption that captures are done at freq0 = 0.5M, freq1 = 2M, freq2 = 4.5M
// And WTR poles are located at [30,30] MHz
/* // First Version
static int16 M_LTE5[3][EQ_LEN] = { {1433,  3145,   -14077,  571,   27422,   28896,   1990,  -17330,  -3188,   3907 }, 
    {784,      -10904,       16821,       14378,      -11665,      -22762,       -4353,       15775,        7158,       -5233 },
    {-1002  ,      4101,       -2018  ,     -4389  ,      -367,        3971 ,       2926,       -1623,       -2967,        1366 }
};
uint8 fracBits_LTE5  = 15;*/

static int16 M_LTE5[3][EQ_LEN] = { {   179,   393, -1760,    71,  3428,  3612,   249, -2166,  -399,   488}, 
                                   {    98, -1363,  2103,  1798, -1458, -2845,  -544,  1972,   895,  -654},
                                   {  -125,   513,  -252,  -549,   -46,   496,   365,  -203,  -370,  171 }
                                 };
uint8 fracBits_LTE5  = 12;

// With assumption that captures are done at freq0 = 0.5M, freq1 = 2M, freq2 = 4.5M
// And WTR poles are located at [30,30] MHz
/* // First Version
static int16 M_LTE10[3][EQ_LEN] = {
    {87,         220,       -2270,        7463,      -12425,        6976,       14081,      -13572,        4283,        -748},
    {46,        -987,        4558,      -12022,       16923,       -2323,      -18836,       16897,       -5091,         837},
    {-101,         615,       -1911,        3834,       -3425,       -1601,        5377,       -3571,         898,        -117}
        };
uint8 fracBits_LTE10 = 12; */

static int16 M_LTE10[3][EQ_LEN] = { {    87,   220, -2270,  7463,-12425,  6976, 14081,-13572,  4283,  -748 },
                                    {    46,  -987,  4558,-12022, 16923, -2323,-18836, 16897, -5091,   837 },
                                    {  -101,   615, -1911,  3834, -3425, -1601,  5377, -3571,   898,  -117 }
                                  };
uint8 fracBits_LTE10 = 12;

// With assumption that captures are done at freq0 = 0.5M, freq1 = 4M, freq2 = 9M
// And WTR poles are located at [30,30] MHz
/* // First Version
static int16 M_LTE15[3][EQ_LEN] = {
    {2224,       -2330,      -10679,       19899,       22853,       -8558,      -14051,        8070,          20,       -1064},
    {-1505,       -2968,       20846,       -9214,      -22807,        6189,       16447,       -6711,       -2058,        1779},
    {-381 ,       3224   ,    -4989  ,     -1057  ,      4742    ,    1088  ,     -3321  ,      -126     ,   1420   ,     -597}
        };
uint8 fracBits_LTE15 = 14; */

static int16 M_LTE15[3][EQ_LEN] = { {   556,  -582, -2670,  4975,  5713, -2139, -3513,  2018,     5,  -266 },
                                    {  -376,  -742,  5212, -2303, -5702,  1547,  4112, -1678,  -514,   445 },
                                    {   -95,   806, -1247,  -265,  1186,   272,  -830,   -32,   355,  -149 }};
uint8 fracBits_LTE15  = 12;

// With assumption that captures are done at freq0 = 0.5M, freq1 = 4M, freq2 = 9M
// And WTR poles are located at [30,30] MHz
/* // First Version
static int16 M_LTE20[3][EQ_LEN] = {
    {451     ,  -1034    ,    -350  ,      7217   ,   -19182  ,     18915   ,    17137  ,    -20970  ,      7420   ,    -1411},
    {-276    ,     -21   ,    3600  ,    -13950   ,    26615  ,    -11381   ,   -23326  ,     25924  ,     -8764   ,     1579},
    {-116    ,     772   ,    -2545 ,       5344  ,     -5292 ,      -1517  ,      7529 ,      -5474 ,       1518  ,      -221}
        };
uint8 fracBits_LTE20 = 13; */

static int16 M_LTE20[3][EQ_LEN] = { {   225,  -517,  -175,  3608, -9591,  9457,  8569,-10485,  3710,  -706 },
                                    {  -138,   -11,  1800, -6975, 13307, -5691,-11663, 12962, -4382,   789 },
                                    {   -58,   386, -1272,  2672, -2646,  -758,  3764, -2737,   759,  -110 }
                                  };
uint8 fracBits_LTE20  = 12;

uint8 num_droop_filters = -1;



#define CPLX_NCALEQ 3  // Supplementary equalizer length for Complex Droop Calibration

// LTE 20 B matrix - each row correspond to each supplemental filter tap, each column correspond to number of STG frequencies
static int16 B_LTE20[CPLX_NCALEQ][8] = {
                                  {-1018,         534,        3088,        5588,       -1018,         534,        3088,        5588},
                                  {6134,         3030,       -2081,       -7083,        6134,        3030,       -2081,       -7083},
                                  {319,          2338,        3305,        3494,        -319,       -2338,       -3305,       -3494}
                             };
// LTE 20 STG Freqs to perform captures for Complex Droop Filter
static int32 FKHZ_LTE20[8] = {500, 4000, 6750, 9000, -500, -4000, -6750, -9000};
// LTE 20 Number of captures for Complex Droop Filter
static uint8 CAPS_LTE20    = 8;

// LTE 15 B matrix - each row correspond to each supplemental filter tap, each column correspond to number of STG frequencies
static int16 B_LTE15[CPLX_NCALEQ][6] = {
                                 {-4941,        1371,       11762,       -4941,        1371,       11762},
                                 {11107,        1968,      -13075,       11107,        1968,      -13075},
                                 {  556,        4068,        5752,        -556,       -4068,       -5752}
                             };
// LTE 15 STG Freqs to perform captures for Complex Droop Filter
static int32 FKHZ_LTE15[6] = {500, 4000, 6750, -500, -4000, -6750};
// LTE 15 Number of captures for Complex Droop Filter
static uint8 CAPS_LTE15    = 6;

// LTE 10 Nominal Real Droop Filter - same as Static NV
static int16 B_LTE10[CPLX_NCALEQ][6] = {
                                { -595,        1626,        7160,        -595,        1626,        7160},
                                { 6269,        2082,       -8351,        6269,        2082,       -8351},
                                {  951,        3836,        5223,        -951,       -3836,       -5223}
                             };
// LTE 10 STG Freqs to perform captures for Complex Droop Filter
static int32 FKHZ_LTE10[6] = {500, 2250, 4500, -500, -2250, -4500};
// LTE 10 Number of captures for Complex Droop Filter
static uint8 CAPS_LTE10    = 6;

// LTE 5 Nominal Real Droop Filter - same as Static NV
static int16 B_LTE5[CPLX_NCALEQ][4] = {
                               {-18434,       26626,      -18434,       26626},
                               { 27076,      -27076,       27076,      -27076},
                               {  2613,       10536,       -2613,      -10536}
                             };
// LTE 5 STG Freqs to perform captures for Complex Droop Filter
static int32 FKHZ_LTE5[4] = {500, 2250, -500, -2250};
// LTE 5 Number of captures for Complex Droop Filter
static uint8 CAPS_LTE5    = 4;


// LTE 20 Nominal Real Droop Filter coeffs - same as Static NV
static int32 filter_LTE20[8] = {-1373,	18383,	1058,	-3232,	2589,	-1471,	533,	-103};
// LTE 20 Nominal Real Droop Filter length - same as Static NV
static uint8 filterlen_LTE20 = 8;
// LTE 20 Nominal Real Droop Filter group delay - same as Static NV
static uint8 filter_grp_delay_LTE20  = 1;


// LTE 15 Nominal Real Droop Filter coeffs - same as Static NV
static int32 filter_LTE15[8] = {-348,	1753,	-4371,	5127,	12478,	3656,	-2418,	506};
// LTE 15 Nominal Real Droop Filter length - same as Static NV
static uint8 filterlen_LTE15 = 8;
// LTE 15 Nominal Real Droop Filter group delay - same as Static NV
static uint8 filter_grp_delay_LTE15  = 4;

// LTE 10 Nominal Real Droop Filter coeffs - same as Static NV
static int32 filter_LTE10[8] = {-145,	-755,	17594,	1128,	-2663,	1938,	-943,	231};
// LTE 10 Nominal Real Droop Filter length - same as Static NV
static uint8 filterlen_LTE10 = 8;
// LTE 10 Nominal Real Droop Filter group delay - same as Static NV
static uint8 filter_grp_delay_LTE10  = 2;

// LTE 5 Nominal Real Droop Filter coeffs - same as Static NV
static int32 filter_LTE5[8] = {-23,	-1761,	5295,	9000,	5477,	-425,	-2019,	840};
// LTE 5 Nominal Real Droop Filter length - same as Static NV
static uint8 filterlen_LTE5 = 8;
// LTE 5 Nominal Real Droop Filter group delay - same as Static NV
static uint8 filter_grp_delay_LTE5  = 3;

// Non-LTE Nominal Real Droop Filter coeffs - same as Static NV
static int32 filter_nonLTE[8] = {1031,-2850,415,5932,8134,4813,-621,-471};
// Non-LTE Nominal Real Droop Filter length - same as Static NV
static uint8 filterlen_nonLTE = 8;
// Non-LTE Nominal Real Droop Filter group delay - same as Static NV
static uint8 filter_grp_delay_nonLTE  = 4;

/*******************************************************/
/*       END MAGIC MATRICES FOR COEFF CALCULATION      */
/*******************************************************/

/*******************************************************/
/*       CONTAINERS FOR FILTER COEFFICIENTS            */
/*******************************************************/
static int32 droop_filter_coeffs[MAX_NUM_DROOP_FILTERS_GEN_PER_SWP][RFCOMMON_FBRX_NUM_FILTER_TAPS] = {{0}, {0}, {0}, {0}};
static uint8 droop_filter_grp_delay[MAX_NUM_DROOP_FILTERS_GEN_PER_SWP] = {0};
static uint8 droop_filter_len[MAX_NUM_DROOP_FILTERS_GEN_PER_SWP] = {0};
static uint8 droop_filter_tech_bw[MAX_NUM_DROOP_FILTERS_GEN_PER_SWP] = {RFCOMMON_FBRX_NUM_TX_CONFIG_MAX};

static int16 sat_int32_to_int16(int32 in, uint8 * overflow_det, uint8 debug_mode_en);
static int32 sat_add_int32(int32 addend1, int32 addend2, uint8 * overflow_det);
static int16 sat_add_int16(int16 addend1, int16 addend2, uint8 * overflow_det);
static int32 sat_int64_to_int32(int64 in_val, uint8 *overflow_det);
static void  print_int64hex(int64 * in_val);
static uint16 sat_uint32_to_uint16(uint32 in_val, uint8 *overflow_det);
static uint32 sat_uint64_to_uint32(uint64 in_val, uint8 *overflow_det);
static int32 sinApprox(uint16 phase, uint8 fracBits);

#ifdef FEATURE_FACTORY_TESTMODE

boolean ftm_enh_internal_device_cal_fb_droop_config_fbrx(void)
/* put this in a common place */
{
  boolean ret_val = TRUE;
  // set up per tech spec
  return ret_val;
}

boolean ftm_enh_internal_device_cal_fb_droop_config_stg(void)
/* put this in a common place */
{
  boolean ret_val = TRUE;
  // set up per tech spec ? 
  return ret_val;
}


boolean ftm_enh_internal_device_cal_fb_droop_calc_coeffs(void)
{
  boolean ret_val = TRUE;
  // calc per tech spec ? 
  return ret_val;
}

boolean ftm_enh_internal_device_cal_fb_droop_update_global_nv_struct(void)
{
  boolean ret_val = TRUE;
  // update per tech/band spec
  return ret_val;
}

boolean ftm_enh_internal_device_cal_fb_droop_get_global_nv_struct(void)
{
  boolean ret_val = TRUE;
  // get per tech/band spec
  return ret_val;
}

static int32 sat_int64_to_int32(int64 in_val, uint8 *overflow_det)
{
  int32 out_val;
  int32 max_val = 0x7FFFFFFF;
  int32 min_val = 0x80000000;
  int32 * temp1;
  *overflow_det = 0;
  if (in_val > (int64)max_val)
  {
    out_val = max_val;
    *overflow_det = 1;
    temp1 = (int32*)(&in_val);
    FTM_MSG_3(FTM_ERROR, "OVERFLOW DETECTED: 0x%08x%08x > 0x%08x", *(temp1+1), *temp1, max_val);
  }
  if (in_val < (int64)min_val)
  {
    out_val = min_val;
    *overflow_det = 1;
    temp1 = (int32*)(&in_val);
    FTM_MSG_3(FTM_ERROR, "OVERFLOW DETECTED: 0x%08x%08x < 0x%08x", *(temp1+1), *temp1, min_val);
  }
  temp1 = (int32*)(&in_val);
  if (*overflow_det == 0)
  {
    out_val = (int32)( in_val  & 0xFFFFFFFF );
  }
  return out_val;

}

static void  print_int64hex(int64 * in_val)
{
  int32 * temp;
  temp = (int32*)(in_val);
  FTM_MSG_2(FTM_HIGH,"int64 hex_val: 0x%08x%08x", *(temp+1), *temp);
}

static uint16 sat_uint32_to_uint16(uint32 in_val, uint8 *overflow_det)
{
  uint16 out_val;
  uint16 max_val = 0xFFFF;
  *overflow_det = 0;
  if (in_val > (uint32)max_val)
  {
    out_val = max_val;
    *overflow_det = 1;
    FTM_MSG_2(FTM_ERROR, "OVERFLOW DETECTED: %d > %d", in_val, max_val);
  }
  if (*overflow_det == 0)
  {
    out_val = (uint16)( in_val  & 0xFFFF );
  }
  return out_val;
}

static uint32 sat_uint64_to_uint32(uint64 in_val, uint8 *overflow_det)
{
  uint32 out_val;
  uint32 max_val = 0xFFFFFFFF;
  *overflow_det = 0;
  if (in_val > (uint64)max_val)
  {
    out_val = max_val;
    *overflow_det = 1;
    FTM_MSG_2(FTM_ERROR, "OVERFLOW DETECTED: %d > %d", in_val, max_val);
  }
  if (*overflow_det == 0)
  {
    out_val = (uint32)( in_val  & 0xFFFFFFFF );
  }
  return out_val;
}


static int32 sinApprox(uint16 phase, uint8 fracBits)
{
  int16 b_2;
  int16 b_1;
  int32 y;
  int16 xn;
  int16 xnt2;
  int32 mult1;
  int32 mult2;
  int16 const_c2 = -3438; // Fractional 1.15, -0.10490503363416896
  int16 const_c1 = 16827; // Fractional 1.15,  0.5135182999737543
  int16 const_c0 = 19733; // Fractional 1.15,  0.6022017475523785

  xn = phase - (1<<fracBits);
  xnt2 = 2*xn;

  b_2 = const_c2;
  mult1 = (int32)b_2*(int32)xnt2;
  b_1  =  (int16)((( mult1 & (0x80000000 + ~(0xFFFFFFFF << fracBits) ) )== (0x80000000 + (1 << (fracBits-1)) )
                   ?((mult1 - (1 << (fracBits-1) )) >> fracBits):((mult1 + (1 << (fracBits-1) )) >> fracBits)) & 0xFFFF) + const_c1;

  mult2 = (int32)b_1*(int32)xn;
  y = (int16)(((mult2 & (0x80000000 + ~(0xFFFFFFFF << fracBits) )) == (0x80000000 + (1 << (fracBits-1)) )?
               ((mult2 - (1<<(fracBits-1)))>> fracBits):((mult2 + (1<<(fracBits-1)))>> fracBits)
               ) & 0xFFFF) + const_c0 - b_2; 

  return y;
}


static int16 sat_int32_to_int16(int32 in_val, uint8 * overflow_det, uint8 debug_mode_en)
{
  int16 out_val;
  int16 max_val = 0x7FFF;
  int16 min_val = 0x8000;
  *overflow_det = 0;
  if (in_val > (int32)max_val)
  {
    out_val = max_val;
    *overflow_det = 1;
    if (debug_mode_en!=0)
    {
      FTM_MSG_2(FTM_ERROR, "OVERFLOW DETECTED: %d > %d", in_val, max_val);
    }
  }
  if (in_val < (int32)min_val)
  {
    out_val = min_val;
    *overflow_det = 1;
    if (debug_mode_en!=0)
    {
      FTM_MSG_2(FTM_ERROR, "OVERFLOW DETECTED: %d < %d", in_val, min_val);
    }
  }

  if (*overflow_det == 0)
  {
    out_val = (int16)( in_val  & 0xFFFF );
  }
  return out_val;

}

static int32 sat_add_int32(int32 addend1, int32 addend2, uint8 * overflow_det)
{
  int32 out_val;
  int32 max_val = 0x7FFFFFFF;
  int32 min_val = 0x80000000;
  *overflow_det = 0;

  if (addend1 >=0)
  {
    if (max_val - addend1 < addend2)
    {
      out_val = max_val;
      *overflow_det = 1;
      FTM_MSG_2(FTM_ERROR, "OVERFLOW DETECTED: %d < %d", max_val - addend1, addend2);
    }
  }
  else
  {
    if ( addend2 < min_val - addend1 )
    {
      out_val = min_val;
      *overflow_det = 1;
      FTM_MSG_2(FTM_ERROR, "OVERFLOW DETECTED: %d < %d", addend2, min_val - addend1);
    }
  }
  if (*overflow_det == 0)
  {
    out_val = addend1 + addend2;
  }

  return out_val;
}

static int16 sat_add_int16(int16 addend1, int16 addend2, uint8 * overflow_det)
{
  int16 out_val;
  int16 max_val = 0x7FFF;
  int16 min_val = 0x8000;
  *overflow_det = 0;

  if (addend1 >=0)
  {
    if (max_val - addend1 < addend2)
    {
      out_val = max_val;
      *overflow_det = 1;
      FTM_MSG_2(FTM_ERROR, "OVERFLOW DETECTED: %d < %d", max_val - addend1, addend2);
    }
  }
  else
  {
    if ( addend2 < min_val - addend1 )
    {
      out_val = min_val;
      *overflow_det = 1;
      FTM_MSG_2(FTM_ERROR, "OVERFLOW DETECTED: %d < %d", addend2, min_val - addend1);
    }
  }
  if (*overflow_det == 0)
  {
    out_val = addend1 + addend2;
  }

  return out_val;
}

ftm_enh_internal_device_cal_error_code_type ftm_enh_internal_device_cal_fb_droop_stg_cplx(uint8 tx_device,
                                                          uint8 num_captures,
                                                          int32 * stg_offset_khz,
                                                          uint16 num_iq_samples,
                                                          uint8 debug_mode_en,
                                                          uint8 do_processing,
                                                          uint32 * tonePwr,
                                                          uint8 fbrx_gain_state
                                                          )
{
  ftm_enh_internal_device_cal_error_code_type ret_val = FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS;
  boolean api_status = TRUE;
  rfdevice_rxtx_common_class *device_ptr = NULL;
  uint64 stg_freq = 0;
  ftm_rf_technology_type ftm_rf_tech  = FTM_RF_TECH_UNKNOWN;
  rfm_mode_enum_type rfm_mode = RFM_INVALID_MODE;
  int32 current_stg_freq_offset_khz = 0;
  uint32 tx_freq = 0;
  ftm_cal_xpt_instr_payload_iq_capture_type iq_cap_instr;
  char filename[20] = {'\0'};
  uint8 samp_buf_id[MAX_FREQ_OFFSETS_STG_IQ_CAP] = {-1};
  uint8 iter = 0;
  uint32 samp_rate = 1000000;
  ftm_enh_internal_device_cal_data_type *internal_device_cal_data;
  uint8 fbrx_bw = RFCOMMON_FBRX_LTE_10;
  uint32 refTonePower = 1;
  int32 minPosStgOffset = 0x7FFFFFFF;
  uint16 eqGainSq[MAX_NUM_DROOP_FILTERS_GEN_PER_SWP][MAX_FREQ_OFFSETS_STG_IQ_CAP];
  uint8  numNomEq = 0;
  uint8 filter_techbw[MAX_NUM_DROOP_FILTERS_GEN_PER_SWP];
  uint8 iter2 = 0;

  uint8 dummy_buffer;
  rfcom_band_type_u band_union;
  rfcom_bw_type_u rfcom_bw;

  #ifdef FEATURE_CDMA
  const rf_cdma_data_status_type *dev_status_r;
  rfm_cdma_band_class_type band;
  #endif

  #if defined(FEATURE_CDMA1X) && defined(FEATURE_CDMA) 
  rf_cdma_ept_data_type *ept_data; 
  #endif

  #ifdef FEATURE_LTE
  rfcom_bw.lte_bw = 3; 
  #endif

  /* Get Current FTM RF Tech in order to get current Tx Freq 
     and also to dispatch tech-specific IQ capture command */
  ftm_rf_tech = ftm_get_rf_technology();
  rfm_mode = ftm_common_ftm_tech_to_rfm_mode(ftm_rf_tech);
  internal_device_cal_data = ftm_common_data_get_enh_internal_device_cal_ptr();

  if (num_captures > MAX_FREQ_OFFSETS_STG_IQ_CAP)
  {
    FTM_MSG_2(FTM_ERROR, "Number of Requested IQ Captures %d exceeds allowed %d", num_captures, MAX_FREQ_OFFSETS_STG_IQ_CAP);
    internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_UNSUPPORTED_CAPTURE_NUM;
    return FTM_ENH_INTERNAL_DEVICE_CAL_UNSUPPORTED_CAPTURE_NUM;
  }

  /****************************************************************/
  /* Get STG Frequency to tune to from Tx channel information     */
  /****************************************************************/

  switch ( ftm_rf_tech )
  {
  #if defined(FEATURE_CDMA1X) && defined(FEATURE_CDMA) 
  case FTM_RF_TECH_CDMA:
    dev_status_r = rf_cdma_get_device_status( RFM_DEVICE_0 ); // what is the current tx device?
    if( dev_status_r != NULL)
    {
      //band = rf_cdma_sys_band_type_to_rf_band_type( dev_status_r->curr_band);
      tx_freq = rf_cdma_get_tx_carrier_freq( dev_status_r->curr_band, (int)
                                             dev_status_r->lo_tuned_to_chan );
      band_union.cdma_band = dev_status_r->curr_band;
      device_ptr = rfcommon_core_get_tx_device_ptr(tx_device, RFCOM_1X_MODE, band_union);
    }
    else
    {
      FTM_MSG(FTM_ERROR, "rfc_common_get_cmn_device_object(0) returns NULL");
      internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_GENERAL_FAILURE;
      return FTM_ENH_INTERNAL_DEVICE_CAL_GENERAL_FAILURE;
    }
    break;
  #endif

  #ifdef FEATURE_WCDMA
  case FTM_RF_TECH_WCDMA:
    tx_freq = rfwcdma_core_get_freq_from_ul_arfcn(rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx);
    FTM_MSG_1(FTM_HIGH, "ftm_enh_internal_device_cal_fb_droop_stg_sequence: curr_tx_chan: %d", tx_freq);
    band_union.wcdma_band = rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx;
    device_ptr = rfcommon_core_get_tx_device_ptr(tx_device, RFCOM_WCDMA_MODE, band_union);
    break;
  #endif /* FEATURE_WCDMA */
  #ifdef FEATURE_LTE
  case FTM_RF_TECH_LTE:
    tx_freq = rflte_core_get_tx_freq_from_uarfcn(
                                   rflte_ftm_mc_sm_get_tx_chan(),
                                   rflte_ftm_mc_sm_get_band());
    band_union.lte_band = rflte_ftm_mc_sm_get_band();
    rfcom_bw.lte_bw = rflte_ftm_mc_sm_get_tx_bw();
    device_ptr = rfcommon_core_get_tx_device_ptr(tx_device, RFCOM_LTE_MODE, band_union);
    break;
  #endif
  #ifdef FEATURE_TDSCDMA
  case FTM_RF_TECH_TDSCDMA:
    tx_freq = rf_tdscdma_core_util_get_freq_from_chan(rf_tdscdma_mc_state.curr_chan_tx);
    band_union.tdscdma_band = rf_tdscdma_mc_state.curr_band_tx;
    device_ptr = rfcommon_core_get_tx_device_ptr(tx_device, RFCOM_TDSCDMA_MODE, band_union);
    break;
  #endif
  default:
    FTM_MSG(FTM_ERROR, "Unsupported Tech Type: Return error handle");
    internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_UNSUPPORTED_TECH;
    return FTM_ENH_INTERNAL_DEVICE_CAL_UNSUPPORTED_TECH;
    /* Flag Error as Unsupported Tech */
  }

  /* check for null pointer and throw error */
  if (device_ptr == NULL)
  {
    FTM_MSG(FTM_ERROR, "rfc_common_get_cmn_device_object(0) returns NULL");
    internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DEV_ACTION_FAILED;
    return FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DEV_ACTION_FAILED;
  }

  #if defined(FEATURE_CDMA1X) && defined(FEATURE_CDMA) 
  ept_data = rf_cdma_get_ept_data();
  ept_data->cal_data.count_iq_capture = 0;
  #endif 
  
  // check if there is going to be too many captures to save on the EFS
  iq_cap_instr.primary_device = RFM_DEVICE_0; /* What is the tx device, doesn't matter */
  iq_cap_instr.feedback_device = RFM_DEVICE_0; /* What is the fbrx device */
  iq_cap_instr.num_samples = num_iq_samples;
  iq_cap_instr.proc_flag = PROC_NONE;
  iq_cap_instr.capture_type = XPT_CAPTURE_FBRX;

  /* Reset Capture Buffers */
  rfcommon_core_xpt_init_buffers(); // must be done before IQ capture code is called - Mark in Documentation for IQ Capture

  fbrx_bw = internal_device_cal_data->internal_device_cal_config.bw;

  for (iter = 0; iter<num_captures; iter++)
  {
    iq_cap_instr.first_trigger = 0;
    iq_cap_instr.last_trigger = 0;
    if (iter == 0)
    {
      iq_cap_instr.first_trigger = 1; 
    }
    if (iter == (num_captures-1))
    {
      iq_cap_instr.last_trigger = 1;
    }

    stg_freq = (tx_freq + stg_offset_khz[iter])*1000;
    /* TODO: check to see if frequency to configure STG at is good
       And Return error as unsupported Freq */
    FTM_MSG_4(FTM_HIGH, "ftm_enh_internal_device_cal_fb_droop_stg_cplx: current tech %d, current tx freq: %d, current_stg_freq_offset_khz: %d, stg_freq: %d",ftm_rf_tech, tx_freq, stg_offset_khz[iter],stg_freq);
    /****************************************************************/
    /*                  Set STG Frequency and Enable                */
    /*                   Also connects STG to FBRx                  */
    /****************************************************************/
    if ( debug_mode_en != 3 )
    {
      api_status = rfdevice_cmn_enable_stg(
                                        device_ptr,
                                        stg_freq, 
                                        RFDEVICE_EXECUTE_IMMEDIATE,
                                        NULL,
                                        rfm_mode,
                                        band_union,
                                        rfcom_bw,
                                        fbrx_gain_state
                                       );
      if (api_status == FALSE)
      {
        FTM_MSG(FTM_ERROR, "rfdevice_cmn_enable_stg returned FALSE ");
        rfcommon_core_xpt_free_all_buffers();  
        internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DEV_ACTION_FAILED;
        return FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DEV_ACTION_FAILED;
      }
    }
    /* Allow settling time for STG?? */

    /****************************************************************/
    /*              Setup and Trigger XPT IQ Capture                */
    /****************************************************************/

    /* Send First Trigger */
    ret_val =  ftm_enh_internal_device_cal_fbrx_fastmulti_iq_capture
                      (
                         ftm_rf_tech, 
                         &iq_cap_instr,
                         &samp_buf_id[iter],
                         &samp_rate
                      );

    if (ret_val != FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS)
    {
      FTM_MSG_1(FTM_ERROR, "ftm_enh_internal_device_cal_fbrx_fastmulti_iq_capture failed for tech %d", (int)ftm_rf_tech);
      if (debug_mode_en != 3 )
      {
        api_status = rfdevice_cmn_disable_stg(
                                        device_ptr,
                                        RFDEVICE_EXECUTE_IMMEDIATE,
                                        NULL,
                                        stg_freq,
                                        rfm_mode,
                                        band_union,
                                        rfcom_bw,
                                        fbrx_gain_state
                                       );
      }
      rfcommon_core_xpt_free_all_buffers();
      internal_device_cal_data->internal_device_cal_status.error_log = ret_val;
      return ret_val;
    }

    if (debug_mode_en !=0)
    {
      FTM_MSG_3(FTM_HIGH, "ftm_enh_internal_device_cal_fbrx_fastmulti_iq_capture: samp_buf_id[%d]=%d, samp_rate=%d", 
                  iter,
                  samp_buf_id[iter],
                  samp_rate
                );
    }

    /* If First Trigger, don't start processing */
    /* Check if (iter-1)-th capture is complete and process it during loop 'iter' */
    // Also check if below processing takes less time than sample capture duration, which is not good!
    if (iter == 0)
    {
      if (do_processing == 1)
      {
        // calc stuff from nominal EQ
        ret_val = ftm_enh_internal_device_cal_calc_nom_eq_gain_sq(
                                                                  &(eqGainSq[0][0]), 
                                                                  &numNomEq, 
                                                                  fbrx_bw, 
                                                                  &stg_offset_khz[0], 
                                                                  num_captures, 
                                                                  samp_rate, 
                                                                  &filter_techbw[0]
                                                                  );
        if (ret_val != FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS)
        {
          FTM_MSG_1(FTM_ERROR, "ftm_enh_internal_device_cal_calc_tone_pwr failed for tech %d", (int)ftm_rf_tech);
          if (debug_mode_en != 3 )
          {
            api_status = rfdevice_cmn_disable_stg(
                                            device_ptr,
                                            RFDEVICE_EXECUTE_IMMEDIATE,
                                            NULL,
                                            stg_freq,
                                            rfm_mode,
                                            band_union,
                                            rfcom_bw,
                                            fbrx_gain_state
                                           );
          }
          rfcommon_core_xpt_free_all_buffers();
          internal_device_cal_data->internal_device_cal_status.error_log = ret_val;
          return ret_val;
        }
      }
      rfcommon_xpt_wait_for_capture_done( 500, 6000, &dummy_buffer );
    }
    if (iter !=0)
    {
      /* Process data for (k-1)-th capture */
      if (do_processing == 1)
      {
        ret_val = ftm_enh_internal_device_cal_calc_tone_pwr(samp_buf_id[iter-1], num_iq_samples, stg_offset_khz[iter-1], samp_rate, &tonePwr[iter-1]);
        if (ret_val != FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS)
        {
          FTM_MSG_1(FTM_ERROR, "ftm_enh_internal_device_cal_calc_tone_pwr failed for tech %d", (int)ftm_rf_tech);
          if (debug_mode_en != 3 )
          {
            api_status = rfdevice_cmn_disable_stg(
                                            device_ptr,
                                            RFDEVICE_EXECUTE_IMMEDIATE,
                                            NULL,
                                            stg_freq,
                                            rfm_mode,
                                            band_union,
                                            rfcom_bw,
                                            fbrx_gain_state
                                           );
          }
          rfcommon_core_xpt_free_all_buffers();
          internal_device_cal_data->internal_device_cal_status.error_log = ret_val;
          return ret_val;
        }

        if ( (stg_offset_khz[iter-1] > 0) && (stg_offset_khz[iter-1] <minPosStgOffset))
        {
          refTonePower = tonePwr[iter-1];
          minPosStgOffset = stg_offset_khz[iter-1];
        }
      }
      rfcommon_xpt_wait_for_capture_done( 100, 6000, &dummy_buffer );
    }

    /* if last trigger, process it after the capture is complete and store data */
    /* save samples if debug bit is enabled */

    /****************************************************************/
    /*                     Disable FBRx STG                         */
    /****************************************************************/

    if (debug_mode_en !=3)
    {
      api_status = rfdevice_cmn_disable_stg(
                                      device_ptr,
                                      RFDEVICE_EXECUTE_IMMEDIATE,
                                      NULL,
                                      stg_freq,
                                      rfm_mode,
                                      band_union,
                                      rfcom_bw,
                                      fbrx_gain_state
                                     );
      if (api_status == FALSE)
      {
        FTM_MSG_1(FTM_ERROR, "rfdevice_cmn_disable_stg failed in loop #%d, breaking loop, check FTM Msgs", iter );
        rfcommon_core_xpt_free_all_buffers(); 
        internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DEV_ACTION_FAILED;
        return FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DEV_ACTION_FAILED;
      }
    }
  
  }
  if ((do_processing == 1) && (iter > 0))
  {
    ret_val = ftm_enh_internal_device_cal_calc_tone_pwr(samp_buf_id[iter-1], num_iq_samples, stg_offset_khz[iter-1], samp_rate, &tonePwr[iter-1]);
    if (ret_val != FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS)
    {
      FTM_MSG_1(FTM_ERROR, "ftm_enh_internal_device_cal_calc_tone_pwr failed for tech %d", (int)ftm_rf_tech);
      rfcommon_core_xpt_free_all_buffers();
      internal_device_cal_data->internal_device_cal_status.error_log = ret_val;
      return ret_val;
    }
    if ( (stg_offset_khz[iter-1] > 0) && (stg_offset_khz[iter-1] <minPosStgOffset))
    {
      refTonePower = tonePwr[iter-1];
      minPosStgOffset = stg_offset_khz[iter-1];
    }
    if (debug_mode_en!=0)
    {
      for (iter = 0; iter<num_captures; iter++)
      {
        FTM_MSG_4(FTM_HIGH,  " STG Freq: %d, Tone Power: %d, Ref Freq: %d, Tone Power: %d", stg_offset_khz[iter], tonePwr[iter], minPosStgOffset, refTonePower);
      }
    }
  }

  // Save IQ samples if debug_mode_en >=2
  if ((debug_mode_en!=0) & (debug_mode_en!=1))
  {
    for (iter = 0; iter<num_captures; iter++)
    {
      FTM_MSG_2(FTM_HIGH, "Saving File %d, stg_offset_khz: %d", iter, stg_offset_khz[iter]);
      memset((void*)&filename[0], '\0', sizeof(char)*20);
      snprintf(&filename[0], 20,"iq_%05d_khz.bin", (int)stg_offset_khz[iter] );
      api_status = rfcommon_mc_save_xpt_iq_data( samp_buf_id[iter],num_iq_samples,&filename[0]);
      if (api_status == FALSE)
      {
        rfcommon_core_xpt_free_all_buffers();
        FTM_MSG_1(FTM_ERROR, "rfcommon_mc_save_xpt_iq_data return %d", api_status);
        internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_EFS_FILESAVE_FAILED;
        return FTM_ENH_INTERNAL_DEVICE_CAL_EFS_FILESAVE_FAILED;
      }
    }
  }
  if (do_processing == 1)
  {
    ret_val = ftm_enh_internal_device_cal_calc_eq(
                                                   num_captures, 
                                                   &tonePwr[0], 
                                                   &stg_offset_khz[0], 
                                                   &(eqGainSq[0][0]), 
                                                   &filter_techbw[0], 
                                                   numNomEq, 
                                                   refTonePower , 
                                                   minPosStgOffset
                                                 );
    if (ret_val != FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS)
    {
      FTM_MSG_1(FTM_ERROR, "ftm_enh_internal_device_cal_calc_eq failed for tech %d", (int)ftm_rf_tech);
      rfcommon_core_xpt_free_all_buffers();
      internal_device_cal_data->internal_device_cal_status.error_log = ret_val;
      return ret_val;
    }
  }

  rfcommon_core_xpt_free_all_buffers();  // must be done after last IQ capture code is called - Mark in Documentation for IQ Capture

  if (debug_mode_en != 0)
  {
    FTM_MSG_1(FTM_HIGH, "num_droop_filters: %d", num_droop_filters);
    for (iter = 0; iter < num_droop_filters; iter++)
    {
      FTM_MSG_3(FTM_HIGH, "tech_bw: %d, filt_len: %d, grp_delay: %d", droop_filter_tech_bw[iter], droop_filter_len[iter], droop_filter_grp_delay[iter]);
      for (iter2 = 0; iter2<droop_filter_len[iter]; iter2++)
      {
        FTM_MSG_2(FTM_HIGH, "coeff[%d]: %d", iter2, droop_filter_coeffs[iter][iter2]);
      }
    }
  }

  return ret_val;

}


ftm_enh_internal_device_cal_error_code_type ftm_enh_internal_device_cal_fb_droop_stg_sequence(uint8 tx_device,
                                                          uint8 num_captures,
                                                          int32 *stg_offset_khz,
                                                          uint16 num_iq_samples,
                                                          uint8 debug_mode_en,
                                                          uint8 do_processing,
                                                          uint8 fbrx_gain_state
                                                          )
{
  ftm_enh_internal_device_cal_error_code_type ret_val = FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS;
  boolean api_status = TRUE;
  rfdevice_rxtx_common_class *device_ptr = NULL;
  uint64 stg_freq = 0;
  ftm_rf_technology_type ftm_rf_tech  = FTM_RF_TECH_UNKNOWN;
  rfm_mode_enum_type rfm_mode = RFM_INVALID_MODE;
  int32 current_stg_freq_offset_khz = 0;
  uint32 tx_freq = 0;
  ftm_cal_xpt_instr_payload_iq_capture_type iq_cap_instr;
  char filename[20] = {'\0'};
  uint8 samp_buf_id[MAX_FREQ_OFFSETS_STG_IQ_CAP] = {-1};
  uint8 iter = 0;
  rfcom_band_type_u band_union;
  rfcom_bw_type_u rfcom_bw;

  #ifdef FEATURE_CDMA
  const rf_cdma_data_status_type *dev_status_r;
  rfm_cdma_band_class_type band;
  #endif

  #ifdef FEATURE_LTE
  rfcom_bw.lte_bw = 3; 
  #endif

  /* Get Current FTM RF Tech in order to get current Tx Freq 
     and also to dispatch tech-specific IQ capture command */
  ftm_rf_tech = ftm_get_rf_technology();
  rfm_mode = ftm_common_ftm_tech_to_rfm_mode(ftm_rf_tech);

  if (num_captures > MAX_FREQ_OFFSETS_STG_IQ_CAP)
  {
    FTM_MSG_2(FTM_ERROR, "Number of Requested IQ Captures %d exceeds allowed %d", num_captures, MAX_FREQ_OFFSETS_STG_IQ_CAP);
    return FTM_ENH_INTERNAL_DEVICE_CAL_UNSUPPORTED_CAPTURE_NUM;
  }


  /****************************************************************/
  /* Get STG Frequency to tune to from Tx channel information     */
  /****************************************************************/

  switch ( ftm_rf_tech )
  {
  #if defined(FEATURE_CDMA1X) && defined(FEATURE_CDMA) 
  case FTM_RF_TECH_CDMA:
    dev_status_r = rf_cdma_get_device_status( RFM_DEVICE_0 ); // what is the current tx device?
    if( dev_status_r != NULL)
    {
      //band = rf_cdma_sys_band_type_to_rf_band_type( dev_status_r->curr_band);
      tx_freq = rf_cdma_get_tx_carrier_freq( dev_status_r->curr_band, (int)
                                             dev_status_r->lo_tuned_to_chan );
      band_union.cdma_band = dev_status_r->curr_band;
      device_ptr = rfcommon_core_get_tx_device_ptr(tx_device, RFCOM_1X_MODE, band_union);
    }
    else
    {
      FTM_MSG(FTM_ERROR, "rfc_common_get_cmn_device_object(0) returns NULL");
      return FTM_ENH_INTERNAL_DEVICE_CAL_GENERAL_FAILURE;
    }
    break;
  #endif

  #ifdef FEATURE_WCDMA
  case FTM_RF_TECH_WCDMA:
    tx_freq = rfwcdma_core_get_freq_from_ul_arfcn(rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx);
    FTM_MSG_1(FTM_HIGH, "ftm_enh_internal_device_cal_fb_droop_stg_sequence: curr_tx_chan: %d", tx_freq);
    band_union.wcdma_band = rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx;
    device_ptr = rfcommon_core_get_tx_device_ptr(tx_device, RFCOM_WCDMA_MODE, band_union);
    break;
  #endif /* FEATURE_WCDMA */
  #ifdef FEATURE_LTE
  case FTM_RF_TECH_LTE:
    tx_freq = rflte_core_get_tx_freq_from_uarfcn(
                                   rflte_ftm_mc_sm_get_tx_chan(),
                                   rflte_ftm_mc_sm_get_band());
    band_union.lte_band = rflte_ftm_mc_sm_get_band();
    rfcom_bw.lte_bw = rflte_ftm_mc_sm_get_tx_bw();
    device_ptr = rfcommon_core_get_tx_device_ptr(tx_device, RFCOM_LTE_MODE, band_union);
    break;
  #endif
  #ifdef FEATURE_TDSCDMA
  case FTM_RF_TECH_TDSCDMA:
    tx_freq = rf_tdscdma_core_util_get_freq_from_chan(rf_tdscdma_mc_state.curr_chan_tx);
    band_union.tdscdma_band = rf_tdscdma_mc_state.curr_band_tx;
    device_ptr = rfcommon_core_get_tx_device_ptr(tx_device, RFCOM_TDSCDMA_MODE, band_union);
    break;
  #endif
  default:
    FTM_MSG(FTM_ERROR, "Unsupported Tech Type: Return error handle");
    return FALSE;
    /* Flag Error as Unsupported Tech */
  }

  /* check for null pointer and throw error */
  if (device_ptr == NULL)
  {
    FTM_MSG(FTM_ERROR, "rfc_common_get_cmn_device_object(0) returns NULL");
    return FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DEV_ACTION_FAILED;
  }
  
  // check if there is going to be too many captures to save on the EFS
  iq_cap_instr.primary_device = RFM_DEVICE_0; /* What is the tx device, doesn't matter */
  iq_cap_instr.feedback_device = RFM_DEVICE_0; /* What is the fbrx device */
  iq_cap_instr.num_samples = num_iq_samples;
  iq_cap_instr.proc_flag = PROC_NONE;
  iq_cap_instr.capture_type = XPT_CAPTURE_FBRX;

  /* Reset Capture Buffers */
  rfcommon_core_xpt_init_buffers(); // must be done before IQ capture code is called - Mark in Documentation for IQ Capture

  for (iter = 0; iter<num_captures; iter++)
  {
    iq_cap_instr.first_trigger = 0;
    iq_cap_instr.last_trigger = 0;
    if (iter == 0)
    {
      iq_cap_instr.first_trigger = 1; 
    }
    if (iter == (num_captures-1))
    {
      iq_cap_instr.last_trigger = 1;
    }

    if (debug_mode_en!=3)
    {
      snprintf(&filename[0], 20,"iq_%05d_khz.bin", (int)stg_offset_khz[iter] );
    }
    else
    {
      snprintf(&filename[0], 20, "iq_mod.bin" );
    }

    stg_freq = (tx_freq + stg_offset_khz[iter])*1000;
    /* TODO: check to see if frequency to configure STG at is good
       And Return error as unsupported Freq */
    FTM_MSG_4(FTM_HIGH, "ftm_enh_internal_device_cal_fb_droop_stg_sequence: current tech %d, current tx freq: %d, current_stg_freq_offset_khz: %d, stg_freq: %d",ftm_rf_tech, tx_freq, current_stg_freq_offset_khz,stg_freq);
    /****************************************************************/
    /*                  Set STG Frequency and Enable                */
    /*                   Also connects STG to FBRx                  */
    /****************************************************************/
    if ( debug_mode_en != 3 )
    {
      api_status = rfdevice_cmn_enable_stg(
                                        device_ptr,
                                        stg_freq, 
                                        RFDEVICE_EXECUTE_IMMEDIATE,
                                        NULL,
                                        rfm_mode,
                                        band_union,
                                        rfcom_bw,
                                        fbrx_gain_state
                                       );
      if (api_status == FALSE)
      {
        FTM_MSG(FTM_ERROR, "rfdevice_cmn_enable_stg returned FALSE ");
        rfcommon_core_xpt_free_all_buffers();  
        return FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DEV_ACTION_FAILED;
      }
    }
    /* Allow settling time for STG?? */

    /****************************************************************/
    /*              Setup and Trigger XPT IQ Capture                */
    /****************************************************************/

    api_status = ftm_enh_internal_device_cal_fbrx_iq_capture(ftm_rf_tech, &iq_cap_instr, &samp_buf_id[iter], debug_mode_en, &filename[0]);

    if (ret_val != FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS)
    {
      FTM_MSG_1(FTM_ERROR, "ftm_enh_internal_device_cal_fbrx_iq_capture failed for tech %d", (int)ftm_rf_tech);
      if (debug_mode_en != 3 )
      {
        api_status = rfdevice_cmn_disable_stg(
                                        device_ptr,
                                        RFDEVICE_EXECUTE_IMMEDIATE,
                                        NULL,
                                        stg_freq,
                                        rfm_mode,
                                        band_union,
                                        rfcom_bw,
                                        fbrx_gain_state
                                       );
      }
      rfcommon_core_xpt_free_all_buffers();
      return ret_val;
    }

    FTM_MSG_1(FTM_HIGH, "Current Capture Buffer is %d", samp_buf_id[iter]);

    /****************************************************************/
    /*                     Disable FBRx STG                         */
    /****************************************************************/

    if (debug_mode_en !=3)
    {
      api_status = rfdevice_cmn_disable_stg(
                                      device_ptr,
                                      RFDEVICE_EXECUTE_IMMEDIATE,
                                      NULL,
                                      stg_freq,
                                      rfm_mode,
                                      band_union,
                                      rfcom_bw,
                                      fbrx_gain_state
                                     );
      if (api_status == FALSE)
      {
        FTM_MSG_1(FTM_ERROR, "Something went wrong on loop #%d, breaking loop, check FTM Msgs", iter );
        rfcommon_core_xpt_free_all_buffers(); 
        return FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DEV_ACTION_FAILED;
      }
    }
  
  }

  // perform calculation of filter coeffs using all the capture buffers
  // set error flag whether calculation was successful

  if (do_processing == 1)
  {
    ret_val = ftm_enh_internal_device_cal_droop_stg_process(FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DROOP_STG_REAL_COEFFS, 
                                                            num_captures, &stg_offset_khz[0], &samp_buf_id[0],
                                                            num_iq_samples);
  }


  rfcommon_core_xpt_free_all_buffers();  // must be done after last IQ capture code is called - Mark in Documentation for IQ Capture

  return ret_val;
}


ftm_enh_internal_device_cal_error_code_type ftm_enh_internal_device_cal_fb_droop_mod_sequence(void)
{
  ftm_enh_internal_device_cal_error_code_type ret_val = FTM_ENH_INTERNAL_DEVICE_CAL_UNSUPPORTED_ACTION;
//  rfdevice_rxtx_common_class *device_ptr = NULL;
//  device_ptr = rfc_common_get_cmn_device_object(0);

  // if first_trig
  //    perform first time configs

  // perform capture
  // if debug flag enabled
  //    save to EFS with certain name
  // perform calc
  // update global nv struct

  // if last_trig
  //    perform last time deconfigs
  FTM_MSG(FTM_ERROR, "ftm_enh_internal_device_cal_fb_droop_mod_sequence(): not supported yet!");

  return ret_val;

}

boolean ftm_enh_internal_device_cal_fb_droop_iq_capture(void)
{
  boolean ret_val = TRUE;


  return ret_val;
}


/*----------------------------------------------------------------------------*/
/*!
  @name: ftm_enh_internal_cal_fb_droop

  @brief: feedback droop calbration
  
 
  @details
  
  @param
  input: req_instr, payload instruction
  output: res_len:  cal result length
             fb_droop_result: output of feedback droop cal NV
  
  @return
     success or fail
*/

boolean ftm_enh_internal_device_cal_fb_droop
(
  void const *req_instr,
  uint16 * res_len,
  void *fb_droop_result 
)
{
   ftm_enh_internal_device_cal_error_code_type ret_val = FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS;
   boolean api_status = TRUE;
   uint8 iter = 0;
   uint8 iter2 = 0;
   uint8 num_captures   = 0;   
   // number of IQ captures to perform; can be total STG freq offsets to capture or any number of captures to perform
   uint8 fbrx_gain_state = 0;
   uint8 tx_device = 0;
   uint8 fbrx_bw = RFCOMMON_FBRX_LTE_10;
   int32 stg_offset_khz[MAX_FREQ_OFFSETS_STG_IQ_CAP] = {0};
   uint16 num_iq_samples = 0;
   uint8 debug_mode_en = 0;
   rfdevice_rxtx_common_class* device_ptr = NULL;
   uint8 do_processing = 1;
   uint32 stg_raw_tone_pwrs[MAX_FREQ_OFFSETS_STG_IQ_CAP] = {0};

   rfcommon_fbrx_nv_droop_type *droop_data_nv = NULL;
   rfcommon_fbrx_error_type nv_read_write_error;
   ftm_enh_internal_device_cal_data_type *internal_device_cal_data;

   ftm_rf_technology_type fbrx_tech = FTM_RF_TECH_UNKNOWN;
   ftm_rf_technology_type curr_tx_ftm_rf_tech = FTM_RF_TECH_UNKNOWN;

   ftm_enh_internal_device_cal_instr_payload_fb_droop_type * fb_droop_instr = (ftm_enh_internal_device_cal_instr_payload_fb_droop_type *)req_instr;
   ftm_enh_internal_device_cal_fb_droop_result_type * droop_result = (ftm_enh_internal_device_cal_fb_droop_result_type *) fb_droop_result;

   #ifdef FEATURE_CDMA
   const rf_cdma_data_status_type *dev_status_r;
   #endif

   rfcom_band_type_u band_union;

   ftm_enh_internal_device_cal_fbrx_droop_cal_method_type cal_method = fb_droop_instr->cal_method;
   tx_device = fb_droop_instr->tx_device; // tx device set up independently, TODO: keep track of the valid tx device
   fbrx_gain_state = fb_droop_instr->fbrx_gain_state;
   num_captures = fb_droop_instr->num_captures;
   num_iq_samples = fb_droop_instr->num_iq_samples;

   memscpy(&stg_offset_khz[0], sizeof(int32)*MAX_FREQ_OFFSETS_STG_IQ_CAP, &(fb_droop_instr->stg_offset_khz[0]), sizeof(int32)*MAX_FREQ_OFFSETS_STG_IQ_CAP);

   internal_device_cal_data = ftm_common_data_get_enh_internal_device_cal_ptr();

   debug_mode_en = internal_device_cal_data->internal_device_cal_config.dbg_mode_en;
   fbrx_tech = (ftm_rf_technology_type)internal_device_cal_data->internal_device_cal_config.tech;

   fbrx_bw = internal_device_cal_data->internal_device_cal_config.bw;

   if (fbrx_bw >= RFCOMMON_FBRX_NUM_TX_CONFIG_MAX )
   {
     FTM_MSG_1(FTM_ERROR, "Unknown FBRx Tech/BW combination %d", fbrx_bw);
     internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_INVALID_TECHBW;
     return FALSE;
   }
   // For LTE5, need to perform LTE10 captures, processing will be different
   // For LTE15,need to perform LTE20 captures, processing will be different
   // For LTE1p4/3, need to perform LTE10 captures, use processing results from LTE5
   switch (fbrx_bw) 
   {
   case RFCOMMON_FBRX_LTE_15:
     fbrx_bw = RFCOMMON_FBRX_LTE_20;
     break;
   case RFCOMMON_FBRX_LTE_1P4:
   case RFCOMMON_FBRX_LTE_3:
   case RFCOMMON_FBRX_LTE_5:
     fbrx_bw = RFCOMMON_FBRX_LTE_10;
     break;
   default:
     break;
   }

   do_processing = fb_droop_instr->do_processing;
   *res_len = sizeof( ftm_enh_internal_device_cal_fb_droop_result_type);

   /*allocate memory*/
   if (droop_data_nv == NULL)
   {
     droop_data_nv = modem_mem_calloc(1, sizeof(rfcommon_fbrx_nv_droop_type), MODEM_MEM_CLIENT_RFA);
     if (droop_data_nv == NULL)
     {
       FTM_MSG(FTM_ERROR, " ftm_enh_internal_device_cal_fb_droop:  Failed to allocate memory for droop_data_nv !");
       internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_MEM_ALLOC_FAILURE;
       return FALSE;
     }
   }

   // make first filter tap 1 and the rest 0 by default in case rest of this flow falls apart
   memset((void*)&droop_filter_coeffs[0][0], 0, sizeof(int32)*RFCOMMON_FBRX_NUM_FILTER_TAPS*MAX_NUM_DROOP_FILTERS_GEN_PER_SWP);
   for (iter = 0; iter < MAX_NUM_DROOP_FILTERS_GEN_PER_SWP; iter++)
   {
     droop_filter_tech_bw[iter] = RFCOMMON_FBRX_NUM_TX_CONFIG_MAX;
     droop_filter_grp_delay[iter] = 0;
     droop_filter_len[iter] = 0;
     droop_filter_coeffs[iter][0] = 1 << 14; // Make default filter a unit impulse function with proper DC gain
   }

   curr_tx_ftm_rf_tech = ftm_get_rf_technology();
   if (fbrx_tech != curr_tx_ftm_rf_tech)
   {
     FTM_MSG_2(FTM_ERROR, "Current RF Tech %d does not match Tech for FBRx Cal %d", curr_tx_ftm_rf_tech, fbrx_tech );
     modem_mem_free((void*)droop_data_nv, MODEM_MEM_CLIENT_RFA);
     internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_TECH_MISMATCH;
     return FALSE;
   }

   FTM_MSG_1(FTM_HIGH, "Entering FB_DROOP_CAL, Cmd: %s", fb_droop_instr);
   if (debug_mode_en!=0)
   {
     FTM_MSG_5(FTM_MED, "Droop Cal Parameters -- calMethod: %d, txDevice: %d, fbrxGainState: %d, numCaptures: %d, numIQSamples: %d ", 
              cal_method, tx_device, fbrx_gain_state, num_captures, num_iq_samples ) ;

     FTM_MSG_4(FTM_MED, "Droop Cal Parameters -- Req FBRx Tech: %d, Current Tx Tech: %d, FBRx Tech/BW: %d, debugModeEn: %d ", 
              fbrx_tech, curr_tx_ftm_rf_tech, fbrx_bw, debug_mode_en ) ;

     for (iter = 0; iter<num_captures; iter++)
     {
       FTM_MSG_2(FTM_MED, "          stg_freq[%2d]  :   %5d kHz", iter, stg_offset_khz[iter] );
     }
   }

   switch (fbrx_tech) 
   {
     #if defined(FEATURE_CDMA1X) && defined(FEATURE_CDMA) 
     case FTM_RF_TECH_CDMA:
       dev_status_r = rf_cdma_get_device_status( RFM_DEVICE_0 ); // what is the current tx device?
       if( dev_status_r != NULL)
       {
         band_union.cdma_band = dev_status_r->curr_band;
         device_ptr = rfcommon_core_get_tx_device_ptr(tx_device, RFCOM_1X_MODE, band_union);
       }
       else
       {
         FTM_MSG(FTM_ERROR, "rfcommon_core_get_tx_device_ptr(0) returns NULL");
       }
       break;
     #endif

     #ifdef FEATURE_WCDMA
     case FTM_RF_TECH_WCDMA:
       band_union.wcdma_band = rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx;
       device_ptr = rfcommon_core_get_tx_device_ptr(tx_device, RFCOM_WCDMA_MODE, band_union);
       break;
     #endif /* FEATURE_WCDMA */
     #ifdef FEATURE_LTE
     case FTM_RF_TECH_LTE:
       band_union.lte_band = rflte_ftm_mc_sm_get_band();
       device_ptr = rfcommon_core_get_tx_device_ptr(tx_device, RFCOM_LTE_MODE, band_union);
       break;
     #endif
     #ifdef FEATURE_TDSCDMA
     case FTM_RF_TECH_TDSCDMA:
       band_union.tdscdma_band = rf_tdscdma_mc_state.curr_band_tx;
       device_ptr = rfcommon_core_get_tx_device_ptr(tx_device, RFCOM_TDSCDMA_MODE, band_union);
       break;
     #endif
     default:
       FTM_MSG(FTM_ERROR, "ftm_enh_internal_device_cal_fb_droop Unsupported Tech Type");
   }
   /* check for null pointer and throw error */
   if (device_ptr == NULL) 
   {
     FTM_MSG(FTM_ERROR, "rfc_common_get_cmn_device_object(0) returns NULL");
     modem_mem_free((void*)droop_data_nv, MODEM_MEM_CLIENT_RFA);
     internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DEV_ACTION_FAILED;
     return FALSE;
   }

   api_status = rfcommon_fbrx_mc_enable_rf_dev(device_ptr, fbrx_gain_state);
   if (api_status == FALSE)
   {
     FTM_MSG_1(FTM_ERROR, "Failed to Enable FBRx Device in Gain State %d " , fbrx_gain_state);
     modem_mem_free((void*)droop_data_nv, MODEM_MEM_CLIENT_RFA);
     internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DEV_ACTION_FAILED;
     return FALSE;
   }
   if (cal_method == FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DROOP_STG_REAL_COEFFS)
   {
     if (debug_mode_en != 0)
     {
       FTM_MSG(FTM_MED, "---------- M_LTE5 -----------" );
       for (iter = 0; iter < 3; iter++)
       {
         FTM_MSG_1(FTM_MED, "--------  Row     %d   ---------", iter);
         for (iter2 = 0; iter2 < EQ_LEN; iter2++)
         {
           FTM_MSG_1(FTM_MED, "    %10d ", M_LTE5[iter][iter2]);
         }
       }

       FTM_MSG(FTM_MED, "---------- M_LTE10 -----------" );
       for (iter = 0; iter < 3; iter++)
       {
         FTM_MSG_1(FTM_MED, "--------  Row     %d   ---------", iter);
         for (iter2 = 0; iter2 < EQ_LEN; iter2++)
         {
           FTM_MSG_1(FTM_MED, "    %10d ", M_LTE10[iter][iter2]);
         }
       }


       FTM_MSG(FTM_MED, "---------- M_LTE15 -----------" );
       for (iter = 0; iter < 3; iter++)
       {
         FTM_MSG_1(FTM_MED, "--------  Row     %d   ---------", iter);
         for (iter2 = 0; iter2 < EQ_LEN; iter2++)
         {
           FTM_MSG_1(FTM_MED, "    %10d ", M_LTE15[iter][iter2]);
         }
       }


       FTM_MSG(FTM_MED, "---------- M_LTE20 -----------" );
       for (iter = 0; iter < 3; iter++)
       {
         FTM_MSG_1(FTM_MED, "--------  Row     %d   ---------", iter);
         for (iter2 = 0; iter2 < EQ_LEN; iter2++)
         {
           FTM_MSG_1(FTM_MED, "    %10d ", M_LTE20[iter][iter2]);
         }
       }

     }
   }

   ret_val = ftm_enh_internal_device_cal_fbrx_iq_cap_config(fbrx_tech, fbrx_bw);
   if (ret_val != FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS)
   {
     FTM_MSG_1(FTM_ERROR, "ftm_enh_internal_device_cal_fbrx_iq_cap_config failed with error code %d", ret_val );
     modem_mem_free((void*)droop_data_nv, MODEM_MEM_CLIENT_RFA);
     rfcommon_fbrx_mc_disable_rf_dev(device_ptr);
     ftm_enh_internal_device_cal_fbrx_iq_cap_deconfig(fbrx_tech);
     internal_device_cal_data->internal_device_cal_status.error_log = ret_val;
     return FALSE;
   }

   if (cal_method == FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DROOP_STG_REAL_COEFFS)
   {
     // need to pass in gain state, num_caps, first_trig, last_trig, base_offset_khz, num_iq_samples
     ret_val = ftm_enh_internal_device_cal_fb_droop_stg_sequence( tx_device,
                                                                   num_captures,
                                                                   &stg_offset_khz[0],
                                                                   num_iq_samples,
                                                                   debug_mode_en,
                                                                  do_processing,
                                                                  fbrx_gain_state
                                                                  );

   }
   else if (cal_method == FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DROOP_STG_CPLX_COEFFS)
   {
     ret_val = ftm_enh_internal_device_cal_fb_droop_stg_cplx(
                                                            tx_device,
                                                            num_captures,
                                                            &stg_offset_khz[0],
                                                            num_iq_samples,
                                                            debug_mode_en,
                                                            do_processing,
                                                            &stg_raw_tone_pwrs[0],
                                                            fbrx_gain_state
                                                          );
   }
   else if (cal_method == FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DROOP_MOD_REAL_COEFFS)
   {
     // need to pass in gain state, num_caps, first_trig, last_trig, num_iq_samples
     // ret_val = ftm_enh_internal_device_cal_fb_droop_mod_sequence(fbrx_gain_state,
     //                                                              num_captures,
     //                                                              first_trig,
     //                                                              last_trig,
     //                                                              num_iq_samples
     //                                                             );
     ret_val = ftm_enh_internal_device_cal_fb_droop_mod_sequence();
   }
   else
   {
     FTM_MSG(FTM_ERROR, "Undefined calibration method!");
     ret_val = FTM_ENH_INTERNAL_DEVICE_CAL_UNSUPPORTED_ACTION;
   }

   if (ret_val != FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS)
   {
     FTM_MSG_1(FTM_ERROR, "ftm_enh_internal_device_cal_fb_droop failed with error code %d", ret_val );
     modem_mem_free((void*)droop_data_nv, MODEM_MEM_CLIENT_RFA);
     rfcommon_fbrx_mc_disable_rf_dev(device_ptr);
     ftm_enh_internal_device_cal_fbrx_iq_cap_deconfig(fbrx_tech);
     internal_device_cal_data->internal_device_cal_status.error_log = ret_val;
     return FALSE;
   }

   ret_val = ftm_enh_internal_device_cal_fbrx_iq_cap_deconfig(fbrx_tech);
   if (ret_val != FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS)
   {
     FTM_MSG_1(FTM_ERROR, "ftm_enh_internal_device_cal_fbrx_iq_cap_deconfig failed with error code %d", ret_val );
     modem_mem_free((void*)droop_data_nv, MODEM_MEM_CLIENT_RFA);
     rfcommon_fbrx_mc_disable_rf_dev(device_ptr);
     internal_device_cal_data->internal_device_cal_status.error_log = ret_val;
     return FALSE;
   }

   api_status = rfcommon_fbrx_mc_disable_rf_dev(device_ptr);
   if (api_status == FALSE)
   {
     FTM_MSG(FTM_ERROR, "Failed to Disable FBRx Device ");
     modem_mem_free((void*)droop_data_nv, MODEM_MEM_CLIENT_RFA);
     internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DEV_ACTION_FAILED;
     return FALSE;
   }

   // init result struct
   droop_result->fbrx_gain_state = fbrx_gain_state;
   droop_result->num_filters = MAX_NUM_DROOP_FILTERS_GEN_PER_SWP;
   for (iter = 0; iter < MAX_FREQ_OFFSETS_STG_IQ_CAP; iter++)
   {
     droop_result->stg_tone_pwrs[iter] = 0;
   }
   for (iter = 0; iter < MAX_NUM_DROOP_FILTERS_GEN_PER_SWP; iter++)
   {
     droop_result->fbrx_droop_res[iter].fbrx_tech_bw_type = -1;
     droop_result->fbrx_droop_res[iter].group_delay = 0;
     droop_result->fbrx_droop_res[iter].num_filter_taps = 1;
     memset((void*)&(droop_result->fbrx_droop_res[iter].fbrx_fir_tap_coeffs_nv[0]), 0, 
            FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DROOP_FILTER_LEN_MAX * sizeof(int32));
     droop_result->fbrx_droop_res[iter].fbrx_fir_tap_coeffs_nv[0] = 1 << 14;
   }
   if (debug_mode_en != 0)
   {
     FTM_MSG_1(FTM_HIGH, "num_droop_filters: %d", num_droop_filters);
     for (iter = 0; iter < num_droop_filters; iter++)
     {
       FTM_MSG_3(FTM_HIGH, "tech_bw: %d, filt_len: %d, grp_delay: %d", droop_filter_tech_bw[iter], droop_filter_len[iter], droop_filter_grp_delay[iter]);
       for (iter2 = 0; iter2<droop_filter_len[iter]; iter2++)
       {
         FTM_MSG_2(FTM_HIGH, "coeff[%d]: %d", iter2, droop_filter_coeffs[iter][iter2]);
       }
     }
   }

   if (debug_mode_en != 0)
   {
     FTM_MSG_2(FTM_MED, " Inited Result: FBRxGainState: %d, NumFilters: %d,  ", 
               droop_result->fbrx_gain_state, 
               droop_result->num_filters );
     for (iter = 0; iter<MAX_NUM_DROOP_FILTERS_GEN_PER_SWP; iter++)
     {
       FTM_MSG_4(FTM_MED,  " Filter #%d, FBRxTechBWType: %d, GroupDelay: %d, NumFilterTaps: %d ", 
                  iter, 
                  droop_result->fbrx_droop_res[iter].fbrx_tech_bw_type,
                  droop_result->fbrx_droop_res[iter].group_delay,
                  droop_result->fbrx_droop_res[iter].num_filter_taps);
       for (iter2 = 0; iter2<FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DROOP_FILTER_LEN_MAX; iter2++)
       {
         FTM_MSG_2(FTM_MED, "        Coeff[%2d]  :   %d     ", 
                   iter2,
                   droop_result->fbrx_droop_res[iter].fbrx_fir_tap_coeffs_nv[iter2]);
       }
     }   
   }

   if (do_processing == 1)
   {
     // fill up with processing results
     droop_result->fbrx_gain_state = fbrx_gain_state;
     droop_result->num_filters = num_droop_filters;
     for (iter = 0; iter < num_captures; iter++)
     {
       droop_result->stg_tone_pwrs[iter] = stg_raw_tone_pwrs[iter];
     }
     for (iter = 0; iter<num_droop_filters; iter++)
     {
       droop_result->fbrx_droop_res[iter].fbrx_tech_bw_type = droop_filter_tech_bw[iter];
       droop_result->fbrx_droop_res[iter].group_delay = droop_filter_grp_delay[iter];
       if (cal_method == FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DROOP_STG_REAL_COEFFS)
       {
         droop_result->fbrx_droop_res[iter].num_filter_taps = EQ_LEN;
       }
       else
       {
         droop_result->fbrx_droop_res[iter].num_filter_taps = droop_filter_len[iter];
       }
       for (iter2 = 0; iter2<droop_result->fbrx_droop_res[iter].num_filter_taps; iter2++)
       {
         droop_result->fbrx_droop_res[iter].fbrx_fir_tap_coeffs_nv[iter2] = droop_filter_coeffs[iter][iter2];
       }
     }
     if (debug_mode_en != 0)
     {
       FTM_MSG_2(FTM_MED, " Inited Result: FBRxGainState: %d, NumFilters: %d,  ", 
                 droop_result->fbrx_gain_state, 
                 droop_result->num_filters );
       for (iter = 0; iter<MAX_NUM_DROOP_FILTERS_GEN_PER_SWP; iter++)
       {
         FTM_MSG_4(FTM_MED,  " Filter #%d, FBRxTechBWType: %d, GroupDelay: %d, NumFilterTaps: %d ", 
                    iter, 
                    droop_result->fbrx_droop_res[iter].fbrx_tech_bw_type,
                    droop_result->fbrx_droop_res[iter].group_delay,
                    droop_result->fbrx_droop_res[iter].num_filter_taps);
         for (iter2 = 0; iter2<FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DROOP_FILTER_LEN_MAX; iter2++)
         {
           FTM_MSG_2(FTM_MED, "        Coeff[%2d]  :   %d     ", 
                     iter2,
                     droop_result->fbrx_droop_res[iter].fbrx_fir_tap_coeffs_nv[iter2]);
         }
       }   
     }


     for ( iter2 = 0; iter2 < num_droop_filters; iter2++ )
     {
       if (debug_mode_en != 0)
       {
         nv_read_write_error = rfcommon_fbrx_read_current_droop_globals( ftm_get_rf_technology(), 
                                                                         band_union,
                                                                         (rfcommon_fbrx_tx_cfg_type)droop_result->fbrx_droop_res[iter2].fbrx_tech_bw_type, 
                                                                         droop_data_nv );
         if (nv_read_write_error!= RFCOMMON_FBRX_SUCCESS)
         {
           FTM_MSG_2(FTM_ERROR, "rfcommon_fbrx_read_current_droop_globals() returned error %d for tx cfg %d", nv_read_write_error, droop_result->fbrx_droop_res[iter2].fbrx_tech_bw_type );
           modem_mem_free((void*)droop_data_nv, MODEM_MEM_CLIENT_RFA);
           internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_NV_ACTION_FAILED;
           return FALSE;
         }

         FTM_MSG_1(FTM_MED, "Current Droop NV data for Tx BW %d", droop_result->fbrx_droop_res[iter2].fbrx_tech_bw_type );
         FTM_MSG_3(FTM_MED, "Droop Filter: NV_ver: %d , Group Delay %d, Number of Taps: %d", droop_data_nv->nv_version, droop_data_nv->group_delay, droop_data_nv->num_filter_taps );
         for (iter = 0; iter < RFCOMMON_FBRX_NUM_FILTER_TAPS; iter++)
         {
           FTM_MSG_2(FTM_MED, "Filter Coeff %d : %d", iter, droop_data_nv->filter_coeff[iter]);
         }
       }

       droop_data_nv->nv_version = 0;
       droop_data_nv->group_delay = droop_result->fbrx_droop_res[iter2].group_delay;
       droop_data_nv->num_filter_taps = droop_result->fbrx_droop_res[iter2].num_filter_taps; // need to find this out
       memset((void*)&(droop_data_nv->filter_coeff), 0, sizeof(int32)*RFCOMMON_FBRX_NUM_FILTER_TAPS);
       memscpy((void*)&(droop_data_nv->filter_coeff), sizeof(int32)*droop_result->fbrx_droop_res[iter2].num_filter_taps,
               (void*)&(droop_result->fbrx_droop_res[iter2].fbrx_fir_tap_coeffs_nv[0]), sizeof(int32)*droop_result->fbrx_droop_res[iter2].num_filter_taps) ; 

       nv_read_write_error = rfcommon_fbrx_override_droop_globals( ftm_get_rf_technology(), 
                                                                   band_union,
                                                                   (rfcommon_fbrx_tx_cfg_type)droop_result->fbrx_droop_res[iter2].fbrx_tech_bw_type, 
                                                                   droop_data_nv);

       if (nv_read_write_error!= RFCOMMON_FBRX_SUCCESS)
       {
         FTM_MSG_2(FTM_ERROR, "rfcommon_fbrx_override_droop_globals() returned error %d for tx cfg %d", nv_read_write_error, droop_result->fbrx_droop_res[iter2].fbrx_tech_bw_type );
         modem_mem_free((void*)droop_data_nv, MODEM_MEM_CLIENT_RFA);
         internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_NV_ACTION_FAILED;
         return FALSE;
       }

       /* Should clear out droop_data_nv before reading again, otherwise previous value to update prints again */
       if (debug_mode_en != 0)
       {
         nv_read_write_error = rfcommon_fbrx_read_current_droop_globals( ftm_get_rf_technology(),
                                                                         band_union, 
                                                                         (rfcommon_fbrx_tx_cfg_type)droop_result->fbrx_droop_res[iter2].fbrx_tech_bw_type, 
                                                                         droop_data_nv);

         if (nv_read_write_error!= RFCOMMON_FBRX_SUCCESS)
         {
           FTM_MSG_2(FTM_ERROR, "rfcommon_fbrx_read_current_droop_globals() returned error %d for tx cfg %d", nv_read_write_error, droop_result->fbrx_droop_res[iter2].fbrx_tech_bw_type );
           modem_mem_free((void*)droop_data_nv, MODEM_MEM_CLIENT_RFA);
           internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_NV_ACTION_FAILED;
           return FALSE;
         }

         FTM_MSG_1(FTM_MED, "Overridden Droop NV data for Tx BW %d", droop_result->fbrx_droop_res[iter2].fbrx_tech_bw_type);
         FTM_MSG_3(FTM_MED, "Droop Filter: NV_ver: %d , Group Delay %d, Number of Taps: %d", droop_data_nv->nv_version, droop_data_nv->group_delay, droop_data_nv->num_filter_taps );
         for (iter = 0; iter < RFCOMMON_FBRX_NUM_FILTER_TAPS; iter++)
         {
           FTM_MSG_2(FTM_MED, "Filter Coeff %d : %d", iter, droop_data_nv->filter_coeff[iter]);
         }

       }

     }
   }
   /*de-allocate memory*/
   modem_mem_free((void*)droop_data_nv, MODEM_MEM_CLIENT_RFA);

   return TRUE;

}


ftm_enh_internal_device_cal_error_code_type 
ftm_enh_internal_device_cal_droop_stg_process(
   ftm_enh_internal_device_cal_fbrx_droop_cal_method_type alg_type, 
   uint8 num_captures, 
   int32 * stg_offset_khz, 
   uint8 * samp_buffers,
   uint16 num_samps
   )
{
  ftm_enh_internal_device_cal_data_type *internal_device_cal_data;
  uint8 fbrx_bw = RFCOMMON_FBRX_LTE_10;
  uint8 iter = 0;
  uint16 iter2 = 0;
  uint8 iter3 = 0;
  int32 samp_freq_hz = 17200000;   /* TODO: MUST GET THIS FROM RXLM BUFFER */
  uint8 freq_ctr = 0;
  int32 stg_offsets[3] = {0};
  uint8 samp_bufs[3] = {-1};
  ftm_enh_internal_device_cal_error_code_type ret_val = FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS; 
  uint8 frac_bits_M[2] = {0};
  int16 magic_matrix[2][3][EQ_LEN] = { {{0}, {0}, {0}}, {{0}, {0}, {0}} };
  uint8 debug_mode_en;
  boolean api_status = TRUE;
  int32 * rx_samps;
  int32 * tx_samps;
  uint32 rx_samp_pcfl20;
  uint8 exponent;
  int32 rxI;
  int32 rxQ;
  int16 rxI_FP16;
  int16 rxQ_FP16;
  uint8 overflow_det;
  double cplxExpI_dbl;
  double cplxExpQ_dbl;
  int16 cplxExpI_FP16;
  int16 cplxExpQ_FP16;
  double freqOffset;
  int32 mult1;
  int32 mult2;
  int32 multSumI_FP32;
  int32 multSumQ_FP32;
  int16 multSumI_FP16;
  int16 multSumQ_FP16;
  int32 sumMultI_FP32;
  int32 sumMultQ_FP32;
  int16 sumMultScaledI_FP16;
  int16 sumMultScaledQ_FP16;
  int32 tonePower_FP32;
  int16 tonePowerScaled_FP16;
  int32 dArray[2] = {0};
  int16 dRef = 0;
  int8 dCtr = -1;
  int32 coeffsMult_FP32[EQ_LEN];
  int16 coeffsMult_FP16[EQ_LEN];
  int16 coeffsSum[2][EQ_LEN] = { {0}, {0} };
  int32 coeffsSum_for_Norm[2] = {0, 0};
  int32 coeffsFinalSum[2] = {0,0};
 
  uint8 peak_loc = -1;
  int16 peak_val = 0x8000;
  int32 * temprxsamp;

  internal_device_cal_data = ftm_common_data_get_enh_internal_device_cal_ptr();

  debug_mode_en = internal_device_cal_data->internal_device_cal_config.dbg_mode_en;
  fbrx_bw = internal_device_cal_data->internal_device_cal_config.bw;

  if (fbrx_bw >= RFCOMMON_FBRX_NUM_TX_CONFIG_MAX )
  {
    FTM_MSG_1(FTM_ERROR, "Unknown FBRx Tech/BW combination %d", fbrx_bw);
    internal_device_cal_data->internal_device_cal_status.error_log = FTM_ENH_INTERNAL_DEVICE_CAL_INVALID_TECHBW;
    return FALSE;
  }

  // For LTE15, need to perform LTE20 captures, different processing
  // For LTE5,  need to perform LTE10 captures, different processing
  // For LTE1p4/3, need to perform LTE10 captures, reuse processing results for LTE5
  switch (fbrx_bw)
  {
  case RFCOMMON_FBRX_LTE_15:
    fbrx_bw = RFCOMMON_FBRX_LTE_20;
    break;
  case RFCOMMON_FBRX_LTE_1P4:
  case RFCOMMON_FBRX_LTE_3:
  case RFCOMMON_FBRX_LTE_5:
    fbrx_bw = RFCOMMON_FBRX_LTE_10;
    break;
  default:
    break;
  }

  if (alg_type != FTM_ENH_INTERNAL_DEVICE_CAL_FBRX_DROOP_STG_REAL_COEFFS)
  {
    FTM_MSG(FTM_ERROR, "Droop Filter Calculation using STG currently supports only Real Coefficients!");
    return FTM_ENH_INTERNAL_DEVICE_CAL_UNSUPPORTED_ACTION;
  }

  if ( (fbrx_bw == RFCOMMON_FBRX_LTE_10) || (fbrx_bw == RFCOMMON_FBRX_LTE_20) )
  {
    num_droop_filters = 2;
    if (fbrx_bw == RFCOMMON_FBRX_LTE_10)
    {
      frac_bits_M[0] = fracBits_LTE10;
      frac_bits_M[1] = fracBits_LTE5;
      droop_filter_tech_bw[0] = RFCOMMON_FBRX_LTE_10;
      droop_filter_tech_bw[1] = RFCOMMON_FBRX_LTE_5;
      memscpy((void*)&magic_matrix[0][0][0], sizeof(M_LTE10), (void*)&M_LTE10, sizeof(M_LTE10));
      memscpy((void*)&magic_matrix[1][0][0], sizeof(M_LTE5), (void*)&M_LTE5, sizeof(M_LTE5));

      for (iter = 0; iter < num_captures; iter++ )
      {
        if (stg_offset_khz[iter] == 500)
        {
          stg_offsets[0] = 500000;
          samp_bufs[0] = samp_buffers[iter];
          freq_ctr++;
        }
        if (stg_offset_khz[iter] == 2000)
        {
          stg_offsets[1] = 2000000;
          samp_bufs[1] = samp_buffers[iter];
          freq_ctr++;
        }
        if (stg_offset_khz[iter] == 4500)
        {
          stg_offsets[2] = 4500000;
          samp_bufs[2] = samp_buffers[iter];
          freq_ctr++;
        }
      }

      if (freq_ctr < 3)
      {
        FTM_MSG_1(FTM_ERROR, "Insufficient set size of STG frequency offsets, %d, need 3 offsets at 0.5k, 2M, 4.5M required", freq_ctr);
        return FTM_ENH_INTERNAL_DEVICE_CAL_INSUFFICIENT_PARAMS;
      }

    }
    else if (fbrx_bw == RFCOMMON_FBRX_LTE_20)
    {
      samp_freq_hz = samp_freq_hz*2;   /* TODO: MUST GET THIS FROM RXLM BUFFER */
      frac_bits_M[0] = fracBits_LTE20;
      frac_bits_M[1] = fracBits_LTE15;
      droop_filter_tech_bw[0] = RFCOMMON_FBRX_LTE_20;
      droop_filter_tech_bw[1] = RFCOMMON_FBRX_LTE_15;
      memscpy((void*)&magic_matrix[0][0][0], sizeof(M_LTE20), (void*)&M_LTE20, sizeof(M_LTE20));
      memscpy((void*)&magic_matrix[1][0][0], sizeof(M_LTE15), (void*)&M_LTE15, sizeof(M_LTE15));

      for (iter = 0; iter < num_captures; iter++ )
      {
        if (stg_offset_khz[iter] == 500)
        {
          stg_offsets[0] = 500000;
          samp_bufs[0] = samp_buffers[iter];
          freq_ctr++;
        }
        if (stg_offset_khz[iter] == 4000)
        {
          stg_offsets[1] = 4000000;
          samp_bufs[1] = samp_buffers[iter];
          freq_ctr++;
        }
        if (stg_offset_khz[iter] == 9000)
        {
          stg_offsets[2] = 9000000;
          samp_bufs[2] = samp_buffers[iter];
          freq_ctr++;
        }
      }

      if (freq_ctr < 3)
      {
        FTM_MSG_1(FTM_ERROR, "Insufficient set size of STG frequency offsets, %d, 3 offsets at 0.5k, 4M, 9M required", freq_ctr);
        return FTM_ENH_INTERNAL_DEVICE_CAL_INSUFFICIENT_PARAMS;
      }

    }
  }
  else if ( (fbrx_bw == RFCOMMON_FBRX_UMTS_DC) || (fbrx_bw == RFCOMMON_FBRX_MCDO_7X) )
  {
    num_droop_filters = 1;
    droop_filter_tech_bw[0] = fbrx_bw;
    frac_bits_M[0] = fracBits_LTE10;    // UMTS_DC and MCDO_7X can reuse same results as LTE10
    memscpy((void*)&magic_matrix[0][0][0], sizeof(M_LTE10), (void*)&M_LTE10, sizeof(M_LTE10));

    for (iter = 0; iter < num_captures; iter++ )
    {
      if (stg_offset_khz[iter] == 500)
      {
        stg_offsets[0] = 500000;
        samp_bufs[0] = samp_buffers[iter];
        freq_ctr++;
      }
      if (stg_offset_khz[iter] == 2000)
      {
        stg_offsets[1] = 2000000;
        samp_bufs[1] = samp_buffers[iter];
        freq_ctr++;
      }
      if (stg_offset_khz[iter] == 4500)
      {
        stg_offsets[2] = 4500000;
        samp_bufs[2] = samp_buffers[iter];
        freq_ctr++;
      }
    }

    if (freq_ctr < 3)
    {
      FTM_MSG_1(FTM_ERROR, "Insufficient set size of STG frequency offsets, %d, need 3 offsets at 0.5k, 2M, 4.5M required", freq_ctr);
      return FTM_ENH_INTERNAL_DEVICE_CAL_INSUFFICIENT_PARAMS;
    }    
  }
  else if ( (fbrx_bw == RFCOMMON_FBRX_1X) || 
            (fbrx_bw == RFCOMMON_FBRX_MCDO_3X) || 
            (fbrx_bw == RFCOMMON_FBRX_UMTS_SC) || 
            (fbrx_bw == RFCOMMON_FBRX_TDS) ||
            (fbrx_bw == RFCOMMON_FBRX_SB_SVDO)
          )  // 1X, MCDO_3X, UMTS_SC, TDS, SB_SVDO can reuse LTE5 results
  {
    num_droop_filters = 1;
    droop_filter_tech_bw[0] = fbrx_bw;
    frac_bits_M[0] = fracBits_LTE5;
    memscpy((void*)&magic_matrix[0][0][0], sizeof(M_LTE5), (void*)&M_LTE5, sizeof(M_LTE5));

    for (iter = 0; iter < num_captures; iter++ )
    {
      if (stg_offset_khz[iter] == 500)
      {
        stg_offsets[0] = 500000;
        samp_bufs[0] = samp_buffers[iter];
        freq_ctr++;
      }
      if (stg_offset_khz[iter] == 2000)
      {
        stg_offsets[1] = 2000000;
        samp_bufs[1] = samp_buffers[iter];
        freq_ctr++;
      }
      if (stg_offset_khz[iter] == 4500)
      {
        stg_offsets[2] = 4500000;
        samp_bufs[2] = samp_buffers[iter];
        freq_ctr++;
      }
    }

    if (freq_ctr < 3)
    {
      FTM_MSG_1(FTM_ERROR, "Insufficient set size of STG frequency offsets, %d, need 3 offsets at 0.5k, 2M, 4.5M required", freq_ctr);
      return FTM_ENH_INTERNAL_DEVICE_CAL_INSUFFICIENT_PARAMS;
    }    
  }
  else
  {
    num_droop_filters = 1;
    droop_filter_tech_bw[0] = fbrx_bw;
    FTM_MSG_1(FTM_ERROR, "Unsupported Calibration for Tech/BW mode %d", fbrx_bw);
    return FTM_ENH_INTERNAL_DEVICE_CAL_UNSUPPORTED_ACTION;
  }

  if (debug_mode_en != 0)
  {
    for (iter = 0; iter < num_droop_filters; iter++)
    {
      for (iter2 = 0; iter2 < 3; iter2++)
      {
        FTM_MSG_1(FTM_MED, "--------  Row     %d   ---------", iter2);
        FTM_MSG_3(FTM_MED, "stg_offset %d samp buf id %d for freq index %d ", stg_offsets[iter2], samp_bufs[iter2], iter2);
        for (iter3 = 0; iter3 < EQ_LEN; iter3++)
        {
          FTM_MSG_1(FTM_MED, "    %10d ", magic_matrix[iter][iter2][iter3]);
        }
      }
    }
  }

  //rx_offset = 0;
  for (iter = 0; iter < freq_ctr; iter++)
  {
    api_status = rfcommon_mdsp_fetch_selftest_iq_data(samp_bufs[iter], num_samps, &rx_samps, &tx_samps);
    if (api_status == FALSE)
    {
      FTM_MSG_1(FTM_ERROR, "rfcommon_mdsp_fetch_selftest_iq_data failed with status %d", api_status);
      return FTM_ENH_INTERNAL_DEVICE_CAL_FETCH_SAMPLES_FAILED;
    }
    temprxsamp = rx_samps;
    freqOffset = ((double)stg_offsets[iter]*9216)/samp_freq_hz;
    if (debug_mode_en != 0)
    {
      FTM_MSG_4(FTM_MED, "---------- Rx Samples for Capture %d, Freq %d, num_samps %d, freqs %d -----------", iter, stg_offsets[iter], num_samps, freq_ctr );
    }
    sumMultI_FP32 = 0;
    sumMultQ_FP32 = 0;
    for (iter2 = 0; iter2 < num_samps; iter2++)
    {
      cplxExpI_dbl =  floor(cos((-2*PI*freqOffset*iter2)/num_samps)*pow(2,KERNEL_FRAC_RES));
      cplxExpQ_dbl =  floor(sin((-2*PI*freqOffset*iter2)/num_samps)*pow(2,KERNEL_FRAC_RES));
      cplxExpI_FP16 = (int16)cplxExpI_dbl;
      cplxExpQ_FP16 = (int16)cplxExpQ_dbl;

      if ( (iter2 <20) &  (debug_mode_en != 0))
      {
        FTM_MSG_3(FTM_MED, "~~~~~~~~~~~~ KERNEL %d : %16d   + j *   %16d ", iter2 , cplxExpI_FP16, cplxExpQ_FP16 );
      }
      if ( (iter2 > 9206) &  (debug_mode_en != 0))
      {
        FTM_MSG_3(FTM_MED, "~~~~~~~~~~~~ KERNEL %d : %16d   + j *   %16d ", iter2 , cplxExpI_FP16, cplxExpQ_FP16 );
      }
      if ( (iter2 <20) &  (debug_mode_en != 0))
      {
        FTM_MSG_1(FTM_MED, "Raw Samp 0x%8x", *(temprxsamp +iter2 ) );
      }
      if ( (iter2 > 9206) &  (debug_mode_en != 0))
      {
        FTM_MSG_1(FTM_MED, "Raw Samp 0x%8x", *(temprxsamp +iter2 ) );
      }

      if (iter2%8 == 0)
      {
        rx_samp_pcfl20 = (((uint32)*rx_samps) >> 0) & 0xFFFFF;
      }
      else if (iter2%8 == 1)
      {
        rx_samp_pcfl20 = ( ( ((uint32)*rx_samps) >> 20 )  +  ( ( (uint32)*(rx_samps + 1) ) << 12 )) & 0xFFFFF ;
      }
      else if (iter2%8 == 2)
      {
        rx_samp_pcfl20 = ( ((uint32)*(rx_samps+1) ) >> 8) & 0xFFFFF;
      }
      else if (iter2%8 == 3)
      {
        rx_samp_pcfl20 = ( ( ( (uint32)*( rx_samps + 1 ) ) >> 28 ) + ( ( (uint32) *( rx_samps + 2 ) ) << 4 ) ) & 0xFFFFF;
      }
      else if (iter2%8 == 4)
      {
        rx_samp_pcfl20 = ( ( ( (uint32) *( rx_samps + 2 ) ) >> 16 ) + (( (uint32) *( rx_samps + 3 ) ) << 16 ) ) & 0xFFFFF;
      }
      else if (iter2%8 == 5)
      {
        rx_samp_pcfl20 = (((uint32)*(rx_samps+3)) >> 4) & 0xFFFFF;
      }
      else if (iter2%8 == 6)
      {
        rx_samp_pcfl20 = ( ( ((uint32) *( rx_samps + 3 ) ) >> 24 ) + ( ( (uint32) *( rx_samps + 4 ) ) << 8 )) & 0xFFFFF;
      }
      else if (iter2%8 == 7)
      {
        rx_samp_pcfl20 = (((uint32)*(rx_samps+4)) >> 12) & 0xFFFFF;
        rx_samps = rx_samps + 5;
      }

      exponent = (uint8)((rx_samp_pcfl20>>16) & 0xFF);
      rxI = (int32)(((int8)((rx_samp_pcfl20>>8) & 0xFF)) << (15-exponent)); 
      rxQ = (int32)(((int8)((rx_samp_pcfl20>>0) & 0xFF)) << (15-exponent)); 

      rxI = rxI>>5;
      rxQ = rxQ>>5;

      rxI_FP16 = sat_int32_to_int16(rxI, &overflow_det, debug_mode_en);
      if (overflow_det == 1)
      {
        return FTM_ENH_INTERNAL_DEVICE_CAL_ARITHMETIC_OVERFLOW;
      }
      rxQ_FP16 = sat_int32_to_int16(rxQ, &overflow_det, debug_mode_en);
      if (overflow_det == 1)
      {
        return FTM_ENH_INTERNAL_DEVICE_CAL_ARITHMETIC_OVERFLOW;
      }

      if ( (iter2 <20) &  (debug_mode_en != 0))
      {
        FTM_MSG_6(FTM_MED, "samp ind %4d    =  %16d  +  j *  %16d   ,  %16d + j * %16d, overflow_det = %d", iter2, rxI, rxQ, rxI_FP16, rxQ_FP16, overflow_det);
      }
      if ( (iter2 > 9206) &  (debug_mode_en != 0))
      {
        FTM_MSG_6(FTM_MED, "samp ind %4d    =  %16d  +  j *  %16d   ,  %16d + j * %16d, overflow_det = %d", iter2, rxI, rxQ, rxI_FP16, rxQ_FP16, overflow_det);
      }

      mult1 = (rxI_FP16*cplxExpI_FP16);
      mult2 = (rxQ_FP16*cplxExpQ_FP16);
      multSumI_FP32 = (sat_add_int32( mult1, -mult2, &overflow_det )) >> 13;
      if (overflow_det == 1)
      {
        return FTM_ENH_INTERNAL_DEVICE_CAL_ARITHMETIC_OVERFLOW;
      }
      multSumI_FP16 = sat_int32_to_int16(multSumI_FP32, &overflow_det, debug_mode_en);
      if (overflow_det == 1)
      {
        return FTM_ENH_INTERNAL_DEVICE_CAL_ARITHMETIC_OVERFLOW;
      }

      mult1 = (rxQ_FP16*cplxExpI_FP16);
      mult2 = (rxI_FP16*cplxExpQ_FP16);
      multSumQ_FP32 = (sat_add_int32( mult1, mult2, &overflow_det )) >> 13;
      if (overflow_det == 1)
      {
        return FTM_ENH_INTERNAL_DEVICE_CAL_ARITHMETIC_OVERFLOW;
      }
      multSumQ_FP16 = sat_int32_to_int16(multSumQ_FP32, &overflow_det, debug_mode_en);
      if (overflow_det == 1)
      {
        return FTM_ENH_INTERNAL_DEVICE_CAL_ARITHMETIC_OVERFLOW;
      }

      if ( (iter2 <20) &  (debug_mode_en != 0))
      {
        FTM_MSG_6(FTM_MED, "MULT RES IND %4d   FP_32  =  %16d  +  j *  %16d,  FP_16   =  %16d  +  j *  %16d, overflow_det = %d", iter2, multSumI_FP32, multSumQ_FP32, multSumI_FP16, multSumQ_FP16, overflow_det );
      }
      if ( (iter2 > 9206) &  (debug_mode_en != 0))
      {
        FTM_MSG_6(FTM_MED, "MULT RES IND %4d   FP_32  =  %16d  +  j *  %16d,  FP_16   =  %16d  +  j *  %16d, overflow_det = %d", iter2, multSumI_FP32, multSumQ_FP32, multSumI_FP16, multSumQ_FP16, overflow_det );
      }

      sumMultI_FP32 = sat_add_int32( sumMultI_FP32, (int32)multSumI_FP16, &overflow_det );
      if (overflow_det == 1)
      {
        return FTM_ENH_INTERNAL_DEVICE_CAL_ARITHMETIC_OVERFLOW;
      }

      sumMultQ_FP32 = sat_add_int32( sumMultQ_FP32, (int32)multSumQ_FP16, &overflow_det );
      if (overflow_det == 1)
      {
        return FTM_ENH_INTERNAL_DEVICE_CAL_ARITHMETIC_OVERFLOW;
      }
    }

    sumMultScaledI_FP16 = sat_int32_to_int16( sumMultI_FP32 >> (10+4), &overflow_det, debug_mode_en);
    if (overflow_det == 1)
    {
      return FTM_ENH_INTERNAL_DEVICE_CAL_ARITHMETIC_OVERFLOW;
    }
    sumMultScaledQ_FP16 = sat_int32_to_int16( sumMultQ_FP32 >> (10+4), &overflow_det, debug_mode_en);
    if (overflow_det == 1)
    {
      return FTM_ENH_INTERNAL_DEVICE_CAL_ARITHMETIC_OVERFLOW;
    }

    if (debug_mode_en != 0)
    {
      FTM_MSG_5(FTM_MED, "Freq %d   Sum Mult FP32:  %16d  +   j  *  %16d , Sum Mult Scaled FP16:   %16d + j * %16d", iter, sumMultI_FP32, sumMultQ_FP32, sumMultScaledI_FP16, sumMultScaledQ_FP16 );
    }

    tonePower_FP32 = sat_add_int32( (sumMultScaledI_FP16*sumMultScaledI_FP16) , (sumMultScaledQ_FP16*sumMultScaledQ_FP16), &overflow_det );
    if (overflow_det == 1)
    {
      return FTM_ENH_INTERNAL_DEVICE_CAL_ARITHMETIC_OVERFLOW;
    }

    tonePowerScaled_FP16 = (int16)(tonePower_FP32 >> 15);
    if (debug_mode_en != 0)
    {
      FTM_MSG_2(FTM_MED, "tonePower_FP32: %d, tonePowerScaled_FP16: %d", tonePower_FP32, tonePowerScaled_FP16);
    }
    if (dCtr == -1)
    {
      dRef = tonePowerScaled_FP16;
    }

    if (dCtr>=0)
    {
      tonePower_FP32 = (int32)((tonePowerScaled_FP16 << 15) & 0xFFFF8000);
      dArray[dCtr] = sat_int32_to_int16( (int32) ((int32)tonePower_FP32 /(int32)dRef ), &overflow_det, debug_mode_en);
      if (overflow_det == 1)
      {
        return FTM_ENH_INTERNAL_DEVICE_CAL_ARITHMETIC_OVERFLOW;
      }
      if (debug_mode_en != 0)
      {
        FTM_MSG_4(FTM_MED, "relative calc - tonePower_FP32: %d, dRef: %d, D[%d]: %d", tonePower_FP32, dRef, dCtr, dArray[dCtr]);
      }
    }

    for (iter2 = 0; iter2 < num_droop_filters; iter2++)
    {
      if (debug_mode_en != 0)
      {
        FTM_MSG_1( FTM_MED, " << Coeff Calculation for Filter %d >> ", iter2  );
      }
      if ( dCtr == -1 )
      {
        for (iter3 = 0; iter3 < EQ_LEN; iter3++)
        {
          if ( (15 - frac_bits_M[iter2] - 3) >=0 )
          {
            coeffsMult_FP32[iter3] = (int32) ( magic_matrix[iter2][0][iter3] << (15 - frac_bits_M[iter2] - 3) ) ;
          }
          else
          {
            coeffsMult_FP32[iter3] = (int32) ( magic_matrix[iter2][0][iter3] >> (frac_bits_M[iter2] + 3 - 15 ) ) ;
          }
          coeffsMult_FP16[iter3] = sat_int32_to_int16(coeffsMult_FP32[iter3], &overflow_det, debug_mode_en);
          if (overflow_det == 1)
          {
            return FTM_ENH_INTERNAL_DEVICE_CAL_ARITHMETIC_OVERFLOW;
          }
          coeffsSum[iter2][iter3] = sat_add_int16(coeffsSum[iter2][iter3], coeffsMult_FP16[iter3] , &overflow_det );
          if (overflow_det == 1)
          {
            return FTM_ENH_INTERNAL_DEVICE_CAL_ARITHMETIC_OVERFLOW;
          }
          if (debug_mode_en != 0)
          {
            FTM_MSG_3( FTM_MED, " Coeff Sum idx %d - FP32: %d ,  FP16:  %d ", iter3, coeffsMult_FP32[iter3], coeffsMult_FP16[iter3] );
          }
        }
      }
      else
      {
        for (iter3 = 0; iter3 < EQ_LEN; iter3++)
        {
          coeffsMult_FP32[iter3] = ( magic_matrix[iter2][dCtr+1][iter3] * dArray[dCtr] ) >>  (frac_bits_M[iter2] + 3);
          coeffsMult_FP16[iter3] = sat_int32_to_int16(coeffsMult_FP32[iter3], &overflow_det, debug_mode_en);
          if (overflow_det == 1)
          {
            return FTM_ENH_INTERNAL_DEVICE_CAL_ARITHMETIC_OVERFLOW;
          }
          coeffsSum[iter2][iter3] = sat_add_int16(coeffsSum[iter2][iter3], coeffsMult_FP16[iter3], &overflow_det );
          if (overflow_det == 1)
          {
            return FTM_ENH_INTERNAL_DEVICE_CAL_ARITHMETIC_OVERFLOW;
          }
          if (debug_mode_en != 0)
          {
            FTM_MSG_3( FTM_MED, " Coeff Sum idx %d - FP32: %d ,  FP16:  %d ", iter3, coeffsMult_FP32[iter3], coeffsMult_FP16[iter3] );
          }
        }
      }
    }

    dCtr += 1;
  }

  FTM_MSG(FTM_MED, "---------------- Unnormalized Coefficients ----------------");
  for (iter2 = 0; iter2 < num_droop_filters; iter2++)
  {
    FTM_MSG_1(FTM_MED, "  << Filter  %d  >>", iter2);
    peak_val = 0x8000;
    for (iter3 = 0; iter3 < EQ_LEN; iter3++)
    {
      droop_filter_coeffs[iter2][iter3] = coeffsSum[iter2][iter3] << 2;
      coeffsSum_for_Norm[iter2] += droop_filter_coeffs[iter2][iter3];
      if (droop_filter_coeffs[iter2][iter3] > peak_val)
      {
        peak_val = droop_filter_coeffs[iter2][iter3];
        peak_loc = iter3;
      }
      FTM_MSG_2(FTM_MED, "tap %d : %d", iter3, droop_filter_coeffs[iter2][iter3]);
    }
    droop_filter_grp_delay[iter2] = peak_loc;
    FTM_MSG_1(FTM_MED, "group delay : %d", droop_filter_grp_delay[iter2]);
  }

  FTM_MSG(FTM_MED, "---------------- Normalized Coefficients ----------------");
  for (iter2 = 0; iter2 < num_droop_filters; iter2++)
  {
    FTM_MSG_1(FTM_MED, "  << Filter  %d  >>", iter2);
    for (iter3 = 0; iter3 < EQ_LEN; iter3++)
    {
      droop_filter_coeffs[iter2][iter3] = (droop_filter_coeffs[iter2][iter3] << 14) / coeffsSum_for_Norm[iter2];
      coeffsFinalSum[iter2] += droop_filter_coeffs[iter2][iter3];
      FTM_MSG_2(FTM_MED, "tap %d : %d", iter3, droop_filter_coeffs[iter2][iter3]);
    }
  }

  FTM_MSG(FTM_MED, "---------------- Filter Norm ----------------");
  for (iter2 = 0; iter2 < num_droop_filters; iter2++)
  {
    FTM_MSG_1(FTM_MED, "  << Filter  %d  >>", iter2);
    FTM_MSG_1(FTM_MED, "Norm  %d", coeffsFinalSum[iter2]);
  }

  // reuse LTE5 filter data generated from LTE10 captures for LTE1p4/3
  if (fbrx_bw == RFCOMMON_FBRX_LTE_10)
  {
    num_droop_filters = 4;
    droop_filter_tech_bw[2] = RFCOMMON_FBRX_LTE_3;
    droop_filter_tech_bw[3] = RFCOMMON_FBRX_LTE_1P4;
    droop_filter_grp_delay[2] = droop_filter_grp_delay[1];
    droop_filter_grp_delay[3] = droop_filter_grp_delay[1];
    memscpy((void*)&droop_filter_coeffs[2][0], sizeof(int32)*EQ_LEN, (void*)&droop_filter_coeffs[1][0], sizeof(int32)*EQ_LEN); // copy over LTE5 filter for LTE3
    memscpy((void*)&droop_filter_coeffs[3][0], sizeof(int32)*EQ_LEN, (void*)&droop_filter_coeffs[1][0], sizeof(int32)*EQ_LEN); // copy over LTE5 filter for LTE3
  }

  return ret_val;


}

ftm_enh_internal_device_cal_error_code_type 
ftm_enh_internal_device_cal_calc_tone_pwr(
   uint8 samp_buf_id, 
   uint16 num_iq_samples, 
   int32 stg_offset_khz, 
   uint32 samp_rate, 
   uint32* tonePwr
   )
{
  ftm_enh_internal_device_cal_error_code_type ret_val = FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS;
  uint8 debug_mode_en;
  ftm_enh_internal_device_cal_data_type *internal_device_cal_data;

  int32 fNorm;
  int64 fNorm_64;
  double fNorm_double;
  uint8 fracBitsFnorm = 29;
  uint8 fracBitsQPhase = 12;
  uint8 overflow_det;
  uint16 iter;
  boolean api_status = TRUE;
  int32 * rx_samps;
  int32 * tx_samps;
  uint32 rx_samp_pcfl20;
  uint32 qPhase1_32;
  uint16 qPhase1;
  uint16 qPhase2;
  uint16 phase1;
  uint16 phase2;
  int8 sign1;
  int8 sign2;
  int32 cos_val;
  int32 sin_val;
  uint8 exponent;
  int32 rxI;
  int32 rxQ;
  int16 kernelQsIm;
  int16 kernelQsRe;
  int32 multI;
  int32 multQ;
  int64 multI_64;
  int64 multQ_64;
  int32 sum_mI;
  int32 sum_mQ;
  int32 tone_cplx_amp_I;
  int32 tone_cplx_amp_Q;
  uint64 tone_pwr_64;
  uint64 tone_pwr_64_trunc;
  uint32 tone_pwr_32;

  #if ENABLE_DEBUG_KERNEL_DUMPS
  int16 kernelQsRe_arr[9216] = {0};
  int16 kernelQsIm_arr[9216] = {0};
  char file1[20]={'\0'};
  char file2[20]={'\0'};
  #endif

  #if ENABLE_DEBUG_MULTRES_DUMPS
  int32 multRe_arr[4096] = {0};
  int32 multIm_arr[4096] = {0};
  char file1[20]={'\0'};
  char file2[20]={'\0'};
  #endif


  internal_device_cal_data = ftm_common_data_get_enh_internal_device_cal_ptr();
  debug_mode_en = internal_device_cal_data->internal_device_cal_config.dbg_mode_en;

  if (debug_mode_en != 0)
  {
    FTM_MSG_3(FTM_HIGH, "stg_offset_khz: %d, samp_rate: %d, fracBitsFnorm: %d", stg_offset_khz, samp_rate, fracBitsFnorm);
  }

  fNorm_double = (double)((double)(8*stg_offset_khz*1000)/(double)samp_rate) + (double)(pow(2.0, -(fracBitsFnorm)));

  fNorm_64 = (int64)(fNorm_double*(double)pow(2.0,fracBitsFnorm));
  fNorm_64 = (fNorm_64>0)?fNorm_64:(fNorm_64-1);
  fNorm = sat_int64_to_int32(fNorm_64, &overflow_det);
  if (overflow_det == 1)
  {
    return FTM_ENH_INTERNAL_DEVICE_CAL_ARITHMETIC_OVERFLOW;
  }

  if (debug_mode_en != 0)
  {
    FTM_MSG(FTM_HIGH, "fNorm_64: --------------");
    print_int64hex(&fNorm_64);
    FTM_MSG(FTM_HIGH, "fNorm_double: ----------");
    print_int64hex((int64*)&fNorm_double);
    FTM_MSG_1(FTM_HIGH, "fNorm_32: 0x%08x", fNorm);
  }

  api_status = rfcommon_mdsp_fetch_selftest_iq_data(samp_buf_id, num_iq_samples, &rx_samps, &tx_samps);
  if (api_status == FALSE)
  {
    FTM_MSG_1(FTM_ERROR, "rfcommon_mdsp_fetch_selftest_iq_data failed with status %d", api_status);
    return FTM_ENH_INTERNAL_DEVICE_CAL_FETCH_SAMPLES_FAILED;
  }
  sum_mI = 0;
  sum_mQ = 0;
  for (iter = 0; iter < num_iq_samples; iter++)
  {
    if (iter%8 == 0)
    {
      rx_samp_pcfl20 = (((uint32)*rx_samps) >> 0) & 0xFFFFF;
    }
    else if (iter%8 == 1)
    {
      rx_samp_pcfl20 = ( ( ((uint32)*rx_samps) >> 20 )  +  ( ( (uint32)*(rx_samps + 1) ) << 12 )) & 0xFFFFF ;
    }
    else if (iter%8 == 2)
    {
      rx_samp_pcfl20 = ( ((uint32)*(rx_samps+1) ) >> 8) & 0xFFFFF;
    }
    else if (iter%8 == 3)
    {
      rx_samp_pcfl20 = ( ( ( (uint32)*( rx_samps + 1 ) ) >> 28 ) + ( ( (uint32) *( rx_samps + 2 ) ) << 4 ) ) & 0xFFFFF;
    }
    else if (iter%8 == 4)
    {
      rx_samp_pcfl20 = ( ( ( (uint32) *( rx_samps + 2 ) ) >> 16 ) + (( (uint32) *( rx_samps + 3 ) ) << 16 ) ) & 0xFFFFF;
    }
    else if (iter%8 == 5)
    {
      rx_samp_pcfl20 = (((uint32)*(rx_samps+3)) >> 4) & 0xFFFFF;
    }
    else if (iter%8 == 6)
    {
      rx_samp_pcfl20 = ( ( ((uint32) *( rx_samps + 3 ) ) >> 24 ) + ( ( (uint32) *( rx_samps + 4 ) ) << 8 )) & 0xFFFFF;
    }
    else if (iter%8 == 7)
    {
      rx_samp_pcfl20 = (((uint32)*(rx_samps+4)) >> 12) & 0xFFFFF;
      rx_samps = rx_samps + 5;
    }

    exponent = (uint8)((rx_samp_pcfl20>>16) & 0xFF);
    rxI = (int32)(((int8)((rx_samp_pcfl20>>8) & 0xFF)) << (15-exponent)); 
    rxQ = (int32)(((int8)((rx_samp_pcfl20>>0) & 0xFF)) << (15-exponent)); 

    qPhase1_32 = (uint32)((int64)(fNorm*iter) & 0xFFFFFFFF); // This assumes that fracBitsFnorm + log2(8) = 32 bits (0xFFFFFFFF)

    qPhase1 = (qPhase1_32 + (1 << (fracBitsFnorm - fracBitsQPhase -1))) >> (fracBitsFnorm - fracBitsQPhase);

    qPhase2 = (qPhase1+(2<<fracBitsQPhase)) & 0x7FFF; // This assumes that fracBitsQPhase + log2(8) = 15 bits (0x7FFF)

    if (qPhase1>=(6<<fracBitsQPhase))
    {
      phase1 = (8<<fracBitsQPhase) - qPhase1;
      sign1 = -1;
    }
    else if (qPhase1>=(4<<fracBitsQPhase))
    {
      phase1 = qPhase1 - (4<<fracBitsQPhase);
      sign1 = -1;
    }
    else if (qPhase1>=(2<<fracBitsQPhase))
    {
      phase1 = (4<<fracBitsQPhase) - qPhase1;
      sign1 = 1;
    }
    else
    {
      phase1 = qPhase1;
      sign1 = 1;
    }

    if (qPhase2>=(6<<fracBitsQPhase))
    {
      phase2 = (8<<fracBitsQPhase) - qPhase2;
      sign2 = -1;
    }
    else if (qPhase2>=(4<<fracBitsQPhase))
    {
      phase2 = qPhase2 - (4<<fracBitsQPhase);
      sign2 = -1;
    }
    else if (qPhase2>=(2<<fracBitsQPhase))
    {
      phase2 = (4<<fracBitsQPhase) - qPhase2;
      sign2 = 1;
    }
    else
    {
      phase2 = qPhase2;
      sign2 = 1;
    }
    sin_val = sign1 * sinApprox(phase1, fracBitsQPhase);
    cos_val = sign2 * sinApprox(phase2, fracBitsQPhase);

    kernelQsIm =  (int16)( ( ((((int32)(-sin_val)) & 0x80000001) == 0x80000001)?
                             ((((int32)(-sin_val)) - 1) >> 1) : ((((int32)(-sin_val)) + 1) >> 1)) & 0xFFFF); 


    kernelQsRe =  (int16)( ( ((((int32)(cos_val)) & 0x80000001) == 0x80000001)?
                             ((((int32)(cos_val)) - 1) >> 1) : ((((int32)(cos_val)) + 1) >> 1)) & 0xFFFF); 


    multI_64 = ((((int64)kernelQsRe)*((int64)rxI)) - (((int64)kernelQsIm)*((int64)rxQ)));
    multQ_64 = ((((int64)kernelQsRe)*((int64)rxQ)) + (((int64)kernelQsIm)*((int64)rxI)));


    multI = (int32)(( ((multI_64 & 0x800000000003FFFFULL ) == 0x8000000000020000ULL)   ?
              ( multI_64 - (1 << 17) ) >> 18  :
              ( multI_64 + (1 << 17) ) >> 18  ) & 0xFFFFFFFF);

    multQ = (int32)(( ((multQ_64 & 0x800000000003FFFFULL ) == 0x8000000000020000ULL)   ?
              ( multQ_64 - (1 << 17) ) >> 18  :
              ( multQ_64 + (1 << 17) ) >> 18  ) & 0xFFFFFFFF);

    if ( ( (iter <20) || (iter > 9206)) &  (debug_mode_en != 0))
    {
      FTM_MSG(FTM_HIGH, "multI_64: ---");
      print_int64hex(&multI_64);
      FTM_MSG(FTM_HIGH, "multQ_64: ---");
      print_int64hex(&multQ_64);
      FTM_MSG_2(FTM_HIGH, "multI: %d, multQ: %d", multI, multQ);
    }
    sum_mI += multI;
    sum_mQ += multQ;


    #if ENABLE_DEBUG_KERNEL_DUMPS
    kernelQsIm_arr[iter] = kernelQsIm;
    kernelQsRe_arr[iter] = kernelQsRe;
    #endif
    #if ENABLE_DEBUG_MULTRES_DUMPS
    if (iter<=4095)
    {
      multIm_arr[iter] = multI;
      multRe_arr[iter] = multQ;
    }
    #endif

    if ( ( (iter <20) || (iter > 9206)) &  (debug_mode_en != 0))
    {
      FTM_MSG_4(FTM_HIGH, "samp ind %4d    =  %16d  +  j *  %16d   ,   overflow_det = %d", iter, rxI, rxQ,overflow_det);
      FTM_MSG_6(FTM_HIGH, "sin_val : %d, cos_val: %d, kernRe: %d, kernIm: %d, multI: %d, multQ: %d" , sin_val,cos_val, kernelQsRe, kernelQsIm, multI, multQ);
    }
  }

  tone_cplx_amp_I =  ( ((sum_mI & 0x80000001) == 0x80000001)?
                     ( (sum_mI - 1) >> 1) : ( (sum_mI + 1) >> 1)) & 0xFFFFFFFF; 
  tone_cplx_amp_Q =  ( ((sum_mQ & 0x80000001) == 0x80000001)?
                     ( (sum_mQ - 1) >> 1) : ( (sum_mQ + 1) >> 1)) & 0xFFFFFFFF; 

  tone_pwr_64 = (uint64)(((int64)tone_cplx_amp_I)* ((int64)tone_cplx_amp_I))  
              + (uint64)(((int64)tone_cplx_amp_Q)* ((int64)tone_cplx_amp_Q)); 
  tone_pwr_64_trunc = (tone_pwr_64 + (1<<30))>>31;
  if (debug_mode_en != 0)
  {
    FTM_MSG(FTM_HIGH, "tone_pwr_64: --------" );
    print_int64hex((int64*)&tone_pwr_64);
    FTM_MSG(FTM_HIGH, "tone_pwr_64_trunc: --------" );
    print_int64hex((int64*)&tone_pwr_64_trunc);
  }
  tone_pwr_32 = sat_uint64_to_uint32(tone_pwr_64_trunc, &overflow_det);
  if (overflow_det == 1)
  {
    return FTM_ENH_INTERNAL_DEVICE_CAL_ARITHMETIC_OVERFLOW;
  }
  if (debug_mode_en != 0)
  {
    FTM_MSG_7(FTM_HIGH, "stg_freq: %d --> sum_mI: %d, sum_mQ: %d, tone_cplx_ampI: %d, tone_cplx_ampQ: %d, tone_pwr_64_trunc: %d, tone_pwr_32: %d",
            stg_offset_khz,sum_mI, sum_mQ,tone_cplx_amp_I, tone_cplx_amp_Q,
            tone_pwr_64_trunc, tone_pwr_32);
  }

  *tonePwr = tone_pwr_32;
  #if ENABLE_DEBUG_KERNEL_DUMPS
  if (debug_mode_en>=2)
  {
    snprintf(&file1[0],20,"iq_kim_%05d.bin", stg_offset_khz);
    snprintf(&file2[0],20,"iq_kre_%05d.bin", stg_offset_khz);
    //rfcommon_mdsp_save_kern_data2(&kernelQsIm_arr[0],num_iq_samples,&file1[0]);
    //rfcommon_mdsp_save_kern_data2(&kernelQsRe_arr[0],num_iq_samples,&file2[0]);
  }
  #endif
  #if ENABLE_DEBUG_MULTRES_DUMPS
  if (debug_mode_en>=2)
  {
    snprintf(&file1[0],20,"iq_mI_%05d.bin", stg_offset_khz);
    snprintf(&file2[0],20,"iq_mQ_%05d.bin", stg_offset_khz);
    //rfcommon_mdsp_save_kern_data32(&multIm_arr[0],4096,&file1[0]);
    //rfcommon_mdsp_save_kern_data32(&multRe_arr[0],4096,&file2[0]);
  }
  #endif

  return ret_val;
}

ftm_enh_internal_device_cal_error_code_type
ftm_enh_internal_device_cal_calc_nom_eq_gain_sq(
        uint16 * eqGainSq, 
        uint8 * numNomEq, 
        uint8 fbrx_bw, 
        int32 * stg_offset_khz,
        uint8 num_captures, 
        uint32 samp_rate,
        uint8 * filter_techbw
        )
{
  ftm_enh_internal_device_cal_error_code_type ret_val = FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS;
  uint8 iter = 0;
  uint8 filter_iter = 0;
  uint8 tap_ind = 0;
  double cValDbl;
  double sValDbl;
  int32 cVal_32;
  int32 sVal_32;
  int16 cVal;
  int16 sVal;
  int32 m1_32;
  int32 m2_32;
  int32 m1_32_acc;
  int32 m2_32_acc;
  uint64 m1_sq_64;
  uint64 m2_sq_64;
  uint16 m1_sq;
  uint16 m2_sq;
  int32 cqstatic_mult_c;
  int32 cqstatic_mult_s;
  uint8 overflow_det;
  uint8 debug_mode_en;

  int32 *nom_filter[4];
  uint8 num_taps[4];
  int32 temp_filter[32];

  ftm_enh_internal_device_cal_data_type *internal_device_cal_data;

  internal_device_cal_data = ftm_common_data_get_enh_internal_device_cal_ptr();
  debug_mode_en = internal_device_cal_data->internal_device_cal_config.dbg_mode_en;

  switch (fbrx_bw)
  {
  case RFCOMMON_FBRX_LTE_20:
  case RFCOMMON_FBRX_LTE_15:
	* numNomEq = 2;
    filter_techbw[0] = RFCOMMON_FBRX_LTE_20;
    filter_techbw[1] = RFCOMMON_FBRX_LTE_15;
    nom_filter[0] = &filter_LTE20[0];
    nom_filter[1] = &filter_LTE15[0];
    num_taps[0]    = filterlen_LTE20;
    num_taps[1]    = filterlen_LTE15;
    break;
  case RFCOMMON_FBRX_LTE_10:
  case RFCOMMON_FBRX_LTE_5:
  case RFCOMMON_FBRX_LTE_3:
  case RFCOMMON_FBRX_LTE_1P4:
    * numNomEq = 2;
    filter_techbw[0] = RFCOMMON_FBRX_LTE_10;
    filter_techbw[1] = RFCOMMON_FBRX_LTE_5;
    nom_filter[0] = &filter_LTE10[0];
    nom_filter[1] = &filter_LTE5[0];
    num_taps[0]    = filterlen_LTE10;
    num_taps[1]    = filterlen_LTE5;
    break;
  case RFCOMMON_FBRX_UMTS_DC:
  case RFCOMMON_FBRX_MCDO_7X:
  case RFCOMMON_FBRX_1X:
  case RFCOMMON_FBRX_MCDO_3X:
  case RFCOMMON_FBRX_UMTS_SC:
  case RFCOMMON_FBRX_TDS:
  case RFCOMMON_FBRX_SB_SVDO:
    * numNomEq = 1;
    filter_techbw[0] = fbrx_bw;
    nom_filter[0] = &filter_nonLTE[0];
    num_taps[0]    = filterlen_nonLTE;
    break;
  default:
    FTM_MSG_1(FTM_ERROR, "Unsupported FBRx tech/bandwidth type %d", fbrx_bw);
    return FTM_ENH_INTERNAL_DEVICE_CAL_UNSUPPORTED_TECH;
    break;
  }

  for (filter_iter = 0; filter_iter < * numNomEq; filter_iter++)
  {
    memscpy((void*)&temp_filter[0],sizeof(int32)*num_taps[filter_iter], (void*)nom_filter[filter_iter],sizeof(int32)*num_taps[filter_iter] );

    for (iter = 0; iter < num_captures; iter++)
    {
      m1_32_acc = 0;
      m2_32_acc = 0;
      for (tap_ind =0; tap_ind < num_taps[filter_iter]; tap_ind++)
      {
        if (( ( temp_filter[tap_ind] & 0xFFFF0000) != 0) && ( ( temp_filter[tap_ind] & 0xFFFF0000) != 0xFFFF0000))
        {
          FTM_MSG(FTM_ERROR, "Nominal Droop Filter from NV is complex, need nominal filter to be real!");
          return FTM_ENH_INTERNAL_DEVICE_CAL_UNSUPPORTED_ACTION;
        }
        cValDbl = cos( ( ((double)stg_offset_khz[iter]*1000) / ((double)samp_rate)) * 2 * PI * tap_ind ) * pow(2.0,16); 
        sValDbl = sin( ( ((double)stg_offset_khz[iter]*1000) / ((double)samp_rate)) * 2 * PI * tap_ind ) * pow(2.0,16); 
        cVal_32 = (int32)cValDbl;
        sVal_32 = (int32)sValDbl;

        cVal_32 =  (( cVal_32 & 0x80000001) == 0x80000001)? ( (cVal_32 - 1) >> 1) : ( (cVal_32 + 1) >> 1); 
        sVal_32 =  (( sVal_32 & 0x80000001) == 0x80000001)? ( (sVal_32 - 1) >> 1) : ( (sVal_32 + 1) >> 1); 

        cVal = sat_int32_to_int16(cVal_32, &overflow_det, debug_mode_en);
        sVal = sat_int32_to_int16(sVal_32, &overflow_det, debug_mode_en);

        cqstatic_mult_c = (int32)(cVal)*(int32)((int16)(temp_filter[tap_ind] & 0xFFFF));
        cqstatic_mult_s = (int32)(sVal)*(int32)((int16)(temp_filter[tap_ind] & 0xFFFF));

        m1_32 = (int32)( ( ( cqstatic_mult_c &  0x8001FFFF ) == 0x80010000)? (( cqstatic_mult_c - (1<<16)) >> 17): (( cqstatic_mult_c + (1<<16)) >> 17) );
        m2_32 = (int32)( ( ( cqstatic_mult_s &  0x8001FFFF ) == 0x80010000)? (( cqstatic_mult_s - (1<<16)) >> 17): (( cqstatic_mult_s + (1<<16)) >> 17) );
        m1_32_acc = m1_32_acc + m1_32;
        m2_32_acc = m2_32_acc + m2_32;
        if (debug_mode_en!=0)
        {
          FTM_MSG_9(FTM_HIGH, "filter_iter: %d, cap_iter: %d, tap_ind: %d, cVal: %d, sVal: %d, m1_32: %d, m2_32: %d, m1_32_acc: %d, m2_32_acc: %d ", filter_iter, iter, tap_ind, cVal, sVal, m1_32, m2_32, m1_32_acc, m2_32_acc);
        }

      }
      m1_sq_64 = ((int64)m1_32_acc)*((int64)m1_32_acc);
      m2_sq_64 = ((int64)m2_32_acc)*((int64)m2_32_acc);
      m1_sq = (uint16)( (( m1_sq_64 + (uint64)(0x1ULL<<14)) >> 15)  & 0xFFFF);
      m2_sq = (uint16)( (( m2_sq_64 + (uint64)(0x1ULL<<14)) >> 15)  & 0xFFFF);
      eqGainSq[(filter_iter*MAX_FREQ_OFFSETS_STG_IQ_CAP)+iter] = m1_sq + m2_sq;
      if (debug_mode_en!=0)
      {
        FTM_MSG(FTM_HIGH, "m1_sq_64 ------");
        print_int64hex((int64*)&m1_sq_64);
        FTM_MSG(FTM_HIGH, "m2_sq_64 ------");
        print_int64hex((int64*)&m2_sq_64);
        FTM_MSG_2(FTM_HIGH, "m1_sq: %d, m2_sq: %d", m1_sq, m2_sq);
        FTM_MSG_3(FTM_HIGH, "eqGainSq[%d][%d] = %d", filter_iter, iter,eqGainSq[(filter_iter*MAX_FREQ_OFFSETS_STG_IQ_CAP)+iter] );
      }

    }
  }

  return ret_val;
}

ftm_enh_internal_device_cal_error_code_type
ftm_enh_internal_device_cal_calc_eq
(
   uint8 num_captures, 
   uint32 * tonePwrs, 
   int32 * stg_offset_khz, 
   uint16 * eqGainSq, 
   uint8 * filter_techbw, 
   uint8 numNomEq, 
   uint32 refTonePower , 
   int32 refFreq
)
{
  ftm_enh_internal_device_cal_error_code_type ret_val = FTM_ENH_INTERNAL_DEVICE_CAL_RESULTS_SUCCESS;
  uint16 dqs[MAX_FREQ_OFFSETS_STG_IQ_CAP] = {0};
  uint64 pScaledqs_64;
  uint8 filt_iter = 0;
  uint8 freq_iter = 0;
  uint8 debug_mode_en;
  uint32 fCompQ_32;
  double sqrtfCompQ_dbl_inv;
  uint32 aVal_32;
  uint16 aVal_16[MAX_FREQ_OFFSETS_STG_IQ_CAP];
  int32  * freqList;
  int32 alpha[CPLX_NCALEQ];
  int16 alpha_16;
  uint8 row_iter;
  uint8 num_caps_techbw;
  uint8 valid_freq_iter;
  uint8 valid_freq_found = 0;
  uint8 aValInd = 0;
  uint8 overflowdet = 0;
  uint8 overall_overflowdet = 0;
  int32 cCalQ_I[CPLX_NCALEQ];
  int32 cCalQ_Q[CPLX_NCALEQ];
  int32 cQs_I[RFCOMMON_FBRX_NUM_FILTER_TAPS];
  int32 cQs_Q[RFCOMMON_FBRX_NUM_FILTER_TAPS];
  uint8 convStartInd; 
  uint8 convStopInd;
  uint8 nomEqLen;
  uint8 col_iter;
  int32 temp_I;
  int32 temp_Q;
  int32 sumCoeffI;
  int32 sumCoeffQ;
  double sqSumCoeff;
  double absSumCoeff;
  double normCoeffI_dbl;
  double normCoeffQ_dbl;
  int32 normCoeffI_32;
  int32 normCoeffQ_32;
  uint8 nom_grp_delay;
  int32 nominal_filter[RFCOMMON_FBRX_NUM_FILTER_TAPS];
  int16 finalCoeffI[RFCOMMON_FBRX_NUM_FILTER_TAPS];
  int16 finalCoeffQ[RFCOMMON_FBRX_NUM_FILTER_TAPS];
  ftm_enh_internal_device_cal_data_type *internal_device_cal_data;

  internal_device_cal_data = ftm_common_data_get_enh_internal_device_cal_ptr();
  debug_mode_en = internal_device_cal_data->internal_device_cal_config.dbg_mode_en;

  for (freq_iter = 0; freq_iter < num_captures; freq_iter++)
  {
    if (stg_offset_khz[freq_iter] == refFreq)
	{
      dqs[freq_iter]  = 1<<14;
	}
	else
	{
	  pScaledqs_64 = ((uint64)(((uint64)tonePwrs[freq_iter]) << 14 )+ (uint64)(((uint64)refTonePower)>>1))/((uint64)refTonePower);
      dqs[freq_iter]  = (uint16)((pScaledqs_64 ) & 0xFFFF);
	}

    if (debug_mode_en!=0)
    {
      FTM_MSG_2(FTM_HIGH, "stg_freq_khz: %d, dqs: %d", stg_offset_khz[freq_iter], dqs[freq_iter] );
    }

  }
  
  if (numNomEq > MAX_NUM_DROOP_FILTERS_GEN_PER_SWP)
  {
    FTM_MSG_2(FTM_ERROR, "Number of Nominal Equalizers %d > max possible %d", numNomEq, MAX_NUM_DROOP_FILTERS_GEN_PER_SWP);
    return FTM_ENH_INTERNAL_DEVICE_CAL_UNSUPPORTED_ACTION;
  }
  num_droop_filters = numNomEq;

  for (filt_iter = 0; filt_iter< numNomEq; filt_iter++)
  {
    aValInd = 0;
    memset((void*)&nominal_filter[0], 0, sizeof(int32)*RFCOMMON_FBRX_NUM_FILTER_TAPS);
    switch (filter_techbw[filt_iter])
    {
    case RFCOMMON_FBRX_LTE_1P4:
    case RFCOMMON_FBRX_LTE_3:
    case RFCOMMON_FBRX_LTE_5:
      num_caps_techbw = CAPS_LTE5;
      freqList = &FKHZ_LTE5[0];
      nomEqLen = filterlen_LTE5;
      nom_grp_delay = filter_grp_delay_LTE5;
      droop_filter_tech_bw[filt_iter] = RFCOMMON_FBRX_LTE_5;
      memscpy((void*)&nominal_filter[0], sizeof(int32)*filterlen_LTE5, (void*)&filter_LTE5[0], sizeof(int32)*filterlen_LTE5);
      break;
    case RFCOMMON_FBRX_LTE_10:
      num_caps_techbw = CAPS_LTE10;
      freqList = &FKHZ_LTE10[0];
      nomEqLen = filterlen_LTE10;
      nom_grp_delay = filter_grp_delay_LTE10;
      droop_filter_tech_bw[filt_iter] = RFCOMMON_FBRX_LTE_10;
      memscpy((void*)&nominal_filter[0], sizeof(int32)*filterlen_LTE10, (void*)&filter_LTE10[0], sizeof(int32)*filterlen_LTE10);
      break;
    case RFCOMMON_FBRX_LTE_15:
      num_caps_techbw = CAPS_LTE15;
      freqList = &FKHZ_LTE15[0];
      nomEqLen = filterlen_LTE15;
      nom_grp_delay = filter_grp_delay_LTE15;
      droop_filter_tech_bw[filt_iter] = RFCOMMON_FBRX_LTE_15;
      memscpy((void*)&nominal_filter[0], sizeof(int32)*filterlen_LTE15, (void*)&filter_LTE15[0], sizeof(int32)*filterlen_LTE15);
      break;
    case RFCOMMON_FBRX_LTE_20:
      num_caps_techbw = CAPS_LTE20;
      freqList = &FKHZ_LTE20[0];
      nomEqLen = filterlen_LTE20;
      nom_grp_delay = filter_grp_delay_LTE20;
      droop_filter_tech_bw[filt_iter] = RFCOMMON_FBRX_LTE_20;
      memscpy((void*)&nominal_filter[0], sizeof(int32)*filterlen_LTE20, (void*)&filter_LTE20[0], sizeof(int32)*filterlen_LTE20);
      break;
    case RFCOMMON_FBRX_UMTS_DC:
    case RFCOMMON_FBRX_MCDO_7X:
      num_caps_techbw = CAPS_LTE10;
      freqList = &FKHZ_LTE10[0];
      nomEqLen = filterlen_nonLTE;
      nom_grp_delay = filter_grp_delay_nonLTE;
      droop_filter_tech_bw[filt_iter] = filter_techbw[filt_iter];
      memscpy((void*)&nominal_filter[0], sizeof(int32)*filterlen_nonLTE, (void*)&filter_nonLTE[0], sizeof(int32)*filterlen_nonLTE);
      break;
    case RFCOMMON_FBRX_1X:
    case RFCOMMON_FBRX_MCDO_3X:
    case RFCOMMON_FBRX_UMTS_SC:
    case RFCOMMON_FBRX_TDS:
    case RFCOMMON_FBRX_SB_SVDO:
      num_caps_techbw = CAPS_LTE5;
      freqList = &FKHZ_LTE5[0];
      nomEqLen = filterlen_nonLTE;
      nom_grp_delay = filter_grp_delay_nonLTE;
      droop_filter_tech_bw[filt_iter] = filter_techbw[filt_iter];
      memscpy((void*)&nominal_filter[0], sizeof(int32)*filterlen_nonLTE, (void*)&filter_nonLTE[0], sizeof(int32)*filterlen_nonLTE);
      break;
    default:
      FTM_MSG_1(FTM_ERROR, "Unsupported FBRx tech/bandwidth type %d", filter_techbw[filt_iter]);
      return FTM_ENH_INTERNAL_DEVICE_CAL_UNSUPPORTED_TECH;
      break;
    }

    for (valid_freq_iter = 0; valid_freq_iter < num_caps_techbw; valid_freq_iter++ )
    {
      valid_freq_found = 0;
      for (freq_iter = 0; freq_iter < num_captures; freq_iter++ )
      {
        if ( freqList[valid_freq_iter] == stg_offset_khz[freq_iter])
        {
          valid_freq_found = 1;
          break;
        }
      }
      if (valid_freq_found == 0)
      {
        continue;
      }
      fCompQ_32 = ((uint32)eqGainSq[(filt_iter*MAX_FREQ_OFFSETS_STG_IQ_CAP)+freq_iter])*((uint32)dqs[freq_iter]);
      fCompQ_32 = ( (fCompQ_32 & 0x800001FF) == 0x80000100) ? ((fCompQ_32 - (1<<8)) >> 9) : ((fCompQ_32 + (1<<8)) >> 9) ;
      sqrtfCompQ_dbl_inv = pow(2.0,22)/sqrt((double)fCompQ_32);
      aVal_32 = (((uint32)sqrtfCompQ_dbl_inv) + 1) >> 1;
      aVal_16[aValInd++] = sat_uint32_to_uint16(aVal_32, &overflowdet);
      if (debug_mode_en!=0)
      {
        FTM_MSG_5(FTM_HIGH, "techbw: %d, stg_freq_khz: %d, fCompQ_32: %d, aVal_32: %d, aVal_16: %d ", filter_techbw[filt_iter], stg_offset_khz[freq_iter], fCompQ_32, aVal_32, aVal_16[aValInd-1] );
      }
      if (overflowdet == 1)
      {
        return FTM_ENH_INTERNAL_DEVICE_CAL_ARITHMETIC_OVERFLOW;
      }

    }

    if (aValInd <num_caps_techbw)
    {
      FTM_MSG_2(FTM_ERROR, "Number of valid freqs found %d < needed %d", aValInd, num_caps_techbw);
      return FTM_ENH_INTERNAL_DEVICE_CAL_INSUFFICIENT_PARAMS;
    }

    for (row_iter = 0; row_iter < CPLX_NCALEQ; row_iter++)
    {
      alpha[row_iter] = 0;
      for (valid_freq_iter = 0; valid_freq_iter < num_caps_techbw; valid_freq_iter++ )
      {
        switch (filter_techbw[filt_iter])
        {
        case RFCOMMON_FBRX_LTE_1P4:
        case RFCOMMON_FBRX_LTE_3:
        case RFCOMMON_FBRX_LTE_5:
          alpha[row_iter] += B_LTE5[row_iter][valid_freq_iter]*aVal_16[valid_freq_iter];
          alpha[row_iter] = ((alpha[row_iter] & 0x80003FFF) == 0x80001000) ? ((alpha[row_iter] - (1<<13))>>14) : ((alpha[row_iter] + (1<<13))>>14);
          alpha_16 = sat_int32_to_int16(alpha[row_iter], &overflowdet, debug_mode_en);
          overall_overflowdet |= overflowdet;
          alpha[row_iter] = ((int32)alpha_16) << 14;
          break;
        case RFCOMMON_FBRX_LTE_10:
          alpha[row_iter] += B_LTE10[row_iter][valid_freq_iter]*aVal_16[valid_freq_iter];
          alpha[row_iter] = ((alpha[row_iter] & 0x80003FFF) == 0x80001000) ? ((alpha[row_iter] - (1<<13))>>14) : ((alpha[row_iter] + (1<<13))>>14);
          alpha_16 = sat_int32_to_int16(alpha[row_iter], &overflowdet, debug_mode_en);
          overall_overflowdet |= overflowdet;
          alpha[row_iter] = ((int32)alpha_16) << 14;
          break;
        case RFCOMMON_FBRX_LTE_15:
          alpha[row_iter] += B_LTE15[row_iter][valid_freq_iter]*aVal_16[valid_freq_iter];
          alpha[row_iter] = ((alpha[row_iter] & 0x80003FFF) == 0x80001000) ? ((alpha[row_iter] - (1<<13))>>14) : ((alpha[row_iter] + (1<<13))>>14);
          alpha_16 = sat_int32_to_int16(alpha[row_iter], &overflowdet, debug_mode_en);
          overall_overflowdet |= overflowdet;
          alpha[row_iter] = ((int32)alpha_16) << 14;
          break;
        case RFCOMMON_FBRX_LTE_20:
          alpha[row_iter] += B_LTE20[row_iter][valid_freq_iter]*aVal_16[valid_freq_iter];
          alpha[row_iter] = ((alpha[row_iter] & 0x80003FFF) == 0x80001000) ? ((alpha[row_iter] - (1<<13))>>14) : ((alpha[row_iter] + (1<<13))>>14);
          alpha_16 = sat_int32_to_int16(alpha[row_iter], &overflowdet, debug_mode_en);
          overall_overflowdet |= overflowdet;
          alpha[row_iter] = ((int32)alpha_16) << 14;
          break;
        case RFCOMMON_FBRX_1X:
        case RFCOMMON_FBRX_MCDO_3X:
        case RFCOMMON_FBRX_UMTS_SC:
        case RFCOMMON_FBRX_TDS:
        case RFCOMMON_FBRX_SB_SVDO:
          alpha[row_iter] += B_LTE5[row_iter][valid_freq_iter]*aVal_16[valid_freq_iter];
          alpha[row_iter] = ((alpha[row_iter] & 0x80003FFF) == 0x80001000) ? ((alpha[row_iter] - (1<<13))>>14) : ((alpha[row_iter] + (1<<13))>>14);
          alpha_16 = sat_int32_to_int16(alpha[row_iter], &overflowdet, debug_mode_en);
          overall_overflowdet |= overflowdet;
          alpha[row_iter] = ((int32)alpha_16) << 14;
          break;
        case RFCOMMON_FBRX_UMTS_DC:
        case RFCOMMON_FBRX_MCDO_7X:
          alpha[row_iter] += B_LTE10[row_iter][valid_freq_iter]*aVal_16[valid_freq_iter];
          alpha[row_iter] = ((alpha[row_iter] & 0x80003FFF) == 0x80001000) ? ((alpha[row_iter] - (1<<13))>>14) : ((alpha[row_iter] + (1<<13))>>14);
          alpha_16 = sat_int32_to_int16(alpha[row_iter], &overflowdet, debug_mode_en);
          overall_overflowdet |= overflowdet;
          alpha[row_iter] = ((int32)alpha_16) << 14;
          break;
        default:
          FTM_MSG(FTM_ERROR, "Unsupported Tech/BW!");
          return FTM_ENH_INTERNAL_DEVICE_CAL_UNSUPPORTED_TECH;
          break;
        }

      }
      alpha[row_iter] = ((alpha[row_iter] & 0x80003FFF) == 0x80001000) ? ((alpha[row_iter] - (1<<13))>>14) : ((alpha[row_iter] + (1<<13))>>14);
      alpha_16 = sat_int32_to_int16(alpha[row_iter], &overflowdet, debug_mode_en);
      overall_overflowdet |= overflowdet;
      alpha[row_iter] = (int32)alpha_16;
    }

    cCalQ_I[0] = ((alpha[1] & 0x80000001) == 0x80000001) ? ((alpha[1] - 1) >> 1) : ((alpha[1] + 1) >> 1);
    cCalQ_Q[0] = (((-alpha[2]) & 0x80000001) == 0x80000001) ? (((-alpha[2]) - 1) >> 1) : (((-alpha[2]) + 1) >> 1);

    cCalQ_I[1] = alpha[0];
    cCalQ_Q[1] = 0;

    cCalQ_I[2] = ((alpha[1] & 0x80000001) == 0x80000001) ? ((alpha[1] - 1) >> 1) : ((alpha[1] + 1) >> 1);
    cCalQ_Q[2] = ((alpha[2] & 0x80000001) == 0x80000001) ? ((alpha[2] - 1) >> 1) : ((alpha[2] + 1) >> 1);

    if (debug_mode_en!=0)
    {
      FTM_MSG_3(FTM_HIGH, "alpha[0]: %d , alpha[1]: %d , beta: %d" , alpha[0], alpha[1], alpha[2]);
      FTM_MSG_6(FTM_HIGH, "cCalQ[0]: %d + j * %d, cCalQ[1]: %d + j * %d, cCalQ[2]: %d + j * %d" , cCalQ_I[0], cCalQ_Q[0], cCalQ_I[1], cCalQ_Q[1], cCalQ_I[2], cCalQ_Q[2]);
    }

    if (nomEqLen + CPLX_NCALEQ - 1 > RFCOMMON_FBRX_NUM_FILTER_TAPS )
    {
      FTM_MSG_2(FTM_ERROR, "Number of filter taps to calculate %d > supported %d", nomEqLen + CPLX_NCALEQ - 1, RFCOMMON_FBRX_NUM_FILTER_TAPS);
      return FTM_ENH_INTERNAL_DEVICE_CAL_UNSUPPORTED_ACTION;
    }
    droop_filter_grp_delay[filt_iter] = nom_grp_delay + (((uint8)CPLX_NCALEQ - 1)/2);
    droop_filter_len[filt_iter]  = nomEqLen + (uint8)(CPLX_NCALEQ) - 1 ;
    FTM_MSG_4(FTM_HIGH, "droop_filter_grp_delay[%d]: %d, droop_filter_len[%d]: %d", filt_iter, droop_filter_grp_delay[filt_iter], filt_iter, droop_filter_len[filt_iter]);
    sumCoeffI = 0;
    sumCoeffQ = 0;
    for (row_iter = 0; row_iter < nomEqLen + CPLX_NCALEQ - 1; row_iter++)
    {
      cQs_I[row_iter] = 0;
      cQs_Q[row_iter] = 0;
      convStartInd = ((row_iter - nomEqLen + 1 ) < 0) ? 0:(row_iter - nomEqLen + 1);
      convStopInd  = (row_iter >  CPLX_NCALEQ - 1  ) ? CPLX_NCALEQ-1:row_iter;
      for (col_iter = convStartInd; col_iter <= convStopInd; col_iter++)
      {
        temp_I = cCalQ_I[col_iter]* nominal_filter[row_iter-col_iter];
        temp_Q = cCalQ_Q[col_iter]* nominal_filter[row_iter-col_iter];
        temp_I = ( ( (temp_I & 0x80003FFF) == 0x80001000) ? ((temp_I - (1<<13))>>14) : ((temp_I + (1<<13))>>14));
        temp_I = (int32)sat_int32_to_int16(temp_I, &overflowdet, debug_mode_en);
        overall_overflowdet |= overflowdet;
        temp_Q = ( ( (temp_Q & 0x80003FFF) == 0x80001000) ? ((temp_Q - (1<<13))>>14) : ((temp_Q + (1<<13))>>14));
        temp_Q = (int32)sat_int32_to_int16(temp_Q, &overflowdet, debug_mode_en);
        overall_overflowdet |= overflowdet;

        cQs_I[row_iter] = cQs_I[row_iter] + temp_I;
        cQs_Q[row_iter] = cQs_Q[row_iter] + temp_Q;
      }
      if (debug_mode_en!=0)
      {
        FTM_MSG_3(FTM_HIGH, "coeff[%d]: %d + j * %d" , row_iter, cQs_I[row_iter], cQs_Q[row_iter]);
      }
      sumCoeffI = sumCoeffI + cQs_I[row_iter];
      sumCoeffQ = sumCoeffQ + cQs_Q[row_iter];
    }
    sqSumCoeff = (((double)sumCoeffI)*((double)sumCoeffI)) + (((double)sumCoeffQ)*((double)sumCoeffQ));
    absSumCoeff = sqrt(sqSumCoeff);
    memset((void*)&droop_filter_coeffs[filt_iter][0], 0, sizeof(int32)*RFCOMMON_FBRX_NUM_FILTER_TAPS);
    for (row_iter = 0; row_iter < nomEqLen + CPLX_NCALEQ - 1; row_iter++)
    {
      normCoeffI_dbl = ((double)cQs_I[row_iter])/absSumCoeff;
      normCoeffQ_dbl = ((double)cQs_Q[row_iter])/absSumCoeff;

      normCoeffI_32  = (int32)(normCoeffI_dbl*pow(2.0,15));
      normCoeffI_32  = ( (normCoeffI_32 & 0x80000001) == 0x80000001) ? ((normCoeffI_32 - 1) >>1 ) : ((normCoeffI_32 + 1) >>1 ) ;
      normCoeffQ_32  = (int32)(normCoeffQ_dbl*pow(2.0,15));
      normCoeffQ_32  = ( (normCoeffQ_32 & 0x80000001) == 0x80000001) ? ((normCoeffQ_32 - 1) >>1 ) : ((normCoeffQ_32 + 1) >>1 ) ;

      finalCoeffI[row_iter] = sat_int32_to_int16(normCoeffI_32, &overflowdet, debug_mode_en);
      overall_overflowdet |= overflowdet;
      finalCoeffQ[row_iter] = sat_int32_to_int16(normCoeffQ_32, &overflowdet, debug_mode_en);
      overall_overflowdet |= overflowdet;
      droop_filter_coeffs[filt_iter][row_iter] = (finalCoeffQ[row_iter] << 16) + (finalCoeffI[row_iter] & 0xFFFF);
      if (debug_mode_en!=0)
      {
        FTM_MSG_4(FTM_HIGH, "norm coeff[%d]: %d + j * %d, concatenated: %d" , row_iter, finalCoeffI[row_iter], finalCoeffQ[row_iter], droop_filter_coeffs[filt_iter][row_iter] );
      }
    }

    if (filter_techbw[filt_iter] == RFCOMMON_FBRX_LTE_5)
    {
      num_droop_filters = 4;
      droop_filter_tech_bw[2] = RFCOMMON_FBRX_LTE_3;
      droop_filter_tech_bw[3] = RFCOMMON_FBRX_LTE_1P4;
      droop_filter_grp_delay[2] = droop_filter_grp_delay[filt_iter] ;
      droop_filter_grp_delay[3] = droop_filter_grp_delay[filt_iter] ;
      droop_filter_len[2] = droop_filter_len[filt_iter];
      droop_filter_len[3] = droop_filter_len[filt_iter];
      memscpy((void*)&droop_filter_coeffs[2][0], sizeof(int32)*RFCOMMON_FBRX_NUM_FILTER_TAPS, (void*)&droop_filter_coeffs[filt_iter][0], sizeof(int32)*RFCOMMON_FBRX_NUM_FILTER_TAPS); // copy over LTE5 filter for LTE3
      memscpy((void*)&droop_filter_coeffs[3][0], sizeof(int32)*RFCOMMON_FBRX_NUM_FILTER_TAPS, (void*)&droop_filter_coeffs[filt_iter][0], sizeof(int32)*RFCOMMON_FBRX_NUM_FILTER_TAPS); // copy over LTE5 filter for LTE3
    }


    if (overall_overflowdet != 0)
    {
      return FTM_ENH_INTERNAL_DEVICE_CAL_ARITHMETIC_OVERFLOW;
    }

  }
  if (debug_mode_en!=0)
  {
    FTM_MSG_1(FTM_HIGH, "num_droop_filters: %d", num_droop_filters);
    for (row_iter = 0; row_iter < num_droop_filters; row_iter++)
    {
      FTM_MSG_3(FTM_HIGH, "tech_bw: %d, filt_len: %d, grp_delay: %d", droop_filter_tech_bw[row_iter], droop_filter_len[row_iter], droop_filter_grp_delay[row_iter]);
      for (col_iter = 0; col_iter<droop_filter_len[row_iter]; col_iter++)
      {
        FTM_MSG_2(FTM_HIGH, "coeff[%d]: %d", col_iter, droop_filter_coeffs[row_iter][col_iter]);
      }
    }
  }
  
  return ret_val;
}
#endif /* FEATURE_FACTORY_TESTMODE */

#endif /* FEATURE_GNSS_ONLY_NO_WWAN */

/*! @} */


