/*============================================================================
  FILE:         sleep_target_config.c

  OVERVIEW:     This file provides target-specific functionality for the RPM.

  DEPENDENCIES: None

                Copyright (c) 2011 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

  $Header: //components/rel/rpm.bf/2.1.1/core/power/sleep/src/8916/sleep_target_config.c#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $
============================================================================*/


#include "msmhwio.h"
#include "msmhwiobase.h"
#include "qfprom_pte_hwio.h"

// retention programmed in uV ( 600000uV = 0.6V )
static const uint32 vddcx_pvs_retention_data[8] =
{
  /* 000 */ 650000,
  /* 001 */ 500000, 
  /* 010 */ 650000, 
  /* 011 */ 650000, 
  /* 100 */ 650000, 
  /* 101 */ 650000, 
  /* 110 */ 650000,
  /* 111 */ 650000
};

// retention programmed in uV ( 600000uV = 0.6V )
static const uint32 vddmx_pvs_retention_data[4] =
{
  /* 00 */ 750000,
  /* 01 */ 650000, 
  /* 10 */ 750000, 
  /* 11 */ 750000, 
};

void set_vdd_mem_sleep_voltage (uint32 *vdd_mem)
{

  /* 3 MSB bits contain the values we need */
  uint32 pte_masked_val = HWIO_INF(QFPROM_CORR_PTE2, MX_RET_BIN);
  *vdd_mem = vddmx_pvs_retention_data[pte_masked_val];
}
void set_vdd_dig_sleep_voltage (uint32 *vdd_dig)
{
  uint32 pte_masked_val; 	

  /* 3 MSB bits contain the values we need */
  pte_masked_val = HWIO_INF(QFPROM_CORR_PTE2, LOGIC_RETENTION);
  *vdd_dig = vddcx_pvs_retention_data[pte_masked_val];
}


