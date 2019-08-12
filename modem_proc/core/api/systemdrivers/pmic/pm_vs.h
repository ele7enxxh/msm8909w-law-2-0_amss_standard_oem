#ifndef PM_VS_H
#define PM_VS_H

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
 * @file pm_vs.h PMIC VOLTAGE SWITCHER related declaration.
 *
 * @brief This header file contains enums and API definitions for the voltage
 *        switcher for PMIC regulator driver.
 */

/* ==========================================================================

                  P M    H E A D E R    F I L E

========================================================================== */

/* ==========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/pmic/pm_vs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/14   mr      Doxygen complaint PMIC Header (CR-602405)
12/06/13   mr      (Tech Pubs) Edited/added Doxygen comments and markup (CR-522045)
01/10/13   kt      Changing the file as per Henry's Phase1 changes.
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
06/04/10   hw      Creation of Voltage Switch (VS) Module
========================================================================== */

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "com_dtypes.h"


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
/**
 *  @brief Voltage switcher peripheral index. This enum type contains all
 *         required VS regulators.
 */
enum
{
  PM_VS_LVS_1,     /**< PM_VS_LVS_1 */
  PM_VS_LVS_2,     /**< PM_VS_LVS_2 */
  PM_VS_LVS_3,     /**< PM_VS_LVS_3 */
  PM_VS_LVS_4,     /**< PM_VS_LVS_4 */
  PM_VS_LVS_5,     /**< PM_VS_LVS_5 */
  PM_VS_LVS_6,     /**< PM_VS_LVS_6 */
  PM_VS_LVS_7,     /**< PM_VS_LVS_7 */
  PM_VS_LVS_8,     /**< PM_VS_LVS_8 */
  PM_VS_LVS_9,     /**< PM_VS_LVS_9 */
  PM_VS_LVS_10,    /**< PM_VS_LVS_10 */
  PM_VS_LVS_11,    /**< PM_VS_LVS_11 */
  PM_VS_LVS_12,    /**< PM_VS_LVS_12 */
  PM_VS_MVS_1,     /**< PM_VS_MVS_1 */
  PM_VS_OTG_1,     /**< PM_VS_OTG_1 */
  PM_VS_HDMI_1,    /**< PM_VS_HDMI_1 */
  PM_VS_INVALID
};

/*===========================================================================

                VOLTAGE SWITCHER FUNCTION PROTOTYPE

===========================================================================*/
/**
 * @name pm_vs_sw_mode_status
 *
 * @brief This function returns the mode status (LPM, NPM, AUTO, BYPASS)
 *        of the selected power rail. Note, the mode of a regulator
 *        changes dynamically.
 *
 * @param [in] pmic_chip Primary: 0. Secondary: 1
 * @param [in] vs_peripheral_index VS peripheral index.
 *
 * @param [out] sw_mode Variable to return to the caller with switcher mode status.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 */
pm_err_flag_type pm_vs_sw_mode_status(uint8 pmic_chip,
                                      uint8 vs_peripheral_index,
                                      pm_sw_mode_type* sw_mode);

/**
 * @name pm_vs_pin_ctrled_status
 *
 * @brief Returns the pin controlled status or hardware enable status (On/Off)
 *        of the selected power rail.
 *
 * @param [in] pmic_chip Primary: 0. Secondary: 1
 * @param [in] vs_peripheral_index VS peripheral index.
 *
 * @param [out] on_off Variable returned to the caller with the switcher pin
 *                     control status.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 */
pm_err_flag_type pm_vs_pin_ctrled_status(uint8 pmic_chip,
                                        uint8 vs_peripheral_index,
                                        pm_on_off_type* on_off);

/** 
 * @name pm_vs_pin_ctrl_status 
 *  
 * @brief This function returns the selected pins being followed 
 *        by the selected power rail enable.
 * 
 * @param[in]  pmic_chip. Primary: 0. Secondary: 1
 * @param[in]  perph_index:
 *                Starts from 0 (for first VS peripheral)
 * @param[out] select_pin Variable returned to the caller with 
 *                        selected pins being followed by the
 *                        power rail.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS else ERROR.
 *
 */
pm_err_flag_type pm_vs_pin_ctrl_status(uint8 pmic_chip, 
                                       uint8 perph_index, 
                                       uint8 *select_pin);

/**
 * @name pm_vs_sw_enable_status
 *
 * @brief Returns the software enable status (On/Off) of the selected power rail.
 *
 * @param [in] pmic_chip Primary: 0. Secondary: 1
 * @param [in] vs_peripheral_index VS peripheral index.
 *
 * @param [out] on_off Variable returned to the caller with the switcher enable status.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 */
pm_err_flag_type pm_vs_sw_enable_status (uint8 pmic_chip,
                                         uint8 vs_peripheral_index,
                                         pm_on_off_type* on_off);

/**
 * @name pm_vs_vreg_ok_status 
 *  
 * @brief Returns the VREG_OK/VREG_READY status of the rail.
 * 
 * @param[in] pmic_chip Primary -- 0; Secondary -- 1.
 * @param[in] vs_peripheral_index VS peripheral index. Starts 
 *       from 0 (for the first VS peripheral).
 * @param[out] on_off On/Off status.
 * 
 * @return 
 *   SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_vs_vreg_ok_status (uint8 pmic_chip, 
                                       uint8 vs_peripheral_index, 
                                       boolean* on_off);

#endif    /* PM_VS_H */
