/**

@file   HALthrottle.c

@brief  Base file for HAL THROTTLE.
        
===========================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/icb/src/common/HALthrottle.c#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2015/05/02   sds     Created 

===========================================================================
            Copyright (c) 2015 QUALCOMM Technologies Incorporated.
                           All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include "HALthrottle.h"
#include "HALthrottleHwio.h"
#include "HALthrottleHwioGeneric.h"

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */
#define THROTTLE_VERIFY_PTR(ptr) if(NULL == ptr) return
#define THROTTLE_VERITY_PORT(info, port) if(port >= info->uNumPorts) return
#define THROTTLE_VERIFY_LVL(lvl) if(lvl == 0 || lvl > 7) return

#define THROTTLE_MAX_BW_MBPS 0xFFFF

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Data
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Functions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Mandatory Interface
** ----------------------------------------------------------------------- */
/** 
@copydoc HAL_throttle_Init
*/
void HAL_throttle_Init
(
  HAL_throttle_InfoType *pInfo
)
{
  THROTTLE_VERIFY_PTR( pInfo );
}

/**
@copydoc HAL_throttle_Reset
*/
void HAL_throttle_Reset
(
  HAL_throttle_InfoType *pInfo
)
{
  THROTTLE_VERIFY_PTR( pInfo );
}

/**
@copydoc HAL_throttle_Save
*/
void HAL_throttle_Save
(
  HAL_throttle_InfoType *pInfo
)
{
  THROTTLE_VERIFY_PTR( pInfo );
}

/**
@copydoc HAL_throttle_Restore
*/
void HAL_throttle_Restore
(
  HAL_throttle_InfoType *pInfo
)
{
  THROTTLE_VERIFY_PTR( pInfo );
}

/* -----------------------------------------------------------------------
** Configuration Interface
** ----------------------------------------------------------------------- */
/**
@copydoc HAL_throttle_SetBW()
*/
void HAL_throttle_SetBW( HAL_throttle_InfoType *pInfo,
                         uint32_t               uPort,
                         uint32_t               uLevel,
                         uint64_t               uBW )
{
  uint64_t uBWMBps;
  uint32_t uRegIdx;
  uint16_t uBWVal;

  THROTTLE_VERIFY_PTR( pInfo );
  THROTTLE_VERITY_PORT( pInfo, uPort );
  THROTTLE_VERIFY_LVL( uLevel );


  /* Calculate our limit in MB/s.
   * Make sure we don't exceed the ceiling in the hardware. */
  uBWMBps = uBW / 1000000;
  if( uBWMBps > THROTTLE_MAX_BW_MBPS )
  {
    uBWMBps = THROTTLE_MAX_BW_MBPS;
  }

  /* THROTTLE_MAX_BW_MBPS is < 16 bits, so this is always safe to do. */
  uBWVal = (uint16_t)uBWMBps;

  /* Figure out which register we need to write.
   * Level 7/6 are in threshold 0, while level 0/1 are in threshold 3, so invert our calculation.
   * There are two thresholds per register. */
  uRegIdx = 3 - (uLevel / 2);
  if( 0 == uLevel % 2 )
  {
    THROTTLE_OUTFI2(pInfo->pBaseAddr, THRESHOLD_i, uPort, uRegIdx, THRESHOLD_LEVEL_0, uBWVal);
  }
  else
  {
    THROTTLE_OUTFI2(pInfo->pBaseAddr, THRESHOLD_i, uPort, uRegIdx, THRESHOLD_LEVEL_1, uBWVal);
  }
}

/* -----------------------------------------------------------------------
** Data Interface
** ----------------------------------------------------------------------- */

