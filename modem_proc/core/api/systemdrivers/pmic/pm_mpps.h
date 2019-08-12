#ifndef PM_MPPS_H
#define PM_MPPS_H

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
 * @file pm_mpps.h PMIC MPPs related declaration.
 *
 * @brief This header file contains enums and API definitions for MPPS driver.
 */

/* ==========================================================================

                  P M    H E A D E R    F I L E

========================================================================== */


/* ==========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/pmic/pm_mpps.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/27/14   mr      Doxygen complaint PMIC Header (CR-602405)
10/21/13   rk      Support for Vunicore 8926 core compilation
10/03/13   rk      Reducing the MPP number to 4
07/05/12   hs      Updated the interface.
03/02/12   hs      Removed deprecated APIs.
05/03/11   dy      Rename digital logic level enumerations
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
07/26/10   wra     Changed documentation from @example to Examples so Deoxygen
                    can parse the file
07/11/10   jtn     Aded APIs to get MPP status, corrected pm_mpp_dlogic_inout_pup_type
                   to reflect PMIC HW capabilities
07/02/10   wra     Changed pm_mpp_which_type to int
03/15/10   fpe     CMI Merge - Made the interface common between 7x30 and SCMM
03/05/10   vk      Added API pm_mpp_config_dtest_output()
10/20/09   jtn     Move init function prototype to pm_device_init_i.h
10/19/09   vk      Removed init API
07/06/09   jtn     Added enums for PM8028 digital logic levels
01/26/09   Vk      Introduced following APIs
                   pm_config_secure_mpp_config_digital_input()
                   pm_secure_mpp_config_digital_input()
12/17/08   APU     Introduced the APIs:
                   1. pm_secure_mpp_config_i_sink ()
                   2. pm_make_secure__mpp_config_i_sink ()
08/01/08   jtn     Move all proc comm functions to pm_pcil.c
06/27/08   jtn     Merge changes from QSC1100 branch, consolidated
                   definition for max. number of MPPs
06/24/08   vk      Provided API that maintains the list of MPPs that have shunt
                   capacitors to ground. In the MPP configuration API for
                   anaolog output this list is used.
03/01/08   jtn     Corrected number of MPPs for Han
02/27/08   jtn     Added support for Kip PMIC
01/07/08   jtn     Merged from QSC6270 branch
(begin QSC6270 changes)
10/11/07   jnoblet Added MPPs for Han
(end QSC6270 changes)
11/13/07   jtn     Added pm_config_secure_mpp_config_digital_output and
                   pm_secure_mpp_config_digital_output to support RPC access
                   to MPP digital outputs
06/18/07   cng     Added an input parameter to pm_mpp_config_analog_output
                   to allow configurable analog output voltage
05/18/07   hs      Restored the third parameter in pm_mpp_config_digital_input()
09/08/06   cng     Removed API pm_mpp_config_d_test and enum pm_mpp_dtest_in_dbus_type
06/22/06   hs      Removed the third parameter in pm_mpp_config_digital_input()
06/08/06   hs      Added pm_mpp_config_atest API support
11/09/05   cng     Added PM6620 support
10/24/05   cng     Added Panoramix support; Removed MPP config type enum;
                   Corrected number of MPPs for PM6650
07/25/05   cng     PM7500 support
07/01/05   cng     Added MPP config type enum
03/15/05   cng     For the analog input function, added the functionality to
                   route the MPP input to one of the 3 analog buses
01/28/04   rmd     Added initial support for multiple PMIC models/tiers.
11/03/03   rmd     Updated the settings for the BIDIRECTIONAL the pull up
                   resistor.
09/08/03   rmd     enum "pm_mpp_aout_switch_type" was in the wrong order.
08/11/03   rmd     Created.
===========================================================================*/

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "pm_err_flags.h"

/** @addtogroup pm_mpps
@{ */

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
/**
 * @brief MPP index. Which MPP needs to be configured.
 */
typedef enum
{
  PM_MPP_1,   /**< MPP 1. */
  PM_MPP_2,   /**< MPP 2. */
  PM_MPP_3,   /**< MPP 3. */
  PM_MPP_4,   /**< MPP 4. */
  PM_MPP_5,   /**< MPP 5. */
  PM_MPP_6,   /**< MPP 6. */
  PM_MPP_7,   /**< MPP 7. */
  PM_MPP_8,   /**< MPP 8. */
  PM_MPP_9,   /**< MPP 9. */
  PM_MPP_10,  /**< MPP 10. */
  PM_MPP_11,  /**< MPP 11. */
  PM_MPP_12,  /**< MPP 12. */
  PM_MPP_13,  /**< MPP 13. */
  PM_MPP_14,  /**< MPP 14. */
  PM_MPP_15,  /**< MPP 15. */
  PM_MPP_16,  /**< MPP 16. */
  PM_MPP_INVALID,
}pm_mpp_which_type;


/**
 * @enum pm_mpp_dlogic_lvl_type
 * @brief DIGITAL LOGIC LEVEL. Configures the output logic level.
 */
typedef enum
{
  PM_MPP__DLOGIC__LVL_VIO_0 = 0,  /**< Voltage I/O level 0. */
  PM_MPP__DLOGIC__LVL_VIO_1,      /**< Voltage I/O level 1. */
  PM_MPP__DLOGIC__LVL_VIO_2,      /**< Voltage I/O level 2. */
  PM_MPP__DLOGIC__LVL_VIO_3,      /**< Voltage I/O level 3. */
  PM_MPP__DLOGIC__LVL_VIO_4,      /**< Voltage I/O level 4. */
  PM_MPP__DLOGIC__LVL_VIO_5,      /**< Voltage I/O level 5. */
  PM_MPP__DLOGIC__LVL_VIO_6,      /**< Voltage I/O level 6. */
  PM_MPP__DLOGIC__LVL_VIO_7,      /**< Voltage I/O level 7. */
  PM_MPP__DLOGIC__LVL_INVALID = 8,
} pm_mpp_dlogic_lvl_type;

/**
 * @enum pm_mpp_dlogic_in_dbus_type
 * @brief MODE = DIGITAL INPUT. Configures the output logic.
 */
typedef enum
{
  PM_MPP__DLOGIC_IN__DBUS1,  /**< DBUS 1. */
  PM_MPP__DLOGIC_IN__DBUS2,  /**< DBUS 2. */
  PM_MPP__DLOGIC_IN__DBUS3,  /**< DBUS 3. */
  PM_MPP__DLOGIC_IN__DBUS4,  /**< DBUS 4. */
  PM_MPP__DLOGIC_IN__DBUS_INVALID
}pm_mpp_dlogic_in_dbus_type;

/**
 * @enum pm_mpp_dlogic_out_ctrl_type
 * @brief MODE = DIGITAL OUT. Configures the output logic.
 *
 * @note TODO - hshen: this enum seems obsolete for BADGER
 */
typedef enum
{
  PM_MPP__DLOGIC_OUT__CTRL_LOW,    /*!< MPP OUTPUT= LOGIC LOW         */
  PM_MPP__DLOGIC_OUT__CTRL_HIGH,   /*!< MPP OUTPUT= LOGIC HIGH        */
  PM_MPP__DLOGIC_OUT__CTRL_MPP,    /*!< MPP OUTPUT= CORRESPONDING
                                                  MPP INPUT         */
  PM_MPP__DLOGIC_OUT__CTRL_NOT_MPP,/*!< MPP OUTPUT= CORRESPONDING
                                                  INVERTED MPP INPUT*/
  PM_MPP__DLOGIC_OUT__CTRL_INVALID
}pm_mpp_dlogic_out_ctrl_type;

/**
 * @enum pm_mpp_dlogic_inout_pup_type
 * @brief MODE = BIDIRECTIONAL. Configures the pull up resistor.
 */
typedef enum
{
  PM_MPP__DLOGIC_INOUT__PUP_1K,  /*!< PULL UP = 1  K Ohms*/
  PM_MPP__DLOGIC_INOUT__PUP_10K, /*!< PULL UP = 10 K Ohms*/
  PM_MPP__DLOGIC_INOUT__PUP_30K, /*!< PULL UP = 30 K Ohms*/
  PM_MPP__DLOGIC_INOUT__PUP_OPEN,  /*!< PULL UP = OPEN */
  PM_MPP__DLOGIC_INOUT__PUP_INVALID
}pm_mpp_dlogic_inout_pup_type;

/**
 * @enum pm_mpp_dlogic_out_dbus_type
 * @brief MODE = DIGITAL TEST OUTPUT.
 */
typedef enum
{
  PM_MPP__DLOGIC_OUT__DBUS1,
  PM_MPP__DLOGIC_OUT__DBUS2,
  PM_MPP__DLOGIC_OUT__DBUS3,
  PM_MPP__DLOGIC_OUT__DBUS4,
  PM_MPP__DLOGIC_OUT__DBUS_INVALID
}pm_mpp_dlogic_out_dbus_type;

/**
 * @enum pm_mpp_ain_ch_type
 * @brief The types defined below are used to configure the following MPPs modes:
 *        ANALOG INPUT.
 */
typedef enum
{
  PM_MPP__AIN__CH_AMUX5,  /**< Analog input channel AMUX5. */
  PM_MPP__AIN__CH_AMUX6,  /**< Analog input channel AMUX6. */
  PM_MPP__AIN__CH_AMUX7,  /**< Analog input channel AMUX7. */
  PM_MPP__AIN__CH_AMUX8,  /**< Analog input channel AMUX8. */
  PM_MPP__AIN__CH_ABUS1,  /**< Analog input channel ABUS1. */
  PM_MPP__AIN__CH_ABUS2,  /**< Analog input channel ABUS2. */
  PM_MPP__AIN__CH_ABUS3,  /**< Analog input channel ABUS3. */
  PM_MPP__AIN__CH_ABUS4,  /**< Analog input channel ABUS4. */
  PM_MPP__AIN__CH_INVALID
} pm_mpp_ain_ch_type;

/**
 * @enum pm_mpp_aout_level_type
 * @brief The types defined below are used to configure the following MPPs modes:
 *        ANALOG OUTPUT.
 */
typedef enum
{
  PM_MPP__AOUT__LEVEL_VREF_1p25_Volts,
    /**< Voltage reference level is 1.25 V. */
  PM_MPP__AOUT__LEVEL_VREF_0p625_Volts,
    /**< Voltage reference level is 0.625 V. */
  PM_MPP__AOUT__LEVEL_VREF_0p3125_Volts,
    /**< Voltage reference level is 0.3125 V. */
  PM_MPP__AOUT__LEVEL_INVALID
}pm_mpp_aout_level_type;

/**
 * @enum pm_mpp_aout_switch_type
 * @brief Analog OUTPUT Switch type.
 */
typedef enum
{
  PM_MPP__AOUT__SWITCH_OFF,             /**< Switch is off. */
  PM_MPP__AOUT__SWITCH_ON,              /**< Switch is on. */
  PM_MPP__AOUT__SWITCH_ON_IF_MPP_HIGH,  /**< Switch is on if MPP is high. */
  PM_MPP__AOUT__SWITCH_ON_IF_MPP_LOW,   /**< Switch is on if MPP is low. */
  PM_MPP__AOUT__SWITCH_INVALID
}pm_mpp_aout_switch_type;

/**
 * @enum pm_mpp_i_sink_level_type
 * @brief The types defined below are used to configure the following MPPs modes:
 *        CURRENT SINK.
 */
typedef enum
{
  PM_MPP__I_SINK__LEVEL_5mA,   /**< Current sink level 5 mA. */
  PM_MPP__I_SINK__LEVEL_10mA,  /**< Current sink level 10 mA. */
  PM_MPP__I_SINK__LEVEL_15mA,  /**< Current sink level 15 mA. */
  PM_MPP__I_SINK__LEVEL_20mA,  /**< Current sink level 20 mA. */
  PM_MPP__I_SINK__LEVEL_25mA,  /**< Current sink level 25 mA. */
  PM_MPP__I_SINK__LEVEL_30mA,  /**< Current sink level 30 mA. */
  PM_MPP__I_SINK__LEVEL_35mA,  /**< Current sink level 35 mA. */
  PM_MPP__I_SINK__LEVEL_40mA,  /**< Current sink level 40 mA. */
  PM_MPP__I_SINK__LEVEL_INVALID
}pm_mpp_i_sink_level_type;

/**
 * @enum pm_mpp_i_sink_switch_type
 * @brief MPP Current sink switch type.
 */
typedef enum
{
  PM_MPP__I_SINK__SWITCH_DIS,              /**< Current sink switch disabled. */
  PM_MPP__I_SINK__SWITCH_ENA,              /**< Current sink switch enabled. */
  PM_MPP__I_SINK__SWITCH_ENA_IF_MPP_HIGH,  /**< Current sink switch enabled if
                                                the MPP is high. */
  PM_MPP__I_SINK__SWITCH_ENA_IF_MPP_LOW,   /**< Current sink switch enabled if
                                                the MPP is low. */
  PM_MPP__I_SINK__SWITCH_INVALID
}pm_mpp_i_sink_switch_type;

/**
 * @enum pm_mpp_ch_atest_type
 * @brief MPP channel ATEST
 */
typedef enum
{
  PM_MPP__CH_ATEST1,  /**< A-test 1. */
  PM_MPP__CH_ATEST2,  /**< A-test 2. */
  PM_MPP__CH_ATEST3,  /**< A-test 3. */
  PM_MPP__CH_ATEST4,  /**< A-test 4. */
  PM_MPP__CH_ATEST_INVALID
}pm_mpp_ch_atest_type;

/**
 * @struct pm_mpp_digital_input_status_type
 * @brief Structure used for returning digital input configuration
 */
typedef struct
{
    pm_mpp_dlogic_lvl_type mpp_dlogic_lvl;    /*!< MPP Dlogic level */
    pm_mpp_dlogic_in_dbus_type mpp_dlogic_in_dbus;    /*!< MPP Dlogic in Dbus */
} pm_mpp_digital_input_status_type;

/**
 * @struct pm_mpp_digital_output_status_type
 * @brief Structure used for returning digital output configuration
 */
typedef struct
{
    pm_mpp_dlogic_lvl_type mpp_dlogic_lvl;    /*!< MPP Dlogic level */
    pm_mpp_dlogic_out_ctrl_type mpp_dlogic_out_ctrl;    /*!< MPP Dlogic out control */
} pm_mpp_digital_output_status_type;

/**
 * @struct pm_mpp_bidirectional_status_type
 * @brief Structure used for returning bidirectional configuration
 */
typedef struct
{
    pm_mpp_dlogic_lvl_type mpp_dlogic_lvl;    /*!< MPP Dlogic level */
    pm_mpp_dlogic_inout_pup_type mpp_dlogic_inout_pup;    /*!< MPP Dlogic in-out pull-up */
}  pm_mpp_bidirectional_status_type;

/**
 * @struct pm_mpp_analog_input_status_type
 * @brief Structure used for returning analog input configuration
 */
typedef struct
{
    pm_mpp_ain_ch_type mpp_ain_ch;    /*!< MPP analog in channel */
}  pm_mpp_analog_input_status_type;

/**
 * @struct pm_mpp_analog_output_status_type
 * @brief Structure used for returning analog output configuration
 */
typedef struct
{
    pm_mpp_aout_level_type mpp_aout_level;    /*!< MPP analog out level */
    pm_mpp_aout_switch_type mpp_aout_switch;    /*!< MPP analog out switch */
}  pm_mpp_analog_output_status_type;

/**
 * @struct pm_mpp_current_sink_status_type
 * @brief Structure used for returning current sink configuration
 */
typedef struct
{
    pm_mpp_i_sink_level_type  mpp_i_sink_level;    /*!< MPP current sink level */
    pm_mpp_i_sink_switch_type mpp_i_sink_switch;    /*!< MPP current sink switch */
}  pm_mpp_current_sink_status_type;

/**
 * @struct pm_mpp_dtest_output_status_type
 * @brief Structure used for returning DTEST output configuration
 */
typedef struct
{
    pm_mpp_dlogic_lvl_type mpp_dlogic_lvl;    /*!< MPP Dlogic level */
    pm_mpp_dlogic_out_dbus_type  mpp_dlogic_out_dbus;    /*!< MPP Dlogic out Dbus */
} pm_mpp_dtest_output_status_type;

/**
 * @enum pm_drv_mpp_config_type
 * @brief Enumeration of available MPP modes
 */
typedef enum
{
    PM_MPP_DIG_IN,    /*!< 0: Digital Input */
    PM_MPP_DIG_OUT,    /*!< 1: Digital Output */
    PM_MPP_DIG_IN_DIG_OUT,    /*!< 2: Digital Input and Digital Output */
    PM_MPP_BI_DIR,    /*!< 3: Bidirectional Logic */
    PM_MPP_ANALOG_IN,    /*!< 4: Analog Input */
    PM_MPP_ANALOG_OUT,    /*!< 5: Analog Output */
    PM_MPP_I_SINK,    /*!< 6: Current Sink */
    PM_MPP_TYPE_INVALID    /*!< 6: MPP type INVALID */
} pm_drv_mpp_config_type;

/**
 * @struct pm_mpp_status_type
 * @brief Structure type used for returning MPP status
 *        The structure that has valid data will depend on
 *        the MPP mode (input, output, etc.) returned in mpp_config.
 */
typedef struct
{
    pm_drv_mpp_config_type mpp_config;    /*!< This returns the MPP's mode (input, output, etc.) */
    union
    {
        pm_mpp_digital_input_status_type mpp_digital_input_status;
        pm_mpp_digital_output_status_type mpp_digital_output_status;
        pm_mpp_bidirectional_status_type mpp_bidirectional_status;
        pm_mpp_analog_input_status_type mpp_analog_input_status;
        pm_mpp_analog_output_status_type mpp_analog_output_status;
        pm_mpp_current_sink_status_type mpp_current_sink_status;
        pm_mpp_dtest_output_status_type mpp_dtest_output_status;
    } mpp_config_settings;    /*!< Union of structures used to return MPP status */
} pm_mpp_status_type;


/*===========================================================================

                    MPP DRIVER FUNCTION PROTOTYPES

===========================================================================*/

/**
 * @name pm_dev_mpp_status_get
 *
 * @brief Returns the status of one of the PMIC MPPs.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] mpp Which MPP
 *
 * @param [out] mpp_status Pointer to the MPP status structure
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 * @note Do not use pm_mpp_status_get(), which is deprecated. Use pm_dev_mpp_status_get() instead.
 */
pm_err_flag_type pm_dev_mpp_status_get(uint8 pmic_chip,
                                       pm_mpp_which_type mpp,
                                       pm_mpp_status_type *mpp_status);
pm_err_flag_type pm_mpp_status_get(pm_mpp_which_type mpp,
                                   pm_mpp_status_type *mpp_status);

/**
 * @name pm_dev_mpp_config_digital_input
 *
 * @brief  Configure the selected Multi Purpose pin (MPP) to be a digital input pin.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] mpp Which MPP
 * @param [in] level The logic level reference that we want to use with this MPP.
 * @param [in] dbus Which data line will the MPP drive. Please note that only
 *             one MPP can be assigned per DBUS.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter 1 is out of range.
 *          PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter 2 is out of range.
 *          PM_ERR_FLAG__PAR3_OUT_OF_RANGE    = Input Parameter 3 is out of range.
 *          PM_ERR_FLAG__DBUS_IS_BUSY_MODE    = The DBUS is busy.
 *          PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate with the PMIC.
 *
 * @warning Interrupts are disable while communicating with the PMIC.
 *
 * @note Do not use pm_mpp_config_digital_input(), which is deprecated. Use pm_dev_mpp_config_digital_input() instead.
 */
extern pm_err_flag_type
pm_dev_mpp_config_digital_input( uint8 pmic_chip,
                                 pm_mpp_which_type mpp,
                                 pm_mpp_dlogic_lvl_type level,
                                 pm_mpp_dlogic_in_dbus_type  dbus);
extern pm_err_flag_type
pm_mpp_config_digital_input( pm_mpp_which_type mpp,
                             pm_mpp_dlogic_lvl_type level,
                             pm_mpp_dlogic_in_dbus_type dbus);

/**
 * @name pm_dev_mpp_config_digital_output
 *
 * @brief Configure the selected Multi Purpose pin (MPP) to be a digital output pin.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] mpp Which MPP
 * @param [in] level The logic level reference that we want to use with this MPP.
 * @param [in] output_ctrl The output setting of the selected MPP.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter 1 is out of range.
 *          PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter 2 is out of range.
 *          PM_ERR_FLAG__PAR3_OUT_OF_RANGE    = Input Parameter 3 is out of range.
 *          PM_ERR_FLAG__DBUS_IS_BUSY_MODE    = The DBUS is busy.
 *          PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate with the PMIC.
 *
 * @warning Interrupts are disable while communicating with the PMIC.
 *
 * @note Do not use pm_mpp_config_digital_output(), which is deprecated. Use pm_dev_mpp_config_digital_output() instead.
 */
 extern pm_err_flag_type
 pm_dev_mpp_config_digital_output( uint8 pmic_chip,
                                   pm_mpp_which_type mpp,
                                   pm_mpp_dlogic_lvl_type level,
                                   pm_mpp_dlogic_out_ctrl_type output_ctrl);
extern pm_err_flag_type
pm_mpp_config_digital_output( pm_mpp_which_type mpp,
                              pm_mpp_dlogic_lvl_type level,
                              pm_mpp_dlogic_out_ctrl_type output_ctrl);

/**
 * @name pm_dev_mpp_config_digital_inout
 *
 * @brief Configure the selected Multi Purpose pin (MPP) to be a digital output pin.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] mpp Which MPP
 * @param [in] level The logic level reference that we want to use with this MPP.
 * @param [in] pull_up The pull-up resistor setting of the selected MPP.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter 1 is out of range.
 *          PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter 2 is out of range.
 *          PM_ERR_FLAG__PAR3_OUT_OF_RANGE    = Input Parameter 3 is out of range.
 *          PM_ERR_FLAG__DBUS_IS_BUSY_MODE    = The DBUS is busy.
 *          PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate with the PMIC.
 *
 * @warning Interrupts are disable while communicating with the PMIC.
 *
 * @note Do not use pm_mpp_config_digital_inout(), which is deprecated. Use pm_dev_mpp_config_digital_inout() instead.
 */
 extern pm_err_flag_type
 pm_dev_mpp_config_digital_inout(uint8 pmic_chip,
                                 pm_mpp_which_type mpp,
                                 pm_mpp_dlogic_lvl_type level,
                                 pm_mpp_dlogic_inout_pup_type pull_up);
extern pm_err_flag_type
pm_mpp_config_digital_inout(pm_mpp_which_type mpp,
                            pm_mpp_dlogic_lvl_type level,
                            pm_mpp_dlogic_inout_pup_type pull_up);

/**
 * @name pm_dev_mpp_config_dtest_output
 *
 * @brief Configure the selected Multi Purpose pin (MPP) to be a digital test output pin.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] mpp Which MPP
 * @param [in] level The logic level reference that we want to use with this MPP.
 * @param [in] dbus Which data line will the MPP drive.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter 1 is out of range.
 *          PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter 2 is out of range.
 *          PM_ERR_FLAG__PAR3_OUT_OF_RANGE    = Input Parameter 3 is out of range.
 *          PM_ERR_FLAG__DBUS_IS_BUSY_MODE    = The DBUS is busy.
 *          PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate with the PMIC.
 *
 * @warning Interrupts are disable while communicating with the PMIC.
 *
 * @note Do not use pm_mpp_config_dtest_output(), which is deprecated. Use pm_dev_mpp_config_dtest_output() instead.
 */
pm_err_flag_type
pm_dev_mpp_config_dtest_output(uint8 pmic_chip,
                               pm_mpp_which_type mpp,
                               pm_mpp_dlogic_lvl_type level,
                               pm_mpp_dlogic_out_dbus_type  dbus);
pm_err_flag_type
pm_mpp_config_dtest_output(pm_mpp_which_type mpp,
                           pm_mpp_dlogic_lvl_type level,
                           pm_mpp_dlogic_out_dbus_type  dbus);

/**
 * @name pm_dev_mpp_config_analog_input
 *
 * @brief This function configures the selected MPP as an Analog Input.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] mpp Which MPP
 * @param [in] ch_select Which analog mux or analog bus will the selected MPP be routed to.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter 1 is out of range.
 *          PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter 2 is out of range.
 *          PM_ERR_FLAG__DBUS_IS_BUSY_MODE    = The DBUS is busy.
 *          PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate with the PMIC.
 *
 * @warning Interrupts are disable while communicating with the PMIC.
 *
 * @note Do not use pm_mpp_config_analog_input(), which is deprecated. Use pm_dev_mpp_config_analog_input() instead.
 */
extern pm_err_flag_type
pm_dev_mpp_config_analog_input(uint8 pmic_chip,
                               pm_mpp_which_type mpp,
                               pm_mpp_ain_ch_type ch_select);
extern pm_err_flag_type
pm_mpp_config_analog_input(pm_mpp_which_type mpp,
                           pm_mpp_ain_ch_type ch_select);

/**
 * @name pm_dev_mpp_config_analog_output
 *
 * @brief This function configures the selected MPP as an analog output.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] mpp Which MPP
 * @param [in] level The logic level reference that we want to use with this MPP.
 * @param [in] OnOff This option allows the user to enable/disable the MPP output.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter 1 is out of range.
 *          PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter 2 is out of range.
 *          PM_ERR_FLAG__DBUS_IS_BUSY_MODE    = The DBUS is busy.
 *          PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate with the PMIC.
 *
 * @warning Interrupts are disable while communicating with the PMIC.
 *
 * @note Do not use pm_mpp_config_analog_output(), which is deprecated. Use pm_dev_mpp_config_analog_output() instead.
 */
extern pm_err_flag_type
pm_dev_mpp_config_analog_output(uint8 pmic_chip,
                                pm_mpp_which_type        mpp,
                                pm_mpp_aout_level_type   level,
                                pm_mpp_aout_switch_type  OnOff);
extern pm_err_flag_type
pm_mpp_config_analog_output(pm_mpp_which_type        mpp,
                            pm_mpp_aout_level_type   level,
                            pm_mpp_aout_switch_type  OnOff);

/**
 * @name pm_dev_mpp_config_i_sink
 *
 * @brief This function configures the selected MPP as an analog output.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] mpp Which MPP
 * @param [in] level The logic level reference that we want to use with this MPP.
 * @param [in] OnOff This option allows the user to enable/disable current sink.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter 1 is out of range.
 *          PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter 2 is out of range.
 *          PM_ERR_FLAG__DBUS_IS_BUSY_MODE    = The DBUS is busy.
 *          PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate with the PMIC.
 *
 * @warning Interrupts are disable while communicating with the PMIC.
 *
 * @note Do not use pm_mpp_config_i_sink(), which is deprecated. Use pm_dev_mpp_config_i_sink() instead.
 */
extern pm_err_flag_type
pm_dev_mpp_config_i_sink(uint8 pmic_chip,
                         pm_mpp_which_type         mpp,
                         pm_mpp_i_sink_level_type  level,
                         pm_mpp_i_sink_switch_type OnOff);
extern pm_err_flag_type
pm_mpp_config_i_sink(pm_mpp_which_type mpp,
                     pm_mpp_i_sink_level_type  level,
                     pm_mpp_i_sink_switch_type OnOff);

/**
 * @name pm_dev_mpp_config_atest
 *
 * @brief This function configures the selected MPP as ATEST.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] mpp Which MPP
 * @param [in] atest_select Which atest will the selected MPP be routed to.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter 1 is out of range.
 *          PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter 2 is out of range.
 *          PM_ERR_FLAG__DBUS_IS_BUSY_MODE    = The DBUS is busy.
 *          PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate with the PMIC.
 *
 * @warning Interrupts are disable while communicating with the PMIC.
 *
 * @note Do not use pm_mpp_config_atest(), which is deprecated. Use pm_dev_mpp_config_atest() instead.
 */
extern pm_err_flag_type
pm_dev_mpp_config_atest(uint8 pmic_chip,
                        pm_mpp_which_type mpp,
                        pm_mpp_ch_atest_type atest_select);
extern pm_err_flag_type
pm_mpp_config_atest(pm_mpp_which_type mpp,
                    pm_mpp_ch_atest_type atest_select);

/**
 * @name pm_dev_mpp_set_list_mpp_with_shunt_cap
 *
 * @brief This function can be used to create an array of MPPs that have shunt
 *        capacitors to ground.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] mpp Which MPP
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter 1 is out of range.
 *          PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter 2 is out of range.
 *          PM_ERR_FLAG__DBUS_IS_BUSY_MODE    = The DBUS is busy.
 *          PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate with the PMIC.
 *
 * @warning Interrupts are disable while communicating with the PMIC.
 *
 * @note Do not use pm_mpp_set_list_mpp_with_shunt_cap(), which is deprecated. Use pm_dev_mpp_set_list_mpp_with_shunt_cap() instead.
 */
pm_err_flag_type
pm_dev_mpp_set_list_mpp_with_shunt_cap(uint8 pmic_chip,
                                       pm_mpp_which_type mpp);
pm_err_flag_type pm_mpp_set_list_mpp_with_shunt_cap(pm_mpp_which_type mpp);

/**
 * @name pm_dev_get_mpp_with_shunt_cap_list_status_for_device
 *
 * @brief This function returns the 32bit status which represents that the
 *        corresponding mpp has shunt capacitor to ground.
 *
 * @param [in] pmic_chip Primary PMIC: 0 Secondary PMIC: 1
 * @param [in] mpp Which MPP
 *
 * @param [out] shuntList Pointer to the Shunt list structure.
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter 1 is out of range.
 *          PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter 2 is out of range.
 *          PM_ERR_FLAG__DBUS_IS_BUSY_MODE    = The DBUS is busy.
 *          PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate with the PMIC.
 *
 * @warning Interrupts are disable while communicating with the PMIC.
 *
 * @note Do not use pm_get_mpp_with_shunt_cap_list_status_for_device(),
 *       which is deprecated. Use pm_dev_get_mpp_with_shunt_cap_list_status_for_device() instead.
 */
pm_err_flag_type
pm_dev_get_mpp_with_shunt_cap_list_status_for_device(uint8 pmic_chip,
                                                     pm_mpp_which_type mpp,
                                                     uint32* shuntList);
pm_err_flag_type
pm_get_mpp_with_shunt_cap_list_status_for_device(pm_mpp_which_type mpp,
                                                 uint32* shuntList);

/**
 * @name pm_mpp_enable
 *
 * @brief Enable/disable MPP.
 *
 * @param[in] pmic_chip Select the device index. Device index starts with zero.
 * @param[in] perph_index MPP to configure. See #pm_mpp_which_type.
 * @param[in] enable TRUE: turn on MPP, FALSE: turn off MPP.
 *
 * @return
 *   SUCCESS or Error -- See #pm_err_flag_type.
 *
 * @dependencies
 *   The following functions must have been called: \n
 *   pm_init()
 *
 * @sideeffects
 *   Interrupts are disabled while communicating with the PMIC.
 */
pm_err_flag_type pm_mpp_enable(uint8 pmic_chip,
                                pm_mpp_which_type perph_index,
                                boolean enable);
								
pm_err_flag_type pm_mpp_get_mpp_with_shunt_cap_list_status_for_device(pm_mpp_which_type perph_index, uint32* shuntList);

/** @} */ /* end_addtogroup pm_mpps */

#endif     /* PM_MPPS_H */
