#ifndef PM_RESOURCES_AND_TYPES_H
#define PM_RESOURCES_AND_TYPES_H

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
 * @file pm_resources_and_types.h PMIC Resources related declaration.
 *
 * @brief This file contains enumerations with resource names for different
 *        module types that should be used by external clients when accessing
 *        the resources that are required.
 */

/* ==========================================================================

                  P M    H E A D E R    F I L E

========================================================================== */


/* ==========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/pmic/pm_resources_and_types.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/14   mr      Doxygen complaint PMIC Header (CR-602405)
09/25/12   hw      moving some npa type to this file so we don't need to include pmapp_npa.h
09/29/11   hs      Added CLK resource IDs.
09/14/11   dy      Move UICC resource IDs to this file
04/25/11   wra     Adding Battery Temperature Management BTM, LPG, more LVS, ADC_ARB,
                    OVP & INTerrupt channel enumeration needed for PM8921
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
07/05/10   wra     Modified Header includes to accomodate MSM8660
=============================================================================*/

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
#include "DALStdDef.h"


/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
/**
 * @brief PMIC power module voltage level
 */
typedef uint32 pm_volt_level_type;

/**
 * @enum pm_on_off_type
 * @brief Power on-off mode. 
 */
typedef enum
{
   PM_OFF,    /*!< Power OFF */ 
   PM_ON,    /*!< Power ON */ 
   PM_INVALID
}pm_on_off_type;

/**
 * @enum pm_sw_mode_type
 * @brief Regulator's power modes.
 */
typedef enum 
{
    PM_SW_MODE_LPM     = 0,	   /*!< Low power mode	*/
    PM_SW_MODE_BYPASS  = 1,	   /*!< Bypass mode       bit 5 */
    PM_SW_MODE_AUTO    = 2,    /*!< Auto mode         bit 6 */
    PM_SW_MODE_NPM     = 4,	   /*!< Normal power mode bit 7 */
    PM_SW_MODE_INVALID
}pm_sw_mode_type;

/**
  @struct peripheral_info_type
  @brief Contains Peripheral Information such as Type, Subtype, 
         Analog and Digital Major Versions. 
 */
typedef struct 
{
    uint16 base_address;
    uint8  peripheral_type;
    uint8  peripheral_subtype;
    uint8  analog_major_version;
    uint8  analog_minor_version;
    uint8  digital_major_version;
    uint8  digital_minor_version;
}peripheral_info_type;

/** @} */ /* end_addtogroup pm_resources_and_types */

#endif /* PM_RESOURCES_AND_TYPES_H */

