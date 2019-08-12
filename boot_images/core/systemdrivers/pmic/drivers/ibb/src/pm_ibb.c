/*! \file
*  
*  \brief  pm_ibb.c driver implementation.
*  \details charger driver implementation.
*  &copy;
*  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved. 
*  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/ibb/src/pm_ibb.c#1 $ 

when       	who     	 what, where, why
--------   	---    		 ---------------------------------------------------------- 
08/20/14        al               Updating comm lib 
07/03/14        al               Adding API for delay config
05/20/14        al               Architecture update 
04/30/14        al               Initial revision
========================================================================== */

/*===========================================================================

					INCLUDE FILES

===========================================================================*/

#include "pm_ibb.h"
#include "pm_ibb_driver.h"

static  pm_err_flag_type pm_ibb_unlock_perph_write(pm_ibb_data_type *ibb_ptr);


pm_err_flag_type pm_ibb_lcd_amoled_sel(uint32 device_index, boolean en_amoled_mode)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;

   pm_ibb_data_type *ibb_ptr  = pm_ibb_get_data(device_index);

   if (NULL == ibb_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type lcd_amoled_sel = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->lcd_amoled_sel;
      data = (en_amoled_mode)? 0x80 : 0x00 ;
      err_flag = pm_ibb_unlock_perph_write(ibb_ptr);
      err_flag |= pm_comm_write_byte_mask(ibb_ptr->comm_ptr->slave_id, lcd_amoled_sel, 0x80, data,0);
   }

   return err_flag;
}


pm_err_flag_type pm_ibb_ibb_module_rdy(uint32 device_index, boolean ibb_rdy)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;

   pm_ibb_data_type *ibb_ptr  = pm_ibb_get_data(device_index);

   if (NULL == ibb_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type module_rdy = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->module_rdy;
      data = (ibb_rdy)? 0x80 : 0x00;
      err_flag = pm_comm_write_byte_mask(ibb_ptr->comm_ptr->slave_id, module_rdy, 0x80, data,0);
   }

   return err_flag;
}




pm_err_flag_type pm_ibb_config_ibb_ctrl(uint32 device_index, boolean en_module, boolean swire_rdy)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;

   pm_ibb_data_type *ibb_ptr  = pm_ibb_get_data(device_index);

   if (NULL == ibb_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type enable_ctl = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->enable_ctl;
      data = (pm_register_data_type)(en_module<<7 | swire_rdy<<6);
      err_flag |= pm_comm_write_byte_mask(ibb_ptr->comm_ptr->slave_id, enable_ctl, 0xC0, data,0);
   }

   return err_flag;
}


pm_err_flag_type pm_ibb_set_soft_strt_chgr_resistor(uint32 device_index, uint32 resistor_kohms)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;

   pm_ibb_data_type *ibb_ptr  = pm_ibb_get_data(device_index);

   if (NULL == ibb_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type soft_start_ctl = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->soft_start_ctl;

      if (resistor_kohms > 32)
      {
         data = (resistor_kohms>64)? 0x0 : 0x1; 
      }
      else
      {
         data = (resistor_kohms>16)? 0x2 : 0x3;  
      }

      err_flag = pm_comm_write_byte_mask(ibb_ptr->comm_ptr->slave_id, soft_start_ctl, 0x03, data,0);
   }

   return err_flag;
}


pm_err_flag_type pm_ibb_set_swire_output_pulse(uint32 device_index, uint32 volt_millivolt)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;

   pm_ibb_data_type *ibb_ptr  = pm_ibb_get_data(device_index);

   if (NULL == ibb_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type swire_ctl = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->swire_ctl;

      data = (volt_millivolt > 7600) ? 0x3F : ((volt_millivolt - 1400)/100); 

      err_flag = pm_comm_write_byte_mask(ibb_ptr->comm_ptr->slave_id, swire_ctl, 0x3F, data,0);
   }

   return err_flag;
}



pm_err_flag_type pm_ibb_config_output_volt(uint32 device_index, boolean override_output_volt, uint32 volt_millivolt)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;

   pm_ibb_data_type *ibb_ptr  = pm_ibb_get_data(device_index);

   if (NULL == ibb_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      pm_register_address_type output_voltage = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->output_voltage;
      
      /*to make sure value does not go negative in next step*/
      volt_millivolt = (volt_millivolt<1400)? 1400 : volt_millivolt;
      volt_millivolt = (volt_millivolt > 7600) ? 0x3F : ((volt_millivolt - 1400)/100); 
      data = (pm_register_data_type)((override_output_volt << 7) | (0x3F & volt_millivolt));

      err_flag = pm_comm_write_byte_mask(ibb_ptr->comm_ptr->slave_id, output_voltage, 0xBF, data,0);
   }

   return err_flag;
}


pm_err_flag_type pm_ibb_config_pwrup_pwrdn_dly(uint32 device_index, pm_ibb_pwrup_pwrdn_dly_type *delay_config)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_register_address_type pwrup_pwrdn_ctl_1;

   pm_ibb_data_type *ibb_ptr  = pm_ibb_get_data(device_index);

   if (NULL == ibb_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if ((NULL == delay_config)
            ||(delay_config->pwrdn_dly_src >= PM_IBB_PWRDN_DLY_SRC_INVALID)
            ||(delay_config->pwrup_dly_src >= PM_IBB_PWRUP_DLY_SRC_INVALID))
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
      /*valid values are 1ms:0x0,2ms:0x1,4ms:0x2,8ms:0x3*/
      delay_config->pwrup_dly_ms =  (delay_config->pwrup_dly_ms > 4)? 0x3 : (delay_config->pwrup_dly_ms)/2;
      delay_config->pwrdn_dly_ms =  (delay_config->pwrdn_dly_ms > 4)? 0x3 : (delay_config->pwrdn_dly_ms)/2;

      pwrup_pwrdn_ctl_1 = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->pwrup_pwrdn_ctl_1;

      data = (pm_register_data_type)((delay_config->pwrdn_dly_ms) | (delay_config->pwrdn_dly_src <<2) | (delay_config->pwrup_dly_ms<<4) | (delay_config->en_pwrup_dly<<6) | (delay_config->pwrup_dly_src<<7));
      
      err_flag |= pm_ibb_unlock_perph_write(ibb_ptr);

      err_flag = pm_comm_write_byte_mask(ibb_ptr->comm_ptr->slave_id, pwrup_pwrdn_ctl_1, 0xF7, data,0);
   }

   return err_flag;
}

pm_err_flag_type pm_ibb_get_ibb_status(uint32 device_index, pm_ibb_status_type* ibb_status)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_register_address_type reg; 

   pm_ibb_data_type *ibb_ptr  = pm_ibb_get_data(device_index);

   if (NULL == ibb_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == ibb_status)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
     reg = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->output_voltage;
     err_flag = pm_comm_read_byte(ibb_ptr->comm_ptr->slave_id, reg, &data, 0);
     ibb_status->override_output_volt = (data & 0x80)? TRUE: FALSE;
     ibb_status->output_volt = ((data& 0x3F)*100) + 1400;

     reg = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->swire_ctl;
     err_flag |= pm_comm_read_byte(ibb_ptr->comm_ptr->slave_id, reg, &data,0);
     ibb_status->swire_pulse_mv = ((data & 0x3F)*100) + 1400;

     reg = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->soft_start_ctl;
     err_flag |= pm_comm_read_byte(ibb_ptr->comm_ptr->slave_id, reg, &data,0);
     switch (data & 0x03)
     {
        case 0: ibb_status->chgr_resistor_kohms = 300; break;
        case 1: ibb_status->chgr_resistor_kohms =  64; break;
        case 2: ibb_status->chgr_resistor_kohms =  32; break;
        case 3: ibb_status->chgr_resistor_kohms =  16; break;
        default: ibb_status->chgr_resistor_kohms = 0;
     }

     reg = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->enable_ctl;
     err_flag |= pm_comm_read_byte(ibb_ptr->comm_ptr->slave_id, reg, &data,0);
     ibb_status->ibb_module_en = (data & 0x80) ? TRUE : FALSE;
     ibb_status->swire_rdy = (data & 0x40)? TRUE : FALSE;

     reg = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->module_rdy;
     err_flag = pm_comm_read_byte(ibb_ptr->comm_ptr->slave_id, reg, &data, 0);
     ibb_status->ibb_module_rdy = (data & 0x80)? TRUE : FALSE;

     reg = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->lcd_amoled_sel;
     err_flag |= pm_comm_read_byte(ibb_ptr->comm_ptr->slave_id, reg, &data,0);
     ibb_status->en_amoled_mode = (data & 0x80)? TRUE : FALSE;

     reg = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->pwrup_pwrdn_ctl_1;
     err_flag |= pm_comm_read_byte(ibb_ptr->comm_ptr->slave_id, reg, &data,0);

     ibb_status->ibb_dly_config.pwrdn_dly_ms     = 1<< (data & 0x03);
     ibb_status->ibb_dly_config.pwrdn_dly_src    = (pm_ibb_pwrdn_dly_src_type)((data & 0x04)>>2);
     ibb_status->ibb_dly_config.pwrup_dly_ms     = 1<<((data & 0x30)>>4);
     ibb_status->ibb_dly_config.en_pwrup_dly     = (data & 0x40)? TRUE : FALSE;
     ibb_status->ibb_dly_config.pwrup_dly_src    = (pm_ibb_pwrup_dly_src_type)((data & 0x80)>>7);

     reg = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->status1;
     err_flag |= pm_comm_read_byte(ibb_ptr->comm_ptr->slave_id, reg, &data,0);
     
     ibb_status->ibb_vreg_ok  =  (data & 0x80) ? TRUE: FALSE; 
     ibb_status->short_circuit = (data & 0x40) ? TRUE: FALSE;
     ibb_status->ilimit_error =  (data & 0x20) ? TRUE: FALSE;
   }

  return err_flag ;
};


static  pm_err_flag_type pm_ibb_unlock_perph_write(pm_ibb_data_type *ibb_ptr)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
   pm_register_address_type sec_access;

   if (NULL == ibb_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      sec_access = ibb_ptr->ibb_reg_table->base_address + ibb_ptr->ibb_reg_table->sec_access;
      err_flag = pm_comm_write_byte(ibb_ptr->comm_ptr->slave_id, sec_access, 0xA5, 0);
   }

   return err_flag;
}
