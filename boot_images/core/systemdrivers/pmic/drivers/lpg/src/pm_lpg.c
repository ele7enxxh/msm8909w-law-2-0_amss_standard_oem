
/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/lpg/src/pm_lpg.c#1 $ 


when       who    what, where, why
--------   ---    ---------------------------------------------------------- 
07/16/14   akm     Comm change Updates
03/31/14   akm    Updated to the latest PMIC driver architecture   
03/04/13   aab    Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

#include "pm_resource_manager.h"
#include "pm_lpg.h"
#include "pm_lpg_driver.h"
#include "pm_comm.h"


pm_err_flag_type pm_lpg_pwm_enable(unsigned pmic_chip, pm_lpg_chan_type pm_lpg_chan, boolean enable )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type reg = 0;
    pm_register_data_type data = 0;
    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);

    if (lpg_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
  
    if((pm_lpg_chan >= PM_LPG_CHAN_INVALID)||(pm_lpg_chan == PM_LPG_CHAN_NONE))
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    reg = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->enable_control;
    if (enable)
    {
        data = 0xE4; // enabling both EN_PWM_HI EN_PWM_LO, Enable output and set PWM_SRC_SELECT to PWM Register
    }
    err_flag = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, reg, 0xE4, (pm_register_data_type)(data), 0); 

    return err_flag;
}

pm_err_flag_type pm_lpg_pwm_output_enable( unsigned pmic_chip, pm_lpg_chan_type pm_lpg_chan,boolean enable )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type reg = 0;
    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);

    if (lpg_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if((pm_lpg_chan >= PM_LPG_CHAN_INVALID)||(pm_lpg_chan == PM_LPG_CHAN_NONE))
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
		
    reg = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->enable_control;
    err_flag = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, reg, 0x20, (pm_register_data_type)(enable<<5), 0); 

    return err_flag;
}


pm_err_flag_type pm_lpg_pwm_set_pwm_value( unsigned pmic_chip, pm_lpg_chan_type pm_lpg_chan,uint16 pwm_value )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type reg_lsb = 0;
    pm_register_address_type reg_msb = 0;
    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);

    if (lpg_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if((pm_lpg_chan >= PM_LPG_CHAN_INVALID)||(pm_lpg_chan == PM_LPG_CHAN_NONE))
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    reg_lsb = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->pwm_value_lsb;
    reg_msb = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->pwm_value_msb;

    if (pwm_value> 0x1FF)
    {
        pwm_value = 0x1FF;
    }

    err_flag  = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, reg_lsb, 0xFF, (pm_register_data_type)(pwm_value), 0); 
    err_flag |= pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, reg_msb, 0x01, (pm_register_data_type)(pwm_value>>8), 0); 

    return err_flag;
}

pm_err_flag_type pm_lpg_pwm_set_pre_divide(unsigned pmic_chip, pm_lpg_chan_type pm_lpg_chan, pm_lpg_pwm_pre_divide_type pre_div, pm_lpg_pwm_freq_expo_type exp )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type reg = 0;
    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;

    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);
    if (lpg_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if((pm_lpg_chan >= PM_LPG_CHAN_INVALID)||(pm_lpg_chan == PM_LPG_CHAN_NONE))
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    reg = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->lpg_pwm_freq_prediv_clk;

    err_flag = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, reg, 0x67, (pm_register_data_type)((pre_div<<5)|exp), 0); 

    return err_flag;
}

pm_err_flag_type pm_lpg_pwm_clock_sel( unsigned pmic_chip, pm_lpg_chan_type lpg_chan, pm_lpg_pwm_clock_type clock )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type reg = 0;
    pm_register_address_type periph_index = (pm_register_address_type)lpg_chan;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);

    if (lpg_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if((lpg_chan >= PM_LPG_CHAN_INVALID)||(lpg_chan == PM_LPG_CHAN_NONE))
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    reg = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->lpg_pwm_size_clk;
    err_flag = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, reg, 0x03, (pm_register_data_type)(clock), 0); 

    return err_flag;
}


pm_err_flag_type pm_lpg_set_pwm_bit_size( unsigned pmic_chip, pm_lpg_chan_type pm_lpg_chan,pm_lpg_pwm_size_type bit_size )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);
    pm_register_address_type pwm_size = 0;
    pm_register_address_type pwm_sync = 0;
    pm_register_data_type data=0;

    if (lpg_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if((pm_lpg_chan >= PM_LPG_CHAN_INVALID)||(pm_lpg_chan == PM_LPG_CHAN_NONE))
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    pwm_size = lpg_ptr->lpg_reg_table->base_address + (periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->lpg_pwm_size_clk;
    pwm_sync = lpg_ptr->lpg_reg_table->base_address + (periph_index * lpg_ptr->lpg_reg_table->peripheral_offset) + lpg_ptr->lpg_reg_table->pwm_sync;

    /*LPG lite supports 6 and 9 bit PWM and reg bit position is 4. LPG legacy supports 6,7,8 and 9 bit PWM and reg bit position is 4,5*/
    data = (bit_size==PM_LPG_PWM_6BIT) ? 0x0 : 1;
    err_flag = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, pwm_size, 0x10, data, 0);
    err_flag |= pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, pwm_sync, 0x1, 0x1, 0);

    return err_flag;
}



pm_err_flag_type pm_lpg_pwm_src_select (unsigned pmic_chip, pm_lpg_chan_type pm_lpg_chan,pm_lpg_src_type lpg_src_type )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);
    pm_register_address_type reg = 0;
    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;

    if (lpg_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if((pm_lpg_chan >= PM_LPG_CHAN_INVALID)||(pm_lpg_chan == PM_LPG_CHAN_NONE))
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    else if(lpg_src_type >= PM_LPG_PWM_SRC_INVALID)
    {
        return PM_ERR_FLAG__PAR3_OUT_OF_RANGE;
    }
		
    reg = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->enable_control;

    err_flag = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, reg, 0x04, (((pm_register_data_type)lpg_src_type)<<2), 0); 

    return err_flag;
}



pm_err_flag_type pm_lpg_config_pwm_type(unsigned pmic_chip, pm_lpg_chan_type pm_lpg_chan, boolean glitch_removal, boolean full_scale, boolean en_phase_stagger, pm_lpg_phase_stag_shift_type phase_stagger)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
	pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);
    pm_register_address_type reg = 0;
    pm_register_data_type data;
    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;

    if (lpg_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if((pm_lpg_chan >= PM_LPG_CHAN_INVALID)||(pm_lpg_chan == PM_LPG_CHAN_NONE))
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    if (phase_stagger >= PM_LPG_PHASE_STAG_SHIFT_INVALID)
    {
        err_flag = PM_ERR_FLAG__PAR6_OUT_OF_RANGE;
    }
    else
    {
        reg = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->lpg_pwm_type_config;

        data = (glitch_removal<<5) | (full_scale << 3) | (en_phase_stagger <<2) | (uint8)phase_stagger;
        err_flag = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, reg, 0x2F, (pm_register_data_type)(data), 0); 
    }

    return err_flag;
}




pm_err_flag_type pm_lpg_pattern_config(unsigned pmic_chip, pm_lpg_chan_type pm_lpg_chan, boolean ramp_direction, boolean pattern_repeat, boolean ramp_toggle, boolean en_pause_hi, boolean en_pause_lo)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);
    pm_register_address_type reg = 0;
    pm_register_data_type data;
    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;

    if (lpg_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if((pm_lpg_chan >= PM_LPG_CHAN_INVALID)||(pm_lpg_chan == PM_LPG_CHAN_NONE))
    {
       return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
		

    reg = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->lpg_pattern_config;

    data = (ramp_direction <<4)| (pattern_repeat<<3) | (ramp_toggle <<2) |(en_pause_hi<<1) | en_pause_lo ;
    err_flag = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, reg, 0x1F, (pm_register_data_type)(data), 0); 

    return err_flag;
}



pm_err_flag_type pm_lpg_lut_config_set(unsigned pmic_chip, int lut_index, int pwm_value)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);
    pm_register_address_type reg_lut_lsb = 0;
    pm_register_address_type reg_lut_msb = 0;
    pm_register_address_type periph_index = (pm_register_address_type)2;

    if (lpg_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    else
    {
        reg_lut_lsb = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->lut_lsb;
        reg_lut_lsb = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->lut_msb;

        err_flag = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, reg_lut_lsb, 0xFF, (pm_register_data_type)(pwm_value), 0); 
        err_flag = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, reg_lut_msb, 0x01, (pm_register_data_type)(pwm_value >> 8), 0); 
    }

    return err_flag;
}



pm_err_flag_type pm_lpg_lut_config_get(unsigned pmic_chip, int lut_index, int* pwm_value)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);
    pm_register_address_type reg_lut_lsb = 0;
    pm_register_address_type reg_lut_msb = 0;
    pm_register_data_type pwm_value_lsb, pwm_value_msb;
    pm_register_address_type periph_index = (pm_register_address_type)2;


    if (lpg_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    else
    {
        reg_lut_lsb = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->lut_lsb;
        reg_lut_msb = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->lut_msb;

        err_flag  = pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, reg_lut_lsb, 0xFF, &pwm_value_lsb,0); 
        err_flag |= pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, reg_lut_msb, 0x01, &pwm_value_msb,0); 
    }

    return err_flag;
}

pm_err_flag_type pm_lpg_lut_config_set_array(unsigned pmic_chip, int start_index, int *value, int count)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);

    int end_index = start_index + count - 1;
    int i_count=0;
    pm_register_address_type reg_lut_lsb;
    pm_register_address_type reg_lut_msb;

    pm_register_address_type periph_index = (pm_register_address_type)2; 

    if (lpg_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    else
    {
        while (end_index >= start_index)
        {
            reg_lut_lsb = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->lut_lsb;
            reg_lut_msb = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->lut_msb;

            /*if value is greater than 9bits then round it to 9bits max value*/
            *(value+i_count) = (*(value+i_count) > 0x1FF)?0x1FF : (*(value+i_count));

            err_flag  = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, reg_lut_lsb, 0xFF, (pm_register_data_type)(*(value+i_count)), 0); 
            err_flag |= pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, reg_lut_msb, 0xFF, (pm_register_data_type)((*(value+i_count))>>8), 0); 

            start_index++;
            i_count++;
        }

    }

    return err_flag;
}


pm_err_flag_type pm_lpg_lut_config_get_array(unsigned pmic_chip, int start_index, int *value, int count)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);
    pm_register_address_type reg_lut_lsb;
    pm_register_address_type reg_lut_msb;
    pm_register_data_type pwm_value_lsb;
    pm_register_data_type pwm_value_msb;
    int i_count=0;
    int end_index = start_index + count - 1;
    pm_register_address_type periph_index = (pm_register_address_type)2;

    if (lpg_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    else
    {
        while (end_index >= start_index)
        {
            reg_lut_lsb = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->lut_lsb;
            reg_lut_msb = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->lut_msb;

            err_flag  = pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, reg_lut_lsb, 0xFF, &pwm_value_lsb,0);
            err_flag |= pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, reg_lut_msb, 0xFF, &pwm_value_msb,0);

            *(value+i_count) = (pwm_value_msb <<8) | pwm_value_lsb;
            start_index++;
            i_count++;
        }
    }

    return err_flag;
}


pm_err_flag_type pm_lpg_pwm_ramp_generator_start(unsigned pmic_chip, pm_lpg_chan_type pm_lpg_chan)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);
    pm_register_address_type reg = 0;
    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;
    

    if (lpg_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if((pm_lpg_chan >= PM_LPG_CHAN_INVALID)||(pm_lpg_chan == PM_LPG_CHAN_NONE))
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }


    reg = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->ramp_control;

    /*
     * The ramp control fields are zero-indexed.
     */
    pm_lpg_chan--;
    err_flag = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, reg, 0xFF, (pm_register_data_type)(1<<pm_lpg_chan), 0); 

    return err_flag;
}


pm_err_flag_type pm_lpg_pwm_lut_index_set(unsigned pmic_chip, pm_lpg_chan_type pm_lpg_chan, int low_index, int high_index)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);
    pm_register_address_type reg_hi_index;
    pm_register_address_type reg_lo_index;
    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;

    if (lpg_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if((pm_lpg_chan >= PM_LPG_CHAN_INVALID)||(pm_lpg_chan == PM_LPG_CHAN_NONE))
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    if ((low_index>0x03F) || (low_index<0))
    {
        err_flag = PM_ERR_FLAG__PAR3_OUT_OF_RANGE;        
    }
    else if ((high_index>0x03F) || (high_index<0))
    {
        err_flag = PM_ERR_FLAG__PAR4_OUT_OF_RANGE;        
    }
    else
    {
        reg_hi_index = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->hi_index;
        reg_lo_index = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->lo_index;

        err_flag  = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, reg_hi_index, 0x3F, (pm_register_data_type)high_index, 0);
        err_flag |= pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, reg_lo_index, 0x3F, (pm_register_data_type)low_index, 0);
    }

    return err_flag;
}


pm_err_flag_type pm_lpg_config_pause_time(unsigned pmic_chip, pm_lpg_chan_type pm_lpg_chan, uint32 ramp_count, uint32 hi_multiplier, uint32 low_multiplier)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);

    pm_register_address_type ramp_step_duration_msb;
    pm_register_address_type ramp_step_duration_lsb;

    pm_register_address_type pause_hi_multiplier_lsb;
    pm_register_address_type pause_hi_multiplier_msb;

    pm_register_address_type pause_lo_multiplier_lsb;
    pm_register_address_type pause_lo_multiplier_msb;

    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;

    if (lpg_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if((pm_lpg_chan >= PM_LPG_CHAN_INVALID)||(pm_lpg_chan == PM_LPG_CHAN_NONE))
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    ramp_step_duration_lsb  = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->ramp_step_duration_lsb;
    ramp_step_duration_msb  = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->ramp_step_duration_msb;
    pause_hi_multiplier_lsb = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->pause_hi_multiplier_lsb;
    pause_hi_multiplier_msb = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->pause_hi_multiplier_msb;
    pause_lo_multiplier_lsb = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->pause_lo_multiplier_lsb;
    pause_lo_multiplier_msb = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->pause_lo_multiplier_msb;

    ramp_count = (ramp_count > 0x1FF)? 0x1FF : ramp_count;

    hi_multiplier = (hi_multiplier) ? hi_multiplier : 1; //if hi_multiplier == 0 then make it 1 to decrement in the following step
    hi_multiplier = (hi_multiplier > 0xFFF)? 0xFFF : (hi_multiplier -1);  //pause time = ramp * (multiplier - 1)

    low_multiplier = (low_multiplier) ? low_multiplier : 1; //if low_multiplier == 0 then make it 1 to decrement in the following step
    low_multiplier = (low_multiplier > 0xFFF)? 0xFFF : (low_multiplier -1);

    err_flag  = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, ramp_step_duration_lsb, 0xFF, (pm_register_data_type)(ramp_count & 0xFF), 0); 
    err_flag |= pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, ramp_step_duration_msb, 0x01, (pm_register_data_type)(ramp_count >> 8), 0); 

    err_flag |= pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, pause_hi_multiplier_lsb, 0xFF, (pm_register_data_type)(ramp_count & 0xFF), 0); 
    err_flag |= pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, pause_hi_multiplier_msb, 0x0F, (pm_register_data_type)(ramp_count >> 8), 0); 

    err_flag |= pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, pause_lo_multiplier_lsb, 0xFF, (pm_register_data_type)(ramp_count & 0xFF), 0); 
    err_flag |= pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, pause_lo_multiplier_msb, 0x0F, (pm_register_data_type)(ramp_count >> 8), 0); 


    return err_flag;
}


pm_err_flag_type pm_lpg_pwm_ramp_generator_enable(unsigned pmic_chip, pm_lpg_chan_type pm_lpg_chan, boolean enable)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);
    pm_register_address_type reg = 0;
    pm_register_data_type data;
    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;

    if (lpg_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if((pm_lpg_chan >= PM_LPG_CHAN_INVALID)||(pm_lpg_chan == PM_LPG_CHAN_NONE))
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    reg = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->enable_control;
    data = (enable) ? 0xFF : 0x00;
    err_flag = pm_comm_write_byte_mask(lpg_ptr->comm_ptr->slave_id, reg, 0x02, (pm_register_data_type)(data), 0); 

    return err_flag;
}


pm_err_flag_type pm_lpg_get_status(unsigned pmic_chip, pm_lpg_chan_type pm_lpg_chan, pm_lpg_status_type *lpg_status)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_lpg_data_type *lpg_ptr = pm_lpg_get_data(pmic_chip);
    pm_register_address_type periph_index = (pm_register_address_type)pm_lpg_chan;

    pm_register_data_type data, data1;
    pm_register_address_type pwm_value_lsb, pwm_value_msb, lpg_pwm_freq_prediv_clk,enable_control,lpg_pwm_size_clk;
    pm_register_address_type lpg_pwm_type_config, lpg_pattern_config, hi_index, lo_index,ramp_step_duration_msb,ramp_step_duration_lsb;

    if (lpg_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if((pm_lpg_chan >= PM_LPG_CHAN_INVALID)||(pm_lpg_chan == PM_LPG_CHAN_NONE))
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    enable_control = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->enable_control;
    err_flag  = pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, enable_control, 0xFF, &data,0);
    if (err_flag)
    {
        return err_flag;  //PM_ERR_FLAG__SPMI_OPT_ERR
    }
    lpg_status->pwm_output = (data & 0x20)? TRUE : FALSE;
    lpg_status->lpg_src_type=(data & 0x04)?PM_LPG_PWM_SRC_PWM_REGISTER:PM_LPG_PWM_SRC_LUT;

    pwm_value_lsb = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->pwm_value_lsb;
    pwm_value_msb = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->pwm_value_msb;    
    err_flag  = pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, pwm_value_msb, 0x01, &data1,0);
    if (err_flag)
    {
        return err_flag;
    }
    err_flag  = pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, pwm_value_lsb, 0xFF, &data,0);
    if (err_flag)
    {
        return err_flag;
    }
    lpg_status->pwm_value= (data1<<8)|data;
    lpg_pwm_freq_prediv_clk = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->lpg_pwm_freq_prediv_clk;
    err_flag  = pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, lpg_pwm_freq_prediv_clk, 0xFF, &data,0);
    if (err_flag)
    {
        return err_flag;
    }
    lpg_status->exp =(pm_lpg_pwm_freq_expo_type)(data & 0x07);
    lpg_status->pre_div = (pm_lpg_pwm_pre_divide_type)((data & 0x60)>>5);
    lpg_pwm_size_clk = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->lpg_pwm_size_clk;
    err_flag  = pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, lpg_pwm_size_clk, 0xFF, &data,0);
    if (err_flag)
    {
        return err_flag;
    }
    lpg_status->pwm_freq_clk = (pm_lpg_pwm_clock_type)(data & 0x03);
    lpg_status->pwm_bit_size = (pm_lpg_pwm_size_type)((data & 0x30)>>4);

    lpg_pwm_type_config = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->lpg_pwm_type_config;
    err_flag  = pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, lpg_pwm_type_config, 0xFF, &data,0);
    if (err_flag)
    {
        return err_flag;
    }
    lpg_status->glitch_removal=(data & 0x20)?TRUE:FALSE;
    lpg_status->full_scale=(data & 0x08)?TRUE:FALSE;
    lpg_status->en_phase_stagger=(data & 0x04)?TRUE:FALSE;
    lpg_status->phase_stagger=(pm_lpg_phase_stag_shift_type)(data & 0x03);

    lpg_pattern_config = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->lpg_pattern_config;    
    err_flag  = pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, lpg_pattern_config, 0xFF, &data,0);
    if (err_flag)
    {
        return err_flag;
    }
    lpg_status->ramp_direction=(data & 0x10)?TRUE:FALSE;
    lpg_status->pattern_repeat=(data & 0x08)?TRUE:FALSE;
    lpg_status->ramp_toggle=(data & 0x04)?TRUE:FALSE;
    lpg_status->en_pause_hi=(data & 0x02)?TRUE:FALSE;
    lpg_status->en_pause_lo=(data & 0x01)?TRUE:FALSE;
    hi_index = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->hi_index;
    lo_index = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->lo_index;
    err_flag  = pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, hi_index, 0x3F, &data,0);
    if (err_flag)
    {
        return err_flag;
    }
    lpg_status->lut_high_index = data&0x3F;

    err_flag  = pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, lo_index, 0x3F, &data,0);
    if (err_flag)
    {
        return err_flag;
    }
    lpg_status->lut_low_index = data&0x3F;

    ramp_step_duration_msb = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->ramp_step_duration_msb;
    ramp_step_duration_lsb = lpg_ptr->lpg_reg_table->base_address + (lpg_ptr->lpg_reg_table->peripheral_offset*periph_index) + lpg_ptr->lpg_reg_table->ramp_step_duration_lsb;
    err_flag  = pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, ramp_step_duration_msb, 0x01, &data1,0);
    if (err_flag)
    {
        return err_flag;
    }
    err_flag  = pm_comm_read_byte_mask(lpg_ptr->comm_ptr->slave_id, ramp_step_duration_lsb, 0xFF, &data,0);
    if (err_flag)
    {
        return err_flag;
    }
    lpg_status->interval_count= (data1<<8)|data;

    return err_flag;
}

