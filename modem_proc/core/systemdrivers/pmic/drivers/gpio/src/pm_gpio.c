/*! \file
*
*  \brief   ----File contains the implementation of the public APIs for GPIO resource type.
*  \details Implementation file for GPIO resourece type.
*  Each of the APIs checks for access and then if necessary directs
*  the call to Driver implementation or to RPC function for the master processor.
*
*    PMIC code generation Version: 2.0.0.19
*    This file contains code for Target specific settings and modes.
*
*  &copy; Copyright 2010-2012 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/drivers/gpio/src/pm_gpio.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------
02/25/15   mr      Removed MPSS PMIC SW Compilation Warnings (CR-799430)
04/10/12   hs      Changed the type for (uint8)gpio from int to uint8.
03/01/12   hs      Updated the driver.
11/01/10   APU     Porting code over from 7x30
xx/yy/zz   ABC     Reverse Chronological Order. Next set of comments here.
06/16/13   kt      Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_gpio.h"
#include "pm_gpio_driver.h"
#include "pm_comm.h"


/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
static pm_err_flag_type pm_gpio_configure_type (uint8 pmic_chip, uint8 gpio, pm_drv_gpio_config_type type);
static pm_err_flag_type pm_gpio_enable (uint8 pmic_chip, uint8 gpio, boolean enable);


/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
pm_err_flag_type pm_dev_gpio_config_bias_voltage(uint8 pmic_chip, pm_gpio_perph_index gpio, pm_gpio_volt_src_type voltage_source)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);

    if (gpio_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ((uint8)gpio >= gpio_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    if (voltage_source >= PM_GPIO_VOLTAGE_SOURCE_TYPE__INVALID)
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }

    /*config GPIO as digital output */
    /* config GPIO as o/p, CMOS, invert the output */
    /* select GND as source */
    /* set high drive strength , enable the block */

    // select GPIO type
    errFlag = pm_gpio_configure_type(pmic_chip, (uint8)gpio, PM_GPIO_DIG_OUT);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    errFlag = pm_dev_gpio_set_voltage_source(pmic_chip, (uint8)gpio, voltage_source);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag;

    errFlag = pm_dev_gpio_set_output_buffer_configuration(pmic_chip, (uint8)gpio, PM_GPIO_OUT_BUFFER_CONFIG_CMOS);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag;

    errFlag = pm_dev_gpio_set_inversion_configuration(pmic_chip, (uint8)gpio, TRUE);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag;

    errFlag = pm_dev_gpio_set_output_buffer_drive_strength(pmic_chip, (uint8)gpio, PM_GPIO_OUT_BUFFER_HIGH);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag;

    //errFlag = pm_gpio_set_ext_pin_config( me, PM_GPIO_EXT_PIN_ENABLE, (uint8)gpio );//no need to do this, since we will enable the GPIO at the end.
    //if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag;

    errFlag = pm_dev_gpio_set_source_configuration(pmic_chip, (uint8)gpio, PM_GPIO_SOURCE_GND);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag;

    errFlag = pm_gpio_enable(pmic_chip, (uint8)gpio, TRUE);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag;

    return errFlag;
}

pm_err_flag_type pm_dev_gpio_config_digital_input(uint8 pmic_chip,
                                                  pm_gpio_perph_index gpio,
                                                  pm_gpio_current_src_pulls_type i_source_pulls,
                                                  pm_gpio_volt_src_type voltage_source,
                                                  pm_gpio_out_buffer_drv_strength_type  out_buffer_strength,
                                                  pm_gpio_src_config_type source)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);

    if (gpio_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ((uint8)gpio >= gpio_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__PAR5_OUT_OF_RANGE;
    }
    if (i_source_pulls >= PM_GPIO_CURRENT_SOURCE_PULLS_TYPE__INVALID)
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
    if (voltage_source >= PM_GPIO_VOLTAGE_SOURCE_TYPE__INVALID)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    if (out_buffer_strength >= PM_GPIO_OUT_BUFFER_DRIVE_STRENGTH__INVALID)
    {
        return PM_ERR_FLAG__PAR3_OUT_OF_RANGE;
    }
    if (source >= PM_GPIO_SOURCE_CONFIG_TYPE__INVALID)
    {
        return PM_ERR_FLAG__PAR4_OUT_OF_RANGE;
    }

    pm_register_address_type dig_in_ctl = gpio_ptr->gpio_register->base_address +
                                          (pm_register_address_type)(gpio_ptr->gpio_register->peripheral_offset*(uint8)gpio) +
                                          gpio_ptr->gpio_register->dig_in_ctl;

    // select GPIO type
    errFlag = pm_gpio_configure_type(pmic_chip, (uint8)gpio, PM_GPIO_DIG_IN);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    errFlag = pm_dev_gpio_set_voltage_source(pmic_chip, (uint8)gpio, voltage_source);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    errFlag = pm_dev_gpio_set_current_source_pulls(pmic_chip, (uint8)gpio, i_source_pulls);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    errFlag = pm_dev_gpio_set_output_buffer_drive_strength(pmic_chip, (uint8)gpio, out_buffer_strength) ;
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    //errFlag = pm_gpio_set_ext_pin_config(PM_GPIO_EXT_PIN_ENABLE, (uint8)gpio);
    //if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag;

    switch (source)
    {
        case PM_GPIO_SOURCE_DTEST1:
            data = 0x1;
            break;
        case PM_GPIO_SOURCE_DTEST2:
            data = 0x2;
            break;
        case PM_GPIO_SOURCE_DTEST3:
            data = 0x4;
            break;
        case PM_GPIO_SOURCE_DTEST4:
            data = 0x8;
            break;
        default:
            data = 0x0;
    }

    /* write the data to the register */
    errFlag = pm_comm_write_byte_mask(gpio_ptr->comm_ptr->slave_id, dig_in_ctl, 0xF, data, 0);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag;

    //no inversion
    errFlag = pm_dev_gpio_set_inversion_configuration(pmic_chip, (uint8)gpio, FALSE);
     if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag;

    errFlag = pm_gpio_enable(pmic_chip, (uint8)gpio, TRUE);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag;

    return errFlag;
}

pm_err_flag_type pm_dev_gpio_config_digital_output(uint8 pmic_chip,
                                                   pm_gpio_perph_index gpio,
                                                   pm_gpio_out_buffer_config_type  out_buffer_config,
                                                   pm_gpio_volt_src_type  voltage_source,
                                                   pm_gpio_src_config_type  source,
                                                   pm_gpio_out_buffer_drv_strength_type  out_buffer_strength,
                                                   boolean  out_inversion)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);

    if (gpio_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ((uint8)gpio >= gpio_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__PAR6_OUT_OF_RANGE;
    }
    if (out_buffer_config >= PM_GPIO_OUT_BUFFER_CONFIG__INVALID)
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
    if (voltage_source >= PM_GPIO_VOLTAGE_SOURCE_TYPE__INVALID)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    if ( source >= PM_GPIO_SOURCE_CONFIG_TYPE__INVALID)
    {
        return PM_ERR_FLAG__PAR3_OUT_OF_RANGE;
    }
    if (out_buffer_strength >= PM_GPIO_OUT_BUFFER_DRIVE_STRENGTH__INVALID)
    {
        return PM_ERR_FLAG__PAR4_OUT_OF_RANGE;
    }

    // select GPIO type
    errFlag = pm_gpio_configure_type(pmic_chip, (uint8)gpio, PM_GPIO_DIG_OUT);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    errFlag = pm_dev_gpio_set_voltage_source(pmic_chip, (uint8)gpio, voltage_source);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag;

    errFlag = pm_dev_gpio_set_inversion_configuration(pmic_chip, (uint8)gpio, out_inversion);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag;

    errFlag = pm_dev_gpio_set_output_buffer_configuration(pmic_chip, (uint8)gpio, out_buffer_config);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag;

    errFlag = pm_dev_gpio_set_output_buffer_drive_strength(pmic_chip, (uint8)gpio, out_buffer_strength);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag;

    errFlag = pm_dev_gpio_set_source_configuration(pmic_chip, (uint8)gpio, source);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag;

    //errFlag = pm_gpio_set_ext_pin_config( PM_GPIO_EXT_PIN_ENABLE, (uint8)gpio );
    //if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag;

    errFlag = pm_gpio_enable(pmic_chip, (uint8)gpio, TRUE);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag;

    return errFlag;
}

pm_err_flag_type pm_dev_gpio_config_digital_input_output(uint8 pmic_chip,
                                                 pm_gpio_perph_index gpio,
                                                 pm_gpio_src_config_type  source,
                                                 pm_gpio_current_src_pulls_type  i_source_pulls,
                                                 pm_gpio_volt_src_type  voltage_source,
                                                 pm_gpio_out_buffer_config_type out_buffer_config,
                                                 pm_gpio_out_buffer_drv_strength_type  out_buffer_strength)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);

    if (gpio_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ((uint8)gpio >= gpio_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__PAR5_OUT_OF_RANGE;
    }
    if (source >= PM_GPIO_SOURCE_CONFIG_TYPE__INVALID)
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
    if (i_source_pulls >= PM_GPIO_CURRENT_SOURCE_PULLS_TYPE__INVALID)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    if (voltage_source >= PM_GPIO_VOLTAGE_SOURCE_TYPE__INVALID)
    {
        return PM_ERR_FLAG__PAR3_OUT_OF_RANGE;
    }
    if (out_buffer_config >= PM_GPIO_OUT_BUFFER_CONFIG__INVALID)
    {
        return PM_ERR_FLAG__PAR4_OUT_OF_RANGE;
    }
    if (out_buffer_strength >= PM_GPIO_OUT_BUFFER_DRIVE_STRENGTH__INVALID)
    {
        return PM_ERR_FLAG__PAR5_OUT_OF_RANGE;
    }

    // select GPIO type
    errFlag = pm_gpio_configure_type(pmic_chip, (uint8)gpio, PM_GPIO_DIG_IN_DIG_OUT);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    errFlag = pm_dev_gpio_set_source_configuration(pmic_chip, (uint8)gpio, source);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag;

    //note: The LSB is used to set the inversion
    errFlag = pm_dev_gpio_set_inversion_configuration(pmic_chip, (uint8)gpio, FALSE);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag;

    errFlag = pm_dev_gpio_set_current_source_pulls(pmic_chip, (uint8)gpio, i_source_pulls);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    errFlag = pm_dev_gpio_set_voltage_source(pmic_chip, (uint8)gpio, voltage_source);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    errFlag = pm_dev_gpio_set_output_buffer_configuration(pmic_chip, (uint8)gpio, out_buffer_config);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag;

    errFlag = pm_dev_gpio_set_output_buffer_drive_strength(pmic_chip, (uint8)gpio, out_buffer_strength) ;
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    //enable GPIO
    errFlag = pm_gpio_enable(pmic_chip, (uint8)gpio, TRUE);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    return errFlag;
}

pm_err_flag_type pm_dev_gpio_set_voltage_source(uint8 pmic_chip, pm_gpio_perph_index gpio, pm_gpio_volt_src_type  voltage_source)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);

    if (gpio_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ((uint8)gpio >= gpio_ptr->num_of_peripherals)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    if (voltage_source >= PM_GPIO_VOLTAGE_SOURCE_TYPE__INVALID)
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }

    pm_register_address_type dig_vin_ctl = gpio_ptr->gpio_register->base_address + (pm_register_address_type)(gpio_ptr->gpio_register->peripheral_offset*(uint8)gpio)
        + gpio_ptr->gpio_register->dig_vin_ctl;

    /* write the data to the register */
    errFlag = pm_comm_write_byte_mask(gpio_ptr->comm_ptr->slave_id, dig_vin_ctl, 0x07, (pm_register_data_type)voltage_source, 0);
    return errFlag;
}

pm_err_flag_type pm_dev_gpio_set_output_buffer_configuration(uint8 pmic_chip, pm_gpio_perph_index gpio, pm_gpio_out_buffer_config_type  out_buffer_config)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);

      if (gpio_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ((uint8)gpio >= gpio_ptr->num_of_peripherals)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    if (out_buffer_config >= PM_GPIO_OUT_BUFFER_CONFIG__INVALID)
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }

    pm_register_address_type dig_out_ctl = gpio_ptr->gpio_register->base_address + (pm_register_address_type)(gpio_ptr->gpio_register->peripheral_offset*(uint8)gpio) +
              gpio_ptr->gpio_register->dig_out_ctl;

    /* write the data to the register */
    errFlag = pm_comm_write_byte_mask(gpio_ptr->comm_ptr->slave_id, dig_out_ctl, 0x30, (pm_register_data_type)(out_buffer_config<<4), 0 ) ;

    return errFlag;
}

pm_err_flag_type pm_dev_gpio_set_inversion_configuration(uint8 pmic_chip, pm_gpio_perph_index gpio, boolean  inversion)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);
    pm_register_data_type data = 0;

    if (gpio_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if ((uint8)gpio >= gpio_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    //The LSB in mode_ctl is used to set the inversion. The non-inversion and inversion are paired.
    if(TRUE == inversion)
    {
        data = 0x01;
    }

    pm_register_address_type mode_ctl = gpio_ptr->gpio_register->base_address + (pm_register_address_type)(gpio_ptr->gpio_register->peripheral_offset*(uint8)gpio) +
              gpio_ptr->gpio_register->mode_ctl;

    /* write the data to the register */
    //Output Source select:
    //0000 = 0
    //0001 = 1
    //0010 = paired GPIO
    //0011 = not paired GPIO
    //0100 = special function1
    //0101 = not special function1
    //0110 = special function2
    //0111 = not special function2
    //1000 = DTEST1
    //1001 = not DTEST1
    //1010 = DTEST2
    //1011 = not DTEST2
    //1100 = DTEST3
    //1101 = not DTEST3
    //1110 = DTEST4
    //1111 = not DTEST4
    errFlag = pm_comm_write_byte_mask(gpio_ptr->comm_ptr->slave_id, mode_ctl, 0x01, data, 0 ) ;//todo ask chris

    return errFlag;
}

pm_err_flag_type pm_dev_gpio_set_current_source_pulls(uint8 pmic_chip, pm_gpio_perph_index gpio, pm_gpio_current_src_pulls_type  i_source_pulls)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);

    if (gpio_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ((uint8)gpio >= gpio_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    if (i_source_pulls >= PM_GPIO_CURRENT_SOURCE_PULLS_TYPE__INVALID)
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }

    pm_register_address_type dig_pull_ctl = gpio_ptr->gpio_register->base_address + (pm_register_address_type)(gpio_ptr->gpio_register->peripheral_offset*(uint8)gpio) +
              gpio_ptr->gpio_register->dig_pull_ctl;

    /* write the data to the register */
    errFlag = pm_comm_write_byte_mask(gpio_ptr->comm_ptr->slave_id, dig_pull_ctl, 0x07,(pm_register_data_type)(i_source_pulls), 0 ) ;

    return errFlag;
}

pm_err_flag_type pm_dev_gpio_set_ext_pin_config(uint8 pmic_chip, pm_gpio_perph_index gpio, pm_gpio_ext_pin_config_type  ext_pin_config)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data = 0;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);

    if (gpio_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ((uint8)gpio >= gpio_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    if (ext_pin_config >= PM_GPIO_EXT_PIN_CONFIG_TYPE__INVALID  )
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }

    pm_register_address_type en_ctl = gpio_ptr->gpio_register->base_address + (pm_register_address_type)(gpio_ptr->gpio_register->peripheral_offset*(uint8)gpio) +
        gpio_ptr->gpio_register->en_ctl;

    if(ext_pin_config == PM_GPIO_EXT_PIN_ENABLE)
    {
        data = 0x80;
    }
    /* write the data to the register */
    errFlag = pm_comm_write_byte_mask(gpio_ptr->comm_ptr->slave_id, en_ctl, 0x80, data, 0 ) ;

    return errFlag;
}

pm_err_flag_type pm_dev_gpio_set_output_buffer_drive_strength(uint8 pmic_chip,pm_gpio_perph_index gpio, pm_gpio_out_buffer_drv_strength_type  out_buffer_strength)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);

    if (gpio_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ((uint8)gpio >= gpio_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    if (out_buffer_strength >= PM_GPIO_OUT_BUFFER_DRIVE_STRENGTH__INVALID)
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }

    pm_register_address_type dig_out_ctl = gpio_ptr->gpio_register->base_address + (pm_register_address_type)(gpio_ptr->gpio_register->peripheral_offset*(uint8)gpio) +
              gpio_ptr->gpio_register->dig_out_ctl;

    /* write the data to the register */
    errFlag = pm_comm_write_byte_mask(gpio_ptr->comm_ptr->slave_id, dig_out_ctl, 0x03, (pm_register_data_type)out_buffer_strength, 0) ;

    return errFlag;
}

pm_err_flag_type pm_dev_gpio_set_source_configuration(uint8 pmic_chip,pm_gpio_perph_index gpio, pm_gpio_src_config_type  source)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);

    if (gpio_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ((uint8)gpio >= gpio_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    if (source >= PM_GPIO_SOURCE_CONFIG_TYPE__INVALID)
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }

    pm_register_address_type mode_ctl = gpio_ptr->gpio_register->base_address + (pm_register_address_type)(gpio_ptr->gpio_register->peripheral_offset*(uint8)gpio) +
              gpio_ptr->gpio_register->mode_ctl;

    /* write the data to the register */
    //Output Source select:
    //0000 = 0
    //0001 = 1
    //0010 = paired GPIO
    //0011 = not paired GPIO
    //0100 = special function1
    //0101 = not special function1
    //0110 = special function2
    //0111 = not special function2
    //1000 = DTEST1
    //1001 = not DTEST1
    //1010 = DTEST2
    //1011 = not DTEST2
    //1100 = DTEST3
    //1101 = not DTEST3
    //1110 = DTEST4
    //1111 = not DTEST4
    //note: The LSB is used to set the inversion, which will be set in pm_gpio_set_inversion_configuration().
    errFlag = pm_comm_write_byte_mask(gpio_ptr->comm_ptr->slave_id, mode_ctl, 0x0E, (pm_register_data_type)(source<<1), 0 ) ;

    return errFlag;
}

pm_err_flag_type pm_gpio_set_mux_ctrl(pm_gpio_uart_path_type uart_path)
{
    (void)uart_path;

    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
}

pm_err_flag_type pm_dev_gpio_status_get(uint8 pmic_chip, pm_gpio_perph_index gpio , pm_gpio_status_type *gpio_status)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type reg_data = 0;
    pm_register_data_type reg_data2 = 0;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);

    if (gpio_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ( (uint8)gpio >= gpio_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }

    if(NULL == gpio_status)
    {
        return PM_ERR_FLAG__INVALID_POINTER;
    }

    pm_register_address_type base_address = gpio_ptr->gpio_register->base_address + (pm_register_address_type)(gpio_ptr->gpio_register->peripheral_offset*(uint8)gpio);
    pm_register_address_type mode_ctl =  base_address + gpio_ptr->gpio_register->mode_ctl;
    pm_register_address_type dig_vin_ctl =  base_address + gpio_ptr->gpio_register->dig_vin_ctl;
    pm_register_address_type dig_pull_ctl =  base_address + gpio_ptr->gpio_register->dig_pull_ctl;
    pm_register_address_type dig_in_ctl =  base_address + gpio_ptr->gpio_register->dig_in_ctl;
    pm_register_address_type dig_out_ctl =  base_address + gpio_ptr->gpio_register->dig_out_ctl;
    pm_register_address_type en_ctl =  base_address + gpio_ptr->gpio_register->en_ctl;

    // Read current source pulls
    errFlag = pm_comm_read_byte(gpio_ptr->comm_ptr->slave_id, dig_pull_ctl, &reg_data, 0 );
    if(PM_ERR_FLAG__SUCCESS == errFlag)
    {
        reg_data = reg_data & 0x07;
        gpio_status->gpio_current_src_pulls = (pm_gpio_current_src_pulls_type)reg_data;
    }

    // Read voltage source
    errFlag = pm_comm_read_byte(gpio_ptr->comm_ptr->slave_id, dig_vin_ctl, &reg_data, 0 );
    if(PM_ERR_FLAG__SUCCESS == errFlag)
    {
        reg_data = reg_data & 0x07;
        gpio_status->gpio_volt_src = (pm_gpio_volt_src_type)reg_data;
    }

    // Read output buffer config Read output buffer strength
    errFlag = pm_comm_read_byte(gpio_ptr->comm_ptr->slave_id,dig_out_ctl, &reg_data, 0 );
    if(PM_ERR_FLAG__SUCCESS == errFlag)
    {
        reg_data2 = reg_data & 0x30;
        gpio_status->gpio_out_buffer_config = (pm_gpio_out_buffer_config_type)(reg_data2>>4);
        reg_data = reg_data & 0x03;
        gpio_status->gpio_out_buffer_drv_strength = (pm_gpio_out_buffer_drv_strength_type)reg_data;
    }

    // Read inversion status and source config
    errFlag = pm_comm_read_byte(gpio_ptr->comm_ptr->slave_id, mode_ctl, &reg_data, 0 );
    if(PM_ERR_FLAG__SUCCESS == errFlag)
    {
        gpio_status->gpio_src_config = (pm_gpio_src_config_type)((reg_data & 0x0E)>>1);
        gpio_status->gpio_invert_ext_pin = (pm_gpio_invert_ext_pin_type)(reg_data & 0x01);
        reg_data2 = (reg_data &0x70)>>4 ;
        if (reg_data2==0)
        {
            gpio_status->gpio_mode_select = PM_GPIO_INPUT_ON;
        }
        else if(reg_data2==1)
        {
            gpio_status->gpio_mode_select = PM_GPIO_OUTPUT_ON;
        }
        else if(reg_data2==2)
        {
            gpio_status->gpio_mode_select = PM_GPIO_INPUT_OUTPUT_ON;
        }
        else
        {
            gpio_status->gpio_mode_select = PM_GPIO_INPUT_OUTPUT_OFF;
        }
    }

    // Read DTEST buffer
    errFlag = pm_comm_read_byte(gpio_ptr->comm_ptr->slave_id, dig_in_ctl, &reg_data, 0 );
    if(PM_ERR_FLAG__SUCCESS == errFlag)
    {
        reg_data = reg_data & 0x0F;
        gpio_status->gpio_dtest_buffer_on_off = (reg_data==0) ? PM_GPIO_DTEST_DISABLE : PM_GPIO_DTEST_ENABLE;
        if(gpio_status->gpio_mode_select == PM_GPIO_INPUT_ON)
        {
            switch (reg_data)
            {
                case 0x0:
                    gpio_status->gpio_src_config = PM_GPIO_SOURCE_GND;
                    break;
                case 0x1:
                    gpio_status->gpio_src_config = PM_GPIO_SOURCE_DTEST1;
                    break;
                case 0x2:
                    gpio_status->gpio_src_config = PM_GPIO_SOURCE_DTEST2;
                    break;
                case 0x4:
                    gpio_status->gpio_src_config = PM_GPIO_SOURCE_DTEST3;
                    break;
                case 0x8:
                    gpio_status->gpio_src_config = PM_GPIO_SOURCE_DTEST4;
                    break;
                default:
                    gpio_status->gpio_src_config = PM_GPIO_SOURCE_CONFIG_TYPE__INVALID;
                    break;
            }
        }
    }

    // Read EXT_Pin config
    errFlag = pm_comm_read_byte(gpio_ptr->comm_ptr->slave_id, en_ctl, &reg_data, 0 );
    if(PM_ERR_FLAG__SUCCESS == errFlag)
    {
        reg_data = reg_data & 0x80;
        gpio_status->gpio_ext_pin_config = (pm_gpio_ext_pin_config_type)(reg_data>>7);
    }

    return errFlag;
}

pm_err_flag_type pm_gpio_configure_type(uint8 pmic_chip, uint8  gpio, pm_drv_gpio_config_type type)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);

    if (gpio_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_register_address_type mode_ctl = gpio_ptr->gpio_register->base_address + (pm_register_address_type)(gpio_ptr->gpio_register->peripheral_offset*gpio) +
              gpio_ptr->gpio_register->mode_ctl;

    // Set GPIO
    //GPIO Mode:
    //- 0: Digital Input
    //- 1: Digital Output
    //- 2: Digital Input and Output
    //- 3: Reserved
    errFlag = pm_comm_write_byte_mask(gpio_ptr->comm_ptr->slave_id, mode_ctl , 0x70, (pm_register_data_type)(type<<4), 0);

    return errFlag;
}

pm_err_flag_type pm_gpio_enable(uint8 pmic_chip, uint8  gpio, boolean enable)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);

    if (gpio_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_register_address_type en_ctl = gpio_ptr->gpio_register->base_address + (pm_register_address_type)(gpio_ptr->gpio_register->peripheral_offset*gpio) +
              gpio_ptr->gpio_register->en_ctl;

    pm_register_data_type data = 0;

    if(TRUE == enable)
    {
        data = 0x80;
    }
    // Enable GPIO
    //GPIO Master Enable
    //0 = puts GPIO_PAD at high Z and disables the block
    //1 = GPIO is enabled
    errFlag = pm_comm_write_byte_mask(gpio_ptr->comm_ptr->slave_id, en_ctl , 0x80, data, 0);

    return errFlag;
}

pm_err_flag_type pm_dev_gpio_peripheral_info_get(uint8 pmic_chip, pm_gpio_perph_index gpio, peripheral_info_type *peripheralInfo)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);

    if (gpio_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ( (uint8)gpio >= gpio_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }

    if(NULL == peripheralInfo)
    {
        return PM_ERR_FLAG__INVALID_POINTER;
    }

    pm_register_address_type base_address = gpio_ptr->gpio_register->base_address + (pm_register_address_type)(gpio_ptr->gpio_register->peripheral_offset*(uint8)gpio);

    // Reset the peripheral info
    peripheralInfo->base_address = base_address;
    peripheralInfo->peripheral_type = 0;
    peripheralInfo->peripheral_subtype = 0;
    peripheralInfo->analog_major_version = 0;
    peripheralInfo->analog_minor_version = 0;
    peripheralInfo->digital_major_version = 0;
    peripheralInfo->digital_minor_version = 0;

    errFlag = pm_target_information_read_peripheral_rev(gpio_ptr->comm_ptr, peripheralInfo);

    return errFlag;
}

pm_err_flag_type pm_gpio_irq_enable(uint8 pmic_chip, pm_gpio_perph_index gpio, boolean enable)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);
    pm_register_address_type IRQ_REG;

    if (gpio_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ((uint8)gpio >= gpio_ptr->num_of_peripherals)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    if (enable)
    {
        IRQ_REG = gpio_ptr->gpio_register->base_address + (pm_register_address_type)(gpio_ptr->gpio_register->peripheral_offset*(uint8)gpio) + gpio_ptr->gpio_register->int_en_set;
    }
    else
    {
        IRQ_REG = gpio_ptr->gpio_register->base_address + (pm_register_address_type)(gpio_ptr->gpio_register->peripheral_offset*(uint8)gpio) + gpio_ptr->gpio_register->int_en_clr;
    }

    errFlag = pm_comm_write_byte(gpio_ptr->comm_ptr->slave_id, IRQ_REG, 0x01, 0);
    return errFlag;
}

pm_err_flag_type pm_gpio_irq_clear(uint8 pmic_chip, pm_gpio_perph_index gpio)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);

    if (gpio_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ((uint8)gpio >= gpio_ptr->num_of_peripherals)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    pm_register_address_type INT_LATCHED_CLR = gpio_ptr->gpio_register->base_address + (pm_register_address_type)(gpio_ptr->gpio_register->peripheral_offset*(uint8)gpio) + gpio_ptr->gpio_register->int_latched_clr;

    errFlag = pm_comm_write_byte(gpio_ptr->comm_ptr->slave_id, INT_LATCHED_CLR, 0x01, 0);
    return errFlag;
}


pm_err_flag_type pm_gpio_irq_set_trigger(uint8 pmic_chip, pm_gpio_perph_index gpio, pm_irq_trigger_type trigger)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);
    pm_register_data_type set_type, polarity_high, polarity_low;

    if (gpio_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ((uint8)gpio >= gpio_ptr->num_of_peripherals)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    pm_register_address_type INT_SET_TYPE = gpio_ptr->gpio_register->base_address + (pm_register_address_type)(gpio_ptr->gpio_register->peripheral_offset*(uint8)gpio) + gpio_ptr->gpio_register->int_set_type;
    pm_register_address_type INT_POLARITY_HIGH = gpio_ptr->gpio_register->base_address + (pm_register_address_type)(gpio_ptr->gpio_register->peripheral_offset*(uint8)gpio) + gpio_ptr->gpio_register->int_polarity_high;
    pm_register_address_type INT_POLARITY_LOW = gpio_ptr->gpio_register->base_address + (pm_register_address_type)(gpio_ptr->gpio_register->peripheral_offset*(uint8)gpio) + gpio_ptr->gpio_register->int_polarity_low;

    switch (trigger)
    {
    case PM_IRQ_TRIGGER_ACTIVE_LOW:
        set_type = 0x00;
        polarity_high = 0x00;
        polarity_low = 0xFF;
        break;
    case PM_IRQ_TRIGGER_ACTIVE_HIGH:
        set_type = 0x00;
        polarity_high = 0xFF;
        polarity_low = 0x00;
        break;
    case PM_IRQ_TRIGGER_RISING_EDGE:
        set_type = 0xFF;
        polarity_high = 0xFF;
        polarity_low = 0x00;
        break;
    case PM_IRQ_TRIGGER_FALLING_EDGE:
        set_type = 0xFF;
        polarity_high = 0x00;
        polarity_low = 0xFF;
        break;
    case PM_IRQ_TRIGGER_DUAL_EDGE:
        set_type = 0xFF;
        polarity_high = 0xFF;
        polarity_low = 0xFF;
        break;
    default:
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    errFlag  = pm_comm_write_byte_mask(gpio_ptr->comm_ptr->slave_id, INT_SET_TYPE, 0x01 , set_type, 0);
    errFlag |= pm_comm_write_byte_mask(gpio_ptr->comm_ptr->slave_id, INT_POLARITY_HIGH, 0x01, polarity_high, 0);
    errFlag |= pm_comm_write_byte_mask(gpio_ptr->comm_ptr->slave_id, INT_POLARITY_LOW, 0x01, polarity_low, 0);

    return errFlag;
}

pm_err_flag_type pm_gpio_irq_status(uint8 pmic_chip, pm_gpio_perph_index gpio, pm_irq_status_type type, boolean *status)
{
    pm_err_flag_type    errFlag    = PM_ERR_FLAG__SUCCESS;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);
    pm_register_data_type data;
    pm_register_address_type INT_STS;

    if (gpio_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ((uint8)gpio >= gpio_ptr->num_of_peripherals)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    switch (type)
    {
    case PM_IRQ_STATUS_RT:
        INT_STS =gpio_ptr->gpio_register->base_address + (pm_register_address_type)(gpio_ptr->gpio_register->peripheral_offset*(uint8)gpio) + gpio_ptr->gpio_register->int_rt_sts;
        break;
    case PM_IRQ_STATUS_LATCHED:
        INT_STS = gpio_ptr->gpio_register->base_address + (pm_register_address_type)(gpio_ptr->gpio_register->peripheral_offset*(uint8)gpio) + gpio_ptr->gpio_register->int_latched_sts;
        break;
    case PM_IRQ_STATUS_PENDING:
        INT_STS = gpio_ptr->gpio_register->base_address + (pm_register_address_type)(gpio_ptr->gpio_register->peripheral_offset*(uint8)gpio) + gpio_ptr->gpio_register->int_pending_sts;
        break;
    case PM_IRQ_STATUS_ENABLE:
        INT_STS = gpio_ptr->gpio_register->base_address + (pm_register_address_type)(gpio_ptr->gpio_register->peripheral_offset*(uint8)gpio) + gpio_ptr->gpio_register->int_en_set;
        break;
    default:
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    errFlag = pm_comm_read_byte_mask(gpio_ptr->comm_ptr->slave_id, INT_STS, 0x01, &data,0);
    *status = data ? TRUE : FALSE;

    return errFlag;

}


/*===========================================================================

                        DEPRECATED FUNCTIONS

===========================================================================*/

pm_err_flag_type pm_gpio_status_get(pm_gpio_perph_index gpio, pm_gpio_status_type  *gpio_status)
{
    return pm_dev_gpio_status_get(0, gpio, gpio_status);
}
pm_err_flag_type pm_gpio_config_bias_voltage(pm_gpio_perph_index gpio, pm_gpio_volt_src_type  voltage_source)
{
    return pm_dev_gpio_config_bias_voltage(0, gpio, voltage_source);
}
pm_err_flag_type pm_gpio_config_digital_input(pm_gpio_perph_index gpio, pm_gpio_current_src_pulls_type i_source_pulls, pm_gpio_volt_src_type        voltage_source,
                                                  pm_gpio_out_buffer_drv_strength_type  out_buffer_strength, pm_gpio_src_config_type source)
{
    return pm_dev_gpio_config_digital_input(0, gpio, i_source_pulls, voltage_source, out_buffer_strength, source);
}
pm_err_flag_type pm_gpio_config_digital_output(pm_gpio_perph_index gpio,pm_gpio_out_buffer_config_type out_buffer_config,pm_gpio_volt_src_type voltage_source,
                                                   pm_gpio_src_config_type source, pm_gpio_out_buffer_drv_strength_type out_buffer_strength, boolean out_inversion)
{
    return pm_dev_gpio_config_digital_output(0, gpio, out_buffer_config, voltage_source, source, out_buffer_strength, out_inversion);
}
pm_err_flag_type pm_gpio_config_digital_input_output(pm_gpio_perph_index gpio, pm_gpio_src_config_type source, pm_gpio_current_src_pulls_type i_source_pulls,
                                                         pm_gpio_volt_src_type voltage_source, pm_gpio_out_buffer_config_type out_buffer_config,pm_gpio_out_buffer_drv_strength_type out_buffer_strength)
{
    return pm_dev_gpio_config_digital_input_output(0, gpio, source, i_source_pulls, voltage_source, out_buffer_config, out_buffer_strength);
}
pm_err_flag_type pm_gpio_set_voltage_source(pm_gpio_perph_index gpio,pm_gpio_volt_src_type voltage_source)
{
    return pm_dev_gpio_set_voltage_source(0, gpio, voltage_source);
}
pm_err_flag_type pm_gpio_set_output_buffer_configuration(pm_gpio_perph_index gpio,pm_gpio_out_buffer_config_type out_buffer_config)
{
    return pm_dev_gpio_set_output_buffer_configuration(0, gpio, out_buffer_config);
}
pm_err_flag_type pm_gpio_set_inversion_configuration(pm_gpio_perph_index gpio,boolean inversion)
{
    return pm_dev_gpio_set_inversion_configuration(0, gpio, inversion);
}
pm_err_flag_type pm_gpio_set_current_source_pulls(pm_gpio_perph_index gpio,pm_gpio_current_src_pulls_type i_source_pulls)
{
    return pm_dev_gpio_set_current_source_pulls(0, gpio, i_source_pulls);
}
pm_err_flag_type pm_gpio_set_ext_pin_config(pm_gpio_perph_index gpio,pm_gpio_ext_pin_config_type ext_pin_config)
{
    return pm_dev_gpio_set_ext_pin_config(0, gpio, ext_pin_config);
}
pm_err_flag_type pm_gpio_set_output_buffer_drive_strength(pm_gpio_perph_index gpio,pm_gpio_out_buffer_drv_strength_type out_buffer_strength)
{
    return pm_dev_gpio_set_output_buffer_drive_strength(0, gpio, out_buffer_strength);
}
pm_err_flag_type pm_gpio_set_source_configuration(pm_gpio_perph_index gpio, pm_gpio_src_config_type source)
{
    return pm_dev_gpio_set_source_configuration(0, gpio, source);
}