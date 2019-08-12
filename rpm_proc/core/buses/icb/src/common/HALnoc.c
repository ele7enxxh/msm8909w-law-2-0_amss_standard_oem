/**

@file   HALnoc.c

@brief  Base file for HAL NOC.
        Intended to be used to check core revision, and call the correct
        implementation - mostly empty now as there is only one core
        revision.
        
===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/buses/icb/src/common/HALnoc.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
08/15/11   sds     Created 

===========================================================================
                  Copyright (c) 2011 Qualcomm Technologies Incorporated.
                           All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include "HALnoci.h"

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */

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
@copydoc HAL_noc_Init
*/
void HAL_noc_Init
(
  HAL_noc_InfoType *pInfo
)
{
  NOC_VERIFY_PTR( pInfo );

  noc_Init( pInfo );
}

/**
@copydoc HAL_noc_Reset
*/
void HAL_noc_Reset
(
  HAL_noc_InfoType *pInfo
)
{
  NOC_VERIFY_PTR( pInfo );

  noc_Reset( pInfo );
}

/**
@copydoc HAL_noc_Save
*/
void HAL_noc_Save
(
  HAL_noc_InfoType *pInfo
)
{
  NOC_VERIFY_PTR( pInfo );

  noc_Save( pInfo );
}

/**
@copydoc HAL_noc_Restore
*/
void HAL_noc_Restore
(
  HAL_noc_InfoType *pInfo
)
{
  NOC_VERIFY_PTR( pInfo );

  noc_Restore( pInfo );
}

/* -----------------------------------------------------------------------
** Configuration Interface
** ----------------------------------------------------------------------- */
/**
@copydoc HAL_noc_SetQosMode()
*/
void HAL_noc_SetQosMode
(
  HAL_noc_InfoType *  pInfo,
  uint32_t            uMasterPort,
  HAL_noc_QosModeType eMode
)
{
  NOC_VERIFY_PTR( pInfo );
  NOC_VERIFY_QOS_MASTER_IDX( pInfo, uMasterPort );

  noc_SetQosMode( pInfo, uMasterPort, eMode );
}

/**
@copydoc HAL_noc_SetQosPriority()
*/
void HAL_noc_SetQosPriority
(
  HAL_noc_InfoType *       pInfo,
  uint32_t                 uMasterPort,
  HAL_noc_QosModeType      eMode,
  HAL_noc_QosPriorityType *pPriority
)
{
  NOC_VERIFY_PTR( pInfo );
  NOC_VERIFY_QOS_MASTER_IDX( pInfo, uMasterPort );
  NOC_VERIFY_PTR( pPriority );

  noc_SetQosPriority( pInfo, uMasterPort, eMode, pPriority );
}

/**
@copydoc HAL_noc_SetQosBandwidth()
*/
void HAL_noc_SetQosBandwidth
(
  HAL_noc_InfoType *        pInfo,
  uint32_t                  uMasterPort,
  HAL_noc_QosBandwidthType *pBandwidth
)
{
  NOC_VERIFY_PTR( pInfo );
  NOC_VERIFY_QOS_MASTER_IDX( pInfo, uMasterPort );
  NOC_VERIFY_PTR( pBandwidth );

  noc_SetQosBandwidth( pInfo, uMasterPort, pBandwidth );
}

/* -----------------------------------------------------------------------
** Data Interface
** ----------------------------------------------------------------------- */
/**
@copydoc HAL_noc_GetQosMode()
*/
void HAL_noc_GetQosMode
(
  HAL_noc_InfoType *   pInfo,
  uint32_t             uMasterPort,
  HAL_noc_QosModeType *pMode
)
{
  NOC_VERIFY_PTR( pInfo );
  NOC_VERIFY_QOS_MASTER_IDX( pInfo, uMasterPort );

  noc_GetQosMode( pInfo, uMasterPort, pMode );
}

/**
@copydoc HAL_noc_GetQosPriority()
*/
void HAL_noc_GetQosPriority
(
  HAL_noc_InfoType *       pInfo,
  uint32_t                 uMasterPort,
  HAL_noc_QosPriorityType *pPriority
)
{
  NOC_VERIFY_PTR( pInfo );
  NOC_VERIFY_QOS_MASTER_IDX( pInfo, uMasterPort );
  NOC_VERIFY_PTR( pPriority );

  noc_GetQosPriority( pInfo, uMasterPort, pPriority );
}

/**
@copydoc HAL_noc_GetQosBandwidth()
*/
void HAL_noc_GetQosBandwidth
(
  HAL_noc_InfoType *        pInfo,
  uint32_t                  uMasterPort,
  HAL_noc_QosBandwidthType *pBandwidth
)
{
  NOC_VERIFY_PTR( pInfo );
  NOC_VERIFY_QOS_MASTER_IDX( pInfo, uMasterPort );
  NOC_VERIFY_PTR( pBandwidth );

  noc_GetQosBandwidth( pInfo, uMasterPort, pBandwidth );
}

