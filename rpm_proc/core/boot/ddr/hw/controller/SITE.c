/*
===========================================================================

FILE:         SITE.c

DESCRIPTION:  
  This is the ddr rpm message driver that will handle the rpm message 
  requests. There is an init function to register with the rpmfw with the
  callback function, and the callback function that will handle the message
  requests.

Copyright 2012-2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/boot/ddr/hw/controller/SITE.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
07/15/13   tw      Force push for site bypass mode
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
#include "HAL_DDR.h"
#include "ddr_target.h"
#include "ddr_params.h"
#include "AHB2PHY.h"
#include "ddr_log.h"
#include "Clock.h"


/*==============================================================================
                                  DATA
==============================================================================*/
extern ddr_interface_state ddr_status;

extern uint32 railway_voltage_cx_corners[];
extern uint32 railway_voltage_mx_corners[];


/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
/* ==========================================================================
**  Function : HAL_SDRAM_SITE_Update_Calibration
** ==========================================================================
*/
/*!
*   @brief
*   This function updates the site calibration to match new clock speed
*
*   @param interface_name   -  [IN] interface to update site calibration for
*   @param new_clk          -  [IN] New clock speed to calibrate for
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
boolean HAL_SDRAM_SITE_Update_Calibration(SDRAM_INTERFACE interface, uint32 new_clk, uint32 jcpll)
{
  struct ddr_trained_params_lpddr3 *trained_params;
  uint32 offset;
  uint32 freq = 0, vddcx = 0, vddmx = 0;
  struct ddr_site_data *site_params;
  uint32 site;
  uint32 value;

  /* Get DDR Trained parameters */
  trained_params = &(ddr_get_trained_params(interface)->lpddr3_training);

  if (trained_params == NULL)
  {
    /* Could not find trained parameters: log error and return FALSE */
    DDR_LOG(DDR_ERROR, DDR_FATAL_ERROR,
            DDR_SITE_UPDATE_CALIBRATION, TRAINING_PARAMETER_NOT_FOUND);
    return FALSE;
  }

  /* Configure SITE offset */
  offset = (interface == SDRAM_INTERFACE_0) ? SITE_0_OFFSET : SITE_1_OFFSET;

  while ((freq < DDR_FREQ_MAX_NUM_LEVELS)
      && (trained_params->freq[freq] != new_clk))
  {
    freq++;
  };

  if(freq == DDR_FREQ_MAX_NUM_LEVELS)
  {
    /* did not find trained params, log error and return false*/
    DDR_LOG(DDR_ERROR, DDR_FATAL_ERROR,
            DDR_SITE_UPDATE_CALIBRATION, SITE_CALIBRATION_NOT_FOUND);
    return FALSE;
  }

  /* find the first voltage level that is lesser than or equal to a railway voltage corner
   * This is to accommodate CPR's dynamically adjusted voltage levels.
   */
  while ((vddcx < DDR_CX_VOLTAGE_MAX_NUM)
      && (ddr_status.vddcx_voltage) > railway_voltage_cx_corners[trained_params->cx_voltage[vddcx]])
  {
    vddcx++;
  };

  if(vddcx == DDR_CX_VOLTAGE_MAX_NUM)
  {
    /* did not find trained params, log error and return false*/
    DDR_LOG(DDR_ERROR, DDR_FATAL_ERROR,
            DDR_SITE_UPDATE_CALIBRATION, SITE_CALIBRATION_NOT_FOUND);
    return FALSE;
  }

  while ((vddmx < DDR_MX_VOLTAGE_MAX_NUM)
      && (ddr_status.vddmx_voltage) > railway_voltage_mx_corners[trained_params->mx_voltage[vddmx]])
  {
    vddmx++;
  };

  if(vddmx == DDR_MX_VOLTAGE_MAX_NUM)
  {
    /* did not find trained params, log error and return false*/
    DDR_LOG(DDR_ERROR, DDR_FATAL_ERROR,
            DDR_SITE_UPDATE_CALIBRATION, SITE_CALIBRATION_NOT_FOUND);
    return FALSE;
  }

  /* having found site calibration that satisfies both
   * voltage and clock, load trained value */

  site_params = trained_params->site_params[freq][jcpll][vddcx][vddmx];

  /* loop through CA, DQ0-DQ3 and apply site calibrated values for each one */
  for (site = 0; site < 5; site++)
  {
    BOOT_HWIO_OUTM(SITE_ADDR(ALIGN_CONFIG), offset + SITE_CA_OFFSET + (site * SITE_DQ00_OFFSET),
                   HWIO_FMSK(SITE_ADDR(ALIGN_CONFIG), PUSH),
                   site_params[site].push << HWIO_SHFT(SITE_ADDR(ALIGN_CONFIG), PUSH));
  }

  if ( HWIO_INF(TLMM_HW_REVISION_NUMBER, VERSION_ID) >= DDR_TLMM_HW_REV_ID_V12 )
  {
    if (interface == SDRAM_INTERFACE_0 && jcpll == 0)
    {
      value = HWIO_IN(CH0_GCD0_CA_EBI1_DLL_GCD_CFG);
      value = (value & 0xFFFF03FF) | (site_params[0].dll_delay << 10);
      HWIO_OUT(CH0_GCD0_CA_EBI1_DLL_GCD_CFG, value);

      value = HWIO_IN(CH0_GCD0_D0_EBI1_DLL_GCD_CFG);
      value = (value & 0xFFFF03FF) | (site_params[1].dll_delay << 10);
      HWIO_OUT(CH0_GCD0_D0_EBI1_DLL_GCD_CFG, value);

      value = HWIO_IN(CH0_GCD0_D1_EBI1_DLL_GCD_CFG);
      value = (value & 0xFFFF03FF) | (site_params[2].dll_delay << 10);
      HWIO_OUT(CH0_GCD0_D1_EBI1_DLL_GCD_CFG, value);

      value = HWIO_IN(CH0_GCD0_D2_EBI1_DLL_GCD_CFG);
      value = (value & 0xFFFF03FF) | (site_params[3].dll_delay << 10);
      HWIO_OUT(CH0_GCD0_D2_EBI1_DLL_GCD_CFG, value);

      value = HWIO_IN(CH0_GCD0_D3_EBI1_DLL_GCD_CFG);
      value = (value & 0xFFFF03FF) | (site_params[4].dll_delay << 10);
      HWIO_OUT(CH0_GCD0_D3_EBI1_DLL_GCD_CFG, value);
    }
    else if (interface == SDRAM_INTERFACE_0 && jcpll == 1)
    {
      value = HWIO_IN(CH0_GCD1_CA_EBI1_DLL_GCD_CFG);
      value = (value & 0xFFFF03FF) | (site_params[0].dll_delay << 10);
      HWIO_OUT(CH0_GCD1_CA_EBI1_DLL_GCD_CFG, value);

      value = HWIO_IN(CH0_GCD1_D0_EBI1_DLL_GCD_CFG);
      value = (value & 0xFFFF03FF) | (site_params[1].dll_delay << 10);
      HWIO_OUT(CH0_GCD1_D0_EBI1_DLL_GCD_CFG, value);

      value = HWIO_IN(CH0_GCD1_D1_EBI1_DLL_GCD_CFG);
      value = (value & 0xFFFF03FF) | (site_params[2].dll_delay << 10);
      HWIO_OUT(CH0_GCD1_D1_EBI1_DLL_GCD_CFG, value);

      value = HWIO_IN(CH0_GCD1_D2_EBI1_DLL_GCD_CFG);
      value = (value & 0xFFFF03FF) | (site_params[3].dll_delay << 10);
      HWIO_OUT(CH0_GCD1_D2_EBI1_DLL_GCD_CFG, value);

      value = HWIO_IN(CH0_GCD1_D3_EBI1_DLL_GCD_CFG);
      value = (value & 0xFFFF03FF) | (site_params[4].dll_delay << 10);
      HWIO_OUT(CH0_GCD1_D3_EBI1_DLL_GCD_CFG, value);
    }
    else if (interface == SDRAM_INTERFACE_1 && jcpll == 0)
    {
      value = HWIO_IN(CH1_GCD0_CA_EBI1_DLL_GCD_CFG);
      value = (value & 0xFFFF03FF) | (site_params[0].dll_delay << 10);
      HWIO_OUT(CH1_GCD0_CA_EBI1_DLL_GCD_CFG, value);

      value = HWIO_IN(CH1_GCD0_D0_EBI1_DLL_GCD_CFG);
      value = (value & 0xFFFF03FF) | (site_params[1].dll_delay << 10);
      HWIO_OUT(CH1_GCD0_D0_EBI1_DLL_GCD_CFG, value);

      value = HWIO_IN(CH1_GCD0_D1_EBI1_DLL_GCD_CFG);
      value = (value & 0xFFFF03FF) | (site_params[2].dll_delay << 10);
      HWIO_OUT(CH1_GCD0_D1_EBI1_DLL_GCD_CFG, value);

      value = HWIO_IN(CH1_GCD0_D2_EBI1_DLL_GCD_CFG);
      value = (value & 0xFFFF03FF) | (site_params[3].dll_delay << 10);
      HWIO_OUT(CH1_GCD0_D2_EBI1_DLL_GCD_CFG, value);

      value = HWIO_IN(CH1_GCD0_D3_EBI1_DLL_GCD_CFG);
      value = (value & 0xFFFF03FF) | (site_params[4].dll_delay << 10);
      HWIO_OUT(CH1_GCD0_D3_EBI1_DLL_GCD_CFG, value);
    }
    else if (interface == SDRAM_INTERFACE_1 && jcpll == 1)
    {
      value = HWIO_IN(CH1_GCD1_CA_EBI1_DLL_GCD_CFG);
      value = (value & 0xFFFF03FF) | (site_params[0].dll_delay << 10);
      HWIO_OUT(CH1_GCD1_CA_EBI1_DLL_GCD_CFG, value);

      value = HWIO_IN(CH1_GCD1_D0_EBI1_DLL_GCD_CFG);
      value = (value & 0xFFFF03FF) | (site_params[1].dll_delay << 10);
      HWIO_OUT(CH1_GCD1_D0_EBI1_DLL_GCD_CFG, value);

      value = HWIO_IN(CH1_GCD1_D1_EBI1_DLL_GCD_CFG);
      value = (value & 0xFFFF03FF) | (site_params[2].dll_delay << 10);
      HWIO_OUT(CH1_GCD1_D1_EBI1_DLL_GCD_CFG, value);

      value = HWIO_IN(CH1_GCD1_D2_EBI1_DLL_GCD_CFG);
      value = (value & 0xFFFF03FF) | (site_params[3].dll_delay << 10);
      HWIO_OUT(CH1_GCD1_D2_EBI1_DLL_GCD_CFG, value);

      value = HWIO_IN(CH1_GCD1_D3_EBI1_DLL_GCD_CFG);
      value = (value & 0xFFFF03FF) | (site_params[4].dll_delay << 10);
      HWIO_OUT(CH1_GCD1_D3_EBI1_DLL_GCD_CFG, value);
    }
  }

  return TRUE;
}

/* ==========================================================================
**  Function : HAL_SDRAM_SITE_Reset_Calibration
** ==========================================================================
*/
/*!
*   @brief
*   This function resets the push/pull configuration to no push/pull
*
*   @param interface_name   -  [IN] interface to update site calibration for
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

void HAL_SDRAM_SITE_Reset_Calibration(SDRAM_INTERFACE interface_name)
{
  uint32 offset;
  uint32 i;

  /* Configure for correct offset */
  if(interface_name == SDRAM_INTERFACE_0)
  {
    offset = SITE_0_OFFSET;
  }
  else
  {
    offset = SITE_1_OFFSET;
  }

  for (i = 0; i < 5; i++)
  {
    BOOT_HWIO_OUTM(SITE_ADDR(ALIGN_CONFIG), offset + SITE_CA_OFFSET + (SITE_DQ00_OFFSET * i),
                   HWIO_FMSK(SITE_ADDR(ALIGN_CONFIG), PUSH),
                   0x1U << HWIO_SHFT(SITE_ADDR(ALIGN_CONFIG), PUSH));

    BOOT_HWIO_OUTM(SITE_ADDR(ALIGN_CONFIG), offset + SITE_CA_OFFSET + (SITE_DQ00_OFFSET * i),
                   HWIO_FMSK(SITE_ADDR(ALIGN_CONFIG), QUADRANT_OVERRIDE),
                   0x1U << HWIO_SHFT(SITE_ADDR(ALIGN_CONFIG), QUADRANT_OVERRIDE));
  }
}

/* ==========================================================================
**  Function : HAL_SDRAM_SITE_Reset_Quadrant_Override
** ==========================================================================
*/
/*!
*   @brief
*   This function clears the quadrant override
*
*   @param interface_name   -  [IN] interface to clear site quadrant override for
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
void HAL_SDRAM_SITE_Reset_Quadrant_Override(SDRAM_INTERFACE interface_name)
{
  uint32 offset;
  uint32 i;

  /* Configure for correct offset */
  if(interface_name == SDRAM_INTERFACE_0)
  {
    offset = SITE_0_OFFSET;
  }
  else
  {
    offset = SITE_1_OFFSET;
  }

  for (i = 0; i < 5; i++)
  {
    /* Reset Quadrant Override */
    BOOT_HWIO_OUTM(SITE_ADDR(ALIGN_CONFIG), offset + SITE_CA_OFFSET + (SITE_DQ00_OFFSET * i),
                   HWIO_FMSK(SITE_ADDR(ALIGN_CONFIG), QUADRANT_OVERRIDE),
                   0x0U << HWIO_SHFT(SITE_ADDR(ALIGN_CONFIG), QUADRANT_OVERRIDE));
  }
}

/* ==========================================================================
**  Function : HAL_SDRAM_SITE_Enable_Rapid_Calibration
** ==========================================================================
*/
/*!
*   @brief
*   This function turns on SITE Rapid calibration
*
*   @param None
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

void HAL_SDRAM_SITE_Enable_Rapid_Calibration(void)
{
  /* Turn on rapid site calibration */
  HWIO_OUTM(BIMC_CALIBRATION_CFG,
            HWIO_FMSK(BIMC_CALIBRATION_CFG, RAPID_CAL_ENA),
            1u << HWIO_SHFT(BIMC_CALIBRATION_CFG, RAPID_CAL_ENA));
}

/* ==========================================================================
**  Function : HAL_SDRAM_SITE_Disable_Rapid_Calibration
** ==========================================================================
*/
/*!
*   @brief
*   This function turns off SITE Rapid calibration
*
*   @param None
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

void HAL_SDRAM_SITE_Disable_Rapid_Calibration(void)
{
  /* Turn off rapid site calibration */
  HWIO_OUTM(BIMC_CALIBRATION_CFG,
            HWIO_FMSK(BIMC_CALIBRATION_CFG, RAPID_CAL_ENA),
            0);
}

/* =============================================================================
**  Function : HAL_SDRAM_SITE_Toggle_DLL_Bypass
** =============================================================================
*/
/**
*   @brief
*   To enable or disable DLL bypass mode.
*
*   @param[in]   jcpll   JCPLL selection to toggle for
*   @param[in]   enable  To enable or disable
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
void HAL_SDRAM_SITE_Toggle_DLL_Bypass(uint32 jcpll, boolean enable)
{
  uint32 value;

  if ( HWIO_INF(TLMM_HW_REVISION_NUMBER, VERSION_ID) < DDR_TLMM_HW_REV_ID_V12 ) { return; }

  value = enable ? 0x306 : 0x106;

  if(!jcpll)
  {
    HWIO_OUT(CH0_GCD0_CA_EBI1_DLL_GCD_USR, value);
    HWIO_OUT(CH0_GCD0_D0_EBI1_DLL_GCD_USR, value);
    HWIO_OUT(CH0_GCD0_D1_EBI1_DLL_GCD_USR, value);
    HWIO_OUT(CH0_GCD0_D2_EBI1_DLL_GCD_USR, value);
    HWIO_OUT(CH0_GCD0_D3_EBI1_DLL_GCD_USR, value);
    HWIO_OUT(CH1_GCD0_CA_EBI1_DLL_GCD_USR, value);
    HWIO_OUT(CH1_GCD0_D0_EBI1_DLL_GCD_USR, value);
    HWIO_OUT(CH1_GCD0_D1_EBI1_DLL_GCD_USR, value);
    HWIO_OUT(CH1_GCD0_D2_EBI1_DLL_GCD_USR, value);
    HWIO_OUT(CH1_GCD0_D3_EBI1_DLL_GCD_USR, value);
  }
  else
  {
    HWIO_OUT(CH0_GCD1_CA_EBI1_DLL_GCD_USR, value);
    HWIO_OUT(CH0_GCD1_D0_EBI1_DLL_GCD_USR, value);
    HWIO_OUT(CH0_GCD1_D1_EBI1_DLL_GCD_USR, value);
    HWIO_OUT(CH0_GCD1_D2_EBI1_DLL_GCD_USR, value);
    HWIO_OUT(CH0_GCD1_D3_EBI1_DLL_GCD_USR, value);
    HWIO_OUT(CH1_GCD1_CA_EBI1_DLL_GCD_USR, value);
    HWIO_OUT(CH1_GCD1_D0_EBI1_DLL_GCD_USR, value);
    HWIO_OUT(CH1_GCD1_D1_EBI1_DLL_GCD_USR, value);
    HWIO_OUT(CH1_GCD1_D2_EBI1_DLL_GCD_USR, value);
    HWIO_OUT(CH1_GCD1_D3_EBI1_DLL_GCD_USR, value);
  }
}
