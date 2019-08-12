/*
===========================================================================

FILE:         SITE_Training.c

DESCRIPTION:

Copyright 2012-2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/boot/ddr/hw/controller/SITE_Training.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
06/26/13   sl      Added DDR_ERR_FATAL().
03/19/13   sl      New SITE training algorithm.
01/31/13   sl      MX scaling fix.
01/04/12   tw      Check against cx and mx specific corners
12/06/12   tw      Added support for mx scaling
11/20/12   tw      Added quadrant override as part of clock switch when 
                   switching from JCPLL enable to bypass mode
10/03/12   tw      Updated SITE calibration table. Code size optimization.
09/28/12   tw      Added support for training per jcpll
09/12/12   sl      Use macro-style DDR logging.
06/25/12   tw      Initial version.
===========================================================================
*/

/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "SITE.h"
#include "ddr_target.h"
#include "ddr_params.h"
#include "AHB2PHY.h"
#include "ddr_log.h"


/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
/* =============================================================================
**  Function : HAL_SDRAM_SITE_Compute_Quad
** =============================================================================
*/
/**
*   @brief
*   Compute which quadrants SITE calibration lands on and return whether
*   quadrants are adjacent.
*
*   @param[in]   offset       SITE offset
*   @param[in]   is_for_push  Whether function called for push values or not
*   @param[out]  quad         Computed quadrants
*
*   @retval  TRUE   Quadrants adjacent
*   @retval  FALSE  Quadrants not adjacent
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
static boolean HAL_SDRAM_SITE_Compute_Quad(uint32 offset, boolean is_for_push, uint8 *quad)
{
  static uint32 push_threshold = 0x8000;
  static uint32 merged_threshold = 0x2000;
  uint32 threshold = is_for_push ? push_threshold : merged_threshold;
  uint32 count = 0, maximum = 0;
  boolean is_adjacent = FALSE;

  *quad = 0;

  if ( HWIO_INF(TLMM_HW_REVISION_NUMBER, VERSION_ID) < DDR_TLMM_HW_REV_ID_V12 )
  {
    if ( (count = BOOT_HWIO_IN(SITE_ADDR(CALIB_CNTR_Q1), offset)) > maximum )
    {
      *quad = 0x1;
      maximum = count;
    }
    if ( (count = BOOT_HWIO_IN(SITE_ADDR(CALIB_CNTR_Q2), offset)) > maximum )
    {
      *quad = 0x2;
      maximum = count;
    }
    if ( (count = BOOT_HWIO_IN(SITE_ADDR(CALIB_CNTR_Q3), offset)) > maximum )
    {
      *quad = 0x4;
      maximum = count;
    }
    if ( (count = BOOT_HWIO_IN(SITE_ADDR(CALIB_CNTR_Q4), offset)) > maximum )
    {
      *quad = 0x8;
      maximum = count;
    }
  }
  else
  {
    if (BOOT_HWIO_IN(SITE_ADDR(CALIB_CNTR_Q1), offset) >= threshold) { *quad |= 0x1; }
    if (BOOT_HWIO_IN(SITE_ADDR(CALIB_CNTR_Q2), offset) >= threshold) { *quad |= 0x2; }
    if (BOOT_HWIO_IN(SITE_ADDR(CALIB_CNTR_Q3), offset) >= threshold) { *quad |= 0x4; }
    if (BOOT_HWIO_IN(SITE_ADDR(CALIB_CNTR_Q4), offset) >= threshold) { *quad |= 0x8; }
  }

  switch (*quad)
  {
    case 0x1: case 0x2: case 0x4: case 0x8:
      is_adjacent = FALSE;
      break;

    case 0x3: case 0x6: case 0x9: case 0xC:
      is_adjacent = TRUE;
      break;

    case 0x0:
      DDR_ERR_FATAL();

    default:
      if (is_for_push) { DDR_ERR_FATAL(); }
      break;
  }

  return is_adjacent;
}

/* =============================================================================
**  Function : HAL_SDRAM_SITE_Calibration
** =============================================================================
*/
/**
*   @brief
*   Do SITE training.
*
*   @param[in]   interface    Interface to train for
*   @param[out]  merged_quad  Merged quadrants
*   @param[out]  site_params  SITE trained parameters
*
*   @retval  TRUE   Training successful
*   @retval  FALSE  Training failed
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
boolean HAL_SDRAM_SITE_Calibration(SDRAM_INTERFACE interface, uint8 *merged_quad, struct ddr_site_data *site_params)
{
  uint32 offset;
  ddr_mask ddr_broadcast_mask = {0};
  uint8 merged, quad[5];
  uint32 site;
  boolean push;

  /* Configure SITE offset */
  offset = (interface == SDRAM_INTERFACE_0) ? SITE_0_OFFSET : SITE_1_OFFSET;

  /* Enable broadcast mode on all DQ */
  ddr_broadcast_mask.CA_SITE = TRUE;
  ddr_broadcast_mask.DQ00_SITE = TRUE;
  ddr_broadcast_mask.DQ01_SITE = TRUE;
  ddr_broadcast_mask.DQ02_SITE = TRUE;
  ddr_broadcast_mask.DQ03_SITE = TRUE;

  HAL_SDRAM_AHB2PHY_Broadcast_Enable(interface, ddr_broadcast_mask);

  /* CALIB_CTRL[COUNT_RESET] = 1
   * Reset calibration counters
   * */

  /* broadcast to all DQ */
  BOOT_HWIO_OUTM(SITE_ADDR(CALIB_CTRL), SITE_BCAST_OFFSET,
      HWIO_FMSK(SITE_ADDR(CALIB_CTRL), COUNT_RESET),
      0x1 << HWIO_SHFT(SITE_ADDR(CALIB_CTRL), COUNT_RESET));

  /* CALIB_CTRL[COUNT_RESET] = 0
   * CALIB_CTRL[COUNT_EN] = 1
   * De-assert reset and enable calibration counters
   * */

  /* broadcast to all DQ */
  BOOT_HWIO_OUTM(SITE_ADDR(CALIB_CTRL), SITE_BCAST_OFFSET,
      HWIO_FMSK(SITE_ADDR(CALIB_CTRL), COUNT_RESET) |
      HWIO_FMSK(SITE_ADDR(CALIB_CTRL), COUNT_ENABLE) ,
      0x1 << HWIO_SHFT(SITE_ADDR(CALIB_CTRL), COUNT_ENABLE));

  /* Enable Rapid Calibration from memory controller.
   * After a chosen time interval, disable rapid calibration
   * This will send a calibration pulse from Memory Controller
   * to SITE
   */
  HWIO_OUTM(BIMC_CALIBRATION_CFG,
            HWIO_FMSK(BIMC_CALIBRATION_CFG, RAPID_CAL_ENA),
            0x1 << HWIO_SHFT(BIMC_CALIBRATION_CFG, RAPID_CAL_ENA));

  /* Wait for calibration */

  /* Wait for 1 ms */
  HAL_SDRAM_DDR_Wait(1000, 0);

  /* Turn off calibration */
  HWIO_OUTM(BIMC_CALIBRATION_CFG,
            HWIO_FMSK(BIMC_CALIBRATION_CFG, RAPID_CAL_ENA),
            0x0);

  /* Calib_CTRL[COUNT_ENABLE] = 0
   * Disable Calibration counters*/
  BOOT_HWIO_OUTM(SITE_ADDR(CALIB_CTRL), SITE_BCAST_OFFSET,
      HWIO_FMSK(SITE_ADDR(CALIB_CTRL), COUNT_ENABLE),
      0x0);

  if (merged_quad != NULL)
  {
    for (merged = 0, site = 0; site < 5; site++)
    {
      HAL_SDRAM_SITE_Compute_Quad(offset + SITE_CA_OFFSET + site * SITE_DQ00_OFFSET,
                                  FALSE, &quad[site]);

      /* Compute merged quadrants */
      merged |= quad[site];
    }

    *merged_quad = merged;
  }

  if (site_params != NULL)
  {
    for (merged = 0, site = 0; site < 5; site++)
    {
      HAL_SDRAM_SITE_Compute_Quad(offset + SITE_CA_OFFSET + site * SITE_DQ00_OFFSET,
                                  TRUE, &quad[site]);

      /* Compute merged quadrants */
      merged |= quad[site];
    }

    /* Compute push value */
    for (site = 0; site < 5; site++)
    {
      switch (merged)
      {
        case 0x1: case 0x8: case 0x9:
          push = FALSE;
          break;

        case 0x2: case 0x3: case 0x4: case 0x6:
          push = TRUE;
          break;

        case 0xC: case 0xE:
          push = (quad[site] & 0x2) || (quad[site] & 0x4);
          break;

        default:
          DDR_ERR_FATAL();
      }

      site_params[site].push = push;
    }
  }

  return ( site_params == NULL || (merged & 0x3) != 0x3 );
}

/* =============================================================================
**  Function : HAL_SDRAM_SITE_Force_DLL_Delay
** =============================================================================
*/
/**
*   @brief
*   Enable or disable DLL fixed delay mode.
*
*   @param[in]  jcpll   JCPLL selection
*   @param[in]  enable  To enable or disable
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
void HAL_SDRAM_SITE_Force_DLL_Delay(uint32 jcpll, boolean enable)
{
  uint32 value;

  if ( HWIO_INF(TLMM_HW_REVISION_NUMBER, VERSION_ID) < DDR_TLMM_HW_REV_ID_V12 ) { return; }

  /* Make boolean value either 0 or 1 */
  enable = !!enable;

  if (jcpll == 0)
  {
    value = HWIO_IN(CH0_GCD0_CA_EBI1_DLL_GCD_CFG);
    value = (value & 0xFFFFFDFF) | (1 << 9);
    HWIO_OUT(CH0_GCD0_CA_EBI1_DLL_GCD_CFG, value);

    value = HWIO_IN(CH0_GCD0_D0_EBI1_DLL_GCD_CFG);
    value = (value & 0xFFFFFDFF) | (enable << 9);
    HWIO_OUT(CH0_GCD0_D0_EBI1_DLL_GCD_CFG, value);

    value = HWIO_IN(CH0_GCD0_D1_EBI1_DLL_GCD_CFG);
    value = (value & 0xFFFFFDFF) | (enable << 9);
    HWIO_OUT(CH0_GCD0_D1_EBI1_DLL_GCD_CFG, value);

    value = HWIO_IN(CH0_GCD0_D2_EBI1_DLL_GCD_CFG);
    value = (value & 0xFFFFFDFF) | (enable << 9);
    HWIO_OUT(CH0_GCD0_D2_EBI1_DLL_GCD_CFG, value);

    value = HWIO_IN(CH0_GCD0_D3_EBI1_DLL_GCD_CFG);
    value = (value & 0xFFFFFDFF) | (enable << 9);
    HWIO_OUT(CH0_GCD0_D3_EBI1_DLL_GCD_CFG, value);

    value = HWIO_IN(CH1_GCD0_CA_EBI1_DLL_GCD_CFG);
    value = (value & 0xFFFFFDFF) | (1 << 9);
    HWIO_OUT(CH1_GCD0_CA_EBI1_DLL_GCD_CFG, value);

    value = HWIO_IN(CH1_GCD0_D0_EBI1_DLL_GCD_CFG);
    value = (value & 0xFFFFFDFF) | (enable << 9);
    HWIO_OUT(CH1_GCD0_D0_EBI1_DLL_GCD_CFG, value);

    value = HWIO_IN(CH1_GCD0_D1_EBI1_DLL_GCD_CFG);
    value = (value & 0xFFFFFDFF) | (enable << 9);
    HWIO_OUT(CH1_GCD0_D1_EBI1_DLL_GCD_CFG, value);

    value = HWIO_IN(CH1_GCD0_D2_EBI1_DLL_GCD_CFG);
    value = (value & 0xFFFFFDFF) | (enable << 9);
    HWIO_OUT(CH1_GCD0_D2_EBI1_DLL_GCD_CFG, value);

    value = HWIO_IN(CH1_GCD0_D3_EBI1_DLL_GCD_CFG);
    value = (value & 0xFFFFFDFF) | (enable << 9);
    HWIO_OUT(CH1_GCD0_D3_EBI1_DLL_GCD_CFG, value);
  }
  else if (jcpll == 1)
  {
    value = HWIO_IN(CH0_GCD1_CA_EBI1_DLL_GCD_CFG);
    value = (value & 0xFFFFFDFF) | (1 << 9);
    HWIO_OUT(CH0_GCD1_CA_EBI1_DLL_GCD_CFG, value);

    value = HWIO_IN(CH0_GCD1_D0_EBI1_DLL_GCD_CFG);
    value = (value & 0xFFFFFDFF) | (enable << 9);
    HWIO_OUT(CH0_GCD1_D0_EBI1_DLL_GCD_CFG, value);

    value = HWIO_IN(CH0_GCD1_D1_EBI1_DLL_GCD_CFG);
    value = (value & 0xFFFFFDFF) | (enable << 9);
    HWIO_OUT(CH0_GCD1_D1_EBI1_DLL_GCD_CFG, value);

    value = HWIO_IN(CH0_GCD1_D2_EBI1_DLL_GCD_CFG);
    value = (value & 0xFFFFFDFF) | (enable << 9);
    HWIO_OUT(CH0_GCD1_D2_EBI1_DLL_GCD_CFG, value);

    value = HWIO_IN(CH0_GCD1_D3_EBI1_DLL_GCD_CFG);
    value = (value & 0xFFFFFDFF) | (enable << 9);
    HWIO_OUT(CH0_GCD1_D3_EBI1_DLL_GCD_CFG, value);

    value = HWIO_IN(CH1_GCD1_CA_EBI1_DLL_GCD_CFG);
    value = (value & 0xFFFFFDFF) | (1 << 9);
    HWIO_OUT(CH1_GCD1_CA_EBI1_DLL_GCD_CFG, value);

    value = HWIO_IN(CH1_GCD1_D0_EBI1_DLL_GCD_CFG);
    value = (value & 0xFFFFFDFF) | (enable << 9);
    HWIO_OUT(CH1_GCD1_D0_EBI1_DLL_GCD_CFG, value);

    value = HWIO_IN(CH1_GCD1_D1_EBI1_DLL_GCD_CFG);
    value = (value & 0xFFFFFDFF) | (enable << 9);
    HWIO_OUT(CH1_GCD1_D1_EBI1_DLL_GCD_CFG, value);

    value = HWIO_IN(CH1_GCD1_D2_EBI1_DLL_GCD_CFG);
    value = (value & 0xFFFFFDFF) | (enable << 9);
    HWIO_OUT(CH1_GCD1_D2_EBI1_DLL_GCD_CFG, value);

    value = HWIO_IN(CH1_GCD1_D3_EBI1_DLL_GCD_CFG);
    value = (value & 0xFFFFFDFF) | (enable << 9);
    HWIO_OUT(CH1_GCD1_D3_EBI1_DLL_GCD_CFG, value);
  }
}

/* =============================================================================
**  Function : HAL_SDRAM_SITE_Read_DLL_Delay
** =============================================================================
*/
/**
*   @brief
*   Read DLL delay.
*
*   @param[in]   interface    Interface to read for
*   @param[in]   jcpll        JCPLL selection to read for
*   @param[out]  site_params  SITE trained parameters
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
void HAL_SDRAM_SITE_Read_DLL_Delay(SDRAM_INTERFACE interface, uint32 jcpll, struct ddr_site_data *site_params)
{
  if ( HWIO_INF(TLMM_HW_REVISION_NUMBER, VERSION_ID) < DDR_TLMM_HW_REV_ID_V12 ) { return; }

  if (site_params == NULL)
  {
    /* Invaid trained parameters: log error */
    DDR_LOG(DDR_ERROR, DDR_FATAL_ERROR,
            DDR_SITE_CALIBRATION, TRAINING_PARAMETER_NOT_FOUND);
    DDR_ERR_FATAL();
  }

  /* Note: status register has x2 DLL delay */
  if (interface == SDRAM_INTERFACE_0 && jcpll == 0)
  {
    site_params[0].dll_delay = HWIO_INF(EBI1_ALL_DLL_CA_STATUS, CH0_DLL0_CA_STATUS) >> 1;
    site_params[1].dll_delay = HWIO_INF(CH0_GCD0_EBI1_DLL_DQ_STATUS, GCD0_D0_STATUS) >> 1;
    site_params[2].dll_delay = HWIO_INF(CH0_GCD0_EBI1_DLL_DQ_STATUS, GCD0_D1_STATUS) >> 1;
    site_params[3].dll_delay = HWIO_INF(CH0_GCD0_EBI1_DLL_DQ_STATUS, GCD0_D2_STATUS) >> 1;
    site_params[4].dll_delay = HWIO_INF(CH0_GCD0_EBI1_DLL_DQ_STATUS, GCD0_D3_STATUS) >> 1;
  }
  else if (interface == SDRAM_INTERFACE_0 && jcpll == 1)
  {
    site_params[0].dll_delay = HWIO_INF(EBI1_ALL_DLL_CA_STATUS, CH0_DLL1_CA_STATUS) >> 1;
    site_params[1].dll_delay = HWIO_INF(CH0_GCD1_EBI1_DLL_DQ_STATUS, GCD1_D0_STATUS) >> 1;
    site_params[2].dll_delay = HWIO_INF(CH0_GCD1_EBI1_DLL_DQ_STATUS, GCD1_D1_STATUS) >> 1;
    site_params[3].dll_delay = HWIO_INF(CH0_GCD1_EBI1_DLL_DQ_STATUS, GCD1_D2_STATUS) >> 1;
    site_params[4].dll_delay = HWIO_INF(CH0_GCD1_EBI1_DLL_DQ_STATUS, GCD1_D3_STATUS) >> 1;
  }
  else if (interface == SDRAM_INTERFACE_1 && jcpll == 0)
  {
    site_params[0].dll_delay = HWIO_INF(EBI1_ALL_DLL_CA_STATUS, CH1_DLL0_CA_STATUS) >> 1;
    site_params[1].dll_delay = HWIO_INF(CH1_GCD0_EBI1_DLL_DQ_STATUS, GCD0_D0_STATUS) >> 1;
    site_params[2].dll_delay = HWIO_INF(CH1_GCD0_EBI1_DLL_DQ_STATUS, GCD0_D1_STATUS) >> 1;
    site_params[3].dll_delay = HWIO_INF(CH1_GCD0_EBI1_DLL_DQ_STATUS, GCD0_D2_STATUS) >> 1;
    site_params[4].dll_delay = HWIO_INF(CH1_GCD0_EBI1_DLL_DQ_STATUS, GCD0_D3_STATUS) >> 1;
  }
  else if (interface == SDRAM_INTERFACE_1 && jcpll == 1)
  {
    site_params[0].dll_delay = HWIO_INF(EBI1_ALL_DLL_CA_STATUS, CH1_DLL1_CA_STATUS) >> 1;
    site_params[1].dll_delay = HWIO_INF(CH1_GCD1_EBI1_DLL_DQ_STATUS, GCD1_D0_STATUS) >> 1;
    site_params[2].dll_delay = HWIO_INF(CH1_GCD1_EBI1_DLL_DQ_STATUS, GCD1_D1_STATUS) >> 1;
    site_params[3].dll_delay = HWIO_INF(CH1_GCD1_EBI1_DLL_DQ_STATUS, GCD1_D2_STATUS) >> 1;
    site_params[4].dll_delay = HWIO_INF(CH1_GCD1_EBI1_DLL_DQ_STATUS, GCD1_D3_STATUS) >> 1;
  }
}
