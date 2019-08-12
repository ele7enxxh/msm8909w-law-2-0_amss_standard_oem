/**
 * @file ddr_target.c
 * @brief
 * Target specific DDR drivers.
 */
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/hw/msm8909/ddr_target.c#2 $
$DateTime: 2015/12/15 20:50:37 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/16/15   aus     Update CA CDC delay at 533 MHz
05/27/15   aus     Update WR CDC delay and CA CDC delay for 533 and 400
01/22/15   yps     Use memsmove instead of memmove
12/15/14   xl      Add ddr parameters enhance set
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
                      Copyright 2013-2014, 2015 Qualcomm Technologies Incorporated
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
#include "mpm_hwio.h"
#include <stringl/stringl.h>

/*==============================================================================
                                  DATA
==============================================================================*/
/* DDR logging level */
enum ddr_log_level ddr_log_level = DDR_ERROR;

/* Initial DDR PHY CDC delay */
uint32 DDR_PHY_INIT_CDC_DELAY;

__attribute__((section("ddr_func_ptr"))) ddr_func ddr_func_tbl;

boolean wdog_reset = FALSE;

uint32 ddr_params_set_enhance_whitelist[][2] =
{
  {HWIO_ADDR(PHY_CA_ADDR(PAD_CFG0)), 0x00330070},
  {HWIO_ADDR(PHY_DQ_ADDR(PAD_CFG0)), 0x00330070},
  {HWIO_ADDR(PHY_DQ_ADDR(PAD_CFG1)), 0x00330070},
  {HWIO_ADDR(PHY_CA_ADDR(CDC_DELAY_CFG)), 0x00000FFF},
  {HWIO_ADDR(PHY_DQ_ADDR(RD_CDC_DELAY_CFG)), 0x00000FFF},
  {HWIO_ADDR(PHY_DQ_ADDR(WR_CDC_DELAY_CFG)), 0x00000FFF},
  {0x0, 0x0}	
};


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
**  Function : HAL_SDRAM_PHY_Update_CDC_Delay_Target
** =============================================================================
*/
/**
*   @brief
*   Update CDC delay.
*
*   @param[in]  interface  Interface to update CDC delay for
*   @param[in]  read       TRUE for read CDC and FALSE for write CDC
*   @param[in]  cdc_delay  CDC delay value
*   @Param[in]  voltage    voltage level
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
void HAL_SDRAM_PHY_Update_CDC_Delay_Target(uint32 base, SDRAM_INTERFACE interface, boolean read, uint32 cdc_delay, uint32 voltage)
{
  uint32 phy_offset;
  uint32 dq;
  uint32 offset;

  /* PHY offset */
  phy_offset = base + ((interface == SDRAM_INTERFACE_0) ? PHY_0_OFFSET : PHY_1_OFFSET);

  if (read)
  {
    for (dq = 0; dq < PHY_DQ_MAX_NUM; dq++)
    {
      /* Offset for each DQ PHY */
      offset = phy_offset + PHY_DQ1_OFFSET * dq;

      /* Update DQ read CDC delay */
      BOOT_HWIO_OUTM(PHY_DQ_ADDR(RD_CDC_DELAY_CFG), offset,
                     HWIO_FMSK(PHY_DQ_ADDR(RD_CDC_DELAY_CFG), DELAY_VAL),
                     cdc_delay << HWIO_SHFT(PHY_DQ_ADDR(RD_CDC_DELAY_CFG), DELAY_VAL));
    }
  }
  else
  {
    for (dq = 0; dq < PHY_DQ_MAX_NUM; dq++)
    {
       /* Offset for each DQ PHY */
       offset = phy_offset + PHY_DQ1_OFFSET * dq;
       /* WR CDC delay for DQ1 and DQ2 to 300ps @ 533MHz */
       if ((dq==1 || dq==2) && (voltage > 1225000)) {
         BOOT_HWIO_OUTM(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), offset,
                        HWIO_FMSK(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), DELAY_VAL),
                        0x12C << HWIO_SHFT(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), DELAY_VAL));

       }
       /* WR CDC delay for all DQs to 400ps @ 400MHz */
       else if (voltage > 1050000 && voltage <= 1225000) {
         /* Update DQ write CDC delay */
         BOOT_HWIO_OUTM(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), offset,
                        HWIO_FMSK(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), DELAY_VAL),
                        0x190 << HWIO_SHFT(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), DELAY_VAL));
         /* Update DQ read CDC delay */
         BOOT_HWIO_OUTM(PHY_DQ_ADDR(RD_CDC_DELAY_CFG), offset,
                        HWIO_FMSK(PHY_DQ_ADDR(RD_CDC_DELAY_CFG), DELAY_VAL),
                        0x271 << HWIO_SHFT(PHY_DQ_ADDR(RD_CDC_DELAY_CFG), DELAY_VAL));
       }
       else {
         /* Update DQ write CDC delay */
         BOOT_HWIO_OUTM(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), offset,
                        HWIO_FMSK(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), DELAY_VAL),
                        cdc_delay << HWIO_SHFT(PHY_DQ_ADDR(WR_CDC_DELAY_CFG), DELAY_VAL));
       }
     }
  }

  /* Offset for CA Master PHY */
  offset = phy_offset + PHY_CA_MST_OFFSET;
  /* CA CDC delay to 469ps @ 533MHz */
  if (voltage > 1225000) {
    /* Update CA CDC delay */
    BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_DELAY_CFG), offset,
                HWIO_FMSK(PHY_CA_ADDR(CDC_DELAY_CFG), DELAY_VAL),
                0x1D5 << HWIO_SHFT(PHY_CA_ADDR(CDC_DELAY_CFG), DELAY_VAL));
  }
  /* CA CDC delay to 400ps @ 400MHz */
  else if (voltage > 1050000 && voltage <= 1225000) {
    /* Update CA CDC delay */
    BOOT_HWIO_OUTM(PHY_CA_ADDR(CDC_DELAY_CFG), offset,
                HWIO_FMSK(PHY_CA_ADDR(CDC_DELAY_CFG), DELAY_VAL),
                0x190 << HWIO_SHFT(PHY_CA_ADDR(CDC_DELAY_CFG), DELAY_VAL));
  }
  
} /* HAL_SDRAM_PHY_Update_CDC_Delay_Target */


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
  
  extern uint32 ddr_bimc_config_MSM8x09_LPDDR3[][2];
  extern uint32 ddr_phy_config_MSM8x09_LPDDR3[][2];
  extern uint32 ddr_caphy_config_MSM8x09_LPDDR3[][2];
  extern uint32 ddr_dqphy_config_MSM8x09_LPDDR3[][2];
  
  //if (DalChipInfo_ChipFamily() == DALCHIPINFO_ID_MSM8916)
  {
    if (ddr_get_params(SDRAM_INTERFACE_0)->common.device_type == DDR_TYPE_LPDDR3)
    {
      ddr_bimc_config_delta = ddr_bimc_config_MSM8x09_LPDDR3;
      ddr_phy_config_delta = ddr_phy_config_MSM8x09_LPDDR3;
      ddr_caphy_config_delta = ddr_caphy_config_MSM8x09_LPDDR3;
      ddr_dqphy_config_delta = ddr_dqphy_config_MSM8x09_LPDDR3;
    }
    DDR_PHY_INIT_CDC_DELAY = 0x271;
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
  ddr_func_tbl.phy_update_cdc_delay = (void (*)(uint32 , SDRAM_INTERFACE , boolean read, uint32 cdc_delay, uint32 voltage)) 
                                      ((uint8 *)&HAL_SDRAM_PHY_Update_CDC_Delay_Target - SCL_RPM_CODE_RAM_BASE);
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
	boolean status = FALSE;
	struct ddr_boot_setting_params *ddr_boot_setting_ptr,*ddr_setting_current_ptr;
	uint32 counter=0;
	uint32 device_params_size=0;

	void *ddr_imem_top_addr;
	
		
	do
	{
		if(((struct ddr_boot_set_header_v1*)ddr_params_ptr)->magic_number==DDR_BOOT_SET_MAGIC_NUM)
		{     

		    if((ddr_params_size==sizeof(struct ddr_boot_set_header_v1))||( ((struct ddr_boot_set_header_v1*)ddr_params_ptr)->num_of_groups==0))
		    {
                 break;
			}
			if(((((struct ddr_boot_set_header_v1*)ddr_params_ptr)->num_of_groups)*sizeof(struct ddr_boot_setting_params)+sizeof(struct ddr_boot_set_header_v1))!=ddr_params_size)
			{
				break;
			}
			ddr_boot_setting_ptr=(struct ddr_boot_setting_params*)((uint32)ddr_params_ptr+sizeof(struct ddr_boot_set_header_v1));
			
			for(ddr_setting_current_ptr=ddr_boot_setting_ptr;((uint32)ddr_setting_current_ptr)<(uint32)ddr_boot_setting_ptr+ddr_params_size;ddr_setting_current_ptr++)
			{ 
				   for(counter=0; ddr_params_set_enhance_whitelist[counter][0]!=0;counter++)
				   {
					if(ddr_setting_current_ptr->address==ddr_params_set_enhance_whitelist[counter][0])
						out_dword_masked_ns(ddr_setting_current_ptr->address,(ddr_setting_current_ptr->mask)&ddr_params_set_enhance_whitelist[counter][1],ddr_setting_current_ptr->value,in_dword(ddr_setting_current_ptr->address) );
				   }
		  
			}
			/* Issue manual IO CAL */
			HAL_SDRAM_PHY_Manual_IO_Cal(0, SDRAM_INTERFACE_0, HAL_SDRAM_DDR_Wait);			
         		/* Issue manual CDC CAL */
			HAL_SDRAM_PHY_Manual_CDC_Cal(0, SDRAM_INTERFACE_0);
			status = TRUE;
			break;
		}
		else if(((struct ddr_post_boot_set_header_v1*)ddr_params_ptr)->magic_number==DDR_POST_BOOT_SET_MAGIC_NUM)
		{

            if((sizeof(struct ddr_post_boot_set_header_v1*)==ddr_params_size) || (((struct ddr_post_boot_set_header_v1*)ddr_params_ptr)->size_of_param==0))
            {
				break;
			}
			if((((struct ddr_post_boot_set_header_v1*)ddr_params_ptr)->size_of_param)!=sizeof(struct ddr_post_boot_setting_params))
			{
				break;
			}			
			
			/* Get size of device table entry */
			switch (ddr_get_params(SDRAM_INTERFACE_0)->common.device_type)
			{
			     case DDR_TYPE_LPDDR2:
			     case DDR_TYPE_LPDDR3:
				       device_params_size = sizeof(struct ddr_device_params_lpddr);
				       break;

			     case DDR_TYPE_PCDDR3:
				       device_params_size = sizeof(struct ddr_device_params_pcddr3);
				       break;
			      default:
				       device_params_size = 0;
					break;
			}

			/* DDR parameter partition is after two device table entries */
			ddr_imem_top_addr =(void*)  ((uint32)ddr_get_params(SDRAM_INTERFACE_0) + device_params_size * 2);
			
			if(ddr_params_is_training_required())
			{
             	ddr_imem_top_addr =(void*)((uint32)ddr_imem_top_addr+sizeof(struct ddr_params_partition));				
			}
			memsmove((void*)ddr_imem_top_addr,ddr_params_size,(void*)ddr_params_ptr,ddr_params_size);		
			status = TRUE;
			break;
		}
		else
		{
			break;
		}

	}	
 	while(0);
	return status;
}
