#ifndef PM_SMPS_H
#define PM_SMPS_H

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
 * @file pm_smps.h PMIC SMPS related declaration.
 *
 * @brief This header file contains enums and API definitions for the Switched
 *        Mode Power Supply (SMPS) PMIC voltage regulator driver.
 */

/* ==========================================================================

                  P M    H E A D E R    F I L E

========================================================================== */

/* ==========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/pmic/pm_smps.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/14   mr      Doxygen complaint PMIC Header (CR-602405)
12/06/13   mr      (Tech Pubs) Edited/added Doxygen comments and markup (CR-522045)
08/26/13   rk      remove pin control support and not supported SMPS.
12/06/12   hw      Rearchitecturing module driver to peripheral driver
=============================================================================*/

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "com_dtypes.h"


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
/** @addtogroup pm_smps
@{ */

/**
 * @brief SMPS peripheral index. This enum type contains all required LDO regulators.
 */
enum
{
  PM_SMPS_1,   /**< SMPS 1. */
  PM_SMPS_2,   /**< SMPS 2. */
  PM_SMPS_3,   /**< SMPS 3. */
  PM_SMPS_4,   /**< SMPS 4. */
  PM_SMPS_5,   /**< SMPS 5. */
  PM_SMPS_6,   /**< SMPS 6. */
  PM_SMPS_7,   /**< SMPS 7. */
  PM_SMPS_8,   /**< SMPS 8. */
  PM_SMPS_9,   /**< SMPS 9. */
  PM_SMPS_10,  /**< SMPS 10. */
  PM_SMPS_11,  /**< SMPS 11. */
  PM_SMPS_12,  /**< SMPS 12. */
  PM_SMPS_INVALID
};

/**
 * @enum pm_smps_ilim_mode_type
 * @brief SMPS mode type.
 */
typedef enum
{
    PM_ILIM_SMPS_PWM_MODE,  /*!< SMPS PWM mode. */
    PM_ILIM_SMPS_AUTO_MODE, /*!< SMPS auto mode. For SMPS mode
                                 transition between PFM and PWM. */
    PM_ILIM_SMPS_MODE_INVALID
}pm_smps_ilim_mode_type;

/**
 * @enum pm_clk_src_type
 * @brief Clock source type.
 */
typedef enum
{
   PM_CLK_TCXO,
   PM_CLK_RC,
   PM_CLK_SOURCE_INVALID
}pm_clk_src_type;

/**
 * @enum pm_smps_switching_freq_type
 * @brief SMPS switching frequency. This enumeration assumes an input clock
 *        frequency of 19.2 MHz and is 5 bits long.
 *        Clock frequency = (input clock freq) / ((CLK_PREDIV+1)(CLK_DIV + 1)).
 */
typedef enum
{
    PM_CLK_19p2_MHz     = 0,   /*!< Clock frequency = 19.2 MHz. */
    PM_CLK_9p6_MHz      = 1,   /*!< Clock frequency = 9.6 MHz. */
    PM_CLK_6p4_MHz      = 2,   /*!< Clock frequency = 6.2 MHz. */
    PM_CLK_4p8_MHz      = 3,   /*!< Clock frequency = 4.8 MHz. */
    PM_CLK_3p84_MHz     = 4,   /*!< Clock frequency = 3.84 MHz. */
    PM_CLK_3p2_MHz      = 5,   /*!< Clock frequency = 3.2 MHz. */
    PM_CLK_2p74_MHz     = 6,   /*!< Clock frequency = 2.74 MHz. */
    PM_CLK_2p4_MHz      = 7,   /*!< Clock frequency = 2.4 MHz. */
    PM_CLK_2p13_MHz     = 8,   /*!< Clock frequency = 2.13 MHz. */
    PM_CLK_1p92_MHz     = 9,   /*!< Clock frequency = 1.92 MHz. */
    PM_CLK_1p75_MHz     = 10,  /*!< Clock frequency = 1.75 MHz. */
    PM_CLK_1p6_MHz      = 11,  /*!< Clock frequency = 1.6 MHz. */
    PM_CLK_1p48_MHz     = 12,  /*!< Clock frequency = 1.48 MHz. */
    PM_CLK_1p37_MHz     = 13,  /*!< Clock frequency = 1.37 MHz. */
    PM_CLK_1p28_MHz     = 14,  /*!< Clock frequency = 1.28 MHz. */
    PM_CLK_1p2_MHz      = 15,  /*!< Clock frequency = 1.2 MHz. */
    PM_CLK_1p13_MHz     = 16,  /*!< Clock frequency = 1.13 MHz. */
    PM_CLK_1p07_MHz     = 17,  /*!< Clock frequency = 1.07 MHz. */
    PM_CLK_1p01_MHz     = 18,  /*!< Clock frequency = 1.01 MHz. */
    PM_CLK_960_KHz      = 19,  /*!< Clock frequency = 960 kHz. */
    PM_CLK_914_KHz      = 20,  /*!< Clock frequency = 914 kHz. */
    PM_CLK_873_KHz      = 21,  /*!< Clock frequency = 873 kHz. */
    PM_CLK_835_KHz      = 22,  /*!< Clock frequency = 835 kHz. */
    PM_CLK_800_KHz      = 23,  /*!< Clock frequency = 800 kHz. */
    PM_CLK_768_KHz      = 24,  /*!< Clock frequency = 768 kHz. */
    PM_CLK_738_KHz      = 25,  /*!< Clock frequency = 738 kHz. */
    PM_CLK_711_KHz      = 26,  /*!< Clock frequency = 711 kHz. */
    PM_CLK_686_KHz      = 27,  /*!< Clock frequency = 686 kHz. */
    PM_CLK_662_KHz      = 28,  /*!< Clock frequency = 662 kHz. */
    PM_CLK_640_KHz      = 29,  /*!< Clock frequency = 640 kHz. */
    PM_CLK_619_KHz      = 30,  /*!< Clock frequency = 619 kHz. */
    PM_CLK_600_KHz      = 31,  /*!< Clock frequency = 600 kHz. */
    PM_SWITCHING_FREQ_INVALID,
    PM_SWITCHING_FREQ_FREQ_NONE
}pm_smps_switching_freq_type;

/**
 * @enum pm_quiet_mode_type
 * @brief Quiet mode.
 */
typedef enum
{
    PM_QUIET_MODE__DISABLE,      /*!< Quiet mode disabled (default). */
    PM_QUIET_MODE__QUIET,        /*!< Quiet mode enabled. */
    PM_QUIET_MODE__SUPER_QUIET,  /*!< Super quiet mode enabled. */
    PM_QUIET_MODE__INVALID
}pm_quiet_mode_type;


/*===========================================================================

                    SMPS DRIVER API PROTOTYPE

===========================================================================*/
/**
 * @name pm_smps_sw_mode_status
 *
 * @brief This function returns the mode status (LPM, NPM, AUTO, BYPASS)
 *        of the selected power rail. Note, the mode of a regulator
 *        changes dynamically.
 *
 * @param [in] pmic_chip Primary: 0. Secondary: 1
 * @param [in] smps_peripheral_index Starts from 0 (for first SMPS peripheral)
 *
 * @param [out] sw_mode Variable to return to the caller with mode status.
 *                     Refer to pm_resources_and_types.h for the enum info.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 */
pm_err_flag_type pm_smps_sw_mode_status(uint8 pmic_chip,
                                        uint8 smps_peripheral_index,
                                        pm_sw_mode_type* sw_mode);

/**
 * @name pm_smps_pin_ctrled_status
 *
 * @brief This function returns the pin ctrled status or h/w
 *        enable status (On/Off) of the selected power rail.
 *
 * @param[in]  pmic_chip Primary: 0. Secondary: 1
 * @param[in]  smps_peripheral_index Starts from 0 (for first SMPS peripheral)
 *
 * @param[out] on_off Variable to return to the caller with pin ctrl status.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Under error conditions
 *          like invalid param entered.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type pm_smps_pin_ctrled_status(uint8 pmic_chip,
                                            uint8 smps_peripheral_index,
                                            pm_on_off_type* on_off);

/** 
 * @name pm_smps_pin_ctrl_status 
 *  
 * @brief This function returns the selected pins being followed 
 *        by the selected power rail enable.
 * 
 * @param[in]  pmic_chip. Primary: 0. Secondary: 1
 * @param[in]  perph_index:
 *                Starts from 0 (for first SMPS peripheral)
 * @param[out] select_pin Variable returned to the caller with 
 *                        selected pins being followed by the
 *                        power rail.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS else ERROR.
 *
 */
pm_err_flag_type pm_smps_pin_ctrl_status(uint8 pmic_chip, 
                                         uint8 perph_index, 
                                         uint8 *select_pin);

/**
 * @name pm_smps_volt_level_status
 *
 * @brief This function returns the voltage level (in micro volts) of the
 *        selected power rail.
 *
 * @param [in] pmic_chip Primary: 0. Secondary: 1
 * @param [in] smps_peripheral_index Starts from 0 (for first SMPS peripheral)
 *
 * @param [out] volt_level Variable to return to the caller with volt
 *              level status in micro volts (uint32).
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 */
pm_err_flag_type pm_smps_volt_level_status (uint8 pmic_chip,
                                            uint8 smps_peripheral_index,
                                            pm_volt_level_type* volt_level);
pm_err_flag_type pm_smps_volt_level (uint8 pmic_chip,
                                    uint8 smps_peripheral_index,
                                    pm_volt_level_type volt_level);

/**
 * @name pm_smps_sw_enable_status
 *
 * @brief This function returns the s/w enable status (On/Off) of the selected power rail.
 *
 * @param [in] pmic_chip Primary: 0. Secondary: 1
 * @param [in] smps_peripheral_index Starts from 0 (for first SMPS peripheral)
 *
 * @param [out] on_off Variable to return to the caller with software
 *              status. Refer to pm_resources_and_types.h for the enum info.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 */
pm_err_flag_type pm_smps_sw_enable_status(uint8 pmic_chip,
                                          uint8 smps_peripheral_index,
                                          pm_on_off_type* on_off);

/**
 * @name pm_smps_vreg_ok_status 
 *  
 * @brief Returns the VREG_OK/VREG_READY status of the rail.
 * 
 * @param[in] pmic_chip Primary -- 0; Secondary -- 1.
 * @param[in] smps_peripheral_index SMPS peripheral index. 
 *       Starts from 0 (for the first SMPS peripheral).
 * @param[out] on_off On/Off status.
 * 
 * @return 
 *   SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_smps_vreg_ok_status(uint8 pmic_chip, 
                                        uint8 smps_peripheral_index, 
                                        boolean* on_off);

/**
 * @name pm_smps_inductor_ilim
 *
 * @brief Select the current limit for the inductor of a selected SMPS.
 *
 * @param [in] pmic_chip Primary: 0. Secondary: 1
 * @param [in] smps_peripheral_index Starts from 0 (for first SMPS peripheral)
 * @param [in] ilim_level Current limit level.
 *              Range:
 *              - 3500 mA to 600 mA
 *              - 2x SMPS subtype current limit range -- 600 mA to 2700 mA.
 *              - 3x SMPS subtype current limit range -- 700 mA to 3500 mA
 * @param [in] smps_mode SMPS mode.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 */
pm_err_flag_type pm_smps_inductor_ilim (uint8 pmic_chip,
                                        uint8 smps_peripheral_index,
                                        uint16 ilim_level,
                                        pm_smps_ilim_mode_type smps_mode);

/**
 * @name pm_smps_inductor_ilim_status
 *
 * @brief Obtain the current limit for the inductor of a selected SMPS by
 *        reading the SPMI register.
 *
 * @param [in] pmic_chip Primary: 0. Secondary: 1
 * @param [in] smps_peripheral_index Starts from 0 (for first SMPS peripheral)
 * @param [in] smps_mode SMPS mode.
 *
 * @param [out] ilim_level Current limit level.
 *              Range:
 *              - 3500 mA to 600 mA
 *              - 2x SMPS subtype current limit range -- 600 mA to 2700 mA.
 *              - 3x SMPS subtype current limit range -- 700 mA to 3500 mA
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *
 */
pm_err_flag_type pm_smps_inductor_ilim_status(uint8 pmic_chip,
                                              uint8 smps_peripheral_index,
                                              uint16* ilim_level,
                                              pm_smps_ilim_mode_type smps_mode);

/**
 * @name pm_smps_volt_calculate_vset
 *
 * @brief This function calculates the PMIC register value for
 *        a supplied voltage
 *
 * @param[in]  pmic_chip. Primary: 0. Secondary: 1
 * @param[in]  perph_index:
 *                Starts from 0 (for first SMPS peripheral)
 * @param[in]  volt_level:
 *                Voltage to convert to PMIC register value
 * @param[out] vset:
 *                PMIC register value for supplied voltage
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Under error conditions
 *          like invalid param entered.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type pm_smps_volt_calculate_vset(uint8 pmic_chip,
                                            uint8 perph_index,
                                            pm_volt_level_type volt_level,
                                            uint32* vset);

/**
 * @name pm_smps_volt_level_stepper_done_status
 *
 * @brief This function returns the stepper done status for
 *        the selected power rail
 *
 * @param[in]  pmic_chip. Primary: 0. Secondary: 1
 * @param[in]  perph_index:
 *                Starts from 0 (for first SMPS peripheral)
 * @param[out]  stepper_done:
 *                Value of stepper done register
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Under error conditions
 *          like invalid param entered.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type pm_smps_volt_level_stepper_done_status(uint8 pmic_chip,
                                                        uint8 ldo_peripheral_index,
                                                        boolean *stepper_done);

pm_err_flag_type pm_smps_clk_source(uint8 pmic_chip, pm_clk_src_type clk);
pm_err_flag_type pm_smps_quiet_mode(uint8 pmic_chip, uint8 perph_index, pm_quiet_mode_type mode, uint16 voltage_mv);

/** @} */ /* end_addtogroup pm_smps */

#endif    /* PM_SMPS_H */

