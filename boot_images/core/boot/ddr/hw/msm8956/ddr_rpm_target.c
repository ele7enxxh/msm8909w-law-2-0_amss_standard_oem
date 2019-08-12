/**
 * @file ddr_target.c
 * @brief
 * Target specific DDR drivers.
 */
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/hw/msm8956/ddr_rpm_target.c#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/21/14   aus     Updated to support shared DDR library
03/27/14   rp	   Added a voltage checksum for railway and rbcpr hash
10/25/13   tw      Added support for LPAE
09/09/13   sr      Removed unused extern variables .
09/06/13   sr      Skeleton Code for PHY training in SBL1
09/05/13   sr      initial changes to support training in boot.
08/27/13   sr      is_training_required moved from params.c to target.c
06/21/13   tw      Initial Port for 8084
05/13/13   sl      Updated delta table calls.
03/01/13   tw      Added calling delta table
11/02/12   tw      re-enabled 800mhz, added site initialization
11/02/12   tw      enabled flag for training required
11/02/12   sy      Back out 800Mhz
09/21/12   sl      Combined LPDDR device parameters.
09/17/12   sl      Updated DPE clock switch function calls.
09/12/12   sl      Use macro-style DDR logging.
09/07/12   sl      Moved ddr_get_partition() to ddr_drivers.c.
09/06/12   sl      Added parameter tweaking functions.
08/17/12   sl      Removed ddr_register_fill().
08/08/12   dh      change CORE_CLK_PERIOD to 50 for bring up
07/24/12   tw      Removed bus configuration from one time settings
11/30/11   tw      Initial version.
================================================================================
                   Copyright 2011-2014 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_common.h"
#include "ddr_drivers.h"
#include "ddr_internal.h"
#include "ddr_sync.h"
#include "ddr_log.h"
#include "ddr_params.h"
#include "HALhwio.h"
#include "ddr_target.h"
#include "HAL_SEQ_DDR.h"
#include "BIMC.h"
#include "ddr_config.h"
#include "ClockBoot.h"
#include "icbcfg.h"
#include "ddrss_rcw.h"
#include "bimc_scale.h"
#include "msmhwiobase.h"
#include "msmhwioreg.h"
#if 0
#include "railway.h"
#include "rbcpr_api.h"
#endif
#include "ddr_func_tbl.h"
#include "mpm_hwio.h"

/*==============================================================================
                                  MACROS
==============================================================================*/

/*==============================================================================
                                  DATA
==============================================================================*/
__attribute__((section("ddr_func_ptr"))) ddr_func ddr_func_tbl;

boolean wdog_reset = FALSE;

/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
#define BUSYWAIT_XO_FREQUENCY_IN_KHZ 20

/* ddr driver implementation for WDOG reset case */
boolean ddr_busywait_init(void)
{
  HWIO_OUTF(MPM2_MPM_CONTROL_CNTCR, EN, 0x1); 
  HWIO_OUTI(MPM2_QTMR_AC_CNTACR_n,0,0x3F);
  
  return TRUE;
}

void ddr_busywait (uint32 pause_time_us)
{
  uint32 start_count = 0;
  uint64 delay_count = 0;

  start_count = HWIO_IN(MPM2_QTMR_V1_CNTPCT_LO);
  /*
   * Perform the delay and handle potential overflows of the timer.
   */

  delay_count = (pause_time_us * (uint64)BUSYWAIT_XO_FREQUENCY_IN_KHZ);
  while ( (HWIO_IN(MPM2_QTMR_V1_CNTPCT_LO) - start_count) < delay_count);
  
}

/* ============================================================================
**  Function : HAL_SDRAM_DPE_Force_Update_Latency
** ============================================================================
*/
/*!
 *   @brief
 *   This function will force update the read write latency based on the given
 *   clock speed
 *
 *   @details
 *   This function will force update the read write latency based on the given
 *   clock speed, unlike HAL_SDRAM_DPE_Update_Latency, this function will apply
 *   the new latency immediately
 *
 *   @param interface  -  [IN] interface to update latency for
 *   @param clk_speed  -  [IN] clock speed in khz
 *
 *   @par Dependencies
 *   None
 *
 *   @par Side Effects
 *   None
 *
 *   @retval  None
 *
 *   @sa None
 */
void HAL_SDRAM_DPE_Force_Update_Latency(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint32 clk_speed)
{
  uint32 rl, wl, data;
 
  rl = HAL_SDRAM_RL(interface, clk_speed);
  wl = HAL_SDRAM_WL(interface, clk_speed); 
     
  if (rl == 3 && wl == 1) { data = 0x1; }
  else if (rl == 4 && wl == 2) { data = 0x2; }
  else if (rl == 5 && wl == 2) { data = 0x3; }
  else if (rl == 6 && wl == 3) { data = 0x4; }
  else if (rl == 7 && wl == 4) { data = 0x5; }
  else if (rl == 8 && wl == 4) { data = 0x6; }
  else if (rl == 9 && wl == 5) { data = 0x7; }
  else if (rl == 10 && wl == 6) { data = 0x8; }
  else if (rl == 11 && wl == 6) { data = 0x9; }
  else { data = 0xA; }  /* Use RL=12/WL=6 in case all else fails */


  HAL_SDRAM_Write_MR(interface, chip_select, 0x2, data);
}

/* ============================================================================
**  Function : ddr_param_interleaved
** ============================================================================
*/
/**
*   @brief
*   Return the interleave setting based on ddr parameters
*
*   @return
*   return a bitmask of interleave status
*
*   @dependencies
*   Must be called after ddr_set_params. Calling it before ddr_set_params will
*   return the default setting which is TRUE.
*
*   @sa
*   None
*
*   @sa
*   None
*/
uint32 ddr_param_interleaved(void)
{
  struct ddr_device_params_header_v2 *ddr_params_header; 
  uint32 func_addr = (uint32)&ddr_param_interleaved;
  
  if (func_addr & SCL_RPM_CODE_RAM_BASE)
    ddr_params_header = (struct ddr_device_params_header_v2*)(SHARED_IMEM_DDR_PARAM_BASE);
  else
    ddr_params_header = (struct ddr_device_params_header_v2*)(RPM_SHARED_IMEM_DDR_PARAM_BASE);
	
  return ddr_params_header->interleaved;
}

uint32 Get_BIMC_Base_Address(void)
{
  uint32 func_addr = (uint32)&Get_BIMC_Base_Address;
  
  if (func_addr & SCL_RPM_CODE_RAM_BASE)
    return 0;
  else
    return RPM_BIMC_BASE_OFFSET;	 
}

/* ============================================================================
**  Function : ddr_get_params
** ============================================================================
*/
/**
*   @brief
*   Get DDR device parameters.
*
*   @param[in]  interface_name  Interface to get DDR device parameters for
*
*   @return
*   DDR device parameters
*
*   @dependencies
*   None
*
*   @sa
*   None
*
*   @sa
*   None
*/
union ddr_device_params *ddr_get_params(SDRAM_INTERFACE interface_name)
{
  union ddr_device_params *device_params, *device_table;
  uint32 device_params_size;
  uint32 func_addr = (uint32)&ddr_get_params;
  
  if (func_addr & SCL_RPM_CODE_RAM_BASE)
    device_table = (union ddr_device_params *)(SHARED_IMEM_DDR_PARAM_BASE + DDR_PARAMS_HEADER_V2_SIZE);
  else
    device_table = (union ddr_device_params *)(RPM_SHARED_IMEM_DDR_PARAM_BASE + DDR_PARAMS_HEADER_V2_SIZE);
	
  if (interface_name == SDRAM_INTERFACE_0)
  {
    device_params = device_table;
  }
  else
  {
    device_params_size = sizeof(struct ddr_device_params_lpddr);
    device_params = (union ddr_device_params *)
                    ((uint32)device_table + device_params_size);
  }

  return device_params;

} /* ddr_get_params */

/* =============================================================================
**  Function : ddr_restore_from_wdog_reset
** =============================================================================
*/
/**
*   @brief
*   function called to take ddr out of self refresh following a wdog reset
*
*   @param [in] clk_speed_khz  clock speed of ddr in khz
*
*   @retval  NONE
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void ddr_restore_from_wdog_reset( uint32 clk_speed_khz )
{
  struct ddr_device_params_common *ddr_params_ch0, *ddr_params_ch1;
  SDRAM_CHIPSELECT ch0_chip_sel = SDRAM_CS_NONE, ch1_chip_sel = SDRAM_CS_NONE;
  /* Get DDR device parameters */
  ddr_params_ch0 = &(ddr_get_params(SDRAM_INTERFACE_0)->common);
  ddr_params_ch1 = &(ddr_get_params(SDRAM_INTERFACE_1)->common);
  
  if(ddr_params_ch0->num_banks_cs0 != 0)
  {
    ch0_chip_sel |= SDRAM_CS0;
  }
  if(ddr_params_ch0->num_banks_cs1 != 0)
  {
    ch0_chip_sel |= SDRAM_CS1;
  }
  if(ddr_params_ch1->num_banks_cs0 != 0)
  {
    ch1_chip_sel |= SDRAM_CS0;
  }
  if(ddr_params_ch1->num_banks_cs1 != 0)
  {
    ch1_chip_sel |= SDRAM_CS1;
  }
  
  /* set status variable to indicate wdog reset */
  wdog_reset = TRUE;
  
  /* initialize ddr version of busywait */
  ddr_busywait_init();
  
  /* hack mpm to unfreeze io */
  HWIO_OUT(MPM2_MPM_DDR_PHY_FREEZEIO_EBI1, 0x0);
  
  /* hack to get DDR out of self refresh manually */
  HWIO_ABHN_SHKE_DRAM_MANUAL_0_OUTM(SEQ_DDRSS_BIMC_BIMC_S_DDR0_SHKE_OFFSET, 0x2000, 0x2000);
  
  /* as we have hacked the exit, we are forcing SW self refresh here */
  HAL_SDRAM_Enter_Self_Refresh(SDRAM_INTERFACE_0, ch0_chip_sel);  
  
  /* restore ddr from self refresh */
  if(ch0_chip_sel != SDRAM_CS_NONE)
  {	
    HAL_SDRAM_Init(SDRAM_INTERFACE_0, ch0_chip_sel, clk_speed_khz);
    
    /* exit self refresh */
    HAL_SDRAM_Exit_Self_Refresh(SDRAM_INTERFACE_0, ch0_chip_sel);
    
    /* update rl\wl latency to match configuration */
    HAL_SDRAM_DPE_Force_Update_Latency(0, SDRAM_INTERFACE_0, ch0_chip_sel, clk_speed_khz);
  }

  if(ch1_chip_sel != SDRAM_CS_NONE)
  {
    HAL_SDRAM_Init(SDRAM_INTERFACE_1, ch1_chip_sel, clk_speed_khz);
    
    /* exit self refresh */
    HAL_SDRAM_Exit_Self_Refresh(SDRAM_INTERFACE_1, ch1_chip_sel);
  }  
}
