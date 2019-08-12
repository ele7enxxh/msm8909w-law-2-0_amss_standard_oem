#ifndef PM_XO_CORE_H
#define PM_XO_CORE_H

/**
 * Copyright (c) 2013-2014 Qualcomm Technologies Incorporated.
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
 * @file pm_xo_core.h PMIC-MEGA XO CORE  related declaration.
 *
 * @brief This header file contains functions and variable declarations
 *        to support Qualcomm PMIC MEGA XO module.
 */

/* ==========================================================================

                  P M    H E A D E R    F I L E

========================================================================== */

/* ==========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/pmic/pm_xo_core.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/14   mr      Doxygen complaint PMIC Header (CR-602405)
12/06/13   mr      (Tech Pubs) Edited/added Doxygen comments and markup (CR-522045)
03/14/12   hs      Initial version.
========================================================================== */

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "pm_err_flags.h"


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
/**
 * @enum pm_xo_core_power_mode_type
 * @brief Crystal oscillator core power modes.
 */
typedef enum
{
    /**
     * Very low power mode.
     */
    PM_XO_CORE_PWR_MODE__VLPM,
    /**
     * Low power mode
     */
    PM_XO_CORE_PWR_MODE__LPM,
    /**
     * Normal power mode
     */
    PM_XO_CORE_PWR_MODE__NPM,
    /**
     * High power mode
     */
    PM_XO_CORE_PWR_MODE__HPM
} pm_xo_core_power_mode_type;


/*===========================================================================

            MEGA XO CORE DRIVER FUNCTION PROTOTYPES

===========================================================================*/
/**
 * @name pm_dev_xo_core_set_power_mode
 *
 * @brief Set power mode for XO Core.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] externalResourceIndex External resource index.
 * @param [in] mode XO core power mode.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @note Do not use pm_xo_core_set_power_mode(), which is deprecated. Use pm_dev_xo_core_set_power_mode() instead.
 *
 */
pm_err_flag_type pm_dev_xo_core_set_power_mode(uint8 pmic_chip,
                                           int externalResourceIndex,
                                           pm_xo_core_power_mode_type mode);
pm_err_flag_type pm_xo_core_set_power_mode(int externalResourceIndex,
                                           pm_xo_core_power_mode_type mode);

/**
 * @name pm_dev_mega_xo_set_xo_trim
 *
 * @brief Trims the 19.2M Hz XO load capacitances.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] externalResourceIndex The On or Off command for the timer
 * @param [in] trim_value Raw trim value to be written, in the range of 0-63(uint8).
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @note Do not use pm_mega_xo_set_xo_trim(), which is deprecated. Use pm_dev_mega_xo_set_xo_trim() instead.
 *
 */
pm_err_flag_type pm_dev_mega_xo_set_xo_trim(uint8 pmic_chip,
                                            int externalResourceIndex,
                                            uint8 trim_value);
pm_err_flag_type pm_mega_xo_set_xo_trim(int externalResourceIndex,
                                        uint8 trim_value);

/**
 * @name pm_dev_mega_xo_get_xo_trim
 *
 * @brief Gets the raw trim value of the load capacitances for the 19.2 MHz XO.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] externalResourceIndex The On or Off command for the timer
 *
 * @param [out] trim_value Pointer to the raw trim value.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @note Do not use pm_xo_core_get_xo_trim(), which is deprecated. Use pm_dev_mega_xo_get_xo_trim() instead.
 *
 */
pm_err_flag_type pm_dev_mega_xo_get_xo_trim(uint8 pmic_chip,
                                            int externalResourceIndex,
                                            uint8* trim_value);
pm_err_flag_type pm_xo_core_get_xo_trim(int externalResourceIndex,
                                        uint8* trim_value);

/**
 * @name pm_dev_mega_xo_set_xo_core_force_on
 *
 * @brief Forces the XO core on if pm_xo_enable(TRUE) has been called.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] externalResourceIndex The On or Off command for the timer
 * @param [in] xo_core_enable TRUE - Force the XO core on.
                              FALSE - Leave the XO core off.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 * @note Do not use pm_xo_core_set_xo_core_force_on(), which is deprecated. Use pm_dev_mega_xo_set_xo_core_force_on() instead.
 *
 */
pm_err_flag_type pm_dev_mega_xo_set_xo_core_force_on(uint8 pmic_chip,
                                                    int externalResourceIndex,
                                                    boolean xo_core_enable);
pm_err_flag_type pm_xo_core_set_xo_core_force_on(int externalResourceIndex,
                                                 boolean xo_core_enable);

#endif /* PM_XO_CORE_H */
