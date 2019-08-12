/**
 * @file ddr_target.c
 * @brief
 * Target specific DDR drivers.
 */
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/hw/mdm9x45/ddr_target.c#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/24/14   tw      Updated update clock period function pointer
02/09/14   tw      added support for common ddr library
01/24/14   tw      initial port for 8916
                   added support for sbl\rpm shared ddr driver
09/05/13   sr      changes to not support training in boot.
06/20/13   rj      Added support for lpddr2 on GIMLI
06/06/13   rj      Updated 8926 delta settings and added function
                   related to ddr draining required.
05/13/13   sl      Removed ddr_apply_delta_settings().
04/10/13   sl      Use voltage mode instead of voltage number.
04/03/13   sl      Added macros for vddmx voltage levels.
03/01/13   tw      Added stubs for applying delta settings
02/21/13   sl      Initial version.
================================================================================
                      Copyright 2013-2014 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_common.h"
#include "ddr_drivers.h"
#include "ddr_params.h"
#include "ddr_internal.h"
#include "ddr_target.h"
#include "ddr_config.h"
#include "ddr_log.h"
#include "DDIChipInfo.h"
#include "BIMC.h"
#include "DDR_PHY.h"
#include "ddr_func_tbl.h"
#include "DDIChipInfo.h"
#include "clock_hwio.h"
#include "mpm_hwio.h"

/*==============================================================================
                                  DATA
==============================================================================*/
/* DDR logging level */
enum ddr_log_level ddr_log_level = DDR_ERROR;

/* Initial DDR PHY CDC delay */
uint32 DDR_PHY_INIT_CDC_DELAY;

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

/* =============================================================================
**  Function : ddr_target_init
** =============================================================================
*/
/**
*   @brief
*   Function called at the end of ddr_init() to initialize target specific
*   configurations.
*
*   @param  None
*
*   @retval  None
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
void ddr_target_init()
{
  struct ddr_shared_params *header;
  
  extern uint32 ddr_bimc_config[][2];
  extern uint32 ddr_phy_config[][2];
  extern uint32 ddr_caphy_config[][2];
  extern uint32 ddr_dqphy_config[][2];
  
  //if (DalChipInfo_ChipFamily() == DALCHIPINFO_ID_MSM8916)
  {
    if (ddr_get_params(SDRAM_INTERFACE_0)->common.device_type == DDR_TYPE_LPDDR2)
    {
      ddr_bimc_config_delta = ddr_bimc_config;
      ddr_phy_config_delta = ddr_phy_config;
      ddr_caphy_config_delta = ddr_caphy_config;
      ddr_dqphy_config_delta = ddr_dqphy_config;
    }
    DDR_PHY_INIT_CDC_DELAY = 0x26B;
  }
  
  /* initialization of function tables for shared ddr library 
   * taking into account RPM view of shared library is different 
   */
  ddr_func_tbl.enter_self_refresh = (void (*)(uint32, SDRAM_INTERFACE, SDRAM_CHIPSELECT, ddr_wait))
                                    ((uint8 *)&HAL_SDRAM_Enter_Self_Refresh - SCL_RPM_CODE_RAM_BASE);
  ddr_func_tbl.exit_self_refresh = (void (*)(uint32, SDRAM_INTERFACE, SDRAM_CHIPSELECT, ddr_wait))
                                   ((uint8 *)&HAL_SDRAM_Exit_Self_Refresh - SCL_RPM_CODE_RAM_BASE);
  
  ddr_func_tbl.get_read_latency = (uint32 (*) (DDR_TYPE , uint32 )) 
                                  ((uint8 *)&HAL_SDRAM_RL - SCL_RPM_CODE_RAM_BASE);
  ddr_func_tbl.get_write_latency = (uint32 (*) (DDR_TYPE , uint32 )) 
                                   ((uint8 *)&HAL_SDRAM_WL - SCL_RPM_CODE_RAM_BASE);
  ddr_func_tbl.bimc_update_clock_period = (void (*)(uint32, uint32, ddr_divide )) 
                                          ((uint8 *)&HAL_SDRAM_BIMC_Update_Clock_Period - SCL_RPM_CODE_RAM_BASE);
  ddr_func_tbl.bimc_dpe_pre_clock_switch = (void (*)(uint32 , SDRAM_INTERFACE , uint32 , uint32 )) 
                                           ((uint8 *)&HAL_SDRAM_DPE_Pre_Clock_Switch - SCL_RPM_CODE_RAM_BASE);
  ddr_func_tbl.bimc_dpe_post_clock_switch = (void (*)(uint32 , SDRAM_INTERFACE , uint32 , uint32 )) 
                                            ((uint8 *)&HAL_SDRAM_DPE_Post_Clock_Switch - SCL_RPM_CODE_RAM_BASE);
  
  ddr_func_tbl.phy_manual_io_cal = (void (*)(uint32 , SDRAM_INTERFACE , ddr_wait )) 
                                   ((uint8 *)&HAL_SDRAM_PHY_Manual_IO_Cal - SCL_RPM_CODE_RAM_BASE);
  ddr_func_tbl.phy_update_cdc_config = (void (*)(uint32 , SDRAM_INTERFACE , uint32 )) 
                                    ((uint8 *)&HAL_SDRAM_PHY_Update_CDC_Config - SCL_RPM_CODE_RAM_BASE);
  ddr_func_tbl.phy_manual_cdc_cal = (void (*)(uint32 , SDRAM_INTERFACE )) 
                                    ((uint8 *)&HAL_SDRAM_PHY_Manual_CDC_Cal - SCL_RPM_CODE_RAM_BASE);
  ddr_func_tbl.phy_update_drive_strength = (void (*) (uint32 ,SDRAM_INTERFACE, uint32 , uint32 , uint32 )) 
                                           ((uint8 *)&HAL_SDRAM_PHY_Update_Drive_Strength - SCL_RPM_CODE_RAM_BASE);
  ddr_func_tbl.phy_update_cdc_delay = (void (*)(uint32 , SDRAM_INTERFACE , boolean read, uint32 cdc_delay)) 
                                      ((uint8 *)&HAL_SDRAM_PHY_Update_CDC_Delay - SCL_RPM_CODE_RAM_BASE);
  ddr_func_tbl.phy_enable_rapid_cdc_cal = (void (*)(uint32 , SDRAM_INTERFACE )) 
                                          ((uint8 *)&HAL_SDRAM_PHY_Enable_Rapid_CDC_Cal - SCL_RPM_CODE_RAM_BASE);
  ddr_func_tbl.phy_disable_rapid_cdc_cal = (void (*)(uint32 , SDRAM_INTERFACE )) 
                                           ((uint8 *)&HAL_SDRAM_PHY_Disable_Rapid_CDC_Cal - SCL_RPM_CODE_RAM_BASE);
  
   
  /* clear out shared header and fill in with updated information */
  header = ddr_get_shared_header();
  memset(header, 0, sizeof(struct ddr_shared_params));
  
  header->ddr_func_tbl_ptr = (void *) ((uint8 *)&ddr_func_tbl - SCL_RPM_CODE_RAM_BASE);
  header->size = sizeof(ddr_func);
} /* ddr_target_init */

/* =============================================================================
**  Function : ddr_post_init
** =============================================================================
*/
/**
*   @brief
*   Function called after DDR is initialized for DDR training if required.
*
*   @param  None
*
*   @retval  TURE   DDR training required
*   @retval  FALSE  DDR training not required
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
boolean ddr_post_init()
{
  return FALSE;

} /* ddr_post_init */

/* =============================================================================
**  Function : ddr_params_is_training_required
** =============================================================================
*/
/**
*   @brief
*   Indicate whether DDR parameter training is required or not. Training is
*   required if and only if DDR itself (e.g. PCDDR3) requires parameter training
*   and DDR parameter partition is invalid. LPDDR3 will always require training syncronization
*   to be done between rpm and sbl
*
*   @param  None
*
*   @retval  TRUE   Training required
*   @retval  FALSE  Training not required
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
boolean ddr_params_is_training_required( void )
{
  return FALSE;
} /* ddr_params_is_training_required */

/* =============================================================================
**  Function : ddr_params_is_training_on_rpm_required
** =============================================================================
*/
/**
*   @brief
*   Indicate whether DDR parameter training is required or not in RPM. Training is
*   required if and only if DDR itself (e.g. PCDDR3) requires parameter training
*   and DDR parameter partition is invalid. 
*
*   @param  None
*
*   @retval  TRUE   Training required
*   @retval  FALSE  Training not required
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
boolean ddr_params_is_training_on_rpm_required( void )
{
  return FALSE;
} /* ddr_params_is_training_on_rpm_required */

/* =============================================================================
**  Function : ddr_do_phy_training
** =============================================================================
*/
/**
*   @brief
*   function called to do the DDR PHY training 
*
*   @param  None
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
void ddr_do_phy_training( void )
{  
} /* ddr_do_phy_training */

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
  
  /* restore cdt parameters */
  ddr_restore_param_wdog_reset();
  
  /* restore ddr from self refresh */
  if(ch0_chip_sel != SDRAM_CS_NONE)
  {
    HAL_SDRAM_Init(SDRAM_INTERFACE_0, ch0_chip_sel, clk_speed_khz);
    
    /* exit self refresh */
    HAL_SDRAM_Exit_Self_Refresh(0, SDRAM_INTERFACE_0, ch0_chip_sel, HAL_SDRAM_DDR_Wait);
    
    /* update rl\wl latency to match configuration */
    HAL_SDRAM_DPE_Force_Update_Latency(0, SDRAM_INTERFACE_0, ch0_chip_sel, clk_speed_khz);
  }

  if(ch1_chip_sel != SDRAM_CS_NONE)
  {
    HAL_SDRAM_Init(SDRAM_INTERFACE_0, ch1_chip_sel, clk_speed_khz);
    
    /* exit self refresh */
    HAL_SDRAM_Exit_Self_Refresh(0, SDRAM_INTERFACE_0, ch1_chip_sel, HAL_SDRAM_DDR_Wait);
  }  
}

/* ============================================================================
**  Function : ddr_set_params_enhance,enhance set
** ============================================================================
*/
/**
*   @brief
*   This function sets the DDR DC timing parameters stored in CDB2 and CDB3.enhance set.
*   For CDB2,this function will directly set the registers of DDR driver .
*   For CDB3,this function will put the data in IMEM for RPM setting.
*   @param[in]  ddr_params_ptr   pointer to the base address of CDB2 or CDB3 in memory
*                
*
*   @return
*   TRUE if the parameters was successfully set.
*
*   @dependencies
*  None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
boolean ddr_set_params_enhance(void *ddr_params_ptr, uint32 ddr_params_size)
{
  return TRUE;
} /*ddr_set_params_enhance*/

