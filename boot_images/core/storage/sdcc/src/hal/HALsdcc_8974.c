/**********************************************************************
* HALsdcc_8974.c
*
* SDCC(Secure Digital Card Controller) driver HAL Platform Specific API.
*
* This file implements the SDCC driver HAL Platform Specific APIs.
*
* Copyright (c) 2007-2014
* Qualcomm Technologies Incorporated.
* All Rights Reserved.
* Qualcomm Confidential and Proprietary
*
**********************************************************************/
/*=======================================================================
                        Edit History

when        who     what, where, why
----------------------------------------------------------------------
2014-03-11  pa      Code cleanup
2013-06-13  bn      Added MCLK_REG_WR_ACTIVE check
2010-04-10  rh      Branched from 7660 HAL file
======================================================================*/
#include SDCC_OSAL_H
#include SDCC_HWIO_H
#include "HALsdcc.h"
#include "HALsdcc_sdcc4.h"

/** Number of SD controllers present in the MSM */
#define HAL_SDCC_CONTROLLER_COUNT    4

#define SDCC_REG_SLOT_1         0
#define SDCC_REG_SLOT_2         1
#define SDCC_REG_SLOT_3         2
#define SDCC_REG_SLOT_4         3

/*
 * Initilize the HWIO address table for access to hwio regs using the
 * drive number as the index:
 */
uint32 sdcc_reg_base[HAL_SDCC_CONTROLLER_COUNT];
uint32 sdcc_reg_phys[HAL_SDCC_CONTROLLER_COUNT];
uint32 sdhc_reg_base[HAL_SDCC_CONTROLLER_COUNT];
uint32 sdhc_reg_phys[HAL_SDCC_CONTROLLER_COUNT];
boolean hwio_init_done   = FALSE;

void 
HAL_sdcc_InitializeRegisters()
{
  if (hwio_init_done == TRUE) 
  {
     return;
  }

  sdcc_reg_base[SDCC_REG_SLOT_1] = HWIO_PERIPH_SS_SDC1_SDCC_MCI_POWER_ADDR;
  sdcc_reg_phys[SDCC_REG_SLOT_1] = HWIO_PERIPH_SS_SDC1_SDCC_MCI_POWER_PHYS;
  sdcc_reg_base[SDCC_REG_SLOT_2] = HWIO_PERIPH_SS_SDC2_SDCC_MCI_POWER_ADDR;
  sdcc_reg_phys[SDCC_REG_SLOT_2] = HWIO_PERIPH_SS_SDC2_SDCC_MCI_POWER_PHYS;
  sdcc_reg_base[SDCC_REG_SLOT_3] = HWIO_PERIPH_SS_SDC3_SDCC_MCI_POWER_ADDR;
  sdcc_reg_phys[SDCC_REG_SLOT_3] = HWIO_PERIPH_SS_SDC3_SDCC_MCI_POWER_PHYS;
  sdcc_reg_base[SDCC_REG_SLOT_4] = HWIO_PERIPH_SS_SDC4_SDCC_MCI_POWER_ADDR;
  sdcc_reg_phys[SDCC_REG_SLOT_4] = HWIO_PERIPH_SS_SDC4_SDCC_MCI_POWER_PHYS;

  sdhc_reg_base[SDCC_REG_SLOT_1] = HWIO_PERIPH_SS_SDC1_SDCC_HC_REG_0_2_ADDR;
  sdhc_reg_phys[SDCC_REG_SLOT_1] = HWIO_PERIPH_SS_SDC1_SDCC_HC_REG_0_2_PHYS;
  sdhc_reg_base[SDCC_REG_SLOT_2] = HWIO_PERIPH_SS_SDC2_SDCC_HC_REG_0_2_ADDR;
  sdhc_reg_phys[SDCC_REG_SLOT_2] = HWIO_PERIPH_SS_SDC2_SDCC_HC_REG_0_2_PHYS;
  sdhc_reg_base[SDCC_REG_SLOT_3] = HWIO_PERIPH_SS_SDC3_SDCC_HC_REG_0_2_ADDR;
  sdhc_reg_phys[SDCC_REG_SLOT_3] = HWIO_PERIPH_SS_SDC3_SDCC_HC_REG_0_2_PHYS;
  sdhc_reg_base[SDCC_REG_SLOT_4] = HWIO_PERIPH_SS_SDC4_SDCC_HC_REG_0_2_ADDR;
  sdhc_reg_phys[SDCC_REG_SLOT_4] = HWIO_PERIPH_SS_SDC4_SDCC_HC_REG_0_2_PHYS;

  hwio_init_done = TRUE;
}

void
HAL_sdcc_SetBusWidth (const uint32 driveno, enum HAL_sdcc_BusWidth width)
{
  uint32 reg_width = 0;

  switch (width)
  {
    case HAL_SDCC_BUS_WIDTH_1_BIT:
      /* 1 bit interface */
      reg_width = MCI_CLK_SET_1BIT_WIDE;
      break;
    case HAL_SDCC_BUS_WIDTH_4_BIT:
      /* 4 bit interface */
      reg_width = MCI_CLK_SET_4BIT_WIDE;
      break;
    case HAL_SDCC_BUS_WIDTH_8_BIT:
      /* 8 bit interface */
      reg_width = MCI_CLK_SET_8BIT_WIDE;
      break;

    default:
      //HAL_MSG_1(HAL_MSG_ERROR, "Bus Width Not Supported", widebus);
      return;
  }

  /* program the bus width */
  HWIO_SDC_REG_OUTMI (MCI_CLK, sdcc_reg_base[driveno], HWIO_FMSK (MCI_CLK, WIDEBUS), reg_width);
  HAL_sdcc_WaitWrActive (driveno);
}

/* Remember that just because the controller supports 8-bit does NOT
 * mean the board layout is actually wired for 8bit on all slots.  7K
 * Surf, for example, allows 8-bit use only on Slot 3. */
uint32
HAL_sdcc_Supports8BitBus (const uint32 driveno)
{
  return 1; /* Good to go. */
}

/**
 * Gets the number of bits in MCI_DATA_LENGTH register
 */
uint32
HAL_sdcc_GetNumBitsDataLength (void)
{
   return 25;
}

