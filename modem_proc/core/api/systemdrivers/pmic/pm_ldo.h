#ifndef PM_LDO_H
#define PM_LDO_H

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
 * @file pm_ldo.h PMIC LDO related declaration.
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

$Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/pmic/pm_ldo.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/14   mr      Doxygen complaint PMIC Header (CR-602405)
10/21/13   rk      Support for Vunicore 8926 core compilation
10/03/13   rk      Adding one LDO
08/26/13   rk      remove pin control support and not supported LDOs.
12/06/12   hw      Rearchitecturing module driver to peripheral driver
===========================================================================*/

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "com_dtypes.h"


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/** @addtogroup pm_ldo
@{ */

/**
 * @brief LDO Peripheral Index.
 *        This enum type contains all ldo regulators that you may need.
 */
enum
{
  PM_LDO_1,       /**< LDO 1. */
  PM_LDO_2,       /**< LDO 2. */
  PM_LDO_3,       /**< LDO 3. */
  PM_LDO_4,       /**< LDO 4. */
  PM_LDO_5,       /**< LDO 5. */
  PM_LDO_6,       /**< LDO 6. */
  PM_LDO_7,       /**< LDO 7. */
  PM_LDO_8,       /**< LDO 8. */
  PM_LDO_9,       /**< LDO 9. */
  PM_LDO_10,      /**< LDO 10. */
  PM_LDO_11,      /**< LDO 11. */
  PM_LDO_12,      /**< LDO 12. */
  PM_LDO_13,      /**< LDO 13. */
  PM_LDO_14,      /**< LDO 14. */
  PM_LDO_15,      /**< LDO 15. */
  PM_LDO_16,      /**< LDO 16. */
  PM_LDO_17,      /**< LDO 17. */
  PM_LDO_18,      /**< LDO 18. */
  PM_LDO_19,      /**< LDO 19. */
  PM_LDO_20,      /**< LDO 20. */
  PM_LDO_21,      /**< LDO 21. */
  PM_LDO_22,      /**< LDO 22. */
  PM_LDO_23,      /**< LDO 23. */
  PM_LDO_24,      /**< LDO 24. */
  PM_LDO_25,      /**< LDO 25. */
  PM_LDO_26,      /**< LDO 26. */
  PM_LDO_27,      /**< LDO 27. */
  PM_LDO_28,      /**< LDO 28. */
  PM_LDO_29,      /**< LDO 29. */
  PM_LDO_30,      /**< LDO 30. */
  PM_LDO_31,      /**< LDO 31. */
  PM_LDO_32,      /**< LDO 32. */
  PM_LDO_33,      /**< LDO 33. */
  PM_LDO_INVALID  /**< Invalid LDO. */
};


/*===========================================================================

                    LDO DRIVER FUNCTION PROTOTYPES

===========================================================================*/

/**
 * @name pm_ldo_sw_mode_status
 *
 * @brief This function returns the mode status (LPM, NPM, AUTO, BYPASS)
 *        of the selected power rail. Note, the mode of a regulator
 *        changes dynamically.
 *
 * @param [in] pmic_chip Primary: 0. Secondary: 1
 * @param [in] ldo_peripheral_index Starts from 0 (for first LDO peripheral)
 *
 * @param [out] sw_mode Variable to return to the caller with mode status.
 *                     Refer to pm_resources_and_types.h for the enum info.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 */
pm_err_flag_type pm_ldo_sw_mode_status (uint8 pmic_chip,
                                        uint8 ldo_peripheral_index,
                                        pm_sw_mode_type* sw_mode);

/**
 * @name pm_ldo_pin_ctrled_status
 *
 * @brief This function returns the pin ctrled status or h/w
 *        enable status (On/Off) of the selected power rail.
 *
 * @param [in] pmic_chip Primary: 0. Secondary: 1
 * @param [in] ldo_peripheral_index Starts from 0 (for first LDO peripheral)
 *
 * @param [out] on_off Variable to return to the caller with pin ctrl status.
 *                    Refer to pm_resources_and_types.h for the enum info.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 */
pm_err_flag_type pm_ldo_pin_ctrled_status (uint8 pmic_chip,
                                           uint8 ldo_peripheral_index,
                                           pm_on_off_type* on_off);

/** 
 * @name pm_ldo_pin_ctrl_status 
 *  
 * @brief This function returns the selected pins being 
 *        followed by the selected power rail enable.
 * 
 * @param[in]  pmic_chip. Primary: 0. Secondary: 1
 * @param[in]  perph_index:
 *                Starts from 0 (for first LDO peripheral)
 * @param[out] select_pin Variable returned to the caller with 
 *                        selected pins being followed by the
 *                        power rail.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS else ERROR.
 *
 */
pm_err_flag_type pm_ldo_pin_ctrl_status (uint8 pmic_chip, 
                                         uint8 perph_index, 
                                         uint8 *select_pin);

/**
 * @name pm_ldo_volt_level_status
 *
 * @brief This function returns the voltage level (in micro
 *        volts) of the selected power rail.
 *
 * @param [in] pmic_chip Primary: 0. Secondary: 1
 * @param [in] ldo_peripheral_index Starts from 0 (for first LDO peripheral)
 *
 * @param [out] volt_level Variable to return to the caller with LDO Volt level status.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 */
pm_err_flag_type pm_ldo_volt_level_status (uint8 pmic_chip,
                                           uint8 ldo_peripheral_index,
                                           pm_volt_level_type* volt_level);
pm_err_flag_type pm_ldo_volt_level (uint8 pmic_chip,
                                    uint8 ldo_peripheral_index,
                                    pm_volt_level_type volt_level);

/**
 * @name pm_ldo_sw_enable_status
 *
 * @brief This function returns the s/w enable status (On/Off)
 *        of the selected power rail.
 *
 * @param [in] pmic_chip Primary: 0. Secondary: 1
 * @param [in] ldo_peripheral_index Starts from 0 (for first LDO peripheral)
 *
 * @param [out] on_off Variable to return to the caller with s/w status.
 *                    Refer to pm_resources_and_types.h for the enum info.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 */
pm_err_flag_type pm_ldo_sw_enable_status (uint8 pmic_chip,
                                          uint8 ldo_peripheral_index,
                                          pm_on_off_type* on_off);

/**
 * @name pm_ldo_vreg_ok_status 
 *  
 * @brief Returns the VREG_OK/VREG_READY status of the rail.
 * 
 * @param[in] pmic_chip Primary -- 0; Secondary -- 1.
 * @param[in] ldo_peripheral_index LDO peripheral index. Starts 
 *       from 0 (for the first LDO peripheral).
 * @param[out] on_off On/Off status.
 * 
 * @return 
 *   SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_ldo_vreg_ok_status (uint8 pmic_chip, 
                                        uint8 ldo_peripheral_index, 
                                        boolean* on_off);

/**
 * @name pm_ldo_soft_reset_status
 *
 * @brief This function returns the local soft reset status of
 *        the selected power rail.
 *
 * @param [in] pmic_chip Primary: 0. Secondary: 1
 * @param [in] ldo_peripheral_index Starts from 0 (for first LDO peripheral)
 *
 * @param [out] status Variable to return to the caller with s/w status.
 *                    Returns TRUE if LDO is in soft reset and FALSE otherwise.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 */
pm_err_flag_type pm_ldo_soft_reset_status (uint8 pmic_chip,
                                           uint8 ldo_peripheral_index,
                                           boolean* status);

/**
 * @name pm_ldo_soft_reset_exit
 *
 * @brief This function brings the selected power rail out of
 *        local soft reset.
 *
 * @param [in] pmic_chip Primary: 0. Secondary: 1
 * @param [in] ldo_peripheral_index Starts from 0 (for first LDO peripheral)
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 */
pm_err_flag_type pm_ldo_soft_reset_exit (uint8 pmic_chip,
                                          uint8 ldo_peripheral_index);

/**
 * Calculates the PMIC register value for
 *        a supplied voltage
 *
 * @param[in]  pmic_chip. Primary: 0. Secondary: 1
 * @param[in]  ldo_peripheral_index:
 *                Starts from 0 (for first LDO peripheral)
 * @param[in]  volt_level:
 *                Voltage to convert to PMIC register value
 * @param[out] vset:
 *                PMIC register value for supplied voltage
 *
 * @return
 *  SUCCESS or Error -- See #pm_err_flag_type.
 *
 */
pm_err_flag_type pm_ldo_volt_calculate_vset(uint8 pmic_chip,
                                            uint8 ldo_peripheral_index,
                                            pm_volt_level_type  volt_level,
                                            uint32* vset);

/**
 * Returns the stepper done status for
 *        the selected power rail
 *
 * @param[in]  pmic_chip. Primary: 0. Secondary: 1
 * @param[in]  ldo_peripheral_index:
 *                Starts from 0 (for first LDO peripheral)
 * @param[out]  stepper_done:
 *                Value of stepper done register
 *
 * @return
 *  SUCCESS or Error -- See #pm_err_flag_type.
 *
 */
pm_err_flag_type pm_ldo_volt_level_stepper_done_status(uint8 pmic_chip,
                                                        uint8 ldo_peripheral_index,
                                                        boolean *stepper_done);

/** @} */ /* end_addtogroup pm_ldo */

#endif    /* PM_LDO_H */

