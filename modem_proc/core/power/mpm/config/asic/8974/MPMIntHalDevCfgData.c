/*==========================================================================

  FILE          MPMIntHalDevCfgData.c

  DESCRIPTION   This file contains the table that stores configuration 
                information about mpm mapped interrupt/gpios. MPM driver
                retrieves the index for this table from another mapping 
                table.

                Copyright © 2012-2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                QUALCOMM Proprietary/GTDR

  $Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/config/asic/8974/MPMIntHalDevCfgData.c#1 $
===========================================================================*/

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
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  102                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,    1                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,    5                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,    9                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   18                     ),
  /*8*/
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   20                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   24                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   27                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   28                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   34                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   35                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   37                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   42                     ),
  /*16*/
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   44                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   46                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   50                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   54                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   59                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   61                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   62                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   64                     ),
  /*24*/
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   65                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   66                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   67                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   68                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   71                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   72                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   73                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   74                     ),
  /*32*/
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   75                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   77                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   79                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   80                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   82                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   86                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   92                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   93                     ),
  /*40*/
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,   95                     ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ),
  /*48*/
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( RISING,  HAL_MPMINT_INVALID_GPIO ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( HIGH,    HAL_MPMINT_INVALID_GPIO ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( HIGH,    HAL_MPMINT_INVALID_GPIO ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( HIGH,    HAL_MPMINT_INVALID_GPIO ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( HIGH,    HAL_MPMINT_INVALID_GPIO ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( HIGH,    HAL_MPMINT_INVALID_GPIO ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( HIGH,    HAL_MPMINT_INVALID_GPIO ),
  /*56*/
  HAL_MPMINT_DEFINE_TABLE_ENTRY( HIGH,    HAL_MPMINT_INVALID_GPIO ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( HIGH,    HAL_MPMINT_INVALID_GPIO ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( HIGH,    HAL_MPMINT_INVALID_GPIO ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( HIGH,    HAL_MPMINT_INVALID_GPIO ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( HIGH,    HAL_MPMINT_INVALID_GPIO ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( HIGH,    HAL_MPMINT_INVALID_GPIO ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( HIGH,    HAL_MPMINT_INVALID_GPIO ),
  HAL_MPMINT_DEFINE_TABLE_ENTRY( HIGH,    HAL_MPMINT_INVALID_GPIO ),
  /*64 - special entry to mark end of the table */
  HAL_MPMINT_DEFINE_TABLE_ENTRY( INVALID, HAL_MPMINT_INVALID_GPIO )
};           

