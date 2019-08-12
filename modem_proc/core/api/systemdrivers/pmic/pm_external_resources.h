#ifndef PM_EXTERNAL_RESOURCES_H
#define PM_EXTERNAL_RESOURCES_H

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
 * @file pm_external_resources.h PMIC External Resources header file.
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

$Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/pmic/pm_external_resources.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/14   mr      Doxygen complaint PMIC Header (CR-602405)
01/10/13   kt      Removed Power Rail IDs.
09/29/11   hs      Added CLK resource IDs.
09/14/11   dy      Move UICC resource IDs to this file
04/25/11   wra     Adding Battery Temperature Management BTM, LPG, more LVS, ADC_ARB,
                    OVP & INTerrupt channel enumeration needed for PM8921
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
07/05/10   wra     Modified Header includes to accomodate MSM8660
=============================================================================*/

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
/**
 * @brief Chargers: This enum contains definitions of all chargers in the target.
 */
enum
{
  PM_CHG_1,
  PM_CHG_INVALID
};

/**
 * @brief This enum contains definitions of all High current LED resources in the target.
 */
enum
{
  PM_LEDH_1,
  PM_LEDH_2,
  PM_LEDH_3,
  PM_LEDH_INVALID
};

/**
 * @brief This enum contains definitions of all Low current LED resources in the target.
 */
enum
{
  PM_LEDL_1,
  PM_LEDL_2,
  PM_LEDL_3,
  PM_LEDL_INVALID
};

/**
 * @brief This enum contains definitions of all PWM resources in the target.
 */
enum
{
  PM_PWM_1,
  PM_PWM_2,
  PM_PWM_3,
  PM_PWM_INVALID
};

/**
 * @brief This enum contains definitions of all HSED resources in the target.
 */
enum
{    
  PM_HSED_1,
  PM_HSED_2,
  PM_HSED_3,
  PM_HSED_INVALID
};

/**
 * @brief This enum contains definitions of all External Charging Hardware
 *		  Resources in the target.
 */
enum
{
  PM_CHG_EXT_1,
  PM_CHG_EXT_INVALID
}; 

/**
 * @brief This enum contains definitions of all Switched Mode Battery Charger
 *		  Resources in the target.
 */
enum
{
  PM_SMBC_1,
  PM_SMBC_INVALID
};

/**
 * @brief This enum contains definitions of all Battery Management System
 *		  Resources in the target.
 */
enum
{
  PM_BMS_1,
  PM_BMS_INVALID
};

/**
 * @brief This enum contains definitions of all Battery Temperature Management
 *		  Resources in the target.
 */
enum
{
  PM_BTM_1,
  PM_BTM_INVALID
};

/**
 * @brief This enum contains definitions of all ADC Arbiter Channel Resources in the target
 *          Typically a processor will own a channel and the arbiter will provide equal access
 *          to the ADC for each processor.
 */
enum
{
    PM_ARB_ADC_1, 
    PM_ARB_ADC_2,
    PM_ARB_ADC_3,
    PM_ARB_ADC_INVALID
};

/**
 * @brief This enum contains definitions of all Light Pulse Generator LPG
 *		  Resources in PMIC.
 */
enum
{
    PM_LPG_1,
    PM_LPG_2,
    PM_LPG_3,
    PM_LPG_4,
    PM_LPG_5,
    PM_LPG_6,
    PM_LPG_7,
    PM_LPG_8,
    PM_LPG_INVALID
};

/**
 * @brief This enum contains definitions of Interrupt resources available on
 *		  PMIC.
 */
enum
{
    PM_INTERRUPT_1, //RPM/SEC
    PM_INTERRUPT_2, //MDM
    PM_INTERRUPT_3, //APP/USER
    PM_INTERRUPT_INVALID
};

/**
 * @brief This enum contains definitions of all Over Voltage Protection OVP
 *		  resources available on PMIC component.
 */
enum
{
    PM_OVP_1,
    PM_OVP_2,
    PM_OVP_INVALID
};

/**
 * @brief This enum contains definitions the Mega XO resources available
 *		  on PMIC component.
 */
enum
{
    PM_MEGA_XO_1,
    PM_MEGA_XO_2,
    PM_MEGA_XO_3,
    PM_MEGA_XO_4,
    PM_MEGA_XO_5,
    PM_MEGA_XO_6,
    PM_MEGA_XO_7,
    PM_MEGA_XO_8,
    PM_MEGA_XO_9,
    PM_MEGA_XO_INVALID,
};

/**
 * @brief This enum contains definitions the UICC resources available on PMIC component.
 */
enum
{
  PM_BATTERY_ALARM,
  PM_UICC_1,
  PM_UICC_2,
  PM_UICC_3,
  PM_UICC_INVALID
};


#endif    /* PM_EXTERNAL_RESOURCES_H */

