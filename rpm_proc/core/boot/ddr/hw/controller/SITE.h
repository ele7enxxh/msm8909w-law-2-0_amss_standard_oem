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

Copyright 2012-2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/boot/ddr/hw/controller/SITE.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
03/19/13   sl      New SITE training algorithm.
12/06/12   tw      Added support for mx scaling
11/20/12   tw      Added quadrant override as part of clock switch when 
                   switching from JCPLL enable to bypass mode
10/26/12   tw      Added api for resetting site calibration to no push\pull
09/28/12   tw      Added support for training per jcpll
06/28/12   tw      Initial version

===========================================================================

*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */
#include "ddr_common.h"
#include "ddr_params.h"

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
boolean HAL_SDRAM_SITE_Calibration(SDRAM_INTERFACE interface, uint8 *merged_quad, struct ddr_site_data *site_params);

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
boolean HAL_SDRAM_SITE_Update_Calibration(SDRAM_INTERFACE interface_name, uint32 new_clk, uint32 jcpll_sel_status);

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
void HAL_SDRAM_SITE_Reset_Calibration(SDRAM_INTERFACE interface_name);

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
void HAL_SDRAM_SITE_Reset_Quadrant_Override(SDRAM_INTERFACE interface_name);

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
void HAL_SDRAM_SITE_Enable_Rapid_Calibration(void);

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
void HAL_SDRAM_SITE_Disable_Rapid_Calibration(void);

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
void HAL_SDRAM_SITE_Force_DLL_Delay(uint32 jcpll, boolean enable);

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
void HAL_SDRAM_SITE_Read_DLL_Delay(SDRAM_INTERFACE interface, uint32 jcpll, struct ddr_site_data *site_params);

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
void HAL_SDRAM_SITE_Toggle_DLL_Bypass(uint32 jcpll, boolean enable);


#endif /* SITE_H_ */
