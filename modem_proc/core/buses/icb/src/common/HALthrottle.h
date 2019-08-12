#ifndef HALTHROTTLE_H
#define HALTHROTTLE_H
/**

@file   HALthrottle.h

@brief  This module provides the Application Programing Interface (API) 
        to the THROTTLE HAL layer.
*/
/*
===========================================================================

FILE:         HALthrottle.h

DESCRIPTION:  Header file for the HAL THROTTLE interface.

===========================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/icb/src/common/HALthrottle.h#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2015/05/02   sds     Created 

===========================================================================
            Copyright (c) 2015 QUALCOMM Technologies Incorporated.
                           All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===========================================================================
*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/** THROTTLE device info structure */
typedef struct
{
  char *   pszBaseName; /**< The HW name of the block to map. */
  uint8_t *pBaseAddr;   /**< Base address to use. */
  uint32_t uRegionSize; /**< The total size of the block in bytes. */
  uint32_t uNumPorts;   /**< The number of ports for this throttle. */
} HAL_throttle_InfoType;

/* -----------------------------------------------------------------------
** Mandatory Interface
** ----------------------------------------------------------------------- */
/** @name Mandatory Interface */
/** @{ */

/** 
@brief Initialization function.

@param[inout] pInfo - A pointer to the THROTTLE info structure

@remarks The base address and non-hardware derived information must
         be prepopulated in the info structure.
*/
void HAL_throttle_Init( HAL_throttle_InfoType * pInfo );

/**
@brief This function resets the selected THROTTLE hardware to a default state.

@param[in] pInfo - A pointer to the THROTTLE info structure

@pre HAL_throttle_Init() must have been called.

@sa HAL_throttle_Init()
*/
void HAL_throttle_Reset( HAL_throttle_InfoType * pInfo ) ;

/**
@brief Save the current THROTTLE state.

@param[in] pInfo - A pointer to the THROTTLE info structure

@pre HAL_throttle_Init() must have been called.

@sa HAL_throttle_Init()
@sa HAL_throttle_Restore()
*/
void HAL_throttle_Save( HAL_throttle_InfoType * pInfo );

/**
@brief Restore an THROTTLE to a previously saved state.

@param[in] pInfo - A pointer to the THROTTLE info structure

@pre HAL_throttle_Init() and HAL_throttle_Save() must have been called.
@pre HAL_throttle_Save() must be called for each restore required.

@sa HAL_throttle_Init()
@sa HAL_throttle_Save()
*/
void HAL_throttle_Restore( HAL_throttle_InfoType * pInfo );
/** @} */

/* -----------------------------------------------------------------------
** Configuration Interface
** ----------------------------------------------------------------------- */
/** @name Configuration Interface */
/** @{ */

/**
@brief Set the bandwidth limit for a given throttle level

@param[in] pInfo -  A pointer to the THROTTLE info structure
@param[in] uPort -  The port of the throttle to adjust
@param[in] uLevel - The throttle level to adjust
@param[in] uBW -    The bandwidth limit to use

@pre HAL_throttle_Init() must have been called.

@sa HAL_throttle_Init()
*/
void HAL_throttle_SetBW( HAL_throttle_InfoType *pInfo,
                         uint32_t               uPort,
                         uint32_t               uLevel,
                         uint64_t               uBW );

/** @} */

/* -----------------------------------------------------------------------
** Data Interface
** ----------------------------------------------------------------------- */
/** @name Data Interface */
/** @{ */

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* #ifndef HALTHROTTLE_H */

