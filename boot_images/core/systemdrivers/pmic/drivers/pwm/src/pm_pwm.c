/*! \file
*  
*  \brief  pm_pwm.c ---PWM Driver implementation.
*  \details PWM Driver implementation.
*  &copy; Copyright (c) 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/pwm/src/pm_pwm.c#1 $ 


when       	who     what, where, why
--------   	---    	---------------------------------------------------------- 
08/20/14        al     Updating comm lib 
05/20/14        al      Architecture update 
10/16/12        al	added LUT support 
07/23/12        al      Created.

===========================================================================*/

/*===========================================================================

INCLUDE FILES 

===========================================================================*/
#include "pm_pwm.h"
#include "pm_pwm_driver.h"

/*************************************
NOTE: VIRTUAL METHOD IMPLEMENTATION
**************************************/

pm_err_flag_type pm_pwm_enable_pwm(uint32 pmic_chip, boolean enable )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data = 0x00;
    pm_register_address_type enable_control;

    pm_pwm_data_type *pwm_ptr = pm_pwm_get_data(pmic_chip);
    if (pwm_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        enable_control = pwm_ptr->pwm_reg_table->base_address +  pwm_ptr->pwm_reg_table->enable_control;
        data = (enable)? 0x80 : 0x00;
        err_flag = pm_comm_write_byte_mask(pwm_ptr->comm_ptr->slave_id, enable_control, 0x80, data, 0);
    }

    return err_flag;
}

pm_err_flag_type pm_pwm_set_pwm_value(uint32 pmic_chip, uint16 pwm_value )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type reg;
    pm_pwm_data_type *pwm_ptr = pm_pwm_get_data(pmic_chip);

    if (pwm_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        if (pwm_value > 0x1FF ) //rounding up value
        {
           pwm_value = 0x1FF;
        }
        reg = pwm_ptr->pwm_reg_table->base_address +  pwm_ptr->pwm_reg_table->pwm_value_lsb;
        err_flag = pm_comm_write_byte_mask(pwm_ptr->comm_ptr->slave_id, reg, 0xFF, (pm_register_data_type)pwm_value, 0);
        reg = pwm_ptr->pwm_reg_table->base_address +  pwm_ptr->pwm_reg_table->pwm_value_msb;
        err_flag |= pm_comm_write_byte_mask(pwm_ptr->comm_ptr->slave_id, reg, 0x01,(pm_register_data_type)(pwm_value>>8), 0);
        reg = pwm_ptr->pwm_reg_table->base_address +  pwm_ptr->pwm_reg_table->pwm_sync;
        //Writing 1 to pwm_sync register will update the 6/9-bit PWM value. This bit is auto-cleared
        err_flag |= pm_comm_write_byte_mask(pwm_ptr->comm_ptr->slave_id, reg, 0x01,(pm_register_data_type)0x01, 0);
    }

    return err_flag;
} 

pm_err_flag_type pm_pwm_clock_config(uint32 pmic_chip, pm_pwm_bit_size_type bit_size,  pm_pwm_clock_freq_type clk_freq, uint32 pre_div, uint32 exp )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type reg;
    
    pm_pwm_data_type *pwm_ptr = pm_pwm_get_data(pmic_chip);

    if (pwm_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(bit_size >= PM_PWM_SIZE__MAX)
    {
        err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    else if(clk_freq >= PM_PWM_CLK__MAX)
    {
        err_flag = PM_ERR_FLAG__PAR3_OUT_OF_RANGE;
    }
    else if(pre_div > 6)
    {
        err_flag = PM_ERR_FLAG__PAR4_OUT_OF_RANGE;
    }
    else if(exp >= 8)
    {
        err_flag = PM_ERR_FLAG__PAR5_OUT_OF_RANGE;
    }
    else
    {
        pre_div = pre_div/2; //0 = PRE_DIV__1, 1 = PRE_DIV__3, 2 = PRE_DIV__5, 3 = PRE_DIV__6

        reg = pwm_ptr->pwm_reg_table->base_address +  pwm_ptr->pwm_reg_table->pwm_size_clk;
        err_flag = pm_comm_write_byte_mask(pwm_ptr->comm_ptr->slave_id, reg, 0x07, (pm_register_data_type)((bit_size<<2)|clk_freq), 0);
        reg = pwm_ptr->pwm_reg_table->base_address +  pwm_ptr->pwm_reg_table->pwm_freq_prediv_clk;
        err_flag |= pm_comm_write_byte_mask(pwm_ptr->comm_ptr->slave_id, reg, 0x67,(pm_register_data_type)((pre_div<<5)|exp), 0);

    }
    return err_flag;
}

pm_err_flag_type pm_pwm_glitch_removal(uint32 pmic_chip, boolean glitch_removal )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data = 0x00;
    pm_register_address_type pwm_type_config;

    pm_pwm_data_type *pwm_ptr = pm_pwm_get_data(pmic_chip);
    
    if (pwm_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pwm_type_config = pwm_ptr->pwm_reg_table->base_address +  pwm_ptr->pwm_reg_table->pwm_type_config;

        data = (glitch_removal) ? 0xFF : 0x00;

        err_flag = pm_comm_write_byte_mask(pwm_ptr->comm_ptr->slave_id, pwm_type_config, 0x20, data, 0);
    }

    return err_flag;
}

pm_err_flag_type pm_pwm_get_status (uint32 pmic_chip, pm_pwm_status_type *pwm_status )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data;
    pm_register_address_type reg;

    pm_pwm_data_type *pwm_ptr = pm_pwm_get_data(pmic_chip);
    
    if (pwm_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    reg = pwm_ptr->pwm_reg_table->base_address +  pwm_ptr->pwm_reg_table->enable_control;
    err_flag =  pm_comm_read_byte_mask(pwm_ptr->comm_ptr->slave_id, reg, 0x80, &data, 0);
    pwm_status->pwm_enable = (data&0x80)?TRUE:FALSE;

    reg = pwm_ptr->pwm_reg_table->base_address +  pwm_ptr->pwm_reg_table->pwm_value_lsb;
    err_flag =  pm_comm_read_byte_mask(pwm_ptr->comm_ptr->slave_id, reg, 0xFF, &data, 0);
    pwm_status->pwm_value = data; //writing LSB for duty cycle

    reg = pwm_ptr->pwm_reg_table->base_address +  pwm_ptr->pwm_reg_table->pwm_value_msb;
    err_flag |= pm_comm_read_byte_mask(pwm_ptr->comm_ptr->slave_id, reg, 0x01, &data, 0);
    pwm_status->pwm_value |= (data<<8); //MSB for duty cycle

    reg = pwm_ptr->pwm_reg_table->base_address +  pwm_ptr->pwm_reg_table->pwm_size_clk;
    err_flag =  pm_comm_read_byte_mask(pwm_ptr->comm_ptr->slave_id, reg, 0x07, &data, 0);
    pwm_status->clk_freq=(pm_pwm_clock_freq_type)(data & 0x03);

    reg = pwm_ptr->pwm_reg_table->base_address +  pwm_ptr->pwm_reg_table->pwm_freq_prediv_clk;
    pwm_status->bit_size=(data & 0x04)? PM_PWM_SIZE__9BIT:PM_PWM_SIZE__6BIT;
    err_flag |= pm_comm_read_byte_mask(pwm_ptr->comm_ptr->slave_id, reg, 0x67,&data, 0);
    pwm_status->exp = (data&0x07);
    if(((data&0x60)>>5)==3)
    {
        pwm_status->pre_div = ((data&0x60)>>5) *2;
    }
    else
    {
        pwm_status->pre_div = (((data&0x60)>>5) *2) + 1;
    }

    reg = pwm_ptr->pwm_reg_table->base_address +  pwm_ptr->pwm_reg_table->pwm_type_config;
    err_flag =  pm_comm_read_byte_mask(pwm_ptr->comm_ptr->slave_id, reg, 0x20, &data, 0);
    pwm_status->glitch_removal=(data)?TRUE:FALSE;

    return err_flag;
}

