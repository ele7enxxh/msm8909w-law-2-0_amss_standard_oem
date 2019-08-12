#ifndef _PM_VERSION_H
#define _PM_VERSION_H
/*! \file 
 *  \n
 *  \brief  pm_version.h ---- PMIC Version 
 *  \n
 *  \n This header file contains enums and API definitions for PMIC version 
 *      detection
 *  \n
 *  \n &copy; Copyright 2010-2011 Qualcomm Technologies Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pm_version.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/18/15   ps      Added PMD9607 support for MDM9x07 (CR-892107) 
03/10/12   wra     Removed old PMIC versions and added Badger PMIC versions. 
                    Reduced hardware versions in enumeration
10/18/11   jtn/mpt Add PM8821 and PM8038
04/04/11   hw      Added pm_get_hardware_version and pm_set_hardware_version API
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
07/05/10   wra     Added pm_model_type entry for the PM8921 and PM8018
07/05/10   wra     Added file header and pm_model_type entry for the PM8901 
                   and ISL9519
=============================================================================*/
#include "com_dtypes.h"
#include "pm_lib_err.h"

/** 
  @enum pm_model_type
  @brief This enum gives the PMIC model type values (Peripheral
  Subtype values) for different PMIC Models. 
 */
typedef enum
{
   PMIC_IS_UNKNOWN   = 0,
   PMIC_IS_PM8941    = 1,
   PMIC_IS_PM8841    = 2,
   PMIC_IS_PM8019    = 3,
   PMIC_IS_PMD9607   = 3,
   PMIC_IS_PM8026    = 4,
   PMIC_IS_PM8110    = 5,
   PMIC_IS_PMA8084   = 6,
   PMIC_IS_PMI8962   = 7,
   PMIC_IS_PMD9635   = 8,
   PMIC_IS_PM8994    = 9,
   PMIC_IS_PMI8994   = 10,
   PMIC_IS_PM8916    = 11,
   PMIC_IS_INVALID   = 0x7FFFFFFF,
} pm_model_type;

/** 
  @struct pm_device_info_type
  @brief Stores the PMIC's Model type value, the All Layer
  Revision number and the Metal Revision number. Please refer
  to pm_model_type enum above to get more info on which PMIC
  model type value represents which PMIC.For example, for
  PM8019 v2.1 the PMIC model type value is 3 (since PM8019
  model type value is 3 as per pm_model_type enum), All layer
  revision number is '2' and Metal revision number is '1'.
 */
typedef struct
{
  pm_model_type  ePmicModel;             /**< PMIC device model type */
  uint32         nPmicAllLayerRevision;  /**< PMIC device all layer revision number */
  uint32         nPmicMetalRevision;     /**< PMIC device metal revision number */
} pm_device_info_type;

/*===========================================================================

FUNCTION pm_get_pmic_model                         EXTERNAL FUNCTION

DESCRIPTION
    This function returns the PMIC's model type value.
    For example, the API returns '1' for PM8941, '2'
    for PM8841 and so on. Please refer to pm_model_type
    enum above to get more info on which PMIC model type
    value represents which PMIC.

INPUT PARAMETER
  None.

RETURN VALUE: pm_model_type
  Returns valid PMIC model/tier type if successful
  Returns PMIC_IS_INVALID under error conditions (like
   invalid device index param).

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
pm_model_type pm_get_pmic_model(uint8 pmic_device_index);

/*===========================================================================

FUNCTION pm_get_pmic_revision                         EXTERNAL FUNCTION

DESCRIPTION
 This function returns the PMIC's All Layer revision
 number. For example, the API returs '1' for PMIC v1.x,
 '2' for PMIC v2.x  and so on.
INPUT PARAMETER
  [in] - pmic_device_index.Primary PMIC:0 Secondary PMIC:1

RETURN VALUE: uint8
 Returns valid PMIC All Layer revision number if
 successful.
 Returns 0 under error conditions (like invalid
 device index param).

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 pm_get_pmic_revision(uint8 pmic_device_index);

#endif
