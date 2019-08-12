#ifndef PM_BOOST_H
#define PM_BOOST_H

/**
 * Copyright (c) 2013-2015 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 * Export of this technology or software is regulated by the U.S. Government.
 * Diversion contrary to U.S. law prohibited.
 *
 * All ideas, data and information contained in or disclosed by
 * this document are confidential and proprietary information of
 * Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
 * By accepting this material the recipient agrees that this material
 * and the information contained therein are held in confidence and in
 * trust and will not be used, copied, reproduced in whole or in part,
 * nor its contents revealed in any manner to others without the express
 * written permission of Qualcomm Technologies Incorporated.
 *
 */

/**
 * @file pm_boost.h VOLTAGE BOOST related declaration.
 *
 * @brief This header file contains enums and API definitions for PMIC voltage
 *        regulator driver.
 */

/* ==========================================================================

                  P M    H E A D E R    F I L E

========================================================================== */


/* ==========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/pmic/pm_boost.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/14   mr      Doxygen complaint PMIC Header (CR-602405)
12/06/12   hw      Rearchitecturing module driver to peripheral driver
=============================================================================*/

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "com_dtypes.h"


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
/**
 * @brief BOOST peripheral index. This enum type contains all ldo regulators
 *		  that you may need.
 */
enum
{
  PM_BOOST_1,
  PM_BOOST_INVALID
};


/*===========================================================================

                VOLTAGE BOOST FUNCTION PROTOTYPE

===========================================================================*/
/**
 * @name pm_boost_volt_level_status
 *
 * @brief Obtain the voltage of a regulator. Note, different type (LDO, HFS etc)
 *        may have different programmable voltage steps. Only support the
 *        correct programmable steps. Not rounding voltages if the voltage
 *        selected is not part of the programmable steps.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] boost_peripheral_index Index of the peripheral
 *
 * @param [out] volt_level returns the voltage.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 */
pm_err_flag_type pm_boost_volt_level_status(uint8 pmic_chip,
                                            uint8 boost_peripheral_index,
                                            pm_volt_level_type* volt_level);

/**
 * @name pm_boost_sw_enable_status 
 *  
 * @brief Returns the sw enabled/disabled status of the rail.
 * 
 * @param[in] pmic_chip Primary -- 0; Secondary -- 1.
 * @param[in] boost_peripheral_index Boost peripheral index.
 * @param[out] on_off sw enable status.
 * 
 * @return 
 *   SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_boost_sw_enable_status(uint8 pmic_chip, 
                                           uint8 boost_peripheral_index, 
                                           pm_on_off_type* on_off);

/**
 * @name pm_boost_vreg_ok_status 
 *  
 * @brief Returns the VREG_OK/VREG_READY status of the rail.
 * 
 * @param[in] pmic_chip Primary -- 0; Secondary -- 1.
 * @param[in] boost_peripheral_index BOOST peripheral index. 
 *       Starts from 0 (for the first BOOST peripheral).
 * @param[out] on_off On/Off status.
 * 
 * @return 
 *   SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_boost_vreg_ok_status(uint8 pmic_chip, 
                                         uint8 boost_peripheral_index, 
                                         boolean* on_off);

/** @} */ /* end_addtogroup pm_boost */

#endif /* PM_BOOST_H */

