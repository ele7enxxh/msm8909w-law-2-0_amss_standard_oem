#ifndef SITE_H_
#define SITE_H_

/*
===========================================================================

FILE:         SITE.h

DESCRIPTION:
  This is the ddr rpm message driver that will handle the rpm message
  requests. There is an init function to register with the rpmfw with the
  callback function, and the callback function that will handle the message
  requests.

Copyright 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================

                             Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/hw/controller/SITE.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
10/04/12   tw      SITE initialization JCPLL workaround
06/28/12   tw      Initial version

===========================================================================

*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */
#include "ddr_common.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
**                           MACROS
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

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

boolean HAL_SDRAM_SITE_Calibration(SDRAM_INTERFACE interface_name, uint32 frequency_level, uint32 voltage_level);

/* ==========================================================================
**  Function : HAL_SDRAM_SITE_push_calc
** ==========================================================================
*/
/*!
*   @brief
*   This function computes whether we need to push or pull the quadrant
*
*   @param   interface_name     - [IN] interface to calibrate for
*   @param   site_quadrant      - [IN] the offset for SITE (CA through dQ)
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
boolean HAL_SDRAM_SITE_push_calc(uint32 merged_quad, boolean site_quadrant, uint16 *raw_cal_count);

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
*   @retval  uint8 - return the quadrant computed
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*
*   @sa None
*/

uint8 HAL_SDRAM_SITE_quadCompute(uint32  interface_offset, uint32 offset, uint16 *raw_cal_count);


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

boolean HAL_SDRAM_SITE_Update_Calibration(SDRAM_INTERFACE interface_name, uint32 new_clk);

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
*   bimc clock speed has to be 200mhz or below.
*
*   @par Side Effects
*
*   @sa None
*/

void SITE_Initialization(SDRAM_INTERFACE interface_name);

#endif /* SITE_H_ */
