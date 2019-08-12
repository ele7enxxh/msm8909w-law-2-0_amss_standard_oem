#ifndef PM_SMPS__H
#define PM_SMPS__H
/*! \file pm_smps.h
 *  \n
 *  \brief This header file contains enums and API definitions for PMIC voltage
 *         regulator driver.
 *  \n
 *  \n &copy; Copyright 2010-2013 QUALCOMM Technologies Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/systemdrivers/pmic/pm_smps.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/15/14   akm     Updated SMPS ilim and freq API
04/05/13   aab     Added support for SMPS Multiphase configuration API
02/22/13   aab     Updated pm_smps_inductor_ilim() and pm_smps_inductor_ilim_status()
12/06/12   hw      Rearchitecturing module driver to peripheral driver
=============================================================================*/
/*===========================================================================

                        HEADER FILES

===========================================================================*/

#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "com_dtypes.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
/* SMPS peripheral index. This enum type contains all smps regulators that you may need. */
enum
{
  PM_SMPS_1,
  PM_SMPS_2,
  PM_SMPS_3,
  PM_SMPS_4,
  PM_SMPS_5,    
  PM_SMPS_6,
  PM_SMPS_7,
  PM_SMPS_8,
  PM_SMPS_9,
  PM_SMPS_10,  
  PM_SMPS_11,  
  PM_SMPS_12,  
  PM_SMPS_INVALID
};

/*! \enum pm_smps_ilim_mode_type
 *  \brief Different buck modes for which the current limit
 *         settings can be done.
 */
typedef enum
{
    PM_ILIM_SMPS_PWM_MODE,  
    PM_ILIM_SMPS_AUTO_MODE,
    PM_ILIM_SMPS_MODE_INVALID
}pm_smps_ilim_mode_type;

/*
 * enumeration assumes input clock freq of 19.2MHz
 * clock frequency = (input clock freq) / ((CLK_PREDIV+1)(CLK_DIV + 1))
 * this enumeration is 5 bit long
 */
typedef enum 
{
    PM_CLK_19p2_MHz     = 0,
    PM_CLK_9p6_MHz      = 1,
    PM_CLK_6p4_MHz      = 2,
    PM_CLK_4p8_MHz      = 3,
    PM_CLK_3p84_MHz     = 4,
    PM_CLK_3p2_MHz      = 5,
    PM_CLK_2p74_MHz     = 6,
    PM_CLK_2p4_MHz      = 7,
    PM_CLK_2p13_MHz     = 8,
    PM_CLK_1p92_MHz     = 9,
    PM_CLK_1p75_MHz     = 10,
    PM_CLK_1p6_MHz      = 11,
    PM_CLK_1p48_MHz     = 12,
    PM_CLK_1p37_MHz     = 13,
    PM_CLK_1p28_MHz     = 14,
    PM_CLK_1p2_MHz      = 15,
    PM_CLK_1p13_MHz     = 16,
    PM_CLK_1p07_MHz     = 17,
    PM_CLK_1p01_MHz     = 18,
    PM_CLK_960_KHz      = 19,
    PM_CLK_914_KHz      = 20,
    PM_CLK_873_KHz      = 21,
    PM_CLK_835_KHz      = 22,
    PM_CLK_800_KHz      = 23,
    PM_CLK_768_KHz      = 24,
    PM_CLK_738_KHz      = 25,
    PM_CLK_711_KHz      = 26,
    PM_CLK_686_KHz      = 27,
    PM_CLK_662_KHz      = 28,
    PM_CLK_640_KHz      = 29,
    PM_CLK_619_KHz      = 30,
    PM_CLK_600_KHz      = 31,
    PM_SWITCHING_FREQ_INVALID,
	PM_SWITCHING_FREQ_FREQ_NONE
}pm_smps_switching_freq_type;

/* Quiet Mode */
typedef enum
{
    PM_QUIET_MODE__DISABLE, // default
    PM_QUIET_MODE__QUIET,
    PM_QUIET_MODE__SUPER_QUIET,
    PM_QUIET_MODE__INVALID
}pm_quiet_mode_type;

/*===========================================================================

                        API PROTOTYPE

===========================================================================*/
/**
 * @name 
 *     pm_smps_pull_down
 *
 * @description
 *     Allows you to enable disable active pulldown.
 * 
 * @param 
 *     on_off - turn on and off active pulldown
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type pm_smps_pull_down
(uint8 pmic_chip, uint8 smps_peripheral_index, pm_on_off_type on_off);

/**
 * @name pm_smps_sw_mode 
 *
 * @description
 *     Switch between NPM, LPM, and other modes of a regulator.
 * 
 * @param 
 *     sw_mode - Select the different mode of a regulator. Example: NPM, LPM and AUTO
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type pm_smps_sw_mode
(uint8 pmic_chip, uint8 smps_peripheral_index, pm_sw_mode_type sw_mode);

/** 
 * @name pm_smps_sw_mode_status 
 *  
 * @brief This function returns the mode status (LPM, NPM, AUTO, BYPASS)
 *        of the selected power rail. Note, the mode of a regulator
 *        changes dynamically.
 * 
 * @param[in]  pmic_chip. Primary: 0. Secondary: 1
 * @param[in]  smps_peripheral_index:
 *                Starts from 0 (for first SMPS peripheral)
 * @param[out] sw_mode:  
 *                Variable to return to the caller with mode status.
 *                Refer to pm_resources_and_types.h for the enum info.
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__SUCCESS = SUCCESS else error.
 *
 */
pm_err_flag_type pm_smps_sw_mode_status
(uint8 pmic_chip, uint8 smps_peripheral_index, pm_sw_mode_type* sw_mode);

/**
 * @name 
 *     pm_smps_pin_ctrled
 *
 * @description
 *     select the pin ( connected to external signal ) that you would like to use
 *     to effect the state ( on/off ) and mode ( HPM, LPM etc ) of a regulator
 * 
 * @param 
 *     select_pin - Select a pin
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type pm_smps_pin_ctrled
(uint8 pmic_chip, uint8 smps_peripheral_index, uint8 select_pin);

/**
 * @name pm_smps_sw_enable 
 *
 * @description
 *     enables or disables the SMPS rail.
 * 
 * @param 
 *     on_off - turn on or off the SMPS.
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type pm_smps_sw_enable
(uint8 pmic_chip, uint8  smps_peripheral_index, pm_on_off_type  on_off);

/**
 * @name 
 *     pm_smps_volt_level
 *
 * @description
 *     set the voltage of a regulator.
 *     Note, differnt type ( LDO, HFS etc ) may have different programmable voltage steps.
 *     Only support the correct programmable steps. Not rounding voltages if the voltage selected
 *     is not part of the programmable steps.
 * 
 * @param 
 *     volt_level - select the voltage in micro volts.
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type pm_smps_volt_level
(uint8 pmic_chip, uint8 smps_peripheral_index, pm_volt_level_type volt_level);

/** 
 * @name pm_smps_volt_level_status 
 *  
 * @brief This function returns the voltage level (in micro 
 *        volts) of the selected power rail.
 * 
 * @param[in]  pmic_chip. Primary: 0. Secondary: 1
 * @param[in]  smps_peripheral_index:
 *                Starts from 0 (for first SMPS peripheral)
 * @param[out] volt_level:  
 *                Variable to return to the caller with volt
 *                level status in micro volts (uint32).
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__SUCCESS = SUCCESS else ERROR.
 *
 */
pm_err_flag_type pm_smps_volt_level_status
(uint8 pmic_chip, uint8 smps_peripheral_index, pm_volt_level_type* volt_level);


/**
 * @name 
 *     pm_smps_switching_freq
 *
 * @description
 *    select a desire frequency for the SMPS. Each SMPS can operate under different frequency.
 * 
 * @param 
 *  
 *     pmic_chip. Primary: 0. Secondary: 1
 *  
 *     smps_peripheral_index: Starts from 0 (for first SMPS
 *     peripheral) 
 *  
 *     switching_freq - Select frequency such as 19.2MHz, 3.2MHz etc.
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type pm_smps_switching_freq
(uint8 pmic_chip, uint8 smps_peripheral_index, pm_smps_switching_freq_type switching_freq);

/**
 * @name 
 *     pm_smps_switching_freq_status
 *
 * @description
 *    obtain the frequency for the SMPS. Each SMPS can operate
 *    under different frequency.
 * 
 * @param 
 *  
 *     pmic_chip. Primary: 0. Secondary: 1
 *  
 *     smps_peripheral_index: Starts from 0 (for first SMPS
 *     peripheral) 
 *  
 *     freq - Returns the freq of SMPS.
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type pm_smps_switching_freq_status
(uint8 pmic_chip, uint8 smps_peripheral_index, pm_smps_switching_freq_type *freq);

/**
 * @name 
 *     pm_smps_inductor_ilim
 *
 * @description
 *    Select the current limit for the inductor of a selected
 *    SMPS.
 * 
 * @param 
 *  
 *     pmic_chip. Primary: 0. Secondary: 1
 *  
 *     smps_peripheral_index: Starts from 0 (for first SMPS
 *     peripheral)
 *  
 *     ilim_level_milli_amps - Select the current limit level in
 *     milli-amps. If a current limit level between the
 *     supported current limit values is sent then it's rounded
 *     to the upper limit value.
 *  
 *     smps_mode :
 *      PM_ILIM_SMPS_PWM_MODE :  Current limit for PWM mode
 *      PM_ILIM_SMPS_AUTO_MODE:  Current limit for AUTO mode
 *      (Whenever there is a mode transition b/n PFM and PWM
 *      mode under Auto-mode operation) Current limit setting
 *      for AUTO mode is not supported on FTS bucks.
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 * @example 
 *     pm_smps_inductor_ilim(0,PM_SMPS_1,2500,PM_ILIM_SMPS_PWM_MODE);
 */
pm_err_flag_type pm_smps_inductor_ilim 
(uint8 pmic_chip, uint8 smps_peripheral_index, uint32 ilim_level_milli_amps, pm_smps_ilim_mode_type smps_mode);

/**
 * @name 
 *     pm_smps_inductor_ilim_status
 *
 * @description
 *    Obtain the current limit for the inductor of a selected
 *    SMPS by reading the SPMI register.
 * 
 * @param 
 *  
 *     pmic_chip. Primary: 0. Secondary: 1
 *  
 *     smps_peripheral_index: Starts from 0 (for first SMPS
 *     peripheral)
 *  
 *     *ilim_level_milli_amps - Returns the current limit level in
 *      milli-amps
 *  
 *     smps_mode : 
 *      PM_ILIM_SMPS_PWM_MODE :  Current limit for PWM mode
 *      PM_ILIM_SMPS_AUTO_MODE:  Current limit for AUTO mode
 *      (Whenever there is a mode transition b/n PFM and PWM
 *      mode under Auto-mode operation) Current limit setting
 *      for AUTO mode is not supported on FTS bucks.
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type pm_smps_inductor_ilim_status
(uint8 pmic_chip, uint8 smps_peripheral_index, uint32 *ilim_level_milli_amps, pm_smps_ilim_mode_type smps_mode);

/**
 * @name 
 *     pm_smps_quiet_mode
 *
 * @description
 *    Set quiet mode for SMPS.
 * 
 * @param 
 *     quiet_mode - Enable/Disable quiet mode as well as selecting different types of quiet mode.
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type pm_smps_quiet_mode
(uint8 pmic_chip, uint8 smps_peripheral_index, pm_quiet_mode_type quiet_mode, uint16 voltage_mv);

/**
 * @name 
 *     pm_smps_set_phase
 *
 * @description
 *    sets the phase count for a selected SMPS.
 * 
 * @param 
 *     phase - Phase count to configured for the SMPS.
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type pm_smps_set_phase 
(uint8 pmic_chip, uint8 smps_peripheral_index, pm_phase_cnt_type phase);

/**
 * @name 
 *     pm_smps_get_phase
 *
 * @description
 *    returns the phase count for the selected SMPS.
 * 
 * @param 
 *     *phase - Provides the phase count of the selected SMPS.
 * 
 * @return 
 *     error flag type - pm_err_flag_type
 */
pm_err_flag_type pm_smps_get_phase
(uint8 pmic_chip, uint8 smps_peripheral_index, pm_phase_cnt_type *phase);

/** 
 * @name pm_smps_vreg_ok_status 
 *  
 * @brief This function returns the vreg_ok status.
 * 
 * @param[in]  pmic_chip. Primary: 0. Secondary: 1
 * @param[in]  ldo_peripheral_index:
 *                Starts from 0 (for first LDO peripheral)
 * @param[out] status:  
 *                Variable to return to the caller with vreg_ok status 
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Under error conditions
 *          like invalid param entered.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type pm_smps_vreg_ok_status
(uint8 pmic_chip, uint8 smps_peripheral_index, boolean* status);




#endif /* PM_SMPS__H */
