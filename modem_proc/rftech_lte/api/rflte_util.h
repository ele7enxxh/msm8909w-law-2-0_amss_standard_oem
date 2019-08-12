/*! 
 @file rflte_util.h 
 
 @brief 
  This is the header file for utility functions for the LTE technology. 
 
 @details 
 
*/ 
  
/*=========================================================================== 
  
Copyright (c) 2013-16 by Qualcomm Technologies, Inc.  All Rights Reserved. 
  
                           EDIT HISTORY FOR FILE 
  
This section contains comments describing changes made to this file. 
Notice that changes are listed in reverse chronological order. 

$Header: 
  
When       who     what, where, why 
-------------------------------------------------------------------------------
07/22/16   cv      Adding prototype for API
07/21/16   cv      Adding support for RX Static NV
07/05/16   cv      Adding LTE B66 support
12/05/14   dw      Changes to support split bands based on split specified in RFC.
12/02/14   yzw     Add utility APIs for DL only chan and band
11/04/14   gvn     Add utility api to switch port A\B
10/29/14   stw     Added APIs rflte_util_get_sub_band and rflte_util_get_full_band
10/02/14   kab     Support for Single Rx RXLM Config Mode Selection.
09/02/14   php     Use Char channel list for Pin compensation
07/31/14   ndb     Added support for extended EARFCN for LTE
06/18/14   sbm     Added api to determine if hdet Nv is present or not.
06/04/14    pv     Added support for FBRx RB Based Freq comp.
04/17/14   svi     PA static NV and code redesign
04/11/14   bsh     Split Band Support
03/14/14   bsh     NlIC changes - move ftl calculation to NLIC component 
03/12/14   bsh     Adding NLIC support - calcualte ftl based on rx freq error
03/13/14   svi     Fix UL DL EARFCN Offset for LTE
02/04/14    pv     Added support for FBRx Gain error processing.
08/31/13   bsh     Initial version. 
  
============================================================================*/ 
#ifndef RFLTE_UTIL_H
#define RFLTE_UTIL_H

#include "appmgr.h"
#include "msgr.h"
#include "rfa_msgr.h"
#include "rfcom.h"

#include "rfm_device_config_type.h"
#include "rflte_ext_mc.h"
#include "rflte_nv.h"

/*! FBRx Related Includes */
#include "rfcommon_fbrx_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LTE_IQ_BUFFER_SIZE  16384  /*hardcoded for 16KB*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Band and NS dependent AMPR constant definitions
 
*/
/* B23 - NS11: Table 6.2.4-5 36.101 (Release 11.2) */
#define RFLTE_UTIL_B23_NS_11_LOWER_FREQ_BOUND_KHZ  2000000
#define RFLTE_UTIL_B23_NS_11_UPPER_FREQ_BOUND_KHZ  2012500

/* B26 - NS15: Table 6.2.4-9\6.2.4-10 36.101 (Release 11) */
#define RFLTE_UTIL_B26_NS_15_LOWER_FREQ_BOUND_KHZ  845000
#define RFLTE_UTIL_B26_NS_15_UPPER_FREQ_BOUND_KHZ  849000
/* Size of the RB in KHZ  Please refer the TS 32.101 V11.2.0 (2012-09)*/

#define RFLTE_UTIL_RB_IN_KHZ 180

/*Please refer the table 6.2.2-1 Note 2. Bandedge in Khz where the maxpower can be relaxed*/
#define RFLTE_UTIL_BAND_EDGE_LIMIT_IN_KHZ 4000

/*Absolute value macro*/
#define RFLTE_UTIL_ABS(x)               (((x) < 0) ? -(x) : (x))
/*Round Macro*/
#define RFLTE_UTIL_FLOOR(x,y)           rflte_util_floor(x, y)
/*Round value macro*/
#define RFLTE_UTIL_ROUND(x,y)           RFLTE_UTIL_FLOOR(((2 * (x)) + (y)), (2 * (y)))
/* Saturate Macro: This is used for clipping the results of adjustment
  calculations where the newly calculated value may exceed the legal range
  for the control.*/
#define RFLTE_UTIL_SATURATE( val, min, max )  MAX( MIN( (val), (max) ), (min) )
/*Sign Macro*/
#define RFLTE_UTIL_SIGN(x)              (((x) < 0) ? -1 : 1)
#define RFLTE_UTIL_INT(x)    ((int)x)
#define RFLTE_UTIL_FRAC(x,y) ( RFLTE_UTIL_ABS((int)((float)(((float)x) - ((int)x))*y)) )

#define RFLTE_UTIL_MIN(val1,val2,val3,val4,val5,val6)   MIN( MIN(val1,val2),MIN(val3,MIN(val4,MIN(val5,val6))))

/*----------------------------------------------------------------------------*/
/*!
  @brief
  FDD UL and DL EARFCN Offset Macros
 
*/
/*!> For FDD Bands between B1 to B28 */
#define RFLTE_UTIL_FDD_DL_UL_EARFCN_OFFSET_B1_TO_B28 18000

/*!> For FDD Bands  B30 and B31 */
#define RFLTE_UTIL_FDD_DL_UL_EARFCN_OFFSET_B30_B31   17890

/*!> For FDD Bands  B66 */
#define RFLTE_UTIL_FDD_DL_UL_EARFCN_OFFSET_B66   65536

/*----------------------------------------------------------------------------*/
/*!
  @brief
  TDD UL and DL EARFCN Offset Macros
 
*/
#define RFLTE_UTIL_TDD_DL_UL_EARFCN_OFFSET           0


/*----------------------------------------------------------------------------*/


typedef enum
{
  RF_LTE_DISABLED_DEBUG_MASK = 0x0,  
  RF_LTE_LEGACY_ENABLE_ALL_MASK = 0x1,  
  RF_LTE_TX_PLL_ERROR_CORRECTION_DEBUG_MASK = 0x2,
  RF_LTE_SMPS_PDM_INTERPOLATE_DEBUG_MASK = 0x4,
  RF_LTE_TEMP_COMP_DEBUG_MASK = 0x8,
  RF_LTE_MTPL_DEBUG_MASK = 0x10,
  RF_LTE_FREQ_COMP_DEBUG_MASK = 0x20,  
  RF_LTE_IRAT_DEBUG_MASK = 0x40,
  RF_LTE_MSG_DISPATCH_DEBUG_MASK = 0x80
} rf_lte_debug_msg_enums;

#define BAND_MASK_WIDTH 4

/*============================================================================= 
                             DATA DEFINITIONS 
=============================================================================*/

#define INVALID_UINT16 						65535

/*LTE channel raster in kHz.*/
#define RF_LTE_CHANNEL_RASTER_IN_KHZ 100

/*LOWEST DL FREQUENCIES FOR EACH BAND IN kHz*/
#define LTE_B1_DL_LOW_FREQ          2110000
#define LTE_B2_DL_LOW_FREQ          1930000
#define LTE_B3_DL_LOW_FREQ          1805000
#define LTE_B4_DL_LOW_FREQ          2110000
#define LTE_B5_DL_LOW_FREQ          869000
#define LTE_B6_DL_LOW_FREQ          875000
#define LTE_B7_DL_LOW_FREQ          2620000
#define LTE_B8_DL_LOW_FREQ          925000
#define LTE_B9_DL_LOW_FREQ          1844900
#define LTE_B10_DL_LOW_FREQ         2110000
#define LTE_B11_DL_LOW_FREQ         1475900
#define LTE_B12_DL_LOW_FREQ         729000
#define LTE_B13_DL_LOW_FREQ         746000
#define LTE_B14_DL_LOW_FREQ         758000
#define LTE_B17_DL_LOW_FREQ         734000
#define LTE_B18_DL_LOW_FREQ         860000
#define LTE_B19_DL_LOW_FREQ         875000
#define LTE_B20_DL_LOW_FREQ         791000
#define LTE_B21_DL_LOW_FREQ         1495900
#define LTE_B23_DL_LOW_FREQ         2180000
#define LTE_B25_DL_LOW_FREQ         1930000
#define LTE_B26_DL_LOW_FREQ         859000
#define LTE_B27_DL_LOW_FREQ         852000
#define LTE_B28_DL_LOW_FREQ         758000
#define LTE_B29_DL_LOW_FREQ         717000
#define LTE_B30_DL_LOW_FREQ         2350000
#define LTE_B33_DL_LOW_FREQ         1900000
#define LTE_B34_DL_LOW_FREQ         2010000
#define LTE_B35_DL_LOW_FREQ         1850000
#define LTE_B36_DL_LOW_FREQ         1930000
#define LTE_B37_DL_LOW_FREQ         1910000
#define LTE_B38_DL_LOW_FREQ         2570000
#define LTE_B39_DL_LOW_FREQ         1880000
#define LTE_B40_DL_LOW_FREQ         2300000
#define LTE_B41_DL_LOW_FREQ	    2496000
#define LTE_B42_DL_LOW_FREQ	    3400000


/*HIGHEST DL FREQUENCIES FOR EACH BAND IN kHz source 36.101 Table 5.5-1*/
#define LTE_B1_DL_HIGH_FREQ         2170000
#define LTE_B2_DL_HIGH_FREQ         1990000
#define LTE_B3_DL_HIGH_FREQ         1880000
#define LTE_B4_DL_HIGH_FREQ         2155000
#define LTE_B5_DL_HIGH_FREQ         894000
#define LTE_B6_DL_HIGH_FREQ         885000
#define LTE_B7_DL_HIGH_FREQ         2690000
#define LTE_B8_DL_HIGH_FREQ         960000
#define LTE_B9_DL_HIGH_FREQ         1879900
#define LTE_B10_DL_HIGH_FREQ        2170000
#define LTE_B11_DL_HIGH_FREQ        1495900
#define LTE_B12_DL_HIGH_FREQ        746000
#define LTE_B13_DL_HIGH_FREQ        756000
#define LTE_B14_DL_HIGH_FREQ        768000
#define LTE_B17_DL_HIGH_FREQ        746000
#define LTE_B18_DL_HIGH_FREQ        875000
#define LTE_B19_DL_HIGH_FREQ        890000
#define LTE_B20_DL_HIGH_FREQ        821000
#define LTE_B21_DL_HIGH_FREQ        1510900
#define LTE_B23_DL_HIGH_FREQ        2200000
#define LTE_B25_DL_HIGH_FREQ        1995000
#define LTE_B26_DL_HIGH_FREQ        894000
#define LTE_B27_DL_HIGH_FREQ        869000
#define LTE_B28_DL_HIGH_FREQ        803000
#define LTE_B29_DL_HIGH_FREQ        728000
#define LTE_B30_DL_HIGH_FREQ        2360000
#define LTE_B33_DL_HIGH_FREQ        1920000
#define LTE_B34_DL_HIGH_FREQ        2025000
#define LTE_B35_DL_HIGH_FREQ        1910000
#define LTE_B36_DL_HIGH_FREQ        1990000
#define LTE_B37_DL_HIGH_FREQ        1930000
#define LTE_B38_DL_HIGH_FREQ        2620000
#define LTE_B39_DL_HIGH_FREQ        1920000
#define LTE_B40_DL_HIGH_FREQ        2400000
#define LTE_B41_DL_HIGH_FREQ	    2690000
#define LTE_B42_DL_HIGH_FREQ	    3600000
/*MINIMUM DL CHANNEL NUMBER FOR EACH BAND*/
#define LTE_B1_DL_MIN_CHAN          0
#define LTE_B2_DL_MIN_CHAN          600
#define LTE_B3_DL_MIN_CHAN          1200
#define LTE_B4_DL_MIN_CHAN          1950
#define LTE_B5_DL_MIN_CHAN          2400
#define LTE_B6_DL_MIN_CHAN          2650
#define LTE_B7_DL_MIN_CHAN          2750
#define LTE_B8_DL_MIN_CHAN          3450
#define LTE_B9_DL_MIN_CHAN          3800
#define LTE_B10_DL_MIN_CHAN         4150
#define LTE_B11_DL_MIN_CHAN         4750
#define LTE_B12_DL_MIN_CHAN         5010
#define LTE_B13_DL_MIN_CHAN         5180
#define LTE_B14_DL_MIN_CHAN         5280
#define LTE_B17_DL_MIN_CHAN         5730
#define LTE_B18_DL_MIN_CHAN         5850
#define LTE_B19_DL_MIN_CHAN         6000
#define LTE_B20_DL_MIN_CHAN         6150
#define LTE_B21_DL_MIN_CHAN         6450
#define LTE_B23_DL_MIN_CHAN         7500
#define LTE_B25_DL_MIN_CHAN    	    8040
#define LTE_B26_DL_MIN_CHAN         8690
#define LTE_B27_DL_MIN_CHAN         9040
#define LTE_B28_DL_MIN_CHAN         9210
#define LTE_B29_DL_MIN_CHAN         9660
#define LTE_B30_DL_MIN_CHAN         9770
#define LTE_B33_DL_MIN_CHAN         36000
#define LTE_B34_DL_MIN_CHAN         36200
#define LTE_B35_DL_MIN_CHAN         36350
#define LTE_B36_DL_MIN_CHAN         36950
#define LTE_B37_DL_MIN_CHAN         37550
#define LTE_B38_DL_MIN_CHAN         37750
#define LTE_B39_DL_MIN_CHAN         38250
#define LTE_B40_DL_MIN_CHAN         38650
#define LTE_B41_DL_MIN_CHAN         39650
#define LTE_B42_DL_MIN_CHAN         41590

/*MAXIMUM CHANNEL NUMBER FOR EACH BAND*/
#define LTE_B1_DL_MAX_CHAN          599
#define LTE_B2_DL_MAX_CHAN          1199
#define LTE_B3_DL_MAX_CHAN          1949
#define LTE_B4_DL_MAX_CHAN          2399
#define LTE_B5_DL_MAX_CHAN          2649
#define LTE_B6_DL_MAX_CHAN          2749
#define LTE_B7_DL_MAX_CHAN          3449
#define LTE_B8_DL_MAX_CHAN          3799
#define LTE_B9_DL_MAX_CHAN          4149
#define LTE_B10_DL_MAX_CHAN         4749
#define LTE_B11_DL_MAX_CHAN         4999
#define LTE_B12_DL_MAX_CHAN         5179
#define LTE_B13_DL_MAX_CHAN         5279
#define LTE_B14_DL_MAX_CHAN         5379
#define LTE_B17_DL_MAX_CHAN         5849
#define LTE_B18_DL_MAX_CHAN         5999
#define LTE_B19_DL_MAX_CHAN         6149
#define LTE_B20_DL_MAX_CHAN         6449
#define LTE_B21_DL_MAX_CHAN         6599
#define LTE_B23_DL_MAX_CHAN         7699
#define LTE_B25_DL_MAX_CHAN    	    8689
#define LTE_B26_DL_MAX_CHAN    	    9039
#define LTE_B27_DL_MAX_CHAN    	    9209
#define LTE_B28_DL_MAX_CHAN    	    9659
#define LTE_B29_DL_MAX_CHAN    	    9769
#define LTE_B30_DL_MAX_CHAN    	    9869
#define LTE_B33_DL_MAX_CHAN         36199
#define LTE_B34_DL_MAX_CHAN         36349
#define LTE_B35_DL_MAX_CHAN         36949
#define LTE_B36_DL_MAX_CHAN         37549
#define LTE_B37_DL_MAX_CHAN         37749
#define LTE_B38_DL_MAX_CHAN         38249
#define LTE_B39_DL_MAX_CHAN         38649
#define LTE_B40_DL_MAX_CHAN         39649
#define LTE_B41_DL_MAX_CHAN	    41589
#define LTE_B42_DL_MAX_CHAN	    43589


/*LOWEST UL FREQUENCIES FOR EACH BAND IN kHz*/
#define LTE_B1_UL_LOW_FREQ          1920000
#define LTE_B2_UL_LOW_FREQ          1850000
#define LTE_B3_UL_LOW_FREQ          1710000
#define LTE_B4_UL_LOW_FREQ          1710000
#define LTE_B5_UL_LOW_FREQ          824000
#define LTE_B6_UL_LOW_FREQ          830000
#define LTE_B7_UL_LOW_FREQ          2500000
#define LTE_B8_UL_LOW_FREQ          880000
#define LTE_B9_UL_LOW_FREQ          1749900
#define LTE_B10_UL_LOW_FREQ         1710000
#define LTE_B11_UL_LOW_FREQ         1427900
#define LTE_B12_UL_LOW_FREQ         699000
#define LTE_B13_UL_LOW_FREQ         777000
#define LTE_B14_UL_LOW_FREQ         788000
#define LTE_B17_UL_LOW_FREQ         704000
#define LTE_B18_UL_LOW_FREQ         815000
#define LTE_B19_UL_LOW_FREQ         830000
#define LTE_B20_UL_LOW_FREQ         832000
#define LTE_B21_UL_LOW_FREQ         1447900
#define LTE_B23_UL_LOW_FREQ         2000000
#define LTE_B25_UL_LOW_FREQ         1850000
#define LTE_B26_UL_LOW_FREQ         814000
#define LTE_B27_UL_LOW_FREQ         807000
#define LTE_B28_UL_LOW_FREQ         703000
#define LTE_B30_UL_LOW_FREQ         2305000
#define LTE_B33_UL_LOW_FREQ         1900000
#define LTE_B34_UL_LOW_FREQ         2010000
#define LTE_B35_UL_LOW_FREQ         1850000
#define LTE_B36_UL_LOW_FREQ         1930000
#define LTE_B37_UL_LOW_FREQ         1910000
#define LTE_B38_UL_LOW_FREQ         2570000
#define LTE_B39_UL_LOW_FREQ         1880000
#define LTE_B40_UL_LOW_FREQ         2300000
#define LTE_B41_UL_LOW_FREQ         2496000
#define LTE_B42_UL_LOW_FREQ         3400000

/*HIGHEST UL FREQUENCIES FOR EACH BAND IN kHz source 36.101 Table 5.5-1*/
#define LTE_B1_UL_HIGH_FREQ         1980000
#define LTE_B2_UL_HIGH_FREQ         1910000
#define LTE_B3_UL_HIGH_FREQ         1785000
#define LTE_B4_UL_HIGH_FREQ         1755000
#define LTE_B5_UL_HIGH_FREQ         849000
#define LTE_B6_UL_HIGH_FREQ         840000
#define LTE_B7_UL_HIGH_FREQ         2570000
#define LTE_B8_UL_HIGH_FREQ         915000
#define LTE_B9_UL_HIGH_FREQ         1784900
#define LTE_B10_UL_HIGH_FREQ        1770000
#define LTE_B11_UL_HIGH_FREQ        1447900
#define LTE_B12_UL_HIGH_FREQ        716000
#define LTE_B13_UL_HIGH_FREQ        787000
#define LTE_B14_UL_HIGH_FREQ        798000
#define LTE_B17_UL_HIGH_FREQ        716000
#define LTE_B18_UL_HIGH_FREQ        830000
#define LTE_B19_UL_HIGH_FREQ        845000
#define LTE_B20_UL_HIGH_FREQ        862000
#define LTE_B21_UL_HIGH_FREQ        1462900
#define LTE_B23_UL_HIGH_FREQ        2020000
#define LTE_B25_UL_HIGH_FREQ        1915000
#define LTE_B26_UL_HIGH_FREQ        849000
#define LTE_B27_UL_HIGH_FREQ        824000
#define LTE_B28_UL_HIGH_FREQ        748000
#define LTE_B30_UL_HIGH_FREQ        2315000
#define LTE_B33_UL_HIGH_FREQ        1920000
#define LTE_B34_UL_HIGH_FREQ        2025000
#define LTE_B35_UL_HIGH_FREQ        1910000
#define LTE_B36_UL_HIGH_FREQ        1990000
#define LTE_B37_UL_HIGH_FREQ        1930000
#define LTE_B38_UL_HIGH_FREQ        2620000
#define LTE_B39_UL_HIGH_FREQ        1920000
#define LTE_B40_UL_HIGH_FREQ        2400000
#define LTE_B41_UL_HIGH_FREQ        2690000
#define LTE_B42_UL_HIGH_FREQ        3600000

/*MINIMUM UL CHANNEL NUMBER FOR EACH BAND*/
#define LTE_B1_UL_MIN_CHAN          18000
#define LTE_B2_UL_MIN_CHAN          18600
#define LTE_B3_UL_MIN_CHAN          19200
#define LTE_B4_UL_MIN_CHAN          19950
#define LTE_B5_UL_MIN_CHAN          20400
#define LTE_B6_UL_MIN_CHAN          20650
#define LTE_B7_UL_MIN_CHAN          20750
#define LTE_B8_UL_MIN_CHAN          21450
#define LTE_B9_UL_MIN_CHAN          21800
#define LTE_B10_UL_MIN_CHAN         22150
#define LTE_B11_UL_MIN_CHAN         22750
#define LTE_B12_UL_MIN_CHAN         23010
#define LTE_B13_UL_MIN_CHAN         23180
#define LTE_B14_UL_MIN_CHAN         23280
#define LTE_B17_UL_MIN_CHAN         23730
#define LTE_B18_UL_MIN_CHAN         23850
#define LTE_B19_UL_MIN_CHAN         24000
#define LTE_B20_UL_MIN_CHAN         24150
#define LTE_B21_UL_MIN_CHAN         24450
#define LTE_B23_UL_MIN_CHAN         25500
#define LTE_B25_UL_MIN_CHAN         26040
#define LTE_B26_UL_MIN_CHAN         26690
#define LTE_B27_UL_MIN_CHAN         27040
#define LTE_B28_UL_MIN_CHAN         27210
#define LTE_B30_UL_MIN_CHAN         27660
#define LTE_B33_UL_MIN_CHAN         36000
#define LTE_B34_UL_MIN_CHAN         36200
#define LTE_B35_UL_MIN_CHAN         36350
#define LTE_B36_UL_MIN_CHAN         36950
#define LTE_B37_UL_MIN_CHAN         37550
#define LTE_B38_UL_MIN_CHAN         37750
#define LTE_B39_UL_MIN_CHAN         38250
#define LTE_B40_UL_MIN_CHAN         38650
#define LTE_B41_UL_MIN_CHAN         39650
#define LTE_B42_UL_MIN_CHAN         41590

/*MAXIMUM UL CHANNEL NUMBER FOR EACH BAND*/
#define LTE_B1_UL_MAX_CHAN          18599
#define LTE_B2_UL_MAX_CHAN          19199
#define LTE_B3_UL_MAX_CHAN          19949
#define LTE_B4_UL_MAX_CHAN          20399
#define LTE_B5_UL_MAX_CHAN          20649
#define LTE_B6_UL_MAX_CHAN          20749
#define LTE_B7_UL_MAX_CHAN          21449
#define LTE_B8_UL_MAX_CHAN          21799
#define LTE_B9_UL_MAX_CHAN          22149
#define LTE_B10_UL_MAX_CHAN         22749
#define LTE_B11_UL_MAX_CHAN         22999
#define LTE_B12_UL_MAX_CHAN         23179
#define LTE_B13_UL_MAX_CHAN         23279
#define LTE_B14_UL_MAX_CHAN         23379
#define LTE_B17_UL_MAX_CHAN         23849
#define LTE_B18_UL_MAX_CHAN         23999
#define LTE_B19_UL_MAX_CHAN         24149
#define LTE_B20_UL_MAX_CHAN         24449
#define LTE_B21_UL_MAX_CHAN         24599
#define LTE_B23_UL_MAX_CHAN         25699
#define LTE_B25_UL_MAX_CHAN         26689
#define LTE_B26_UL_MAX_CHAN         27039
#define LTE_B27_UL_MAX_CHAN         27209
#define LTE_B28_UL_MAX_CHAN         27659
#define LTE_B30_UL_MAX_CHAN         27759
#define LTE_B33_UL_MAX_CHAN         36199
#define LTE_B34_UL_MAX_CHAN         36349
#define LTE_B35_UL_MAX_CHAN         36949
#define LTE_B36_UL_MAX_CHAN         37549
#define LTE_B37_UL_MAX_CHAN         37749
#define LTE_B38_UL_MAX_CHAN         38249
#define LTE_B39_UL_MAX_CHAN         38649
#define LTE_B40_UL_MAX_CHAN         39649
#define LTE_B41_UL_MAX_CHAN         41589
#define LTE_B42_UL_MAX_CHAN         43589


/*Bandwidth in kHz  */

#define LTE_BW_1P4MHz	1400
#define LTE_BW_3MHz	3000
#define LTE_BW_5MHz	5000
#define LTE_BW_10MHz	10000
#define LTE_BW_15MHz	15000
#define LTE_BW_20MHz	20000
#define LTE_BW_40MHz	40000


// LTE Band mask definitions

#define LTE_B1_MASK_IDX   0
#define LTE_B2_MASK_IDX   1
#define LTE_B3_MASK_IDX   2
#define LTE_B4_MASK_IDX   3
#define LTE_B5_MASK_IDX   4
#define LTE_B6_MASK_IDX   5
#define LTE_B7_MASK_IDX   6
#define LTE_B8_MASK_IDX   7
#define LTE_B9_MASK_IDX   8
#define LTE_B10_MASK_IDX  9
#define LTE_B11_MASK_IDX  10
#define LTE_B12_MASK_IDX  11
#define LTE_B13_MASK_IDX  12
#define LTE_B14_MASK_IDX  13
#define LTE_B15_MASK_IDX  14
#define LTE_B16_MASK_IDX  15
#define LTE_B17_MASK_IDX  16
#define LTE_B18_MASK_IDX  17
#define LTE_B19_MASK_IDX  18
#define LTE_B20_MASK_IDX  19
#define LTE_B21_MASK_IDX  20
#define LTE_B22_MASK_IDX  21
#define LTE_B23_MASK_IDX  22
#define LTE_B24_MASK_IDX  23
#define LTE_B25_MASK_IDX  24
#define LTE_B26_MASK_IDX  25
#define LTE_B27_MASK_IDX  26
#define LTE_B28_MASK_IDX  27
#define LTE_B29_MASK_IDX  28
#define LTE_B30_MASK_IDX  29
#define LTE_B31_MASK_IDX  30
#define LTE_B32_MASK_IDX  31
#define LTE_B33_MASK_IDX  32
#define LTE_B34_MASK_IDX  33
#define LTE_B35_MASK_IDX  34
#define LTE_B36_MASK_IDX  35
#define LTE_B37_MASK_IDX  36
#define LTE_B38_MASK_IDX  37
#define LTE_B39_MASK_IDX  38
#define LTE_B40_MASK_IDX  39
#define LTE_B41_MASK_IDX  40
#define LTE_B42_MASK_IDX  41
#define LTE_B43_MASK_IDX  42
#define LTE_B44_MASK_IDX  43


#define LTE_B1_MASK   (uint64)(1<<(uint64)(LTE_B1_MASK_IDX))
#define LTE_B2_MASK   (uint64)(1<<(uint64)(LTE_B2_MASK_IDX))
#define LTE_B3_MASK   (uint64)(1<<(uint64)(LTE_B3_MASK_IDX))
#define LTE_B4_MASK   (uint64)(1<<(uint64)(LTE_B4_MASK_IDX))
#define LTE_B5_MASK   (uint64)(1<<(uint64)(LTE_B5_MASK_IDX))
#define LTE_B6_MASK   (uint64)(1<<(uint64)(LTE_B6_MASK_IDX))
#define LTE_B7_MASK   (uint64)(1<<(uint64)(LTE_B7_MASK_IDX))
#define LTE_B8_MASK   (uint64)(1<<(uint64)(LTE_B8_MASK_IDX))
#define LTE_B9_MASK   (uint64)(1<<(uint64)(LTE_B9_MASK_IDX))
#define LTE_B10_MASK  (uint64)(1<<(uint64)(LTE_B10_MASK_IDX))
#define LTE_B11_MASK  (uint64)(1<<(uint64)(LTE_B11_MASK_IDX))
#define LTE_B12_MASK  (uint64)(1<<(uint64)(LTE_B12_MASK_IDX))
#define LTE_B13_MASK  (uint64)(1<<(uint64)(LTE_B13_MASK_IDX))
#define LTE_B14_MASK  (uint64)(1<<(uint64)(LTE_B14_MASK_IDX))
#define LTE_B15_MASK  (uint64)(1<<(uint64)(LTE_B15_MASK_IDX))
#define LTE_B16_MASK  (uint64)(1<<(uint64)(LTE_B16_MASK_IDX))
#define LTE_B17_MASK  (uint64)(1<<(uint64)(LTE_B17_MASK_IDX))
#define LTE_B18_MASK  (uint64)(1<<(uint64)(LTE_B18_MASK_IDX))
#define LTE_B19_MASK  (uint64)(1<<(uint64)(LTE_B19_MASK_IDX))
#define LTE_B20_MASK  (uint64)(1<<(uint64)(LTE_B20_MASK_IDX))
#define LTE_B21_MASK  (uint64)(1<<(uint64)(LTE_B21_MASK_IDX))
#define LTE_B22_MASK  (uint64)(1<<(uint64)(LTE_B22_MASK_IDX))
#define LTE_B23_MASK  (uint64)(1<<(uint64)(LTE_B23_MASK_IDX))
#define LTE_B24_MASK  (uint64)(1<<(uint64)(LTE_B24_MASK_IDX))
#define LTE_B25_MASK  (uint64)(1<<(uint64)(LTE_B25_MASK_IDX))
#define LTE_B26_MASK  (uint64)(1<<(uint64)(LTE_B26_MASK_IDX))
#define LTE_B27_MASK  (uint64)(1<<(uint64)(LTE_B27_MASK_IDX))
#define LTE_B28_MASK  (uint64)(1<<(uint64)(LTE_B28_MASK_IDX))
#define LTE_B29_MASK  (uint64)(1<<(uint64)(LTE_B29_MASK_IDX))
#define LTE_B30_MASK  (uint64)(1<<(uint64)(LTE_B30_MASK_IDX))
#define LTE_B31_MASK  (uint64)(1<<(uint64)(LTE_B31_MASK_IDX))
#define LTE_B32_MASK  ((uint64)SYS_BM_64BIT(LTE_B32_MASK_IDX))
#define LTE_B33_MASK  ((uint64)SYS_BM_64BIT(LTE_B33_MASK_IDX))
#define LTE_B34_MASK  ((uint64)SYS_BM_64BIT(LTE_B34_MASK_IDX))
#define LTE_B35_MASK  ((uint64)SYS_BM_64BIT(LTE_B35_MASK_IDX))
#define LTE_B36_MASK  ((uint64)SYS_BM_64BIT(LTE_B36_MASK_IDX))
#define LTE_B37_MASK  ((uint64)SYS_BM_64BIT(LTE_B37_MASK_IDX))
#define LTE_B38_MASK  ((uint64)SYS_BM_64BIT(LTE_B38_MASK_IDX))
#define LTE_B39_MASK  ((uint64)SYS_BM_64BIT(LTE_B39_MASK_IDX))
#define LTE_B40_MASK  ((uint64)SYS_BM_64BIT(LTE_B40_MASK_IDX))
#define LTE_B41_MASK  ((uint64)SYS_BM_64BIT(LTE_B41_MASK_IDX))
#define LTE_B42_MASK  ((uint64)SYS_BM_64BIT(LTE_B42_MASK_IDX))
#define LTE_B43_MASK  ((uint64)SYS_BM_64BIT(LTE_B43_MASK_IDX))
#define LTE_B44_MASK  ((uint64)SYS_BM_64BIT(LTE_B44_MASK_IDX))



/*----------------------------------------------------------------------------*/
rfcom_lte_earfcn_type rflte_util_get_uarfcn_from_rx_freq(uint32 rx_freq, rfcom_lte_band_type band);

/*----------------------------------------------------------------------------*/
rfcom_lte_earfcn_type rflte_util_get_uarfcn_from_tx_freq(uint32 tx_freq, rfcom_lte_band_type band);

/*----------------------------------------------------------------------------*/
void rflte_util_get_tx_freq_lo_hi_from_band(rfcom_lte_band_type band, uint32 *f_ul_low, uint32 *f_ul_hi);

/*----------------------------------------------------------------------------*/
rfcom_lte_band_type rflte_util_get_band_from_rx_uarfcn(rfcom_lte_earfcn_type rx_chan);

/*----------------------------------------------------------------------------*/
rfcom_lte_band_type rflte_util_get_band_from_tx_uarfcn(rfcom_lte_earfcn_type tx_chan);

/*----------------------------------------------------------------------------*/
rfcom_lte_band_type
rflte_util_integer_band_to_rfcom_band(
  uint16 integer_band
);

/*----------------------------------------------------------------------------*/
uint32 rflte_util_get_tx_freq_from_tx_chan(rfcom_lte_earfcn_type tx_chan);

/*----------------------------------------------------------------------------*/
void rflte_util_get_tx_freq_lo_hi_from_rb(rfcom_lte_earfcn_type tx_chan, 
                                          rfcom_lte_bw_type tx_bw,
                                          uint8 rb_start, 
                                          uint8 rb_block, 
                                          uint32 *f_ul_low, 
                                          uint32 *f_ul_hi);

/*----------------------------------------------------------------------------*/
rflte_nv_tbl_type *rflte_util_init_nv_table(rfcom_device_enum_type device, rfcom_lte_band_type band);

/* ----------------------------------------------------------------------- */
rf_card_band_type  rflte_util_lte_band_helper(rfcom_lte_band_type rfcom_band);

/* ----------------------------------------------------------------------- */
rfcom_lte_band_type rflte_util_convert_rfc_band_to_rfcom_band(rf_card_band_type rfc_band);

/* ----------------------------------------------------------------------- */
rflte_nv_supported_band_type rflte_util_convert_rfcom_band_to_rfnv_band(rfcom_lte_band_type rfcom_band);
/* ----------------------------------------------------------------------- */
void rflte_util_get_tx_cal_boundaries(tx_band_cal_type *tx_band_cal_data, rfcom_device_enum_type device, rfcom_lte_band_type band);
/* ----------------------------------------------------------------------- */
void rflte_util_get_fbrx_cal_boundaries(rfcom_lte_earfcn_type *fbrx_cal_chan,uint8 fbrx_cal_chan_size);
/* ----------------------------------------------------------------------- */
rfcommon_fbrx_tx_cfg_type rflte_util_get_fbrx_bw_for_sys_bw(rfcom_lte_bw_type bw);

/*----------------------------------------------------------------------------*/
uint8 
rflte_util_get_pa_range_from_pa_state
(
  uint8                  pa_state, 
  rflte_nv_tbl_type     *rflte_nv_tbl_ptr,
  rfm_device_enum_type   rfm_device
);

/*----------------------------------------------------------------------------*/
rfcom_lte_band_type rflte_util_get_first_band_from_mask(uint64 band_mask);

/*----------------------------------------------------------------------------*/
boolean rflte_util_is_band_tdd( rfcom_lte_band_type rf_band);

/*----------------------------------------------------------------------------*/
boolean rflte_util_is_hdet_nv_present(rflte_nv_tbl_type   *rflte_nv_tbl_ptr);

/*----------------------------------------------------------------------------*/
rfcom_lte_earfcn_type rflte_util_get_rx_uarfcn_from_tx_uarfcn(rfcom_lte_earfcn_type tx_chan,rfcom_lte_band_type band);

/*----------------------------------------------------------------------------*/
rfcom_lte_earfcn_type rflte_util_get_tx_uarfcn_from_rx_uarfcn(rfcom_lte_earfcn_type rx_chan,rfcom_lte_band_type band);

/*----------------------------------------------------------------------------*/
rfnv_item_id_enum_type rflte_util_get_txlin_nvid(rfcom_lte_band_type band);

/*----------------------------------------------------------------------------*/
rfcom_lte_earfcn_type rflte_util_get_ul_arfcn_from_freq (uint32 freq);

/*----------------------------------------------------------------------------*/
rfcom_lte_band_type rflte_util_convert_nv_band_to_rfcom_band(rflte_nv_supported_band_type band);
/*----------------------------------------------------------------------------*/
boolean rflte_util_check_rfc_band_support(void);
/*----------------------------------------------------------------------------*/
rfnv_item_id_enum_type rflte_util_get_hdetVsAgc_nvid(rfcom_lte_band_type band);
/*----------------------------------------------------------------------------*/
uint16 rflte_util_get_num_rb_from_bandwidth(rfcom_lte_bw_type bw);
/*----------------------------------------------------------------------------*/
boolean rflte_util_get_min_max_rx_freqs(rfcom_lte_band_type band, uint32* min, uint32* max);
/*----------------------------------------------------------------------------*/
uint64 rflte_util_get_band_mask(rfcom_lte_band_type band);
/*----------------------------------------------------------------------------*/
void rflte_util_get_min_max_tx_freqs(rfcom_lte_band_type band,
                                   uint32* ful_low,
                                   uint32* ful_high);
/*----------------------------------------------------------------------------*/
boolean rflte_util_rfcom_band_to_integer_band(rfcom_lte_band_type band,uint16* num);
/*----------------------------------------------------------------------------*/
uint16 rflte_util_get_integer_band_from_rfcom_band(rfcom_lte_band_type band);
/*----------------------------------------------------------------------------*/
void rflte_util_get_fullband_from_subband(rfcom_lte_band_type* lte_rfcom_band);
/*----------------------------------------------------------------------------*/
uint32 rflte_util_get_tx_freq_from_uarfcn(rfcom_lte_earfcn_type tx_chan, rfcom_lte_band_type band);
/*----------------------------------------------------------------------------*/
uint16 rflte_util_get_bw_in_khz(rfcom_lte_bw_type bw);
/*----------------------------------------------------------------------------*/
uint32 rflte_util_get_rx_freq_from_uarfcn(rfcom_lte_earfcn_type rx_chan, rfcom_lte_band_type band);
/*----------------------------------------------------------------------------*/
int32 rflte_util_floor(int32 dividend, int32 divisor);
/*----------------------------------------------------------------------------*/
int32 rflte_util_get_txagc_from_dbm10( int16 dbm_10);
/*----------------------------------------------------------------------------*/
int16 rflte_util_get_dbm10_from_txagc( int32 txagc);
/*----------------------------------------------------------------------------*/
rfcom_lte_earfcn_type rflte_util_get_ul_dl_earfcn_offset( rfcom_lte_band_type lte_band );
/*----------------------------------------------------------------------------*/
/* New APIs to support band detection from RFC based on channel, bw and path override index */
rfcom_lte_band_type 
rflte_util_get_band_from_rx_uarfcn_bw_path_override(rfcom_lte_band_type current_band,
                                                    rfcom_device_enum_type device,
                                                    rfcom_lte_earfcn_type rx_chan,
                                                    rfcom_lte_bw_type bw,
                                                    uint8 path_override_index);
/*----------------------------------------------------------------------------*/
rfcom_lte_band_type 
rflte_util_get_band_from_tx_uarfcn_bw_path_override(rfcom_lte_band_type current_band,
                                                    rfcom_lte_earfcn_type tx_chan,
                                                    rfcom_lte_bw_type bw,
                                                    uint8 path_override_index);
/*----------------------------------------------------------------------------*/
rfcom_lte_bw_type rflte_util_get_rfcom_bw_type(uint16 rx_bw_calculated);
/*----------------------------------------------------------------------------*/
rfcom_lte_sub_band_type rflte_util_get_sub_band(rfcom_lte_band_type band);
/*----------------------------------------------------------------------------*/
rfcom_lte_band_type rflte_util_get_full_band(rfcom_lte_band_type band);
/*----------------------------------------------------------------------------*/
uint8 rflte_util_switch_port_index(uint8 current_port_index);
/*----------------------------------------------------------------------------*/
boolean rflte_util_is_dl_only_chan(rfcom_lte_earfcn_type chan);
/*----------------------------------------------------------------------------*/
boolean rflte_util_is_dl_only_band(rfcom_lte_band_type band);
/*----------------------------------------------------------------------------*/
uint64 rflte_util_get_splitbands_from_fullband(rfcom_lte_band_type band);
/*----------------------------------------------------------------------------*/
rfcom_lte_band_type rflte_util_sys_band_to_rfcom_band( sys_sband_lte_e_type band);

/*-----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
boolean rflte_util_find_nv_container_in_static_struct_and_return_item_id
(
   uint32 nv_container_idx,
   rfnv_rx_static_internal_data_type *rx_static_internal_data,
   uint32 *item_id
);

rfcom_lte_earfcn_type
rflte_util_dl_min_chan(
  rfcom_lte_band_type band
);

#ifdef __cplusplus
}
#endif

#endif

