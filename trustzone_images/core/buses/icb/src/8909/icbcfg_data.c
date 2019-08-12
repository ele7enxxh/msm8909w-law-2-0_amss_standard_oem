/*==============================================================================

FILE:      icbcfg_data.c

DESCRIPTION: This file implements the ICB Configuration driver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/icb/src/8909/icbcfg_data.c#1 $ 
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $
$Change: 10727476 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
05/30/14     ms	 Created

 
        Copyright (c) 2014 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "icbcfg.h"
#include "icbcfgi.h"
#include "icbcfg_hwio.h"
#include "HALbimc.h"
#include "HALbimcHwioGeneric.h"

/*---------------------------------------------------------------------------*/
/*          Macro and constant definitions                                   */
/*---------------------------------------------------------------------------*/
/* BIMC register value macros */
#define SLAVE_SEGMENT(slave,index,addr_base,addr_mask)  \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_SEGMENTn_ADDR_BASEm_LOWER_ADDR(BIMC_BASE,slave,index), \
   BIMC_SEGMENTn_ADDR_BASEm_LOWER_RMSK, \
   BIMC_SEGMENTn_ADDR_BASEm_LOWER_RMSK, \
   (addr_base) }, \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_SEGMENTn_ADDR_MASKm_LOWER_ADDR(BIMC_BASE,slave,index), \
   BIMC_SEGMENTn_ADDR_MASKm_LOWER_RMSK, \
   BIMC_SEGMENTn_ADDR_MASKm_LOWER_RMSK, \
   (addr_mask) }

#define ARB_MODE(slave,mode) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_S_ARB_MODE_ADDR(BIMC_BASE,slave), \
   BIMC_S_ARB_MODE_RMSK, \
   BIMC_S_ARB_MODE_RMSK, \
   (mode) }

/* Slave indexes */
#define SLAVE_DDR_CH0 0
#define SLAVE_APPS_L2 1
#define SLAVE_SNOC    2

#define MPORT_MODE(master,mode) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_MODE_ADDR(BIMC_BASE,master), \
   BIMC_M_MODE_RMSK, \
   BIMC_M_MODE_RMSK, \
   (mode) }

/* Master indexes */
#define MASTER_APP   0
#define MASTER_DSP   1
#define MASTER_GPU   2
#define MASTER_SYS0  3
#define MASTER_SYS1  4
#define MASTER_TCU0  5
#define MASTER_TCU1  6

/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/

/*---------------------------------------------------------------------------*/
/*          Properties data for device ID  = "icbcfg/boot"                   */
/*---------------------------------------------------------------------------*/

/* ICBcfg Boot Configuration Data*/

icbcfg_data_type icbcfg_boot_data[] = 
{
    /* Add configuration data using
      ICBCFG_HWIO_*() or
      ICBCFG_RAW_*() macros below
      .
      .                          
      .                          */
    /* Enable priority arbitration mode for DDR CH 0 */
    ARB_MODE(SLAVE_DDR_CH0, 0x10000001),
    /* Master port mode settings. */
    MPORT_MODE(MASTER_APP,  0x2011),
    MPORT_MODE(MASTER_DSP,  0x2011),
    MPORT_MODE(MASTER_GPU,  0x2011),
    MPORT_MODE(MASTER_SYS0, 0x2011),
    MPORT_MODE(MASTER_SYS1, 0x80002011),
    MPORT_MODE(MASTER_TCU0, 0x2011),
    MPORT_MODE(MASTER_TCU1, 0x2011),
};

icbcfg_prop_type icbcfg_boot_prop = 
{
    /* Length of the config  data array */
    sizeof(icbcfg_boot_data)/sizeof(icbcfg_data_type), 
    /* Pointer to config data array */ 
    icbcfg_boot_data                                    
};

/* DDR map information. */
uint32 map_ddr_region_count = 1; 
icbcfg_mem_region_type map_ddr_regions[1] =
{
  { 0x80000000ULL, 0x100000000ULL },
};

uint32 channel_map[1] = { SLAVE_DDR_CH0 };

HAL_bimc_InfoType bimc_hal_info =
{
  (uint8_t *)BIMC_BASE, /* Base address */
  19200,     /* QoS frequency */
};

/* Make sure the config region is always prohibited when "resetting" */
HAL_bimc_SlaveSegmentType safe_reset_seg =
{
  true,
  0x00000000ULL,                 /* start of config region */
  0x80000000ULL,                 /* 2 GB */
  BIMC_SEGMENT_TYPE_SUBTRACTIVE,
  BIMC_INTERLEAVE_NONE,
};

/*---------------------------------------------------------------------------*/
/*          Properties data for new Device IDs: Add below                    */
/*---------------------------------------------------------------------------*/
