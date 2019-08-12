#ifndef __PM_COINCELL_H__
#define __PM_COINCELL_H__

/*! \file  pm_coincell.h
 *  \n
 *  \brief  PMIC-COINCELL MODULE RELATED DECLARATION
 *  \details  This file contains functions and variable declarations to support
 *   the PMIC COINCELL module.
 *
 *  \n &copy; Copyright (c) 2009-2014 QUALCOMM Technologies Incorporated. All Rights Reserved.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/systemdrivers/pmic/pm_coincell.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/29/15   mr      Added support for Coincell Driver (CR-688196)
09/16/14   akm     Created
===========================================================================*/

/*===========================================================================
                INCLUDE FILES FOR MODULE
===========================================================================*/
#include "pm_err_flags.h"
#include "com_dtypes.h"
#include "pm_resources_and_types.h"


/*=============================================================================
                    LOCAL VARIABLE DEFINITIONS
=============================================================================*/
/*! \enum pm_coincell_chg_rset_type
 *  \brief Select Resistor value
*/

typedef enum
{
  PM_COINCELL_RSET_2K1,
  PM_COINCELL_RSET_1K7,
  PM_COINCELL_RSET_1K2,
  PM_COINCELL_RSET_0K8,
  PM_COINCELL_RSET_INVALID,
}pm_coincell_chg_rset_type;

/*! \enum pm_coincell_chg_vset_type
 *  \brief Select Charging volatage value
*/


typedef enum
{
  PM_COINCELL_VSET_2V5,
  PM_COINCELL_VSET_3V2,
  PM_COINCELL_VSET_3V1,
  PM_COINCELL_VSET_3V0,
  PM_COINCELL_VSET_INVALID,
}pm_coincell_chg_vset_type;


/*===========================================================================
                          FUNCTION DEFINITIONS
===========================================================================*/

/**
 * @brief This function enables COINCELL module
 *
 * @param[in]  pmic_device_index:
 *               0 for primary pmic
 *               1 for secondary pmic
 * @param[in] enable:  PM_OFF / PM_ON
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_coincell_enable
(
  uint8 pmic_chip,
  pm_on_off_type enable
);

/**
 * @brief This function gets STATUS of the COINCELL module
 *
 * @param[in]  pmic_device_index:
 *               0 for primary pmic
 *               1 for secondary pmic
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */
pm_err_flag_type pm_coincell_get_status
(
  uint8 pmic_chip,
  boolean *status
);

/**
 * @brief This function gets sets COIN_CHG_RSET
 *
 * @param[in]  pmic_device_index:
 *               0 for primary pmic
 *               1 for secondary pmic
  * @param[in] coin_chg_rset_value :  800 to 2100 ( 800 ohm to 2.1k Ohm)
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */

pm_err_flag_type pm_coincell_set_cc_current_limit_resistor
(
  uint8 pmic_chip,
  pm_coincell_chg_rset_type coin_chg_rset_value
);


/**
 * @brief This function gets sets COIN_CHG_VSET
 *
 * @param[in]  pmic_device_index:
 *               0 for primary pmic
 *               1 for secondary pmic
  * @param[in] coin_chg_rset_value :  800 to 2100 ( 800 ohm to 2.1k Ohm)
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 */

pm_err_flag_type pm_coincell_set_cc_charging_voltage
(
  uint8 pmic_chip,
  pm_coincell_chg_vset_type coin_chg_rset_value
);


#endif


