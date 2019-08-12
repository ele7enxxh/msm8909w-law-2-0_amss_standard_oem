/**

@file   HALtcsr.c

@brief  Base file for HAL TCSR.
        
===========================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/icb/src/common/HALtcsr.c#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2014/02/07   sds     Created 

===========================================================================
                  Copyright (c) 2014 Qualcomm Technologies Incorporated.
                           All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include "HALtcsr.h"
#include "HALtcsrHwio.h"
#include "HALtcsrHwioGeneric.h"
#include "busywait.h"

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */
#define TCSR_VERIFY_PTR(ptr) if(NULL == ptr) return
#define TCSR_VERIFY_PORT(info,port_id) if(info->uNumPorts <= port_id) return false

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
@copydoc HAL_tcsr_Init
*/
void HAL_tcsr_Init
(
  HAL_tcsr_InfoType *pInfo
)
{
  TCSR_VERIFY_PTR( pInfo );
}

/**
@copydoc HAL_tcsr_Reset
*/
void HAL_tcsr_Reset
(
  HAL_tcsr_InfoType *pInfo
)
{
  TCSR_VERIFY_PTR( pInfo );
}

/**
@copydoc HAL_tcsr_Save
*/
void HAL_tcsr_Save
(
  HAL_tcsr_InfoType *pInfo
)
{
  TCSR_VERIFY_PTR( pInfo );
}

/**
@copydoc HAL_tcsr_Restore
*/
void HAL_tcsr_Restore
(
  HAL_tcsr_InfoType *pInfo
)
{
  TCSR_VERIFY_PTR( pInfo );
}

/* -----------------------------------------------------------------------
** Configuration Interface
** ----------------------------------------------------------------------- */
/**
@copydoc HAL_tcsr_HaltPort()
*/
bool HAL_tcsr_HaltPort
(
  HAL_tcsr_InfoType *  pInfo,
  uint32_t             uPort
)
{
  uint32_t uWaitCnt;
  TCSR_VERIFY_PORT(pInfo,uPort);
  
  /* Issue the halt request */
  TCSR_OUTI(pInfo->pBaseAddr, pInfo->uHaltOffset, pInfo->uHaltBlockSize,
            HALT_REQn, uPort, TCSR_FMSK(HALT_REQn, HALTREQ));

  /* Wait for the corresponding ACK. */
  uWaitCnt = 0;
  while((uWaitCnt < pInfo->uTimeoutCnt) &&
        (TCSR_INFI(pInfo->pBaseAddr, pInfo->uHaltOffset, pInfo->uHaltBlockSize,
                   HALT_ACKn, uPort, HALTACK) == 0))
  {
    busywait(pInfo->uTimeoutCntUs);
    uWaitCnt++;
  }

  /* Check to see if the ACK was not received.
   * If so, stop asserting the request - it may not be able to respond.
   */
  if(uWaitCnt >= pInfo->uTimeoutCnt)
  {
    HAL_tcsr_UnhaltPort(pInfo, uPort);
  }

  return (uWaitCnt < pInfo->uTimeoutCnt);
}

/**
@copydoc HAL_tcsr_UnhaltPort()
*/
bool HAL_tcsr_UnhaltPort
(
  HAL_tcsr_InfoType *  pInfo,
  uint32_t             uPort
)
{
  TCSR_VERIFY_PORT(pInfo,uPort);

  TCSR_OUTI(pInfo->pBaseAddr, pInfo->uHaltOffset, pInfo->uHaltBlockSize,
            HALT_REQn, uPort, 0x0);

  return true;
}

/* -----------------------------------------------------------------------
** Data Interface
** ----------------------------------------------------------------------- */

