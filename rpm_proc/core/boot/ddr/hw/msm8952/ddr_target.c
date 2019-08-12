/**
 * @file ddr_target.c
 * @brief
 * Target specific DDR drivers.
 */
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/rpm.bf/2.1.1/core/boot/ddr/hw/msm8952/ddr_target.c#1 $
$DateTime: 2015/02/25 02:53:43 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/16/14   rj      Fixed PHY Low power settings issue in active mode
08/11/14   rj      PHY Low power settings in active mode
07/16/14   aus     Setup a buffer for DDR params in RPM Data RAM
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
#include "ddr_target.h"
#include "mpm.h"
#include "HAL_SEQ_DDR.h"
#include "railway.h"
#include "ddr_func_tbl.h"
#include "ddr_hwio.h"
#include "busywait.h"

/*==============================================================================
                                  DATA
==============================================================================*/
/* Variable for saving SHKE_CONFIG register */
static uint32 shke_config;

/* DDR status */
extern ddr_interface_state ddr_status;

ddr_func *ddr_func_tbl;					

/* Pointer to shared imem DDR device table, this table is valid after ddr initialization is done */
static union ddr_device_params *shared_ddr_device_table_wdog = (union ddr_device_params *)(SHARED_IMEM_DDR_PARAM_BASE + DDR_PARAMS_HEADER_V2_SIZE);
static struct ddr_device_params_header_v2 *ddr_params_header = (struct ddr_device_params_header_v2*)(SHARED_IMEM_DDR_PARAM_BASE);	

/* DDR params in RPM Data RAM */
uint32 testDDR[256] __attribute__((at(SHARED_IMEM_DDR_PARAM_BASE)))={0xFF};	  

/*==============================================================================
                                  FUNCTIONS
==============================================================================*/

/* Overload api for funtion table */
void HAL_SDRAM_Init(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint32 clk_speed) 
{}

void HAL_SDRAM_Enter_Deep_Power_Down(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select)
{}

void HAL_SDRAM_Exit_Deep_Power_Down(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select, uint32 clkspeed)
{}

void HAL_SDRAM_Enter_Self_Refresh(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select)
{
  ddr_func_tbl->enter_self_refresh(interface, chip_select);
}

void HAL_SDRAM_Exit_Self_Refresh(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select)
{
  ddr_func_tbl->exit_self_refresh(interface, chip_select);
}

uint32 HAL_SDRAM_Get_Base_Addr(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select)
{
  return 0;
}

uint32 HAL_SDRAM_Read_MR(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint32 mr_addr)
{
  return 0;
}

void HAL_SDRAM_Pre_Clock_Switch(SDRAM_INTERFACE interface, uint32 clk_freq_in_khz)
{
  ddr_func_tbl->pre_clock_switch(interface, clk_freq_in_khz);
}

void HAL_SDRAM_Post_Clock_Switch(SDRAM_INTERFACE interface, uint32 clk_freq_in_khz)
{
  ddr_func_tbl->post_clock_switch(interface, clk_freq_in_khz);
}

void EBI1_PHY_CFG_disable_csd_rx(uint32 base, uint32 chnl, int selfrefresh_mode)
{
  ddr_func_tbl->ebi_phy_cfg_disable_csd_rx(base, chnl, selfrefresh_mode);
}

void EBI1_PHY_CFG_enable_csd_rx(uint32 base, uint32 chnl, phy_clk_type clk_mode)
{
  ddr_func_tbl->ebi_phy_cfg_enable_csd_rx(base, chnl, clk_mode);
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
  return ddr_params_header->interleaved;
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

  device_table = shared_ddr_device_table_wdog;

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

/* ============================================================================
**  Function : ddr_get_shared_header
** ============================================================================
*/
/**
*   @brief
*   This function returns a pointer to the shared header that contains useful
*   information from SBL to RPM
*
*   @param
*   None
*                         
*
*   @return
*   pointer to shared header location
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
struct ddr_shared_params * ddr_get_shared_header(void)
{
  return (struct ddr_shared_params *)SHARED_IMEM_DDR_PARAM_BASE;
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
  uint32 *ddr_func_tbl_ptr = (uint32 *)(SHARED_IMEM_DDR_PARAM_BASE+4);
  ddr_func_tbl = (ddr_func *)(*ddr_func_tbl_ptr);

#if 0
    /* Eng change to test out function table and shared library */
	HAL_SDRAM_Pre_Clock_Switch (SDRAM_INTERFACE_0, 800000);
	HAL_SDRAM_Post_Clock_Switch (SDRAM_INTERFACE_0, 800000);
#endif
  
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
**  Function : ddr_pre_clock_switch
** =============================================================================
*/
/**
*   @brief
*   Function called right before DDR clock switch.
*
*   @param[in]  curr_clk        Current clock speed (in KHz)
*   @param[in]  new_clk         New clock speed (in KHz)
*   @param[in]  interface_name  Interface to switch clock for
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
void ddr_pre_clock_switch(uint32 curr_clk, uint32 new_clk, SDRAM_INTERFACE interface_name)
{  
  HAL_SDRAM_Pre_Clock_Switch(interface_name, new_clk);
  if (new_clk > 533000)
  {
	/* Enable RX TX for non-legacy mode */
    EBI1_PHY_CFG_enable_csd_rx( EBI1_PHY_CFG_BASE, SDRAM_INTERFACE_0, (phy_clk_type)2);
  }

} /* ddr_pre_clock_switch */

/* =============================================================================
**  Function : ddr_post_clock_switch
** =============================================================================
*/
/**
*   @brief
*   Function called right after DDR clock switch.
*
*   @param[in]  curr_clk        Current clock speed (in KHz)
*   @param[in]  new_clk         New clock speed (in KHz)
*   @param[in]  interface_name  Interface to switch clock for
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
void ddr_post_clock_switch(uint32 curr_clk, uint32 new_clk, SDRAM_INTERFACE interface_name)
{
  HAL_SDRAM_Post_Clock_Switch(interface_name, new_clk);
  if (new_clk <= 533000)
  {
	/* Disable RX TX for legacy mode */
    EBI1_PHY_CFG_disable_csd_rx( EBI1_PHY_CFG_BASE, SDRAM_INTERFACE_0,0);
  }	
} /* ddr_post_clock_switch */

/* =============================================================================
**  Function : ddr_pre_xo_shutdown
** =============================================================================
*/
/**
*   @brief
*   Function called right before DDR XO shutdown.
*
*   @param[in]  clk_speed  Clock speed (in KHz) before XO shutdown
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
void ddr_pre_xo_shutdown(uint32 clk_speed)
{
  ddr_enter_self_refresh_all(clk_speed);

   /* Disable RX TX */
  EBI1_PHY_CFG_disable_csd_rx( EBI1_PHY_CFG_BASE, SDRAM_INTERFACE_0,1);

} /* ddr_pre_xo_shutdown */

/* =============================================================================
**  Function : ddr_post_xo_shutdown
** =============================================================================
*/
/**
*   @brief
*   Function called right after DDR XO shutdown.
*
*   @param[in]  clk_speed  Clock speed (in KHz) after XO shutdown
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
void ddr_post_xo_shutdown(uint32 clk_speed)
{
  ddr_exit_self_refresh_all(clk_speed);

  /* Enable RX TX */
  EBI1_PHY_CFG_enable_csd_rx( EBI1_PHY_CFG_BASE, SDRAM_INTERFACE_0, (phy_clk_type)2);  
  
} /* ddr_post_xo_shutdown */


/* =============================================================================
**  Function : ddr_enter_power_collapse
** =============================================================================
*/
/**
*   @brief
*   Function called before DDR controller goes into power collapse.
*
*   @param[in]  clk_speed  Current clock speed (in KHz)
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
void ddr_enter_power_collapse(uint32 clk_speed)
{
  /* hack to get DDR out of self refresh manually */
  BOOT_HWIO_OUTM(BIMC_S_DDR0_SHKE_DRAM_MANUAL_0,0,0x2000,0x2000);
  
  /* Put DDR into self refresh */
  ddr_enter_self_refresh_all(clk_speed);

  /* Disable RX TX */
  EBI1_PHY_CFG_disable_csd_rx( EBI1_PHY_CFG_BASE, SDRAM_INTERFACE_0,1);

  /* Enable EBI1 clamps via freeze_io */
  mpm_config_ebi1_freeze_io(TRUE);

  /* Save SHKE_CONFIG registers that are not backed up by DEHR */
  shke_config = BOOT_HWIO_IN(DDR_ADDR(SHKE_CONFIG), SDRAM_0_OFFSET);

} /* ddr_enter_power_collapse */

/* =============================================================================
**  Function : ddr_exit_power_collapse
** =============================================================================
*/
/**
*   @brief
*   Function called after DDR controller comes out of power collapse.
*
*   @param[in]  clk_speed  Current clock speed (in KHz)
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
void ddr_exit_power_collapse(uint32 clk_speed)
{
  uint32 rank_init_fmsk;
  SDRAM_CHIPSELECT chip_select;

  /* Field masks for SHKE_CONFIG[RANK0/1_INITCOMPLETE] */
  rank_init_fmsk = HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK0_INITCOMPLETE) |
                   HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK1_INITCOMPLETE);

  chip_select = SDRAM_CS_NONE;
  if (ddr_status.sdram0_cs0 == DDR_SELF_REFRESH)
  {
    chip_select |= SDRAM_CS0;
  }
  if (ddr_status.sdram0_cs1 == DDR_SELF_REFRESH)
  {
    chip_select |= SDRAM_CS1;
  }

  if (chip_select != SDRAM_CS_NONE)
  {
    /* Compute timing parameters in cycles */
    BOOT_HWIO_OUTM(DDR_ADDR(DPE_CONFIG_4), SDRAM_0_OFFSET,
                   HWIO_FMSK(DDR_ADDR(DPE_CONFIG_4), RECALC_PS_PARAMS),
                   0x1U << HWIO_SHFT(DDR_ADDR(DPE_CONFIG_4), RECALC_PS_PARAMS));
    /* Wait for computing done */
    while ( BOOT_HWIO_INM(DDR_ADDR(DPE_MEMC_STATUS_1), SDRAM_0_OFFSET,
                          HWIO_FMSK(DDR_ADDR(DPE_MEMC_STATUS_1), CYC_CALC_VALID)) )
    {}

    /* Load timing registers */
    BOOT_HWIO_OUTM(DDR_ADDR(DPE_CONFIG_4), SDRAM_0_OFFSET,
                   HWIO_FMSK(DDR_ADDR(DPE_CONFIG_4), LOAD_ALL_CONFIG),
                   0x1U << HWIO_SHFT(DDR_ADDR(DPE_CONFIG_4), LOAD_ALL_CONFIG));

    /* Restore SHKE_CONFIG expect SHKE_CONFIG[RANK0/1_INITCOMPLETE] */
    BOOT_HWIO_OUT(DDR_ADDR(SHKE_CONFIG), SDRAM_0_OFFSET,
                  shke_config & ~rank_init_fmsk);

    /* Toggle SHKE_CONFIG[RANK0/1_INITCOMPLETE] as workaround for HW issue that
       TREFI is not loaded correctly */
    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_CONFIG), SDRAM_0_OFFSET,
                   rank_init_fmsk, 0x0);

    /* Wait for 1us (at least 3 XO cycles) */
    busywait(1);

    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_CONFIG), SDRAM_0_OFFSET,
                   rank_init_fmsk, shke_config);

    /* hack to get DDR out of self refresh manually */
    BOOT_HWIO_OUTM(BIMC_S_DDR0_SHKE_DRAM_MANUAL_0,0,0x2000,0x2000);				   
				   
    /* Put DDR into self refresh with EBI1 clamped to restore controller state */
    HAL_SDRAM_Enter_Self_Refresh(SDRAM_INTERFACE_0, chip_select);
  }

  /* Disable EBI1 clamps via freeze_io */
  mpm_config_ebi1_freeze_io(FALSE);

   /* Enable RX TX */
  EBI1_PHY_CFG_enable_csd_rx( EBI1_PHY_CFG_BASE, SDRAM_INTERFACE_0, (phy_clk_type)2);  
  
  /* Take DDR out of self refresh */
  ddr_exit_self_refresh_all(clk_speed);

} /* ddr_exit_power_collapse */
