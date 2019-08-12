/**********************************************************************
* sdcc_bsp_boot.c
*
* SDCC(Secure Digital Card Controller) driver BSP.
*
* This file implements the SDCC driver BSP for the board in use for BOOT.
*
* Copyright (c) 2009-2015 by Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
*
**********************************************************************/

/*=======================================================================
                        Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/sdcc/src/bsp/9x45/sdcc_bsp_boot.c#1 $
$DateTime: 2015/09/01 00:30:35 $

YYYY-MM-DD   who     what, where, why
----------------------------------------------------------------------
2015-01-28   sb      Fix Klocwork errors
2014-09-24   sb      Add GPIO config for 9x45
2014-07-11   sb      Updating to support 9x45
2014-05-16   sb      Non-eMMC slots support up to 200MHz on 8916
2014-05-01   sb      Maximum apps clock support for SDC1 is 177MHz on 8916 
2014-03-13   bn      Added support for 192MHz apps clock. Add busywait init for mjsdload
2014-03-11   bn      Enabled slot type query. Enabled cache op on non-L2 memory
2013-11-05   pk      Add target specific changes for 8916
2013-10-03   bn      Enable support for memory_barrier
2013-08-21   bn      Code clean up. Avoid hard coded drive number
2013-08-12   bn      Code clean up
2013-05-14   bb      Enable 8-bit mode for eMMC
2013-04-03   bb      Remove CLOCK_DRIVER_NOT_READY flag
2012-07-31   vk      Use ClockBoot APIs for frequency setting
2012-06-22   vk      Use Dalsys_busywait instead of software polling
2012-05-25   vk      Enable H clock for SDC3
2012-02-10   rh      Update the HWIO definition file
2011-12-20   rh      Adding the cache flush routine
2011-09-13   rh      Mark SDC3 as an embedded slot
2011-03-22   vin     Mark SDC3 as an open slot
2010-12-10   rh      Temporarily add RUMI support - to be removed later
2010-12-08   rh      SDC1 port should use special TLMM/GPIO registers
2010-12-08   rh      Use APCS_DGT instead of SCSS DGT register
2010-08-12   rh      Switching to DGT timer by the request of the RF/PMIC team
2010-07-02   vin     Added sdcc_bsp_clk_type
2010-06-16   vj      Added sdcc_bsp_get_max_supported_clk
2010-06-11   rh      Mark the third slot as embedded slot type
2010-05-25   rh      Mark the first slot as embedded slot type
2010-04-28   rh      device change to device_id in sdcc_bsp_dmov_properties
2010-04-14   rh      Use the Scorpion delay counter for SSBL and SPBL
2010-03-18   rh      Changes require for SURF bringup
2009-12-04   rh      Initial version adapted from 7x30
===========================================================================*/

/*==============================================================================
  Includes for this module.
  ----------------------------------------------------------------------------*/
#include "sdcc_bsp.h"
#include "sdcc_priv.h"
#include SDCC_OSAL_H
#include SDCC_HWIO_H
#include "ClockBoot.h"
#include "busywait.h"
#include "boot_cache_mmu.h"

/*==============================================================================
                           S D C C    B S P
==============================================================================*/

/* Max number of slots supported by the Board */
#define SDCC_BSP_NUM_SLOTS       1
/* Used for slot number validation*/
#define SDCC_BSP_INVALID_SLOT    SDCC_BSP_NUM_SLOTS

/* There are currently no designated slots for WLAN,UBM & CMMB*/
#define SDCC_BSP_WLAN_SLOT_DEFAULT   SDCC_BSP_INVALID_SLOT
#define SDCC_BSP_UBM_SLOT_DEFAULT    SDCC_BSP_INVALID_SLOT
#define SDCC_BSP_CMMB_SLOT_DEFAULT   SDCC_BSP_INVALID_SLOT

/* MPM Counter Control Register. Used for busywait init in mjsdload build */
#define BUSYWAIT_MPM2_MPM_CONTROL_CNTCR 0x004A1000

#define SDCC_APCS_GLB_QGIC_CFG 0xB01002C

#define SDCC_TLMM_QDSD_BOOT_CTL 0x119D000

/* Cache line size */
#define SDCC_BSP_CACHE_LINE_SIZE   32

/* Slot configuration information for the board */
static sdcc_bsp_slot_type sdcc_bsp_slot_config [SDCC_MAX_NUM_CONTROLLER] = {
   SDCC_BSP_EMBEDDED_MEM_SLOT|SDCC_BSP_8BIT_SLOT,
   SDCC_BSP_NO_SLOT,
   SDCC_BSP_NO_SLOT
};

/* Slot internal or external configuration information */
static sdcc_bsp_slot_access_type sdcc_bsp_slot_access_config [SDCC_MAX_NUM_CONTROLLER] = {
   SDCC_BSP_SLOT_INTERNAL,
   SDCC_BSP_SLOT_INVALID,
   SDCC_BSP_SLOT_INVALID
};

#define GPIO_DRIVE_2MA        0x0
#define GPIO_DRIVE_4MA        0x1
#define GPIO_DRIVE_6MA        0x2
#define GPIO_DRIVE_8MA        0x3
#define GPIO_DRIVE_10MA       0x4
#define GPIO_DRIVE_12MA       0x5
#define GPIO_DRIVE_14MA       0x6
#define GPIO_DRIVE_16MA       0x7

#define GPIO_NO_PULL          0x0
#define GPIO_PULL_DOWN        0x1
#define GPIO_KEEPER           0x2
#define GPIO_PULL_UP          0x3

#define HWIO_GCC_SDCCn_BCR_OUT(n, v)    \
        out_dword(HWIO_GCC_SDCC1_BCR_ADDR + (0x40 * (n)), v)

#define SDCC_GPIO_CONFIG( gpio_num, gpio_func, gpio_drv, gpio_pull ) \
HWIO_OUTI( TLMM_GPIO_CFGn, gpio_num, ( (gpio_func) << HWIO_SHFT( TLMM_GPIO_CFGn, FUNC_SEL ) ) | \
( (gpio_drv) << HWIO_SHFT( TLMM_GPIO_CFGn, DRV_STRENGTH ) ) | \
( (gpio_pull) << HWIO_SHFT( TLMM_GPIO_CFGn, GPIO_PULL ) ) )

/******************************************************************************
* Description:
*    Sets the SDC controller clock speed based on the input parameter.
*
* Arguments:
*    driveno        [IN] : drive number
*    speed          [IN/OUT] : clock speed
*    mode           [IN] : which clock branch to adjust
******************************************************************************/
void sdcc_bsp_config_clock (uint32 driveno, uint32 *speed, uint32 mode)
{
  ClockSDCType clk;
  ClockBootPerfLevelType level;

  if (mode == SDCC_BSP_CLK_CONFIG_HCLK)
  {
    /* Clocks for eMMC are turned on in PBL */
    /* Turn on the clocks for external SD */
    /* External SD is the SDCC_BSP_OPEN_SLOT as set in sdcc_bsp_slot_config */
    if(SDCC_BSP_OPEN_SLOT == sdcc_bsp_slot_config [driveno])
    {
      switch(driveno)
      {
        case 0:
          HWIO_OUTF(GCC_SDCC1_AHB_CBCR, CLK_ENABLE, driveno);
          break;

        default:
          break;
      }
    }

    return;
  }

  if (mode == SDCC_BSP_CLK_CONFIG_RESET)
  {
    HWIO_GCC_SDCCn_BCR_OUT (driveno, HWIO_GCC_SDCC1_BCR_BLK_ARES_BMSK);
    sdcc_bsp_hw_clk_busy_wait (500);
    HWIO_GCC_SDCCn_BCR_OUT (driveno, 0);
    return;
  }

  switch(driveno)
  {
    case 0:
      clk = CLK_SDC1;
      break;
    case 1:
      clk = CLK_SDC2;
      break;
    default:
      clk = CLK_SDC_NONE;
  }

  /* CLOCK_BOOT_PERF_MIN,   400 kHz
     CLOCK_BOOT_PERF_NOMINAL,  25 MHz
     CLOCK_BOOT_PERF_MAX,   50 MHz  */

  level = CLOCK_BOOT_PERF_MIN;

  if(speed != NULL)
  {
    if (*speed <= 400)
    {
      level = CLOCK_BOOT_PERF_MIN;
      Clock_SetSDCClockFrequency (level, clk);
    }
    else if (*speed <= 26000)
    {
      level = CLOCK_BOOT_PERF_NOMINAL;
      Clock_SetSDCClockFrequency (level, clk);
    }
    else if (*speed <= 52000)
    {
      level = CLOCK_BOOT_PERF_MAX;
      Clock_SetSDCClockFrequency (level, clk);
    }
    else if (*speed <= 100000)
    {
      /* To get 100MHz and 192MHz, a different CLK API is used */
      Clock_SetSDCClockFrequencyExt (100000, clk);
    }   
    else /* 192MHz */
    {
      if (0 == driveno)
      {
        /* On 8916/36, 177MHz is only supported on eMMC (SDC1) slot */
        Clock_SetSDCClockFrequencyExt (177000, CLK_SDC1);
    }
    else
    {
        /* Other non-eMMC slots support up to 200MHz */
        Clock_SetSDCClockFrequencyExt (200000, clk);
      }
    }
  }

  return;

} /* sdcc_bsp_config_clock */

/******************************************************************************
* Name: sdcc_hw_clk_busy_wait_init
*
* Description:
*    This function provides initialization for a busy wait.
*
* Arguments:
*    None
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_bsp_hw_busy_wait_init(void)
{
/* busywait is initialized by PBL. In mjsdload tool, make sure busywait is initialized */
#if defined(FEATURE_SDCC_FOR_TOOLS_ENV)
   busywait_init();

   /* Make sure MPM Timer is enabled */
   out_dword (BUSYWAIT_MPM2_MPM_CONTROL_CNTCR, 0x1);
   
   out_dword(SDCC_APCS_GLB_QGIC_CFG,0x100);
#endif     
}

/******************************************************************************
* Name: sdcc_hw_clk_busy_wait
*
* Description:
*    This function provides a busy wait for the caller.
*
* Arguments:
*    us              [IN] : time in micro seconds
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_bsp_hw_clk_busy_wait(uint32 us)
{
   busywait(us);
}

/*=============================================================================
 * FUNCTION      sdcc_bsp_get_slot_type
 *
 * DESCRIPTION   This function queries the slot to find out if the slot is designated
 *               for eMMC, SD or no device.
 *
 * PARAMETERS    [IN]: slot number
 *
 * RETURN VALUE  sdcc_bsp_slot_type
 *
 *===========================================================================*/
sdcc_bsp_slot_type
sdcc_bsp_get_slot_type (uint32 slot_num)
{
   sdcc_bsp_slot_type slot_type = SDCC_BSP_NO_SLOT;

   if (slot_num >= SDCC_BSP_NUM_SLOTS)
   {
      /* Return No slot available*/
      slot_type = SDCC_BSP_NO_SLOT;
   }
   else
   {
      slot_type = sdcc_bsp_slot_config [slot_num];
   }

   return slot_type;
}

/*=============================================================================
 * FUNCTION      sdcc_bsp_get_slot_access_type
 *
 * DESCRIPTION   This function queries the slot to find out if the slot is designated
 *               as internal or external memory device. Hotplug needs this info
 *               to determine which memory device to boot from.
 *
 * PARAMETERS    [IN]: slot number
 *
 * RETURN VALUE  sdcc_bsp_slot_access_type
 *
 *===========================================================================*/
sdcc_bsp_slot_access_type
sdcc_bsp_get_slot_access_type (uint32 slot_num)
{
   sdcc_bsp_slot_access_type slot_access_type = SDCC_BSP_NO_SLOT;

   if (slot_num >= SDCC_BSP_NUM_SLOTS)
   {
      /* Return No slot available*/
      slot_access_type = SDCC_BSP_NO_SLOT;
   }
   else
   {
      slot_access_type = sdcc_bsp_slot_access_config [slot_num];
   }

   return slot_access_type;
}

sdcc_bsp_err_type
sdcc_bsp_vdd_control (uint32 slot_num, uint32 state)
{
   return SDCC_BSP_NO_ERROR;
}

sdcc_bsp_err_type
sdcc_bsp_vdd_configure (uint32 slot_num)
{
   (void) slot_num;

   /* Place holder for sdcc vdd config */
   return SDCC_BSP_NO_ERROR;
}

/*=============================================================================
 * FUNCTION      sdcc_bsp_gpio_control
 *
 * DESCRIPTION   This function enables/disables the GPIO interface to the
 *               card slot.
 *
 * PARAMETERS    [IN]: slot number
 *               [IN]: GPIO ON/OFF
 *
 * RETURN VALUE  sdcc_bsp_err_type.
 *
 *===========================================================================*/
sdcc_bsp_err_type
sdcc_bsp_gpio_control (uint32 slot_num, uint32 state)
{
   uint32 i = 0;
   uint32 reg_value = 0;
   
   (void)i;

   if(slot_num >= SDCC_BSP_NUM_SLOTS)
   {
      return SDCC_BSP_ERR_INVALID_PARAM;
   }

   if(SDCC_BSP_NO_SLOT == sdcc_bsp_slot_config [slot_num])
   {
      return SDCC_BSP_ERR_INVALID_PARAM;
   }

   if (slot_num == SDCC_BSP_SLOT_0)
   {
     /* Config TLMM QDSD register */
     reg_value = in_dword(SDCC_TLMM_QDSD_BOOT_CTL);
     reg_value |= 0x1;
     out_dword(SDCC_TLMM_QDSD_BOOT_CTL, reg_value);      

     HWIO_OUTM( TLMM_SDC1_HDRV_PULL_CTL,
              ( HWIO_FMSK(TLMM_SDC1_HDRV_PULL_CTL, SDC1_CMD_PULL)  |
                HWIO_FMSK(TLMM_SDC1_HDRV_PULL_CTL, SDC1_DATA_PULL) |
                HWIO_FMSK(TLMM_SDC1_HDRV_PULL_CTL, SDC1_CLK_HDRV)  |
                HWIO_FMSK(TLMM_SDC1_HDRV_PULL_CTL, SDC1_CMD_HDRV)  |
                HWIO_FMSK(TLMM_SDC1_HDRV_PULL_CTL, SDC1_DATA_HDRV) ),
              ( ( GPIO_PULL_UP << HWIO_SHFT(TLMM_SDC1_HDRV_PULL_CTL, SDC1_CMD_PULL) ) |
                ( GPIO_PULL_UP << HWIO_SHFT(TLMM_SDC1_HDRV_PULL_CTL, SDC1_DATA_PULL) ) |
                ( GPIO_DRIVE_8MA << HWIO_SHFT(TLMM_SDC1_HDRV_PULL_CTL, SDC1_CLK_HDRV) ) |
                ( GPIO_DRIVE_8MA << HWIO_SHFT(TLMM_SDC1_HDRV_PULL_CTL, SDC1_CMD_HDRV) ) |
                ( GPIO_DRIVE_12MA << HWIO_SHFT(TLMM_SDC1_HDRV_PULL_CTL, SDC1_DATA_HDRV) ) ) );

     /* SDC_DATA_OUT_4 */
     SDCC_GPIO_CONFIG( 96, 0, GPIO_DRIVE_12MA, GPIO_PULL_UP );   
     /* SDC_DATA_OUT_5 */
     SDCC_GPIO_CONFIG( 97, 0, GPIO_DRIVE_12MA, GPIO_PULL_UP );   
     /* SDC_DATA_OUT_6 */
     SDCC_GPIO_CONFIG( 98, 0, GPIO_DRIVE_12MA, GPIO_PULL_UP );   
     /* SDC_DATA_OUT_7 */
     SDCC_GPIO_CONFIG( 99, 0, GPIO_DRIVE_12MA, GPIO_PULL_UP );  

     /* select eMMC boot */
     HWIO_OUT(TLMM_EBI2_EMMC_GPIO_CFG, 0x4) ;
   }
 
   return SDCC_BSP_NO_ERROR;
}

/******************************************************************************
* FUNCTION      sdcc_bsp_get_default_slot
*
* DESCRIPTION   SDCC would call this function to determine the
*               default slot for a given application.
*
* PARAMETERS    [IN]: slot type indicating the intended application
*
* RETURN VALUE  slot number
*
******************************************************************************/
uint32 sdcc_bsp_get_default_slot(sdcc_bsp_slot_type app)
{
   switch(app)
   {
   case SDCC_BSP_WLAN_SLOT:
      return SDCC_BSP_WLAN_SLOT_DEFAULT;
   case SDCC_BSP_UBM_SLOT:
      return SDCC_BSP_UBM_SLOT_DEFAULT;
   case SDCC_BSP_CMMB_SLOT:
      return SDCC_BSP_CMMB_SLOT_DEFAULT;
   default:
      return SDCC_BSP_INVALID_SLOT;
   }
}

/******************************************************************************
* FUNCTION      sdcc_bsp_get_cache_align_size
*
* DESCRIPTION   Function to get the cache alignment size.
*
* PARAMETERS    None
*
* RETURN VALUE  cache alignment size
*
******************************************************************************/
uint32 sdcc_bsp_get_cache_align_size(void)
{
   return SDCC_BSP_CACHE_LINE_SIZE;
}

/*=============================================================================
* FUNCTION      sdcc_bsp_set_vreg_level
*
* DESCRIPTION   Set the VREG voltage level
*
* PARAMETERS    [IN]: Voltage to set the VREG to
*
* RETURN VALUE  None
*
*===========================================================================*/
void sdcc_bsp_set_vreg_level(uint32 voltageSelection)
{
   (void) voltageSelection;
}

/******************************************************************************
* FUNCTION      sdcc_bsp_cache_operation
*
* DESCRIPTION   Performs cache operations on the provided memory region.
*               Used for DMOV related operations only.
*               Clean operation is done for writes to basically push
*               data in the cache to the physical memory
*               Invalidate operation is done for reads to invalidate
*               a memory region in the cache
*
* PARAMETERS    [IN]: cache operation type
*               [IN]: start address of the memory region
*               [IN]: length of the memory region
*
* RETURN VALUE  None
*
******************************************************************************/
void sdcc_bsp_cache_operation (uint32 op, void *addr, uint32 length)
{
   if (op & SDCC_BSP_MEM_BARRIER)
   {
      __dmb(0xF);
   }

/* mjsdload tool does not have cache support */
#ifndef FEATURE_SDCC_FOR_TOOLS_ENV

   /* Only perform cache operations on memory outside of L2 cache range */
   /* Note: build/ms/<target>.builds file has the info on L2 cache addr and size */
   if (op & SDCC_BSP_CACHE_CLEAN)
   {
      if (((uint32)addr < 0x08000000) || ((uint32)addr >= 0x08080000))
      {	  
         dcache_flush_region(addr, length);
      }
   }

   if (op & SDCC_BSP_CACHE_INVAL)
   {
      if (((uint32)addr < 0x08000000) || ((uint32)addr >= 0x08080000))
      {	  
         dcache_flush_region(addr, length);
      }
   }

#endif
}

/*=============================================================================
* FUNCTION      sdcc_bsp_get_max_supported_clk
*
* DESCRIPTION   Get the maximum supported clock
*
* PARAMETERS    [IN]: drive number
*
* RETURN VALUE  Clock frequency supported in kHz
*===========================================================================*/
uint32 sdcc_bsp_get_max_supported_clk (uint32 driveno)
{
  if (0 == driveno)
  {
    /* On MSM8916/8936, 177MHz is only supported on eMMC (SDC1) slot */
    return 177000;
  }
  else
  {
    /* Other non-eMMC slots support up to 200MHz */
    return 200000;
  }
}
