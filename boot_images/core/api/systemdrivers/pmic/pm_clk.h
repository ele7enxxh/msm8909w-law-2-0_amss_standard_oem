#ifndef PM_CLK__H
#define PM_CLK__H

/*! \file pm_clk.h
*  \n
*  \brief This header file contains functions and variable declarations
*         to support PMIC Clock peripheral.
*  \n
*  \n &copy; Copyright 2012-2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/systemdrivers/pmic/pm_clk.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/14   akm     Cleanup
03/13/13   hw      Rearchitecting clk module driver to peripheral driver
12/06/12   hw      Remove comdef.h and use com_dtypes.h
03/14/12   hs      Initial version. 
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
/*! \enum pm_clk_type
 *  \brief This enum contains definitions of Clock types
 */
typedef enum
{
    PM_CLK_SLEEP,
    PM_CLK_XO,
    PM_CLK_RF_1,
    PM_CLK_RF_2,
    PM_CLK_RF_3,
    PM_CLK_BB_1,
    PM_CLK_BB_2,
    PM_CLK_DIFF_1,
    PM_CLK_DIV_1,
    PM_CLK_DIV_2,
    PM_CLK_DIV_3,
    PM_CLK_DIST,
    PM_CLK_LN_BB,
    PM_ALL_CLKS,
    PM_CLK_INVALID
}pm_clk_type;

typedef enum
{
    PM_CLK_DRV_STRENGTH_1X,
    PM_CLK_DRV_STRENGTH_2X,
    PM_CLK_DRV_STRENGTH_3X,
    PM_CLK_DRV_STRENGTH_4X,
    PM_CLK_DRV_STRENGTH_INVALID
}pm_clk_drv_strength_type;

/*===========================================================================

                        API

===========================================================================*/

/**
* 
* @brief Sets the output clk buffer drive strength.
* @param pmic_chip Selects the pmic chip that this clk buffer locates on. Device index starts with zero
* @param periph Selects which clk peripheral to use
* @param strenght Select the strength desired.
* 
* @return pm_err_flag_type PM_ERR_FLAG__SUCCESS if successful
*/
pm_err_flag_type pm_clk_drv_strength(uint8 pmic_chip, pm_clk_type periph, pm_clk_drv_strength_type strength);

/**
* 
* @brief Enables the clk.
* @param pmic_chip Selects the pmic chip in which the buffers being controlled are located. Device index starts with zero
* @param periph Selects the clk peripheral that you want to turn on.
* @param on_off Turn a clk off or on ( enable or disable )
* 
* @return pm_err_flag_type PM_ERR_FLAG__SUCCESS if successful
*/
pm_err_flag_type pm_clk_sw_enable(uint8 pmic_chip, pm_clk_type periph, pm_on_off_type on_off);

/**
* 
* @brief Enables clk pull down.
* @param pmic_chip Selects the pmic chip in which the buffers being controlled are located. Device index starts with zero
* @param clk_type Selects the clk peripheral that you want the pull down to take effect on.
* @param on_off Switch the pull down on or off ( enable or disable )
* 
* @return pm_err_flag_type PM_ERR_FLAG__SUCCESS if successful
*/
pm_err_flag_type pm_clk_pull_down( uint8 pmic_chip, pm_clk_type clk_type, pm_on_off_type on_off );

/**
* 
* @brief Enables clk to be pin_ctrled.
* @param pmic_chip Selects the pmic chip in which the buffers being controlled are located. Device index starts with zero
* @param periph Selects the clk peripheral that you want to be pin controlled.
* @param on_off Switch the pin control capability on or off ( enable or disable )
* 
* @return pm_err_flag_type PM_ERR_FLAG__SUCCESS if successful
*/
pm_err_flag_type pm_clk_pin_ctrled(uint8 pmic_chip, pm_clk_type periph, pm_on_off_type on_off);

#endif /* PM_CLK__H */
