/*! 
  @file
  rf_cdma_utils_freq.c
 
  @brief
  This file defines the common utility functions relating to frequency
  translation used by the CDMA RF driver.
 
  @addtogroup RF_CDMA_COMMON_UTILS
  @{
*/

/*==============================================================================

  Copyright (c) 2005 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/src/rf_cdma_utils_freq.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
07/17/14   adk     Added function rf_cdma_get_min_max_tx_freqs()
04/07/13   sty     KW fixes
05/30/13   JJ      Add function rf_cdma_rf_band_type_to_sys_band_type
05/20/13   sty     Code cleanup
04/15/13   APU     Added band chan validity for spreading rate 1 as per the 
                   standard.
02/05/13    kg     Added API's to return rfcom type band mask and max/min
                   freq based on band
06/29/12   spa     Compiler warning fix  
03/22/12    id     Added function to convert freq to chan
03/08/12   spa     Updated execute_wakeup_rx to use CDMA specific band and chan 
12/19/11   sty     Fixed bug in rf_cdma_get_rx_inter_chan_separation()
12/01/11   sty     [1] Implemented rf_cdma_get_rx_inter_chan_separation() and 
                       rf_cdma_convert_rx_freq_khz_to_chan()
                   [2] used rfm_cdma_band_class_type intead of rf_card_band_type
07/18/11   aro     Fixed the channel return type
06/22/11   Saul    Added test for Rx and Tx channel computations
06/22/11   Saul    Expanded tune chan computation to handle non-contiguous chans.
06/13/11   Saul    Now computing center channel for tuning.
03/18/11   bmg     Removed rf_cdma_get_freq_from_channel, has been replaced with
                   rf_cdma_get_tx_carrier_freq and rf_cdma_get_rx_carrier_freq.
03/17/11   bmg     Moved rf_cdma_get_freq_from_channel to this file
                   Added rf_cdma_sys_band_type_to_rf_band_type() to translate
                   between SYS band types and RF CDMA band types.
03/15/11   bmg     Created from rf_freq_util.c, including one or more of the
                   following changes:
 02/03/11  tws     Support BC5 Block L.
 12/14/10  cd      Fix channel offsets for BC10 for channels 720-919
 12/10/10  sar     rfa/api decoupling changes.
 09/09/10  cd      Corrected base frequency for BC10 rx and tx table 
 06/04/10  Vish    - Added valid chan step size information to Rx/Tx freq tables. 
                   - Fixed rf_get_[rx/tx]_carrier_freq() APIs to validate input channel.
                   - Updated rf_convert_rx_freq_khz_to_chan() to consider non-unity
                     valid channel number stepping (delta=2 as in the case of BC3).
                   - Fixed rf_get_rx_inter_chan_separation() to return the separation
                     between any two consecutive valid channel numbers.
 04/13/10  Vish    Added rf_get_rx_inter_chan_separation() to get inter-channel 
                   separation for a given band class. 
                   Modified rf_convert_rx_freq_khz_to_chan() to take additional
                   args to specify if the function should return lower/higher freq
                   RF channel for the case when the specified frequency lies exactly
                   between 2 valid RF channels in the specified band.
 03/29/10  Vish    Fixed signed to unsigned conversion bug in rf_get_tx_carrier_freq().
 03/05/10  zg      Added BC0 SubClass3 support. 
 03/21/09  dyc     Updated with rf1x headers.
 07/14/08  adk     Merged 8650 code.
 10/16/07  Vish    Added rf_convert_rx_freq_khz_2_chan().
                   Fixed bug in rf_get_rx_carrier_freq().
 12/16/07  dyc     BC10 support.
 03/09/07  bmg     BC15 feature fixes
 01/17/06  rmd     Fixed Lint error.
 01/16/06  rmd     Implemented BC11 and BC14 code review results. Including:
                   * Mainline BC11 and BC14 code.
                   * Moving Translate code into rf_util_freq.c.
                   * Moving rf_get_valid_bc_and_mask() into rf_util_freq.c.
                   * Replacing ASSERT() with RF_ASSERT() and ERROR_FATAL() with
                     RF_FATAL_ERROR().
 11/08/05  rmd     * Added support for BC11 and BC14.
                   * Added support for BC5 block K.
                   * Increased the resolution of the base frequency to KHz.
 10/21/05  sar     Fixed Lint warnings.
 10/17/05  sar     Initial Revision.
==============================================================================*/

#include "comdef.h"
#include "rf_cdma_utils_freq.h"
#include "rfcommon_msg.h"
#include "rfcommon_core_error.h"
#include "rfm_cdma_carrier_types.h"
#include "sys.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Table of mappings from band and channel to Rx carrier freqency.

  @details
  This structure holds the parameters required for calculation
  of Rx Carrier Frequency as described below:
    Find the row where the band is matched and chan lies between
    lo and hi limits then
      Rx Carrier Freq (KHz)= ( base + (spacing * (chan + offset)))

  Note:  The functions utilizing these tables perform the appropriate
  scaling within the calculations to return the Carrier frequencies in KHz.
*/
static const rf_carrier_freq_type rf_rx_freq_tbl[] =
{
  /*  band        low   high  spacing  offset ch_step  base (KHz)                   */
  {RFM_CDMA_BC0,     1,   799,  300,        0,      1,  870000}, /* BC0: Ch 1-799    */
  {RFM_CDMA_BC0,   991,  1023,  300,    -1023,      1,  870000}, /* BC0: Ch 991-1023 */
  {RFM_CDMA_BC0,  1024,  1323,  300,    -1024,      1,  860040}, /* BC0: Ch 1024-1323 */
  {RFM_CDMA_BC1,     0,  1199,  500,        0,      1, 1930000}, /* BC1: Ch 0-1199   */
  {RFM_CDMA_BC3,     2,   798,  125,        0,      2,  860000}, /* BC3: Ch 2-798    */
  {RFM_CDMA_BC3,   802,  1038,  125,     -800,      2,  843000}, /* BC3: Ch 802-1038 */
  {RFM_CDMA_BC3,  1042,  1198,  125,    -1040,      2,  832000}, /* BC3: Ch 1042-1198*/
  {RFM_CDMA_BC3,  1202,  1600,  125,    -1200,      2,  838000}, /* BC3: Ch 1202-1600*/
  {RFM_CDMA_BC4,     0,   599,  500,        0,      1, 1840000}, /* BC4: Ch 0-599    */
  {RFM_CDMA_BC5,     1,   400,  250,       -1,      1,  460000}, /* BC5: Ch 1-400    */
  {RFM_CDMA_BC5,   472,   871,  250,     -472,      1,  420000}, /* BC5: Ch 472-871  */
  {RFM_CDMA_BC5,  1039,  1473,  200,    -1024,      1,  461010}, /* BC5: Ch 1039-1473*/
  {RFM_CDMA_BC5,  1536,  1715,  250,    -1536,      1,  489000}, /* BC5: Ch 1536-1715*/
  {RFM_CDMA_BC5,  1792,  2016,  200,    -1792,      1,  489000}, /* BC5: Ch 1792-2016*/
  {RFM_CDMA_BC6,     0,  1199,  500,        0,      1, 2110000}, /* BC6: Ch 0-1199   */
  {RFM_CDMA_BC10,    0,   719,  250,        0,      1,  851000}, /* BC10:Ch 0-919    */
  {RFM_CDMA_BC10,  720,   919,  250,     -720,      1,  935000}, /* BC10:Ch 0-919    */
  {RFM_CDMA_BC11,    1,   400,  250,       -1,      1,  460000}, /* BC11:Ch 1-400    */
  {RFM_CDMA_BC11,  539,   871,  250,     -512,      1,  421000}, /* BC11:Ch 539-871  */
  {RFM_CDMA_BC11, 1039,  1473,  200,    -1024,      1,  461010}, /* BC11:Ch 1039-1473*/
  {RFM_CDMA_BC11, 1536,  1715,  250,    -1536,      1,  489000}, /* BC11:Ch 1536-1715*/
  {RFM_CDMA_BC11, 1792,  2016,  200,    -1792,      1,  489000}, /* BC11:Ch 1792-2016*/
  {RFM_CDMA_BC14,    0,  1299,  500,        0,      1, 1930000}, /* BC14:Ch 0-1299   */
  {RFM_CDMA_BC15,    0,   899,  500,        0,      1, 2110000}, /* BC15:Ch 0-899    */
};

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Table of mappings from band and channel to Rx carrier freqency.

  @details
  This structure holds the parameters required for calculation
  of Tx Carrier Frequency as described below:
    Find the row where the band is matched and chan lies between
    lo and hi limits then
      Tx Carrier Freq (KHz)= ( base + (spacing * (chan + offset)))

  Note:  The functions utilizing these tables perform the appropriate
  scaling within the calculations to return the Carrier frequencies in KHz.
*/
static const rf_carrier_freq_type rf_tx_freq_tbl[] =
{
  /*   band        low  high   spacing offset ch_step base (KHz) */
  {RFM_CDMA_BC0,     1,   799,  300,        0,      1,  825000}, /* BC0: Ch 1-799 */
  {RFM_CDMA_BC0,   991,  1023,  300,    -1023,      1,  825000}, /* BC0: Ch 991-1023 */
  {RFM_CDMA_BC0,  1024,  1323,  300,    -1024,      1,  815040}, /* BC0: Ch 1024-1323 */
  {RFM_CDMA_BC1,     0,  1199,  500,        0,      1, 1850000}, /* BC1: Ch 0-1199   */
  {RFM_CDMA_BC3,     2,   798,  125,        0,      2,  915000}, /* BC3: Ch 2-798    */
  {RFM_CDMA_BC3,   802,  1038,  125,     -800,      2,  898000}, /* BC3: Ch 802-1038 */
  {RFM_CDMA_BC3,  1042,  1198,  125,    -1040,      2,  887000}, /* BC3: Ch 1042-1198*/
  {RFM_CDMA_BC3,  1202,  1600,  125,    -1200,      2,  893000}, /* BC3: Ch 1202-1600*/
  {RFM_CDMA_BC4,     0,   599,  500,        0,      1, 1750000}, /* BC4: Ch 0-599    */
  {RFM_CDMA_BC5,     1,   400,  250,       -1,      1,  450000}, /* BC5: Ch 1-400    */
  {RFM_CDMA_BC5,   472,   871,  250,     -472,      1,  410000}, /* BC5: Ch 472-871  */
  {RFM_CDMA_BC5,  1039,  1473,  200,    -1024,      1,  451010}, /* BC5: Ch 1039-1473*/
  {RFM_CDMA_BC5,  1536,  1715,  250,    -1536,      1,  479000}, /* BC5: Ch 1536-1715*/
  {RFM_CDMA_BC5,  1792,  2016,  200,    -1792,      1,  479000}, /* BC5: Ch 1792-2016*/
  {RFM_CDMA_BC6,     0,  1199,  500,        0,      1, 1920000}, /* BC6: Ch 0-1199   */
  {RFM_CDMA_BC10,    0,   719,  250,        0,      1,  806000}, /* BC10:Ch 0-919    */
  {RFM_CDMA_BC10,  720,   919,  250,     -720,      1,  896000}, /* BC10:Ch 0-919    */
  {RFM_CDMA_BC11,    1,   400,  250,       -1,      1,  450000}, /* BC11:Ch 1-400    */
  {RFM_CDMA_BC11,  539,   871,  250,     -512,      1,  411000}, /* BC11:Ch 539-871  */
  {RFM_CDMA_BC11, 1039,  1473,  200,    -1024,      1,  451010}, /* BC11:Ch 1039-1473*/
  {RFM_CDMA_BC11, 1536,  1715,  250,    -1536,      1,  479000}, /* BC11:Ch 1536-1715*/
  {RFM_CDMA_BC11, 1792,  2016,  200,    -1792,      1,  479000}, /* BC11:Ch 1792-2016*/
  {RFM_CDMA_BC14,    0,  1299,  500,        0,      1, 1850000}, /* BC14:Ch 0-1299   */
  {RFM_CDMA_BC15,    0,   899,  500,        0,      1, 1710000}, /* BC15:Ch 0-899   */
};

enum 
{
  CHANNEL_VALID = 0 ,
  CHANNEL_CONDITIONALLY_VALID = 1 
} ;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Tables for Band class and its Sub Band Class if applicable, and channels.
*/
static const rf_cdma_band_descriptor rf_band_chan_tbl_bc0[] =
{
  /* ***** BC0 ***** ***** BC0 *****   */
  /*  sub_class  lo_chan    high_chan  iSChannelConditionallyValid */
  {    0 ,       1013 ,	  		1023 ,    CHANNEL_VALID                 },
  {    0 ,          1 ,		    311  ,    CHANNEL_VALID                 },
  {    0 ,        356 ,		    644  ,    CHANNEL_VALID                 },
  {    0 ,        689 ,   		694  ,    CHANNEL_VALID                 },
  {    0 ,        739 ,   		777  ,    CHANNEL_VALID                 },

  {    1 ,       1013 ,	  		1023 ,    CHANNEL_VALID                 },
  {    1 ,       1    ,	  		311  ,    CHANNEL_VALID                 },
  {    1 ,       356  ,	  		644  ,    CHANNEL_VALID                 },
  {    1 ,       689  ,	  		716  ,    CHANNEL_VALID                 },
  {    1 ,       717  ,	  		779  ,    CHANNEL_VALID                 },
                                   
  {    2 ,       991  ,	  		1023 ,    CHANNEL_VALID                 },
  {    2 ,       1    ,	  		142  ,    CHANNEL_VALID                 },
                                  
  {    3 ,       1048 ,	  		1323 ,    CHANNEL_VALID                 },
  {    3 ,       991  ,	  		1023 ,    CHANNEL_VALID                 },
  {    3 ,       1    ,	  		142  ,    CHANNEL_VALID                 } 
} ;

static const rf_cdma_band_descriptor rf_band_chan_tbl_bc1[] =
{
  /* ***** BC1 ***** ***** BC1 *****   */
  /*  sub_class  lo_chan    high_chan  iSChannelConditionallyValid */
  {    -1 ,      25   ,	  		1175 ,    CHANNEL_VALID                 }
} ;
   
static const rf_cdma_band_descriptor rf_band_chan_tbl_bc3[] =
{
  /* ***** BC3 ***** ***** BC3 *****   */
  /*  sub_class  lo_chan    high_chan  iSChannelConditionallyValid */
  {    -1 ,      1100 ,	  		1140 ,    CHANNEL_VALID                 },
  {    -1 ,      1260 ,	  		1600 ,    CHANNEL_VALID                 },
  {    -1 ,      801  ,	  		980  ,    CHANNEL_VALID                 },
  {    -1 ,      60   ,	  		740  ,    CHANNEL_VALID                 } 
} ;
   
static const rf_cdma_band_descriptor rf_band_chan_tbl_bc4[] =
{
  /* ***** BC4 ***** ***** BC4 *****   */
  /*  sub_class  lo_chan    high_chan  iSChannelConditionallyValid */
  {    -1 ,      25   ,	  		575  ,    CHANNEL_VALID                 },

} ;
   
static const rf_cdma_band_descriptor rf_band_chan_tbl_bc5[] =
{
  /* ***** BC5 ***** ***** BC5 *****   */
  /*  sub_class  lo_chan    high_chan  iSChannelConditionallyValid */
  {    0 ,       126  ,	  		145  ,    CHANNEL_CONDITIONALLY_VALID   },
  {    0 ,       146  ,	  		275  ,    CHANNEL_VALID                 },

  {    1 ,       106  ,	  		235  ,    CHANNEL_VALID                 },

  {    2 ,       26   ,	  		168  ,    CHANNEL_VALID                 },

  {    3 ,       564  ,	    	681  ,    CHANNEL_VALID                 },

  {    4 ,       717  ,	    	846  ,    CHANNEL_VALID                 },

  {    5 ,       1823 ,	    	1985 ,    CHANNEL_VALID                 },

  {    6 ,       1266 ,	    	1442 ,    CHANNEL_VALID                 },

  {    7 ,       1070 ,	    	1229 ,    CHANNEL_VALID                 },

  {    8 ,       79   ,	    	205  ,    CHANNEL_VALID                 },

  {    9 ,       235  ,	    	376  ,    CHANNEL_VALID                 },

  {    10,       1561 ,	    	1690 ,    CHANNEL_VALID                 },

  {    11,       505  ,	    	646  ,    CHANNEL_VALID                 },

  {    12,       97   ,	    	375  ,    CHANNEL_VALID                 },
  {    12,       2017 ,	    	2018 ,    CHANNEL_VALID                 },

  {    13,       26   ,	    	375  ,    CHANNEL_VALID                 },
  {    13,       2017 ,	    	2018 ,    CHANNEL_VALID                 }

} ;
   
static const rf_cdma_band_descriptor rf_band_chan_tbl_bc6[] =
{
  /* ***** BC6 ***** ***** BC6 *****   */
  /*  sub_class  lo_chan    high_chan  iSChannelConditionallyValid */
  {    -1 ,      25   ,	  		1175 ,    CHANNEL_VALID                 },

} ;
   
static const rf_cdma_band_descriptor rf_band_chan_tbl_bc10[] =
{
  /* ***** BC10***** ***** BC10*****   */
  /*  sub_class  lo_chan    high_chan  iSChannelConditionallyValid */
  {    0 ,       50   ,	    	670  ,    CHANNEL_VALID                 },
  {    4 ,       770  ,	  		870  ,    CHANNEL_VALID                 }

} ;
   
static const rf_cdma_band_descriptor rf_band_chan_tbl_bc11[] =
{
  /* ***** BC11***** ***** BC11*****   */
  /*  sub_class  lo_chan    high_chan  iSChannelConditionallyValid */
  {    0 ,       126  ,	  		145  ,    CHANNEL_CONDITIONALLY_VALID   },
  {    0 ,       146  ,	    	275  ,    CHANNEL_VALID                 },
   
  {    1 ,       106  ,	    	235  ,    CHANNEL_VALID                 },
   
  {    2 ,       26   ,	    	168  ,    CHANNEL_VALID                 },
   
  {    3 ,       564  ,	    	681  ,    CHANNEL_VALID                 },
   
  {    4 ,       717  ,	    	846  ,    CHANNEL_VALID                 },

  {    8 ,       79   ,	    	205  ,    CHANNEL_VALID                 },

  {    9 ,       235  ,	    	376  ,    CHANNEL_VALID                 },

  {    10,       1561 ,	    	1690 ,    CHANNEL_VALID                 },

  {    11,       505  ,	    	646  ,    CHANNEL_VALID                 }

} ;
   
static const rf_cdma_band_descriptor rf_band_chan_tbl_bc14[] =
{
  /* ***** BC14***** ***** BC14*****   */
  /*  sub_class  lo_chan    high_chan  iSChannelConditionallyValid */
  {    -1,       25   ,	    	1275 ,    CHANNEL_VALID                 }

} ;
   
static const rf_cdma_band_descriptor rf_band_chan_tbl_bc15[] =
{
  /* ***** BC15***** ***** BC15*****   */
  /*  sub_class  lo_chan    high_chan  iSChannelConditionallyValid */
  {    -1,       25   ,	    	875  ,    CHANNEL_VALID                 }

} ;
   
const rf_cdma_band_descriptor* cdma_band_ptr[] =
{
  rf_band_chan_tbl_bc0  ,
  rf_band_chan_tbl_bc1  ,
  rf_band_chan_tbl_bc3  ,
  rf_band_chan_tbl_bc4  ,
  rf_band_chan_tbl_bc5  ,
  rf_band_chan_tbl_bc6  ,
  rf_band_chan_tbl_bc10 ,
  rf_band_chan_tbl_bc11 ,
  rf_band_chan_tbl_bc14 ,
  rf_band_chan_tbl_bc15
} ;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Provides access to band-subClass-channel table.

  @param band
  The band whose table is queried.
 
  @param nos_records
  The size of the table queried.

  @return
  The band-subClass-channel table.
 
*/
const
rf_cdma_band_descriptor*
rf_get_band_descriptor 
( 
  rfm_cdma_band_class_type band , 
  size_t* nos_records 
)
{
  const rf_cdma_band_descriptor* band_descriptor = NULL ;
  switch ( band )
  {
    case RFM_CDMA_BC0 :
    band_descriptor = rf_band_chan_tbl_bc0 ;
    *nos_records = ( sizeof (rf_band_chan_tbl_bc0)/
                    sizeof(rf_band_chan_tbl_bc0[0]) ) ;
    break;

    case RFM_CDMA_BC1:
     band_descriptor = rf_band_chan_tbl_bc1 ;
     *nos_records = ( sizeof (rf_band_chan_tbl_bc1)/
                     sizeof(rf_band_chan_tbl_bc1[0]) ) ;
    break;

    case RFM_CDMA_BC3 :
    band_descriptor = rf_band_chan_tbl_bc3 ;
    *nos_records = ( sizeof (rf_band_chan_tbl_bc3)/
                    sizeof(rf_band_chan_tbl_bc3[0]) ) ;
    break;

    case RFM_CDMA_BC4 :
    band_descriptor = rf_band_chan_tbl_bc4 ;
    *nos_records = ( sizeof (rf_band_chan_tbl_bc4)/
                    sizeof(rf_band_chan_tbl_bc4[0]) ) ;
    break;

    case RFM_CDMA_BC5 :
    band_descriptor = rf_band_chan_tbl_bc5 ;
    *nos_records = ( sizeof (rf_band_chan_tbl_bc5)/
                    sizeof(rf_band_chan_tbl_bc5[0]) ) ;
    break;

    case RFM_CDMA_BC6 :
    band_descriptor = rf_band_chan_tbl_bc6 ;
    *nos_records = ( sizeof (rf_band_chan_tbl_bc6)/
                    sizeof(rf_band_chan_tbl_bc6[0]) ) ;
    break;

    case RFM_CDMA_BC10 :
    band_descriptor = rf_band_chan_tbl_bc10 ;
    *nos_records = ( sizeof (rf_band_chan_tbl_bc10)/
                    sizeof(rf_band_chan_tbl_bc10[0]) ) ;
    break;

    case RFM_CDMA_BC11 :
    band_descriptor = rf_band_chan_tbl_bc11 ;
    *nos_records = ( sizeof (rf_band_chan_tbl_bc11)/
                    sizeof(rf_band_chan_tbl_bc11[0]) ) ;
      break;

    case RFM_CDMA_BC14 :
    band_descriptor = rf_band_chan_tbl_bc14 ;
    *nos_records = ( sizeof (rf_band_chan_tbl_bc14)/
                    sizeof(rf_band_chan_tbl_bc14[0]) ) ;
    break;

    case RFM_CDMA_BC15 :
    band_descriptor = rf_band_chan_tbl_bc15 ;  
    *nos_records = ( sizeof (rf_band_chan_tbl_bc15)/
                    sizeof(rf_band_chan_tbl_bc15[0]) ) ;
    break ;

    default:
    RF_MSG_1 ( RF_ERROR , 
               "rf_get_band_descriptor: Invalid Band passed: %d" , band ) ;
    break;

  } /* switch ( rf_band ) */

  return band_descriptor ;

} /* rf_get_band_descriptor  */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Provides access to rx frequency table.

  @param size
  Provides the size of the table being provided.

  @return
  Rx frequency table.
*/
const rf_carrier_freq_type * 
rf_cdma_get_rx_freq_tbl
( 
  uint16 *size 
)
{
  *size = sizeof(rf_rx_freq_tbl)/sizeof(rf_rx_freq_tbl[0]);

  return rf_rx_freq_tbl;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Provides access to tx frequency table.

  @param size
  Provides the size of the table being provided.

  @return
  tx frequency table.
*/
const rf_carrier_freq_type * 
rf_cdma_get_tx_freq_tbl
( 
  uint16 * size 
)
{
  *size = sizeof(rf_tx_freq_tbl)/sizeof(rf_tx_freq_tbl[0]);

  return rf_tx_freq_tbl;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Computes the Tx carrier frequency in kHz of a band and channel.

  @details
  If the band or channel is not supported then 0 is returned.

  @param band
  3GPP2 band class.

  @param chan
  3GPP2 channel that should be valid for the band class in the band
  parameter.

  @return
  Tx frequency in kHz of the channel, or 0 if the RF driver does not
  support the channel.
*/
uint32
rf_cdma_get_tx_carrier_freq
(
  rfm_cdma_band_class_type band,
  int chan
)
{
  int32 freq=0;
  int32  delta = 0;
  int16 max_entries= sizeof(rf_tx_freq_tbl)/sizeof(rf_tx_freq_tbl[0]);
  int16 i = 0;

  while (i < max_entries)
  {
    if ((band == rf_tx_freq_tbl[i].band ) &&
        (chan >= rf_tx_freq_tbl[i].lo_chan ) &&
        (chan <= rf_tx_freq_tbl[i].hi_chan ))
      break;
    i++;
  }

  if ( (i < max_entries)
       && ( (1 == rf_tx_freq_tbl[i].ch_step)
            /* 'lo_chan' is the lowest valid channel#; the following checks if
               'chan' is integral multiples of 'ch_step' above 'lo_chan' */
            || ( ( (chan - rf_tx_freq_tbl[i].lo_chan) % rf_tx_freq_tbl[i].ch_step ) == 0 ) ) )
  {
    /* To avoid doing floating point math:
         1) All calculations are done in Khz.
         2) To avoid losing accuracy, the ch_sp (spacing) parameter in the
            table is scaled to 10 times the value in KHz. At the end of the
            delta calculation, the 10 is divided off.
    */           
    delta = (rf_tx_freq_tbl[i].ch_sp * (chan + rf_tx_freq_tbl[i].ch_offst))/10;
    freq = (rf_tx_freq_tbl[i].base_freq) + delta;
  }
  return(uint32) freq;
} /* rf_cdma_get_tx_carrier_freq() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Computes the Rx carrier frequency in kHz of a band and channel.

  @details
  If the band or channel is not supported then 0 is returned.

  @param band
  3GPP2 band class.

  @param chan
  3GPP2 channel that should be valid for the band class in the band
  parameter.

  @return
  Rx frequency in kHz of the channel, or 0 if the RF driver does not
  support the channel.
*/
uint32
rf_cdma_get_rx_carrier_freq
(
  rfm_cdma_band_class_type band,
  int chan
)
{
  int32 freq=0;
  int32  delta = 0;
  int16 max_entries= sizeof(rf_rx_freq_tbl)/sizeof(rf_rx_freq_tbl[0]);
  int16 i = 0;

  while (i < max_entries)
  {
    if ((band == rf_rx_freq_tbl[i].band ) &&
        (chan >= rf_rx_freq_tbl[i].lo_chan ) &&
        (chan <= rf_rx_freq_tbl[i].hi_chan ))
      break;
    i++;
  }

  if ( (i < max_entries)
       && ( (1 == rf_rx_freq_tbl[i].ch_step)
            /* 'lo_chan' is the lowest valid channel#; the following checks if
               'chan' is integral multiples of 'ch_step' above 'lo_chan' */
            || ( ( (chan - rf_rx_freq_tbl[i].lo_chan) % rf_rx_freq_tbl[i].ch_step ) == 0 ) ) )
  {
    /* To avoid doing floating point math:
         1) All calculations are done in Khz.
         2) To avoid losing accuracy, the ch_sp (spacing) parameter in the
            table is scaled to 10 times the value in KHz. At the end of the
            delta calculation, the 10 is divided off.
    */
    delta = (rf_rx_freq_tbl[i].ch_sp * (chan + rf_rx_freq_tbl[i].ch_offst))/10;
    freq = (rf_rx_freq_tbl[i].base_freq) + delta;
  }
  return(uint32) freq;
} /* rf_cdma_get_rx_carrier_freq() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Translates sys_band_class_e_type values to rfm_cdma_band_class_type

  @param band
  A band value in sys_band_class_e_type format that must correspond to a
  valid CDMA band.

  @return
  The equivalent value of the band parameter in the rfm_cdma_band_class_type
  domain.
*/
rfm_cdma_band_class_type
rf_cdma_sys_band_type_to_rf_band_type
(
  sys_band_class_e_type band
)
{
  if ( band >= SYS_BAND_BC0 && band <= SYS_BAND_BC19 )
  {
    /* Sys band types representation maps directly to RFM CDMA band
       class type representation. */
    return (rfm_cdma_band_class_type)band;
  }
  else
  {
    RF_MSG_1( RF_ERROR,
              "System band %d cannot be converted to a CDMA RF driver band value",
              band );
    return RFM_CDMA_MAX_BAND;
  }
} /* rf_cdma_sys_band_type_to_rf_band_type() */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Translates rfm_cdma_band_class_type values to sys_band_class_e_type 

  @param band
  A band value in rfm_cdma_band_class_type format that must correspond to a
  valid sys band.

  @return
  The equivalent value of the band parameter in the sys_band_class_e_type
  domain.
*/
sys_band_class_e_type 
rf_cdma_rf_band_type_to_sys_band_type
(
  rfm_cdma_band_class_type band
)
{
  if ( band >= RFM_CDMA_BC0 && band <= RFM_CDMA_BC19 )
  {
    /* RFM CDMA band class type representation maps directly to Sys band 
        types representation. */
    return (sys_band_class_e_type)band;
  }
  else
  {
    RF_MSG_1( RF_ERROR,
              "CDMA RF band %d cannot be converted to a sys band value",
              band );
    return SYS_BAND_BC_MAX;
  }
} /* rf_cdma_rf_band_type_to_sys_band_type() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Computes the actual tune channel based on the multi-channel list

  @param num_band_chan
  The total number of channels available in the list.
 
  @param band
  The band.
 
  @param chan_array
  The list of channels.

  @return
  The channel corresponding to the center of the channels list.
*/
rfm_cdma_chan_type
rf_cdma_compute_center_tune_chan
(
  const uint32 num_band_chan,
  const rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type * const chan_array
)
{
  uint16 center_chan = 0; /* The center channel to calculate */
  uint16 chan_index; /* Var to index through channels array */
  uint32 freq_khz = 0;/* Var to index through channels array */
  boolean status = FALSE;/* Var to hols status */
  
  if ( num_band_chan > RFM_CDMA_CARRIER_NUM )
  {

    RF_MSG_1( RF_ERROR, 
              "rf_cdma_compute_center_tune_chan: Invalid num of band_chan %d", 
              num_band_chan );
    return (center_chan = 0);
  }

  /* For now this expects channels to be adjacent in number and freq */
  for(chan_index = 0; chan_index < num_band_chan; ++chan_index)
  {
    freq_khz += rf_cdma_get_rx_carrier_freq( band, 
                                             chan_array[chan_index] );
  }

  if(num_band_chan > 1)
  {
    freq_khz /= num_band_chan;
  }

  status = rf_cdma_convert_rx_freq_khz_to_chan(
                                          freq_khz,
                                          band,
                                          RX_FREQ_KHZ_TO_CHAN_PREFERENCE__LOWER_FREQ,
                                          &center_chan,
                                          NULL
                                         );

  if(status != TRUE)
  {
    RF_MSG_2( RF_ERROR,
              "Tune chan %d for BC %d may not be valid",
              center_chan, band );
  }

  return center_chan;

} /* rf_cdma_compute_center_tune_chan */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return Rx Channel Separation

  @details
  This function returns the separation between adjacent valid channels in the
  specified band class.
 
  @param band
  Band under consideration
 
  @return
  On success, return the separation between adjacent channels (in KHz), and
  zero on failure
*/
int32 
rf_cdma_get_rx_inter_chan_separation
(
  rfm_cdma_band_class_type const band
)
{
   int16 i = 0;

   for (i=0; i < ARR_SIZE(rf_rx_freq_tbl); i++)
   {
      if (band == rf_rx_freq_tbl[i].band)
      {
        /* return in KHz */
        return (rf_rx_freq_tbl[i].ch_sp * rf_rx_freq_tbl[i].ch_step) / 10; 
      }
   }

   return 0;
} /* rf_cdma_get_rx_inter_chan_separation */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return Channel Number for given frequency

  @details
  This function returns the channel number that is located closest to the
  specified Rx carrier frequency (in KHz) in the indicated band. If the freq
  falls within the band, a successful result is returned.

  @param chan_freq_khz 
  RF freq in KHz on which a reverse channel lookup needs to be done
 
  @param band_class
  Band class in which the lookup needs to be done
 
  @param mid_point_preference
  Specifies if the function should return lower/higher freq RF channel for the 
  case when the specified frequency lies exactly between 2 valid RF channels in 
  the specified band
 
  @param chan_ptr
  Pointer through which the RF channel is returned to the caller
 
  @param converted_chan_freq_khz_ptr
  Pointer through which the frequency of the converted channel is returned to 
  the caller. Might be useful to the caller when the requested RF freq does NOT 
  line up exactly with a valid RF channel. Caller may specify NULL if this info 
  is not needed.

  @return
  Status indicating if channel falls within the given band class
*/
boolean
rf_cdma_convert_rx_freq_khz_to_chan
(
  uint32 const                       chan_freq_khz,
  rfm_cdma_band_class_type const     band_class,
  rx_freq_khz_to_chan_preference_type const mid_point_preference,
  word                               *const chan_ptr,
  uint32                             *const converted_chan_freq_khz_ptr
)
{
  boolean status = FALSE;
  uint32 index = 0;
  uint32 lo_freq_khz = 0;
  uint32 hi_freq_khz = 0;
  typedef struct
  {
     word   chan;
     uint32 chan_freq_khz;
     uint32 freq_offset_khz;
  } candidate_chan_type;
  candidate_chan_type candidate_chan_set[2]
     = {
          {0, 0, 0},
          {0, 0, 0}
       };
  uint32 candidate_chan_set_size = 0;
  uint32 temp_freq_offset_khz = 0;
  int32 inter_chan_khz_x_10 = 0;
  candidate_chan_type *result_ptr = NULL;

  for ( index = 0; index < ARR_SIZE(rf_rx_freq_tbl); index++ )
  {
    /* Process each band class matching entry in 'rf_rx_freq_tbl' as follows:
      - if the requested 'chan_freq_khz'
        - falls within the range of 'hi_chan' and 'lo_chan', then extrapolate
          to find out the closest channel. Return TRUE (SUCCESS) and the channel.
        - is below 'lo_chan' or above 'hi_chan'
          - by less than 'ch_sp' spacing, record the potential candidate in
            a candidate array. Continue to process other entries in 'rf_rx_freq_tbl'.
          - by more than or equal to 'ch_sp' spacing, ignore this. Continue to
            process other entries in 'rf_rx_freq_tbl'.
    
     After checking all entries in 'rf_rx_freq_tbl' (case where SUCCESSful result hasn't
     yet been obtained),
      - if there are exactly 2 entries in the candidate array, return TRUE (SUCCESS)
        and the closest channel in the candidate array.
      - else return FALSE (FAILURE).
    */
    if (band_class == rf_rx_freq_tbl[index].band)
    {
      lo_freq_khz = rf_cdma_get_rx_carrier_freq( band_class,
                                                 rf_rx_freq_tbl[index].lo_chan);
      hi_freq_khz = rf_cdma_get_rx_carrier_freq( band_class, 
                                                 rf_rx_freq_tbl[index].hi_chan);

      inter_chan_khz_x_10 = 
                   rf_rx_freq_tbl[index].ch_sp * rf_rx_freq_tbl[index].ch_step;
      if ( (chan_freq_khz >= lo_freq_khz)
           && (chan_freq_khz <= hi_freq_khz) )
      {
         temp_freq_offset_khz = (chan_freq_khz - lo_freq_khz);
         candidate_chan_set[0].chan = ( word )( rf_rx_freq_tbl[index].lo_chan
                                      + ( ( ( 10                     // since ch_sp is scaled by a factor of 10
                                              * temp_freq_offset_khz )
                                            / inter_chan_khz_x_10 )
                                          * rf_rx_freq_tbl[index].ch_step ) );
         candidate_chan_set[0].chan_freq_khz = rf_cdma_get_rx_carrier_freq(
                                                       band_class,
                                                       candidate_chan_set[0].chan);
         candidate_chan_set[0].freq_offset_khz =  chan_freq_khz
                                                  - candidate_chan_set[0].chan_freq_khz;
         if (0 == candidate_chan_set[0].freq_offset_khz)
         {
            /* either of lo_chan or hi_chan is exactly equal to candidate_chan_set[0].chan;
               in this case, make the 2nd candidate an alias of the 1st one in order to make use of
               the common logic for the best candidate detection. */
            candidate_chan_set[1] = candidate_chan_set[0];
         }
         else
         {
            candidate_chan_set[1].chan = candidate_chan_set[0].chan + rf_rx_freq_tbl[index].ch_step;
            candidate_chan_set[1].chan_freq_khz = rf_cdma_get_rx_carrier_freq(
                                                     band_class,
                                                     candidate_chan_set[1].chan);
            candidate_chan_set[1].freq_offset_khz =  candidate_chan_set[1].chan_freq_khz
                                                     - chan_freq_khz;
         }
         candidate_chan_set_size = 2;
      }
      else if (chan_freq_khz < lo_freq_khz)
      {
         temp_freq_offset_khz = (lo_freq_khz - chan_freq_khz);
         if ( (temp_freq_offset_khz*10) < ( uint32 )inter_chan_khz_x_10 )
         {
            candidate_chan_set[candidate_chan_set_size].chan = rf_rx_freq_tbl[index].lo_chan;
            candidate_chan_set[candidate_chan_set_size].chan_freq_khz = lo_freq_khz;
            candidate_chan_set[candidate_chan_set_size].freq_offset_khz = temp_freq_offset_khz;
            candidate_chan_set_size ++;
         }
      }
      else if (chan_freq_khz > hi_freq_khz)
      {
         temp_freq_offset_khz = (chan_freq_khz - hi_freq_khz);
         if ( (temp_freq_offset_khz*10) < ( uint32 )inter_chan_khz_x_10 )
         {
            candidate_chan_set[candidate_chan_set_size].chan = rf_rx_freq_tbl[index].hi_chan;
            candidate_chan_set[candidate_chan_set_size].chan_freq_khz = hi_freq_khz;
            candidate_chan_set[candidate_chan_set_size].freq_offset_khz = temp_freq_offset_khz;
            candidate_chan_set_size ++;
         }
      }

      RF_ASSERT(candidate_chan_set_size <= 2);

      if (2 == candidate_chan_set_size)
      {
         break;
      }
      /* else continue with searching for more candidate channel(s) */

    } /*if (band_class == rf_rx_freq_tbl[index].band) */
  } /* for (index = 0; index < ARR_SIZE(rf_rx_freq_tbl); index ++) */

   // At this point, there should be exactly 2 entries in candidate set for the successful case
   if (2 == candidate_chan_set_size)
   {
      if (candidate_chan_set[0].freq_offset_khz < candidate_chan_set[1].freq_offset_khz)
      {
         result_ptr = &candidate_chan_set[0];
      }
      else if (candidate_chan_set[1].freq_offset_khz < candidate_chan_set[0].freq_offset_khz)
      {
         result_ptr = &candidate_chan_set[1];
      }
      else
      {
         result_ptr = &candidate_chan_set[0];
         if ( ( (RX_FREQ_KHZ_TO_CHAN_PREFERENCE__LOWER_FREQ == mid_point_preference)
                && (candidate_chan_set[1].chan_freq_khz < candidate_chan_set[0].chan_freq_khz) )
              || ( (RX_FREQ_KHZ_TO_CHAN_PREFERENCE__HIGHER_FREQ == mid_point_preference)
                   && (candidate_chan_set[1].chan_freq_khz > candidate_chan_set[0].chan_freq_khz) ) )
         {
            result_ptr = &candidate_chan_set[1];
         }
      }

      *chan_ptr = result_ptr->chan;
      if (NULL != converted_chan_freq_khz_ptr)
      {
         *converted_chan_freq_khz_ptr = result_ptr->chan_freq_khz;
      }
      status = TRUE;
   }

   return status;

} /* rf_cdma_convert_rx_freq_khz_to_chan */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Computes the Tx channel of a band and carrier frequency.

  @details
  If the band or frequency is not supported then 0 is returned.

  @param band
  3GPP2 band class.

  @param freq
  Frequency in kHz that should be valid for the band class in the band
  parameter.

  @return
  Tx channel or 0 if the RF driver does not
  support the frequency in the corresponding band.
*/
uint16
rf_cdma_get_tx_chan_from_freq
(
  rfm_cdma_band_class_type band,
  uint32 freq
)
{
  uint16 chan=0;
  int32  delta = 0;
  int16 max_entries= sizeof(rf_tx_freq_tbl)/sizeof(rf_tx_freq_tbl[0]);
  int16 i = 0;
  uint32 lo_freq_khz = 0;
  uint32 hi_freq_khz = 0;

  while (i < max_entries)
  {
    if (band == rf_tx_freq_tbl[i].band)
      break;
    i++;
  }

  /* Kw fix */
  if (i == max_entries)
  {
    RF_MSG_1( RF_ERROR, "Invalid Frequency ", 0);
    return (0); 
  }
  lo_freq_khz = rf_cdma_get_tx_carrier_freq( band,
                                             rf_tx_freq_tbl[i].lo_chan);
  hi_freq_khz = rf_cdma_get_tx_carrier_freq( band, 
                                             rf_tx_freq_tbl[i].hi_chan);

  if( (freq >= lo_freq_khz) && (freq <= hi_freq_khz) )
  {
    /* To avoid doing floating point math:
         1) All calculations are done in Khz.
         2) To avoid losing accuracy, the ch_sp (spacing) parameter in the
            table is scaled to 10 times the value in KHz. That is ajusted in 
            the calculation here.
       Tx Chan = ((Tx Carrier Frequency(KHz) - base) * 10 / (spacing)) - offset
    */           
    delta = (freq - rf_tx_freq_tbl[i].base_freq)*10;
    chan = (uint16) ( ( delta / rf_tx_freq_tbl[i].ch_sp ) - 
           rf_tx_freq_tbl[i].ch_offst ); 
    return(chan);
  }
  else
  {
    return(0);
  }
} /* rf_cdma_get_tx_chan_from_freq */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function gets the min and max Rx freq (in KHz) of a CDMA band

  @details

  @return
  boolean TRUE if band is supported, FALSE otherwise
*/
boolean 
rf_cdma_get_min_max_rx_freqs
(
  rfm_cdma_band_class_type band, 
  uint32* min, 
  uint32* max
)
{
  boolean ret_val = TRUE;
  switch(band)
  {
  case RFM_CDMA_BC0:
    *min = RF_CDMA_BC0_RX_MIN_FREQ;
    *max = RF_CDMA_BC0_RX_MAX_FREQ;
    break;

  case RFM_CDMA_BC1:
    *min = RF_CDMA_BC1_RX_MIN_FREQ;
    *max = RF_CDMA_BC1_RX_MAX_FREQ;
    break;

  case RFM_CDMA_BC3:
    *min = RF_CDMA_BC3_RX_MIN_FREQ;
    *max = RF_CDMA_BC3_RX_MAX_FREQ;
    break;

  case RFM_CDMA_BC4:
    *min = RF_CDMA_BC4_RX_MIN_FREQ;
    *max = RF_CDMA_BC4_RX_MAX_FREQ;
    break;

  case RFM_CDMA_BC5:
    *min = RF_CDMA_BC5_RX_MIN_FREQ;
    *max = RF_CDMA_BC5_RX_MAX_FREQ;
    break;

  case RFM_CDMA_BC6:
    *min = RF_CDMA_BC6_RX_MIN_FREQ;
    *max = RF_CDMA_BC6_RX_MAX_FREQ;
    break;

  case RFM_CDMA_BC10:
    *min = RF_CDMA_BC10_RX_MIN_FREQ;
    *max = RF_CDMA_BC10_RX_MAX_FREQ;
    break;

  case RFM_CDMA_BC11:
    *min = RF_CDMA_BC11_RX_MIN_FREQ;
    *max = RF_CDMA_BC11_RX_MAX_FREQ;
    break;

  case RFM_CDMA_BC14:
    *min = RF_CDMA_BC14_RX_MIN_FREQ;
    *max = RF_CDMA_BC14_RX_MAX_FREQ;
    break;

  case RFM_CDMA_BC15:
    *min = RF_CDMA_BC15_RX_MIN_FREQ;
    *max = RF_CDMA_BC15_RX_MAX_FREQ;
    break;

  default:
    // RF_MSG_1(RF_MED, "RF_get_min_max_rx_freqs: CDMA band %d NOT SUPPORTED!",
    //          band);
    ret_val = FALSE;
  } // end switch

  return ret_val;
} /*rf_cdma_get_min_max_rx_freqs*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function gets the min and max Tx freq (in KHz) of a CDMA band

  @details

  @return
  boolean TRUE if band is supported, FALSE otherwise
*/
boolean 
rf_cdma_get_min_max_tx_freqs
(
  rfm_cdma_band_class_type band, 
  uint32* min, 
  uint32* max
)
{
  boolean ret_val = TRUE;
  switch( band)
  {
  case RFM_CDMA_BC0:
    *min = RF_CDMA_BC0_TX_MIN_FREQ;
    *max = RF_CDMA_BC0_TX_MAX_FREQ;
    break;

  case RFM_CDMA_BC1:
    *min = RF_CDMA_BC1_TX_MIN_FREQ;
    *max = RF_CDMA_BC1_TX_MAX_FREQ;
    break;

  case RFM_CDMA_BC3:
    *min = RF_CDMA_BC3_TX_MIN_FREQ;
    *max = RF_CDMA_BC3_TX_MAX_FREQ;
    break;

  case RFM_CDMA_BC4:
    *min = RF_CDMA_BC4_TX_MIN_FREQ;
    *max = RF_CDMA_BC4_TX_MAX_FREQ;
    break;

  case RFM_CDMA_BC5:
    *min = RF_CDMA_BC5_TX_MIN_FREQ;
    *max = RF_CDMA_BC5_TX_MAX_FREQ;
    break;

  case RFM_CDMA_BC6:
    *min = RF_CDMA_BC6_TX_MIN_FREQ;
    *max = RF_CDMA_BC6_TX_MAX_FREQ;
    break;

  case RFM_CDMA_BC10:
    *min = RF_CDMA_BC10_TX_MIN_FREQ;
    *max = RF_CDMA_BC10_TX_MAX_FREQ;
    break;

  case RFM_CDMA_BC11:
    *min = RF_CDMA_BC11_TX_MIN_FREQ;
    *max = RF_CDMA_BC11_TX_MAX_FREQ;
    break;

  case RFM_CDMA_BC14:
    *min = RF_CDMA_BC14_TX_MIN_FREQ;
    *max = RF_CDMA_BC14_TX_MAX_FREQ;
    break;

  case RFM_CDMA_BC15:
    *min = RF_CDMA_BC15_TX_MIN_FREQ;
    *max = RF_CDMA_BC15_TX_MAX_FREQ;
    break;

  default:
    // RF_MSG_1(RF_MED, "RF_get_min_max_tx_freqs: CDMA band %d NOT SUPPORTED!",
    //          band);
    ret_val = FALSE;
  } // end switch

  return ret_val;
} /*rf_cdma_get_min_max_tx_freqs*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function gets the CDMA mask

  @return
  uint8 CDMA mask  
*/

uint64
rf_cdma_get_band_mask
(
  rfm_cdma_band_class_type band
)
{
  uint64 ret_val = 0;

  switch(band)	 
  {
  case  RFM_CDMA_BC0:
    ret_val = RF_CDMA_BC0_MASK;
    break;
   
  case RFM_CDMA_BC1:
     ret_val = RF_CDMA_BC1_MASK;
    break;

  case  RFM_CDMA_BC2:
    ret_val = RF_CDMA_BC2_MASK;
    break;

  case  RFM_CDMA_BC3:
    ret_val = RF_CDMA_BC3_MASK;
    break;
   
  case  RFM_CDMA_BC4:
    ret_val = RF_CDMA_BC4_MASK;
    break;

  case  RFM_CDMA_BC5:
    ret_val = RF_CDMA_BC5_MASK;
    break;
   
  case  RFM_CDMA_BC6:
    ret_val = RF_CDMA_BC6_MASK;
    break;
   
  case  RFM_CDMA_BC7:
    ret_val = RF_CDMA_BC7_MASK;
    break;

  case  RFM_CDMA_BC8:
    ret_val = RF_CDMA_BC8_MASK;
    break;
     case  RFM_CDMA_BC9:
    ret_val = RF_CDMA_BC9_MASK;
    break;
   
  case  RFM_CDMA_BC10:
    ret_val = RF_CDMA_BC10_MASK;
    break;
   
  case RFM_CDMA_BC11 :
    ret_val = RF_CDMA_BC11_MASK;
    break;

  case  RFM_CDMA_BC12:
    ret_val = RF_CDMA_BC12_MASK;
    break;
    	
    case  RFM_CDMA_BC13:
    ret_val = RF_CDMA_BC13_MASK;
    break;
     case  RFM_CDMA_BC14:
    ret_val = RF_CDMA_BC14_MASK;
    break;
   
  case  RFM_CDMA_BC15:
    ret_val = RF_CDMA_BC15_MASK;
    break;
   
  case  RFM_CDMA_BC16:
    ret_val = RF_CDMA_BC16_MASK;
    break;

  case  RFM_CDMA_BC17:
    ret_val = RF_CDMA_BC17_MASK;
    break;

  case  RFM_CDMA_BC18:
    ret_val = RF_CDMA_BC18_MASK;
    break;
   
  case  RFM_CDMA_BC19:
    ret_val = RF_CDMA_BC19_MASK;
    break;

  case  RFM_CDMA_BC20:
    ret_val = RF_CDMA_BC20_MASK;
    break;

  default:
    RF_MSG_1(RF_MED, "rf_cdma_get_band_mask: Invalid CDMA band %d!", band);
    break;
  }

  return ret_val;
} /* rf_cdma_get_band_mask */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function gets the CDMA mask for the RFM band

  @return
  uint64 CDMA mask  
*/

uint64
rf_cdma_get_sys_band_mask
(
  rfm_cdma_band_class_type band
)
{
  uint64 ret_val = 0;

  switch(band)
  {
  case  RFM_CDMA_BC0:
    ret_val = SYS_BAND_MASK_BC0;
    break;
   
  case RFM_CDMA_BC1:
     ret_val = SYS_BAND_MASK_BC1;
    break;

  case  RFM_CDMA_BC2:
    ret_val = SYS_BAND_MASK_BC2;
    break;

  case  RFM_CDMA_BC3:
    ret_val = SYS_BAND_MASK_BC3;
    break;
   
  case  RFM_CDMA_BC4:
    ret_val = SYS_BAND_MASK_BC4;
    break;

  case  RFM_CDMA_BC5:
    ret_val = SYS_BAND_MASK_BC5;
    break;
   
  case  RFM_CDMA_BC6:
    ret_val = SYS_BAND_MASK_BC6;
    break;
   
  case  RFM_CDMA_BC7:
    ret_val = SYS_BAND_MASK_BC7;
    break;

  case  RFM_CDMA_BC8:
    ret_val = SYS_BAND_MASK_BC8;
    break;

  case  RFM_CDMA_BC9:
    ret_val = SYS_BAND_MASK_BC9;
    break;
   
  case  RFM_CDMA_BC10:
    ret_val = SYS_BAND_MASK_BC10;
    break;
   
  case RFM_CDMA_BC11 :
    ret_val = SYS_BAND_MASK_BC11;
    break;

  case  RFM_CDMA_BC12:
    ret_val = SYS_BAND_MASK_BC12;
    break;

  case  RFM_CDMA_BC14:
    ret_val = SYS_BAND_MASK_BC14;
    break;
   
  case  RFM_CDMA_BC15:
    ret_val = SYS_BAND_MASK_BC15;
    break;
   
  case  RFM_CDMA_BC16:
    ret_val = SYS_BAND_MASK_BC16;
    break;

  case  RFM_CDMA_BC17:
    ret_val = SYS_BAND_MASK_BC17;
    break;

  case  RFM_CDMA_BC18:
    ret_val = SYS_BAND_MASK_BC18;
    break;
   
  case  RFM_CDMA_BC19:
    ret_val = SYS_BAND_MASK_BC19;
    break;

  default:
    RF_MSG_1(RF_MED, "rf_cdma_get_sys_band_mask: Invalid CDMA band %d!", band);
    break;
  }

  return ret_val;
} /* rf_cdma_get_sys_band_mask */

/*! @} */

