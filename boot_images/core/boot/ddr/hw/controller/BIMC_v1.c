/*
===========================================================================

FILE:         BIMC_v1.c

DESCRIPTION:  
  This is the platform hardware abstraction layer implementation for 
  HSDDRX controller.

Copyright 2012-2013 by QUALCOMM Technologies, Inc.  All Rights Reserved.
===========================================================================

                             Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/hw/controller/BIMC_v1.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
11/28/14   aus     Added 6Gb support
05/27/14   sk      Added support for 512MB density based on CDT parameter read
02/24/14   tw      refactored code to avoid using divide when possible
                   use divide function pointer passed in when not possible
02/09/14   tw      added support for common ddr library
01/13/14   tw      Updated to support 1ps resolution for ddr timing
08/12/13   sl      Adjust RL/WL with frequency for LPDDR2.
05/13/13   sl      Updated delta table calls.
04/03/13   sl      Added Read Latency for 266MHz.
03/01/13   tw      Added calling delta table
11/13/12   tw      Re-enabled 800mhz, enable hw self refresh
11/13/12   sl      Corrected HAL_SDRAM_Get_Base_Addr().
11/02/12   sy      Back out 800Mhz
10/31/12   sl      Changed ddr_static_config_init() to ddr_set_config().
10/12/12   sl      Added parameter detection.
10/04/12   tw      SITE initialization JCPLL workaround
09/21/12   sl      Added BOOT_PRE_SILICON flag.
09/19/12   sl      Updated HAL_SDRAM_Enter/Exit_Self_Refresh().
09/17/12   sl      Updated HAL_SDRAM_RL/WL().
09/12/12   sl      Use macro-style DDR logging.
09/07/12   sl      Removed functions for HAL DDR status.
09/06/12   sl      Moved parameter tweaking functions to ddr_target.c.
08/30/12   sl      Reordered DDR initialization sequence
08/24/12   sl      Update for rank detection
08/17/12   sl      Removed function pointers
08/10/12   sl      Added HAL_SDRAM_PHY_Init() call
08/03/12   tw      Added IO Init and IO Calibration to init sequence
08/03/12   tw      Fixed incorrect base address calculation
08/01/12   tw      Added calls to ddr_config for one time settings
07/10/12   tw      Added api calls for training sequences
05/15/12   tw      Updated api name to Power_Down instead of powerdown
                   Added new bimc api for deep power down
01/19/12   tw      Initial revision
===========================================================================
*/


/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_params.h"
#include "ddr_internal.h"
#include "ddr_target.h"
#include "ddr_config.h"
#include "ddr_log.h"
#include "HAL_DDR.h"
#include "BIMC.h"
#include "DDR_PHY.h"
#include "SITE.h"
#include "busywait.h"

/*==============================================================================
                                   DATA
==============================================================================*/
extern boolean wdog_reset;

/*==============================================================================
                                  FUNCTIONS
==============================================================================*/

extern void ddr_busywait(uint32);

uint32 ddr_divide_func(uint32 a, uint32 b)
{
  uint32 quotient = 0;
  
  while(a >= b) 
 { 
  a=a-b; 
  quotient=quotient+1; 
 } 

  return quotient;
}

/* =============================================================================
**  Function : HAL_SDRAM_Init
** =============================================================================
*/
/**
*   @brief
*   Initialize DDR controller, as well as DDR device.
*
*   @param[in]  interface    Interface to initialize for
*   @param[in]  chip_select  Chip select to initialize for
*   @param[in]  clk_speed    Clock speed (in KHz) to initialize for
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
void HAL_SDRAM_Init(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint32 clk_speed)
{
  uint32 offset;
  struct ddr_device_params_common *ddr_params;
  uint32 shke_config;

  /* Channel offset */
  offset = (interface == SDRAM_INTERFACE_0) ? SDRAM_0_OFFSET : SDRAM_1_OFFSET;

  /* Get DDR device parameters */
  ddr_params = &(ddr_get_params(interface)->common);

  /* first check for driver compatibility */
  if(ddr_params->device_type != DDR_TYPE_LPDDR2 &&
     ddr_params->device_type != DDR_TYPE_LPDDR3 )
  {
    /* Unsupported DDR Type, log error and quit */
    DDR_LOG(DDR_ERROR, DDR_INIT, DDR_UNSUPPORTED_DEVICE);
    return;
  }

  /* for wdog reset path save the shke_config */
  shke_config = BOOT_HWIO_IN(DDR_ADDR(SHKE_CONFIG), offset);
  
  ddr_set_config(offset, ddr_bimc_config_base, ddr_bimc_config_delta);

  /* Disable auto refresh */
  HAL_SDRAM_SHKE_Disable_Auto_Refresh(0, interface, SDRAM_BOTH);
  /* Disable HW based self refresh */
  HAL_SDRAM_SHKE_Disable_HW_Self_Refresh(0, interface, SDRAM_BOTH);
  /* Disable auto ZQCAL */
  HAL_SDRAM_SHKE_Disable_ZQCAL(0, interface, SDRAM_BOTH);
  /* Disable auto SRR */
  HAL_SDRAM_SHKE_Disable_Periodic_SRR(0, interface, SDRAM_BOTH);
  /* Clear SHKE_CONFIG[RANK0/1_EN] and SHKE_CONFIG[RANK0/1_INITCOMPLETE] */
  BOOT_HWIO_OUTM(DDR_ADDR(SHKE_CONFIG), offset,
                 HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK0_EN) |
                 HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK1_EN) |
                 HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK0_INITCOMPLETE) |
                 HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK1_INITCOMPLETE),
                 0x0);

  /* Configure SHKE_CONFIG[RANK0/1_EN] */
  if (chip_select & SDRAM_CS0)
  {
    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_CONFIG), offset,
                   HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK0_EN),
                   0x1U << HWIO_SHFT(DDR_ADDR(SHKE_CONFIG), RANK0_EN));
  }
  if (chip_select & SDRAM_CS1)
  {
    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_CONFIG), offset,
                   HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK1_EN),
                   0x1U << HWIO_SHFT(DDR_ADDR(SHKE_CONFIG), RANK1_EN));
  }

  /* Configure BIMC clock period */
  HAL_SDRAM_BIMC_Update_Clock_Period(0, clk_speed, ddr_divide_func);

  /* Configure DPE timing */
  HAL_SDRAM_DPE_Update_AC_Parameters(0, interface, clk_speed);

#ifndef BOOT_PRE_SILICON
  /* Initialize DDR PHY */
  HAL_SDRAM_PHY_Init(0, interface, clk_speed);
#endif

  /* Initialize DDR device */
  
  /* detect if restore from wdog reset */
  if(BOOT_HWIO_INM(DDR_ADDR(SHKE_DRAM_STATUS), 
                   offset, 
                   HWIO_FMSK(DDR_ADDR(SHKE_DRAM_STATUS), WDOG_SELF_RFSH)) == 0)
  {
    HAL_SDRAM_SHKE_Device_Init(0, interface, chip_select, clk_speed);

#ifndef BOOT_PRE_SILICON
    /* Rank detection */
    chip_select = HAL_SDRAM_BIMC_Rank_Detection(interface);

    /* Size detection */
    HAL_SDRAM_Ram_Size_Detection(interface);

    /* Parameter detection */
    ddr_params_detection(interface);

    /* Re-configure DPE timing */
    HAL_SDRAM_DPE_Update_AC_Parameters(0, interface, clk_speed);

#endif
    
  }
  else
  {
    uint32 rank_init_fmsk;
    
    /* Mark ddr as initialized */

    BOOT_HWIO_OUT(DDR_ADDR(SHKE_CONFIG), 
                   offset, 
                   shke_config);

    /* Clear WAIT_TIMER_BEFORE_HW_CLEAR to 0 */
    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_1), 
                   offset, 
                   HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_BEFORE_HW_CLEAR), 
                   0x0 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_1), WAIT_TIMER_BEFORE_HW_CLEAR));     
                   
    /* Compute timing parameters in cycles */
    BOOT_HWIO_OUTM(DDR_ADDR(DPE_CONFIG_4), offset,
                   HWIO_FMSK(DDR_ADDR(DPE_CONFIG_4), RECALC_PS_PARAMS),
                   0x1U << HWIO_SHFT(DDR_ADDR(DPE_CONFIG_4), RECALC_PS_PARAMS));
                   
    /* Wait for computing done */
    while ( BOOT_HWIO_INM(DDR_ADDR(DPE_MEMC_STATUS_1), offset,
                          HWIO_FMSK(DDR_ADDR(DPE_MEMC_STATUS_1), CYC_CALC_VALID)) )
    {}

    /* Load timing registers */
    BOOT_HWIO_OUTM(DDR_ADDR(DPE_CONFIG_4), offset,
                   HWIO_FMSK(DDR_ADDR(DPE_CONFIG_4), LOAD_ALL_CONFIG),
                   0x1U << HWIO_SHFT(DDR_ADDR(DPE_CONFIG_4), LOAD_ALL_CONFIG));    
                   
    /* toggle for tREFI load */
    /* Field masks for SHKE_CONFIG[RANK0/1_INITCOMPLETE] */
    rank_init_fmsk = HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK0_INITCOMPLETE) |
                   HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK1_INITCOMPLETE);

    /* Restore SHKE_CONFIG expect SHKE_CONFIG[RANK0/1_INITCOMPLETE] */
    BOOT_HWIO_OUT(DDR_ADDR(SHKE_CONFIG), offset,
                chip_select & ~rank_init_fmsk);

    /* Toggle SHKE_CONFIG[RANK0/1_INITCOMPLETE] as workaround for HW issue that
     TREFI is not loaded correctly */
    BOOT_HWIO_OUTM(DDR_ADDR(SHKE_CONFIG), offset,
                 rank_init_fmsk, 0x0);

    /* Wait for 1us (at least 3 XO cycles) */
    HAL_SDRAM_DDR_Wait(1, 0);

    BOOT_HWIO_OUT(DDR_ADDR(SHKE_CONFIG), 
                   offset, 
                   shke_config);

              
  }
  
  /* Initialize BIMC DPE */
  HAL_SDRAM_DPE_Init(0, interface);
  /* Initialize BIMC SHKE */
  HAL_SDRAM_SHKE_Init(0, interface);
  /* Initialize BIMC SCMO */
  HAL_SDRAM_SCMO_Init(0, interface);

  if (chip_select != SDRAM_CS_NONE)
  {
    /* Enable auto refresh */
    HAL_SDRAM_SHKE_Enable_Auto_Refresh(0, interface, chip_select);
    /* Enable HW based self refresh */
    HAL_SDRAM_SHKE_Enable_HW_Self_Refresh(0, interface, chip_select);
    /* Enable auto ZQCAL */
    HAL_SDRAM_SHKE_Enable_ZQCAL(0, interface, chip_select);
    /* Enable auto SRR */
    HAL_SDRAM_SHKE_Enable_Periodic_SRR(0, interface, chip_select);

#ifndef BOOT_PRE_SILICON
    /* Enable auto IOCAL */
    HAL_SDRAM_PHY_Enable_IO_Cal(0, interface);
#endif
  }

} /* HAL_SDRAM_Init */

/* ============================================================================
**  Function : HAL_SDRAM_Ram_Size_Detection
** ============================================================================
*/
/*!
*   @brief
*   This function will probe various address of ddr to find out the exact
*   configuration of row and column size
*   
*   @details
*   This function will probe various address of ddr to find out the exact
*   configuration of row and column size. Special care is needed with device
*   configured as 2x 16 bit or 4x 8 bit devices that combines into 1 32 bit 
*   device as the MR8 reading will return just 1 of the device. We know however
*   that HSDDRx does not support 16 or 8 bit devices, thus it has to be a a 
*   multiple of the detected device that makes up to 32 bit.
*   
*   @param interface_name   -  [IN] The interface to intialize for
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
boolean HAL_SDRAM_Ram_Size_Detection(SDRAM_INTERFACE interface)
{
  uint32 offset;
  struct ddr_device_params_common *ddr_params;
  struct ddr_device_params_lpddr *ddr_params_lpddr;
  uint32 mr8;
  uint32 device_type, device_density, device_io_width;
  uint32 nrows, ncols, nbanks, width, density_cdt = 0;

  /* Channel offset */
  offset = ((interface == SDRAM_INTERFACE_0) ? SDRAM_0_OFFSET : SDRAM_1_OFFSET);

  /* Get DDR device parameters */
  ddr_params = &(ddr_get_params(interface)->common);
  ddr_params_lpddr = &(ddr_get_params(interface)->lpddr);  

  if ( BOOT_HWIO_INM(DDR_ADDR(SHKE_CONFIG), offset,
                     HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK0_INITCOMPLETE)) )
  {
    /* Read manufacture ID from MR5 */
    ddr_params->manufacture_name = HAL_SDRAM_Read_MR(0, interface, SDRAM_CS0, 0x5);

    /*
      Mode Register 8

      BIT[1:0] - Type
        'b00: S4 SDRAM (LPDDR2)
        'b11: S8 SDRAM (LPDDR3)
        All others: Do not detect

      BIT[5:2] - Density
        'b0100: 1Gb
        'b0101: 2Gb
        'b0110: 4Gb
        'b0111: 8Gb
        'b1000: 16Gb
        All others: Do not detect

      BIT[7:6] - I/O Width
        'b00: x32
        'b01: x16
        All others: Do not detect
    */
    mr8 = HAL_SDRAM_Read_MR(0, interface, SDRAM_CS0, 0x8);
    device_type = (mr8 & 0x3);
    device_density = (mr8 & 0x3C) >> 2;
    device_io_width = (mr8 & 0xC0) >> 6;

    if (device_type != 0 && device_type != 3)
    {
      /* Detect S4 LPDDR2 and S8 LPDDR3 only */
      return FALSE;
    }

    density_cdt = ((ddr_params_lpddr->tRmin) >> 0x10) & 0xF;
    if (density_cdt == 1)
    {
     /* 1Gb */
     nrows = 13; ncols = 9; nbanks = 8;
    }
    else if (density_cdt == 2)
    {
     /* 2Gb */
     nrows = 14; ncols = 9; nbanks = 8;
    }
    else if (density_cdt == 3)
    {
     /* 4Gb */
     nrows = 14; ncols = 10; nbanks = 8;
    }
    else
    {   
      switch (device_density)
        {
         case 4:
            /* 1Gb */
            nrows = 13; ncols = 9; nbanks = 8;
            break;
        case 5:
            /* 2Gb */
            nrows = 14; ncols = 9; nbanks = 8;
            break;
        case 6:
            /* 4Gb */
            nrows = 14; ncols = 10; nbanks = 8;
            break;
        case 14:
            /* 6Gb */
            nrows = 15; ncols = 10; nbanks = 8;
            break;
        case 7:
            /* 8Gb */
            nrows = 15; ncols = 10; nbanks = 8;
            break;
        case 8:
            /* 16Gb */
            nrows = 15; ncols = 11; nbanks = 8;
            break;
        default:
            /* Detect 1Gb-16Gb only */
            return FALSE;
        }
    }

    if (device_io_width == 0)
    {
      /* x32 */
      width = 32;
    }
    else if (device_io_width == 1)
    {
      /* x16 */
      width = 16;
      ncols++;
    }
    else
    {
      /* Detect x32 and x16 only */
      return FALSE;
    }

    ddr_params->num_rows_cs0 = nrows;
    ddr_params->num_cols_cs0 = ncols;
    ddr_params->num_banks_cs0 = nbanks;
    ddr_params->interface_width = width;
  }

  if ( BOOT_HWIO_INM(DDR_ADDR(SHKE_CONFIG), offset,
                     HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK1_INITCOMPLETE)) )
  {
    /* Read manufacture ID from MR5 */
    ddr_params->manufacture_name = HAL_SDRAM_Read_MR(0, interface, SDRAM_CS1, 0x5);

    /*
      Mode Register 8

      BIT[1:0] - Type
        'b00: S4 SDRAM (LPDDR2)
        'b11: S8 SDRAM (LPDDR3)
        All others: Do not detect

      BIT[5:2] - Density
        'b0100: 1Gb
        'b0101: 2Gb
        'b0110: 4Gb
        'b0111: 8Gb
        'b1000: 16Gb
        All others: Do not detect

      BIT[7:6] - I/O Width
        'b00: x32
        'b01: x16
        All others: Do not detect
    */
    mr8 = HAL_SDRAM_Read_MR(0, interface, SDRAM_CS1, 0x8);
    device_type = (mr8 & 0x3);
    device_density = (mr8 & 0x3C) >> 2;
    device_io_width = (mr8 & 0xC0) >> 6;

    if (device_type != 0 && device_type != 3)
    {
      /* Detect S4 LPDDR2 and S8 LPDDR3 only */
      return FALSE;
    }

    switch (device_density)
    {
      case 4:
        /* 1Gb */
        nrows = 13; ncols = 9; nbanks = 8;
        break;
      case 5:
        /* 2Gb */
        nrows = 14; ncols = 9; nbanks = 8;
        break;
      case 6:
        /* 4Gb */
        nrows = 14; ncols = 10; nbanks = 8;
        break;
      case 14:
        /* 6Gb */
        nrows = 15; ncols = 10; nbanks = 8;
        break;        
      case 7:
        /* 8Gb */
        nrows = 15; ncols = 10; nbanks = 8;
        break;
      case 8:
        /* 16Gb */
        nrows = 15; ncols = 11; nbanks = 8;
        break;
      default:
        /* Detect 1Gb-16Gb only */
        return FALSE;
    }

    if (device_io_width == 0)
    {
      /* x32 */
      width = 32;
    }
    else if (device_io_width == 1)
    {
      /* x16 */
      width = 16;
      ncols++;
    }
    else
    {
      /* Detect x32 and x16 only */
      return FALSE;
    }

    ddr_params->num_rows_cs1 = nrows;
    ddr_params->num_cols_cs1 = ncols;
    ddr_params->num_banks_cs1 = nbanks;
    ddr_params->interface_width |= (width << 16);
  }
  
  /* update tRFC and tXSR based on detected size */
  if(ddr_params->num_rows_cs0 > 14 || ddr_params->num_rows_cs1 > 14)
  {
    /* > 4Gb need longer tRFC & tXSR */
    ddr_params->tRFC = 2100;
    ddr_params->tXSR = 2200;
  }

  return TRUE;

} /* HAL_SDRAM_Ram_Size_Detection */

/* ============================================================================
**  Function : HAL_SDRAM_Get_Base_Addr
** ============================================================================
*/
/*!
*   @brief
*   This function return the base address of the specific interface and chip select
*   
*   @details
*   This function will return pre determined base address for the particular interface\chipselect requested
*   
*   @param  None
*   
*   @par Dependencies
*   
*   
*   @par Side Effects
*
*   
*   @retval  uint32 - a return value of 0xFFFFFFFF indicates that this is not a valid interface\chipselect to use.
*
*   
*   @sa None
*/

uint32 HAL_SDRAM_Get_Base_Addr(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select)
{
  uint32 base_addr = 0xFFFFFFFF;
  uint32 sdram0_cs0_size = 0;
  uint32 sdram0_cs1_size = 0;
  uint32 sdram1_cs0_size = 0;
  uint32 ddr_interleaved = ddr_param_interleaved();
  struct ddr_device_params_common *ddr_params;

  /* Calculate DDR size based on parameters */
  ddr_params = &(ddr_get_params(SDRAM_INTERFACE_0)->common);

  sdram0_cs0_size = ddr_params->num_banks_cs0
                    << (ddr_params->num_rows_cs0 + ddr_params->num_cols_cs0 + 2);
  sdram0_cs1_size = ddr_params->num_banks_cs1
                    << (ddr_params->num_rows_cs1 + ddr_params->num_cols_cs1 + 2);

  ddr_params = &(ddr_get_params(SDRAM_INTERFACE_1)->common);

  sdram1_cs0_size = ddr_params->num_banks_cs0
                    << (ddr_params->num_rows_cs0 + ddr_params->num_cols_cs0 + 2);

  if(chip_select == SDRAM_CS0)
  {
    if(interface_name == SDRAM_INTERFACE_0)
    {
        /* Regardless of interleave vs non-interleave
         * we always populate CH0 CS0 at the base,
         * no calculation needed.
         */
        base_addr = DDR_BASE;
    }
    else /* if (interface_name == SDRAM_INTERFACE_1) */
    {
      /* CH1 CS0 base address depends on interleave vs non-interleave
       * interleaved base will be [base of CH0 CS0]
       * non-interleaved base will be [base of CH0 CS0] +
       *                              [ramsize of CH0 CS0] +
       *                              [ramsize of CH0 CS1]
       */
      if(ddr_interleaved & SDRAM_CS0_INTERLEAVE)
      {
        base_addr = DDR_BASE;
      }
      else
      {
        /* Non-interleaved memory follows logical base address increments
         * CH0 CS1 follows CH0 CS0,
         * CH1 CS0 follows CH0 CS1,
         * etc.
         */
        base_addr = DDR_BASE + (sdram0_cs0_size + sdram0_cs1_size);
      }
    }
  }
  else /* if(chip_select = SDRAM_CS1) */
  {
    /* CH0 CS1 base address depends on interleave vs non-interleave
     * interleaved base will be [base of CH0 CS0] + [ramsize of CH0 CS0] * 2
     * non-interleaved base will be [base of CH0 CS0] + [ramsize of CH0 CS0]
     */
    if(ddr_interleaved & SDRAM_CS1_INTERLEAVE)
    {
      base_addr = DDR_BASE + (sdram0_cs0_size * 2);
    }
    else
    {
      /* Non-interleaved memory follows logical base address increments
       * CH0 CS1 follows CH0 CS0,
       * CH1 CS0 follows CH0 CS1,
       * etc.
       */
      if(interface_name == SDRAM_INTERFACE_0)
      {
        base_addr = DDR_BASE + sdram0_cs0_size;
      }
      else
      {
        base_addr = DDR_BASE + (sdram0_cs0_size + sdram0_cs1_size + sdram1_cs0_size);
      }
    }
  }

  return base_addr;

} /* HAL_SDRAM_Get_Base_Addr */

/* ============================================================================
**  Function : HAL_SDRAM_DDR_Wait
** ============================================================================
*/
/*!
*   @brief
*   Waits for the given amount of time in us
*   
*   @details
*   waits the given amount of time in us
*   implemented using pause timer when available
*   
*   @param wait_us     -  [IN] Time to wait in terms of microseconds
*   @param clk_speed   -  [IN] Current clock frequency in KHz
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
void HAL_SDRAM_DDR_Wait(uint32 wait_us, uint32 clk_speed)
{
  /* Note: Do not use clk_speed since it could be passed incorrectly */
  if(!wdog_reset)
  {
    busywait(wait_us);
  }
  else
  {
    ddr_busywait(wait_us);
  }

} /* HAL_SDRAM_DDR_Wait */

/* ============================================================================
**  Function : HAL_SDRAM_Enter_Self_Refresh
** ============================================================================
*/
/*!
*   @brief
*   Puts the ddr into self refresh mode using idle self refresh
*
*   @details
*   Puts the ddr into self refresh mode using idle self refresh.
*   This means the controller will wait for the device to become idle
*   before putting the ddr into self refresh.
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   Once ddr has entered self refresh no further access to it is allowed.
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_Enter_Self_Refresh(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, ddr_wait wait)
{
  /* Disable ranks */
  HAL_SDRAM_SCMO_Rank_Disable(base, interface, chip_select);

  /* Disable HW based self refresh */
  HAL_SDRAM_SHKE_Disable_HW_Self_Refresh(base, interface, chip_select);

  /* Disable auto IOCAL */
  HAL_SDRAM_PHY_Disable_IO_Cal(base, interface, wait);

  /* Enter self refresh */
  HAL_SDRAM_SHKE_Enter_Self_Refresh(base, interface, chip_select);

  /* Update DDR log */
  DDR_LOG(DDR_STATUS, DDR_SELF_REFRESH_ENTRY_IDLE, interface, chip_select);

} /* HAL_SDRAM_Enter_Self_Refresh */

/* ============================================================================
**  Function : HAL_SDRAM_Exit_Self_Refresh
** ============================================================================
*/
/*!
*   @brief
*   Takes the ddr out of self refresh mode using idle self refresh
*
*   @details
*   Takes the ddr out of self refresh mode.
*   After the ddr has been taken out of self refresh memory access can resume
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_Exit_Self_Refresh(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, ddr_wait wait)
{
  /* Enable ranks */
  HAL_SDRAM_SCMO_Rank_Enable(base, interface, chip_select);

  /* Issue manual IOCAL */
  HAL_SDRAM_PHY_Manual_IO_Cal(base, interface, wait);

  /* Exit self refresh */
  HAL_SDRAM_SHKE_Exit_Self_Refresh(base, interface, chip_select);

  /* Enable auto IOCAL */
  HAL_SDRAM_PHY_Enable_IO_Cal(base, interface);

  /* Enable HW based self refresh */
  HAL_SDRAM_SHKE_Enable_HW_Self_Refresh(base, interface, chip_select);

  /* Update DDR log */
  DDR_LOG(DDR_STATUS, DDR_SELF_REFRESH_EXIT, interface, chip_select);

} /* HAL_SDRAM_Exit_Self_Refresh */

/* =============================================================================
**  Function : HAL_SDRAM_BIMC_Rank_Detection
** =============================================================================
*/
/**
*   @brief
*   Detect present ranks by reading a mode register on that rank. A rank is
*   present if and only if the rank is initialized and the mode register read
*   does not time out.
*
*   @param[in]  interface  Interface to detect for
*
*   @return
*   Present ranks.
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
SDRAM_CHIPSELECT HAL_SDRAM_BIMC_Rank_Detection(SDRAM_INTERFACE interface)
{
  uint32 offset;
  struct ddr_device_params_common *ddr_params;
  SDRAM_CHIPSELECT chip_select;

  /* Channel offset */
  offset = ((interface == SDRAM_INTERFACE_0) ? SDRAM_0_OFFSET : SDRAM_1_OFFSET);

  /* Get DDR device parameters */
  ddr_params = &(ddr_get_params(interface)->common);

  /* No rank detected yet */
  chip_select = SDRAM_CS_NONE;

  /* Rank detection for CS0 */
  if ( BOOT_HWIO_INM(DDR_ADDR(SHKE_CONFIG), offset,
                     HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK0_INITCOMPLETE)) )
  {
    /* Read MR32 */
    HAL_SDRAM_Read_MR(0, interface, SDRAM_CS0, 0x20);

    /* Read SHKE_MREG_RDATA_STATUS[MREG_TIMEOUT] to determine if MRR times out */
    if ( BOOT_HWIO_INM(DDR_ADDR(SHKE_MREG_RDATA_STATUS), offset,
                       HWIO_FMSK(DDR_ADDR(SHKE_MREG_RDATA_STATUS), MREG_TIMEOUT)) )
    {
      /* Rank is not present since MRR times out */
      /* Disable rank */
      BOOT_HWIO_OUTM(DDR_ADDR(SHKE_CONFIG), offset,
                     HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK0_EN) |
                     HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK0_INITCOMPLETE),
                     0x0U << HWIO_SHFT(DDR_ADDR(SHKE_CONFIG), RANK0_EN) |
                     0x0U << HWIO_SHFT(DDR_ADDR(SHKE_CONFIG), RANK0_INITCOMPLETE));

      /* Disable CKE */
      BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
                     HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL) |
                     HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), CKE_OFF),
                     SDRAM_CS0 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL) |
                     0x1U << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), CKE_OFF));
      /* Poll for CKE off done */
      while ( BOOT_HWIO_INM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
                            HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), CKE_OFF)) )
      {}

      /* Update row/column/bank numbers */
      ddr_params->num_rows_cs0 = 0;
      ddr_params->num_cols_cs0 = 0;
      ddr_params->num_banks_cs0 = 0;
    }
    else
    {
      /* Rank is present since MRR does not time out */
      chip_select |= SDRAM_CS0;
    }
  }

  /* Rank detection for CS1 */
  if ( BOOT_HWIO_INM(DDR_ADDR(SHKE_CONFIG), offset,
                     HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK1_INITCOMPLETE)) )
  {
    /* Read MR32 */
    HAL_SDRAM_Read_MR(0, interface, SDRAM_CS1, 0x20);

    /* Read SHKE_MREG_RDATA_STATUS[MREG_TIMEOUT] to determine if MRR times out */
    if ( BOOT_HWIO_INM(DDR_ADDR(SHKE_MREG_RDATA_STATUS), offset,
                       HWIO_FMSK(DDR_ADDR(SHKE_MREG_RDATA_STATUS), MREG_TIMEOUT)) )
    {
      /* Rank is not present since MRR times out */
      /* Disable rank */
      BOOT_HWIO_OUTM(DDR_ADDR(SHKE_CONFIG), offset,
                     HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK1_EN) |
                     HWIO_FMSK(DDR_ADDR(SHKE_CONFIG), RANK1_INITCOMPLETE),
                     0x0U << HWIO_SHFT(DDR_ADDR(SHKE_CONFIG), RANK1_EN) |
                     0x0U << HWIO_SHFT(DDR_ADDR(SHKE_CONFIG), RANK1_INITCOMPLETE));

      /* Disable CKE */
      BOOT_HWIO_OUTM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
                     HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL) |
                     HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), CKE_OFF),
                     SDRAM_CS1 << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), RANK_SEL) |
                     0x1U << HWIO_SHFT(DDR_ADDR(SHKE_DRAM_MANUAL_0), CKE_OFF));
      /* Poll for CKE off done */
      while ( BOOT_HWIO_INM(DDR_ADDR(SHKE_DRAM_MANUAL_0), offset,
                            HWIO_FMSK(DDR_ADDR(SHKE_DRAM_MANUAL_0), CKE_OFF)) )
      {}

      /* Update row/column/bank numbers */
      ddr_params->num_rows_cs1 = 0;
      ddr_params->num_cols_cs1 = 0;
      ddr_params->num_banks_cs1 = 0;
    }
    else
    {
      /* Rank is present since MRR does not time out */
      chip_select |= SDRAM_CS1;
    }
  }

  return chip_select;

} /* HAL_SDRAM_BIMC_Rank_Detection */

/* =============================================================================
**  Function : HAL_SDRAM_RL
** =============================================================================
*/
/**
*   @brief
*   Return Read Latency according to DDR type and frequency.
*
*   @param[in]  ddr_type   DDR type
*   @param[in]  clk_speed  Clock speed (in KHz)
*
*   @return
*   Read Latency.
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
uint32 HAL_SDRAM_RL(DDR_TYPE ddr_type, uint32 clk_speed)
{
  uint32 rl = 0;

  if (ddr_type == DDR_TYPE_LPDDR2)
  {
    if (clk_speed <= 267000) { rl = 4; }
    else if (clk_speed <= 334000) { rl = 5; }
    else if (clk_speed <= 400000) { rl = 6; }
	else if (clk_speed <= 466500) { rl = 7; }
    else if (clk_speed <= 534000) { rl = 8; }
  }
  else if (ddr_type == DDR_TYPE_LPDDR3)
  {
    if (clk_speed <= 400000) { rl = 6; }
    else if (clk_speed <= 533000) { rl = 8; }
    else if (clk_speed <= 600000) { rl = 9; }
    else if (clk_speed <= 667000) { rl = 10; }
    else if (clk_speed <= 733000) { rl = 11; }
    else { rl = 12; }
  }

  return rl;

} /* HAL_SDRAM_RL */

/* =============================================================================
**  Function : HAL_SDRAM_WL
** =============================================================================
*/
/**
*   @brief
*   Return Write Latency according to DDR type and frequency.
*
*   @param[in]  ddr_type   DDR type
*   @param[in]  clk_speed  Clock speed (in KHz)
*
*   @return
*   Write Latency.
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
uint32 HAL_SDRAM_WL(DDR_TYPE ddr_type, uint32 clk_speed)
{
  uint32 wl = 0;

  if (ddr_type == DDR_TYPE_LPDDR2)
  {
    if (clk_speed <= 334000) { wl = 2; }
    else if (clk_speed <= 400000) { wl = 3; }
    else if (clk_speed <= 534000) { wl = 4; }
  }
  else if (ddr_type == DDR_TYPE_LPDDR3)
  {
    if (clk_speed <= 400000) { wl = 3; }
    else if (clk_speed <= 533000) {  wl = 4; }
    else if (clk_speed <= 600000) {  wl = 5; }
    else { wl = 6; }
  }

  return wl;

} /* HAL_SDRAM_WL */

/* =============================================================================
**  Function : HAL_SDRAM_BIMC_Update_Clock_Period
** =============================================================================
*/
/**
*   @brief
*   Updates the bimc clock period for clock switching
*
*   @param[in]  interface_name  interface to configure new clock for
*   @param[in]  clk_speed       Clock speed (in KHz)
*
*   @return
*   None
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
void HAL_SDRAM_BIMC_Update_Clock_Period(uint32 base, uint32 clk_speed, ddr_divide divide_func)
{
  uint32 period;
  
  if(BOOT_HWIO_IN(BIMC_CORE_CLK_PERIOD, base) & HWIO_FMSK(BIMC_CORE_CLK_PERIOD, CLK_PERIOD_RESOLUTION))
  {
  /* 1ps resolution */
    if(clk_speed == 19200)
    {
      period = 0xCB73;
    }
    else
    {
      period = MIN(divide_func(1000000000,clk_speed), 0x1ffff);
    }
  }
  else
  {
  	/* 100ps resolution */
    if(clk_speed == 19200)
    {
      period = 0x208;
    }   
    else
    {
      period = MIN(divide_func(10000000,clk_speed), 0xFF);
    }
  }
  
  BOOT_HWIO_OUTM(BIMC_CORE_CLK_PERIOD, base, HWIO_FMSK(BIMC_CORE_CLK_PERIOD, PERIOD), period);  
}

/* ============================================================================
**  Function : HAL_SDRAM_Enter_Deep_Power_Down
** ============================================================================
*/
/*!
*   @brief
*   This function will put the ddr into DPD mode
*
*   @details
*   This function will trigger the ddr to be put into dee power down mode.
*   In the case where we only have 1 device on the interface (CS0 only), or both
*   chip select are chosen, we will also turn off ddr clock.
*
*   @param interface_name   -  [IN] Interface to configure dpd for
*
*   @par Dependencies
*   The desired CS to be configured for Deep Power Down Must be configured first by
*   calling HAL_SDRAM_Chipselect().
*
*   @par Side Effects
*   After ddr has gone into Deep Power Down, all the data stored will be lost.
*   Also, when returning from deep power down, we must reinitialize the ddr.
*
*   @retval  None
*
*   @sa None
*/

void HAL_SDRAM_Enter_Deep_Power_Down(uint32 base, SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select)
{
  /* Disable Rank access */
  HAL_SDRAM_SCMO_Rank_Disable(base, interface_name, chip_select);

  /* Put ddr into deep power down */
  HAL_SDRAM_SHKE_DPD_Enter(base, interface_name, chip_select);

  DDR_LOG(DDR_STATUS, DDR_DPD_ENTRY, interface_name, chip_select);

} /* HAL_SDRAM_Enter_Deep_Power_Down */

/* ============================================================================
**  Function : HAL_SDRAM_Exit_Deep_Power_Down
** ============================================================================
*/
/*!
*   @brief
*   This function will take ddr out of deep power down
*
*   @details
*   This function will take ddr out of deep power down
*
*   @param clk_speed   -  [IN] Clock speed to reinitialize ddr for
*
*   @par Dependencies
*   Clock must be setup for initialization speed.
*   LPDDR1 - no limitation
*   LPDDR2 - < 50Mhz
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/

void HAL_SDRAM_Exit_Deep_Power_Down(uint32 base, SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select, uint32 clk_speed)
{
  /* Enable Rank access */
  HAL_SDRAM_SCMO_Rank_Enable(base, interface_name, chip_select);

  /* Take ddr out of deep power down */
  HAL_SDRAM_SHKE_DPD_Exit(base, interface_name, chip_select, clk_speed);

  DDR_LOG(DDR_STATUS, DDR_DPD_EXIT, interface_name, chip_select);

} /* HAL_SDRAM_Exit_Deep_Power_Down */
