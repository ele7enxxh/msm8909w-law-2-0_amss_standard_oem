/**********************************************************************
* HALsdcc_8916.c
*
* SDCC(Secure Digital Card Controller) driver HAL Platform Specific API.
*
* This file implements the SDCC driver HAL Platform Specific APIs.
*
* Copyright (c) 2013-2014
* Qualcomm Technologies Incorporated.
* All Rights Reserved.
* Qualcomm Confidential and Proprietary
*
**********************************************************************/
/*=======================================================================
                        Edit History

when        who     what, where, why
----------------------------------------------------------------------
2014-11-11  sb      Fixed SetCapabilities
2014-05-16  sb      Add SDHCi SetCapabilities
2014-11-03  pa      Code cleanup
2013-11-05  pk      Branched from 8974 HAL file
======================================================================*/
#include SDCC_OSAL_H
#include SDCC_HWIO_H
#include "HALsdcc.h"
#include "HALsdcc_sdcc4.h"
#include "sdcc_hwio_8916.h"
#include "HALsdhci.h"
#include "sdcc_bsp.h"

/** Number of SD controllers present in the MSM */
#define HAL_SDCC_CONTROLLER_COUNT    2

#define SDCC_REG_SLOT_1         0
#define SDCC_REG_SLOT_2         1

#define HAL_SDHCI_CAPS_BASE_SDCLK_FREQ_200MHZ        0x0000C800 /* Bit[8:15] inclusive. Caps0 */
#define HAL_SDHCI_CAPS_SUPPORT_8_BIT_ENABLE          0x00040000 /* Bit18. Caps0 */
#define HAL_SDHCI_CAPS_ADMA2_SUPPORT_ENABLE          0x00080000 /* Bit19. Caps0*/
#define HAL_SDHCI_CAPS_HS_SUPPORT_ENABLE             0x00200000 /* Bit21. Caps0 */
#define HAL_SDHCI_CAPS_SDR_50_SUPPORT_ENABLE         0x00000001 /* Bit0. Caps1 */
#define HAL_SDHCI_CAPS_SDR_104_SUPPORT_ENABLE        0x00000002 /* Bit1. Caps1 */
#define HAL_SDHCI_CAPS_DDR_50_SUPPORT_ENABLE         0x00000004 /* Bit2. Caps1 */
#define HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_1_8V_ENABLE   0x04000000 /* Bit26. Caps0 */
#define HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_3_0V_ENABLE   0x02000000 /* Bit25. Caps0 */
#define HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_3_3V_ENABLE   0x01000000 /* Bit24. Caps0 */
#define HAL_SDHCI_CAPS_SLOT_TYPE_EMBEDDED_SLOT       0x40000000 /* Bit[30:31]. Caps0 */
#define HAL_SDHCI_CAPS_SLOT_TYPE_SD_SLOT             0x00000000 /* Bit[30:31]. Caps0 */

#define HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_1_8V_DISABLE  0x00000000
#define HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_3_0V_DISABLE  0x00000000
#define HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_3_3V_DISABLE  0x00000000
#define HAL_SDHCI_CAPS_SUPPORT_8_BIT_DISABLE         0x00000000
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
  sdcc_reg_base[SDCC_REG_SLOT_2] = SDC2_SDCC5_REG_BASE;
  sdcc_reg_phys[SDCC_REG_SLOT_2] = SDC2_SDCC5_REG_BASE_PHYS;

  sdhc_reg_base[SDCC_REG_SLOT_1] = SDCC5_HC_REG_BASE;
  sdhc_reg_phys[SDCC_REG_SLOT_1] = SDCC5_HC_REG_BASE_PHYS;
  sdhc_reg_base[SDCC_REG_SLOT_2] = SDC2_SDCC5_HC_REG_BASE;
  sdhc_reg_phys[SDCC_REG_SLOT_2] = SDC2_SDCC5_HC_REG_BASE_PHYS;

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

#if 0
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

#endif

/**
 * Reg Offset: 0x11C and 0x120.
 * Set the SDHCi Capabitilities. 
 * Each SDCC controller supports a specific set of features.
 * Setting the bits in the HC_VENDOR_SPECIFIC_CAPABILITIES0 and 
 * HC_VENDOR_SPECIFIC_CAPABILITIES1 to indicate the features that are supported
 * in this SDCC controller.
 * Note that the setting of the obove mentioned registers will be reflected in the read-only bits of
 * the Capbilities registers (register offset 0x40-0x46)
 * 
 */
void HAL_sdhci_SetCapabilities (const uint32 nControllerId)
{
  uint32 caps0 = 0, caps1 = 0;
  sdcc_bsp_slot_type slot_type = sdcc_bsp_get_slot_type(nControllerId);

  caps0 = in_dword (sdhc_reg_base[nControllerId] + HAL_SDHCI_REG_40_42_OFFSET);
  caps1 = in_dword (sdhc_reg_base[nControllerId] + HAL_SDHCI_REG_44_46_OFFSET);

  if ((slot_type & SDCC_BSP_EMBEDDED_MEM_SLOT) || 
      (slot_type & SDCC_BSP_OPEN_SLOT))
  {
    /* In addition to the default setting of the Capabilities, */
    /* make sure the following features are supported */

    /* Support ADMA2 */
    caps0 &= ~HAL_SDHCI_CAPS_ADMA2_SUPPORT_BMSK;
    caps0 |= HAL_SDHCI_CAPS_ADMA2_SUPPORT_ENABLE;
	
    /* Support High Speed */
    caps0 &= ~HAL_SDHCI_CAPS_HS_SUPPORT_BMSK;
    caps0 |= HAL_SDHCI_CAPS_HS_SUPPORT_ENABLE;

    /* Support SDR50 */
    caps1 &= ~HAL_SDHCI_CAPS_SDR_50_SUPPORT_BMSK;
    caps1 |= HAL_SDHCI_CAPS_SDR_50_SUPPORT_ENABLE;

    /* Support SDR104 */
    caps1 &= ~HAL_SDHCI_CAPS_SDR_104_SUPPORT_BMSK;
    caps1 |= HAL_SDHCI_CAPS_SDR_104_SUPPORT_ENABLE;

    /* Support DDR50 */
    caps1 &= ~HAL_SDHCI_CAPS_DDR_50_SUPPORT_BMSK;
    caps1 |= HAL_SDHCI_CAPS_DDR_50_SUPPORT_ENABLE;
  }
  
  if (slot_type & SDCC_BSP_EMBEDDED_MEM_SLOT)
  {
    /* Support 8-bit bus */
    caps0 &= ~HAL_SDHCI_CAPS_SUPPORT_8_BIT_BMSK;
    caps0 |= HAL_SDHCI_CAPS_SUPPORT_8_BIT_ENABLE;

    /* Support 1.8V for eMMC only. No support for 3.0V and 3.3V */
    caps0 &= ~HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_3_0V_BMSK;
    caps0 |= HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_3_0V_DISABLE;

    caps0 &= ~HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_3_3V_BMSK;
    caps0 |= HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_3_3V_DISABLE;

    caps0 &= ~HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_1_8V_BMSK;
    caps0 |= HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_1_8V_ENABLE;

    /* Embedded slot */
    caps0 &= ~HAL_SDHCI_CAPS_SLOT_TYPE_BMSK;
    caps0 |= HAL_SDHCI_CAPS_SLOT_TYPE_EMBEDDED_SLOT;
  }
  else if (slot_type & SDCC_BSP_OPEN_SLOT)   
  {
    /* Support 3.0V and 3.3V for external SD card only. */
    /* No support for 1.8V power supply to SD card */
    caps0 &= ~HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_3_0V_BMSK;
    caps0 |= HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_3_0V_ENABLE;

    caps0 &= ~HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_3_3V_BMSK;
    caps0 |= HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_3_3V_ENABLE;

    caps0 &= ~HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_1_8V_BMSK;
    caps0 |= HAL_SDHCI_CAPS_VOLTAGE_SUPPORT_1_8V_DISABLE;
    
    /* External SD slot */
    caps0 &= ~HAL_SDHCI_CAPS_SLOT_TYPE_BMSK;
    caps0 |= HAL_SDHCI_CAPS_SLOT_TYPE_SD_SLOT;
  }
  out_dword ((sdhc_reg_base[nControllerId] +
              HAL_SDHCI_VENDOR_CAPS0_OFFSET), caps0);

  out_dword ((sdhc_reg_base[nControllerId] +
              HAL_SDHCI_VENDOR_CAPS1_OFFSET), caps1);
  
  return;
}

