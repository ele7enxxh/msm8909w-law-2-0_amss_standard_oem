/*==============================================================================

FILE:      icbcfg_data.c

DESCRIPTION: This file implements the ICB Configuration driver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/icb/src/8956/icbcfg_data.c#1 $ 
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
$Change: 8935858 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2014/11/07  rc     Branched for 8952
2014/09/12  sds    Update danger context allocation
2014/06/19  sds    Update to v0.9 8936 spreadsheet
2014/05/20  sds    Change the hardware base address to a uint8_t*.
2014/05/20  sds    Allow 3GB memory maps
2014/01/27  sds    Branched for 8936
2013/12/11  sds    First pass at DDR settings spreadsheet
2013/10/09  sds    Branched for 8916
2013/02/26  sds    Changed how available DDR regions are handled.
2013/02/04  jc     Updated masters and slaves to match TO
2012/12/06  jc     8x10 implementation
2012/03/26  av     Created
 
        Copyright (c) 2012-2014 Qualcomm Technologies Incorporated.
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
   (void *)BIMC_SEGMENTn_ADDR_BASEm_LOWER_ADDR((uint8_t *)BIMC_BASE,slave,index), \
   BIMC_SEGMENTn_ADDR_BASEm_LOWER_RMSK, \
   BIMC_SEGMENTn_ADDR_BASEm_LOWER_RMSK, \
   (addr_base) }, \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_SEGMENTn_ADDR_MASKm_LOWER_ADDR((uint8_t *)BIMC_BASE,slave,index), \
   BIMC_SEGMENTn_ADDR_MASKm_LOWER_RMSK, \
   BIMC_SEGMENTn_ADDR_MASKm_LOWER_RMSK, \
   (addr_mask) }

#define ARB_MODE(slave,mode) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_S_ARB_MODE_ADDR((uint8_t *)BIMC_BASE,slave), \
   BIMC_S_ARB_MODE_RMSK, \
   BIMC_S_ARB_MODE_RMSK, \
   (mode) }

#define SWAY_GATHERING(slave,mode) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_S_SWAY_GATHERING_ADDR(BIMC_BASE,slave), \
   BIMC_S_SWAY_GATHERING_RMSK, \
   BIMC_S_SWAY_GATHERING_RMSK, \
   (mode) }

/* Slave indexes */
#define SLAVE_DDR_CH0 0
#define SLAVE_DDR_CH1 1
#define SLAVE_APPS_L2 2
#define SLAVE_SNOC    3

#define MPORT_MODE(master,mode) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_MODE_ADDR((uint8_t *)BIMC_BASE,master), \
   BIMC_M_MODE_RMSK, \
   BIMC_M_MODE_RMSK, \
   (mode) }

#define MPORT_PIPE0_GATHERING(master,value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_PIPE0_GATHERING_ADDR(BIMC_BASE,master), \
   BIMC_M_PIPE0_GATHERING_RMSK, \
   BIMC_M_PIPE0_GATHERING_RMSK, \
   (value) }

#define MPORT_RD_TRACKING_INDEX(master,value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_RD_TRACKING_INDEX_ADDR(BIMC_BASE,master), \
   BIMC_M_RD_TRACKING_INDEX_RMSK, \
   BIMC_M_RD_TRACKING_INDEX_RMSK, \
   (value) }

#define MPORT_WR_TRACKING_INDEX(master,value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_WR_TRACKING_INDEX_ADDR(BIMC_BASE,master), \
   BIMC_M_WR_TRACKING_INDEX_RMSK, \
   BIMC_M_WR_TRACKING_INDEX_RMSK, \
   (value) }

#define MPORT_BKE_ENABLE(master,value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_BKE_ENABLE_ADDR(BIMC_BASE,master), \
   BIMC_M_BKE_ENABLE_RMSK, \
   BIMC_M_BKE_ENABLE_RMSK, \
   (value) }

#define MPORT_BKE1_ENABLE(master,value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_BKE1_ENABLE_ADDR(BIMC_BASE,master), \
   BIMC_M_BKE1_ENABLE_RMSK, \
   BIMC_M_BKE1_ENABLE_RMSK, \
   (value) }

#define MPORT_BKE3_ENABLE(master,value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_BKE3_ENABLE_ADDR((uint8_t *)BIMC_BASE,master), \
   BIMC_M_BKE3_ENABLE_RMSK, \
   BIMC_M_BKE3_ENABLE_RMSK, \
   (value) }

#define MPORT_BKE3_GRANT_PERIOD(master,value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_BKE3_GRANT_PERIOD_ADDR((uint8_t *)BIMC_BASE,master), \
   BIMC_M_BKE3_GRANT_PERIOD_RMSK, \
   BIMC_M_BKE3_GRANT_PERIOD_RMSK, \
   (value) }

#define MPORT_BKE3_GRANT_COUNT(master,value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_BKE3_GRANT_COUNT_ADDR((uint8_t *)BIMC_BASE,master), \
   BIMC_M_BKE3_GRANT_COUNT_RMSK, \
   BIMC_M_BKE3_GRANT_COUNT_RMSK, \
   (value) }

#define MPORT_BKE3_THRESHOLD_MEDIUM(master,value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_BKE3_THRESHOLD_MEDIUM_ADDR((uint8_t *)BIMC_BASE,master), \
   BIMC_M_BKE3_THRESHOLD_MEDIUM_RMSK, \
   BIMC_M_BKE3_THRESHOLD_MEDIUM_RMSK, \
   (value) }

#define MPORT_BKE3_THRESHOLD_LOW(master,value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_BKE3_THRESHOLD_LOW_ADDR((uint8_t *)BIMC_BASE,master), \
   BIMC_M_BKE3_THRESHOLD_LOW_RMSK, \
   BIMC_M_BKE3_THRESHOLD_LOW_RMSK, \
   (value) }

#define MPORT_BKE3_HEALTH_0(master,value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_M_BKE3_HEALTH_0_ADDR((uint8_t *)BIMC_BASE,master), \
   BIMC_M_BKE3_HEALTH_0_RMSK, \
   BIMC_M_BKE3_HEALTH_0_RMSK, \
   (value) }

/* Master indexes */
#define MASTER_APP   0
#define MASTER_DSP   1
#define MASTER_GPU   2
#define MASTER_MMSS0 3
#define MASTER_MMSS1 4
#define MASTER_SYS   5
#define MASTER_TCU   6

#define QOS_CTRL(value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_QOS_CTRL_ADDR(BIMC_BASE), \
   BIMC_QOS_CTRL_RMSK, \
   BIMC_QOS_CTRL_RMSK, \
   (value) }

#define QOS_CFG(value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_QOS_CFG_ADDR(BIMC_BASE), \
   BIMC_QOS_CFG_RMSK, \
   BIMC_QOS_CFG_RMSK, \
   (value) }

#define QOS_TIMEOUT_CNT_LOW_URGENCY(value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_QOS_TIMEOUT_CNT_LOW_URGENCY_ADDR(BIMC_BASE), \
   BIMC_QOS_TIMEOUT_CNT_LOW_URGENCY_RMSK, \
   BIMC_QOS_TIMEOUT_CNT_LOW_URGENCY_RMSK, \
   (value) }

#define QOS_TIMEOUT_CNT_HIGH_URGENCY(value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_QOS_TIMEOUT_CNT_HIGH_URGENCY_ADDR(BIMC_BASE), \
   BIMC_QOS_TIMEOUT_CNT_HIGH_URGENCY_RMSK, \
   BIMC_QOS_TIMEOUT_CNT_HIGH_URGENCY_RMSK, \
   (value) }

#define QOS_FSSH_URGENCY_SEL(value) \
  {ICBCFG_32_BIT_REG, \
   (void *)BIMC_QOS_FSSH_URGENCY_SEL_ADDR(BIMC_BASE), \
   BIMC_QOS_FSSH_URGENCY_SEL_RMSK, \
   BIMC_QOS_FSSH_URGENCY_SEL_RMSK, \
   (value) }

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
    ICBCFG_DEFAULT, 
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
  { 0x40000000ULL, 0x100000000ULL },
};

uint32 channel_map[2] = { SLAVE_DDR_CH0, SLAVE_DDR_CH1 };

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
  0x40000000ULL,                 /* 1 GB */
  BIMC_SEGMENT_TYPE_SUBTRACTIVE,
  BIMC_INTERLEAVE_NONE,
};

/*---------------------------------------------------------------------------*/
/*          Properties data for new Device IDs: Add below                    */
/*---------------------------------------------------------------------------*/
