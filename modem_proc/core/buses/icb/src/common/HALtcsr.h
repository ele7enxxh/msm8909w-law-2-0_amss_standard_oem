#ifndef HALTCSR_H
#define HALTCSR_H
/**

@file   HALtcsr.h

@brief  This module provides the Application Programing Interface (API) 
        to the TCSR HAL layer.
*/
/*
===========================================================================

FILE:         HALtcsr.h

DESCRIPTION:  Header file for the HAL TCSR interface.

===========================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/icb/src/common/HALtcsr.h#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2014/03/12   sds     Change the hardware base address to a uint8_t*.
2014/02/07   sds     Created 

===========================================================================
                  Copyright (c) 2014 Qualcomm Technologies Incorporated.
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

/** TCSR device info structure */
typedef struct
{
  char *   pszBaseName;    /**< The HW name of the block to map. */
  uint8_t *pBaseAddr;      /**< Base address to use. */
  uint32_t uRegionSize;    /**< The total size of the block in bytes. */
  uint32_t uNumPorts;      /**< The total number of haltable ports. */
  uint32_t uHaltOffset;    /**< Halt block start from base. */
  uint32_t uHaltBlockSize; /**< The size of each halt block. */
  uint32_t uTimeoutCntUs;  /**< Timeout in us per wait cycle. */
  uint32_t uTimeoutCnt;    /**< Total number of times to wait. */
} HAL_tcsr_InfoType;

/* -----------------------------------------------------------------------
** Mandatory Interface
** ----------------------------------------------------------------------- */
/** @name Mandatory Interface */
/** @{ */

/** 
@brief Initialization function.

@param[inout] pInfo - A pointer to the TCSR info structure

@remarks The base address and non-hardware derived information must
         be prepopulated in the info structure.
*/
void HAL_tcsr_Init( HAL_tcsr_InfoType * pInfo );

/**
@brief This function resets the selected TCSR hardware to a default state.

@param[in] pInfo - A pointer to the TCSR info structure

@pre HAL_tcsr_Init() must have been called.

@sa HAL_tcsr_Init()
*/
void HAL_tcsr_Reset( HAL_tcsr_InfoType * pInfo ) ;

/**
@brief Save the current TCSR state.

@param[in] pInfo - A pointer to the TCSR info structure

@pre HAL_tcsr_Init() must have been called.

@sa HAL_tcsr_Init()
@sa HAL_tcsr_Restore()
*/
void HAL_tcsr_Save( HAL_tcsr_InfoType * pInfo );

/**
@brief Restore an TCSR to a previously saved state.

@param[in] pInfo - A pointer to the TCSR info structure

@pre HAL_tcsr_Init() and HAL_tcsr_Save() must have been called.
@pre HAL_tcsr_Save() must be called for each restore required.

@sa HAL_tcsr_Init()
@sa HAL_tcsr_Save()
*/
void HAL_tcsr_Restore( HAL_tcsr_InfoType * pInfo );
/** @} */

/* -----------------------------------------------------------------------
** Configuration Interface
** ----------------------------------------------------------------------- */
/** @name Configuration Interface */
/** @{ */

/**
@brief Halt the given port.

@param[in] pInfo - A pointer to the TCSR info structure
@param[in] uPort - The port number of the port

@pre HAL_tcsr_Init() must have been called.

@returns true if successful, false otherwise

@sa HAL_tcsr_Init()
@sa HAL_tcsr_UnhaltPort()
*/
bool HAL_tcsr_HaltPort( HAL_tcsr_InfoType *  pInfo,
                        uint32_t             uPort );

/**
@brief Unhalt the given port.

@param[in] pInfo - A pointer to the TCSR info structure
@param[in] uPort - The port number of the port

@pre HAL_tcsr_Init() must have been called.

@returns true if successful, false otherwise

@sa HAL_tcsr_Init()
@sa HAL_tcsr_HaltPort()
*/
bool HAL_tcsr_UnhaltPort( HAL_tcsr_InfoType *  pInfo,
                          uint32_t             uPort );
                         

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

#endif /* #ifndef HALTCSR_H */

