#ifndef PM_PBS_CLIENT_H
#define PM_PBS_CLIENT_H

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
 * @file pm_pbs_client.h PMIC PBS Client related declaration.
 *
 * @brief This header file contains API and type definitions for PBS Client driver.
 */

/* ==========================================================================

                  P M    H E A D E R    F I L E

========================================================================== */


/* ==========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/pmic/pm_pbs_client.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/14   mr      Doxygen complaint PMIC Header (CR-602405)
06/16/13   kt      Created.
===========================================================================*/

/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "pm_err_flags.h"
#include "com_dtypes.h"
#include "pm_irq.h"


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
/**
 * @enum pm_pbs_client_type
 * @brief PBS Client types
 */
typedef enum
{
  PM_PBS_CLIENT_0  = 0,     /**< PM_PBS_CLIENT_0   */
  PM_PBS_CLIENT_1  = 1,     /**< PM_PBS_CLIENT_1   */
  PM_PBS_CLIENT_2  = 2,     /**< PM_PBS_CLIENT_2   */
  PM_PBS_CLIENT_3  = 3,     /**< PM_PBS_CLIENT_3   */
  PM_PBS_CLIENT_4  = 4,     /**< PM_PBS_CLIENT_4   */
  PM_PBS_CLIENT_5  = 5,     /**< PM_PBS_CLIENT_5   */
  PM_PBS_CLIENT_6  = 6,     /**< PM_PBS_CLIENT_6   */
  PM_PBS_CLIENT_7  = 7,     /**< PM_PBS_CLIENT_7   */
  PM_PBS_CLIENT_8  = 8,     /**< PM_PBS_CLIENT_8   */
  PM_PBS_CLIENT_9  = 9,     /**< PM_PBS_CLIENT_9   */
  PM_PBS_CLIENT_10 = 10,    /**< PM_PBS_CLIENT_10  */
  PM_PBS_CLIENT_11 = 11,    /**< PM_PBS_CLIENT_11  */
  PM_PBS_CLIENT_12 = 12,    /**< PM_PBS_CLIENT_12  */
  PM_PBS_CLIENT_13 = 13,    /**< PM_PBS_CLIENT_13  */
  PM_PBS_CLIENT_14 = 14,    /**< PM_PBS_CLIENT_14  */
  PM_PBS_CLIENT_15 = 15,    /**< PM_PBS_CLIENT_15  */
  PM_PBS_CLIENT_INVALID     /**< PM_PBS_CLIENT_INVALID */
}pm_pbs_client_type;

/**
 * @enum pm_pbs_client_irq_type
 * @brief different types of irq bit fields of PBS Client
 */
typedef enum
{
  PM_PBS_CLIENT_IRQ_SEQ_ERROR = 0,    /**< PM_PBS_CLIENT_IRQ_SEQ_ERROR */
  PM_PBS_CLIENT_IRQ_SEQ_ENDED = 1,    /**< PM_PBS_CLIENT_IRQ_SEQ_ENDED */
  PM_PBS_CLIENT_IRQ_INVALID           /**< PM_PBS_CLIENT_IRQ_INVALID */
}pm_pbs_client_irq_type;

/**
 * @enum pm_pbs_client_trigger_type
 * @brief Type definition for different PBS triggers
 */
typedef enum
{
  PM_PBS_CLIENT_TRIGGER_RISING_EDGE,    /**< PM_PBS_CLIENT_TRIGGER_RISING_EDGE */
  PM_PBS_CLIENT_TRIGGER_FALLING_EDGE,   /**< PM_PBS_CLIENT_TRIGGER_FALLING_EDGE */
  PM_PBS_CLIENT_TRIGGER_DUAL_EDGE,      /**< PM_PBS_CLIENT_TRIGGER_DUAL_EDGE */
  PM_PBS_CLIENT_TRIGGER_INVALID         /**< PM_PBS_CLIENT_TRIGGER_INVALID */
} pm_pbs_client_trigger_type;


/*===========================================================================

                    PBS DRIVER FUNCTUION PROTOTYPE

===========================================================================*/
/**
 * @name pm_pbs_client_enable
 *
 * @brief This function enables or disables all the triggers
 *        corresponding to the specific PBS Client.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] client PBS Client type.
 * @param [in] enable TRUE: Enable PBS Client triggers
 *                   FALSE:Disable PBS Client triggers
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *          PM_ERR_FLAG__SBI_OPT_ERR = The SBI driver failed to communicate with the PMIC.
 */
pm_err_flag_type
pm_pbs_client_enable(uint8 pmic_chip, pm_pbs_client_type client, boolean enable);

/**
 * @name pm_pbs_client_enable_status
 *
 * @brief This function returns the PBS Client enable/disable
 *        status.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] client PBS Client type.
 * @param [out] status TRUE: PBS Client triggers are enabled
 *                    FALSE:PBS Client triggers are disabled
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *          PM_ERR_FLAG__SBI_OPT_ERR = The SBI driver failed to communicate with the PMIC.
 */
pm_err_flag_type
pm_pbs_client_enable_status(uint8 pmic_chip, pm_pbs_client_type client, boolean *status);

/**
 * @name pm_pbs_client_trigger_cfg
 *
 * @brief This function configures the PBS trigger type for the
 *        specific PBS Client.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] client PBS Client type.
 * @param [in] trigger PBS trigger type.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *          PM_ERR_FLAG__SBI_OPT_ERR = The SBI driver failed to communicate with the PMIC.
 */
pm_err_flag_type
pm_pbs_client_trigger_cfg(uint8 pmic_chip, pm_pbs_client_type client, pm_pbs_client_trigger_type trigger);

/**
 * @name pm_pbs_client_irq_enable
 *
 * @brief This function enables or disables the PBS Client IRQ.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] client PBS Client type.
 * @param [in] irq PBS Client irq type.
 * @param [in] enable TRUE: Enable corresponding PBS Client interrupt
 *                   FALSE:Disable corresponding PBS Client interrupt
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *          PM_ERR_FLAG__SBI_OPT_ERR = The SBI driver failed to communicate with the PMIC.
 */
pm_err_flag_type
pm_pbs_client_irq_enable(uint8 pmic_chip, pm_pbs_client_type client, pm_pbs_client_irq_type irq, boolean enable);

/**
 * @name pm_pbs_client_irq_clear
 *
 * @brief This function clears the PBS Client IRQ.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] client PBS Client type.
 * @param [in] irq PBS Client irq type.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *          PM_ERR_FLAG__SBI_OPT_ERR = The SBI driver failed to communicate with the PMIC.
 */
pm_err_flag_type
pm_pbs_client_irq_clear(uint8 pmic_chip, pm_pbs_client_type client, pm_pbs_client_irq_type irq);

/**
 * @name pm_pbs_client_irq_set_trigger
 *
 * @brief This function configures the PBS Client IRQ trigger
 *        type.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] client PBS Client type.
 * @param [in] irq PBS Client irq type.
 * @param [in] trigger IRQ trigger type.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *          PM_ERR_FLAG__SBI_OPT_ERR = The SBI driver failed to communicate with the PMIC.
 */
pm_err_flag_type
pm_pbs_client_irq_set_trigger(uint8 pmic_chip, pm_pbs_client_type client, pm_pbs_client_irq_type irq, pm_irq_trigger_type trigger);

/**
 * @name pm_pbs_client_irq_status
 *
 * @brief This function returns the PBS Client IRQ status.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] client PBS Client type.
 * @param [in] irq PBS Client irq type.
 * @param [in] type Type of IRQ status to read.
 *
 * @param [out] status IRQ status.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *          Else SPMI errors.
 */
pm_err_flag_type
pm_pbs_client_irq_status(uint8 pmic_chip, pm_pbs_client_type client, pm_pbs_client_irq_type irq, pm_irq_status_type type, boolean *status);

/**
 * Triggers the supplied PBS sequence
 *        by writing to the TRIG_CTL register
 *
 * @param[in] pmic_chip. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] client: PBS Client type. Refer enum
 *                    pm_pbs_client_type above.
 *
 * @return
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type
pm_pbs_client_sw_trigger(uint8 pmic_chip, pm_pbs_client_type client);

/** @} */ /* end_addtogroup pm_pbs_client */

#endif /* PM_PBS_CLIENT_H */
