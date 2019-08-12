/*! 
  @file
  rfcommon_core_xpt.c
 
  @brief
  This file defines the functionality and algorithms to be performed for Tx  
  AGC operations
*/

/*==============================================================================

  Copyright (c) 2012 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/src/rfcommon_core_xpt.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/15/14   vbh     Added device as a parameter to support Tx sharing
08/27/14   kai     Disable dpd_debug_mode flag
07/21/14   aro/kai Added expansion ratio param in DPD expansion function
05/16/14   cri     IQ capture segment reduction changes
04/24/14   zhw     Use rfcommon_mdsp_xpt_mode_enum_type for all DPD loading
03/04/14   zhw     Clean up dpd save/commit API 
01/06/14   cd      Linearizer and Tx AGC updates for loading xPT information to 
                   RFLM
12/04/13   nrk     Fixed compiler warnings
12/02/13   cri     Support for RFLM Tx override for LTE
11/22/13   id      Load DPD changes for BOLT
11/20/13   cd      Added time profiling for common Tx override lin load intf
11/19/13   jps     Support for RFLM-based Tx AGC override (TD-SCDMA)
11/14/13   cd      Support for RFLM-based Tx AGC override
10/25/13   id      Added initial support for Load DPD
08/30/13    pv     Optimization of Delay vs Temp vs Freq offset for XPT.
08/08/13   sl/cri  Common implementation for DPD scaling
07/24/13    pv     Cleaned up F3 appearing every ms.
07/23/13   aka     Self-test and FFT related types
07/10/13    pv     Support for Delay vs Temp vs Freq offset for XPT.
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
05/21/13   Saul    Common XPT. Allocate/write DPD to MDSP only if present.
04/22/13   Kai     Fix DPD bank index conversion
04/19/13    id     Bug fix for being able to save multiple DPD and Load
04/15/13   Saul    Common. Consolidated XPT data into one structure.
03/20/13   Saul    MultiLin. Each RGI now has info for which freqcomp to use.
03/19/13   cd/rsr  Fixed detrough table expansion to not expand NV to zero 
                   values
03/12/13   bn      Check for Unique DPD index and don't process DPD for already exsisting one
03/11/13   sar     Updated file for APQ, GNSS only target.
02/22/13   cri     Load DPD tables based on tx chain
01/18/13   jf      Fix to load multiple DPD tables in hardware
01/10/13   cri     Track allocated DPD tables
12/06/12   rsr     Added DPD expansion code.
12/10/12   cri     Load DDP from current linearizer
12/05/12   Saul    Common XPT. Process DPD corresponding to relevant linearizer.
11/16/12   Saul    Sample Capture. Use common response.
11/15/12   Saul    Common XPT. Update to populate dpd API.
11/13/12   Saul    Common XPT. Fixed warnings.
11/13/12   Saul    Common XPT. DPD expansion framework.
11/13/12   Saul    Common XPT. Reverted changes made to commit_dpd_data.
11/12/12   Saul    MultiLin. Removed dpd_data from lin table type.
11/08/12   cd      Commit DPD will send FW override to latch the tables
11/08/12   Saul    MultiLin. Restructured EPT_V2 and ET DPDs.
11/05/12   Saul    Common XPT. Changed tech-specific feature to common feature.
10/26/12   Saul    Common XPT. Added num elements check.
10/22/12   Saul    Common XPT. DPD data save now provides bank index and offset.
10/19/12   Saul    Common XPT. Partitioned dpd data per tx path.
10/12/12   Saul    Common XPT. Load DPD framework.
08/22/12   Saul    MultiLin. Temp-comp supports V3 lin.
08/21/12   Saul    MultiLin. Added support to load EPT V2 and ET NV Data.
08/17/12   Saul    Initial revision. Created to modularize DPD code for feature
                   removal.
==============================================================================*/
#ifndef FEATURE_GNSS_ONLY_NO_WWAN

#include "comdef.h"
#include "rfcommon_core_xpt.h"
#include "rfcommon_core_utils.h"
#include "rfcommon_msg.h"
#include "stringl.h" /* for memscpy */

#include "rfcommon_data.h"
#include "rfcommon_mc.h"
#include "rfcommon_math.h"
#include "zlib.h"
#include "rflm_txagc_modem_api.h"
#ifdef FEATURE_CDMA1X
#include "rf_cdma_tx_agc.h"
#endif
#ifdef FEATURE_LTE
#include "rflte_mc.h"
#endif
#ifdef FEATURE_TDSCDMA
#include "rf_tdscdma_core_txlin.h"
#endif

#ifdef FEATURE_WCDMA
#include "rfwcdma_core_txctl.h"
#endif

#include "modem_mem.h"
#define MAX_INTERP_IN 16
#define INTERP_QFACTOR 15
#define IC_QFACTOR 4

/*----------------------------------------------------------------------------*/
uint8
rf_common_xpt_find_nxt_bank_with_space
(
   rfm_device_enum_type device,
   uint8 start_bank_num,
   load_dpd_input_table_type tbl_type,
   uint16 *rem_space_in_bnk
);

/*----------------------------------------------------------------------------*/
boolean
rf_common_xpt_bank_allows_table_type
(
   rfm_device_enum_type device,
   uint8 bank_id, 
   load_dpd_input_table_type tbl_type
);

/*----------------------------------------------------------------------------*/
boolean rfcommon_dpd_debug_mode_en = FALSE;

/*----------------------------------------------------------------------------*/
static void 
rfcommon_xpt_dpd_padding
( 
  load_dpd_input_table_type dpd_tbl_type,
  uint16 dpd_table_sz, 
  uint16 interp_dpd_num,
  uint16 *dpd_table
)
{
  if ( dpd_table == NULL )
  {
    RF_MSG( RF_ERROR, "rfcommon_xpt_dpd_padding: NULL DPD Table" );
    return;
  } /* if ( dpd_table != NULL ) */

  if ( dpd_table_sz >  interp_dpd_num )
  {
    uint8 idx = interp_dpd_num;
    uint16 last_amam = dpd_table[idx - 1];
    for ( idx = interp_dpd_num;
          idx < dpd_table_sz;
          idx++ )
    {
      dpd_table[idx] = last_amam;
    } /* for ( idx = interp_dpd_num;
          idx < dpd_table_sz;
          idx++ ) */
    RF_MSG_3( RF_HIGH, "rfcommon_xpt_dpd_padding: DPD %d Padded - Table "
              "Size %d, Final Table %d",
              dpd_tbl_type,
              dpd_table_sz,
              interp_dpd_num );
  } /* if ( dpd_table_sz >  interp_dpd_num ) */
} /* rfcommon_xpt_dpd_padding */

/*----------------------------------------------------------------------------*/
static void 
rfcommon_xpt_dpd_debug
( 
  load_dpd_input_table_type dpd_tbl_type,
  uint16 dpd_table_sz, 
  uint16 *dpd_table
)
{
  int16 dpd_idx = -1;
  uint16 dpd_idx_temp[8];
  uint8 i = 0;
  uint32 dpd_and_idx = 0;
  uint16 while_count = 0;

  boolean break_flag = FALSE;

  while ( break_flag == FALSE )
  {
    i = 0;

    for ( i = 0; i<8; i++ )
    {
      dpd_idx++;
      if ( dpd_idx < dpd_table_sz )
      {
        dpd_idx_temp[i] = dpd_idx;
      }
      else
      {
        dpd_idx_temp[i] = dpd_table_sz - 1;
      }
    }

    dpd_and_idx = (dpd_tbl_type<<24) | (dpd_idx<<8) | (while_count);

    while_count++;

    RF_MSG_9( RF_FATAL, "rfcommon_xpt_dpd_debug: [DPD 0x%5x] "
              "[0x%5x][0x%5x][0x%5x][0x%5x][0x%5x][0x%5x][0x%5x][0x%5x]",
              dpd_and_idx,
              dpd_table[dpd_idx_temp[0]],
              dpd_table[dpd_idx_temp[1]],
              dpd_table[dpd_idx_temp[2]],
              dpd_table[dpd_idx_temp[3]],
              dpd_table[dpd_idx_temp[4]],
              dpd_table[dpd_idx_temp[5]],
              dpd_table[dpd_idx_temp[6]],
              dpd_table[dpd_idx_temp[7]] );

    if ( dpd_idx >= dpd_table_sz )
    {
      break_flag = TRUE;
    } /* if ! ( dpd_idx < dpd_table_sz ) */

  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Expands DPD data
 
@details 
  Algorithm :- ( Reference Numerical Recipes in C )
  This function does spline interpolation between any 2 adjacent points to expand the input table to the output table.
  Since this function is interpolating between points, it assumes that the for AMAM/ EPDT tables the first point is 0 and 
that for AMPM tables the first point is a duplicate of the first input entry in the DPD table. 
i.e. if the input is a DPD table of type AMAM or EPDT, then the input DPD table dpd_coeffs[0..N-1] is expanded such that 
dpd_coeffs[-1] = 0. If the input table is an AMPM table then the input table is expanded as dpd_coeffs[-1]= dpd_coeffs[0]. 
This expanded DPD table is then used to run the interpolation. This ensures that dpd_coeffs[0] and dpd_coeffs[N-1] are both part of the expanded tables.

 INTRODUCTION TO SPLINE:
  A spline is a set of 3'rd order curves (e.g. y = ax^3 + bx^2 + cx + d) between adjacent points ,  
  with the constraint that the boundary between two adjacent 3'rd order curves is smooth, 
  i.e. the first derivative dy/dx of one 3'rd order curve calculated at the boundary  = 
       the first derivative of the adjacent 3'rd order curve also evalated at the same boundary. 
  Additionally each 3'rd order curve can be though of as the sum of a  line and 
  a 3'rd order curve that is zero at the end points. 
  i.e. if we have 2 adjacent data points in the input curve (x[i],y[i]) and (x[i+1],y[i+1]),
  we can write the equation for the spline as 
  y = yline + y3rd_order, 
  yline = A*y[i] + B*y[i+1] , where A = (x[i+1] - x) / (x[i+1] - x[i]) ,  
                                    B = (x - x[i]) / (x[i+1] - x[i]) ,  --- (EQN 1)
  yspline = C*y"[i] + D*y"[i+1], where y"[i] = 2'rd derivative of y evaluated at i, i+1,
                                       C = 1/6 * (x[i+1] - x[i])^2 * (A^3 - A) , 
                                  D = 1/6 * (x[i+1] - x[i])^2 * (B^3 - B) ------- (EQN 2)
  SideBar :- Eqn 2, is derived from the following facts   
        y" = yline" + yspline", yline" = 0 => y" = yspline",
      yspline" = a line between y"[i] and y"[i+1] ( since 2'nd derivative of 3'rd order curve is a always a line).
      and there fore we can write yspline" = A*y"[i] + B* y[i+1], where A, B are same as in EQN 1. 
      Also, yspline = integral{ integral{yspline"dx} dx} and yspline has value zero at x = x[i], x= x[i+1]. 
  
  From (EQN 1) and (EQN 2)
  y = A*y[i] + B*y[i+1] + C*y"[i] + D*y"[i+1] ------------ (EQN 3)

  The inputs to this function are y[i]. 
  x[i] are assumed to be apart by expansn_ratio, i.e. x[0] = 0, x[1] = expansn_ratio, x[2] = 2*expansn_ratio etc. 
  A, B, C, D can all be computed based on x, and x[i]s.
  But the y"[i] are still unknown.
    
  This algorithm, starts by computing y"[i].
  To compute y"[i] :-
  -------------------
  We use the constraint that the boundary between two adjacent 3'rd order curves that form the spline is smooth. 
  i.e. the first derivative dy/dx of one 3'rd order curve calculated at the boundary  = 
      the first derivative of the adjacent 3'rd order curve also evalated at the boundary :-
  y' = dy/dx of spline between x[i], x[i+1] = dA[i,i+1]/dx *y[i] + dB[i,i+1]/dx * y[i+1] + dC[i,i+1]/dx * y"[i] + dD[i,i+1]/dx*y[i+1]" 
   = (y[i+1] - y[i])/ (x[i+1] - x[i])  - (x[i+1] -x[i])/3 * y"[i]  - (x[i+1] - x[i])/6 * y"[i+1] 
  y' = dy/dx of spline between x[i-1], x[i] = dA[i-1,i]/dx *y[i-1] + dB[i-1,i]/dx * y[i] + dC[i-1,i]/dx * y"[i-1] + dD[i-1,i]/dx*y[i]" 
    = (y[i] - y[i-1])/ (x[i] - x[i-1])  + (x[i] -x[i-1])/6 * y"[i-1] + (x[i+1] - x[i])/3 * y"[i] 
  => 
  (x[i] -x[i-1])/6 * y"[i-1] + (x[i+1] - x[i-1])/3 * y"[i] +(x[i+1] - x[i])/6 * y"[i+1]  = 
   (y[i+1] - y[i])/ (x[i+1] - x[i]) - (y[i] - y[i-1])/ (x[i] - x[i-1]) -----------------------( EQN 4)
  
  In this function the input y = amam and it goes from y[0] to y[16], so i = 0..N. 
  EQN 4 yields a system of eqns for i = 0..N. , that can be written in tridiagonal form. 
  We assume, y"[0] = y"[16] = 0, and when i = 0
  (y[i+1] - y[i])/ (x[i+1] - x[i]) - (y[i] - y[i-1])/ (x[i] - x[i-1]) = 0  (assuming that the slope extends from the first 2 points backwards)
  
  This tridiagonal matrix eqn is solved to get y"[i]. See http://en.wikipedia.org/wiki/Tridiagonal_matrix_algorithm for explanation.
  The first 2 for loops accomplish this, and use the variables cmod (or c' in the wiki document above.), and the variable dmod_expratio_sq 
  (or d'). The factors cmod have been precomputed for a given expansn_Ratio. 
  Also refer to doc for more info.

  Compute y (expanded DPD table)
  ----------
  Once y"[i] are computed we can use EQN 1 to compute y for 
  x = {(h-1)/h * (x[i+1] - x[i]),(h-2)/h * (x[i+1] - x[i]),.... 1/h * (x[i+1] - x[i])} -> has NUM_OF_X_VALUES.
  The function precomputes the values of A,B,C,D for each of the x values above for 2 different values of expansn_ratio =>
  Interpolation_Weights_4[4][NUM_OF_X_VALUES = 4]  = values of A,B,C,D for different values of x  when expansn_ratio = 4.
  Interpolation_Weights_8[4][NUM_OF_X_VALUES = 8]  = values of A,B,C,D for different values of x  when expansn_ratio = 8.
  
  @param dpd_tbl_type
  The table type to be expanded
 
  @param num_dpd_coeffs
  Number of DPD coefficients available for expansion.
 
  @param dpd_coeffs
  Input table to expand.

  @param expansn_ratio
    Expansion ratio. Size of output tables = num_dpd_coeffs *expansn_ratio. So if input size is 17 (16+1),
    output size will be 64 when expansn_ratio is 4 and 128 when expansn_ratio is 8.
 
  @param dpd_tbl
  OUTPUT. Expanded DPD table.
 
  @return
  TRUE if successful, else FALSE

*/ 
boolean
rf_common_xpt_expand_dpd_data(
   load_dpd_input_table_type dpd_tbl_type,
   uint16 num_dpd_coeffs,
   uint16* dpd_coeffs,
   uint16 expansn_ratio,
   uint16* dpd_tbl,
   xpt_dpd_scaling_value_type *dpd_scale_factor
)
{
  boolean api_status = TRUE;
  uint16* dpd_tbl_input = NULL; 
  int32 dmod [MAX_INTERP_IN];
  int32 y2 [MAX_INTERP_IN + 1];
  int32 in0, in1, in2, delta;
  /* Internal variables to use for signed expansion (AMPM) */
  int32 y_lo, y_hi, y2_lo, y2_hi;
  /* Internal variables to use for unsigned expansion (AMAM/Det) */
  //uint32 y_lo_uns, y_hi_uns, y2_lo_uns, y2_hi_uns;
  int32 idx, k;
  int8 * ic = NULL;
  int8 * ic_p = NULL;
  uint32 y_scale;
  uint8 is_signed = 0; //FALSE
  
  /* Q32. Precalculated negative c~ multiplied by a factor of 2^32 */
  static int32 cmod [MAX_INTERP_IN + 1] =
  {
    0,
    -1073741824,
    -1145324613,
    -1150437669,
    -1150804635,
    -1150830981,
    -1150832873,
    -1150833008,
    -1150833018,
    -1150833019,
    -1150833019,
    -1150833019,
    -1150833019,
    -1150833019,
    -1150833019,
    -1150833019,
    0
  };

  static int8 ic_2 [2 * 4] =
  {
    8,   8, -4, -4,
    0,  16,   0,   0
  };

  static int8 ic_4 [4 * 4] =
  {
   12,   4, -14, -10,
    8,   8, -16, -16,
    4,  12, -10, -14,
    0,  16,   0,   0
  };

  static int8 ic_8 [8 * 4] =
  {
   14,   2, -35, -21,
   12,   4, -56, -40,
   10,   6, -65, -55,
    8,   8, -64, -64,
    6,  10, -55, -65,
    4,  12, -40, -56,
    2,  14, -21, -35,
    0,  16,   0,   0
  };

  /* ------------Input parameters Checking START----------*/
  if(dpd_coeffs == NULL)
  {
     RF_MSG( RF_ERROR, "rf_common_xpt_expand_dpd_data - NULL DPD coeffs." );
     api_status = FALSE;
  }

  if(dpd_tbl == NULL)
  {
     RF_MSG( RF_ERROR, "rf_common_xpt_expand_dpd_data - NULL DPD output table." );
    api_status = FALSE;
  }

  if (dpd_scale_factor == NULL)
  {
    RF_MSG( RF_ERROR, "rf_common_xpt_expand_dpd_data - NULL DPD scaling input." );
    api_status = FALSE;
  }

  if ((expansn_ratio != 8) && (expansn_ratio != 4) && (expansn_ratio != 2) )
  {
    RF_MSG( RF_ERROR, "rf_common_xpt_expand_dpd_data - Expansion ratio has to be 2 or 4 or 8." );
    api_status = FALSE;
  }

  if ( (expansn_ratio ==8) && 
       ( (dpd_tbl_type == DPD_TYPE_AMPM)||(dpd_tbl_type == DPD_TYPE_AMAM) ) )
  {
    RF_MSG( RF_ERROR, "rf_common_xpt_expand_dpd_data - Expansion ratio for AMAM and AMPM should be 4. " );
    api_status = FALSE;
  }
 
  if ( (expansn_ratio ==4) && (dpd_tbl_type == DPD_TYPE_EPDT))
  {
    RF_MSG( RF_ERROR, "rf_common_xpt_expand_dpd_data - Expansion ratio for EPDT should be 8. " );
    api_status = FALSE;
  }

  if ( (expansn_ratio ==2) && 
       ( (dpd_tbl_type == DPD_TYPE_AMPM)||(dpd_tbl_type == DPD_TYPE_EPDT) ) )
  {
    RF_MSG( RF_ERROR, "rf_common_xpt_expand_dpd_data - Expansion ratio for AMAM and AMPM should be 4. " );
    api_status = FALSE;
  }
   
  /* ------------Input parameters Checking END----------*/

  dpd_tbl_input = modem_mem_alloc((num_dpd_coeffs+1)*sizeof(uint16), MODEM_MEM_CLIENT_RFA);
  if (dpd_tbl_input == NULL)
  {
    RF_MSG( RF_ERROR, "rf_common_xpt_expand_dpd_data - malloc failed. Expansion aborted. " );
    api_status = FALSE;
  }
  if (api_status == TRUE)
  {
    /* Copy input table into the second entry and on*/
    memscpy((void*)(&dpd_tbl_input[1]), num_dpd_coeffs*sizeof(uint16),
            dpd_coeffs, num_dpd_coeffs*sizeof(uint16));

    switch (dpd_tbl_type)
    {
     case DPD_TYPE_AMPM:
      is_signed = TRUE;
      /* Assumign that AMPM tables will be expanded from 16 to 64 only. so expansion factor is 4*/
      ic_p = &ic_4[0];
      /* If the input table is an AMPM table then the input table is expanded as dpd_coeffs[-1]= dpd_coeffs[0].  */
      dpd_tbl_input[0] = dpd_tbl_input[1];
      break;

    case DPD_TYPE_AMAM:
       is_signed = 0;//FALSE;
       /* Assumign that AMAM tables will be expanded from 16 to 64 only. so expansion factor is 4*/
       if ( expansn_ratio == 2 )
       {
         ic_p = &ic_2[0];
       }
       else
       {
         ic_p = &ic_4[0];
       }
      /* If the input is a DPD table of type AMAM or EPDT, then the input DPD table dpd_coeffs[0..N-1] is expanded such that 
         dpd_coeffs[-1] = 0.*/
       dpd_tbl_input[0] = 0;
       break;

     case DPD_TYPE_EPDT:
      is_signed = 0;//FALSE;
      /* Assumign that EPDT tables will be expanded from 16 to 128 only. so expansion factor is 8*/
      ic_p = &ic_8[0];
      /* If the input is a DPD table of type AMAM or EPDT, then the input DPD table dpd_coeffs[0..N-1] is expanded such that 
         dpd_coeffs[-1] = 0.*/
      dpd_tbl_input[0] = dpd_tbl_input[1];
       break;

     case DPD_TYPE_GENERIC:
      is_signed = 0; //FALSE;
      ic_p = &ic_8[0];
      dpd_tbl_input[0] = 0;
      break;

    case DPD_TYPE_NONE:
      RF_MSG(RF_ERROR,"rf_common_xpt_expand_dpd_data:DPD type is NONE!");
      api_status = FALSE;
       break;
    }
  } /* if api_status == TRUE */
  if (api_status == TRUE)
  {
    /* outptu size = input size *expansion ratio */
    
    // 6/(h^2), Q32 , i.e. 6*2^32 / (h^2)
    //(int32) ((6LL << 32) / (128 / 16) / (128 / 16)))
    y_scale = (int32) ((6LL << 32) / expansn_ratio / expansn_ratio);

    /* 
    The tridiagonal algorithm uses 3 inputs at a time to compute y". in0, in1,in2 
    hold the 3 current inputs in a single iteration of the triadiagonal solution algorithm*/
    in0 = dpd_tbl_input[0];
    in1 = dpd_tbl_input[1];
    if (is_signed)
    {
      /* If its signed. Then sign extend to full variable bitwidth */
      in0 = Q6_R_sxth_R (in0);
      in1 = Q6_R_sxth_R (in1);
    }

    /* dmod = modified d parameters in triadiagonal algorithm or d~ , multiplied by a factor of 2^INTERP_FACTOR. */
    dmod[0] = 0;
    for (idx = 0; idx < num_dpd_coeffs - 1; idx++)
    {
      in2 = dpd_tbl_input[2 + idx];
      if (is_signed)
      {
        /* If its signed. Then sign extend to full variable bitwidth */
        in2 = Q6_R_sxth_R (in2);
      }
      delta = ((in1 - in2) + (in1 - in0)) << INTERP_QFACTOR;
      /* Q6_R_mpy_RR multiplies 32 bit numbers and puts the result in another 32 bit integer. i.e. truncates the last 32 bits
       d~[i+1] = [ d~[i] + (delta*yscale) ]*c~[i+1] 
      */
      dmod[idx + 1] = Q6_R_mpy_RR (dmod[idx] + Q6_R_mpy_RR (delta, y_scale),
                                cmod[idx + 1]);
      in0 = in1;
      in1 = in2;
    }
    /* BACK SUBSTITUTION LOOP of tridiagonal algorithm. Computes y". Variable y2 holds y" */
    y2[num_dpd_coeffs] = 0;
    for (idx = num_dpd_coeffs - 1; idx >= 0; idx--)
    {
      y2[idx] = Q6_R_mpy_RR (y2[idx + 1], cmod[idx]) + dmod[idx];
    }

   /* FINAL EXPANSION LOOP:-
       y = A*y[i] + B*y[i+1] + C*y"[i] + D*y"[i+1] ------------ (EQN 3)
     i.e dpd_tbl_expanded = A*dpd_tbl_input[i] + B*dpd_tbl_input[i+1] + C*y2[i] + D*y2[i+1] 
   */

   /* Temporary variables during expansion loop.
       y_lo = y[i] = dpd_tbl_input[i]
       y_hi = y[i+1] = dpd_tbl_input[i+1]
       y2_lo = y"[i] = y2[i]
       y2_hi = y"[i+1] = y2[i+1]
    */
    y_lo = dpd_tbl_input[0];
    if (is_signed)
    {
      /* sign extend */ 
      y_lo = Q6_R_sxth_R (y_lo);
    }
    y_lo = y_lo << INTERP_QFACTOR;
    y2_lo = y2[0];
 
    for (idx = 0; idx < num_dpd_coeffs; idx++)
    {
      ic = ic_p;
     
      y_hi = dpd_tbl_input[idx + 1];
      if (is_signed)
      {
        /* Sign extend */
        y_hi = Q6_R_sxth_R (y_hi);
      }
      y_hi =  y_hi<< INTERP_QFACTOR;
      y2_hi = y2[idx + 1];
      for (k = 0; k < expansn_ratio; k++, ic += 4)
      {
        int64 val;
        int32 val32;
        int32 offset_val32;
        val = (1LL << (INTERP_QFACTOR + IC_QFACTOR - 1)); /* rounding factor - Qfactor of result/2. */
        val = Q6_P_mpyacc_RR (val, y_lo, ic[0]); /* rounding factor + A* y[i] */
        val = Q6_P_mpyacc_RR (val, y_hi, ic[1]); /* rounding factor + A* y[i]  +B*y[i+1]*/
        val = Q6_P_mpyacc_RR (val, y2_lo, ic[2]);/* rounding factor + A* y[i]  +B*y[i+1] + C*y"[i]*/
        val = Q6_P_mpyacc_RR (val, y2_hi, ic[3]);/* rounding factor + A* y[i]  +B*y[i+1] + C*y"[i] + D*y2[i+1]*/
        val32 = Q6_P_asr_PR (val, INTERP_QFACTOR + IC_QFACTOR); /* shift down by the factors multiplied to get final value */ 

        if (is_signed)
        {
          /* saturate sign-extended */   
          val32 = Q6_R_sath_R (val32);
        }
        else
        {
          /* saturate unsigned */
          val32 = Q6_R_satuh_R (val32);
        }
        if ((dpd_tbl_type == DPD_TYPE_AMPM) && 
            (dpd_scale_factor->valid == TRUE))
        {
          offset_val32 = Q6_R_asrrnd_RI(Q6_P_asr_PR(Q6_P_mpy_RR(val32,
                                                                dpd_scale_factor->scale_value),
                                                                DPD_SCALING_Q_FACTOR-1),1);
          *dpd_tbl++ = (uint16) offset_val32;
        }
        else
        {
          *dpd_tbl++ = (uint16) val32;
        }
      }
      y_lo = y_hi;
      y2_lo = y2_hi;
    }
  }/* if api_status == TRUE*/
  modem_mem_free(dpd_tbl_input,MODEM_MEM_CLIENT_RFA);
  return api_status;
} /* rf_common_xpt_expand_dpd_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Populates DPD data
 
  @details
  Invokes expansion of DPD coefficients.
  Invokes programming of expanded DPD.
  Provides DPD location info to caller.
 
  @param device
  Device for which to populate DPD.
 
  @param tx_lin_srch_params
  Search conditions for linearizer
  
  @param tx_band_cal_data
  Tx Multilin data

  @param tx_final_linearizer_data
  Linearizer table to populated
 
  @param amam_bank_info
  OUTPUT. AMAM DPD bank_info
 
  @param ampm_bank_info
  OUTPUT. AMPM DPD bank_info
 
  @param epdt_bank_info
  OUTPUT. EPDT DPD bank_info
 
  @param num_rgi_with_dpd_info
  OUTPUT. The number of RGIs actually populated with DPD info

  @return
  TRUE if successful, else FALSE
*/
boolean 
rfcommon_core_populate_tx_dpd
(
  rfm_device_enum_type device,
  tx_linearizer_table_data_type * tx_final_linearizer_data,
  rfcommon_xpt_dpd_bank_info_type* amam_bank_info,
  rfcommon_xpt_dpd_bank_info_type* ampm_bank_info,
  rfcommon_xpt_dpd_bank_info_type* epdt_bank_info,
  uint16* num_rgi_with_dpd_info,
  xpt_dpd_scaling_value_type *dpd_scale_factor,
  xpt_expansion_ratio_type *expansion_ratio_p
)
{
  boolean api_status = TRUE;
  uint16 num_dpd_coeffs = 0;
  xpt_am_table_type* am_coeffs = NULL;
  xpt_pm_table_type* pm_coeffs = NULL;
  xpt_dt_table_type* dt_coeffs = NULL;
  uint8  bank_index;
  uint16 bank_offset;
  uint16 amam_tbl[64];
  uint16 ampm_tbl[64];
  uint16 epdt_tbl[128];
  uint16 lin_row_idx;
  uint16 dpd_idx;
  rfcommon_mdsp_xpt_mode_enum_type  load_dpd_xpt_mode = 0;
  xpt_expansion_ratio_type expansion_ratio;

  /* lin_dpd_lut table will hold unique dpd index in the linearizer with the corresponding linearizer index */
  uint8 lin_dpd_lut[RFCOM_TXLIN_NUM_SEGMENTS];

  /* Initialized to 0xFF */
  memset(lin_dpd_lut,0xFF,sizeof(uint8)*RFCOM_TXLIN_NUM_SEGMENTS);
  
  if ( expansion_ratio_p == NULL )
  {
    expansion_ratio.amam = ARR_SIZE(amam_tbl)/XPT_DPD_NUM_COEFF;
    expansion_ratio.ampm = ARR_SIZE(ampm_tbl)/XPT_DPD_NUM_COEFF;
    expansion_ratio.epdt = ARR_SIZE(epdt_tbl)/XPT_DPD_NUM_COEFF;
  }
  else
  {
    expansion_ratio.amam = expansion_ratio_p->amam;
    expansion_ratio.ampm = expansion_ratio_p->ampm;
    expansion_ratio.epdt = expansion_ratio_p->epdt;
  }
  RF_MSG_3( RF_HIGH, "rfcommon_core_populate_tx_dpd - expansion_ratio: amam %d ampm %d epdt %d.",
            expansion_ratio.amam, expansion_ratio.ampm, expansion_ratio.epdt );

  /* Check Params */
  if (tx_final_linearizer_data == NULL)
  {
    RF_MSG( RF_ERROR, "rfcommon_core_populate_tx_dpd - NULL final lin data." );
    return FALSE;
  }

  if (tx_final_linearizer_data->lin_data == NULL)
  {
    RF_MSG( RF_ERROR, "rfcommon_core_populate_tx_dpd - NULL linearizer table." );
    return FALSE;
  }

  if (tx_final_linearizer_data->xpt_data == NULL)
  {
    RF_MSG( RF_ERROR, "rfcommon_core_populate_tx_dpd - NULL xpt data table." );
    return FALSE;
  }

  if ((amam_bank_info == NULL) || (ampm_bank_info == NULL))
  {
    RF_MSG( RF_ERROR, "rfcommon_core_populate_tx_dpd - NULL AMAM or AMPM bank info.");
    return FALSE;
  }

  if(tx_final_linearizer_data->lin_data->num_dpd > RFCOM_TXLIN_NUM_SEGMENTS)
  {
    RF_MSG_2( RF_ERROR, "rfcommon_core_populate_tx_dpd - Num DPD %d exceeds %d.",
              tx_final_linearizer_data->lin_data->num_dpd,
              RFCOM_TXLIN_NUM_SEGMENTS );
    return FALSE;
  }

  if (num_rgi_with_dpd_info == NULL)
  {
    RF_MSG( RF_ERROR, "rfcommon_core_populate_tx_dpd - NULL param: num_rgi_with_dpd_info" );
    return FALSE;
  }
  else
  {
    *num_rgi_with_dpd_info = 0;
  }

  /* Check for supported DPD format */
  switch( tx_final_linearizer_data->aux_lin_data )
  {
    case TX_LINEARIZER_NO_AUX_DATA:
      /* This API is allowed to be called with "no aux data" 
         in which case, just return immediately with "zero dpds used" 
         as set just above */
      return TRUE;
    break;

    case TX_LINEARIZER_EPT_DPD_AUX_DATA:
      /* NOT SUPPORTED! SHOULD NEVER GET HERE */
      RF_MSG_1( RF_ERROR, "rfcommon_core_populate_tx_dpd - Obsolete DPD type %d not supported.",
                tx_final_linearizer_data->aux_lin_data );
      return FALSE;
    break;

    case TX_LINEARIZER_EPT_DPD_V2_DATA:
      num_dpd_coeffs = tx_final_linearizer_data->lin_data->num_dpd;
      am_coeffs = tx_final_linearizer_data->xpt_data->xpt_am_table;
      pm_coeffs = tx_final_linearizer_data->xpt_data->xpt_pm_table;

      load_dpd_xpt_mode = RFCOMMON_MDSP_XPT_MODE_EPT;
    break;

     case TX_LINEARIZER_ET_DPD_DATA:
       {
         if (epdt_bank_info == NULL)
         {
           RF_MSG( RF_ERROR, "rfcommon_core_populate_tx_dpd - NULL EPDT bank info.");
           return FALSE;
         }
         num_dpd_coeffs = tx_final_linearizer_data->lin_data->num_dpd;
         am_coeffs = tx_final_linearizer_data->xpt_data->xpt_am_table;
         pm_coeffs = tx_final_linearizer_data->xpt_data->xpt_pm_table;
         dt_coeffs = tx_final_linearizer_data->xpt_data->xpt_dt_table;

         load_dpd_xpt_mode = RFCOMMON_MDSP_XPT_MODE_ET;
       }
    break;

    default:
      RF_MSG_1( RF_ERROR, "rfcommon_core_populate_tx_dpd - Linearizer Aux Data "
                "Type %d not supported.",
                tx_final_linearizer_data->aux_lin_data );
      return FALSE;
    break;
  }

  if (api_status == TRUE)
  {
    /* for (each lin_row_idx) */
    for ( lin_row_idx = 0; 
          (lin_row_idx < RFCOM_TXLIN_NUM_SEGMENTS) && (api_status == TRUE); 
          lin_row_idx++)
    {
      /* Bounds check */
      dpd_idx = tx_final_linearizer_data->lin_data->table[lin_row_idx].dpd_index;
      if(dpd_idx >= num_dpd_coeffs)
      {
        /* Error if the first item starts out as invalid. 
           Othersiwe, subsequent rows can have invalid vals, if so, just break */
        if (lin_row_idx == 0)
        {
          RF_MSG_2( RF_ERROR, "rfcommon_core_populate_tx_dpd - DPD idx %d must < %d",
                    dpd_idx, num_dpd_coeffs );
          api_status = FALSE;
        }
        if ((lin_row_idx+1) < num_dpd_coeffs)
        {
          RF_MSG_2( RF_ERROR, "rfcommon_core_populate_tx_dpd - Num lin rows (%d) "
                    "populated with DPD info is less than the expected number of "
                    "total DPDs (%d) avail for this linearizer.",
                    (lin_row_idx+1), num_dpd_coeffs );
          api_status = FALSE;
        }
        break;
      }

      /* Process DPD only for unique index */
      if (lin_dpd_lut[dpd_idx] == 0xFF)
      {
        /* store new unique DPD idx and its corresponding lin_row_idx in a table */
        lin_dpd_lut[dpd_idx] = lin_row_idx;

        /* SAVE AMAM */
        if (api_status == TRUE)
        {
          api_status &= rf_common_xpt_expand_dpd_data( 
                           DPD_TYPE_AMAM, XPT_DPD_NUM_COEFF,
                           (uint16*) am_coeffs->xpt_am_row[dpd_idx], 
                           expansion_ratio.amam, amam_tbl,
                           dpd_scale_factor );
          /* Perform padding if the DPD table is bigger than the total
             number of interpolated coefficients */
          if ( ARR_SIZE(amam_tbl) > (expansion_ratio.amam*XPT_DPD_NUM_COEFF) )
          {
            rfcommon_xpt_dpd_padding( DPD_TYPE_AMAM,
                                      ARR_SIZE(amam_tbl),
                                      expansion_ratio.amam*XPT_DPD_NUM_COEFF,
                                      amam_tbl );
          } /* if ( ARR_SIZE(amam_tbl) > (expansion_ratio.amam*XPT_DPD_NUM_COEFF) ) */

          if ( rfcommon_dpd_debug_mode_en == TRUE )
          {
            rfcommon_xpt_dpd_debug( DPD_TYPE_AMAM,
                                    ARR_SIZE(amam_tbl),
                                    amam_tbl );
          } /* if ( rfcommon_dpd_debug_mode_en == TRUE ) */
        }
        if (api_status == TRUE)
        {
          api_status &=
                  rf_common_xpt_load_dpd_tbls(device, 
                                              amam_tbl,
                                              ARR_SIZE(amam_tbl),
                                              DPD_TYPE_AMAM,
                                              &bank_index,
                                              &bank_offset, 
                                              XPT_DPD_ACTION_SAVE,
                                              load_dpd_xpt_mode);

          amam_bank_info[lin_row_idx].bank_id     = bank_index;
          amam_bank_info[lin_row_idx].bank_offset = bank_offset;
        }

        /* SAVE AMPM */
        if (api_status == TRUE)
        {
          api_status &= rf_common_xpt_expand_dpd_data( 
                           DPD_TYPE_AMPM, XPT_DPD_NUM_COEFF,
                           (uint16*)pm_coeffs->xpt_pm_row[dpd_idx], 
                           expansion_ratio.ampm, ampm_tbl,
                           dpd_scale_factor );
        }
        if (api_status == TRUE)
        {
          api_status &=
                  rf_common_xpt_load_dpd_tbls(device, 
                                              ampm_tbl,
                                              ARR_SIZE(ampm_tbl),
                                              DPD_TYPE_AMPM,
                                              &bank_index,
                                              &bank_offset, 
                                              XPT_DPD_ACTION_SAVE,
                                              load_dpd_xpt_mode);

          ampm_bank_info[lin_row_idx].bank_id     = bank_index;
          ampm_bank_info[lin_row_idx].bank_offset = bank_offset;
        }

        /* SAVE EPDT */
        if( (tx_final_linearizer_data->aux_lin_data == TX_LINEARIZER_ET_DPD_DATA) &&
            (epdt_bank_info != NULL) )
        {
          if (api_status == TRUE)
          {
            api_status &= rf_common_xpt_expand_dpd_data( 
                             DPD_TYPE_EPDT, XPT_DPD_NUM_COEFF,
                             (uint16*)dt_coeffs->xpt_dt_row[dpd_idx],
                             expansion_ratio.epdt, epdt_tbl,
                             dpd_scale_factor );
          }
          if (api_status == TRUE)
          {
            api_status &=
                    rf_common_xpt_load_dpd_tbls(device, 
                                                epdt_tbl,
                                                ARR_SIZE(epdt_tbl),
                                                DPD_TYPE_EPDT,
                                                &bank_index,
                                                &bank_offset, 
                                                XPT_DPD_ACTION_SAVE,
                                                load_dpd_xpt_mode);

            epdt_bank_info[lin_row_idx].bank_id     = bank_index;
            epdt_bank_info[lin_row_idx].bank_offset = bank_offset;
          }
        }
      } 
      else /* DPD idx is already processed and present  */
      {
        amam_bank_info[lin_row_idx].bank_id     = amam_bank_info[lin_dpd_lut[dpd_idx]].bank_id;
        amam_bank_info[lin_row_idx].bank_offset = amam_bank_info[lin_dpd_lut[dpd_idx]].bank_offset ;
        ampm_bank_info[lin_row_idx].bank_id     = ampm_bank_info[lin_dpd_lut[dpd_idx]].bank_id;
        ampm_bank_info[lin_row_idx].bank_offset = ampm_bank_info[lin_dpd_lut[dpd_idx]].bank_offset ;
        epdt_bank_info[lin_row_idx].bank_id     = epdt_bank_info[lin_dpd_lut[dpd_idx]].bank_id;
        epdt_bank_info[lin_row_idx].bank_offset = epdt_bank_info[lin_dpd_lut[dpd_idx]].bank_offset;
      }
    } /* for (each lin_row_idx) */
  }

  if (api_status == TRUE)
  {
    /* Update only is all went OK */
    *num_rgi_with_dpd_info = lin_row_idx;
  }

  return api_status;
} /* rfcommon_core_populate_tx_dpd */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Performs temp-comp on DPD data
 
  @param tx_lut
  Original Tx linearizer table
 
  @param temp_comp_lut
  Compensated LUT

  @param first_sweep_lut
  Full-bias LUT
 
  @param aux_data
  Indicates the type of data contained.
 
  @param temp_comp_val
  Compensation value
 
  @param valid_lut_entries
  The number of valid LUT entries

  @return
  TRUE if freq comp was successful, else FALSE
*/
boolean
rfcommon_core_txlin_temp_comp_update_dpd
(
  tx_linearizer_table_type *tx_lut,
  tx_linearizer_table_type *temp_comp_lut,
  tx_linearizer_table_type *first_sweep_lut,
  tx_linearizer_aux_data_type aux_data,
  int16 temp_comp_val,
  uint8 valid_lut_entries
)
{
  uint8 lut_index,iq_local,valid_first_lut_entries,fs_lut_index,cnt,lut_beg;
  int16 new_power = 0;

  /* Check for NULL pointers */
  if (first_sweep_lut == NULL )
  {
    RF_MSG( RF_ERROR, "Common Txlin temp comp update: NULL pointer in First sweep LUT!");   
    return FALSE;
  }
  
  /* Step 1 - Finding the no. of valid entries in first sweep */
  for ( cnt = 0; cnt< RFCOM_TXLIN_NUM_SEGMENTS; cnt++ )
  {  
    if(first_sweep_lut->table[cnt].rgi == 0xFFFF)
    {
      break;
    }
  }

  valid_first_lut_entries = cnt;
  if (valid_first_lut_entries == 0)
  {
    RF_MSG( RF_ERROR, "rfcommon_core_txlin_temp_comp_update_dpd: No valid linearizer entries");
    return FALSE;
  }
  for ( lut_index = 0; lut_index< valid_lut_entries; lut_index++ )
  {
    /* check Invalid entry in the LUT */
    if ( tx_lut->table[lut_index].rgi != 0xFFFF )
    {
      fs_lut_index = lut_index;

      /* Copy over DPD data */
      if( aux_data != TX_LINEARIZER_NO_AUX_DATA )
      {
        /* Copy over DPD index */
        temp_comp_lut->table[lut_index].dpd_index = tx_lut->table[lut_index].dpd_index;

        /* TODO: now that we stopped messing with first sweep data in process linearizer,
           clean up this code to search from 0 and stop at 0xFF */    
        for( cnt = 0; cnt < valid_first_lut_entries; cnt++ )
        {
          /* Find FS index */
          if( first_sweep_lut->table[cnt].rgi == tx_lut->table[lut_index].rgi )
          {
            fs_lut_index = cnt;
            break;
          }
        } /* for( cnt ) */

        if( cnt == valid_first_lut_entries )
        {
          RF_MSG_1( RF_HIGH, "Could not find RGI %d in first sweep..Things will be messy herein",
                    tx_lut->table[lut_index].rgi );
        }
      } /* Copy over DPD data */

      /* Copy over fcomp index */
      temp_comp_lut->table[lut_index].fcomp_idx = tx_lut->table[lut_index].fcomp_idx;

      /* calculating the new power with temp comp val */
      new_power = first_sweep_lut->table[fs_lut_index].power + (-1)*temp_comp_val;
      /* copying the power as such with no changes; Only RGI will change */
      temp_comp_lut->table[lut_index].power = tx_lut->table[lut_index].power;

      /* Only RGI will be replaced in the new second sweep LUT */
      if( new_power >= first_sweep_lut->table[valid_first_lut_entries-1].power )
      {
        /* Bias should be based on second sweep */
        temp_comp_lut->table[lut_index].apt = tx_lut->table[valid_lut_entries-1].apt;
        /* RGI should be coming from first sweep; Need to check this with Dimeng */
        temp_comp_lut->table[lut_index].rgi = first_sweep_lut->table[valid_first_lut_entries-1].rgi;
        /* Set IQ offset to zero */
        temp_comp_lut->table[lut_index].iq_offset = 0;

      } /* if(new_power >=  power) */
      else if ( new_power <= first_sweep_lut->table[0].power )
      {
        /* Bias chosen from second sweep */
        temp_comp_lut->table[lut_index].apt = tx_lut->table[0].apt;
        /* RGI from first sweep */
        temp_comp_lut->table[lut_index].rgi = first_sweep_lut->table[0].rgi;
        /* Calc IQ based on diff */
        iq_local = first_sweep_lut->table[0].power - new_power;
        iq_local = (iq_local > 63) ? 63 : iq_local;
        temp_comp_lut->table[lut_index].iq_offset = iq_local;

      }/* if(new_power <=  power) */
      else
      {
        /* Linear search implementation
           Start from fs_lut_index instead of lut_index as we need to start from matching RGI
           From here, we can go up or down based on temp comp val */
        lut_beg = fs_lut_index;
        while( lut_beg <= (valid_first_lut_entries-1) && (temp_comp_val != 0) )
        {
          if( ( lut_beg != 0 ) && 
              ( first_sweep_lut->table[lut_beg].power >= new_power ) &&
              ( first_sweep_lut->table[lut_beg-1].power < new_power ) )
          {
            break;
          }
          else
          {
            /* if temp comp is negative, search for lower indices,
               if temp comp is positive , search for higher indices */
            (temp_comp_val < 0) ? (lut_beg++) : (lut_beg--);
          }
        } /* while() */

        /* Here we find the exact RGI for the new power */
        temp_comp_lut->table[lut_index].rgi = first_sweep_lut->table[lut_beg].rgi;
        /* storing the difference as IQ offset */
        iq_local = ( first_sweep_lut->table[lut_beg].power - new_power );
        iq_local = ( iq_local > 63 ) ? 63 : iq_local;
        temp_comp_lut->table[lut_index].iq_offset = iq_local;
        temp_comp_lut->table[lut_index].apt = tx_lut->table[lut_index].apt;

      } /* else */

    } /* rgi != 0xFFFF */
    else /* rgi == 0xFFFF */
    {
      /* For invalid RGIs, copy the second sweep LUT entries to the temp-comp'ed LUT */
      temp_comp_lut->table[lut_index].power= tx_lut->table[lut_index].power;
      temp_comp_lut->table[lut_index].rgi = tx_lut->table[lut_index].rgi;
      temp_comp_lut->table[lut_index].iq_offset = tx_lut->table[lut_index].iq_offset;
      temp_comp_lut->table[lut_index].dpd_index = tx_lut->table[lut_index].dpd_index;
      temp_comp_lut->table[lut_index].fcomp_idx = tx_lut->table[lut_index].fcomp_idx;
      temp_comp_lut->table[lut_index].apt = tx_lut->table[lut_index].apt;

    } /* rgi == 0xFFFF */

  } /* for(lut_index ) */

  /* Copy items from the original table */
  temp_comp_lut->chan_freq = tx_lut->chan_freq;
  temp_comp_lut->num_fcomp = tx_lut->num_fcomp;
  temp_comp_lut->fcomp_type = tx_lut->fcomp_type;
 
  return TRUE; 
} /* rfcommon_core_txlin_temp_comp_update_dpd */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initializes DPD data variables and HW
 
  @details
  Should always be the first DPD API to call when preparing to
  save and program DPD.
 
  Clears dpd data variables and frees any allocated dpd HW. 
 
  @param device
  Device corresponding to the DPD action.
 
  @return
  TRUE when success, else FALSE
*/
boolean
rf_common_xpt_init_dpd
(
   rfm_device_enum_type device
)
{
  boolean api_status = TRUE;
  uint8 amam_bank_index;
  uint16 amam_bank_offset;

  /* Clear dpd tables of any previous data */
  api_status &= 
    rf_common_xpt_load_dpd_tbls(device, 
                                NULL, 
                                0, 
                                0, 
                                &amam_bank_index, 
                                &amam_bank_offset, 
                                XPT_DPD_ACTION_INIT,
                                RFCOMMON_MDSP_XPT_MODE_EPT);

  api_status &= 
    rf_common_xpt_load_dpd_tbls(device, 
                                NULL, 
                                0, 
                                0, 
                                &amam_bank_index, 
                                &amam_bank_offset, 
                                XPT_DPD_ACTION_INIT,
                                RFCOMMON_MDSP_XPT_MODE_ET);

  return api_status;
}/* rf_common_xpt_init_dpd */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Saves DPD data in variables for later use. No HW action.
 
  @pre rf_common_xpt_init_dpd
 
  @param device
  Device corresponding to the DPD action.
 
  @param dpd_tbl_ptr
  Table with DPD information to save
 
  @param dpd_tbl_sz
  Number of elements in table to save
 
  @param tbl_type
  The type of DPD being saved
 
  @param bank_index
  OUTPUT. The bank where the data was saved
 
  @param bank_offset
  OUTPUT. The offset within the bank where the data was saved
 
  @return
  TRUE when success, else FALSE
*/
boolean
rf_common_xpt_save_dpd_data
(
  rfm_device_enum_type device,
  uint16* dpd_tbl_ptr,
  uint16  dpd_tbl_sz,
  load_dpd_input_table_type tbl_type,
  uint8* bank_index,
  uint16* bank_offset
)
{
  RF_MSG( RF_ERROR, "rf_common_xpt_save_dpd_data: Deprecated API stubbed out" );
  return TRUE;
} /* rf_common_xpt_save_dpd_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Commits the saved DPD data to HW.
 
  @pre rf_common_xpt_save_dpd_data
 
  @param device
  Device corresponding to the DPD action.
 
  @return
  TRUE when success, else FALSE
*/
boolean
rf_common_xpt_commit_dpd_data
(
  rfm_device_enum_type device
)
{
  boolean api_status = TRUE;
  uint8 amam_bank_index;
  uint16 amam_bank_offset;

  api_status &= 
    rf_common_xpt_load_dpd_tbls(device, 
                                NULL, 
                                0, 
                                0, 
                                &amam_bank_index, 
                                &amam_bank_offset, 
                                XPT_DPD_ACTION_COMMIT,
                                RFCOMMON_MDSP_XPT_MODE_EPT);

  return api_status;
}/* rf_common_xpt_commit_dpd_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Finds bank with space and compatible DPD data
 
  @param device
  Device corresponding to the DPD action.

  @param start_bank_num
  The first bank to check for space and compatibility
 
  @param tbl_type
  The type of data about to be saved

  @param rem_space_in_bnk
  Output: Remaining space in bank
 
  @return
  The available bank
*/
uint8
rf_common_xpt_find_nxt_bank_with_space
(
   rfm_device_enum_type device,
   uint8 start_bank_num,
   load_dpd_input_table_type tbl_type,
   uint16 *rem_space_in_bnk
)
{
  rfcommon_data_tx_status_type* tx_status = NULL;
  uint8 bank_id;

  /* Get tx status */
  tx_status = rfcommon_data_get_tx_status( device );

  if (tx_status == NULL)
  {
    RF_MSG( RF_ERROR, "rf_common_xpt_find_nxt_bank_with_space: NULL Tx Data." );
    return MAX_DPD_BANKS;
  }

  /* Find available bank */
  for (bank_id=start_bank_num; bank_id<MAX_DPD_BANKS; bank_id++)
  {
    /* If bank already in use */
    if (tx_status->xpt_dpd_data.bank_used_mask & (1<<bank_id)) 
    {
      /* If current bank is full or 
         dpd table not allowed in bank */
      if ( (tx_status->xpt_dpd_data.bank_full_mask & (1<<bank_id))  || 
          (rf_common_xpt_bank_allows_table_type(device,bank_id, tbl_type) == FALSE) )
      {
        continue;
      } 
      else
      {
        /* Found bank */
        *rem_space_in_bnk = MAX_DPD_BANK_SIZE - tx_status->xpt_dpd_data.bank_used_space[bank_id]; 
        break;
      }
    }
    else
    {
      /* This is a new bank */
      tx_status->xpt_dpd_data.bank_used_mask |= (0x1<<bank_id);
      *rem_space_in_bnk = MAX_DPD_BANK_SIZE;
      tx_status->xpt_dpd_data.bank_used_space[bank_id] = 0;
      break;
    }

  }/* end for */ 
  return bank_id;
}/* rf_common_xpt_find_nxt_bank_with_space */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Determine if DPD data to be saved is compatible with existing data.
 
  @param device
  Device corresponding to the DPD action.
 
  @param bank_id
  The bank to check.
 
  @param tbl_type
  The type of data about to be saved
 
  @return
  TRUE if data is compatible; FALSE otherwise
*/
boolean
rf_common_xpt_bank_allows_table_type
(
   rfm_device_enum_type device,
   uint8 bank_id,
   load_dpd_input_table_type tbl_type
)
{
  rfcommon_data_tx_status_type* tx_status = NULL;
  //int k;
  boolean tbl_allowed = FALSE;

  /* Get tx status */
  tx_status = rfcommon_data_get_tx_status( device );

  if (tx_status == NULL)
  {
    RF_MSG( RF_ERROR, "rf_common_xpt_bank_allows_table_type: NULL Tx Data." );
    return FALSE;
  }

  /* Indicate whether the new table is compatible with
     data already saved */
  //for (k = 0 ; k < MAX_DPD_TBLS_PER_BANK; k++)
  {
    if( (tx_status->xpt_dpd_data.dpd_types_in_bank[bank_id][0] == DPD_TYPE_NONE) ||
        (tx_status->xpt_dpd_data.dpd_types_in_bank[bank_id][0] == tbl_type ) )
    {
      tbl_allowed = TRUE;
      //break;
    }
  }
  return tbl_allowed;
} /* rf_common_xpt_bank_allows_table_type */

/*----------------------------------------------------------------------------*/
/*!
  @brief
   1. Loop through all the number of channels enabled,
   2. Re-align 2D matrix based on the Calibration temperature.
   
  @param rfnv_tx_delay_offsets_type
  Pointer to the structure with tx_carrier*_static offsets.
 
  @param therm_bin_array
  Pointer to the structure with temperature based bin values.

  @param calibration_temp_read
  Calibration Temp read which is scaled Temp read for some techs and
  unscaled for some techs.
 
  @return
  TRUE if data is usable; FALSE otherwise
*/

boolean 
rf_common_xpt_calibration_based_delay_offset_realignment
( 
  rfnv_tx_delay_offsets_type *offset_nv_ptr,
  const uint16 *therm_bin_array,
  uint16 calibration_temp_read
)
{

  boolean api_status = FALSE;
  uint8 i = 0, j = 0, k = 0;
  uint8 num_of_channels_enabled_for_delay_offset = 0;
  int16 calibration_ref_delay_offset = 0;
  int16 delay_offset_temp_array[XPT_TEMP_BIN_SIZ] = {0,0,0,0,0,0,0,0};
  
  if ((offset_nv_ptr == NULL) ||
      (therm_bin_array == NULL ))
  {
    RF_MSG( RF_ERROR, "rf_common_xpt_calibration_based_delay_offset_realignment offset_nv_ptr "
                      " or therm_bin_array or final_delay_offset is NULL, Check NV" );
    return api_status;
  }

  /* Assign the channels enabled from the NV */
  num_of_channels_enabled_for_delay_offset = offset_nv_ptr->channels_enabled;

  RF_MSG_2( RF_MED, 
            "rf_common_xpt_calibration_based_delay_offset_realignment"
            ", calibration_temp_read = %d,"
            ", Num of Channels enabled = %d ",
            calibration_temp_read, num_of_channels_enabled_for_delay_offset);

  /* Scale the entire Maxtrix based on Calibration Temp */
  for (i = 0; i < num_of_channels_enabled_for_delay_offset; i++)
  {
    for (j = 0; j<XPT_TEMP_BIN_SIZ; j++)
    {
      /* Calculate the Temp delay offset array based on current Freq selected */
      delay_offset_temp_array[j] = (offset_nv_ptr->delay_offset_vs_temp_vs_freq[j][i]);
    }

    /* Find the Reference Calibration Offset based on the Calibration Temp ADC*/
    calibration_ref_delay_offset = rf_lininterp_u16_s16(therm_bin_array,
                                                       &delay_offset_temp_array[0],
                                                       XPT_TEMP_BIN_SIZ,
                                                       calibration_temp_read);
    /* Proceed with Temp re-alignment */
    for (k = 0; k <XPT_TEMP_BIN_SIZ; k++ )
    {
      offset_nv_ptr->delay_offset_vs_temp_vs_freq[k][i] = 
            offset_nv_ptr->delay_offset_vs_temp_vs_freq[k][i] - 
        calibration_ref_delay_offset;      
    }      
  }
  api_status = TRUE;

  return api_status;
} /* rf_common_xpt_calibration_based_delay_offset_realignment */

/*----------------------------------------------------------------------------*/
/*!
  @brief
   1. Loop through all the number of channels enabled,
   2. Calculate 1D matrix based on the Current temperature which will
      provide simple Freq look up.
   
  @param rfnv_tx_delay_offsets_type
  Pointer to the structure with tx_carrier*_static offsets.
 
  @param therm_bin_array
  Pointer to the structure with temperature based bin values.

  @param current_temp_read
  Current temp read which is Scaled for some techs and unscaled for some techs.
 
  @param *delay_vs_freq_array
  1D array passed to store the Freq based delay reads interpolated over temp.
 
  @return
  TRUE if data is usable; FALSE otherwise
*/

boolean 
rf_common_xpt_get_delay_offset_array_for_current_temp
( 
  rfnv_tx_delay_offsets_type *offset_nv_ptr,
  const uint16 *therm_bin_array,
  uint16 current_temp_read,
  int16 *delay_vs_freq_array
)
{

  boolean api_status = FALSE;
  uint8 i = 0, j = 0;
  uint8 num_of_channels_enabled_for_delay_offset = 0;
  int16 current_delay_offset = 0;
  int16 delay_offset_temp_array[XPT_TEMP_BIN_SIZ] = {0,0,0,0,0,0,0,0};
  
  if ((offset_nv_ptr == NULL) ||
      (therm_bin_array == NULL )|| 
      (delay_vs_freq_array == NULL))
      
  {
    RF_MSG( RF_ERROR, 
            "rf_common_xpt_get_delay_offset_array_for_current_temp offset_nv_ptr "
            " or therm_bin_array or delay_vs_freq_array is NULL, Check Params" );
    return api_status;
  }

  /* Assign the channels enabled from the NV */
  num_of_channels_enabled_for_delay_offset = offset_nv_ptr->channels_enabled;

  RF_MSG_2( RF_MED, 
            "rf_common_xpt_get_delay_offset_array_for_current_temp"
            ", current_temp_read = %d,"
            ", Num of Channels enabled = %d ",
            current_temp_read, num_of_channels_enabled_for_delay_offset);

  for (i = 0; i < num_of_channels_enabled_for_delay_offset; i++)
  {
    for (j = 0; j<XPT_TEMP_BIN_SIZ; j++)
    {
      /* Calculate the Temp delay offset array based on current Freq selected */
      delay_offset_temp_array[j] = 
        (offset_nv_ptr->delay_offset_vs_temp_vs_freq[j][i]);
    }

    /* Find the Current Offset based on the Current Temp read */
    current_delay_offset = rf_lininterp_u16_s16(therm_bin_array,
                                               &delay_offset_temp_array[0],
                                               XPT_TEMP_BIN_SIZ,
                                               current_temp_read);

    delay_vs_freq_array[i] = current_delay_offset;  
    
  }
  api_status = TRUE;
  
  return api_status;
} /* rf_common_xpt_get_delay_offset_array_for_current_temp */


/*----------------------------------------------------------------------------*/
/*!
  @brief
   1. Check for current tx channel to be within a valid upper bound channel,
      if not return error.
   2. Find out of the frequency row to be selected based on valid output of step 1.
   3. Return the offset value after Freq lookup.
 
  @param rfnv_tx_delay_offsets_type
  Pointer to the structure with tx_carrier*_static offsets.
 
  @param *delay_vs_freq_array
  1D array passed to store the Freq based delay reads interpolated over temp.
 
  @param current_tx_freq
  Current operating Tx Frequency.
 
  @param final_delay_offset
  pointer to final computed offset.
 
  @param convert_freq_handler
  Function pointer to compute Tx channel to Tx freq conversion.
 
  @return
  TRUE if data is usable; FALSE otherwise
*/

boolean 
rf_common_xpt_delay_offset_computation_v2
( 
  rfnv_tx_delay_offsets_type *offset_nv_ptr,
  int16 *temp_delay_vs_temp_vs_freq,
  uint32 current_tx_freq,
  int16 *final_delay_offset,
  rfcommon_core_convert_freq_handler_type convert_freq_handler
)
{

  boolean api_status = FALSE;
  uint8 i = 0, col_index = XPT_PATH_DELAY_ARRAY_SIZ;
  uint32 current_bin_upper_bound_tx_freq = 0;
  uint8 num_of_channels_enabled_for_delay_offset = 0;
  
  if ((offset_nv_ptr == NULL) ||
      (final_delay_offset == NULL)||
      (temp_delay_vs_temp_vs_freq == NULL))
  {
    RF_MSG( RF_ERROR, "rf_common_xpt_delay_offset_computation_v2 offset_nv_ptr "
                      " offset_nv_ptr or "
                      " final_delay_offset or "
                      " temp_delay_vs_temp_vs_freq"
                      "is NULL, Check NV" );
    return api_status;
  }

  /* Assign the channels enabled from the NV */
  num_of_channels_enabled_for_delay_offset = offset_nv_ptr->channels_enabled;

  for (i = 0; i < num_of_channels_enabled_for_delay_offset; i++)
  {
    /* Convert channel to frequency */
    current_bin_upper_bound_tx_freq = 
      convert_freq_handler(offset_nv_ptr->tx_upper_bound_chan_list[i]);

    if (current_tx_freq <= current_bin_upper_bound_tx_freq)
    {
      /* copying the Index of upper bound channel to the actual freq bin */
      col_index = i;
      break;
    }
  }

  if ( col_index >= XPT_PATH_DELAY_ARRAY_SIZ)
  {
    return api_status;
  }
  else
  {
    *(final_delay_offset) = temp_delay_vs_temp_vs_freq[col_index];
    api_status = TRUE;
  }

  return api_status;
} /* rf_common_xpt_delay_offset_computation */

/*----------------------------------------------------------------------------*/
/*!
  @brief
   1. Check for current tx channel to be within a valid upper bound channel,
      if not return error
   2. Find out of the frequency row to be selected based on valid output of step 1,
     and whether rb_based_freq_search_enabled flag is enabled or not.
   3. Scale the frequency row based on the calibration temp reading of the Delay NV.
   4. Identify the temp bin to be selected based on current_temp_read.
   5. Return the offset value after interpolating over temp.
 
  @param rfnv_tx_delay_offsets_type
  Pointer to the structure with tx_carrier*_static offsets.
 
  @param therm_bin_array
  Pointer to the structure with temperature based bin values.
 
  @param current_temp_adc
  This is current temperature adc. LTE always passes scaled value.
 
  @param calibration_temp_adc
  This is calibration temperature adc. LTE always passes scaled value.
 
  @param current_tx_freq
  Current operating Tx Frequency.
 
  @param calibration_temp_based_realignment
  boolean flag to determine if temp re-alignmet is needed or not.
 
  @param final_delay_offset
  pointer to final computed offset.
 
  @param convert_freq_handler
  Function pointer to compute Tx channel to Tx freq conversion.
 
  @return
  TRUE if data is usable; FALSE otherwise
*/

boolean 
rf_common_xpt_delay_offset_computation
( 
  rfnv_tx_delay_offsets_type *offset_nv_ptr,
  const uint16 *therm_bin_array,
  uint16 current_temp_adc, 
  uint16 calibration_temp_adc,
  uint32 current_tx_freq,
  boolean calibration_temp_based_realignment,
  int16 *final_delay_offset,
  rfcommon_core_convert_freq_handler_type convert_freq_handler
)
{

  boolean api_status = FALSE;
  uint8 i = 0, j = 0, col_index = XPT_PATH_DELAY_ARRAY_SIZ;
  uint32 current_bin_upper_bound_tx_freq = 0;
  uint8 num_of_channels_enabled_for_delay_offset = 0;
  int16 calibration_ref_delay_offset = 0;
  int16 delay_offset_temp_array[XPT_TEMP_BIN_SIZ] = {0,0,0,0,0,0,0,0};
  
  if ((offset_nv_ptr == NULL) ||
      (therm_bin_array == NULL )||
      (final_delay_offset == NULL))
  {
    RF_MSG( RF_ERROR, "rf_common_xpt_delay_offset_computation offset_nv_ptr "
                      " or therm_bin_array or final_delay_offset is NULL, Check NV" );
    return api_status;
  }

  /* Assign the channels enabled from the NV */
  num_of_channels_enabled_for_delay_offset = offset_nv_ptr->channels_enabled;

  for (i = 0; i < num_of_channels_enabled_for_delay_offset; i++)
  {
    /* Convert channel to frequency */
    current_bin_upper_bound_tx_freq = 
      convert_freq_handler(offset_nv_ptr->tx_upper_bound_chan_list[i]);

    if (current_tx_freq <= current_bin_upper_bound_tx_freq)
    {
      /* copying the Index of upper bound channel to the actual freq bin */
      col_index = i;

      break;
    }
  }

  if ( col_index >= XPT_PATH_DELAY_ARRAY_SIZ)
  {
    return api_status;
  }
  else
  {
    for (j = 0; j<XPT_TEMP_BIN_SIZ; j++)
    {
      /* Calculate the Temp delay offset array based on current Freq selected */
      delay_offset_temp_array[j] = (offset_nv_ptr->delay_offset_vs_temp_vs_freq[j][col_index]);
    }

    if (calibration_temp_based_realignment == TRUE)
    {
      /* Find the Reference Calibration Offset based on the Calibration Temp ADC*/
     calibration_ref_delay_offset = rf_lininterp_u16_s16(therm_bin_array,
                                                         &delay_offset_temp_array[0],
                                                         XPT_TEMP_BIN_SIZ,
                                                         calibration_temp_adc);
      /* Proceed with Temp re-alignment */
      for (i = 0; i <XPT_TEMP_BIN_SIZ; i++ )
      {
        offset_nv_ptr->delay_offset_vs_temp_vs_freq[i][col_index] = 
              offset_nv_ptr->delay_offset_vs_temp_vs_freq[i][col_index] - 
          calibration_ref_delay_offset;
        delay_offset_temp_array[i] = (offset_nv_ptr->delay_offset_vs_temp_vs_freq[i][col_index]);
      }
    }
    
    *(final_delay_offset) = 
      rf_lininterp_u16_s16 (therm_bin_array,
                            &delay_offset_temp_array[0],
                            XPT_TEMP_BIN_SIZ,
                            current_temp_adc);
    api_status = TRUE;
  }
  return api_status;
} /* rf_common_xpt_delay_offset_computation */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API to find DPD scale entry based on current channel
 
  @param scaling_nv_ptr
  Pointer to DPD scale data (from NV)

  @param tx_char_freq
  Tx frequency list

  @param tx_char_chan_size
  Num channels

  @param current_tx_freq
  Current Tx frequency
 
  @param final_dpd_scale_factor
  Final DPD scale value and valid flag
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal
*/
boolean 
rf_common_xpt_dpd_scale_computation
(
   xpt_dpd_scaling_data_type *scaling_nv_ptr,
   uint32 *tx_char_freq,
   uint8 tx_char_chan_size,
   uint32 current_tx_freq,
   xpt_dpd_scaling_value_type *final_dpd_scale_factor
)
{ 
  if ((scaling_nv_ptr == NULL) ||
      (tx_char_freq == NULL )||
      (final_dpd_scale_factor == NULL))
  {
    RF_MSG( RF_ERROR, "rf_common_xpt_dpd_scale_computation: "
                      "NULL input params, Check NV" );
    return FALSE;
  }

  final_dpd_scale_factor->valid = FALSE;

  if (scaling_nv_ptr->rev != XPT_DPD_SCALING_REV_INVALID)
  {
    final_dpd_scale_factor->valid = TRUE;
    final_dpd_scale_factor->scale_value = (int32) rf_lininterp_u32_s16(tx_char_freq,
                                                                       scaling_nv_ptr->scaling_data, 
                                                                       tx_char_chan_size,
                                                                       current_tx_freq);
  }
  
  return TRUE;
}  

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API for extracting XPT DPD scaling V1 data
 
  @param nv_data_ptr
  Pointer to data to be extracted

  @param num_bytes_available
  The number of nv data bytes available 

  @param extracted_element_size
  Output. The number of bypes extracted for the element

  @param xpd_dpd_scaling_data
  Output. Destination for the extracted data.
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal
*/
boolean
rfcommon_xpt_load_dpd_scaling_v1_data
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  uint32 * const extracted_element_size,
  xpt_dpd_scaling_data_type *xpt_dpd_scaling_data
)
{
  boolean api_status = TRUE;
  uint8 chan_idx = 0;
  int32 scale_factor_q10;
  int32 capped_scale_factor_q10;

  /* The item to extract from NV */
  xpt_dpd_scaling_v1_nv_type xpt_dpd_scaling_nv;
  /* Contains the number of bytes for the element in NV */
  uint32 num_bytes_needed;

  /* Initialize to zero here in case checks below fail */
  *extracted_element_size = 0;

  /* The number of bytes needed to extract element */
  num_bytes_needed = sizeof( xpt_dpd_scaling_nv );

  /* Ensure there is enough memory left before actually extracting the element */
  if( num_bytes_available < num_bytes_needed )
  {
    RF_MSG( RF_ERROR, "rfcommon_xpt_load_dpd_scaling_v1_data - Not enough elements left to extract item: xpt dpd scaling" );
    api_status = FALSE;
  }

  if( api_status == TRUE )
  {
    /* Extract the element */
    memscpy( (void*) &xpt_dpd_scaling_nv, num_bytes_needed, nv_data_ptr, num_bytes_needed );
    *extracted_element_size = num_bytes_needed;

    /* Copy data */
    for( chan_idx = 0; chan_idx < RFCOM_TXLIN_NUM_CHANNELS; chan_idx++ )
    {
      /* Use Q10 */
      scale_factor_q10 = xpt_dpd_scaling_nv.scaling_data[ chan_idx ] << DPD_SCALING_Q_FACTOR;
      /* Value stored in NV as scale factor * 100, so divide out */
      scale_factor_q10 = rf_divide_with_rounding(scale_factor_q10,1000);
      capped_scale_factor_q10 = rf_limit_s32( scale_factor_q10, -32768, 32767 );
      if (scale_factor_q10 != capped_scale_factor_q10)
      {
        RF_MSG_1( RF_ERROR, "rfcommon_xpt_load_dpd_scaling_v1_data - "
                            "DPD scaling NV value exceeds max size of 8x: %d",
                xpt_dpd_scaling_nv.scaling_data[ chan_idx ]);
        api_status = FALSE;
      }
      xpt_dpd_scaling_data->scaling_data[ chan_idx ] = (int16)capped_scale_factor_q10;
    }
  } /* if( api_status == TRUE ) */

  return api_status;
} /* rfcommon_xpt_load_dpd_scaling_v1_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Populate the mission mode internal data structure from NV
*/
boolean
rfcommon_xpt_load_dpd_scaling_nv
(
  rfnv_item_id_enum_type item_code,
  xpt_dpd_scaling_data_type *xpt_dpd_scaling_data,
  rex_tcb_type* const caller_tcb_ptr,
  const rex_sigs_type task_nv_wait_sig,
  void (* const task_wait_func_ptr)( rex_sigs_type )
)
{
  boolean api_status = TRUE;
  boolean item_exists = FALSE;
  /* Raw NV data pointer before decompression */
  rfnv_item_type *nv_data_raw_ptr;
  /* Data pointer after decompression */
  uint8 *nv_data_ptr = NULL;
  uint8 *temp_data_ptr = NULL;
  /* NV item size (maximum) */
  uint16 nv_item_size = sizeof(rfnv_item_type);
  /* actual NV data size after decompression */
  uint32 uncompress_size = XPT_DPD_SCALING_V1_NV_SIZE_MAX;
  /* NV read status */
  nv_stat_enum_type nv_read_status = NV_FAIL_S;
  /* NV decompression status */
  int8 uncompress_status = Z_ERRNO;
  /* Used for looping through TYPE IDs */
  uint16 type_id_idx;
  /* The TYPE ID of the current element */
  uint16 type_id;
  /* Contains the number of bytes for an element in NV */
  uint32 nv_element_size;
  /* Keeps track of the running total of number of bytes already parsed */
  uint32 num_bytes_parsed;
  /* Helper */
  int32 temp_num_bytes_left;
  /* Contains info about data contained in the NV item */
  rf_nv_data_variant_marker_type variant_marker;

  if (xpt_dpd_scaling_data == NULL)
  {
    RF_MSG( RF_ERROR, "rfcommon_xpt_load_dpd_scaling_nv - NULL destination ptr." );
    api_status = FALSE;
  }
  else
  {
    memset( xpt_dpd_scaling_data, 0, sizeof(xpt_dpd_scaling_data_type) );
    xpt_dpd_scaling_data->rev = XPT_DPD_SCALING_REV_INVALID;
  }

  /* Allocate heap memory for raw NV data*/
  nv_data_raw_ptr = (rfnv_item_type *)modem_mem_alloc(nv_item_size, MODEM_MEM_CLIENT_RFA);

  /* Null pointer check */
  if (nv_data_raw_ptr == NULL)
  {
    RF_MSG( RF_ERROR, "rfcommon_xpt_load_dpd_scaling_nv - Failed to allocate memory for raw NV data pointer ");
    api_status = FALSE;
  }

  if( api_status == TRUE )
  {
    /* Retrieve raw NV data from EFS */
    nv_read_status = rfcommon_rfnv_get_item(  item_code, 
                                              nv_data_raw_ptr,
                                              &nv_item_size,
                                              caller_tcb_ptr,
                                              task_nv_wait_sig,
                                              task_wait_func_ptr );

    if (nv_read_status == NV_DONE_S)
    {
      RF_MSG_1( RF_HIGH, "rfcommon_xpt_load_dpd_scaling_nv - Processing NV ID %d.", item_code );
      item_exists = TRUE;

      /* Allocate worst-case memory size for NV data decompression. */
      nv_data_ptr = modem_mem_alloc( uncompress_size, MODEM_MEM_CLIENT_RFA );
    
      if (nv_data_ptr == NULL)
      {
        RF_MSG_1( RF_ERROR, "rfcommon_xpt_load_dpd_scaling_nv - Failed to allocate memory for %d bytes.", uncompress_size );
        api_status = FALSE;
      }

      if( api_status == TRUE )
      {
        /* Decompress NV data */
        uncompress_status = uncompress( (uint8 *)nv_data_ptr,   
                                        &uncompress_size,
                                        (const uint8 *)nv_data_raw_ptr, 
                                        (uint32)nv_item_size
                                      );
      
        if (uncompress_status != Z_OK)
        {
          RF_MSG_1( RF_ERROR, "rfcommon_xpt_load_dpd_scaling_nv - Decompression of NV ID %d failed.", item_code );
          api_status = FALSE;
        }
      }
    } /* (nv_read_status == NV_DONE_S) */
    else if (nv_read_status == NV_NOTACTIVE_S)
    {
      /* This NV is allowed to not be active */
      RF_MSG_1( RF_HIGH, "rfcommon_xpt_load_dpd_scaling_nv - NV ID %d not active.", item_code );
    } /* (nv_read_status == NV_NOTACTIVE_S) */
    else //if (nv_read_status == NV_FAIL_S)
    {
      RF_MSG_2( RF_ERROR, "rfcommon_xpt_load_dpd_scaling_nv - NV ID %d read failed with status %d.",
                item_code, nv_read_status );
      api_status = FALSE;
    } /* (nv_read_status == NV_FAIL_S) */
  }

  /* By now, NV data has been successfully retrieved, 
     proceed to populating the mission mode data structure */

  if( ( api_status == TRUE ) && (item_exists == TRUE) )
  {
    /* 
      FORMULA: To ensure there is enough memory left 
        if( (TOTAL - PARSED) < NEEDED )
        {
          ERROR;
        }
    */ 

    /* Initialize running total of bytes already parsed */
    num_bytes_parsed = 0;

    /* First element in NV item must be the number of type IDs it contains */
    nv_element_size = sizeof( variant_marker );

    /* Ensure there is enough memory left before actually extracting the element */
    if( (uncompress_size - num_bytes_parsed) < nv_element_size )
    {
      RF_MSG( RF_ERROR, "rfcommon_xpt_load_dpd_scaling_nv - Not enough elements present to extract "
                "first item NUM_TYPE_ID." );
      api_status = FALSE;
    }
  }

  if( ( api_status == TRUE ) && (item_exists == TRUE) )
  {
    /* EXTRACT ELEMENT: Number of type IDs present */
    temp_data_ptr = nv_data_ptr + num_bytes_parsed;
    memscpy( (void*) &variant_marker, nv_element_size,(void*)temp_data_ptr, nv_element_size );
    /* Update running total of bytes parsed with num bytes from last element extracted */
    num_bytes_parsed += nv_element_size;
    /* Check number of elements validity */
    if( variant_marker.num_elements != 1 )
    {
      RF_MSG_1( RF_ERROR, "rfcommon_xpt_load_dpd_scaling_nv - Number of elements value of %d "
                "is invalid. The only valid value must be 1.", variant_marker.num_elements);
      api_status = FALSE;
    }
  }

  if( ( api_status == TRUE ) && (item_exists == TRUE) )
  {
    for( type_id_idx = 0; (type_id_idx < variant_marker.num_elements) && (api_status == TRUE); type_id_idx++ )
    {
      /* Next element in NV item must be the type ID of the subsequent element */
      nv_element_size = sizeof( type_id );

      /* Ensure there is enough memory left before actually extracting the element */
      if( (uncompress_size - num_bytes_parsed) < nv_element_size )
      {
        RF_MSG_1( RF_ERROR, "rfcommon_xpt_load_dpd_scaling_nv - Not enough elements left to extract "
                  "item named TYPE_ID for type index %d", type_id_idx );
        api_status = FALSE;
        break;
      }

      /* EXTRACT ELEMENT: Type ID */
      temp_data_ptr = nv_data_ptr + num_bytes_parsed;
      memscpy( (void*) &type_id, nv_element_size,(void*)temp_data_ptr, nv_element_size );
      /* Update running total of bytes parsed with num bytes from last element extracted */
      num_bytes_parsed += nv_element_size;

      /* EXTRACT ELEMENT: Type corresponding to the type ID just extracted. */
  
      temp_data_ptr = nv_data_ptr + num_bytes_parsed;
      temp_num_bytes_left = uncompress_size - num_bytes_parsed;
      if( temp_num_bytes_left <= 0 )
      {
        RF_MSG_2( RF_ERROR, "rfcommon_xpt_load_dpd_scaling_nv - Not elements left to extract "
                  "item of TYPE_ID for type index %d", type_id, type_id_idx );
        api_status = FALSE;
        break;
      }

      switch( type_id )
      {
      case RFNV_DATA_TYPE_ID_XPT_DPD_SCALING_V1:
          api_status &= rfcommon_xpt_load_dpd_scaling_v1_data( temp_data_ptr, 
                                                               temp_num_bytes_left, 
                                                               &nv_element_size,
                                                               xpt_dpd_scaling_data );
          if( api_status == TRUE )
          {
            /* Set lin rev to valid rev only if all went well */
            xpt_dpd_scaling_data->rev = XPT_DPD_SCALING_REV_1;
          }
        break;

        default:
          {
            RF_MSG_2( RF_ERROR, "rfcommon_xpt_load_dpd_scaling_nv - element of type ID %d not supported "
                      "for type index %d", type_id, type_id_idx);
            api_status = FALSE;
            break;
          }
        break;

      } /* switch( type_id ) */

      if( api_status == TRUE )
      {
        /* Update running total of bytes parsed with num bytes from last element extracted */
        num_bytes_parsed += nv_element_size;
      }
      else
      {
        RF_MSG_2( RF_ERROR, "rfcommon_xpt_load_dpd_scaling_nv - Error extracting "
                  "item of TYPE_ID %d for type index %d", type_id, type_id_idx );
        api_status = FALSE;
        break;/* DO NOT CONTINUE as memory may not ge aligned any more for proper extraction */
      }
    } /* for( type_id_idx ) */
  } /* if( api_status == TRUE ) */

  /* Cleanup */

  if( nv_data_ptr != NULL )
  {
    modem_mem_free( nv_data_ptr, MODEM_MEM_CLIENT_RFA );
  }

  if( nv_data_raw_ptr != NULL )
  {
    modem_mem_free( nv_data_raw_ptr, MODEM_MEM_CLIENT_RFA );
  }
   
  return api_status;
} /* rfcommon_xpt_load_dpd_scaling_nv */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Common Load DPD Function.
 
  @param device
  Device corresponding to the DPD action.
 
  @param dpd_tbl_ptr
  Table with DPD information to save
 
  @param dpd_tbl_sz
  Number of elements in table to save
 
  @param tbl_type
  The type of DPD being saved
 
  @param bank_index
  OUTPUT. The bank where the data was saved
 
  @param bank_offset
  OUTPUT. The offset within the bank where the data was saved
 
  @return
  TRUE when success, else FALSE
*/
boolean
rf_common_xpt_load_dpd_tbls
(
  uint8 device,
  uint16* dpd_tbl_ptr,
  uint16  dpd_tbl_sz,
  load_dpd_input_table_type tbl_type,
  uint8* bank_index,
  uint16* bank_offset,
  xpt_dpd_action_type action_type,
  rfcommon_mdsp_xpt_mode_enum_type xpt_mode
)
{
  boolean api_status = TRUE;
  rflm_txagc_load_dpd_cfg_t load_dpd_cfg;
  uint8 modem_chain = 0;
  uint8 index = 0;
  uint16 offset = 0;

  modem_chain = rfcommon_core_get_tx_chain_from_device( (rfm_device_enum_type)device );

  load_dpd_cfg.device = device;
  load_dpd_cfg.modem_chain = modem_chain; 
  load_dpd_cfg.dpd_tbl_ptr = dpd_tbl_ptr;
  load_dpd_cfg.dpd_tbl_sz = dpd_tbl_sz;
  load_dpd_cfg.tbl_type = (uint8)tbl_type;
  load_dpd_cfg.action_type = (uint8)action_type;
  load_dpd_cfg.xpt_mode = xpt_mode;

  /* Call the rfmodem function here */
  api_status &= 
    rflm_txagc_modem_xpt_load_dpd_tbls( &load_dpd_cfg, &index, &offset );

  *bank_index = index;
  *bank_offset = offset;

  return api_status;
} /* rf_common_xpt_load_dpd_tbls */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure linearizer interface for XPT Tx override
  
  @details
  Queries tech-specific driver code to obtain Tx device pointer information. 
  Provides device pointers and Tx override parameters to common linearizer 
  interface in order to populate to RFLM DM

  @param rfm_mode
  RFM Mode for which override is being requested

  @param device
  RFM device for which override is being requested

  @param rfm_band
  Structure holding tech-specific band information

  @param tx_ovr_params
  Structure holding Tx override parameters
  
  @return
  TRUE when success, else FALSE
*/
boolean
rfcommon_xpt_load_linearizer_for_override
(
  rfm_mode_enum_type rfm_mode,
  rfm_device_enum_type device,
  rfcom_band_type_u rfm_band,
  rfcommon_core_xpt_tx_override_type* tx_ovr_params
)
{
  boolean api_status = TRUE;
  rf_time_tick_type prof_start_t;
  rf_time_type prof_elapsed_t;

  rfcommon_core_txlin_agc_device_info device_info;

  /* Get start time for profiling */
  prof_start_t = rf_time_get_tick( );

  switch ( rfm_mode )
  {
  case RFM_1X_MODE:
  case RFM_1XEVDO_MODE:
    #ifdef FEATURE_CDMA1X
    api_status &= rf_cdma_tx_agc_get_device_info( device,
                                                  rfm_band.cdma_band,
                                                  &device_info );
    #endif
    break;

  case RFM_IMT_MODE:
    #ifdef FEATURE_WCDMA
    api_status &= rfwcdma_core_txctl_get_device_info( device,
                                                      rfm_band.wcdma_band,
                                                      &device_info);
    #endif
    break;

  case RFM_LTE_MODE:
  case RFM_LTE_FTM_MODE:
    #ifdef FEATURE_LTE
    api_status &= rflte_mc_tx_hw_ctl_get_device_info(device,
                                                     rfm_band.lte_band,
                                                     &device_info);
    #endif
    break;

  case RFM_TDSCDMA_MODE:
  case RFM_TDSCDMA_MODE_FTM:
    #ifdef FEATURE_TDSCDMA
    api_status &=rf_tdscdma_core_txlin_get_tx_device_info( device, 
                                                           rfm_band.tdscdma_band,
                                                           &device_info);
    #endif
    break;

  default:
    RF_MSG_1( RF_ERROR, "rfcommon_xpt_load_linearizer_for_override: "
                        "Unsupported RFM Mode %d", 
              rfm_mode );
    api_status = FALSE;
    break;
  }

  if ( api_status == TRUE )
  {
    api_status &= rfcommon_core_txlin_config_tx_override( rfm_mode, device, 
                                                          rfm_band,
                                                          &device_info,
                                                          tx_ovr_params );
  }

  prof_elapsed_t = rf_time_get_elapsed( prof_start_t, RF_USEC );

  RF_MSG_4( RF_HIGH, "rfcommon_xpt_load_linearizer_for_override: "
                     "Configured linearizer interface for override. "
                     "RFM Mode %d, Dev %d, [Status %d], Elapsed Time %d",
            rfm_mode, device, api_status, prof_elapsed_t ); 

  return api_status;

}/* rfcommon_xpt_load_linearizer_for_override */
#endif /* FEATURE_GNSS_ONLY_NO_WWAN */
