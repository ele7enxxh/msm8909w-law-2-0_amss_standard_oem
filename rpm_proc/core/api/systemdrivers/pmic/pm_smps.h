#ifndef PM_SMPS__H
#define PM_SMPS__H

/** @file pm_smps.h 
 *
 * This header file contains enums and API definitions for PMIC SMPS
 *         power rail driver.
 *
*/
/*
 * Copyright (c) 2012-2014 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rpm.bf/2.1.1/core/api/systemdrivers/pmic/pm_smps.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/03/14   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
09/03/14   mr      Added support for SMPS Volt Level API. (CR-713705)
05/07/14   rk      PMIC API to read the volt level of SMPS
09/03/12   rk      Remove unused SMPS and pin control and Quiet Mode functionality.
02/04/13   hs      pm_smps_sw_mode_status_lite.
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
/** @addtogroup pm_smps
@{ */


/** @anonenum{smps_peripheral_index} @xreflabel{hdr:smpsPeripheralIndex}

   SMPS peripheral index, containing all necessary SMPS regulators.
*/
enum
{
  PM_SMPS_1,   /**< SMPS 1. */
  PM_SMPS_2,   /**< SMPS 2. */
  PM_SMPS_3,   /**< SMPS 3. */
  PM_SMPS_4,   /**< SMPS 4. */
  PM_SMPS_5,   /**< SMPS 5. */    
  PM_SMPS_6,   /* SMPS 6. */
  PM_SMPS_7,   /* SMPS 7. */
  PM_SMPS_8,   /* SMPS 8. */
  PM_SMPS_9,   /* SMPS 9. */
  PM_SMPS_10,  /* SMPS 10. */  
  PM_SMPS_11,  /* SMPS 11. */   
  PM_SMPS_12,  /* SMPS 12. */   
  PM_SMPS_INVALID
};

/** SMPS switching frequency. This enumeration assumes an input clock
    frequency of 19.2 MHz and is five bits long.

    Clock frequency = 
    (input clock frequency) / ((CLK_PREDIV+1)(CLK_DIV + 1)).
 */
typedef enum
{
    PM_CLK_19p2_MHz     = 0,   /**< Clock frequency = 19.2 MHz. */
    PM_CLK_9p6_MHz      = 1,   /**< Clock frequency = 9.6 MHz. */
    PM_CLK_6p4_MHz      = 2,   /**< Clock frequency = 6.2 MHz. */
    PM_CLK_4p8_MHz      = 3,   /**< Clock frequency = 4.8 MHz. */
    PM_CLK_3p84_MHz     = 4,   /**< Clock frequency = 3.84 MHz. */
    PM_CLK_3p2_MHz      = 5,   /**< Clock frequency = 3.2 MHz. */
    PM_CLK_2p74_MHz     = 6,   /**< Clock frequency = 2.74 MHz. */
    PM_CLK_2p4_MHz      = 7,   /**< Clock frequency = 2.4 MHz. */
    PM_CLK_2p13_MHz     = 8,   /**< Clock frequency = 2.13 MHz. */
    PM_CLK_1p92_MHz     = 9,   /**< Clock frequency = 1.92 MHz. */
    PM_CLK_1p75_MHz     = 10,  /**< Clock frequency = 1.75 MHz. */
    PM_CLK_1p6_MHz      = 11,  /**< Clock frequency = 1.6 MHz. */
    PM_CLK_1p48_MHz     = 12,  /**< Clock frequency = 1.48 MHz. */
    PM_CLK_1p37_MHz     = 13,  /**< Clock frequency = 1.37 MHz. */
    PM_CLK_1p28_MHz     = 14,  /**< Clock frequency = 1.28 MHz. */
    PM_CLK_1p2_MHz      = 15,  /**< Clock frequency = 1.2 MHz. */
    PM_CLK_1p13_MHz     = 16,  /**< Clock frequency = 1.13 MHz. */
    PM_CLK_1p07_MHz     = 17,  /**< Clock frequency = 1.07 MHz. */
    PM_CLK_1p01_MHz     = 18,  /**< Clock frequency = 1.01 MHz. */
    PM_CLK_960_KHz      = 19,  /**< Clock frequency = 960 kHz. */
    PM_CLK_914_KHz      = 20,  /**< Clock frequency = 914 kHz. */
    PM_CLK_873_KHz      = 21,  /**< Clock frequency = 873 kHz. */
    PM_CLK_835_KHz      = 22,  /**< Clock frequency = 835 kHz. */
    PM_CLK_800_KHz      = 23,  /**< Clock frequency = 800 kHz. */
    PM_CLK_768_KHz      = 24,  /**< Clock frequency = 768 kHz. */
    PM_CLK_738_KHz      = 25,  /**< Clock frequency = 738 kHz. */
    PM_CLK_711_KHz      = 26,  /**< Clock frequency = 711 kHz. */
    PM_CLK_686_KHz      = 27,  /**< Clock frequency = 686 kHz. */
    PM_CLK_662_KHz      = 28,  /**< Clock frequency = 662 kHz. */
    PM_CLK_640_KHz      = 29,  /**< Clock frequency = 640 kHz. */
    PM_CLK_619_KHz      = 30,  /**< Clock frequency = 619 kHz. */
    PM_CLK_600_KHz      = 31,  /**< Clock frequency = 600 kHz. */
    PM_SWITCHING_FREQ_INVALID,
    PM_SWITCHING_FREQ_FREQ_NONE
}pm_smps_switching_freq_type;

/** Quiet mode. */
typedef enum
{
    PM_QUIET_MODE__DISABLE,      /**< Quiet mode disabled (default). */
    PM_QUIET_MODE__QUIET,        /**< Quiet mode enabled. */
    PM_QUIET_MODE__SUPER_QUIET,  /**< Super Quiet mode enabled. @newpage */
    PM_QUIET_MODE__INVALID
}pm_quiet_mode_type;

/*===========================================================================

                        API PROTOTYPE

===========================================================================*/

/** Switches between HPM, LPM, and  modes of a regulator.

    @param[in] pmic_chip              Primary PMIC: 0, Secondary PMIC: 1.

    @param[in] smps_peripheral_index SMPS peripheral index. Starts at 
                                     0 (for the first SMPS peripheral).
                                     See @xnameref{hdr:smpsPeripheralIndex}.

    @param[in] sw_mode                Selects the mode of a regulator.
 
    @return 
    Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_smps_sw_mode
(uint8 pmic_chip, uint8 smps_peripheral_index, pm_sw_mode_type sw_mode);

/** Returns the mode status (LPM, NPM, Auto, Bypass) of the selected
    power rail.
    
    @note1hang The regulator mode changes dynamically.
 
    @param[in] pmic_chip             Primary PMIC: 0, Secondary PMIC: 1.

    @param[in] smps_peripheral_index SMPS peripheral index. Starts at 
                                     0 (for the first SMPS peripheral).
                                     See @xnameref{hdr:smpsPeripheralIndex}.

    @param[out] sw_mode              Variable to return to the caller
                                     with the mode status. Refer to
                                     #peripheral_info_type.

    @return   
    Error flag type -- See #pm_err_flag_type.
    @newpage
 */
pm_err_flag_type pm_smps_sw_mode_status
(uint8 pmic_chip, uint8 smps_peripheral_index, pm_sw_mode_type* sw_mode);

/** Gets the register value [MODE_CTL] of an instance.
 
    @note1hang The regulator mode changes dynamically.

    @param[in] pmic_chip              Primary PMIC: 0, Secondary PMIC: 1.

    @param[in] smps_peripheral_index  SMPS peripheral index. Starts at 
                                      0 (for the first SMPS peripheral).
                                      See @xnameref{hdr:smpsPeripheralIndex}.

    @param[in] mode                   Obtains the mode_ctl register
                                      value of a regulator, i.e., NPM,
                                      Auto, etc.
 
    @return 
    Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_smps_sw_mode_status_raw
(uint8 pmic_chip, uint8 smps_peripheral_index, uint8* mode);

/** Enables or disables the SMPS rail.

    @param[in] pmic_chip             Primary PMIC: 0, Secondary PMIC: 1.

    @param[in] smps_peripheral_index SMPS peripheral index. Starts at 
                                     0 (for the first SMPS peripheral).
									 See @xnameref{hdr:smpsPeripheralIndex}.

    @param[in] on_off                Turn the SMPS on/off. See #pm_on_off_type.
 
    @return 
    Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_smps_sw_enable
(uint8 pmic_chip, uint8 smps_peripheral_index, pm_on_off_type on_off);

/** Returns the software enable status of the SMPS.

    @param[in] pmic_chip              Primary PMIC: 0, Secondary PMIC: 1.

    @param[in] smps_peripheral_index  SMPS peripheral index. Starts at 
                                      0 (for the first SMPS peripheral).
                                      See @xnameref{hdr:smpsPeripheralIndex}.

    @param[in] on_off                 Pointer to the on/off status of
                                      the regulator.
 
    @return 
    Error flag type -- See #pm_err_flag_type.

    @newpage
 */
pm_err_flag_type pm_smps_sw_enable_status
(uint8 pmic_chip, uint8 smps_peripheral_index, pm_on_off_type* on_off);

/** Returns the VREG_OK status of the SMPS.

    @param[in] pmic_chip              Primary PMIC: 0, Secondary PMIC: 1.

    @param[in] smps_peripheral_index  SMPS peripheral index. Starts at 
                                      0 (for the first SMPS peripheral).
                                      See @xnameref{hdr:smpsPeripheralIndex}.

    @param[out] on_off                Pointer to the VREG_OK status.
  
    @return 
    Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_smps_vreg_ok_status
(uint8 pmic_chip, uint8 smps_peripheral_index, boolean* on_off);

/**
 * Sets the voltage of a SMPS rail.
 
 *  @note1hang Differnt types of voltage rails (HFS, FTS, etc.) may have different
 *   programmable voltage steps. Only support the correct
 *   programmable steps. Do not round voltage values if the voltage
 *   selected is not part of the programmable steps.
 * 
 * @param[in] pmic_chip              Primary PMIC: 0, Secondary PMIC: 1.
 *
 * @param[in] smps_peripheral_index  SMPS peripheral index. Starts at 
 *                                   0 (for the first SMPS peripheral).
 *                                   See @xnameref{hdr:smpsPeripheralIndex}.
 * 
 * @param[in] volt_level Select the voltage.
 * 
 * @return 
 * Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_smps_volt_level
(uint8 pmic_chip, uint8 smps_peripheral_index, pm_volt_level_type volt_level);

/**
 * Returns the voltage level (in microvolts) of the selected power rail.
 *
 * @param[in] pmic_chip              Primary PMIC: 0, Secondary PMIC: 1.
 *
 * @param[in] smps_peripheral_index  SMPS peripheral index. Starts at 
 *                                   0 (for the first SMPS peripheral).
 *                                   See @xnameref{hdr:smpsPeripheralIndex}.
 *
 * @param[out] volt_level            Variable to return to the caller
 *                                   with the voltage level in
 *                                   microvolts (uint32).
 *
 * @return 
 * Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_smps_volt_level_status
(uint8 pmic_chip, uint8 smps_peripheral_index, pm_volt_level_type* volt_level);

/** Gets VSET for the SMPS.
 
    @param[in] pmic_chip             Primary: 0, Secondary: 1.

    @param[in] smps_peripheral_index SMPS peripheral index. Starts at 
                                     0 (for the first SMPS peripheral).
                                     See @xnameref{hdr:smpsPeripheralIndex}.

    @param[in] volt_level            Voltage level.

    @param[out] vset                 Pointer to the deduced voltage register
                                     value obtained using volt_level.

    @return 
    Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_smps_calculate_vset
(uint8 pmic_chip, uint8 smps_peripheral_index, pm_volt_level_type volt_level, uint32 *vset);

/** Selects a frequency for the SMPS. Each SMPS can operate under a
    different frequency.
 
    @param[in] pmic_chip             Primary: 0, Secondary: 1.

    @param[in] smps_peripheral_index SMPS peripheral index. Starts at 
                                      0 (for the first SMPS peripheral).
                                      See @xnameref{hdr:smpsPeripheralIndex}.

    @param[in] switching_freq         Select a frequency, e.g.,
                                      19.2 MHz, 3.2 MHz, etc. See 
                                      #pm_smps_switching_freq_type.
 
    @return 
    Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_smps_switching_freq
(uint8 pmic_chip, uint8 smps_peripheral_index, pm_smps_switching_freq_type switching_freq);

/** Returns the SMPS frequency. Each SMPS can operate under a
    different frequency.
 
    @param[in] pmic_chip             Primary: 0, Secondary: 1.

    @param[in] smps_peripheral_index SMPS peripheral index. Starts at 
                                     0 (for the first SMPS peripheral).
                                     See @xnameref{hdr:smpsPeripheralIndex}.

    @param[out] freq                 Pointer to the SMPS frequency.

    @return 
    Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_smps_switching_freq_status
(uint8 pmic_chip, uint8 smps_peripheral_index, pm_smps_switching_freq_type *freq);

/** Sets Quiet mode for SMPS.
 
    @param[in] pmic_chip             Primary: 0, Secondary: 1.

    @param[in] smps_peripheral_index SMPS peripheral index. Starts at 
                                     0 (for the first SMPS peripheral).
                                     See @xnameref{hdr:smpsPeripheralIndex}.

    @param[in] quiet_mode            Enable/disable Quiet mode and
                                     select the Quiet mode type. See
                                     #pm_quiet_mode_type.

	@param[in] voltage_mv            Set the voltage in millivolts.

	@return 
    Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_smps_quiet_mode
(uint8 pmic_chip, uint8 smps_peripheral_index, pm_quiet_mode_type quiet_mode, uint16 voltage_mv);

/**
 * Configures the Soft-start stepper setting.

    @param[in] pmic_chip    Primary: 0, Secondary: 1.

    @param[in] periph_index SMPS peripheral index. Starts at 
                            0 (for the first SMPS peripheral).

                            See @xnameref{hdr:smpsPeripheralIndex}.
    @param[in] ss_cfg       Soft-start stepper setting.

    @return 
    Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_smps_set_softstart
(
  uint8 pmic_chip,
  uint8 periph_index,
  uint8 ss_cfg
);

/** Returns the current Soft-start stepper setting.

    @param[in] pmic_chip   Primary: 0, Secondary: 1.

    @param[in] periph_index SMPS peripheral index. Starts at 
                            0 (for the first SMPS peripheral).
                            See @xnameref{hdr:smpsPeripheralIndex}.

    @param[in] ss_ptr       Pointer to the current soft-start stepper setting.

    @return 
    Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_smps_get_softstart
(
  uint8 pmic_chip,
  uint8 periph_index,
  uint8* ss_ptr
);

/** Returns the current Voltage stepper setting.

    @param[in] pmic_chip    Primary: 0, Secondary: 1.

    @param[in] periph_index SMPS peripheral index. Starts at 
                            0 (for the first SMPS peripheral).
                            See @xnameref{hdr:smpsPeripheralIndex}.

    @param[in] vstep_ptr    Pointer to the current voltage stepper setting.

    @return 
    Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_smps_get_vstep
(
  uint8 pmic_chip,
  uint8  periph_index,
  uint8* vstep_ptr
);

/**
 *  Enables/disables the Over Current Protection (OCP) for a
 *  selected SMPS. Applicable for HF bucks only.
 *
 * @param[in] pmic_chip             Primary: 0, Secondary: 1.
 *
 * @param[in] smps_peripheral_index SMPS peripheral index. Starts at 
 *                                  0 (for the first SMPS peripheral).
 *                                  See @xnameref{hdr:smpsPeripheralIndex}.
 *
 * @param[in] enable                Enable/disable OCP.

    @return 
    Error flag type -- See #pm_err_flag_type.
 */
pm_err_flag_type pm_smps_ocp_enable
(uint8 pmic_chip, uint8 smps_peripheral_index, boolean enable);	

/** @} */ /* end_addtogroup pm_smps */

#endif /* PM_SMPS__H */

