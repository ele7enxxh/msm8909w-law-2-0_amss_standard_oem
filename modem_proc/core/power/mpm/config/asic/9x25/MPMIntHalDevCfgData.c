/*===========================================================================
   FILE:         HALmpmintPlatform.c
 
   DESCRIPTION:   This is the platform hardware abstraction layer implementation for the
                  MPM interrupt controller block.
                  This platform is for the RPM on 9x25.

                 Copyright © 2012-2013 QUALCOMM Technologies Incorporated.
                 All Rights Reserved.
                 QUALCOMM Confidential and Proprietary.

  =============================================================================
  =============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/config/asic/9x25/MPMIntHalDevCfgData.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $
========================================================================*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "HALmpmint.h"
#include "HALmpmintInternal.h"

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

/*
 * Target-specific interrupt configurations
 */
HAL_mpmint_PlatformIntType devcfgMpmIntHalTable[] = 
{
  /*                             Trigger            GPIO          */
  /*                             -------  ----------------------- */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( HIGH,    HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                            */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                            */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                            */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                            */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  0                       ), /* HAL_MPMINT_GPIO0_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  1                       ), /* HAL_MPMINT_GPIO1_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  2                       ), /* HAL_MPMINT_GPIO2_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  3                       ), /* HAL_MPMINT_GPIO3_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  4                       ), /* HAL_MPMINT_GPIO4_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  5                       ), /* HAL_MPMINT_GPIO5_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  6                       ), /* HAL_MPMINT_GPIO6_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  7                       ), /* HAL_MPMINT_GPIO7_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  8                       ), /* HAL_MPMINT_GPIO8_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  9                       ), /* HAL_MPMINT_GPIO9_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  10                      ), /* HAL_MPMINT_GPIO10_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  11                      ), /* HAL_MPMINT_GPIO11_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  12                      ), /* HAL_MPMINT_GPIO12_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  13                      ), /* HAL_MPMINT_GPIO13_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  14                      ), /* HAL_MPMINT_GPIO14_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  15                      ), /* HAL_MPMINT_GPIO15_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  16                      ), /* HAL_MPMINT_GPIO16_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  17                      ), /* HAL_MPMINT_GPIO17_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  18                      ), /* HAL_MPMINT_GPIO18_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  19                      ), /* HAL_MPMINT_GPIO19_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  20                      ), /* HAL_MPMINT_GPIO20_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  21                      ), /* HAL_MPMINT_GPIO21_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  24                      ), /* HAL_MPMINT_GPIO24_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  25                      ), /* HAL_MPMINT_GPIO25_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  51                      ), /* HAL_MPMINT_GPIO51_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  61                      ), /* HAL_MPMINT_GPIO61_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  62                      ), /* HAL_MPMINT_GPIO62_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  63                      ), /* HAL_MPMINT_GPIO63_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  64                      ), /* HAL_MPMINT_GPIO64_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  65                      ), /* HAL_MPMINT_GPIO65_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  66                      ), /* HAL_MPMINT_GPIO66_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  67                      ), /* HAL_MPMINT_GPIO67_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  69                      ), /* HAL_MPMINT_GPIO69_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  71                      ), /* HAL_MPMINT_GPIO71_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                            */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                            */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                            */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_HSIC_STROBE_ISR                 */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                            */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                            */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_SDC1_DATA1_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_SDC1_DATA3_ISR                  */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_SRST_N_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_USB2_PHY_OTG_VDDPX3_0_ISR       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_USB2_PHY_ID_VDDPX3_0_ISR        */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_USB2_PHY_DPSE_0_ISR             */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                            */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                            */ 
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                            */ 
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                            */ 
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                            */ 
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                            */ 
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                            */ 
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                            */ 
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_USB2_PHY_DMSE_0_ISR             */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                            */ 
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_UIM_CARD_ISR                    */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_UIM_BATT_ISR                    */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_SPMI_ISR                        */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                            */ 

  /*64 - special entry to mark end of the table */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( INVALID, HAL_MPMINT_INVALID_GPIO )
};           
