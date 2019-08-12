#ifndef CLOCKCONFIG_H
#define CLOCKCONFIG_H
/*
===========================================================================
*/
/**
  @file ClockSBLConfig.h

  Internal header file for the SBL configuration data structures.
*/
/*
  ====================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/clock/hw/msm8909w/src/ClockSBLConfig.h#1 $
  $DateTime: 2015/10/01 02:54:06 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     -------------------------------------------------

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockBoot.h"
#include "ClockSBLCommon.h"
#include "DDIChipInfo.h"


/*=========================================================================
      References
==========================================================================*/

/*=========================================================================
      Definitions
==========================================================================*/


/*=========================================================================
      Type Definitions
==========================================================================*/

/* Data structure for SBL configuration data */
typedef struct
{
  /* PLL configurations */
  ClockConfigPLLType PLL0_Cfg;
  ClockConfigPLLType PLL1_Cfg;
  ClockConfigPLLType PLL2_Cfg;
  ClockConfigPLLType BIMCPLL_Cfg;
  ClockConfigPLLType A7PLL_Cfg;

  /* Configurations for CPU */
  ClockAPCSCfgType CPU_Cfg[CLOCK_BOOT_PERF_NUM];

  /* System NOC config data */
  ClockConfigMuxType SNOC_Cfg[CLOCK_BOOT_PERF_NUM];

  /* PCNOC config data */
  ClockConfigMuxType PCNOC_Cfg[CLOCK_BOOT_PERF_NUM];

  /* SDC table (for backwards compatibility) */
  uint32 SDC_Cfg[CLOCK_BOOT_PERF_NUM];
  /* SDC1,SDC2 extended configurations */
  ClockSDCCfgType SDC1_Ext_Cfg[6];
  ClockSDCCfgType SDC2_Ext_Cfg[6];

  /* Crypto clock config */
  ClockConfigMuxType CE_Cfg;

  /* USB clock config */
  ClockConfigMuxType USB_Cfg;

  /* UART clock config */
  ClockConfigMuxType UART_Cfg[CLOCK_BOOT_PERF_NUM];
  
  /*RPM clock config*/
  ClockConfigMuxType RPM_Cfg;

  /* I2C clock config */
  ClockConfigMuxType I2C_Cfg[CLOCK_BOOT_PERF_NUM];

} Clock_SBLConfigType;

extern Clock_SBLConfigType *Clock_SBLConfig( void );

extern boolean Clock_EnableSource( ClockSourceType eSource );
extern boolean Clock_ConfigureSource( ClockSourceType eSource );

boolean Clock_SourceMapToMux
(
  const ClockConfigMuxType *pConfig,
  uint32 *nMuxValue
);

boolean Clock_MuxMapToSource
(
  ClockConfigMuxType *pConfig,
  uint32 nSource
);

typedef struct ClockPlatformInfo 
{
  DalChipInfoIdType       chip_id;
} ClockPlatformInfoType;


#endif /* !CLOCKCONFIG_H */

