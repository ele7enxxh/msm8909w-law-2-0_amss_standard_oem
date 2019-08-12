/*!
  @file 
  rfd_grfc.h

  @brief
  header file for all grfc typedef and functions that are made available to
  external mododule

*/

/*===========================================================================

  Copyright (c) 2011, 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfd/common/inc/rfd_grfc.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/01/12   sbm     Clean-up un-used functions.
04/07/12   ycl     Added DAC0_UPDATE GRFC.
02/15/12   dej     Added function to enable/disable grfc_stall_bypass_register
02/10/11   tnt     Modify for nikeL  modem
07/12/10   tnt     Resolve name conflict for TX_ON
07/01/10   tnt     Adding Common RFC support
02/03/10   tnt     RFD re-organization
04/10/09   tnt     Initial Check-in
===========================================================================*/
#ifndef RFD_GRFC_H
#define RFD_GRFC_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "msm.h"
#include "rfd_grfc_modem.h"

/*===========================================================================

                   EXTERNAL MACROS AND TYPES

===========================================================================*/
/*! bitmask = 32 bits mask, each bit represents an grfc engine, lsb = engine 0, msb = engine 31 */
/*  0 = active high, 1 = active low */
#define RFD_GRFC_POL_CONFIG_ALL_ENGINES(bitmask)		HWIO_OUT( GRFC_POL_CONFIG, bitmask )

/*! val = 32 bits, each bit represents an grfc engine, lsb = engine 0, msb = engine 31 */
/*  0 = active high, 1 = active low */
#define RFD_GRFC_POL_CONFIG_READ_ALL_ENGINES(val)		val = HWIO_IN( GRFC_POL_CONFIG )

/*! engine = grfc engine, engine = [0..31] */
/*  pol = polarity, 0 = active high, 1 = active low */
#define RFD_GRFC_POL_CONFIG_ONE_ENGINE(engine,pol)		HWIO_OUTM(GRFC_POL_CONFIG,1<<engine,pol<<engine)

/*! engine = grfc engine, engine = [0..31] */
/*  val = polarity, 0 = active high, 1 = active low */
#define RFD_GRFC_POL_CONFIG_READ_ONE_ENGINE(engine,val)	val = ( HWIO_INM(GRFC_POL_CONFIG,1<<engine) >> engine )

/*! bitmask = 13 bits mask, each bit represents an grfc rf engine, lsb = engine 0, 12th bit = grfc rf engine 12 */
/*  0 = active high, 1 = active low */
#define RFD_GRFC_POL_CONFIG_RF_ALL_ENGINES(bitmask)		HWIO_OUT( GRFC_POL_CONFIG_RF, bitmask )

/*! val = 13 bits, each bit represents an grfc rf engine, lsb = engine 0, 12th bit = grfc rf engine 12 */
/*  0 = active high, 1 = active low */
#define RFD_GRFC_POL_CONFIG_RF_READ_ALL_ENGINES(val)	val = HWIO_IN( GRFC_POL_CONFIG_RF )

/*! engine = grfc rf engine, engine = [0..12] */
/*  pol = polarity, 0 = active high, 1 = active low */
#define RFD_GRFC_POL_CONFIG_RF_ONE_ENGINE(engine,pol)	HWIO_OUTM(GRFC_POL_CONFIG_RF,1<<engine,pol<<engine)

/*! engine = grfc rf engine, engine = [0..12] */
/*  val = polarity, 0 = active high, 1 = active low */
#define RFD_GRFC_POL_CONFIG_RF_READ_ONE_ENGINE(engine,val) val = ( HWIO_INM(GRFC_POL_CONFIG_RF,1<<engine) >> engine )

/*! bitmask = 32 bit mask, each bit represents an grfc engine, lsb = engine 0, msb = engine 31 */
/*  0 = clear, normal grfc effective, 1 = activate */
#define RFD_GRFC_ACTIVATE_IMM_ALL_ENGINES(bitmask)		HWIO_OUT(GRFC_DEACTIVATE_IMM, 0x0 ); \
														HWIO_OUT(GRFC_ACTIVATE_IMM, bitmask ); \
														HWIO_OUT(GRFC_ACTIVATE_IMM, 0x0 )

/*! engine = grfc engine, engine = [0..31] */
/*  activate GRFC ENGINE,engine, and clear the bit after done */
#define RFD_GRFC_ACTIVATE_IMM_ONE_ENGINE(engine)		HWIO_OUTM(GRFC_DEACTIVATE_IMM,1<<engine,0x0); \
														HWIO_OUTM(GRFC_ACTIVATE_IMM,1<<engine,1<<engine); \
                                                        HWIO_OUTM(GRFC_ACTIVATE_IMM,1<<engine,0x0)

/*! bitmask = 13 bits mask, each bit represents an grfc rf engine, lsb = engine 0, 12th bit = grfc rf engine 12 */
/*  0 = clear, normal grfc effective, 1 = activate */
#define RFD_GRFC_ACTIVATE_IMM_RF_ALL_ENGINES(bitmask)	HWIO_OUT(GRFC_DEACTIVATE_IMM_RF, 0x0 ); \
														HWIO_OUT(GRFC_ACTIVATE_IMM_RF, bitmask ); \
														HWIO_OUT(GRFC_ACTIVATE_IMM_RF, 0x0 )
/*! engine = grfc engine, engine = [0..31] */
/*  activate RF GRFC ENGINE,engine, and clear the bit after done */
#define RFD_GRFC_ACTIVATE_IMM_RF_ONE_ENGINE(engine)	    HWIO_OUTM(GRFC_DEACTIVATE_IMM_RF,1<<engine,0x0); \
														HWIO_OUTM(GRFC_ACTIVATE_IMM_RF,1<<engine,1<<engine); \
														HWIO_OUTM(GRFC_ACTIVATE_IMM_RF,1<<engine,0x0)

/*! bitmask = 32 bit mask, each bit represents an grfc engine, lsb = engine 0, msb = engine 31 */
/*  0 = clear, normal grfc effective, 1 = deactivate */
#define RFD_GRFC_DEACTIVATE_IMM_ALL_ENGINES(bitmask)	HWIO_OUT(GRFC_ACTIVATE_IMM,0x0); \
														HWIO_OUT(GRFC_DEACTIVATE_IMM, bitmask); \
														HWIO_OUT(GRFC_DEACTIVATE_IMM, 0x0)

/*! engine = grfc engine, engine = [0..31] */
/*  deactivate GRFC ENGINE,engine, and clear the bit after done */
#define RFD_GRFC_DEACTIVATE_IMM_ONE_ENGINE(engine)	    HWIO_OUTM(GRFC_ACTIVATE_IMM,1<<engine,0x0); \
														HWIO_OUTM(GRFC_DEACTIVATE_IMM,1<<engine,1<<engine); \
														HWIO_OUTM(GRFC_DEACTIVATE_IMM,1<<engine,0x0)
													
/*! bitmask = 13 bits mask, each bit represents an grfc rf engine, lsb = engine 0, 12th bit = grfc rf engine 12 */
/*  0 = clear, normal grfc effective, 1 = deactivate */
#define RFD_GRFC_DEACTIVATE_IMM_RF_ALL_ENGINES(bitmask)	HWIO_OUT(GRFC_ACTIVATE_IMM_RF,0x0); \
														HWIO_OUT(GRFC_DEACTIVATE_IMM_RF, bitmask); \
														HWIO_OUT(GRFC_DEACTIVATE_IMM_RF, 0x0)
/*! engine = grfc engine, engine = [0..31] */
/*  deactivate GRFC ENGINE,engine, and clear the bit after done */
#define RFD_GRFC_DEACTIVATE_IMM_RF_ONE_ENGINE(engine)	HWIO_OUTM(GRFC_ACTIVATE_IMM_RF,1<<engine,0x0); \
														HWIO_OUTM(GRFC_DEACTIVATE_IMM_RF,1<<engine,1<<engine); \
														HWIO_OUTM(GRFC_DEACTIVATE_IMM_RF,1<<engine,0x0)

/*----------------------------------------------------------------------------*/
/*!
  @brief return type of grfc APIs

*/
typedef enum
{
    GRFC_SUCCESS = 0,
    GRFC_FAILURE = 1
} rfd_grfc_result_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief polarity type of GRFC Engines

*/
typedef enum
{
    GRFC_ENGINE_POLARITY_NORMAL   = 0,  /*Active HIGH*/
    GRFC_ENGINE_POLARITY_INVERTED = 1   /*Active LOW */
} rfd_grfc_engine_polarity_enum_type;

/*----------------------------------------------------------------------------*/
/*!
   @brief 
   All 14 control signals input to RFD Routing Table

   @details 
   These input will drive GRFCs
*/
typedef enum
{
    PA_RANGE_0     = 0,
    PA_RANGE_1     = 1,
    PA_RANGE2_0    = 2,
    PA_RANGE2_1    = 3,
    TX_ON_SRC      = 4, 
    TX_ON2_SRC     = 5,
    PA_ON_SRC      = 6,
    PA_ON2_SRC     = 7,
    TRK_LO_ADJ     = 8,
    TRK_LO_ADJ_OE  = 9,
    TX_AGC_ADJ     = 10,
    TX_AGC_ADJ_OE  = 11,
    TX_DAC_UPATE   = 12,
    GND            = 13,
    VDD            = 14,
    RF_CTL_SIG_MAX = 15
}rfd_rf_control_signal_type;

/*----------------------------------------------------------------------------*/
/*!
   @brief 
   All 40 GRFC output pins

   @details 
   These output pins will be driven by the GRFC engines or rf_control_signals
*/
typedef enum
{
    GRFC_OUT_00 = 0,
    GRFC_OUT_01 = 1,
    GRFC_OUT_02 = 2,
    GRFC_OUT_03 = 3,
    GRFC_OUT_04 = 4,
    GRFC_OUT_05 = 5,
    GRFC_OUT_06 = 6,
    GRFC_OUT_07 = 7,
    GRFC_OUT_08 = 8,
    GRFC_OUT_09 = 9,
    GRFC_OUT_10 = 10,
    GRFC_OUT_11 = 11,
    GRFC_OUT_12 = 12,
    GRFC_OUT_13 = 13,
    GRFC_OUT_14 = 14,
    GRFC_OUT_15 = 15,
    GRFC_OUT_16 = 16,
    GRFC_OUT_17 = 17,
    GRFC_OUT_18 = 18,
    GRFC_OUT_19 = 19,
    GRFC_OUT_20 = 20,
    GRFC_OUT_21 = 21,
    GRFC_OUT_22 = 22,
    GRFC_OUT_23 = 23,
    GRFC_OUT_24 = 24,
    GRFC_OUT_25 = 25,
    GRFC_OUT_26 = 26,
    GRFC_OUT_27 = 27,
    GRFC_OUT_28 = 28,
    GRFC_OUT_29 = 29,
    GRFC_OUT_30 = 30,
    GRFC_OUT_31 = 31,
    GRFC_OUT_32 = 32,
    GRFC_OUT_33 = 33,
    GRFC_OUT_34 = 34,
    GRFC_OUT_35 = 35,
    GRFC_OUT_36 = 36,
    GRFC_OUT_37 = 37,
    GRFC_OUT_38 = 38,
    GRFC_OUT_39 = 39,
    GRFC_OUT_MAX = 40
}rfd_rt_grfc_out_type;

/*----------------------------------------------------------------------------*/
/*!
   @brief 
   list of grfc_out pins that is driven by grfc engines or control signal

   
   @details 
   The index will be the name of the grfc_out pin.  The value of
   1 = connected, 0 = not connected
*/
typedef uint8 rfd_grfc_connected_list_type[GRFC_OUT_MAX];

extern rfd_internal_grfc_engine_rf_enum_type rfd_grfc_engine_to_control_signal_mapping[RF_CTL_SIG_MAX];

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
void rfd_grfc_stall_bypass_register( boolean flag );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* RFD_GRFC_H */
