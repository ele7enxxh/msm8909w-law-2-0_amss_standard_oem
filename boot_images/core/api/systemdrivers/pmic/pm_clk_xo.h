#ifndef PM_CLK_XO__H
#define PM_CLK_XO__H

/*! \file pm_clk_xo.h
*  \n
*  \brief This header file contains functions and variable declarations 
*         to support PMIC XO Clock peripheral.
*  
*  \n &copy; Copyright 2013 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                                Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/systemdrivers/pmic/pm_clk_xo.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/13/13   hw      Rearchitecting clk module driver to peripheral driver
========================================================================== */
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "pm_clk.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
typedef enum 
{
   PM_MODE_VLPM, 
   PM_MODE_LPM, 
   PM_MODE_NPM, 
   PM_MODE_HPM, 
   PM_MODE_INVALID,
}pm_clk_xo_mode_type;

/*===========================================================================

                 API PROTOTYPES

===========================================================================*/
/**
* 
* @brief This function sets the xo mode.
* @param pmic_chip Selects the pmic chip that this clk buffer locates on. Device index starts with zero
* @param clk_type Selects which clk peripheral to use
* @param mode Select the mode type.
* @note
*    Usage Example:  pm_clk_xo_mode(0, PM_CLK_XO, PM_MODE_NPM);
* 
* @return pm_err_flag_type PM_ERR_FLAG__SUCCESS if successful
*/
pm_err_flag_type pm_clk_xo_mode(uint8 pmic_chip, pm_clk_type clk_type, pm_clk_xo_mode_type mode);

/**
* 
* @brief This function configures xo trim.
* @param pmic_chip Selects the pmic chip that this clk buffer locates on. Device index starts with zero
* @param clk_type Selects which clk peripheral to use
* @param trim_value enter trim value.
* @note
*    Usage Example:  pm_clk_xo_trim(0, PM_CLK_XO, 127);
* 
* @return pm_err_flag_type PM_ERR_FLAG__SUCCESS if successful
*/
pm_err_flag_type pm_clk_xo_trim(uint8 pmic_chip, pm_clk_type clk_type, uint32 trim_value);


#endif /* PM_CLK_XO__H */

