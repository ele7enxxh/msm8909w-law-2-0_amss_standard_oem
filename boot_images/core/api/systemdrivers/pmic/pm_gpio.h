#ifndef PM_GPIO__H
#define PM_GPIO__H

/*! \file  pm_gpio.h
*  \n
 *  \brief  PMIC-GPIO MODULE RELATED DECLARATION
 *  \details  This file contains functions and variable declarations to support 
*   the PMIC GPIO module.
*
*
*  \n Copyright 2010-2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/systemdrivers/pmic/pm_gpio.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/31/14   akm     Removed unused APIs
03/20/13   aab     Adding IRQ related APIs 
12/06/12   hw      Remove comdef.h and use com_dtypes.h
07/05/12   hs      Updated the interface.
03/01/12   hs	   Updated the interface.
04/26/11   wra     Adding more GPIO enumeration needed from PM8921
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
                    Added struct pm_gpio_config_type
                    Added pm_gpio_config() and pm_gpio_set_interrupt_polarity()
08/11/10   wra     Removed doxygen quotations. They are causing parsing errors
07/26/10   wra     Changed documentation from @example to Examples so Deoxygen can parse the file
07/09/10   jtn     Added API to get GPIO status
07/02/10   wra     Changed pm_gpio_perph_index to int
06/23/10   vk      Added pm_gpio_set_mux_ctrl()
03/15/10   fpe     Removed RPC remoting because the application processor can do this directly
10/20/09   jtn     Move init function prototype to pm_device_init_i.h
10/19/09   vk      Removed init API
08/01/09   vk      Modified gpio_digital_input prototype
05/20/09   vk      New file
===========================================================================*/
/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

/*! \enum pm_gpio_perph_index
 *  \brief Type definition for different GPIOs
*/
typedef enum
{
    PM_GPIO_1,
    PM_GPIO_2,
    PM_GPIO_3,
    PM_GPIO_4,
    PM_GPIO_5,
    PM_GPIO_6,
    PM_GPIO_7,
    PM_GPIO_8,
    PM_GPIO_9,
    PM_GPIO_10,
    PM_GPIO_11,
    PM_GPIO_12,
    PM_GPIO_13,
    PM_GPIO_14,
    PM_GPIO_15,
    PM_GPIO_16,
    PM_GPIO_17,
    PM_GPIO_18,
    PM_GPIO_19,
    PM_GPIO_20,
    PM_GPIO_21,
    PM_GPIO_22,
    PM_GPIO_23,
    PM_GPIO_24,
    PM_GPIO_25,
    PM_GPIO_26,
    PM_GPIO_27,
    PM_GPIO_28,
    PM_GPIO_29,
    PM_GPIO_30,
    PM_GPIO_31,
    PM_GPIO_32,
    PM_GPIO_33,
    PM_GPIO_34,
    PM_GPIO_35,
    PM_GPIO_36,
    PM_GPIO_37,
    PM_GPIO_38,
    PM_GPIO_39,
    PM_GPIO_40,
    PM_GPIO_41,
    PM_GPIO_42,
    PM_GPIO_43,
    PM_GPIO_44  
}pm_gpio_perph_index;

/*! \enum pm_gpio_volt_src_type
 *  \brief Select voltage source
*/
typedef enum
{
    PM_GPIO_VIN0,
    PM_GPIO_VIN1,
    PM_GPIO_VIN2,
    PM_GPIO_VIN3,
    PM_GPIO_VIN4,
    PM_GPIO_VIN5,
    PM_GPIO_VIN6,
    PM_GPIO_VIN7,
    PM_GPIO_VOLTAGE_SOURCE_TYPE__INVALID
}pm_gpio_volt_src_type;


/*! \enum pm_gpio_invert_ext_pin_type
 *  \brief Invert/Not Invert the output of ext pin
 */
typedef enum
{
  PM_GPIO_INVERT_EXT_PIN_OUTPUT_DISABLE,
  PM_GPIO_INVERT_EXT_PIN_OUTPUT_ENABLE,  
  PM_GPIO_INVERT_EXT_PIN_OUTPUT_INVALID  
}pm_gpio_invert_ext_pin_type;

/*! \enum pm_gpio_out_buffer_config_type
 *  \brief select Output buffer configuration
*/
typedef enum
{
    PM_GPIO_OUT_BUFFER_CONFIG_CMOS,
    PM_GPIO_OUT_BUFFER_CONFIG_OPEN_DRAIN,
    PM_GPIO_OUT_BUFFER_CONFIG__INVALID
}pm_gpio_out_buffer_config_type;

/*! \enum pm_gpio_out_buffer_drv_strength_type
 *  \brief select output buffer strength
*/
typedef enum
{
    PM_GPIO_OUT_BUFFER_OFF,
    PM_GPIO_OUT_BUFFER_LOW,
    PM_GPIO_OUT_BUFFER_MEDIUM,
    PM_GPIO_OUT_BUFFER_HIGH,
    PM_GPIO_OUT_BUFFER_DRIVE_STRENGTH__INVALID
}pm_gpio_out_buffer_drv_strength_type;

/*! \enum pm_gpio_current_src_pulls_type
 *  \brief select current source pulls type
*/
typedef enum
{
    PM_GPIO_I_SOURCE_PULL_UP_30uA,
    PM_GPIO_I_SOURCE_PULL_UP_1_5uA,
    PM_GPIO_I_SOURCE_PULL_UP_31_5uA,
    PM_GPIO_I_SOURCE_PULL_UP_1_5uA_PLUS_30uA_BOOST,
    PM_GPIO_I_SOURCE_PULL_DOWN_10uA,
    PM_GPIO_I_SOURCE_PULL_NO_PULL,
    PM_GPIO_CURRENT_SOURCE_PULLS_TYPE__INVALID
}pm_gpio_current_src_pulls_type;


/*! \enum pm_gpio_src_config_type
 *  \brief Source select
*/
typedef enum
{
    PM_GPIO_SOURCE_GND,
    PM_GPIO_SOURCE_PAIRED_GPIO,
    PM_GPIO_SOURCE_SPECIAL_FUNCTION1,
    PM_GPIO_SOURCE_SPECIAL_FUNCTION2,
    PM_GPIO_SOURCE_DTEST1,
    PM_GPIO_SOURCE_DTEST2,
    PM_GPIO_SOURCE_DTEST3,
    PM_GPIO_SOURCE_DTEST4,
    PM_GPIO_SOURCE_CONFIG_TYPE__INVALID
}pm_gpio_src_config_type;



/*! \enum pm_gpio_ext_pin_config_type
 *  \brief Ext_Pin configuration
*/
typedef enum
{
    PM_GPIO_EXT_PIN_ENABLE,
    PM_GPIO_EXT_PIN_DISABLE,  /*! Puts EXT_PIN at high Z state & disables the block */
    PM_GPIO_EXT_PIN_CONFIG_TYPE__INVALID    
}pm_gpio_ext_pin_config_type;

/*! \enum pm_gpio_uart_path_type
 *  \brief UART MUX configuration
*/
typedef enum
{
    PM_UART_MUX_NO,
    PM_UART_MUX_1,
    PM_UART_MUX_2,
    PM_UART_MUX_3,
    PM_UART_MUX_INVALID
}pm_gpio_uart_path_type;

/**
 * Enumeration for GPIO output modes
 */
typedef enum
{
    /**
     * GPIO is configured as input
     */
    PM_GPIO_INPUT_ON,
    /**
     * GPIO is configured as input and output
     */
    PM_GPIO_INPUT_OUTPUT_ON,
    /**
     * GPIO is configured as output
     */
    PM_GPIO_OUTPUT_ON,
    /**
     * Both input and output are off
     */
    PM_GPIO_INPUT_OUTPUT_OFF
} pm_gpio_mode_select_type;


/**
 * Structure used to return GPIO status.  This structure includes all of the enums that are used
 * when configuring the GPIOs
*/
typedef struct 
{
    pm_gpio_mode_select_type  gpio_mode_select;
    pm_gpio_volt_src_type gpio_volt_source;
    pm_on_off_type gpio_mode_on_off;
    pm_gpio_out_buffer_config_type gpio_out_buffer_config;
    pm_gpio_invert_ext_pin_type gpio_invert_ext_pin_config;
    pm_gpio_out_buffer_drv_strength_type gpio_out_buffer_drv_strength;
    pm_gpio_current_src_pulls_type gpio_current_src_pulls;
    pm_gpio_src_config_type gpio_src_config;
    pm_on_off_type gpio_dtest_buffer_on_off;
    pm_gpio_ext_pin_config_type gpio_ext_pin_config;
} pm_gpio_status_type;

/**
 * Structure to be used in conjunction with pm_gpio_config().  
 * This structure includes all fields that are available when configuring the GPIOs
*/
typedef struct 
{
    pm_gpio_perph_index gpio;                                     // which GPIO to configure
    boolean config_gpio;                                         // whether GPIO should be configured
    pm_gpio_volt_src_type gpio_volt_source;             // which voltage source
    boolean  gpio_mode_on_off;                                   // enables/disables mode selection
    pm_gpio_mode_select_type  gpio_mode_select;                  // mode select type
    pm_gpio_out_buffer_config_type gpio_out_buffer_config;       // output buffur config
    boolean invert_ext_pin;                                      // invert ext_pin
    pm_gpio_current_src_pulls_type gpio_current_src_pulls; // current source pull config
    pm_gpio_out_buffer_drv_strength_type gpio_out_buffer_drv_strength; // output buffer drive strength
    pm_on_off_type gpio_dtest_buffer_on_off;     // dtest buffer config
    pm_gpio_ext_pin_config_type gpio_ext_pin_config;             // ext pin config
    pm_gpio_src_config_type gpio_src_config;               // gpio source config
    boolean interrup_polarity;                                   // interrupt polarity
} pm_gpio_config_type;



/**
 * @brief  Return the status of one of the PMIC GPIOs
 * 
 * @param gpio  The GPIO identifier
 * @param gpio_status Pointer to the GPIO status
 * 
 * @return pm_err_flag_type
*/
pm_err_flag_type pm_gpio_status_get
(
 unsigned pmic_chip, 
 pm_gpio_perph_index gpio, 
 pm_gpio_status_type *gpio_status
);

/*===========================================================================

                        API PROTOTYPE

===========================================================================*/


/*=========================================================================== */
/*                        pm_gpio_config_bias_voltage                         */
/*=========================================================================== */

/*! \details Configure selected GPIO for bias voltage.
 *   <table border="0">
 *      <tr><td><b>Input Parameter</b></td>
 *          <td><b>Type</b></td>
 *          <td><b>Description</b></td>
 *      </tr>
 *     <tr><td>  pmic_chip </td>
 *          <td>pm_gpio_uart_path_type</td>
 *          <td>Each PMIC device in the systems is enumerated starting with zero.</td>
 *     <tr><td> gpio </td>
 *          <td>pm_gpio_perph_index</td>
 *          <td>GPIO to configure for bias voltage</td>
 *      </tr>
 *     <tr><td> volt_src </td>
 *          <td>pm_gpio_volt_src_type</td>
 *          <td>GPIO Voltage source</td>
 *      </tr>
 *  </table>
 *  \return pm_err_flag_type
 *  \n\n
 *  <b>Example </b>
 *  \n "Configure GPIO5 to logic high referencing to an analog (quiet) supply"
 *  \n errFlag = pm_gpio_config_bias_voltage(PM_GPIO_5, 
 *  \n &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; PM_GPIO_VIN4);
 * 
*/
extern pm_err_flag_type pm_gpio_config_bias_voltage
(
 unsigned                     pmic_chip, 
 pm_gpio_perph_index          perph_index,
 pm_gpio_volt_src_type        volt_src
);

/*=========================================================================== */
/*                        pm_gpio_config_digital_input                        */
/*=========================================================================== */

/*! \details Configure selected GPIO as a digital input.
 *   <table border="0">
 *      <tr><td><b>Input Parameter</b></td>
 *          <td><b>Type</b></td>
 *          <td><b>Description</b></td>
 *      </tr>
 *     <tr><td> pmic_chip </td>
 *          <td>pm_gpio_uart_path_type</td>
 *          <td>Each PMIC device in the systems is enumerated starting with zero.</td>
 *     <tr><td> gpio </td>
 *          <td>pm_gpio_perph_index</td>
 *          <td>GPIO to configure as digital input</td>
 *      </tr>
 *     <tr><td> i_src_pulls </td>
 *          <td>pm_gpio_current_src_pulls_type</td>
 *          <td>current source pulls</td>
 *      </tr>
 *     <tr><td> mode </td>
 *          <td>pm_gpio_mode_type</td>
 *          <td>GPIO Mode type</td>
 *      </tr>
 *     <tr><td> volt_src </td>
 *          <td>pm_gpio_volt_src_type</td>
 *          <td>GPIO Voltage source</td>
 *      </tr>
 *     <tr><td> out_buffer_strength </td>
 *          <td>pm_gpio_out_buffer_drv_strength_type</td>
 *          <td>GPIO output buffer strength</td>
 *      </tr>
 *     <tr><td> src </td>
 *          <td>pm_gpio_src_config_type</td>
 *          <td>Select Source</td>
 *      </tr>
 *  </table>
 *  \return pm_err_flag_type
 *  \n\n
 *  <b>Example </b>
 *  \n Keypad scan module require GPIO to drive and sense the keypad.
 *  \n Keypad drive signal should be configured as open_drain output with low drive strength
 *  \n Keypad sense module should be configured as input with 1.5uA pull up +30uA boost.
 *  \n e.g Reference voltage VIN2 for both drive and sense lines.
 *  \n "Configure GPIO5 as a sense line"
 *  \n errFlag = pm_gpio_config_digital_input(PM_GPIO_5, 
 *  \n &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; PM_GPIO_I_SOURCE_PULL_UP_1_5uA_PLUS_30uA_BOOST,
 *  \n &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; PM_GPIO_VIN2,
 *  \n &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; PM_GPIO_OUT_BUFFER_OFF,
 *  \n &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; PM_GPIO_SOURCE_GND);
 * 
*/
extern pm_err_flag_type pm_gpio_config_digital_input
(
 unsigned                              pmic_chip, 
 pm_gpio_perph_index                   perph_index,
 pm_gpio_current_src_pulls_type        i_src_pulls,
 pm_gpio_volt_src_type                 volt_src,
 pm_gpio_out_buffer_drv_strength_type  out_buffer_strength,
 pm_gpio_src_config_type               src
);

/*======================================================================= */
/*                        pm_gpio_config_digital_output                   */
/*======================================================================= */
/*! \details Configure selected GPIO as digital output.
 *   <table border="0">
 *      <tr><td><b>Input Parameter</b></td>
 *          <td><b>Type</b></td>
 *          <td><b>Description</b></td>
 *      </tr>
 *     <tr><td> pmic_chip </td>
 *          <td>pm_gpio_uart_path_type</td>
 *          <td>Each PMIC device in the systems is enumerated starting with zero.</td>
 *     <tr><td> gpio </td>
 *          <td>pm_gpio_perph_index</td>
 *          <td>GPIO to configure as digital output</td>
 *      </tr>
 *     <tr><td> out_buffer_config </td>
 *          <td>pm_gpio_out_buffer_config_type</td>
 *          <td>GPIO output buffer configuration cmos/open_drain</td>
 *      </tr>
 *     <tr><td> volt_src </td>
 *          <td>pm_gpio_volt_src_type</td>
 *          <td>GPIO Voltage source</td>
 *      </tr>
 *     <tr><td> src </td>
 *          <td>pm_gpio_src_config_type</td>
 *          <td>Select Source</td>
 *      </tr>
 *     <tr><td> out_buffer_strength </td>
 *          <td>pm_gpio_out_buffer_drv_strength_type</td>
 *          <td>GPIO output buffer strength</td>
 *      </tr>
 *     <tr><td> out_inversion </td>
 *          <td>boolean</td>
 *          <td>Invert the output of Ext_Pin</td>
 *      </tr>
 *  </table>
 *  \return pm_err_flag_type
 *  \n\n
 *  <b>Example </b>
 *  \n Keypad scan module require GPIO to drive and sense the keypad.
 *  \n Keypad drive signal should be configured as open_drain output with low drive strength
 *  \n Keypad sense module should be configured as input with 1.5uA pull up +30uA boost.
 *  \n e.g Reference voltage VIN2 for both drive and sense lines.
 *  \n "Configure GPIO5 to as drive signal"
 *  \n errFlag = pm_gpio_config_digital_output(PM_GPIO_5, 
 *  \n &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; PM_GPIO_OUT_BUFFER_CONFIG_OPEN_DRAIN,
 *  \n &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; PM_GPIO_VIN2,
 *  \n &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; PM_GPIO_SOURCE_SPECIAL_FUNCTION1,
 *  \n &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; PM_GPIO_OUT_BUFFER_LOW,
 *  \n &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; FALSE);
 * 
*/
extern pm_err_flag_type pm_gpio_config_digital_output
(
 unsigned                             pmic_chip, 
 pm_gpio_perph_index                  perph_index,
 pm_gpio_out_buffer_config_type       out_buffer_config,
 pm_gpio_volt_src_type                volt_src,
 pm_gpio_src_config_type              src,
 pm_gpio_out_buffer_drv_strength_type out_buffer_strength,
 boolean                              out_inversion
);

extern pm_err_flag_type pm_gpio_config_digital_input_output
(
 unsigned                             pmic_chip, 
 pm_gpio_perph_index                  perph_index,
 pm_gpio_src_config_type              src,
 pm_gpio_current_src_pulls_type       i_src_pulls,
 pm_gpio_volt_src_type                volt_src,
 pm_gpio_out_buffer_config_type       out_buffer_config,
 pm_gpio_out_buffer_drv_strength_type out_buffer_strength
);

/*=========================================================================== */
/*                        pm_gpio_set_volt_source                          */
/*=========================================================================== */

/*! \details Set voltage source
 *   <table border="0">
 *      <tr><td><b>Input Parameter</b></td>
 *          <td><b>Type</b></td>
 *          <td><b>Description</b></td>
 *      </tr>
 *     <tr><td> pmic_chip </td>
 *          <td>pm_gpio_uart_path_type</td>
 *          <td>Each PMIC device in the systems is enumerated starting with zero.</td>
 *     <tr><td> gpio </td>
 *          <td>pm_gpio_perph_index</td>
 *          <td>GPIO to configure voltage source</td>
 *      </tr>
 *      <tr><td> volt_src </td> 
 *          <td>pm_gpio_volt_src_type</td>
 *          <td>GPIO Voltage source</td>
 *      </tr>
 *  </table>
 *  \return pm_err_flag_type
 *  \n\n
 *  <b>Example </b>
 *  \n "Set voltage source to VIN2 for GPIO5"
 *  \n errFlag = pm_gpio_set_volt_source(PM_GPIO_5, 
 *  \n &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; PM_GPIO_VIN2);
 * 
*/
extern pm_err_flag_type pm_gpio_set_volt_source
(
 unsigned                     pmic_chip, 
 pm_gpio_perph_index          perph_index,
 pm_gpio_volt_src_type        volt_src
);

/*=========================================================================== */
/*                        pm_gpio_config_mode_selection()               */
/*=========================================================================== */
/*! \details Enable/disable mode selection
 *   <table border="0">
 *      <tr><td><b>Input Parameter</b></td>
 *          <td><b>Type</b></td>
 *          <td><b>Description</b></td>
 *      </tr>
 *     <tr><td> gpio </td>
 *          <td>pm_gpio_perph_index</td>
 *          <td>Which GPIO </td>
 *      </tr>
 *     <tr><td> enable_disable </td>
 *          <td>pm_on_off_type</td>
 *          <td>True/False</td>
 *      </tr>
 *  </table>
 *  \return pm_err_flag_type
 *  \n\n
 *  <b>Example </b>
 *  \n "For  disable mode selection" 
 *  \n errFlag = pm_gpio_config_mode_selection(PM_GPIO_1, FALSE);
 * 
 */
extern pm_err_flag_type pm_gpio_config_mode_selection
(
    unsigned               pmic_chip, 
    pm_gpio_perph_index    perph_index,
    pm_on_off_type         enable_disable
);

/*=========================================================================== */
/*                        pm_gpio_set_output_buffer_configuration            */
/*=========================================================================== */

/*! \details Set output buffer configuration
 *   <table border="0">
 *      <tr><td><b>Input Parameter</b></td>
 *          <td><b>Type</b></td>
 *          <td><b>Description</b></td>
 *      </tr>
 *     <tr><td> pmic_chip </td>
 *          <td>pm_gpio_uart_path_type</td>
 *          <td>Each PMIC device in the systems is enumerated starting with zero.</td>
 *     <tr><td> gpio </td>
 *          <td>pm_gpio_perph_index</td>
 *          <td>GPIO to configure current source pulls</td>
 *      </tr>
 *     <tr><td> out_buffer_config </td>
 *          <td>pm_gpio_out_buffer_config_type</td>
 *          <td>GPIO output buffer configuration: 0:CMOS/1: open drain</td>
 *      </tr>
 *  </table>
 *  \return pm_err_flag_type
 *  \n\n
 *  <b>Example </b>
 *  \n "Set output buffer configuration to CMOS in bias mode for GPIO5"
 *  \n errFlag = pm_gpio_set_output_buffer_configuration(PM_GPIO_5, 
 *  \n &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; PM_GPIO_OUT_BUFFER_CONFIG_CMOS);
 * 
*/
extern pm_err_flag_type pm_gpio_set_output_buffer_configuration
(
 unsigned                                pmic_chip, 
 pm_gpio_perph_index                     perph_index,
 pm_gpio_out_buffer_config_type          out_buffer_config
);

/*=========================================================================== */
/*                        pm_gpio_set_inversion_configuration()               */
/*=========================================================================== */
/*! \details Set inversion configuration
 *   <table border="0">
 *      <tr><td><b>Input Parameter</b></td>
 *          <td><b>Type</b></td>
 *          <td><b>Description</b></td>
 *      </tr>
 *     <tr><td> pmic_chip </td>
 *          <td>pm_gpio_uart_path_type</td>
 *          <td>Each PMIC device in the systems is enumerated starting with zero.</td>
 *     <tr><td> gpio </td>
 *          <td>pm_gpio_perph_index</td>
 *          <td>which GPIO </td>
 *      </tr>
 *     <tr><td> inversion </td>
 *          <td>boolean</td>
 *          <td>True/False</td>
 *      </tr>
 *  </table>
 *  \return pm_err_flag_type
 *  \n\n
 *  <b>Example </b>
 *  \n "For  GPIO5 set inversion to true" 
 *  \n errFlag = pm_gpio_set_inversion_configuration(PM_GPIO_5, TRUE);
 * 
*/
extern pm_err_flag_type pm_gpio_set_inversion_configuration
(
 unsigned                  pmic_chip, 
 pm_gpio_perph_index       perph_index,
 boolean                   inversion
);
                                    
/*=========================================================================== */
/*                        pm_gpio_set_current_src_pulls                    */
/*=========================================================================== */

/*! \details Set current source pulls
 *   <table border="0">
 *      <tr><td><b>Input Parameter</b></td>
 *          <td><b>Type</b></td>
 *          <td><b>Description</b></td>
 *      </tr>
 *     <tr><td> pmic_chip </td>
 *          <td>pm_gpio_uart_path_type</td>
 *          <td>Each PMIC device in the systems is enumerated starting with zero.</td>
 *     <tr><td> gpio </td>
 *          <td>pm_gpio_perph_index</td>
 *          <td>GPIO to configure current source pulls</td>
 *      </tr>
 *     <tr><td> i_src_pulls </td>
 *          <td>pm_gpio_current_src_pulls_type</td>
 *          <td>current source pulls</td>
 *      </tr>
 *  </table>
 *  \return pm_err_flag_type
 *  \n\n
 *  <b>Example </b>
 *  \n "Set current source pulls to 1_5uA PLUS 30uA_BOOST for GPIO5"
 *  \n errFlag = pm_gpio_set_current_src_pulls(PM_GPIO_5, 
 *  \n &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; PM_GPIO_I_src_PULL_UP_1_5uA_PLUS_30uA_BOOST);
 * 
*/
extern pm_err_flag_type pm_gpio_set_current_source_pulls
(
 unsigned                          pmic_chip, 
 pm_gpio_perph_index               perph_index,
 pm_gpio_current_src_pulls_type    i_src_pulls
);

/*=========================================================================== */
/*                        pm_gpio_set_ext_pin_config()               */
/*=========================================================================== */
/*! \details Set EXT_PIN configuration
 *   <table border="0">
 *      <tr><td><b>Input Parameter</b></td>
 *          <td><b>Type</b></td>
 *          <td><b>Description</b></td>
 *      </tr>
 *     <tr><td>pmic_chip </td>
 *          <td>pm_gpio_uart_path_type</td>
 *          <td>Each PMIC device in the systems is enumerated starting with zero.</td>
 *     <tr><td> gpio </td>
 *          <td>pm_gpio_perph_index</td>
 *          <td>GPIO to configure current source pulls</td>
 *      </tr>
 *     <tr><td> ext_pin_config </td>
 *          <td>pm_gpio_ext_pin_config_type</td>
 *          <td>ENABLE/DISABLE(Puts EXT_PIN at high Z state & disables the block)</td>
 *      </tr>
 *  </table>
 *  \return pm_err_flag_type
 *  \n\n
 *  <b>Example </b>
 *  \n To ENABLE EXT_PIN
 *  \n errFlag = pm_gpio_set_ext_pin_config(PM_GPIO_5, PM_GPIO_EXT_PIN_ENABLE);
 * 
*/
extern pm_err_flag_type pm_gpio_set_ext_pin_config
(
 unsigned                       pmic_chip, 
 pm_gpio_perph_index            gpio,
 pm_gpio_ext_pin_config_type    ext_pin_config
);


/*=========================================================================== */
/*                        pm_gpio_set_output_buffer_drive_strength            */
/*=========================================================================== */
/*! \details Set output buffer drive strength
 *   <table border="0">
 *      <tr><td><b>Input Parameter</b></td>
 *          <td><b>Type</b></td>
 *          <td><b>Description</b></td>
 *      </tr>
 *     <tr><td>pmic_chip </td>
 *          <td>pm_gpio_uart_path_type</td>
 *          <td>Each PMIC device in the systems is enumerated starting with zero.</td>
 *     <tr><td> gpio </td>
 *          <td>pm_gpio_perph_index</td>
 *          <td>GPIO to configure current source pulls</td>
 *      </tr>
 *     <tr><td> out_buffer_strength </td>
 *          <td>pm_gpio_out_buffer_drv_strength_type</td>
 *          <td>GPIO output buffer drive strength</td>
 *      </tr>
 *  </table>
 *  \return pm_err_flag_type
 *  \n\n
 *  <b>Example </b>
 *  \n "Set output buffer drive strength for GPIO5 to HIGH"
 *  \n errFlag = pm_gpio_set_output_buffer_drv_strength(PM_GPIO_5, 
 *  \n &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; PM_GPIO_OUT_BUFFER_HIGH);
 * 
*/
extern pm_err_flag_type pm_gpio_set_output_buffer_drive_strength
(
 unsigned                              pmic_chip, 
 pm_gpio_perph_index                   perph_index,
 pm_gpio_out_buffer_drv_strength_type  out_buffer_strength
);

/*======================================================================= */
/*                        pm_gpio_set_src_configuration                */
/*======================================================================= */
/*! \details Set source configuration
 *   <table border="0">
 *      <tr><td><b>Input Parameter</b></td>
 *          <td><b>Type</b></td>
 *          <td><b>Description</b></td>
 *      </tr>
 *     <tr><td>pmic_chip </td>
 *          <td>pm_gpio_uart_path_type</td>
 *          <td>Each PMIC device in the systems is enumerated starting with zero.</td>
 *     <tr><td> gpio </td>
 *          <td>pm_gpio_perph_index</td>
 *          <td>GPIO to configure current source pulls</td>
 *      </tr>
 *     <tr><td> source </td>
 *          <td>pm_gpio_src_config_type</td>
 *          <td>Select Source</td>
 *      </tr>
 *  </table>
 *  \return pm_err_flag_type
 *  \n\n
 *  <b>Example </b>
 *  \n To serve GPIO5 as a output in level translator mode select 'Pair In' as Source
 *  \n errFlag = pm_gpio_set_src_configuration(PM_GPIO_5, 
 *  \n &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; PM_GPIO_SOURCE_PAIRED_GPIO);
 * 
*/
extern pm_err_flag_type pm_gpio_set_source_configuration
(
 unsigned                   pmic_chip, 
 pm_gpio_perph_index        perph_index,
 pm_gpio_src_config_type    src
);
                                   


/*======================================================================= */
/*                        pm_gpio_set_mux_ctrl                            */
/*======================================================================= */
/*! \details Set GPIO mux control
 *   <table border="0">
 *      <tr><td><b>Input Parameter</b></td>
 *          <td><b>Type</b></td>
 *          <td><b>Description</b></td>
 *      </tr>
 *     <tr><td>pmic_chip </td>
 *          <td>pm_gpio_uart_path_type</td>
 *          <td>Each PMIC device in the systems is enumerated starting with zero.</td>
 *     <tr><td> uart_path </td>
 *          <td>pm_gpio_uart_path_type</td>
 *          <td>Select UART path</td>
 *      </tr>
 *  </table>
 *  \return pm_err_flag_type
 *  \n\n
*/
extern pm_err_flag_type pm_gpio_set_mux_ctrl
(
 unsigned                pmic_chip, 
 pm_gpio_uart_path_type  uart_path
);


#endif /* PM_GPIO__H */
