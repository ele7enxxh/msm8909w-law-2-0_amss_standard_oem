/*============================================================================
  FILE:         sleep_target_config.c

  OVERVIEW:     This file provides target-specific functionality for the RPM.

  DEPENDENCIES: None

                Copyright (c) 2011 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

  $Header: //components/rel/rpm.bf/2.1.1/core/power/sleep/src/8976/sleep_target_config.c#1 $
  $DateTime: 2015/02/25 02:53:43 $
  $Author: pwbldsvc $
============================================================================*/


#include "msmhwio.h"
#include "msmhwiobase.h"
#include "qfprom_pte_hwio.h"

// retention programmed in uV ( 600000uV = 0.6V )
static const uint32 vddcx_pvs_retention_data[8] =
{
	//TODO
  /* 000 */ 700000,
  /* 001 */ 700000, 
  /* 010 */ 700000, 
  /* 011 */ 700000, 
  /* 100 */ 700000, 
  /* 101 */ 700000, 
  /* 110 */ 700000,
  /* 111 */ 700000
};

// retention programmed in uV ( 600000uV = 0.6V )
static const uint32 vddmx_pvs_retention_data[4] =
{
  /* 00 */ 750000,
  /* 01 */ 700000, 
  /* 10 */ 650000, 
  /* 11 */ 600000, 
};

void set_vdd_mem_sleep_voltage (uint32 *vdd_mem)
{

  /* 3 MSB bits contain the values we need */
  uint32 pte_masked_val = HWIO_INF(QFPROM_CORR_PTE2, MX_RET_BIN);
//  uint32 lookup_val = pte_masked_val;
  *vdd_mem = vddmx_pvs_retention_data[pte_masked_val];
}
void set_vdd_dig_sleep_voltage (uint32 *vdd_dig)
{
  uint32 pte_masked_val; 	
  *vdd_dig = 700000;

  /* 3 MSB bits contain the values we need */
  pte_masked_val = HWIO_INF(QFPROM_CORR_PTE2, LOGIC_RETENTION);
//  uint32 lookup_val = pte_masked_val >> 5;
  *vdd_dig = vddcx_pvs_retention_data[pte_masked_val];
}


