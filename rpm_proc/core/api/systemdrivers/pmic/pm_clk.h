#ifndef PM_CLK__H
#define PM_CLK__H

/** @file pm_clk.h
*
*  This file contains functions and variable declarations
*         to support PMIC Clock peripheral.
*/
/*
 *  Copyright (c) 2012-2014 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/api/systemdrivers/pmic/pm_clk.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/30/14   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
04/30/13   kt      Created. 
========================================================================== */
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"

/*===========================================================================

                        TYPE DEFINE AND ENUMS

===========================================================================*/
/** @addtogroup pm_clk
@{ */

/** PMIC clock type. */
typedef enum
{
	PM_CLK_SLEEP,   /**< Sleep clock. */
	PM_CLK_XO,      /**< XO clock. */
	PM_CLK_RF_1,    /**< RF 1 clock. */
	PM_CLK_RF_2,    /**< RF 2 clock. */
	PM_CLK_RF_3,    /**< RF  clock. */
    PM_CLK_BB_1,    /**< BB 1 clock. */
    PM_CLK_BB_2,    /**< BB 2 clock. */
    PM_CLK_DIFF_1,  /**< Diff 1 clock. */
    PM_CLK_DIV_1,   /**< Div 1 clock. */
    PM_CLK_DIV_2,   /**< Div 2 clock. */
    PM_CLK_DIV_3,   /**< Div 3 clock. */
    PM_CLK_DIST,    /**< Clock distribution module.  */ 
    PM_CLK_LN_BB,   /**< Low-noise baseband clock. */
    PM_ALL_CLKS,    /**< All clocks. */
    PM_CLK_INVALID  /**< Invalid clock. */
}pm_clk_type;

/** Clock buffer output drive strength. */
typedef enum
{
    PM_CLK_DRV_STRENGTH_1X, 	/**< Output drive strength is normal. */
    PM_CLK_DRV_STRENGTH_2X,		/**< Output drive strength is double. */
    PM_CLK_DRV_STRENGTH_3X,		/**< Output drive strength is triple. */
    PM_CLK_DRV_STRENGTH_4X,		/**< Output drive strength is quadruple. */
    PM_CLK_DRV_STRENGTH_INVALID /**< Output drive strength is invalid. */
}pm_clk_drv_strength_type;

/*===========================================================================

                        API

===========================================================================*/

/**
*  Sets the output clock buffer drive strength.
*
* @param[in] pmic_chip Selects the PMIC chip in which this clock buffer is
*                      located. The device index starts with zero.
* @param[in] periph Selects which clock peripheral to use.
*                   See #pm_clk_type.
* @param[in] strength Sets the desired strength.
*                     See #pm_clk_drv_strength_type.
* 
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
*/
pm_err_flag_type pm_clk_drv_strength(uint8 pmic_chip, pm_clk_type periph, pm_clk_drv_strength_type strength);

/**
* Enables the clock.
*
* @param[in] pmic_chip Selects the PMIC chip in which the buffers being
*                      controlled are located. The device index starts with
*                      zero.
* @param[in] periph Selects the clock peripheral to turn on.
*               See #pm_clk_type.
* @param[in] on_off Turns a clock on or off (enable or disable).
*                   See #pm_on_off_type.
* 
* @return
* SUCCESS or Error -- See #pm_err_flag_type.
*/
pm_err_flag_type pm_clk_sw_enable(uint8 pmic_chip, pm_clk_type periph, pm_on_off_type on_off);

/**
*  Enables the clock to be pin controlled.
*
* @param[in] pmic_chip Selects the PMIC chip in which the buffers being 
*                      controlled are located. The device index starts with
*                      zero.
* @param[in] periph Selects the clock peripheral that is to be pin
*                   controlled. See #pm_clk_type. 
* @param[in] on_off Switches the pin control capability on or off (enable or
*                   disable). See #pm_on_off_type.
* 
* @return
* SUCCESS or Error -- See #pm_err_flag_type.
*/
pm_err_flag_type pm_clk_pin_ctrled(uint8 pmic_chip, pm_clk_type periph, pm_on_off_type on_off);

/** @} */ /* end_addtogroup pm_clk */
#endif /* PM_CLK__H */
