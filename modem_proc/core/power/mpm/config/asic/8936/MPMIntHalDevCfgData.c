/*===========================================================================
   FILE:         HALmpmintPlatform.c
 
   DESCRIPTION:   This is the platform hardware abstraction layer implementation for the
                  MPM interrupt controller block.
                  This platform is for the RPM on 8916.

                 Copyright © 2012-2013 QUALCOMM Technologies Incorporated.
                 All Rights Reserved.
                 QUALCOMM Confidential and Proprietary.

  =============================================================================
  =============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/config/asic/8936/MPMIntHalDevCfgData.c#1 $
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
  HAL_MPMINT_DEFINE_TABLE_ENTRY( HIGH,    HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                             */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                             */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                             */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  1                       ), /* HAL_MPMINT_GPIO1_ISR                        */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  4                       ), /* HAL_MPMINT_GPIO4_ISR                        */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  5                       ), /* HAL_MPMINT_GPIO5_ISR                        */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  9                       ), /* HAL_MPMINT_GPIO9_ISR                        */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  13                      ), /* HAL_MPMINT_GPIO13_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  17                      ), /* HAL_MPMINT_GPIO17_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  21                      ), /* HAL_MPMINT_GPIO21_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  27                      ), /* HAL_MPMINT_GPIO27_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  28                      ), /* HAL_MPMINT_GPIO29_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  31                      ), /* HAL_MPMINT_GPIO31_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  33                      ), /* HAL_MPMINT_GPIO33_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  35                      ), /* HAL_MPMINT_GPIO35_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  37                      ), /* HAL_MPMINT_GPIO37_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  38                      ), /* HAL_MPMINT_GPIO38_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  49                      ), /* HAL_MPMINT_GPIO49_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  50                      ), /* HAL_MPMINT_GPIO50_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  51                      ), /* HAL_MPMINT_GPIO51_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  52                      ), /* HAL_MPMINT_GPIO52_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  54                      ), /* HAL_MPMINT_GPIO54_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  62                      ), /* HAL_MPMINT_GPIO62_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  63                      ), /* HAL_MPMINT_GPIO63_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  64                      ), /* HAL_MPMINT_GPIO64_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  65                      ), /* HAL_MPMINT_GPIO65_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  66                      ), /* HAL_MPMINT_GPIO66_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  67                      ), /* HAL_MPMINT_GPIO67_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  68                      ), /* HAL_MPMINT_GPIO68_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  69                      ), /* HAL_MPMINT_GPIO69_ISR                       */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  107                     ), /* HAL_MPMINT_GPIO107_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  108                     ), /* HAL_MPMINT_GPIO108_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  109                     ), /* HAL_MPMINT_GPIO109_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  110                     ), /* HAL_MPMINT_GPIO110_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  111                     ), /* HAL_MPMINT_GPIO111_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  112                     ), /* HAL_MPMINT_GPIO112_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  113                     ), /* HAL_MPMINT_GPIO113_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  114                     ), /* HAL_MPMINT_GPIO114_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  115                     ), /* HAL_MPMINT_GPIO115_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  116                     ), /* HAL_MPMINT_GPIO116_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  117                     ), /* HAL_MPMINT_GPIO117_ISR                      */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                             */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_SDC1_DATA1_ISR                   */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_SDC1_DATA3_ISR                   */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_SDC2_DATA1_ISR                   */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_SDC2_DATA3_ISR                   */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_SRST_N                           */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_USB2_PHY_OTGSESSVLD_VDDPX3_0_ISR */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_USB2_PHY_ID_VDDPX3_0_ISR         */ 
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_USB2_PHY_VDDPX3_0_ISR            */ 
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                             */ 
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                             */ 
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                             */ 
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                             */ 
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                             */ 
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                             */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                             */ 
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                             */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_USB2_PHY_DMSE_0_ISR              */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* RESERVED/UNUSED                             */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_UIM_CARD_ISR                     */ 
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_UIM_BATT_ISR                     */ 
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ), /* HAL_MPMINT_SPMI_ISR                         */ 
   
  /*64 - special entry to mark end of the table */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( INVALID, HAL_MPMINT_INVALID_GPIO )
};           
