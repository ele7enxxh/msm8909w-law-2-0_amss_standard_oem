/*! \file pm_gpio.c
 *  \n 
 *  \brief Implementation file for GPIO public APIs. 
 *  \n  
 *  \n &copy; Copyright 2010-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/gpio/src/pm_gpio.c#1 $ 
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture    
03/20/13   aab     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_gpio.h"
#include "pm_gpio_driver.h"
#include "CoreVerify.h"
#include "pm_comm.h"


/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
static pm_err_flag_type pm_gpio_configure_type	(unsigned pmic_chip, uint8  gpio, pm_drv_gpio_config_type type);
static pm_err_flag_type pm_gpio_enable        	(unsigned pmic_chip, uint8  gpio, boolean enable);

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
pm_err_flag_type pm_gpio_config_bias_voltage(unsigned pmic_chip, pm_gpio_perph_index gpio, pm_gpio_volt_src_type  voltage_source)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
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
    err_flag = pm_gpio_configure_type(pmic_chip, gpio, PM_GPIO_DIG_OUT);
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag ;

    err_flag = pm_gpio_set_volt_source(pmic_chip, gpio, voltage_source);
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

    err_flag = pm_gpio_set_output_buffer_configuration(pmic_chip, gpio, PM_GPIO_OUT_BUFFER_CONFIG_CMOS);
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

    err_flag = pm_gpio_set_inversion_configuration(pmic_chip, gpio, TRUE);
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

    err_flag = pm_gpio_set_output_buffer_drive_strength(pmic_chip, gpio, PM_GPIO_OUT_BUFFER_HIGH);
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

    //err_flag = pm_gpio_set_ext_pin_config( me, PM_GPIO_EXT_PIN_ENABLE, (uint8)gpio );//no need to do this, since we will enable the GPIO at the end.
    //if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

    err_flag = pm_gpio_set_source_configuration(pmic_chip, gpio, PM_GPIO_SOURCE_GND);
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

    err_flag = pm_gpio_enable(pmic_chip, gpio, TRUE);
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

    return err_flag;
}

pm_err_flag_type pm_gpio_config_digital_input(unsigned pmic_chip,
                                              pm_gpio_perph_index                 gpio,
											  pm_gpio_current_src_pulls_type  i_source_pulls,
											  pm_gpio_volt_src_type        voltage_source,
											  pm_gpio_out_buffer_drv_strength_type  out_buffer_strength,
											  pm_gpio_src_config_type         source
											  )
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
	pm_register_data_type data;
	pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);
    pm_register_address_type periph_index = (pm_register_address_type)gpio;
    pm_register_address_type dig_in_ctl;

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

	dig_in_ctl = gpio_ptr->gpio_reg_table->base_address + (gpio_ptr->gpio_reg_table->peripheral_offset*periph_index) + gpio_ptr->gpio_reg_table->dig_in_ctl;
	
    // select GPIO type
    err_flag = pm_gpio_configure_type(pmic_chip, gpio, PM_GPIO_DIG_IN);
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag ;

    err_flag = pm_gpio_set_volt_source(pmic_chip, gpio, voltage_source);
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag ;

    err_flag = pm_gpio_set_current_source_pulls(pmic_chip, gpio, i_source_pulls);
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag ;

    err_flag = pm_gpio_set_output_buffer_drive_strength(pmic_chip, gpio, out_buffer_strength) ;
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag ;

    //err_flag = pm_gpio_set_ext_pin_config(PM_GPIO_EXT_PIN_ENABLE, (uint8)gpio);
    //if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

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
    err_flag = pm_comm_write_byte_mask(gpio_ptr->comm_ptr->slave_id, dig_in_ctl, 0xF, data, 0);
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

    //no inversion
    err_flag = pm_gpio_set_inversion_configuration(pmic_chip, gpio, FALSE);
     if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

    err_flag = pm_gpio_enable(pmic_chip, gpio, TRUE);
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

    return err_flag;
}

pm_err_flag_type pm_gpio_config_digital_output(unsigned pmic_chip,
                                                  pm_gpio_perph_index gpio,
                                                  pm_gpio_out_buffer_config_type  out_buffer_config,
                                                  pm_gpio_volt_src_type  voltage_source,
                                                  pm_gpio_src_config_type  source,
                                                  pm_gpio_out_buffer_drv_strength_type  out_buffer_strength,
                                                  boolean  out_inversion)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
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
    err_flag = pm_gpio_configure_type(pmic_chip, gpio, PM_GPIO_DIG_OUT);
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag ;

    err_flag = pm_gpio_set_volt_source(pmic_chip, gpio, voltage_source);
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

    err_flag = pm_gpio_set_inversion_configuration(pmic_chip, gpio, out_inversion);
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

    err_flag = pm_gpio_set_output_buffer_configuration(pmic_chip, gpio, out_buffer_config);
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

    err_flag = pm_gpio_set_output_buffer_drive_strength(pmic_chip, gpio, out_buffer_strength);
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

    err_flag = pm_gpio_set_source_configuration(pmic_chip, gpio, source);
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

    //err_flag = pm_gpio_set_ext_pin_config( PM_GPIO_EXT_PIN_ENABLE, (uint8)gpio );
    //if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

    err_flag = pm_gpio_enable(pmic_chip, gpio, TRUE);
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

    return err_flag;
}

pm_err_flag_type pm_gpio_config_digital_input_output(unsigned pmic_chip,
                                                     pm_gpio_perph_index gpio,
											         pm_gpio_src_config_type  source,
                                                     pm_gpio_current_src_pulls_type  i_source_pulls,
                                                     pm_gpio_volt_src_type  voltage_source,
												     pm_gpio_out_buffer_config_type out_buffer_config,
                                                     pm_gpio_out_buffer_drv_strength_type  out_buffer_strength)											 
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
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
    err_flag = pm_gpio_configure_type(pmic_chip, gpio, PM_GPIO_DIG_IN_DIG_OUT);
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag ;

    err_flag = pm_gpio_set_source_configuration(pmic_chip, gpio, source);
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

	//note: The LSB is used to set the inversion
    err_flag = pm_gpio_set_inversion_configuration(pmic_chip, gpio, FALSE);
	if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

    err_flag = pm_gpio_set_current_source_pulls(pmic_chip, gpio, i_source_pulls);
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag ;

    err_flag = pm_gpio_set_volt_source(pmic_chip, gpio, voltage_source);
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag ;

    err_flag = pm_gpio_set_output_buffer_configuration(pmic_chip, gpio, out_buffer_config);
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag;

    err_flag = pm_gpio_set_output_buffer_drive_strength(pmic_chip, gpio, out_buffer_strength) ;
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag ;

	//enable GPIO
	err_flag = pm_gpio_enable(pmic_chip, gpio, TRUE);
    if(err_flag != PM_ERR_FLAG__SUCCESS) return err_flag ;

    return err_flag;
}


pm_err_flag_type pm_gpio_set_volt_source(unsigned pmic_chip, pm_gpio_perph_index gpio, pm_gpio_volt_src_type  voltage_source)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);
    pm_register_address_type periph_index = (pm_register_address_type)gpio;
    pm_register_address_type dig_vin_ctl;

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

	dig_vin_ctl = gpio_ptr->gpio_reg_table->base_address + (gpio_ptr->gpio_reg_table->peripheral_offset*periph_index) + gpio_ptr->gpio_reg_table->dig_vin_ctl;
	
    /* write the data to the register */
    err_flag = pm_comm_write_byte_mask(gpio_ptr->comm_ptr->slave_id, dig_vin_ctl, 0x07, (pm_register_data_type)voltage_source, 0);
    return err_flag;
}


pm_err_flag_type pm_gpio_set_output_buffer_configuration(unsigned pmic_chip, pm_gpio_perph_index gpio, pm_gpio_out_buffer_config_type  out_buffer_config)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);
    pm_register_address_type periph_index = (pm_register_address_type)gpio;
    pm_register_address_type dig_out_ctl;

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

	dig_out_ctl = gpio_ptr->gpio_reg_table->base_address + (gpio_ptr->gpio_reg_table->peripheral_offset*periph_index) + gpio_ptr->gpio_reg_table->dig_out_ctl;
	
    /* write the data to the register */
    err_flag = pm_comm_write_byte_mask(gpio_ptr->comm_ptr->slave_id, dig_out_ctl, 0x30, (pm_register_data_type)(out_buffer_config<<4), 0 ) ;

    return err_flag;
}

pm_err_flag_type pm_gpio_set_inversion_configuration(unsigned pmic_chip, pm_gpio_perph_index gpio, boolean  inversion)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);
    pm_register_address_type periph_index = (pm_register_address_type)gpio;
    pm_register_address_type mode_ctl;

    pm_register_data_type data = 0;
	
	CORE_VERIFY(gpio_ptr);

    if ((uint8)gpio >= gpio_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    //The LSB in mode_ctl is used to set the inversion. The non-inversion and inversion are paired.
    if(TRUE == inversion)
    {
        data = 0x01;
    }
	
	mode_ctl = gpio_ptr->gpio_reg_table->base_address + (gpio_ptr->gpio_reg_table->peripheral_offset*periph_index) + gpio_ptr->gpio_reg_table->mode_ctl;
	
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
    err_flag = pm_comm_write_byte_mask(gpio_ptr->comm_ptr->slave_id, mode_ctl, 0x01, data, 0 ) ;//todo ask chris

    return err_flag;
}

pm_err_flag_type pm_gpio_set_current_source_pulls(unsigned pmic_chip, pm_gpio_perph_index gpio, pm_gpio_current_src_pulls_type  i_source_pulls)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);
    pm_register_address_type periph_index = (pm_register_address_type)gpio;
    pm_register_address_type dig_pull_ctl;

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

	dig_pull_ctl = gpio_ptr->gpio_reg_table->base_address + (gpio_ptr->gpio_reg_table->peripheral_offset*periph_index) + gpio_ptr->gpio_reg_table->dig_pull_ctl;
		
    /* write the data to the register */
    err_flag = pm_comm_write_byte_mask(gpio_ptr->comm_ptr->slave_id, dig_pull_ctl, 0x07,(pm_register_data_type)(i_source_pulls), 0 ) ;

    return err_flag;
}

pm_err_flag_type pm_gpio_set_ext_pin_config(unsigned pmic_chip, pm_gpio_perph_index gpio, pm_gpio_ext_pin_config_type  ext_pin_config)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data = 0;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);
    pm_register_address_type periph_index = (pm_register_address_type)gpio;
    pm_register_address_type en_ctl;

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

	en_ctl = gpio_ptr->gpio_reg_table->base_address + (gpio_ptr->gpio_reg_table->peripheral_offset*periph_index) + gpio_ptr->gpio_reg_table->en_ctl;

    if(ext_pin_config == PM_GPIO_EXT_PIN_ENABLE)
    {
        data = 0x80;
    }
    /* write the data to the register */
    err_flag = pm_comm_write_byte_mask(gpio_ptr->comm_ptr->slave_id, en_ctl, 0x80, data, 0 ) ;

    return err_flag;
}

pm_err_flag_type pm_gpio_set_output_buffer_drive_strength(unsigned pmic_chip, pm_gpio_perph_index gpio, pm_gpio_out_buffer_drv_strength_type  out_buffer_strength)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);
    pm_register_address_type periph_index = (pm_register_address_type)gpio;
    pm_register_address_type dig_out_ctl;

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

	dig_out_ctl = gpio_ptr->gpio_reg_table->base_address +(gpio_ptr->gpio_reg_table->peripheral_offset*periph_index) + gpio_ptr->gpio_reg_table->dig_out_ctl;
	
    /* write the data to the register */
    err_flag = pm_comm_write_byte_mask(gpio_ptr->comm_ptr->slave_id, dig_out_ctl, 0x03, (pm_register_data_type)out_buffer_strength, 0) ;

    return err_flag;
}

pm_err_flag_type pm_gpio_set_source_configuration(unsigned pmic_chip, pm_gpio_perph_index gpio, pm_gpio_src_config_type  source)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);
    pm_register_address_type periph_index = (pm_register_address_type)gpio;
    pm_register_address_type mode_ctl;

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
	
	mode_ctl = gpio_ptr->gpio_reg_table->base_address + (gpio_ptr->gpio_reg_table->peripheral_offset*periph_index) + gpio_ptr->gpio_reg_table->mode_ctl;
	
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
    err_flag = pm_comm_write_byte_mask(gpio_ptr->comm_ptr->slave_id, mode_ctl, 0x0E, (pm_register_data_type)(source<<1), 0 ) ;

    return err_flag;
}

pm_err_flag_type pm_gpio_set_mux_ctrl(unsigned pmic_chip, pm_gpio_uart_path_type uart_path)
{
    (void)uart_path;

    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
}

pm_err_flag_type pm_gpio_status_get(unsigned pmic_chip, pm_gpio_perph_index gpio, pm_gpio_status_type  *gpio_status)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type reg_data = 0;
    pm_register_data_type reg_data2 = 0;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);
    pm_register_address_type periph_index = (pm_register_address_type)gpio;
	
    pm_register_address_type base_address;
    pm_register_address_type mode_ctl;
    pm_register_address_type dig_vin_ctl;
    pm_register_address_type dig_pull_ctl;
    pm_register_address_type dig_in_ctl;
    pm_register_address_type dig_out_ctl;
    pm_register_address_type en_ctl;

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

    base_address =  gpio_ptr->gpio_reg_table->base_address + (gpio_ptr->gpio_reg_table->peripheral_offset*periph_index);	
    mode_ctl     =  base_address + gpio_ptr->gpio_reg_table->mode_ctl;
    dig_vin_ctl  =  base_address + gpio_ptr->gpio_reg_table->dig_vin_ctl;
    dig_pull_ctl =  base_address + gpio_ptr->gpio_reg_table->dig_pull_ctl;
    dig_in_ctl   =  base_address + gpio_ptr->gpio_reg_table->dig_in_ctl;
    dig_out_ctl  =  base_address + gpio_ptr->gpio_reg_table->dig_out_ctl;
    en_ctl       =  base_address + gpio_ptr->gpio_reg_table->en_ctl;
	
    // Read current source pulls
    err_flag = pm_comm_read_byte(gpio_ptr->comm_ptr->slave_id, dig_pull_ctl, &reg_data, 0 );
    if(PM_ERR_FLAG__SUCCESS == err_flag)
    {
        reg_data = reg_data & 0x07;
        gpio_status->gpio_current_src_pulls = (pm_gpio_current_src_pulls_type)reg_data;
    }

    // Read voltage source
    err_flag = pm_comm_read_byte(gpio_ptr->comm_ptr->slave_id, dig_vin_ctl, &reg_data, 0 );
    if(PM_ERR_FLAG__SUCCESS == err_flag)
    {
        reg_data = reg_data & 0x07;
        gpio_status->gpio_volt_source = (pm_gpio_volt_src_type)reg_data;
    }

    // Read output buffer config Read output buffer strength
    err_flag = pm_comm_read_byte(gpio_ptr->comm_ptr->slave_id,dig_out_ctl, &reg_data, 0 );
    if(PM_ERR_FLAG__SUCCESS == err_flag)
    {
        reg_data2 = reg_data & 0x30;
        gpio_status->gpio_out_buffer_config = (pm_gpio_out_buffer_config_type)(reg_data2>>4);
        reg_data = reg_data & 0x03;
        gpio_status->gpio_out_buffer_drv_strength = (pm_gpio_out_buffer_drv_strength_type)reg_data;
    }

    // Read inversion status and source config
    err_flag = pm_comm_read_byte(gpio_ptr->comm_ptr->slave_id, mode_ctl, &reg_data, 0 );
    if(PM_ERR_FLAG__SUCCESS == err_flag)
    {
        gpio_status->gpio_src_config = (pm_gpio_src_config_type)((reg_data & 0x0E)>>1);
        gpio_status->gpio_invert_ext_pin_config = (pm_gpio_invert_ext_pin_type)(reg_data & 0x01);
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
    err_flag = pm_comm_read_byte(gpio_ptr->comm_ptr->slave_id, dig_in_ctl, &reg_data, 0 );
    if(PM_ERR_FLAG__SUCCESS == err_flag)
    {
        reg_data = reg_data & 0x0F;
        gpio_status->gpio_dtest_buffer_on_off = (reg_data==0)? PM_OFF: PM_ON;
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
    err_flag = pm_comm_read_byte(gpio_ptr->comm_ptr->slave_id, en_ctl, &reg_data, 0 );
    if(PM_ERR_FLAG__SUCCESS == err_flag)
    {
        reg_data = reg_data & 0x80;
        gpio_status->gpio_ext_pin_config = (pm_gpio_ext_pin_config_type)(reg_data>>7);
    }

    return err_flag;
}

pm_err_flag_type pm_gpio_configure_type(unsigned pmic_chip, uint8  gpio, pm_drv_gpio_config_type type)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);
    pm_register_address_type periph_index = (pm_register_address_type)gpio;
    pm_register_address_type mode_ctl;

	if (gpio_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
	
    mode_ctl = gpio_ptr->gpio_reg_table->base_address + (gpio_ptr->gpio_reg_table->peripheral_offset*periph_index) + gpio_ptr->gpio_reg_table->mode_ctl;
	
    // Set GPIO
    //GPIO Mode:
    //- 0: Digital Input
    //- 1: Digital Output
    //- 2: Digital Input and Output
    //- 3: Reserved
    err_flag = pm_comm_write_byte_mask(gpio_ptr->comm_ptr->slave_id, mode_ctl , 0x70, (pm_register_data_type)(type<<4), 0);

    return err_flag;
}

pm_err_flag_type pm_gpio_enable(unsigned pmic_chip, uint8  gpio, boolean enable)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_gpio_data_type *gpio_ptr = pm_gpio_get_data(pmic_chip);
    pm_register_address_type periph_index = (pm_register_address_type)gpio;
    pm_register_address_type en_ctl;
    pm_register_data_type data = 0;

	if (gpio_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
	
    en_ctl = gpio_ptr->gpio_reg_table->base_address + (gpio_ptr->gpio_reg_table->peripheral_offset*periph_index) + gpio_ptr->gpio_reg_table->en_ctl;
			  
    if(TRUE == enable)
    {
        data = 0x80;
    }
    // Enable GPIO
    //GPIO Master Enable
    //0 = puts GPIO_PAD at high Z and disables the block
    //1 = GPIO is enabled
    err_flag = pm_comm_write_byte_mask(gpio_ptr->comm_ptr->slave_id, en_ctl , 0x80, data, 0);

    return err_flag;
}

