#ifndef __PM_PBS_CLIENT_H__
#define __PM_PBS_CLIENT_H__

/*! \file pm_pbs_client.h
 *  \n
 *  \brief This header file contains API and type definitions for PBS Client driver.
 *  \n
 *  \n &copy; Copyright 2013 QUALCOMM Technologies Incorporated, All Rights Reserved
 */
/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/systemdrivers/pmic/pm_pbs_client.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/16/13   kt      Created.
=============================================================================*/
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "pm_err_flags.h"
#include "com_dtypes.h"
#include "pm_resources_and_types.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
/** 
  @enum pm_pbs_client_type
  @brief PBS Client types
 */
typedef enum 
{
  PM_PBS_CLIENT_0  = 0,
  PM_PBS_CLIENT_1  = 1,
  PM_PBS_CLIENT_2  = 2,
  PM_PBS_CLIENT_3  = 3,
  PM_PBS_CLIENT_4  = 4,
  PM_PBS_CLIENT_5  = 5,
  PM_PBS_CLIENT_6  = 6,
  PM_PBS_CLIENT_7  = 7,
  PM_PBS_CLIENT_8  = 8,
  PM_PBS_CLIENT_9  = 9,
  PM_PBS_CLIENT_10 = 10,
  PM_PBS_CLIENT_11 = 11,
  PM_PBS_CLIENT_12 = 12,
  PM_PBS_CLIENT_13 = 13,
  PM_PBS_CLIENT_14 = 14,
  PM_PBS_CLIENT_15 = 15,
  PM_PBS_CLIENT_INVALID
}pm_pbs_client_type;

/** 
  @enum pm_pbs_client_irq_type
  @brief different types of irq bit fields of PBS Client 
 */
typedef enum 
{
  PM_PBS_CLIENT_IRQ_SEQ_ERROR = 0,
  PM_PBS_CLIENT_IRQ_SEQ_ENDED = 1,
  PM_PBS_CLIENT_IRQ_INVALID
}pm_pbs_client_irq_type;

/*! @enum pm_pbs_client_trigger_type
 *  @brief Type definition for different PBS triggers
 */
typedef enum 
{
  PM_PBS_CLIENT_TRIGGER_RISING_EDGE,
  PM_PBS_CLIENT_TRIGGER_FALLING_EDGE,
  PM_PBS_CLIENT_TRIGGER_DUAL_EDGE,
  PM_PBS_CLIENT_TRIGGER_INVALID
} pm_pbs_client_trigger_type;

/*===========================================================================

                        API PROTOTYPE

===========================================================================*/
/**
 * @name pm_pbs_client_enable
 * 
 * @brief This function enables or disables all the triggers
 *        corresponding to the specific PBS Client.
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1  
 * @param[in] client: PBS Client type. Refer enum 
 *                    pm_pbs_client_type above.
 * @param[in] enable:  
 *                TRUE: Enable PBS Client triggers 
 *                FALSE:Disable PBS Client triggers
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *          Else SPMI errors.
 */
pm_err_flag_type 
pm_pbs_client_enable(uint8 pmic_device_index, pm_pbs_client_type client, boolean enable);

/**
 * @name pm_pbs_client_enable_status
 * 
 * @brief This function returns the PBS Client enable/disable 
 *        status.
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1  
 * @param[in] client: PBS Client type. Refer enum 
 *                    pm_pbs_client_type above.
 * @param[out] status:  
 *                TRUE: PBS Client triggers are enabled
 *                FALSE:PBS Client triggers are disabled
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *          Else SPMI errors.
 */
pm_err_flag_type 
pm_pbs_client_enable_status(uint8 pmic_device_index, pm_pbs_client_type client, boolean *status);

/**
 * @name pm_pbs_client_trigger_cfg 
 *  
 * @brief This function configures the PBS trigger type for the 
 *        specific PBS Client.
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] client: PBS Client type. Refer enum 
 *                    pm_pbs_client_type above.
 * @param[in] trigger:  
 *                PBS trigger type. Refer enum
 *                pm_pbs_client_trigger_type above.
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *          Else SPMI errors.
 */
pm_err_flag_type 
pm_pbs_client_trigger_cfg(uint8 pmic_device_index, pm_pbs_client_type client, pm_pbs_client_trigger_type trigger);

/**
 * @name pm_pbs_client_irq_enable 
 *  
 * @brief This function enables or disables the PBS Client IRQ.
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] client: PBS Client type. Refer enum 
 *                    pm_pbs_client_type above.
 * @param[in] irq:  
 *                PBS Client irq type. Refer enum
 *                pm_pbs_client_irq_type above.
 *  
 * @param[in] enable:  
 *                TRUE: Enable corresponding PBS Client interrupt 
 *                FALSE:Disable corresponding PBS Client interrupt
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *          Else SPMI errors.
 */
pm_err_flag_type 
pm_pbs_client_irq_enable(uint8 pmic_device_index, pm_pbs_client_type client, pm_pbs_client_irq_type irq, boolean enable);

/**
 * @name pm_pbs_client_irq_clear 
 *  
 * @brief This function clears the PBS Client IRQ.
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] client: PBS Client type. Refer enum 
 *                    pm_pbs_client_type above.
 * @param[in] irq:  
 *                PBS Client irq type. Refer enum
 *                pm_pbs_client_irq_type above.
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *          Else SPMI errors.
 */
pm_err_flag_type 
pm_pbs_client_irq_clear(uint8 pmic_device_index, pm_pbs_client_type client, pm_pbs_client_irq_type irq);

/**
 * @name pm_pbs_client_irq_set_trigger 
 *  
 * @brief This function configures the PBS Client IRQ trigger 
 *        type.
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] client: PBS Client type. Refer enum 
 *                    pm_pbs_client_type above.
 * @param[in] irq:  
 *                PBS Client irq type. Refer enum
 *                pm_pbs_client_irq_type above.
 * @param[in] trigger:  
 *                IRQ trigger type. Refer enum
 *                pm_irq_trigger_type from pm_irq.h for more
 *                details.
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *          Else SPMI errors.
 */
pm_err_flag_type 
pm_pbs_client_irq_set_trigger(uint8 pmic_device_index, pm_pbs_client_type client, pm_pbs_client_irq_type irq, pm_irq_trigger_type trigger);

/**
 * @name pm_pbs_client_irq_status 
 *  
 * @brief This function returns the PBS Client IRQ status.
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] client: PBS Client type. Refer enum 
 *                    pm_pbs_client_type above.
 * @param[in] irq:  
 *                PBS Client irq type. Refer enum
 *                pm_pbs_client_irq_type above.
 * @param[in] type:  
 *                Type of IRQ status to read. Refer enum pm_irq_status_type 
 *                from pm_irq.h for more details.
 * @param[out] status:  
 *                IRQ status.
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *          Else SPMI errors.
 */
pm_err_flag_type 
pm_pbs_client_irq_status(uint8 pmic_device_index, pm_pbs_client_type client, pm_pbs_client_irq_type irq, pm_irq_status_type type, boolean *status);

#endif /* __PM_PBS_CLIENT_H__ */
