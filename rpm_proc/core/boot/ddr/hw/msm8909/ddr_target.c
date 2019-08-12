/**
 * @file ddr_target.c
 * @brief
 * Target specific DDR drivers.
 */
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/rpm.bf/2.1.1/core/boot/ddr/hw/msm8909/ddr_target.c#4 $
$DateTime: 2015/06/04 23:00:23 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/29/15   aus     Updates for 8208 HW with Fmax as 400MHz
03/04/15   sc      Updated phy_update_cdc_delay and cdc delay for turbo mode
12/19/14   xl      Add post boot ddr enhance set
01/19/15   aus     Revert cdc delay to 469ps
01/02/15   aus     Update wr cdc delay to 280ps 
02/24/14   tw      Updated static vddmx ddr configuration
                   updated update clock period api to match new definition
02/09/14   tw      Added support for common shared library
02/21/13   sl      Initial version.
================================================================================
                      Copyright 2013-2015 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_common.h"
#include "ddr_drivers.h"
#include "ddr_internal.h"
#include "ddr_log.h"
#include "ddr_hwio.h"
#include "ddr_target.h"
#include "HAL_DDR.h"
#include "BIMC.h"
#include "DDR_PHY.h"
#include "mpm.h"
#include "railway.h"
#include "ddr_func_tbl.h"
#include "busywait.h"

/*==============================================================================
                                  DATA
==============================================================================*/
#define HW_PART_NUM_8208 0x962

/* DDR status */
extern ddr_interface_state ddr_status;

/* DDR logging level */
enum ddr_log_level ddr_log_level = DDR_ERROR;

/* DDR PHY dynamic settings per vddmx voltage level for CA and RD CDC delay */
static uint32 ddr_phy_config_vddmx[RAILWAY_CORNERS_COUNT] =
{
  0x271,  /* RAILWAY_NO_REQUEST */
  0x271,  /* RAILWAY_RETENTION */
  0x271,  /* RAILWAY_SVS_KRAIT */
  0x271,  /* RAILWAY_SVS_SOC */
  0x271,  /* RAILWAY_SVS_HIGH */
  0x271,  /* RAILWAY_NOMINAL */
  0x1D5,  /* RAILWAY_TURBO */
  0x1D5,  /* RAILWAY_TURBO_HIGH */
  0x1D5,  /* RAILWAY_SUPER_TURBO */
  0x1D5,  /* RAILWAY_SUPER_TURBO_NO_CPR */
};

/* Variable for saving SHKE_CONFIG register */
static uint32 shke_config;
uint32 hw_part_num = 0;
  
ddr_func *ddr_func_tbl;

/*==============================================================================
                                  FUNCTIONS
==============================================================================*/

/* Overload api for funtion table */
void HAL_SDRAM_Init(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint32 clk_speed) {}

void HAL_SDRAM_Enter_Self_Refresh(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, ddr_wait wait)
{
  ddr_func_tbl->enter_self_refresh(base, interface, chip_select, HAL_SDRAM_DDR_Wait);
}

void HAL_SDRAM_Exit_Self_Refresh(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, ddr_wait wait)
{
  ddr_func_tbl->exit_self_refresh(base, interface, chip_select, HAL_SDRAM_DDR_Wait);
}

void HAL_SDRAM_Enter_Deep_Power_Down(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select)
{}
void HAL_SDRAM_Exit_Deep_Power_Down(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint32 clkspeed)
{}
uint32 HAL_SDRAM_Get_Base_Addr(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select)
{
  return 0;
}
void HAL_SDRAM_DDR_Wait(uint32 wait_us, uint32 clk_speed)
{
  busywait(wait_us);
}
uint32 ddr_divide_func(uint32 a, uint32 b)
{
  return a/b;
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
  /* shared information from sbl */
  struct ddr_shared_params *sbl_shared_info;
  
  /* Railway voter variables */
  const int cx_rail_id = rail_id("vddcx");
  const int mx_rail_id = rail_id("vddmx");
   
  ddr_status.vddmx_voltage = VDDMX_VOLTAGE_INIT_IN_UV;

  /* Register ddr driver for railway callback */
  railway_set_callback(cx_rail_id,RAILWAY_PRECHANGE_CB, ddr_pre_vddcx_switch,NULL);
  railway_set_callback(cx_rail_id,RAILWAY_POSTCHANGE_CB, ddr_post_vddcx_switch,NULL);
  railway_set_callback(mx_rail_id,RAILWAY_PRECHANGE_CB, ddr_pre_vddmx_switch,NULL);
  railway_set_callback(mx_rail_id,RAILWAY_POSTCHANGE_CB, ddr_post_vddmx_switch,NULL);
  
  /* initialize function table pointer */
  
  sbl_shared_info = ddr_get_shared_header();
  ddr_func_tbl = (ddr_func *)sbl_shared_info->ddr_func_tbl_ptr;
  hw_part_num = HWIO_INF(TLMM_HW_REVISION_NUMBER, PARTNUM);
  
  /* Eng change to test out function table and shared library */
//  HAL_SDRAM_Enter_Self_Refresh(DDR_BASE, SDRAM_INTERFACE_0, SDRAM_CS0, HAL_SDRAM_DDR_Wait);
//  HAL_SDRAM_Exit_Self_Refresh(DDR_BASE, SDRAM_INTERFACE_0, SDRAM_CS0, HAL_SDRAM_DDR_Wait);
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
    if (interface_name == SDRAM_INTERFACE_0)
    {
      /* Configure BIMC clock period */
      ddr_func_tbl->bimc_update_clock_period(DDR_BASE, new_clk, ddr_divide_func);

      /* Configure DPE before clock switch */
      ddr_func_tbl->bimc_dpe_pre_clock_switch(DDR_BASE, interface_name, curr_clk, new_clk);
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
  if (interface_name == SDRAM_INTERFACE_0)
  {
    /* Configure DPE before clock switch */
    ddr_func_tbl->bimc_dpe_post_clock_switch(DDR_BASE, interface_name, curr_clk, new_clk);
  }

} /* ddr_post_clock_switch */

void adjust_ddr_phy_config_vddmx(SDRAM_INTERFACE interface, railway_corner mode)
{
	struct ddr_post_boot_setting_params *temp;
	temp=ddr_get_post_boot_params(interface);
    if(temp!=NULL&&mode >= RAILWAY_TURBO && mode < RAILWAY_CORNERS_COUNT)
      ddr_phy_config_vddmx[mode] =temp->dq_write_cdc_delay;
}
/* =============================================================================
**  Function : ddr_pre_vddmx_switch
** =============================================================================
*/
/**
*   @brief
*   Function called right before vddmx switch.
*
*   @param[in]  voltage  New vddmx voltage (in uV)
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
void ddr_pre_vddmx_switch(const railway_settings *proposal, void * callback_cookie)
{
  if (ddr_status.sdram0_cs0 == DDR_ACTIVE ||
      ddr_status.sdram0_cs1 == DDR_ACTIVE)
  {
    if (proposal->microvolts < ddr_status.vddmx_voltage)
    {
	  /*post boot enhance set*/
	  adjust_ddr_phy_config_vddmx(SDRAM_INTERFACE_0, proposal->mode);
      /* Update CDC configuration according to voltage */
      ddr_func_tbl->phy_update_cdc_config(DDR_BASE, SDRAM_INTERFACE_0, ddr_phy_config_vddmx[proposal->mode]);
	  
	  if ((hw_part_num == HW_PART_NUM_8208) && (proposal->microvolts > 1225000))
        ddr_func_tbl->phy_update_cdc_delay(DDR_BASE, SDRAM_INTERFACE_0, FALSE, 0x271, 1225000);
      else
        ddr_func_tbl->phy_update_cdc_delay(DDR_BASE, SDRAM_INTERFACE_0, FALSE, 0x271, proposal->microvolts);

      /* Issue manual CDCCAL */
      ddr_func_tbl->phy_manual_cdc_cal(DDR_BASE, SDRAM_INTERFACE_0);
    }

    /* Enable rapid CDCCAL */
    ddr_func_tbl->phy_enable_rapid_cdc_cal(DDR_BASE, SDRAM_INTERFACE_0);
  }

} /* ddr_pre_vddmx_switch */

/* =============================================================================
**  Function : ddr_post_vddmx_switch
** =============================================================================
*/
/**
*   @brief
*   Function called right after vddmx switch.
*
*   @param[in]  voltage  New vddmx voltage (in uV)
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
void ddr_post_vddmx_switch(const railway_settings *proposal, void * callback_cookie)
{
  if (ddr_status.sdram0_cs0 == DDR_ACTIVE ||
      ddr_status.sdram0_cs1 == DDR_ACTIVE)
  {
    /* Disable rapid CDCCAL */
    ddr_func_tbl->phy_disable_rapid_cdc_cal(DDR_BASE, SDRAM_INTERFACE_0);

    if (proposal->microvolts > ddr_status.vddmx_voltage)
    {
	  adjust_ddr_phy_config_vddmx(SDRAM_INTERFACE_0, proposal->mode);
      /* Update CDC configuration according to voltage */
      ddr_func_tbl->phy_update_cdc_config(DDR_BASE, SDRAM_INTERFACE_0, ddr_phy_config_vddmx[proposal->mode]);
	  
	  if ((hw_part_num == HW_PART_NUM_8208) && (proposal->microvolts > 1225000))
        ddr_func_tbl->phy_update_cdc_delay(DDR_BASE, SDRAM_INTERFACE_0, FALSE, 0x271, 1225000);
      else
        ddr_func_tbl->phy_update_cdc_delay(DDR_BASE, SDRAM_INTERFACE_0, FALSE, 0x271, proposal->microvolts);

      /* Issue manual CDCCAL */
      ddr_func_tbl->phy_manual_cdc_cal(DDR_BASE, SDRAM_INTERFACE_0);
    }
  }

  /* Update VDDMX voltage */
  ddr_status.vddmx_voltage = proposal->microvolts;

} /* ddr_post_vddmx_switch */

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
  /* Put DDR into self refresh */
  ddr_enter_self_refresh_all(clk_speed);

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
    HAL_SDRAM_DDR_Wait(1, 0);

    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_CONFIG), SDRAM_0_OFFSET,
                   rank_init_fmsk, shke_config);

    /* Put DDR into self refresh with EBI1 clamped to restore controller state */
    ddr_func_tbl->enter_self_refresh(DDR_BASE, SDRAM_INTERFACE_0, chip_select, HAL_SDRAM_DDR_Wait);
  }

  /* Disable EBI1 clamps via freeze_io */
  mpm_config_ebi1_freeze_io(FALSE);

  /* Take DDR out of self refresh */
  ddr_exit_self_refresh_all(clk_speed);

} /* ddr_exit_power_collapse */

void ddr_pre_vddcx_switch(const railway_settings *proposal, void * callback_cookie) {}
void ddr_post_vddcx_switch(const railway_settings *proposal, void * callback_cookie) {}



/* =============================================================================
**  Function : ddr_get_post_boot_params
** =============================================================================
*/
/**
*   @brief
*   Get post boot ddr parameter address in IMEM ,enhance set
*
*   @param[in]  SDRAM_INTERFACE
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
struct ddr_post_boot_setting_params *ddr_get_post_boot_params(SDRAM_INTERFACE interface_name)
{
  struct ddr_post_boot_setting_params *post_boot_params_ptr=NULL;
  union ddr_device_params *device_table;
  struct ddr_post_boot_set_header_v1 *post_boot_head_ptr;
  uint32 device_AC_params_size;

  device_table =ddr_get_params(SDRAM_INTERFACE_0);
	 
  switch (device_table[0].common.device_type)
  {
      case DDR_TYPE_LPDDR2:
      case DDR_TYPE_LPDDR3:
           device_AC_params_size = sizeof(struct ddr_device_params_lpddr);
           break;

      case DDR_TYPE_PCDDR3:
           device_AC_params_size = sizeof(struct ddr_device_params_pcddr3);
           break;

      default:
           device_AC_params_size = 0;
           break;
  }
  post_boot_head_ptr=(struct ddr_post_boot_set_header_v1*)((uint32)device_table+device_AC_params_size*2);
  if(ddr_get_training_cookie()==DDR_PARAMS_MAGIC_NUM)
  {
      post_boot_head_ptr=(struct ddr_post_boot_set_header_v1 *)((uint32)post_boot_head_ptr+sizeof(struct ddr_params_partition));
  }
  do
  {     
     if(post_boot_head_ptr->magic_number!=DDR_POST_BOOT_SET_MAGIC_NUM)
     {
         break;
     }
	 post_boot_params_ptr=(struct ddr_post_boot_setting_params *)((uint32)post_boot_head_ptr+sizeof(struct ddr_post_boot_set_header_v1));	  
	  
  }while(0);

  return post_boot_params_ptr;
}
