#ifndef BSP_MPM_H
#define BSP_MPM_H
/*
===========================================================================

FILE:         HALgpiointInternal.h

DESCRIPTION:  
  This is the hardware abstraction layer internal interface for the MPM
  interrupt controller block.

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/mpm/hal/bsp/include/BSPmpm.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
04/09/12   mda     MPMv2 for 8974

===========================================================================
             Copyright © 2008-2009 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#ifdef __cplusplus
extern "C" {
#endif


/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */
#include "HALmpm.h"
#include "HALcomdef.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

typedef struct
{
  const uint32_t cxoDelay;
  const uint32_t deepSleepExitDelay;
} BSP_mpm_WakeupDelaysType;

typedef struct
{
  bool32 freezeIOs;
  bool32 clampIOs;
  bool32 swEbi1CtlEnable;
  bool32 wbootFreezeEBI1;
  bool32 wbootFreezeEBI2;
} BSP_mpm_IOCntlCfgType;

typedef struct
{
  BSP_mpm_WakeupDelaysType   wakeupDelays;
  BSP_mpm_IOCntlCfgType      ioCfg;
  bool32                     dbgThroughPCEnable;
  bool32                     dbgBusEnable;
  bool32                     dbgBusEnable_1;
} BSP_mpm_MpmCfgType;

typedef struct
{
  bool32                       enable;
  HAL_mpm_PenDebounceDelayType delay;
} BSP_mpm_PenDebounceCfgType;

typedef struct
{
  BSP_mpm_PenDebounceCfgType penDebounceCfg;
} BSP_mpm_PeriCfgType;

typedef struct
{
  uint16_t digAddress;
  uint16_t memAddress;
  uint16_t digID;
  uint16_t memID;
  uint32_t vregStepSize;
} BSP_mpm_VregCfgType;

typedef struct
{
  BSP_mpm_MpmCfgType  mpmCfg;
  BSP_mpm_PeriCfgType periCfg;
  BSP_mpm_VregCfgType vregCfg;
} BSP_mpm_ConfigDataType;

#define HAL_mpm_CfgType BSP_mpm_ConfigDataType 

/* BSP MPM Version of the data file to be used by SW and BSP to access the 
   data */
#define HAL_MPM_BSP_VERSION  \
                    BSP_ENCODE_VERSION(2, 0, 0)  /* major, minor, build */

/* -----------------------------------------------------------------------
**                           GLOBAL DATA
** ----------------------------------------------------------------------- */

extern BSP_mpm_ConfigDataType MPM_BSP_DATA;

#ifdef __cplusplus
}
#endif

#endif /* BSP_MPM_H */


