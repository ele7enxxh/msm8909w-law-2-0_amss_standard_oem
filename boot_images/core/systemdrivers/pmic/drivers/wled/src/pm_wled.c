/*! \file
*  
*  \brief  pm_wled.c ---WLED Driver implementation.
*  \details WLED Driver implementation.
*  &copy; Copyright 2012-2014 QUALCOMM Technologies Inc, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/wled/src/pm_wled.c#1 $ 


when       	who     	 what, where, why
--------   	---    	     ---------------------------------------------------------- 
09/18/14    al          AMOLED data bitwhere, why
--------    ---          ---------------------------------------------------------- ?
09/22/14    al          PSM shift fix 
08/20/14    al          Updating comm lib 
08/29/14    al          KW fixes
08/12/14    al          Mask and data fix
05/20/14    al          Architecture update 
06/06/14    al          Fixed mask and data for sink write
05/19/14    al          AMOLED, PSM and PFM support 
03/26/14    al          8994 WLED support 
03/15/13    al          Rewriting according to new architecture 
02/11/13    al          Removing warning
01/14/13    al          Replacing enums with uint32   
11/30/12    al          Accommodating SWITCHING FREQUENCY change in pmic version 3  
11/30/12    al          Fixed vref control and enable modulator 
10/10/12    al          impelemented sleepb and softstart api
08/27/12    al          Removed necessity for additional API call after setting brightness
08/27/12    al          made changes for update condition
08/09/12    al          added api for brightness
08/09/12    al          updated with latest offtarget
07/24/12    al          New file

===========================================================================*/

/*===========================================================================

INCLUDE FILES 

===========================================================================*/
#include "pm_wled.h"
#include "pm_wled_driver.h"


/*************************************
NOTE: VIRTUAL METHOD IMPLEMENTATION
**************************************/
static pm_err_flag_type pm_wled_sink_write(uint32 pmic_chip, pm_wled_led_type led_type, pm_register_address_type led1_reg_addr, pm_register_data_type data, pm_register_data_type mask);

pm_err_flag_type pm_wled_enable_module(uint32 pmic_chip, boolean enable)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type module_enable;
    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    module_enable = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->module_enable;

    err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, module_enable, 0x80, ((pm_register_data_type)enable) << 7, 0);

    return err_flag;
}

pm_err_flag_type pm_wled_enable_modulator(uint32 pmic_chip, pm_wled_led_type led_type)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type modulator_en1;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if(led_type >= PM_WLED_INVALID)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    modulator_en1 = wled_ptr->wled_reg_table->wled_sink_reg->base_address + wled_ptr->wled_reg_table->wled_sink_reg->led1_modulator_en;

    err_flag = pm_wled_sink_write(pmic_chip, led_type, modulator_en1, 0x80, 0x80);

    return err_flag;
}


pm_err_flag_type pm_wled_enable_current_sink(uint32 pmic_chip, pm_wled_led_type led_type)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type en_current_sink;
    pm_register_data_type mask, data;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if(led_type >= PM_WLED_INVALID)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    switch (led_type)
    {
    case PM_WLED_NONE:
        mask = 0xF0; data = 0x00; break;
    case PM_WLED_LED1:
        mask = 0x10; data = 0x10; break;
    case PM_WLED_LED2:
        mask = 0x20; data = 0x20; break;
    case PM_WLED_LED3:
        mask = 0x40; data = 0x40; break;
    case PM_WLED_LED4:
        mask = 0x80; data = 0x80; break;
    case PM_WLED_ALL:
        mask = 0xF0; data = 0xF0; break;
    default:
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    en_current_sink =  wled_ptr->wled_reg_table->wled_sink_reg->base_address + wled_ptr->wled_reg_table->wled_sink_reg->current_sink_en;

    err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, en_current_sink, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_wled_set_sync_delay(uint32 pmic_chip, pm_wled_led_type led_type, uint32 delay_usec)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type led1_idac_sync_delay;
    uint32 data;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

   
    if (led_type >= PM_WLED_INVALID)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    led1_idac_sync_delay = wled_ptr->wled_reg_table->wled_sink_reg->base_address + wled_ptr->wled_reg_table->wled_sink_reg->led1_idac_sync_delay;

    //rounding: if more than half then roof value else floor. 0 =0, 1=200, 2=400,....., 7=1400.
    data = (delay_usec > 1400) ? 7 : ((delay_usec + 100) / 200);

    err_flag = pm_wled_sink_write(pmic_chip, led_type, led1_idac_sync_delay, data, 0x07);

    return err_flag;
}

pm_err_flag_type pm_wled_set_current_sink(uint32 pmic_chip, pm_wled_led_type led_type, uint32 current_milliamp)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type led1_full_scale_current;

    pm_register_data_type data, mask;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if(led_type >= PM_WLED_INVALID)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    led1_full_scale_current = wled_ptr->wled_reg_table->wled_sink_reg->base_address + wled_ptr->wled_reg_table->wled_sink_reg->led1_full_scale_current;
    /*data = current_milliamp/2.5 ; current_milliamp range: 0 to 30*/
    data = (current_milliamp > 30) ? 12 : ((current_milliamp * 10) / 25);
    mask = 0x0F;

    err_flag = pm_wled_sink_write(pmic_chip, led_type, led1_full_scale_current, data, mask);

    return err_flag;
}

/************************************************************************/
/*  modulator_src_sel :  0 = use internally generated modulator signals for dimming
                         1 = use externally generated modulator signals (i.e LPG) for dimming                                                                   */
/************************************************************************/
pm_err_flag_type pm_wled_en_ext_pwm(uint32 pmic_chip, pm_wled_led_type led_type, boolean enable)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;

    pm_register_address_type led1_modulator_src_sel;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    
    if (led_type >= PM_WLED_INVALID)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    led1_modulator_src_sel = wled_ptr->wled_reg_table->wled_sink_reg->base_address + wled_ptr->wled_reg_table->wled_sink_reg->led1_modulator_src_sel;

    err_flag = pm_wled_sink_write(pmic_chip, led_type, led1_modulator_src_sel, enable, 0x01);

    return err_flag;
}

pm_err_flag_type pm_wled_set_slope_trim(uint32 pmic_chip, uint32 resource_index, uint32 trim_nfarad)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;

    pm_register_address_type slope_comp_cap;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if (trim_nfarad > 6000)
    {
        trim_nfarad = 6500;
    }
    else if (trim_nfarad < 3500)
    {
        trim_nfarad = 3000;
    }
    trim_nfarad = (trim_nfarad > 4500) ? (((6500 - trim_nfarad)/500) + 4) : ((4500 - trim_nfarad)/500);
    slope_comp_cap = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->slope_comp_cap;
    err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, slope_comp_cap, 0x07, (pm_register_data_type)trim_nfarad, 0);

    return err_flag;
}


pm_err_flag_type pm_wled_set_led_duty_cycle(uint32 pmic_chip, pm_wled_led_type led_type, uint16 data)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;

    pm_register_address_type led1_brightness_setting_lsb, led1_brightness_setting_msb, iled_sync_bit;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }    
    else if (led_type >= PM_WLED_INVALID)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    led1_brightness_setting_lsb = wled_ptr->wled_reg_table->wled_sink_reg->base_address + wled_ptr->wled_reg_table->wled_sink_reg->led1_brightness_setting_lsb;
    led1_brightness_setting_msb = wled_ptr->wled_reg_table->wled_sink_reg->base_address + wled_ptr->wled_reg_table->wled_sink_reg->led1_brightness_setting_msb;
    iled_sync_bit = wled_ptr->wled_reg_table->wled_sink_reg->base_address + wled_ptr->wled_reg_table->wled_sink_reg->iled_sync_bit;
 

    err_flag = pm_wled_sink_write(pmic_chip, led_type, led1_brightness_setting_lsb, (pm_register_data_type)data, 0xFF);
    err_flag = pm_wled_sink_write(pmic_chip, led_type, led1_brightness_setting_msb, (pm_register_data_type)(data >> 8), 0x0F);

    /*first write 0 and then 1*/
    err_flag |= pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, iled_sync_bit, 0x0F, 0x00, 0);  
    err_flag |= pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, iled_sync_bit, 0x0F, 0x0F, 0);  
    return err_flag;
}

pm_err_flag_type pm_wled_set_slope_comp(uint32 pmic_chip, uint32 resource_index, uint32 current_uamp)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type slope_comp_adj;

    pm_register_data_type data = 0;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    slope_comp_adj = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->slope_comp_adj;

    (void)resource_index;

    if (current_uamp <= 3)
    {
        data = 0;
    }
    if (current_uamp == 4)
    {
        data = 1; //both 1 and 2 correspond to 4uA slope compensation settings
    }
    if (current_uamp >= 5)
    {
        data = 3;
    }

    err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, slope_comp_adj, 0x03, data, 0);

    return err_flag;
}


pm_err_flag_type pm_wled_en_cabc(uint32 pmic_chip, pm_wled_led_type led_type)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;

    pm_register_address_type led1_cabc_en;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if(led_type >= PM_WLED_INVALID)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    led1_cabc_en = wled_ptr->wled_reg_table->wled_sink_reg->base_address + wled_ptr->wled_reg_table->wled_sink_reg->led1_cabc_en;

    err_flag = pm_wled_sink_write(pmic_chip, led_type, led1_cabc_en, 0x80, 0x80);

    return err_flag;
}


pm_err_flag_type pm_wled_en_sync(uint32 pmic_chip, pm_wled_led_type led_type, boolean enable)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;

    pm_register_address_type iled_sync_bit;
    pm_register_data_type mask, data;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if(led_type >= PM_WLED_INVALID)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    iled_sync_bit = wled_ptr->wled_reg_table->wled_sink_reg->base_address + wled_ptr->wled_reg_table->wled_sink_reg->iled_sync_bit;

    mask = (led_type == PM_WLED_ALL) ? 0x0F : (0x1 << led_type) >> 1;

    if (enable)
    {
        data = 0xFF;
        //making sure the transition will be 0 -> 1 , as required for update
        err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, iled_sync_bit, mask, 0x00, 0);
    }
    else
    {
        data = 0x00;
    }

    err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, iled_sync_bit, mask, data, 0);

    return err_flag;
}


/************************************************************************/
/*      SELECTS INTERNAL_MOD_RESOLUTION  PM_WLED_MODE_12b_SDM = 12 bit resolution
                                         PM_WLED_MODE_9b_PWM = 9 bit resolution*/                                                                 
/************************************************************************/

pm_err_flag_type pm_wled_config_sink_modulation(uint32 pmic_chip, 
                                                pm_wled_mode_clk_type mode_clk,
                                                boolean en_phase_stagger,
                                                pm_wled_mode_clk_type accum_clk,
                                                boolean en_hybrid_dim,
                                                pm_wled_pwm_mode_type pwm_mode, 
                                                pm_wled_mode_dim_mode_type dim_mode)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;

    pm_register_data_type data;
    pm_register_address_type modulation_scheme;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    data = (mode_clk << 6) | (en_phase_stagger << 5) | (accum_clk << 3) | (en_hybrid_dim << 2) | (pwm_mode << 1) | dim_mode;

    modulation_scheme = wled_ptr->wled_reg_table->wled_sink_reg->base_address + wled_ptr->wled_reg_table->wled_sink_reg->modulation_scheme;

    err_flag = pm_comm_write_byte(wled_ptr->comm_ptr->slave_id, modulation_scheme, data, 0);

    return err_flag;
}


pm_err_flag_type pm_wled_set_bst_max_duty(uint32 pmic_chip, uint32 resource_index, pm_wled_bst_max_duty_type bst_max_duty_type)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;

    pm_register_address_type boost_max_duty_cycle;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (bst_max_duty_type >= PM_WLED_BST_MAX_DUTY_SUB_INVALID)
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }

    boost_max_duty_cycle = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->boost_max_duty_cycle;
    err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, boost_max_duty_cycle, 0x03, (pm_register_data_type)bst_max_duty_type, 0);

    return err_flag;
}


pm_err_flag_type pm_wled_set_fsw_ctrl(uint32 pmic_chip, uint32 resource_index, uint32   fsw_khz)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;

    pm_register_data_type data;

    pm_register_address_type switching_frequency;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    
    switching_frequency = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->switching_frequency;
    
    if (fsw_khz > 9600)
        {
            fsw_khz = 9600;
        }
        else if (fsw_khz < 600)
        {
            fsw_khz = 300;
        }
        //FSW = 9600KHz/(data+1)
        data = (9600/fsw_khz)-1;

    err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, switching_frequency, 0x0F, data, 0);

    return err_flag;
}


pm_err_flag_type pm_wled_sel_warmup_dly(uint32 pmic_chip, uint32 resource_index, pm_wled_warmup_dly_type warmup_dly_type)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    pm_register_address_type bias_gen_warmup_delay;

    if (wled_ptr == NULL)
    {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (warmup_dly_type >= PM_WLED_WARMUP_DELAY_INVALID)
    {
        err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
    else
    {
        bias_gen_warmup_delay = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->bias_gen_warmup_delay;
        err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, bias_gen_warmup_delay, 0x03, (pm_register_data_type)warmup_dly_type, 0);
    }

    return err_flag;
}


pm_err_flag_type pm_wled_sel_ovp_thr(uint32 pmic_chip, uint32 resource_index, pm_wled_ovp_threshold_type ovp_threshold)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data;
    pm_register_address_type wled_ovp;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    wled_ovp = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->wled_ovp;


    if (ovp_threshold >= PM_WLED_OVP_THR_INVALID)
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
    if (resource_index >= ((pm_wled_data_type *)wled_ptr)->num_of_peripherals)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    data = (pm_register_data_type)ovp_threshold;

    err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, wled_ovp, 0x03, data, 0);

    return err_flag;
}


pm_err_flag_type pm_wled_set_ramp_time(uint32 pmic_chip, uint32 resource_index, uint32 time_usec)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type softstart_ramp_delay;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    pm_register_data_type data;

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    
    softstart_ramp_delay = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->softstart_ramp_delay;
    
    if (time_usec > 750)
    {
        time_usec = 750;
    }
    else if (time_usec < 94)
    {
        time_usec = 94;
    }

    /* time_usec = 3 * (1/32kHz) * (data + 1) */
    data = ((time_usec * 32) / 3000) - 1;

    err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, softstart_ramp_delay, 0x07, data, 0);

    return err_flag;
}


pm_err_flag_type pm_wled_en_soft_start(uint32 pmic_chip, uint32 resource_index, boolean enable)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;

    pm_register_address_type en_soft_start, sec_access;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    en_soft_start = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->test4;

    sec_access = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->sec_access;

    err_flag = pm_comm_write_byte(wled_ptr->comm_ptr->slave_id, sec_access, (pm_register_data_type)0xA5, 0);

    err_flag |= pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, en_soft_start, 0x02, (pm_register_data_type)(enable << 1), 0);

    return err_flag;
}


pm_err_flag_type pm_wled_sel_bst_ilimit(uint32 pmic_chip, uint32 resource_index, pm_wled_bst_ilimit_type bst_ilimit_type)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    
    pm_register_data_type data;

    pm_register_address_type wled_ilim;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (bst_ilimit_type >= PM_WLED_BST_ILIMIT_INVALID)
    {
        err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
    else
    {
        switch (bst_ilimit_type)
        {
        case PM_WLED_BST_ILIMIT_105mA   :
            data = 0x0; break;
        case PM_WLED_BST_ILIMIT_385mA   :
            data = 0x1; break;
        case PM_WLED_BST_ILIMIT_660mA   :
            data = 0x2; break;
        case PM_WLED_BST_ILIMIT_980mA   :
            data = 0x3; break;
        case PM_WLED_BST_ILIMIT_1150mA  :
            data = 0x4; break;
        case PM_WLED_BST_ILIMIT_1420mA  :
            data = 0x5; break;
        case PM_WLED_BST_ILIMIT_1700mA  :
            data = 0x6; break;
        case PM_WLED_BST_ILIMIT_1980mA  :
            data = 0x7; break;
        default:
            return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
        }

        wled_ilim = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->wled_ilim;

        err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, wled_ilim, 0x07, data, 0);
    }

    return err_flag;
}


pm_err_flag_type pm_wled_force_vmin(uint32 pmic_chip, pm_wled_led_type led_type)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data;
    pm_register_address_type feedback_control;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(led_type >= PM_WLED_INVALID)
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }

    feedback_control = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->feedback_control;

    data = (led_type == PM_WLED_ALL) ? 0x7 : (pm_register_data_type)((0x1 << led_type) >> 1);

    err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, feedback_control, 0x07, data, 0);

    return err_flag;
}


pm_err_flag_type pm_wled_sel_slew_rate(uint32 pmic_chip, uint32 resource_index, pm_wled_slew_rate_type slew_rate_type)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type sw_slew_rate_control;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (slew_rate_type >= PM_WLED_SLEW_RATE_INVALID)
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }

    sw_slew_rate_control = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->sw_slew_rate_control;
    err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, sw_slew_rate_control, 0x03, (pm_register_data_type)slew_rate_type, 0);

    return err_flag;
}


pm_err_flag_type pm_wled_sel_rz(uint32 pmic_chip, uint32 rz_kohm)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type vloop_comp_res;

    pm_register_data_type data = 0;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);
    
    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    vloop_comp_res = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->vloop_comp_res;

    if (rz_kohm > 320)
    {
        data = 15;
    }
    if (rz_kohm < 20)
    {
        data = 0;
    }
    if (rz_kohm >= 20 && rz_kohm <= 320)
    {
        data = (rz_kohm / 20) - 1;
    }

    err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, vloop_comp_res, 0x07, data, 0);

    return err_flag;
}


pm_err_flag_type pm_wled_sel_gm(uint32 pmic_chip, uint32 gm_sel)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type vloop_comp_gm;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    vloop_comp_gm = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->vloop_comp_gm;

    if (gm_sel > 15)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, vloop_comp_gm, 0x07, (pm_register_data_type)gm_sel, 0);

    return err_flag;
}


pm_err_flag_type pm_wled_en_sleepb(uint32 pmic_chip, uint32 resource_index, boolean enable)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type en_sleep_b, sec_access;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    en_sleep_b = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->test4;
    sec_access = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->sec_access;
    err_flag = pm_comm_write_byte(wled_ptr->comm_ptr->slave_id, sec_access, (pm_register_data_type)0xA5, 0);
    err_flag |= pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, en_sleep_b, 0x04, (pm_register_data_type)(enable << 2), 0);

    return err_flag;
}


pm_err_flag_type pm_wled_sel_vref_ctrl(uint32 pmic_chip, uint32 resource_index, uint32 milli_volt)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type vref_control;

    pm_register_data_type data = 0;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
   
    vref_control = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->vref_control;
   
    if (milli_volt > 675)
    {
        milli_volt = 675;
    }
    else if (milli_volt < 300)
    {
        milli_volt = 300;
    }

    data = (milli_volt - 300)/25;
    err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, vref_control, 0x07, data, 0);

    return err_flag;
}


pm_err_flag_type pm_wled_sel_comp_cap(uint32 pmic_chip, uint32 cz_picofarad)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type vloop_comp_cap;
    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);
    pm_register_data_type data = 0;
    
    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    vloop_comp_cap = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->vloop_comp_cap;

    if (cz_picofarad > 80)
    {
        data = 3;
    }
    if (cz_picofarad < 50)
    {
        data = 0;
    }
    if (cz_picofarad >= 50 && cz_picofarad <= 80)
    {
        data = (cz_picofarad / 10) - 5;
    }
    err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, vloop_comp_cap, 0x03, data, 0);

    return err_flag;
}


pm_err_flag_type pm_wled_sel_high_pole_cap(uint32 pmic_chip, uint32 cp_picofarad)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type vloop_comp_hcap;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    vloop_comp_hcap = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->vloop_comp_hcap;

    if (cp_picofarad > 3)
    {
        return PM_ERR_FLAG__PAR3_OUT_OF_RANGE;
    }

    err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, vloop_comp_hcap, 0x03, (pm_register_data_type)cp_picofarad, 0);

    return err_flag;
}

pm_err_flag_type pm_wled_en_amoled(uint32 pmic_chip, boolean enable)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type sec_access, en_amoled;
    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    pm_register_data_type data = (enable)? 0xFF : 0x00;
    
    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    
    sec_access = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->sec_access;
    en_amoled = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->en_amoled;
    err_flag = pm_comm_write_byte(wled_ptr->comm_ptr->slave_id, sec_access, 0xA5, 0);
    err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, en_amoled, 0x80, data, 0);

    return err_flag;
}


pm_err_flag_type pm_wled_get_status(uint32 pmic_chip, pm_wled_status_type *wled_status)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type led1_modulator_en, led2_modulator_en, led3_modulator_en, led4_modulator_en;
    pm_register_data_type    temp_data1,temp_data2,temp_data3,temp_data4;
    pm_register_address_type slope_comp_cap, modulation_scheme, boost_max_duty_cycle, current_sink_en, led1_brightness_setting_lsb, led1_brightness_setting_msb;
    pm_register_address_type led1_full_scale_current, led2_full_scale_current, led3_full_scale_current, led4_full_scale_current;
    pm_register_address_type switching_frequency, softstart_ramp_delay, wled_ilim, sw_slew_rate_control, vref_control, bias_gen_warmup_delay, wled_ovp;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);
    
    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }


   
    led1_modulator_en =  wled_ptr->wled_reg_table->wled_sink_reg->base_address + wled_ptr->wled_reg_table->wled_sink_reg->led1_modulator_en;
    led2_modulator_en =  wled_ptr->wled_reg_table->wled_sink_reg->base_address + wled_ptr->wled_reg_table->wled_sink_reg->led2_modulator_en;
    led3_modulator_en =  wled_ptr->wled_reg_table->wled_sink_reg->base_address + wled_ptr->wled_reg_table->wled_sink_reg->led3_modulator_en;
    led4_modulator_en =  wled_ptr->wled_reg_table->wled_sink_reg->base_address + wled_ptr->wled_reg_table->wled_sink_reg->led4_modulator_en;

    err_flag = pm_comm_read_byte_mask(wled_ptr->comm_ptr->slave_id, led1_modulator_en, 0x80, &temp_data1, 0);
    if (err_flag)
    {
        return err_flag;
    }
    err_flag = pm_comm_read_byte_mask(wled_ptr->comm_ptr->slave_id, led2_modulator_en, 0x80, &temp_data2, 0);
    if (err_flag)
    {
        return err_flag;
    }
    err_flag = pm_comm_read_byte_mask(wled_ptr->comm_ptr->slave_id, led3_modulator_en, 0x80, &temp_data3, 0);
    if (err_flag)
    {
        return err_flag;
    }
    err_flag = pm_comm_read_byte_mask(wled_ptr->comm_ptr->slave_id, led4_modulator_en, 0x80, &temp_data4, 0);
    if (err_flag)
    {
        return err_flag;
    }
    

    wled_status->led1_modulator_enable = (temp_data1 & 0x80)? TRUE : FALSE ;
    wled_status->led2_modulator_enable = (temp_data2 & 0x80)? TRUE : FALSE ;
    wled_status->led3_modulator_enable = (temp_data3 & 0x80)? TRUE : FALSE ;
    wled_status->led4_modulator_enable = (temp_data4 & 0x80)? TRUE : FALSE ;
                               
    current_sink_en =  wled_ptr->wled_reg_table->wled_sink_reg->base_address + wled_ptr->wled_reg_table->wled_sink_reg->current_sink_en;

    err_flag = pm_comm_read_byte(wled_ptr->comm_ptr->slave_id, current_sink_en, &temp_data1, 0);
    if (err_flag)
    {
        return err_flag;
    }

    wled_status->led1_current_sink_enable = (temp_data1 & 0x10)? TRUE : FALSE ; 
    wled_status->led2_current_sink_enable = (temp_data1 & 0x20)? TRUE : FALSE ; 
    wled_status->led3_current_sink_enable = (temp_data1 & 0x40)? TRUE : FALSE ; 
    wled_status->led4_current_sink_enable = (temp_data1 & 0x80)? TRUE : FALSE ; 

    slope_comp_cap = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->slope_comp_cap;
    err_flag |= pm_comm_read_byte_mask(wled_ptr->comm_ptr->slave_id, slope_comp_cap, 0x07, &temp_data1, 0);
    wled_status->slope_trim = (temp_data1>3)? (6500 - (500*(temp_data1-4))) : (4500 - (500*temp_data1));


    modulation_scheme = wled_ptr->wled_reg_table->wled_sink_reg->base_address + wled_ptr->wled_reg_table->wled_sink_reg->modulation_scheme;
    err_flag |= pm_comm_read_byte(wled_ptr->comm_ptr->slave_id, modulation_scheme, &temp_data1, 0);

    wled_status->mode_clk         = (pm_wled_mode_clk_type)(temp_data1>>6);                
    wled_status->en_phase_stagger = (temp_data1 &0x20)? TRUE : FALSE;            
    wled_status->accum_clk        = (pm_wled_mode_clk_type)((temp_data1 & 0x18)>> 3);      
    wled_status->en_hybrid_dim    = (temp_data1 & 0x04)?TRUE : FALSE;            
    wled_status->pwm_mode         = (pm_wled_pwm_mode_type)((temp_data1 & 0x02) >> 1);    
    wled_status->dim_mode         = (pm_wled_mode_dim_mode_type)(temp_data1 & 0x01);      

    boost_max_duty_cycle = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->boost_max_duty_cycle;
    err_flag |= pm_comm_read_byte_mask(wled_ptr->comm_ptr->slave_id, boost_max_duty_cycle, 0x03, &temp_data1, 0);
    wled_status->bst_max_duty = (pm_wled_bst_max_duty_type)(temp_data1 & 0x03);


    switching_frequency = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->switching_frequency;
    err_flag |= pm_comm_read_byte_mask(wled_ptr->comm_ptr->slave_id, switching_frequency, 0x0F, &temp_data1, 0);
    wled_status->set_fsw_ctrl = 9600/((temp_data1 & 0x0F)+1);
    
    softstart_ramp_delay = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->softstart_ramp_delay;
    err_flag |= pm_comm_read_byte_mask(wled_ptr->comm_ptr->slave_id, softstart_ramp_delay, 0x07, &temp_data1, 0);
    wled_status->ramp_time = (3000*(temp_data1+1))/32;

    wled_ilim = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->wled_ilim;
    err_flag |= pm_comm_read_byte_mask(wled_ptr->comm_ptr->slave_id, wled_ilim, 0x07, &temp_data1, 0);
    wled_status->bst_ilimit = (pm_wled_bst_ilimit_type)(temp_data1 & 0x07);

    sw_slew_rate_control = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->sw_slew_rate_control;
    err_flag |= pm_comm_read_byte_mask(wled_ptr->comm_ptr->slave_id, sw_slew_rate_control, 0x03, &temp_data1, 0);
    wled_status->slew_rate = (pm_wled_slew_rate_type)(temp_data1 & 0x03);

    vref_control = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->vref_control;
    err_flag |= pm_comm_read_byte_mask(wled_ptr->comm_ptr->slave_id, vref_control, 0x0F, &temp_data1, 0);
    wled_status->vref_ctrl = (temp_data1 * 25)+300;

    bias_gen_warmup_delay = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->bias_gen_warmup_delay;
    err_flag |= pm_comm_read_byte_mask(wled_ptr->comm_ptr->slave_id, bias_gen_warmup_delay, 0x03, &temp_data1, 0);
    wled_status->warmup_dly = (pm_wled_warmup_dly_type)(temp_data1 & 0x03);

    wled_ovp = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->wled_ovp;
    err_flag |= pm_comm_read_byte_mask(wled_ptr->comm_ptr->slave_id, wled_ovp, 0x03, &temp_data1, 0);
    wled_status->ovp_thr = (pm_wled_ovp_threshold_type)(temp_data1 & 0x03);

    led1_full_scale_current = wled_ptr->wled_reg_table->wled_sink_reg->base_address + wled_ptr->wled_reg_table->wled_sink_reg->led1_full_scale_current;
    led2_full_scale_current = wled_ptr->wled_reg_table->wled_sink_reg->base_address + wled_ptr->wled_reg_table->wled_sink_reg->led2_full_scale_current;
    led3_full_scale_current = wled_ptr->wled_reg_table->wled_sink_reg->base_address + wled_ptr->wled_reg_table->wled_sink_reg->led3_full_scale_current;
    led4_full_scale_current = wled_ptr->wled_reg_table->wled_sink_reg->base_address + wled_ptr->wled_reg_table->wled_sink_reg->led4_full_scale_current;

    err_flag = pm_comm_read_byte_mask(wled_ptr->comm_ptr->slave_id, led1_full_scale_current, 0xF, &temp_data1, 0);
    err_flag |= pm_comm_read_byte_mask(wled_ptr->comm_ptr->slave_id, led2_full_scale_current, 0xF, &temp_data2, 0);
    err_flag |= pm_comm_read_byte_mask(wled_ptr->comm_ptr->slave_id, led3_full_scale_current, 0xF, &temp_data3, 0);
    err_flag |= pm_comm_read_byte_mask(wled_ptr->comm_ptr->slave_id, led4_full_scale_current, 0xF, &temp_data4, 0);

    wled_status->led1_full_scale_current = (temp_data1 > 0x0C)? 30 : (temp_data1*25)/10;
    wled_status->led2_full_scale_current = (temp_data2 > 0x0C)? 30 : (temp_data2*25)/10;
    wled_status->led3_full_scale_current = (temp_data3 > 0x0C)? 30 : (temp_data3*25)/10;
    wled_status->led4_full_scale_current = (temp_data4 > 0x0C)? 30 : (temp_data4*25)/10;

    led1_brightness_setting_lsb = wled_ptr->wled_reg_table->wled_sink_reg->base_address + wled_ptr->wled_reg_table->wled_sink_reg->led1_brightness_setting_lsb;
    led1_brightness_setting_msb = wled_ptr->wled_reg_table->wled_sink_reg->base_address + wled_ptr->wled_reg_table->wled_sink_reg->led1_brightness_setting_msb;
   
    err_flag |= pm_comm_read_byte(wled_ptr->comm_ptr->slave_id, led1_brightness_setting_lsb, &temp_data1, 0);
    err_flag |= pm_comm_read_byte(wled_ptr->comm_ptr->slave_id, led1_brightness_setting_msb, &temp_data2, 0);

    wled_status->led1_brightness = (uint32)(((temp_data2 & 0xF)<<8)|temp_data1);

    err_flag |= pm_comm_read_byte(wled_ptr->comm_ptr->slave_id, led1_brightness_setting_lsb+0x10, &temp_data1, 0);
    err_flag |= pm_comm_read_byte(wled_ptr->comm_ptr->slave_id, led1_brightness_setting_msb+0x10, &temp_data2, 0);
    wled_status->led2_brightness = (uint32)(((temp_data2 & 0xF)<<8)|temp_data1);

    err_flag |= pm_comm_read_byte(wled_ptr->comm_ptr->slave_id, led1_brightness_setting_lsb+0x20, &temp_data1, 0);
    err_flag |= pm_comm_read_byte(wled_ptr->comm_ptr->slave_id, led1_brightness_setting_msb+0x20, &temp_data2, 0);
    wled_status->led3_brightness = (uint32)(((temp_data2 & 0xF)<<8)|temp_data1);

    err_flag |= pm_comm_read_byte(wled_ptr->comm_ptr->slave_id, led1_brightness_setting_lsb+0x30, &temp_data1, 0);
    err_flag |= pm_comm_read_byte(wled_ptr->comm_ptr->slave_id, led1_brightness_setting_msb+0x30, &temp_data2, 0);
    wled_status->led4_brightness = (uint32)(((temp_data2 & 0xF)<<8)|temp_data1);
   
   return err_flag;
}


pm_err_flag_type pm_wled_irq_enable(uint32 pmic_chip, pm_wled_irq_type irq, boolean enable)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type irq_reg;
    uint8 data = 1 << irq;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);
    
    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (irq >= PM_WLED_IRQ_INVALID)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if (enable)
    {
        irq_reg = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->int_en_set;
    }
    else
    {
        irq_reg = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->int_en_clr;
    }
    err_flag = pm_comm_write_byte(wled_ptr->comm_ptr->slave_id, irq_reg, data, 0);
    return err_flag;
}

pm_err_flag_type pm_wled_irq_clear(uint32 pmic_chip, pm_wled_irq_type irq)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type int_latched_clr;
    uint8 data = 1 << irq;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);
   
    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (irq >= PM_WLED_IRQ_INVALID)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    int_latched_clr = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->int_latched_clr;

    err_flag = pm_comm_write_byte(wled_ptr->comm_ptr->slave_id, int_latched_clr, data, 0);
    return err_flag;
}


pm_err_flag_type pm_wled_irq_set_trigger(uint32 pmic_chip, pm_wled_irq_type irq, pm_irq_trigger_type trigger)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type int_set_type, int_polarity_high, int_polarity_low;
    uint8 data = 1 << irq;

    pm_register_data_type set_type, polarity_high, polarity_low;
    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);
    
    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (irq >= PM_WLED_IRQ_INVALID)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    int_set_type = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->int_set_type;
    int_polarity_high = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->int_polarity_high;
    int_polarity_low = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->int_polarity_low;

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
    err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id,  int_set_type, data, set_type, 0);
    err_flag |= pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, int_polarity_high, data, polarity_high, 0);
    err_flag |= pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, int_polarity_low, data, polarity_low, 0);

    return err_flag;
}

pm_err_flag_type pm_wled_irq_status(uint32 pmic_chip, pm_wled_irq_type irq, pm_irq_status_type type, boolean *status)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data;
    uint8 mask = 1 << irq;    
    pm_register_address_type int_sts;
    
    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (irq >= PM_WLED_IRQ_INVALID)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    switch (type)
    {
    case PM_IRQ_STATUS_RT:
        int_sts = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->int_rt_sts;
        break;
    case PM_IRQ_STATUS_LATCHED:
        int_sts = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->int_latched_sts;
        break;
    case PM_IRQ_STATUS_PENDING:
        int_sts = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->int_pending_sts;
        break;
    default:
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    err_flag = pm_comm_read_byte_mask(wled_ptr->comm_ptr->slave_id, int_sts, mask, &data, 0);
    *status = data ? TRUE : FALSE;

    return err_flag;
}


static pm_err_flag_type pm_wled_sink_write(uint32 pmic_chip, pm_wled_led_type led_type, pm_register_address_type led1_reg_addr, pm_register_data_type data, pm_register_data_type mask)
{
    pm_err_flag_type      err_flag    = PM_ERR_FLAG__SUCCESS;

    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);

    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if (led_type >= PM_WLED_INVALID)
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    /*led4_reg_addr=led1_reg_addr+0x30; led3_reg_addr=led1_reg_addr+0x20; led2_reg_addr=led1_reg_addr+0x10;*/
    switch (led_type)
    {
    case PM_WLED_LED1:
        err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, led1_reg_addr, mask, data, 0);
        break;
    case PM_WLED_LED2:
        err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, (pm_register_address_type)(led1_reg_addr + 0x10), mask, data, 0);
        break;
    case PM_WLED_LED3:
        err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, (pm_register_address_type)(led1_reg_addr + 0x20), mask, data, 0);
        break;
    case PM_WLED_LED4:
        err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, (pm_register_address_type)(led1_reg_addr + 0x30), mask, data, 0);
        break;
    case PM_WLED_ALL:
        err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, led1_reg_addr, mask, data, 0);
        err_flag |= pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, (pm_register_address_type)(led1_reg_addr + 0x10), mask, data, 0);
        err_flag |= pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, (pm_register_address_type)(led1_reg_addr + 0x20), mask, data, 0);
        err_flag |= pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, (pm_register_address_type)(led1_reg_addr + 0x30), mask, data, 0);
        break;
    case PM_WLED_NONE:
        err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, led1_reg_addr, mask, 0x00, 0);
        err_flag |= pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, (pm_register_address_type)(led1_reg_addr + 0x10), mask, 0x00, 0);
        err_flag |= pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, (pm_register_address_type)(led1_reg_addr + 0x20), mask, 0x00, 0);
        err_flag |= pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, (pm_register_address_type)(led1_reg_addr + 0x30), mask, 0x00, 0);
        break;
    default:
        err_flag =  PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
        break;
    }

    return err_flag;
}


pm_err_flag_type pm_wled_config_psm(uint32 pmic_chip, boolean hyst_lvl_high, uint32 ref_volt_mv, boolean psm_en)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type psm_ctrl, en_psm;
    pm_register_data_type data = 0;
    
    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);
    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    
    psm_ctrl = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->psm_ctrl;
    en_psm   = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->en_psm;

    /* 000 - 0.4 V; 001 - 0.45 V; 010 - 0.5 V; 011 - 0.55 V; 100 - 0.6 V; 101 - 0.65 V; 110 - 0.7 V; 111 - 0.75 V*/
    ref_volt_mv = (ref_volt_mv > 750) ? 0x7 : ((ref_volt_mv - 400) / 50);

    data = (pm_register_data_type)((hyst_lvl_high << 3) | ref_volt_mv);

    err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, psm_ctrl, 0x0F, data, 0);
    err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, en_psm, 0x80, (pm_register_data_type)(psm_en<<7), 0);

    return err_flag;
}

pm_err_flag_type pm_wled_config_pfm(uint32 pmic_chip, boolean hyst_lvl_high, uint32 nfet_off_time, uint32 current_limit, boolean pfm_en)
{
    pm_err_flag_type    err_flag    = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type wled_pfm;
        
    pm_register_data_type data = 0;
    
    pm_wled_data_type *wled_ptr = pm_wled_get_data(pmic_chip);
    if (wled_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    
    wled_pfm = wled_ptr->wled_reg_table->wled_ctrl_reg->base_address + wled_ptr->wled_reg_table->wled_ctrl_reg->wled_pfm;
    /*00 - 130mA, 01 - 200mA, 10 - 270 mA, 11 - 340mA*/
    current_limit = (current_limit > 340) ? 0x3 : ((current_limit - 130) / 70);

    /*00 - 110ns, 01 - 240ns, 10 - 350ns, 11 - 480ns*/
    nfet_off_time = (nfet_off_time > 400) ? 0x3 : ((nfet_off_time / 100) - 1);

    data = (pm_register_data_type)((pfm_en << 7) | (hyst_lvl_high << 4) | (current_limit << 2) | nfet_off_time);

    err_flag = pm_comm_write_byte_mask(wled_ptr->comm_ptr->slave_id, wled_pfm, 0x9F, data, 0);

    return err_flag;

}


