#ifndef PM_BOOT_H
#define PM_BOOT_H

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
 * @file pm_boot.h PMIC BOOT related declaration.
 *   
 * @brief This header file contains enums and API definitions for PMIC boot.
 */

/* ==========================================================================

                  P M    H E A D E R    F I L E

========================================================================== */


/* ==========================================================================

                        EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/systemdrivers/pmic/pm_boot.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/18/14   mr      Doxygen complaint PMIC Header (CR-602405)
02/26/13   kt      Added PMIC SMEM Init API.
12/06/12   hw      Remove comdef.h and use com_dtypes.h
09/25/12   hw      Added support for pmic api by creating pm_driver_init
09/21/11   hs      Added support for TZ(Trust Zone). 
04/21/11   mpt     initial version
========================================================================== */

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"


/*===========================================================================

                 BOOT FUNCTION PROTOTYPES

===========================================================================*/
/**
 * @name pm_oem_init
 *
 * @brief This function is executed after pm_driver_init function. It gives
 *         customer the ability to configure PMIC HW using PMIC APIs.
 *   
 * @param [in] void 
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 */
 pm_err_flag_type pm_oem_init( void ); 

/**
 * @name pm_driver_init
 *
 * @brief This function executes the pmic driver initialization allowing API access.
 *   
 * @param [in] void
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 */ 
pm_err_flag_type pm_driver_init( void );

/**
 * @name pm_device_init
 *
 * @brief This function executes the pmic device initialization.
 *   
 * @param [in] void
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 */
pm_err_flag_type pm_device_init( void );

/**
 * @name pm_tz_device_init
 *
 * @brief This function executes the pmic device initialization in TZ.
 * 
 * @details This function executes the PMIC device initialization in TZ, such as
 *  initializing the SSBI module, etc.
 *   
 * @param [in] void 
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 *
 */
pm_err_flag_type pm_tz_device_init( void );

/**
 * @name pm_enable_qfuse_power_rail
 *
 * @brief This function enables the powe rail for Qfprom block.
 * 
 * @param[in] void 
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 *
 */
pm_err_flag_type pm_enable_qfuse_power_rail(void);

/**
 * @name pm_disable_qfuse_power_rail
 * 
 * @brief This function disables the power rail for Qfprom block.
 *   
 * @param[in] void 
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 *
 */
pm_err_flag_type pm_disable_qfuse_power_rail(void);

/**
 * @name pm_smem_init
 * 
 * @brief This function executes the pmic shared memory initialization.
 * 
 * @details This function stores the PON reasons and PBS info in the
 *          shared memory and should be called by boot after smem is
 *          initialized.
 *   
 * @param [in] void 
 *
 * @return pm_err_flag_type PM_ERR_FLAG__INVALID = ERROR. 
 *         PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type pm_smem_init(void);

/**
 * @name pm_pon_channel_config
 * 
 * @brief This function executes the basic init to ensure PON peripheral well accessed.
 *    NOTE: This API is only provide for limited scenario which must call PON before calling pm_device_init
 *   
 * @param [in] void 
 *
 * @return pm_err_flag_type PM_ERR_FLAG__INVALID = ERROR. 
 *         PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type pm_pon_channel_config(void);

#endif    /* PM_BOOT_H */
