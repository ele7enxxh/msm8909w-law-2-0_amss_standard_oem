#ifndef RF_TDSCDMA_MDSP_UTIL_H
#define RF_TDSCDMA_MDSP_UTIL_H

/*! 
  @file
  rf_tdscdma_mdsp_util.h
 
  @brief
  This file defines the utility function between Q6SW and Q6FW.

  @addtogroup RF_TDSCDMA_MDSP_UTIL_H
  @{
*/

/*==============================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Technologies Proprietary

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/qdsp6_tdscdma/inc/rf_tdscdma_mdsp_util.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
04/24/13   qzh     Porting FTM RF code from NIKEL to DIME
02/14/12   ycl     Rx LNA NV to MDSPconversion function.
12/20/11   ycl     Update RF/MDSP conversion function.
09/26/11   ycl     Change RxAGC calculation to match FTM AGC formula.
08/23/11   ycl     Change AGC conversion format.
08/15/11   ycl     Initial version. 

==============================================================================*/
#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 
    @brief
 	FW AGC unit
 	  a) 16Q9 dB512 unit for TxAGC
 	  b) 16Q8 dB256 unit for RxAGC
 	RF driver unit
 	  a) TxAGC
 		 0 ~ 1023 (10bits) AGC10 unit
 		 0 	  --- -70   dBm
 		 700  ---   0   dBm
 		 1023 ---  32.3 dBm 
 	  b) RxAGC
 		 -512 ~ 511
 		 -512 --- -106  dBm
 		  511 --- -3.7  dBm
*/

/* Common Section */
#define RF_TDSCDMA_MDSP_INT16_SIGNED_BIT                  ( 0x8000 )
#define RF_TDSCDMA_MDSP_DB10_SCALAR						  (   10   )

/* Rx Section 16Q8 */
#define RF_TDSCDMA_RXAGC_MIN_PWR_DBM10                    ( -1060  )
#define RF_TDSCDMA_RXAGC_MIN_SW_UNIT                      ( -512   )
#define RF_TDSCDMA_RXAGC_DBM10_SW_DELTA 				  ( 548 ) //1060 - 512
#define RF_TDSCDMA_MDSP_RXAGC_MDSP_SHFT                      8    

/* Tx Section 16Q9 */
#define RF_TDSCDMA_MDSP_TXAGC_MDSP_SHFT                      9    
#define RF_TDSCDMA_MDSP_TXAGC_MIN_TX_PWR_DBM10            (   -700 )
#define RF_TDSCDMA_MDSP_TXAGC_MIN_SW_UNIT                 (   0    )

#define RF_TDSCDMA_MDSP_TIME_US_TO_CHPX2_100_SHFT         (  8  )
#define RF_TDSCDMA_MDSP_TIME_DIVISION_FACTOR              ( 100 )

#define RF_TDSCDMA_MDSP_TXAGC_MDSP_16Q4_SHFT                 4

#define RF_TDSCDMA_SATURATE( val, min, max )  MAX( MIN( (val), (max) ), (min) )

#ifdef __cplusplus
}
#endif

/* Assume Arithmetic shift is supported! MDSP/RF driver conversion equation */
static inline int16 rf_tdscdma_mdsp_rxagc_mdsp_to_rf_unit
(
  int16 val
)
{ /* 16Q8 to 10 bit unit in RF Driver */ 
  int32 tmp_val = 0;

  tmp_val = (int32) val * RF_TDSCDMA_MDSP_DB10_SCALAR;
  return ((int16) ((tmp_val>>RF_TDSCDMA_MDSP_RXAGC_MDSP_SHFT)+RF_TDSCDMA_RXAGC_DBM10_SW_DELTA));
}

static inline int16 rf_tdscdma_mdsp_rxagc_rf_to_mdsp_unit
(
  int16 val
)
{ /* 10 bit RF unit to 16Q8 */
  int32 tmp_val;

  /* Convert to dBm10,  val - 1060 + 512 */
  tmp_val = val + RF_TDSCDMA_RXAGC_MIN_PWR_DBM10 - RF_TDSCDMA_RXAGC_MIN_SW_UNIT;
  return ( (int16) ((tmp_val<<RF_TDSCDMA_MDSP_RXAGC_MDSP_SHFT)/RF_TDSCDMA_MDSP_DB10_SCALAR ) );
}

/* Convert a positive mdsp 16q9 format to rf db10 value with rounding */
static inline uint16 rf_tdscdma_mdsp_txagc_mdsp_to_rf_unit
(
  uint16 val
)
{
  uint32 tmp_val;
  tmp_val = (( ((uint32) val) * RF_TDSCDMA_MDSP_DB10_SCALAR + (2<<(RF_TDSCDMA_MDSP_TXAGC_MDSP_SHFT-1)) ) \
    >> RF_TDSCDMA_MDSP_TXAGC_MDSP_SHFT) - \
	RF_TDSCDMA_MDSP_TXAGC_MIN_TX_PWR_DBM10 + RF_TDSCDMA_MDSP_TXAGC_MIN_SW_UNIT;
  return ( (uint16) tmp_val );
}

static inline int16 rf_tdscdma_mdsp_txagc_rf_to_mdsp_unit
(
  uint16 val
)
{
  int32 tmp_val;
  tmp_val = (( ((int32) val + RF_TDSCDMA_MDSP_TXAGC_MIN_TX_PWR_DBM10 ) << RF_TDSCDMA_MDSP_TXAGC_MDSP_SHFT) / \
	RF_TDSCDMA_MDSP_DB10_SCALAR );
  return ( (int16) tmp_val );
}

static inline int16 rf_tdscdma_agc10_to_mdsp_16q9
(
  int16 val
)
{
  int32 tmp_val;
  tmp_val = ( ((int32) val << RF_TDSCDMA_MDSP_TXAGC_MDSP_SHFT) / \
	RF_TDSCDMA_MDSP_DB10_SCALAR );
  return ( (int16) tmp_val );
}

static inline int16 rf_tdscdma_dBm10_to_mdsp_16q4
(
  int16 val
)
{
  int32 tmp_val;
  tmp_val = ( ((int32) val << RF_TDSCDMA_MDSP_TXAGC_MDSP_16Q4_SHFT) / \
	RF_TDSCDMA_MDSP_DB10_SCALAR );
  return ( (int16) tmp_val );
}

static inline int16 rf_tdscdma_mdsp_16q4_to_dB10(int16 val)
{
   int32 tmp_val;

   tmp_val = ( (int32)val * RF_TDSCDMA_MDSP_DB10_SCALAR ) >> 
      RF_TDSCDMA_MDSP_TXAGC_MDSP_16Q4_SHFT;

   return ( (int16) tmp_val );
}

static inline int16 rf_tdscdma_mdsp_txagc_to_dB10(int16 val)
{
   int32 tmp_val;

   tmp_val = ( (int32)val * RF_TDSCDMA_MDSP_DB10_SCALAR ) >> 
      RF_TDSCDMA_MDSP_TXAGC_MDSP_SHFT;

   return ( (int16) tmp_val );
}

static inline int16 rf_tdscdma_mdsp_rxagc_to_dB10(int16 val)
{
   int32 tmp_val;

   tmp_val = ( (int32)val * RF_TDSCDMA_MDSP_DB10_SCALAR ) >> 
      RF_TDSCDMA_MDSP_RXAGC_MDSP_SHFT;

   return ( (int16) tmp_val );
}


static inline int16 rf_tdscdma_mdsp_agc10_to_mdsp_16q8_unit
(
  int16 val
)
{
  int32 tmp_val;
  tmp_val = ((((int32)val)<<8)/10);
  return((int16)tmp_val);
}

static inline uint16 rf_tdscdma_mdsp_rx_mdsp_dbm10_to_rf
(
  int16 val
)
{ /* 16Q8 to 10 bit unit in RF Driver */ 
  int32 tmp_val = 0;

  tmp_val = (int32) val * RF_TDSCDMA_MDSP_DB10_SCALAR;
  return ((uint16) (tmp_val>>RF_TDSCDMA_MDSP_RXAGC_MDSP_SHFT));
}

static inline int16 rf_tdscdma_mdsp_util_micro_to_chpx2
( 
  int16 val
)
{
  int32 tmp_val = 0;

  tmp_val = (((int32)val) << RF_TDSCDMA_MDSP_TIME_US_TO_CHPX2_100_SHFT) /RF_TDSCDMA_MDSP_TIME_DIVISION_FACTOR ;

  return ((int16) tmp_val);
}


static inline int16 rf_tdscdma_mdsp_convert_lna_swpt_to_mdsp_16q8_unit
(
  int16 val
)
{
  int32 tmp_val;
  tmp_val = -((((int32)val)<<8)/2);
  return((int16)tmp_val);
}

#endif /* RF_TDSCDMA_MDSP_UTIL_H */
