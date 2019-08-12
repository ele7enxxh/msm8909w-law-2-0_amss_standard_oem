/*
===========================================================================

FILE:         AHB2PHY.c

DESCRIPTION:
  This is the driver implementation of ahb2phy broadcast mode

===========================================================================

                             Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/hw/phy/AHB2PHY.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
05/16/11   tw      Initial version.
===========================================================================
             Copyright 2012 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "HAL_DDR.h"
#include "ddr_target.h"
#include "AHB2PHY.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
**                           MACROS
** ----------------------------------------------------------------------- */
#define DDR_AHB2PHY_BROADCAST_OFFSET 0x1F800

#define BOOT_HWIO_BROADCAST_OUTM(base, mask, val)  \
        out_dword_masked_ns((base + DDR_AHB2PHY_BROADCAST_OFFSET),mask , val, \
                            in_dword(base + DDR_AHB2PHY_BROADCAST_OFFSET))

#define BOOT_HWIO_BROADCAST_OUT(base, val)  \
        out_dword_ns((base + DDR_AHB2PHY_BROADCAST_OFFSET), val)

#define BOOT_HWIO_BROADCAST_IN(base)  \
        in_dword((base | DDR_AHB2PHY_BROADCAST_OFFSET))


/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/* ==========================================================================
**  Function : HAL_SDRAM_AHB2PHY_init
** ==========================================================================
*/
/*!
*   @brief
*   This function initializes the AHB2PHY
*
*   @param   None
*
*   @retval  None
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*
*   @sa None
*/

void HAL_SDRAM_AHB2PHY_init(void)
{
}

/* ==========================================================================
**  Function : HAL_SDRAM_AHB2PHY_Broadcast_Enable
** ==========================================================================
*/
/*!
*   @brief
*   This function initializes the AHB2PHY
*
*   @param interface_name   -  [IN] The interface to enable broadcast mode
*   @param mask             -  [IN] The broadcast mask
*
*
*   @retval  None
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*
*   @sa None
*/
void HAL_SDRAM_AHB2PHY_Broadcast_Enable(SDRAM_INTERFACE interface_name, ddr_mask mask)
{
  uint32 ahb2phy_mask = 0;
  /* Generate CS mask for AHB phy based on the mask provided */

  if(mask.CA_PHY)
  {
    ahb2phy_mask |= 0x1;
  }
  if(mask.DQ00_PHY)
  {
    ahb2phy_mask |= 0x2;
  }
  if(mask.DQ01_PHY)
  {
    ahb2phy_mask |= 0x4;
  }
  if(mask.DQ02_PHY)
  {
    ahb2phy_mask |= 0x8;
  }
  if(mask.DQ03_PHY)
  {
    ahb2phy_mask |= 0x10;
  }

  if(mask.CA_SITE)
  {
    ahb2phy_mask |= 0x20;
  }
  if(mask.DQ00_SITE)
  {
    ahb2phy_mask |= 0x40;
  }
  if(mask.DQ01_SITE)
  {
    ahb2phy_mask |= 0x80;
  }
  if(mask.DQ02_SITE)
  {
    ahb2phy_mask |= 0x100;
  }
  if(mask.DQ03_SITE)
  {
    ahb2phy_mask |= 0x200;
  }

  if(interface_name == SDRAM_INTERFACE_1)
  {
    /* shift bit mask to appropriate cs */
    ahb2phy_mask <<= 10;
  }

  /* Set AHB2PHY chip select enable for the broadcast */
  /* upper 32bit enable is not used */
  HWIO_OUT(EBI1_AHB2PHY_BROADCAST_EN_CFG_LOWER, ahb2phy_mask);

}

void HAL_SDRAM_AHB2PHY_Broadcast_Disable()
{
  /* Set AHB2PHY chip select enable for the broadcast to be 0 to disable */
  /* upper 32bit enable is not used */
  HWIO_OUT(EBI1_AHB2PHY_BROADCAST_EN_CFG_LOWER, 0x0);
}

