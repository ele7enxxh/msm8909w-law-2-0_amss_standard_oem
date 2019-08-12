/**********************************************************************
* HALsdcc_9x45.c
*
* SDCC(Secure Digital Card Controller) driver HAL Platform Specific API.
*
* This file implements the SDCC driver HAL Platform Specific APIs.
*
* Copyright (c) 2014
* Qualcomm Technologies Incorporated.
* All Rights Reserved.
* Qualcomm Confidential and Proprietary
*
**********************************************************************/
/*=======================================================================
                        Edit History

when        who     what, where, why
----------------------------------------------------------------------
2014-07-11  sb      Initial Version
======================================================================*/
#include SDCC_OSAL_H
#include SDCC_HWIO_H
#include "HALsdcc.h"
#include "HALsdcc_sdcc4.h"
#include "sdcc_hwio_9x45.h"
#include "HALsdhci.h"

/** Number of SD controllers present in the MSM */
#define HAL_SDCC_CONTROLLER_COUNT    1

#define SDCC_REG_SLOT_1         0

#define HAL_SDHCI_CAPS_BASE_SDCLK_FREQ_200MHZ  0x0000C800
#define HAL_SDHCI_CAPS_SUPPORT_8_BIT_ENABLE    0x00040000
#define HAL_SDHCI_CAPS_ADMA2_SUPPORT_ENABLE    0x00080000
#define HAL_SDHCI_CAPS_HS_SUPPORT_ENABLE       0x00200000
#define HAL_SDHCI_CAPS_SDR_50_SUPPORT_ENABLE   0x00000001
#define HAL_SDHCI_CAPS_SDR_104_SUPPORT_ENABLE  0x00000002
#define HAL_SDHCI_CAPS_DDR_50_SUPPORT_ENABLE   0x00000004

/*
 * Initialize the HWIO address table for access to hwio regs using the
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

  sdcc_reg_base[SDCC_REG_SLOT_1] = SDC1_SDCC5_REG_BASE;
  sdcc_reg_phys[SDCC_REG_SLOT_1] = SDC1_SDCC5_REG_BASE_PHYS;

  sdhc_reg_base[SDCC_REG_SLOT_1] = SDC1_SDCC5_HC_REG_BASE;
  sdhc_reg_phys[SDCC_REG_SLOT_1] = SDC1_SDCC5_HC_REG_BASE_PHYS;

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

/**
 * Set the SDHCi Capabitilities
 */
void
HAL_sdhci_SetCapabilities (const uint32 driveno)
{
  uint32 caps0 = 0, caps1 = 0;

  caps0 = in_dword (sdhc_reg_base[driveno] + HAL_SDHCI_REG_40_42_OFFSET);
  caps1 = in_dword (sdhc_reg_base[driveno] + HAL_SDHCI_REG_44_46_OFFSET);

  /* In addition to the default setting of the Capabilities, */
  /* make sure the following supports are enabled */
 
  /* Set the Base Clock Frequency to 200MHz */
  caps0 &= ~HAL_SDHCI_CAPS_BASE_SDCLK_FREQ_BMSK;
  caps0 |= HAL_SDHCI_CAPS_BASE_SDCLK_FREQ_200MHZ;

  /* Support 8-bit bus */
  caps0 &= ~HAL_SDHCI_CAPS_SUPPORT_8_BIT_BMSK;
  caps0 |= HAL_SDHCI_CAPS_SUPPORT_8_BIT_ENABLE;

  /* Support ADMA2 */
  caps0 &= ~HAL_SDHCI_CAPS_ADMA2_SUPPORT_BMSK;
  caps0 |= HAL_SDHCI_CAPS_ADMA2_SUPPORT_ENABLE;

  /* Support High Speed */
  caps0 &= ~HAL_SDHCI_CAPS_HS_SUPPORT_BMSK;
  caps0 |= HAL_SDHCI_CAPS_HS_SUPPORT_ENABLE;
  out_dword ((sdhc_reg_base[driveno] +
              HAL_SDHCI_REG_40_42_OFFSET), caps0);

  /* Support SDR50 */
  caps1 &= ~HAL_SDHCI_CAPS_SDR_50_SUPPORT_BMSK;
  caps1 |= HAL_SDHCI_CAPS_SDR_50_SUPPORT_ENABLE;

  /* Support SDR104 */
  caps1 &= ~HAL_SDHCI_CAPS_SDR_104_SUPPORT_BMSK;
  caps1 |= HAL_SDHCI_CAPS_SDR_104_SUPPORT_ENABLE;

  /* Support DDR50 */
  caps1 &= ~HAL_SDHCI_CAPS_DDR_50_SUPPORT_BMSK;
  caps1 |= HAL_SDHCI_CAPS_DDR_50_SUPPORT_ENABLE;
  out_dword ((sdhc_reg_base[driveno] +
              HAL_SDHCI_REG_44_46_OFFSET), caps1);
  return;
}
