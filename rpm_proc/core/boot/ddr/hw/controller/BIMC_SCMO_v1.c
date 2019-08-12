/*
===========================================================================

FILE:         BIMC_SCMO_v1.c

DESCRIPTION:  
  This is the platform hardware abstraction layer implementation for 
  BIMC SCMO.

Copyright 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/boot/ddr/hw/controller/BIMC_SCMO_v1.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
09/06/12   sl      Removed ddr_target_param.h.
08/24/12   sl      Updated HAL_SDRAM_SCMO_Init().
05/15/12   tw      Added API for rank disable
02/02/12   tw      Initial revision
===========================================================================
*/


/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_params.h"
#include "ddr_target.h"
#include "BIMC.h"
#include "HAL_DDR.h"


/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
/* ============================================================================
**  Function : HAL_SDRAM_SCMO_Init
** ============================================================================
*/
/*!
*   @brief
*   This function will initialize the DPE block.
*   
*   @details
*   Detailed_Description_Goes_here
*   
*   @param interface_name   -  [IN] Param_description_goes_here
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
void HAL_SDRAM_SCMO_Init(SDRAM_INTERFACE interface_name)
{
  uint32 offset;
  uint32 ddr_size_megabyte_cs0;
  uint32 ddr_size_megabyte_cs1;
  uint32 data;

  struct ddr_device_params_common *ddr_param;

  /* Get DDR device parameters */
  ddr_param = &(ddr_get_params(interface_name)->common);

  /* Configure for correct offset */
  if(interface_name == SDRAM_INTERFACE_0)
  {
    offset = SDRAM_0_OFFSET;
  }
  else
  {
    offset = SDRAM_1_OFFSET;
  }

  BOOT_HWIO_OUT(DDR_ADDR(SCMO_SLV_INTERLEAVE_CFG), offset, 0x0);
  BOOT_HWIO_OUTI(DDR_ADDR(SCMO_ADDR_BASE_CSn), offset, 0x0, 0x0);
  BOOT_HWIO_OUTI(DDR_ADDR(SCMO_ADDR_BASE_CSn), offset, 0x1, 0x0);
  BOOT_HWIO_OUTI(DDR_ADDR(SCMO_ADDR_MAP_CSn), offset, 0x0, 0x0);
  BOOT_HWIO_OUTI(DDR_ADDR(SCMO_ADDR_MAP_CSn), offset, 0x1, 0x0);
  BOOT_HWIO_OUTI(DDR_ADDR(SCMO_ADDR_MASK_CSn), offset, 0x0, 0x0);
  BOOT_HWIO_OUTI(DDR_ADDR(SCMO_ADDR_MASK_CSn), offset, 0x1, 0x0);

  if(ddr_param_interleaved() & SDRAM_CS0)
  {
    BOOT_HWIO_OUTM(DDR_ADDR(SCMO_SLV_INTERLEAVE_CFG), 
                   offset, 
                   HWIO_FMSK(DDR_ADDR(SCMO_SLV_INTERLEAVE_CFG), INTERLEAVE_CS0), 
                   0x1 << HWIO_SHFT(DDR_ADDR(SCMO_SLV_INTERLEAVE_CFG), INTERLEAVE_CS0));    
  }
  if(ddr_param_interleaved() & SDRAM_CS1)
  {
    BOOT_HWIO_OUTM(DDR_ADDR(SCMO_SLV_INTERLEAVE_CFG), 
                   offset, 
                   HWIO_FMSK(DDR_ADDR(SCMO_SLV_INTERLEAVE_CFG), INTERLEAVE_CS1), 
                   0x1 << HWIO_SHFT(DDR_ADDR(SCMO_SLV_INTERLEAVE_CFG), INTERLEAVE_CS1));    
  }	

  /* Configure the address mask based on configured size */

  /* first calculate intermediate size in terms of megabits */
  ddr_size_megabyte_cs0 =  (1ul<<(ddr_param->num_rows_cs0 + 
                             ddr_param->num_cols_cs0 - 20));
  /* Calculate final size in terms of MegaBytes */ 
  ddr_size_megabyte_cs0 = ddr_size_megabyte_cs0 * ddr_param->num_banks_cs0 * 32 >> 3;
  
  /* first calculate intermediate size in terms of megabits */
  ddr_size_megabyte_cs1 =  (1ul<<(ddr_param->num_rows_cs1 + 
                             ddr_param->num_cols_cs1 - 20));
  /* Calculate final size in terms of MegaBytes */ 
  ddr_size_megabyte_cs1 = ddr_size_megabyte_cs1 * ddr_param->num_banks_cs1 * 32 >> 3;

  /*
    For each Chip Select n (0 <= n < DDR_RANK_NUMBER)
  */
  if(ddr_size_megabyte_cs0 != 0)
  {
    /* rank0 is available */

    switch (ddr_size_megabyte_cs0)
    {
      case 2048:
        data = 0x20;
        break;
      case 1024:
        data = 0x30;
        break;
      case 512:
        data = 0x38;
        break;
      case 256:
        data = 0x3C;
        break;
      case 128:
        data = 0x3E;
        break;
      case 64:
        data = 0x3F;
        break;
      default:
        data = 0;
        break;

    }

    BOOT_HWIO_OUTMI(DDR_ADDR(SCMO_ADDR_MASK_CSn), 
                    offset,
                    0x0,
                    HWIO_FMSK(DDR_ADDR(SCMO_ADDR_MASK_CSn), ADDR_MASK),
                    data << HWIO_SHFT(DDR_ADDR(SCMO_ADDR_MASK_CSn), ADDR_MASK));

    /* Configure SCMO address map */
    BOOT_HWIO_OUTMI(DDR_ADDR(SCMO_ADDR_MAP_CSn), 
                    offset,
                    0x0,
                    HWIO_FMSK(DDR_ADDR(SCMO_ADDR_MAP_CSn), COL_SIZE)  |
                    HWIO_FMSK(DDR_ADDR(SCMO_ADDR_MAP_CSn), ROW_SIZE)  |
                    HWIO_FMSK(DDR_ADDR(SCMO_ADDR_MAP_CSn), BANK_SIZE) |
                    HWIO_FMSK(DDR_ADDR(SCMO_ADDR_MAP_CSn), ADDR_MODE) |
                    HWIO_FMSK(DDR_ADDR(SCMO_ADDR_MAP_CSn), RANK_EN),
                    (ddr_param->num_cols_cs0 - 8) << HWIO_SHFT(DDR_ADDR(SCMO_ADDR_MAP_CSn), COL_SIZE)  |
                    (ddr_param->num_rows_cs0 - 13) << HWIO_SHFT(DDR_ADDR(SCMO_ADDR_MAP_CSn), ROW_SIZE)  |
                    ((ddr_param->num_banks_cs0 == 4)? 0 : 1) << HWIO_SHFT(DDR_ADDR(SCMO_ADDR_MAP_CSn), BANK_SIZE) |
                    0x0 << HWIO_SHFT(DDR_ADDR(SCMO_ADDR_MAP_CSn), ADDR_MODE) |
                    0x1 << HWIO_SHFT(DDR_ADDR(SCMO_ADDR_MAP_CSn), RANK_EN));
    BOOT_HWIO_OUTMI(DDR_ADDR(SCMO_ADDR_BASE_CSn),
                    offset,
                    0x0,
                    HWIO_FMSK(DDR_ADDR(SCMO_ADDR_BASE_CSn), ADDR_BASE),
                    (HAL_SDRAM_Get_Base_Addr(interface_name, SDRAM_CS0) >> 16)
                    << HWIO_SHFT(DDR_ADDR(SCMO_ADDR_BASE_CSn), ADDR_BASE));
  }
  else
  {
    BOOT_HWIO_OUTMI(DDR_ADDR(SCMO_ADDR_BASE_CSn),
                    offset,
                    0x0,
                    HWIO_FMSK(DDR_ADDR(SCMO_ADDR_BASE_CSn), ADDR_BASE),
                    0xFFFF << HWIO_SHFT(DDR_ADDR(SCMO_ADDR_BASE_CSn), ADDR_BASE));
  }

if(ddr_size_megabyte_cs1 != 0)
  {
    /* rank1 is available */
    switch (ddr_size_megabyte_cs1)
    {
      case 2048:
        data = 0x20;
        break;
      case 1024:
        data = 0x30;
        break;
      case 512:
        data = 0x38;
        break;
      case 256:
        data = 0x3C;
        break;
      case 128:
        data = 0x3E;
        break;
      case 64:
        data = 0x3F;
        break;
      default:
        data = 0;
        break;

    }

    BOOT_HWIO_OUTMI(DDR_ADDR(SCMO_ADDR_MASK_CSn),
                    offset,
                    0x1,
                    HWIO_FMSK(DDR_ADDR(SCMO_ADDR_MASK_CSn), ADDR_MASK),
                    data << HWIO_SHFT(DDR_ADDR(SCMO_ADDR_MASK_CSn), ADDR_MASK));

    /* Configure SCMO address map */
    BOOT_HWIO_OUTMI(DDR_ADDR(SCMO_ADDR_MAP_CSn),
                    offset,
                    0x1,
                    HWIO_FMSK(DDR_ADDR(SCMO_ADDR_MAP_CSn), COL_SIZE)  |
                    HWIO_FMSK(DDR_ADDR(SCMO_ADDR_MAP_CSn), ROW_SIZE)  |
                    HWIO_FMSK(DDR_ADDR(SCMO_ADDR_MAP_CSn), BANK_SIZE) |
                    HWIO_FMSK(DDR_ADDR(SCMO_ADDR_MAP_CSn), ADDR_MODE) |
                    HWIO_FMSK(DDR_ADDR(SCMO_ADDR_MAP_CSn), RANK_EN),
                    (ddr_param->num_cols_cs1 - 8) << HWIO_SHFT(DDR_ADDR(SCMO_ADDR_MAP_CSn), COL_SIZE)  |
                    (ddr_param->num_rows_cs1 - 13) << HWIO_SHFT(DDR_ADDR(SCMO_ADDR_MAP_CSn), ROW_SIZE)  |
                    ((ddr_param->num_banks_cs1 == 4)? 0 : 1) << HWIO_SHFT(DDR_ADDR(SCMO_ADDR_MAP_CSn), BANK_SIZE) |
                    0x0 << HWIO_SHFT(DDR_ADDR(SCMO_ADDR_MAP_CSn), ADDR_MODE) |
                    0x1 << HWIO_SHFT(DDR_ADDR(SCMO_ADDR_MAP_CSn), RANK_EN));

    /* Configure SCMO base address */
    BOOT_HWIO_OUTMI(DDR_ADDR(SCMO_ADDR_BASE_CSn),
                    offset,
                    0x1,
                    HWIO_FMSK(DDR_ADDR(SCMO_ADDR_BASE_CSn), ADDR_BASE),
                    HAL_SDRAM_Get_Base_Addr(interface_name, SDRAM_CS1) >> 24);
  }
  else
  {
    BOOT_HWIO_OUTMI(DDR_ADDR(SCMO_ADDR_BASE_CSn),
                    offset,
                    0x1,
                    HWIO_FMSK(DDR_ADDR(SCMO_ADDR_BASE_CSn), ADDR_BASE),
                    0xFFFF << HWIO_SHFT(DDR_ADDR(SCMO_ADDR_BASE_CSn), ADDR_BASE));
  }
}

/* ============================================================================
**  Function : HAL_SDRAM_SCMO_Rank_Disable
** ============================================================================
*/
/*!
*   @brief
*   This function will initialize the disable the rank given in the parameter
*
*   @details
*   This function will initialize the disable the rank given in the parameter
*
*   @param interface_name   -  [IN] interface to disable the rank for
*   @param chip_select      -  [IN] chipselect for rank disable
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   Any new traffic going to the rank disabled will be blocked and generate an error
*
*   @retval  None
*
*   @sa None
*/
void HAL_SDRAM_SCMO_Rank_Disable(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select)
{
  uint32 offset;

  /* Configure for correct offset */
  if(interface_name == SDRAM_INTERFACE_0)
  {
    offset = SDRAM_0_OFFSET;
  }
  else
  {
    offset = SDRAM_1_OFFSET;
  }

  /* Set SCMO_ADDR_MAP_CSn[RANK_EN] = 0 for the ranks which needs to be disabled */
  if(chip_select & SDRAM_CS0)
  {
    BOOT_HWIO_OUTMI(DDR_ADDR(SCMO_ADDR_MAP_CSn),
        offset,
        0,
        HWIO_FMSK(DDR_ADDR(SCMO_ADDR_MAP_CSn), RANK_EN),
        0);
  }
  if(chip_select & SDRAM_CS1)
  {
    BOOT_HWIO_OUTMI(DDR_ADDR(SCMO_ADDR_MAP_CSn),
        offset,
        1,
        HWIO_FMSK(DDR_ADDR(SCMO_ADDR_MAP_CSn), RANK_EN),
        0);
  }
  /* Flush all buffers */
  BOOT_HWIO_OUTM(DDR_ADDR(SCMO_FLUSH_CMD),
                 offset,
                 HWIO_FMSK(DDR_ADDR(SCMO_FLUSH_CMD), FLUSH_ALL_BUF),
                 0x1 << HWIO_SHFT(DDR_ADDR(SCMO_FLUSH_CMD), FLUSH_ALL_BUF));
  while(BOOT_HWIO_INM(DDR_ADDR(SCMO_FLUSH_CMD), offset,
                      HWIO_FMSK(DDR_ADDR(SCMO_FLUSH_CMD), FLUSH_ALL_BUF)))
  {}

  /* Wait for ranks to become idle */
  while(BOOT_HWIO_INM(DDR_ADDR(SCMO_SLV_STATUS), offset,
                      HWIO_FMSK(DDR_ADDR(SCMO_SLV_STATUS), RANK_IDLE)) == 0)
  {}
}

/* ============================================================================
**  Function : HAL_SDRAM_SCMO_Rank_Enable
** ============================================================================
*/
/*!
*   @brief
*   This function will initialize the enable the rank given in the parameter
*
*   @details
*   This function will initialize the enable the rank given in the parameter
*
*   @param interface_name   -  [IN] interface to enable the rank for
*   @param chip_select      -  [IN] chipselect for rank enable
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
void HAL_SDRAM_SCMO_Rank_Enable(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select)
{
  uint32 offset;

  /* Configure for correct offset */
  if(interface_name == SDRAM_INTERFACE_0)
  {
    offset = SDRAM_0_OFFSET;
  }
  else
  {
    offset = SDRAM_1_OFFSET;
  }

  /* Set SCMO_ADDR_MAP_CSn[RANK_EN] = 0 for the ranks which needs to be disabled */
  if(chip_select & SDRAM_CS0)
  {
    BOOT_HWIO_OUTMI(DDR_ADDR(SCMO_ADDR_MAP_CSn),
        offset,
        0,
        HWIO_FMSK(DDR_ADDR(SCMO_ADDR_MAP_CSn), RANK_EN),
        1 << HWIO_SHFT(DDR_ADDR(SCMO_ADDR_MAP_CSn), RANK_EN));
  }
  if(chip_select & SDRAM_CS1)
  {
    BOOT_HWIO_OUTMI(DDR_ADDR(SCMO_ADDR_MAP_CSn),
        offset,
        1,
        HWIO_FMSK(DDR_ADDR(SCMO_ADDR_MAP_CSn), RANK_EN),
        1 << HWIO_SHFT(DDR_ADDR(SCMO_ADDR_MAP_CSn), RANK_EN));
  }
}
