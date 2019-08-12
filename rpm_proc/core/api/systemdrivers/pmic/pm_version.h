#ifndef _PM_VERSION__H
#define _PM_VERSION__H
/** @file pm_version.h 
 * 
 *  PMIC Version
 *  This header file contains enums and API definitions for PMIC version 
 *      detection
 *  
 *  Copyright (c) 2010-2014 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rpm.bf/2.1.1/core/api/systemdrivers/pmic/pm_version.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/03/14   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
10/17/14   rk      PMIC Dev Changes for SAHI - First Drop (CR - 740886)
06/02/14   akt     Added PM8909 model.
10/22/13   rk      Updated PM8916 model.
09/03/12   rk      Added PM8916 model.
01/28/13   kt      Added pm_get_pmic_info API to get the pmic device info.  
01/28/13   kt      Removed pm_set_hardware_version and pm_get_hardware_version APIs. 
12/06/12   hw      Remove comdef.h and use com_dtypes.h
09/19/12   vk      Add PM8019
03/10/12   wra     Removed old PMIC versions and added Badger PMIC versions. 
                   Reduced hardware versions in enumeration
10/18/11   jtn/mpt Add PM8821 and PM8038
04/04/11   hw      Added pm_get_hardware_version and pm_set_hardware_version API
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
07/05/10   wra     Added pm_model_type entry for the PM8921 and PM8018
07/05/10   wra     Added file header and pm_model_type entry for the PM8901 
                   and ISL9519
=============================================================================*/
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"

/** @addtogroup pm_version
@{ */
/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

/** 
Provides the PMIC model type values (peripheral
  subtype values) for various PMIC models. 
 */
typedef enum
{
   PMIC_IS_UNKNOWN   = 0,  /**< Unknown. */
   PMIC_IS_PM8941    = 1,  /**< PM8941. */
   PMIC_IS_PM8841    = 2,  /**< PM8841. */
   PMIC_IS_PM8019    = 3,  /**< PM8019. */
   PMIC_IS_PM8026    = 4,  /**< PM8026. */
   PMIC_IS_PM8110    = 5,  /**< PM8110. */
   PMIC_IS_PMA8084   = 6,  /**< PMA8084. */
   PMIC_IS_PMI8962   = 7,  /**< PMI8962. */
   PMIC_IS_PMD9635   = 8,  /**< PMD9635. */
   PMIC_IS_PM8994    = 9,  /**< PM8994. */
   PMIC_IS_PMI8994   = 10,  /**< PMI8994. */
   PMIC_IS_PM8916    = 11,  /**< PM8916. */
   PMIC_IS_PM8004    = 12,	/**< PM8004. */
   PMIC_IS_PM8909    = 13,	/**< PM8909. */
   PMIC_IS_PM2433    = 14,	/**< PM2433. */
   PMIC_IS_PM8950    = 16,	/**< PM8950. */
   PMIC_IS_PMI8950   = 17,	/**< PMI8950. */
   PMIC_IS_INVALID   = 0x7FFFFFFF,
} pm_model_type;

/** 
  Stores the PMIC's model type value, the All Layer
  revision number, and the Metal revision number.
  
  See #pm_model_type for more information on which PMIC
  model type value represents which PMIC.For example, for
  PM8019 v2.1, the PMIC model type value is 3 (since PM8019
  model type value is 3, as per pm_model_type). The All Layer
  revision number is 2 and the Metal revision number is 1.
 */
typedef struct
{
  pm_model_type  ePmicModel;             /**< PMIC device model type. */
  uint32         nPmicAllLayerRevision;  /**< PMIC device All Layer
                                              revision number. */
  uint32         nPmicMetalRevision;     /**< PMIC device Metal revision number. */
} pm_device_info_type;

/*===========================================================================

                        API PROTOTYPE

===========================================================================*/

/** 
 * Returns the PMIC's model type value.
 * For example, the API returns 1 for PM8941, 2
 * for PM8841, and so on. See #pm_model_type
 * for more information on which PMIC model type
 * value represents which PMIC.
 * 
 * @param[in] pmic_device_index Primary PMIC: 0, Secondary PMIC: 1. 
 *
 * @return  
 * Returns a valid PMIC model/tier type if successful. \n
 * Returns PMIC_IS_INVALID under error conditions (such as
 * invalid device index param).
 *
 */
pm_model_type pm_get_pmic_model(uint8 pmic_device_index);

/** 
 * Returns the PMIC's All Layer revision
 * number, e.g., 1 = PMIC v1.x,
 * 2 = PMIC v2.x, etc.
 * 
 * @param[in] pmic_device_index Primary PMIC:0, Secondary PMIC:1.
 *
 * @return  
 * Returns a valid PMIC All Layer revision number if successful. \n
 * Returns 0 under error conditions (such as invalid device index param).
 * @newpage
 */
uint8 pm_get_pmic_revision(uint8 pmic_device_index);

/** 
 *  Returns information about a PMIC device 
 *  for a specific device index in the pmic_device_info
 *  argument.
 *
 * This function returns a PMIC's model type value,
 * All Layer revision number, and Metal revision number in
 * the pmic_device_info structure. For example, for PM8019 v2.1,
 * the PMIC model type value is 3 (since PM8019 model type value is
 * 3 as per #pm_model_type). The All Layer revision number is 2
 * and the Metal revision number is 1.
 * 
 * @param[in]  pmic_index Primary    PMIC: 0, Secondary PMIC: 1.
 *
 * @param[out] pmic_device_info_ptr  Variable to return to the caller with the PMIC
 *                                   device information. See #pm_device_info_type.
 *
 *
 * @return  
 * PM_ERR_FLAG__FEATURE_NOT_SUPPORTED -- Error in
 * collecting and placing PMIC information. \n
 * PM_ERR_FLAG__SUCCESS -- Success.
 */

pm_err_flag_type pm_get_pmic_info(uint8 pmic_index, pm_device_info_type* pmic_device_info_ptr);
 
/** @} */ /* end_addtogroup pm_version */

#endif // PM_VERSION__H
