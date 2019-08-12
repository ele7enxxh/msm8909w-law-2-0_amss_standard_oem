#ifndef PM_BUA_H
#define PM_BUA_H

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
 * @file pm_bua.h BATTERY UICC ALARM related declaration.
 *
 * @brief This header file contains API and type definitions for BUA driver.
 */

/* ==========================================================================

                  P M    H E A D E R    F I L E

========================================================================== */


/* ==========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/pmic/pm_bua.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/14   mr      Doxygen complaint PMIC Header (CR-602405)
06/16/13   kt      Created.
===========================================================================*/

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
#include "pm_err_flags.h"
#include "com_dtypes.h"
#include "pm_irq.h"

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
/**
 * @enum pm_bua_alarm_type
 * @brief different types of BUA alarms supported across targets
 */
typedef enum
{
  PM_BUA_BATT_ALARM,
  PM_BUA_UICC1_ALARM,
  PM_BUA_UICC2_ALARM,
  PM_BUA_UICC3_ALARM,
  PM_BUA_UICC4_ALARM,
  PM_BUA_INVALID_ALARM
}pm_bua_alarm_type;

/*===========================================================================

                    BUA DRIVER FUNCTION PROTOTYPES

===========================================================================*/
/**
 * @name pm_bua_enable
 *
 * @brief This function enables or disables the BUA.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] enable TRUE: Enable BUA
 *                     FALSE:Disable BUA
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__SUCCESS = SUCCESS.
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *         Else SPMI errors.
 */
pm_err_flag_type pm_bua_enable(uint8 pmic_chip, boolean enable);

/**
 * @name pm_bua_enable_status
 *
 * @brief This function returns BUA enable/disable status
 *    (BUA_OK status).
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 *
 * @param [out] status TRUE: BUA is enabled
 *                      FALSE: BUA is disabled
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__SUCCESS = SUCCESS.
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *         Else SPMI errors.
 */
pm_err_flag_type pm_bua_enable_status(uint8 pmic_chip, boolean *status);

/**
 * @name pm_bua_alarm_detect_status
 *
 * @brief This function returns the BUA alarm detected status.
 *        BATT_GONE status for BATT_ALARM type and
 *        UICCx_ALARM_DETECTED status for UICCx_ALARM type.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] alarm BUA alarm type. Refer to enum pm_bua_alarm_type.
 *
 * @param [out] status TRUE: BUA alarm detected
 *                      FALSE:BUA alarm not detected
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__SUCCESS = SUCCESS.
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *         Else SPMI errors.
 */
pm_err_flag_type pm_bua_alarm_detect_status(uint8 pmic_chip,
                                            pm_bua_alarm_type alarm,
                                            boolean *status);

/**
 * @name pm_bua_irq_enable
 *
 * @brief This function enables or disables the BUA alarm IRQ.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] alarm BUA alarm type. Refer to enum pm_bua_alarm_type.
 * @param [in] enable TRUE: Enable BUA alarm interrupt
 *                     FALSE:Disable BUA alarm interrupt
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__SUCCESS = SUCCESS.
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *         Else SPMI errors.
 */
pm_err_flag_type pm_bua_irq_enable(uint8 pmic_chip,
                                   pm_bua_alarm_type alarm,
                                   boolean enable);

/**
 * @name pm_bua_irq_clear
 *
 * @brief This function clears the BUA alarm IRQ.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] alarm BUA alarm type. Refer to enum pm_bua_alarm_type.
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__SUCCESS = SUCCESS.
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *         Else SPMI errors.
 */
pm_err_flag_type pm_bua_irq_clear(uint8 pmic_chip,
                                  pm_bua_alarm_type alarm);

/**
 * @name pm_bua_irq_set_trigger
 *
 * @brief This function configures the BUA alarm IRQ trigger type.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] alarm BUA alarm type. Refer enum pm_bua_alarm_type.
 * @param [in] trigger IRQ trigger type. Refer enum
 *                      pm_irq_trigger_type from pm_irq.h for more details.
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__SUCCESS = SUCCESS.
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *         Else SPMI errors.
 */
pm_err_flag_type pm_bua_irq_set_trigger(uint8 pmic_chip,
                                        pm_bua_alarm_type alarm,
                                        pm_irq_trigger_type trigger);

/**
 * @name pm_bua_irq_get_trigger
 *
 * @brief This function returns the BUA alarm IRQ trigger type.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] alarm BUA alarm type. Refer enum pm_bua_alarm_type.
 *
 * @param [out] trigger IRQ trigger type set for the BUA alarm IRQ.
 *                       Refer enum pm_irq_trigger_type from pm_irq.h
 *                       for more details.
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__SUCCESS = SUCCESS.
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *         Else SPMI errors.
 */
pm_err_flag_type pm_bua_irq_get_trigger(uint8 pmic_chip,
                                        pm_bua_alarm_type alarm,
                                        pm_irq_trigger_type *trigger);

/**
 * @name pm_bua_irq_status
 *
 * @brief This function returns the BUA alarm IRQ status.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] alarm BUA alarm type. Refer to enum pm_bua_alarm_type.
 * @param [in] type Type of IRQ status to read. Refer enum pm_irq_status_type
 *                   from pm_irq.h for more details.
 *
 * @param [out] status IRQ status.
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__SUCCESS = SUCCESS.
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *         Else SPMI errors.
 */
pm_err_flag_type pm_bua_irq_status(uint8 pmic_chip,
                                   pm_bua_alarm_type alarm,
                                   pm_irq_status_type type,
                                   boolean *status);

#endif /* PM_BUA_H */
