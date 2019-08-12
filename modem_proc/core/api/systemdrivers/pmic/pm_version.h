#ifndef PM_VERSION_H
#define PM_VERSION_H

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
 * @file pm_version.h PMIC MODEL/VERSION related declaration.
 *
 * @brief This header file contains enums and API definitions for PMIC model/
 *          version detection.
 */

/* ==========================================================================

                  P M    H E A D E R    F I L E

========================================================================== */

/* ==========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/pmic/pm_version.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/23/15   ps      Added PMD9607 model (CR-878990)
02/27/14   mr      Doxygen complaint PMIC Header (CR-602405)
12/06/13   mr      (Tech Pubs) Edited/added Doxygen comments and markup (CR-522045)
10/22/13   rk      Define PM8916 model enum type.
08/26/13   rk      Define PM8916 enum type.
02/14/13   kt      Adding pm_get_pbs_info API to get the pmic device pbs info.
01/28/13   kt      Adding pm_get_pmic_info API to get the pmic device info.
01/28/13   kt      Removing pm_set_hardware_version and pm_get_hardware_version APIs.
01/10/13   kt      Changing the file as per Henry's Phase1 changes.
03/10/12   wra     Removed old PMIC versions and added Badger PMIC versions.
                    Reduced hardware versions in enumeration
10/18/11   jtn/mpt Add PM8821 and PM8038
04/04/11   hw      Added pm_get_hardware_version and pm_set_hardware_version API
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
07/05/10   wra     Added pm_model_type entry for the PM8921 and PM8018
07/05/10   wra     Added file header and pm_model_type entry for the PM8901
                   and ISL9519
===========================================================================*/

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
/**
 * @enum pm_model_type
 * @brief This enum gives the PMIC model type values (Peripheral Subtype values)
 *          for different PMIC Models.
 */
typedef enum
{
   PMIC_IS_UNKNOWN   = 0,  /*!< Unknown. */
   PMIC_IS_PM8941    = 1,  /*!< 1 -- PM8941. */
   PMIC_IS_PM8841    = 2,  /*!< 2 -- PM8841. */
   PMIC_IS_PM8019    = 3,  /*!< 3 -- PM8019. */
   PMIC_IS_PMD9607   = 3,  /*!< 3 -- PMD9607. */
   PMIC_IS_PM8026    = 4,  /*!< 4 -- PM8026. */
   PMIC_IS_PM8110    = 5,  /*!< 5 -- PM8110. */
   PMIC_IS_PMA8084   = 6,  /*!< 6 -- PMA8084. */
   PMIC_IS_PMI8962   = 7,  /*!< 7 -- PMI8962. */
   PMIC_IS_PMD9635   = 8,  /*!< 8 -- PMD9635. */
   PMIC_IS_PMD9645   = 8,  /*!< 8 -- PMD9645. */
   PMIC_IS_PM8994    = 9,  /*!< 9 -- PM8994. */
   PMIC_IS_PMI8994   = 10, /*!< 10 -- PMI8994. */
   PMIC_IS_PM8916    = 11, /*!< 11 -- PM8916. */
   PMIC_IS_PM8004    = 12, /*!< 12 -- PM8004. */
   PMIC_IS_PM8909    = 13, /*!< 13 -- PM8909. */
   PMIC_IS_PM8950    = 16, /*!< 16 -- PM8950. */
   PMIC_IS_PMI8950   = 17, /*!< 17 -- PMI8950. */
   PMIC_IS_PMI8937   = 17, /**< 17 -- PMI8937. */
   PMIC_IS_PMK8001   = 18, /**< 18 -- PMK8001. */
   PMIC_IS_PM8937    = 25, /**< 25 -- PM8937. */
   PMIC_IS_INVALID   = 0x7FFFFFFF,
} pm_model_type;

/**
 * @struct pm_device_info_type
 * @brief Stores the PMIC's Model type value, the All Layer
 *        Revision number and the Metal Revision number. Please refer
 *        to pm_model_type enum above to get more info on which PMIC
 *        model type value represents which PMIC.For example, for
 *        PM8019 v2.1 the PMIC model type value is 3 (since PM8019
 *        model type value is 3 as per pm_model_type enum), All layer
 *        revision number is '2' and Metal revision number is '1'.
 */
typedef struct
{
  pm_model_type  ePmicModel;             /*!< PMIC device model type. */
  uint32         nPmicAllLayerRevision;  /*!< PMIC device all layer revision number. */
  uint32         nPmicMetalRevision;     /*!< PMIC device metal revision number. */
} pm_device_info_type;

/**
 * @brief Number of LOT IDs. Each Lot id represents an Ascii value.
 */
#define PM_PBS_INFO_NUM_LOT_IDS   12

/** 
  @struct pm_pbs_info_mfgid_type 
  @brief Stores the PMIC Manufacture ID Info.
         Major Version, Minor Version and Shrink/Non-Shrink info.
 */
typedef struct
{
  uint32  major : 4;                        /* FT Major revision */
  uint32  minor : 3;                        /* FT Minor revision */
  uint32  shrink : 1;                       /* FT Shrink/Non-Shrink */
  uint32  reserved : 24;                    /* FT Shrink/Non-Shrink */
} pm_pbs_info_mfgid_type;

/** 
  Stores the PMIC PBS-related information, such as PBS Lot
         ID, ROM version, and RAM version. The PBS ROM/RAM revision
         ID and variant (or branch) ID are stored in the last 16
         bits(upper and lower 8 bits) of nPBSROMVersion and
         nPBSRAMVersion.
 */
typedef struct
{
  uint8          nLotId[PM_PBS_INFO_NUM_LOT_IDS];  /**< PMIC device lot number. */
  uint32         nPBSROMVersion;                   /**< PBS ROM Version number. */
  uint32         nPBSRAMVersion;                   /**< PBS RAM Version number. */
  uint32         nFabId;                           /**< PBS Fab Id */
  uint32         nWaferId;                         /**< PBS Wafer Id */
  uint32         nXcoord;                          /**< PBS X Coord */
  uint32         nYcoord;                          /**< PBS Y Coord */
  uint32         nTestPgmRev;                      /**< PBS Test PGM Rev */
  pm_pbs_info_mfgid_type  eMfgId;                  /**< PBS MGF ID */
} pm_pbs_info_type;


/*===========================================================================

                PMIC MODEL/VERSION READ FUNCTION PROTOTYPE

===========================================================================*/

/**
 * @name pm_get_pmic_model
 *
 * @brief Returns the PMIC's model type value. For example, this function
 *          returns 4 for PM8x26. See #pm_model_type for more information as to
 *          which PMIC model type value represents which PMIC.
 *
 * @param [in] pmic_device_index Primary: 0. Secondary: 1
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 */
pm_model_type pm_get_pmic_model(uint8 pmic_chip);

/**
 * @name pm_get_pmic_revision
 *
 * @brief Returns the PMIC's all layer revision number.
 *          For example, this function returns 1 for PMIC v1.x, 2 for PMIC v2.x, etc.
 *
 * @param [in] pmic_device_index Primary: 0. Secondary: 1
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 */
uint8 pm_get_pmic_revision(uint8 pmic_chip);

/**
 * @name pm_get_pmic_info
 *
 * @brief Returns information about a PMIC device for a specific device index
 *          in the pmic_device_info argument. This function returns the PMIC's
 *          model type value, the all layer revision number, and the metal
 *          revision number in the pmic_device_info structure.
 *
 * @param [in] pmic_device_index Primary: 0. Secondary: 1
 *
 * @param [out] pmic_device_info Variable returned to the caller with the
 *                                 PMIC device information.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 */
pm_err_flag_type pm_get_pmic_info(uint8 pmic_chip, pm_device_info_type* pmic_device_info);

/**
 *  Returns pmic option pin information.
 * @datatypes
 *  #pm_device_info_type
 *
 * @param[in] pmic_chip Primary PMIC -- 0; Secondary PMIC -- 1.
 * @param[out] pmic_opt_pin_data Option pin data returned to the caller
 *
 * @return
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_get_pmic_option_pin_info(uint8 pmic_index, uint8 *pmic_opt_pin_data);

/**
 * @name pm_get_pbs_info
 *
 * @brief Returns information about the specified PMIC device, such as PBS
 *          lot ID, ROM version, RAM version, fab ID, wafer ID, X coordinate,
 *          and Y coordinate, for a specified device index in the pbs_info argument.
 *
 * @param [in] pmic_device_index Primary: 0. Secondary: 1
 *
 * @param [out] pbs_info Variable returned to the caller with the PMIC
 *                         device PBS information.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 */
pm_err_flag_type pm_get_pbs_info(uint8 pmic_chip, pm_pbs_info_type* pbs_info);

#endif    /* PM_VERSION_H */
