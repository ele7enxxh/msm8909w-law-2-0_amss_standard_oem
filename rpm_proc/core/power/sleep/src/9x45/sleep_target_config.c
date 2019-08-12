/*============================================================================
  FILE:         sleep_target_config.c

  OVERVIEW:     This file provides target-specific functionality for the RPM.

  DEPENDENCIES: None

                Copyright (c) 2011 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

  $Header: //components/rel/rpm.bf/2.1.1/core/power/sleep/src/9x45/sleep_target_config.c#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $
============================================================================*/

#include "msmhwio.h"
#include "msmhwiobase.h"
#include "qfprom_pte_hwio.h"

// retention programmed in uV ( 600000uV = 0.6V )
static const uint32 vddcx_pvs_retention_data[8] =
{
  /* 000 */ 700000,
  /* 001 */ 550000,
  /* 010 */ 500000,
  /* 011 */ 450000,
  /* 100 */ 787500, // rounded 780000
  /* 101 */ 787500, // rounded 780000
  /* 110 */ 787500, // rounded 780000
  /* 111 */ 787500, // rounded 780000
};

// retention programmed in uV ( 600000uV = 0.6V )
static const uint32 vddmx_pvs_retention_data[8] =
{
  /* 000 */ 850000,
  /* 001 */ 700000,
  /* 010 */ 650000,
  /* 011 */ 600000,
  /* 100 */ 987500, // rounded 980000
  /* 101 */ 987500, // rounded 980000
  /* 110 */ 987500, // rounded 980000
  /* 111 */ 987500, // rounded 980000
};

void set_vdd_dig_sleep_voltage (uint32 *vdd_dig)
{
  // VDDCX rentation fuse - 0x6005C00C[7:5]
  uint32 pte_masked_val = HWIO_INF(QFPROM_CORR_PTE2, LOGIC_RETENTION);
  *vdd_dig = vddcx_pvs_retention_data[pte_masked_val];
}

void set_vdd_mem_sleep_voltage (uint32 *vdd_mem)
{
  // VDDMX rentation fuse - 0x6005C00C[31:29]
  #define QFPROM_CORR_PTE2_VDDMX_MASK (0xE0000000)
  #define QFPROM_CORR_PTE2_VDDMX_SHFT (0x1D)
  uint32 pte_masked_val = ((HWIO_INM(QFPROM_CORR_PTE2, QFPROM_CORR_PTE2_VDDMX_MASK)) >> QFPROM_CORR_PTE2_VDDMX_SHFT);
  *vdd_mem = vddmx_pvs_retention_data[pte_masked_val];
}

