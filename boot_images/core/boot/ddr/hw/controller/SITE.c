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

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/hw/controller/SITE.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
05/13/13   sl      Updated delta table calls.
03/01/13   tw      Added calling delta table
10/04/12   tw      SITE initialization JCPLL workaround
09/12/12   sl      Use macro-style DDR logging.
06/25/12   tw      Initial version.
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */
#include "SITE.h"
#include "HAL_DDR.h"
#include "ddr_internal.h"
#include "ddr_target.h"
#include "ddr_params.h"
#include "AHB2PHY.h"
#include "ddr_log.h"
#include "ddr_config.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
**                           MACROS
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */  
extern ddr_interface_state ddr_status;
/* -----------------------------------------------------------------------      
**                           FUNCTIONS                                          
** ----------------------------------------------------------------------- */  

/* ==========================================================================
**  Function : HAL_SDRAM_SITE_Calibration
** ==========================================================================
*/
/*!
*   @brief
*   This function Calibrates the SITE
*
*   @param   interface_name    - [IN] interface to calibrate for
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

boolean HAL_SDRAM_SITE_Calibration(SDRAM_INTERFACE interface_name, uint32 frequency_level, uint32 voltage_level)
{
  /* STUB */
  return TRUE;
}


/* ==========================================================================
**  Function : HAL_SDRAM_SITE_push_calc
** ==========================================================================
*/
/*!
*   @brief
*   This function computes whether we need to push or pull the quadrant
*
*   @param   interface_name     - [IN] interface to calibrate for
*   @param   offset             - [IN] the offset for SITE (CA through dQ)
*   @param   raw_cal_count      - [IN] the raw calcualted count to save
*
*   @retval  uint32 - return the quadrant computed
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*
*   @sa None
*/
boolean HAL_SDRAM_SITE_push_calc(uint32 merged_quad, boolean site_quadrant, uint16 *raw_cal_count)
{
  /* STUB */
  return TRUE;
}

/* ==========================================================================
**  Function : HAL_SDRAM_SITE_quadCompute
** ==========================================================================
*/
/*!
*   @brief
*   This function computes which quadrant the site calibration lands on
*
*   @param   interface_name     - [IN] interface to calibrate for
*   @param   offset             - [IN] the offset for SITE (CA through dQ)
*   @param   raw_cal_count      - [IN] the raw calcualted count to save
*
*   @retval  uint32 - return the quadrant computed
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*
*   @sa None
*/

uint8 HAL_SDRAM_SITE_quadCompute(uint32  interface_offset, uint32 offset, uint16 *raw_cal_count)
{
  /* STUB */
  return TRUE;
}

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

boolean HAL_SDRAM_SITE_Update_Calibration(SDRAM_INTERFACE interface_name, uint32 new_clk)
{
  /* STUB */
  return TRUE;
}

/* ==========================================================================
**  Function : SITE_Initialization
** ==========================================================================
*/
/*!
*   @brief
*   This function toggles the site calibration by force loading quadrant 1
*
*   @param interface_name   -  [IN] interface to update site calibration for
*
*   @retval  None
*
*   @par Dependencies
*   bimc clock speed has to be 200mhz or below in JCPLL bypass mode.
*
*   @par Side Effects
*
*   @sa None
*/

void SITE_Initialization(SDRAM_INTERFACE interface_name)
{
  ddr_mask ddr_broadcast_mask = {0};

  /* Enable broadcast mode on all DQ */
  ddr_broadcast_mask.CA_SITE   = TRUE;
  ddr_broadcast_mask.DQ00_SITE = TRUE;
  ddr_broadcast_mask.DQ01_SITE = TRUE;
  ddr_broadcast_mask.DQ02_SITE = TRUE;
  ddr_broadcast_mask.DQ03_SITE = TRUE;

  HAL_SDRAM_AHB2PHY_Broadcast_Enable(interface_name, ddr_broadcast_mask);

  /* Initialize static settings */
  ddr_set_config(SITE_BCAST_OFFSET, ddr_site_config_base, ddr_site_config_delta);

  /* Toggle site calibration CH0 */
  BOOT_HWIO_OUTM(SITE_ADDR(ALIGN_CONFIG),
                 SITE_BCAST_OFFSET,
                 HWIO_FMSK(SITE_ADDR(ALIGN_CONFIG), QUADRANT),
                 1u << HWIO_SHFT(SITE_ADDR(ALIGN_CONFIG), QUADRANT));

  BOOT_HWIO_OUTM(SITE_ADDR(ALIGN_CONFIG),
                 SITE_BCAST_OFFSET,
                 HWIO_FMSK(SITE_ADDR(ALIGN_CONFIG), SW_OVERRIDE),
                 1u << HWIO_SHFT(SITE_ADDR(ALIGN_CONFIG), SW_OVERRIDE));

   BOOT_HWIO_OUTM(SITE_ADDR(ALIGN_CONFIG),
                  SITE_BCAST_OFFSET,
                  HWIO_FMSK(SITE_ADDR(ALIGN_CONFIG), SW_OVERRIDE),
                  0x0);
}


