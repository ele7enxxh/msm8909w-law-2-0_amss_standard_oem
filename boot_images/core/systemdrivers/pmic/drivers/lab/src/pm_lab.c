/*! \file
*  
*  \brief  pm_lab.c driver implementation.
*  \details charger driver implementation.
*  &copy;
*  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved. 
*  Qualcomm Technologies Proprietary and Confidential.
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/lab/src/pm_lab.c#1 $ 

when       	who     	 what, where, why
--------   	---    		 ---------------------------------------------------------- 
08/20/14        al               Updating comm lib 
08/12/14        al               Mask and range fixes
07/03/14        al               Updating API to read LAB VREG status    
06/20/14        al               Updated coypright info 
05/20/14        al               Architecture update 
05/29/14        al               Adding API to configure Vdisp 
04/30/14        al               Initial revision
========================================================================== */

/*===========================================================================

					INCLUDE FILES

===========================================================================*/

#include "pm_lab.h"
#include "pm_lab_driver.h"
#include "pm_resource_manager.h"

static  pm_err_flag_type pm_lab_unlock_perph_write(pm_lab_data_type *lab_ptr);


pm_err_flag_type pm_lab_lcd_amoled_sel(uint32 device_index, boolean en_amoled_mode)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_register_address_type lcd_amoled_sel;

   pm_lab_data_type *lab_ptr  = pm_lab_get_data(device_index);

   if (NULL == lab_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      lcd_amoled_sel = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->lcd_amoled_sel;
      data = (en_amoled_mode)? 0x80 : 0x00;
      err_flag = pm_lab_unlock_perph_write(lab_ptr);
      err_flag |= pm_comm_write_byte_mask(lab_ptr->comm_ptr->slave_id, lcd_amoled_sel, 0x80, data,0);
   }

   return err_flag;
}


pm_err_flag_type pm_lab_lab_module_rdy(uint32 device_index, boolean lab_rdy)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_register_address_type module_rdy;

   pm_lab_data_type *lab_ptr  = pm_lab_get_data(device_index);

   if (NULL == lab_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      module_rdy = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->module_rdy;
      data = (lab_rdy)? 0x80 : 0x00;
      err_flag = pm_comm_write_byte_mask(lab_ptr->comm_ptr->slave_id, module_rdy, 0x80, data,0);
   }

   return err_flag;
}


pm_err_flag_type pm_lab_en_lab_module(uint32 device_index, boolean en_module)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_register_address_type enable_ctl;

   pm_lab_data_type *lab_ptr  = pm_lab_get_data(device_index);

   if (NULL == lab_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      enable_ctl = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->enable_ctl;
      data = (en_module)? 0x80 : 0x00;
      err_flag = pm_lab_unlock_perph_write(lab_ptr);
      err_flag |= pm_comm_write_byte_mask(lab_ptr->comm_ptr->slave_id, enable_ctl, 0x80, data,0);
   }

   return err_flag;
}


pm_err_flag_type pm_lab_ibb_rdy_en(uint32 device_index, boolean ibb_en_rdy)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_register_address_type ibb_en_rdy_reg;

   pm_lab_data_type *lab_ptr  = pm_lab_get_data(device_index);

   if (NULL == lab_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      ibb_en_rdy_reg = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->ibb_en_rdy;
      data = (ibb_en_rdy)? 0x80 : 0x00;
      err_flag = pm_comm_write_byte_mask(lab_ptr->comm_ptr->slave_id, ibb_en_rdy_reg, 0x80, data,0);
   }

   return err_flag;
}



pm_err_flag_type pm_lab_get_lab_status(uint32 device_index, pm_lab_status_type* lab_status)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_register_address_type reg;

   pm_lab_data_type *lab_ptr  = pm_lab_get_data(device_index);

   if (NULL == lab_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else if (NULL == lab_status)
   {
      err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
   }
   else
   {
    reg = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->lcd_amoled_sel;
    err_flag |= pm_comm_read_byte(lab_ptr->comm_ptr->slave_id, reg, &data,0);
    lab_status->en_amoled_mode = (data & 0x80)? TRUE : FALSE;
    reg = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->module_rdy;
    err_flag = pm_comm_read_byte(lab_ptr->comm_ptr->slave_id, reg, &data, 0);
    lab_status->lab_module_rdy =(data & 0x80)? TRUE : FALSE;
    reg = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->enable_ctl;
    err_flag |= pm_comm_read_byte(lab_ptr->comm_ptr->slave_id, reg, &data,0);
    lab_status->lab_module_en = (data & 0x80)? TRUE : FALSE;

    reg = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->ibb_en_rdy;
    err_flag |= pm_comm_read_byte(lab_ptr->comm_ptr->slave_id, reg, &data,0);
    lab_status->lab_ibb_en_rdy = (data & 0x80)? TRUE : FALSE;

    reg = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->precharge_ctl;
    err_flag |= pm_comm_read_byte(lab_ptr->comm_ptr->slave_id, reg, &data, 0);
    lab_status->en_fast_precharge = (data & 0x04)? TRUE : FALSE ;
    lab_status->max_prechg_time_usec = ((data & 0x03)*100) + 200;

    reg = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->output_voltage;
    err_flag |= pm_comm_read_byte(lab_ptr->comm_ptr->slave_id, reg, &data, 0);
    lab_status->override_output_volt = (data & 0x80)? TRUE: FALSE;
    lab_status->output_volt = ((data& 0xF)*100) + 4600;

    reg = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->status1;
    err_flag |= pm_comm_read_byte(lab_ptr->comm_ptr->slave_id, reg, &data, 0);
    lab_status->lab_vreg_ok     = (data & 0x80) ? TRUE: FALSE; 
    lab_status->short_circuit   = (data & 0x40) ? FALSE: TRUE; //if false then no short circuit else short circuit;
    lab_status->lab_status_en   = (data & 0x20) ? TRUE: FALSE;

   }

   return err_flag ;
}


pm_err_flag_type pm_lab_config_precharge_ctrl(uint32 device_index, boolean en_fast_precharge, uint32 max_prechg_time_usec)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_register_address_type reg;

   pm_lab_data_type *lab_ptr  = pm_lab_get_data(device_index);

   if (NULL == lab_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      reg = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->precharge_ctl;
      /*range for max_prechg_time_usec 200 to 400*/
      max_prechg_time_usec = (max_prechg_time_usec < 200)? 200 : max_prechg_time_usec;
      max_prechg_time_usec = (max_prechg_time_usec>400)? 0x3 : ((max_prechg_time_usec-200)/100);
      data = (pm_register_data_type)((en_fast_precharge<<2) | max_prechg_time_usec);
      err_flag = pm_comm_write_byte_mask(lab_ptr->comm_ptr->slave_id, reg, 0x07, data,0);
   }

   return err_flag;
}


pm_err_flag_type pm_lab_config_output_volt(uint32 device_index, boolean override_output_volt, uint32 volt_millivolt)
{
   pm_err_flag_type  err_flag  = PM_ERR_FLAG__SUCCESS;
   pm_register_data_type data;
   pm_register_address_type reg;

   pm_lab_data_type *lab_ptr  = pm_lab_get_data(device_index);

   if (NULL == lab_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      reg = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->output_voltage;

      /*limit is 4600 to 6100*/
      volt_millivolt = (volt_millivolt < 4600)? 4600 : volt_millivolt;

      volt_millivolt = (volt_millivolt > 6100) ? 0xF : ((volt_millivolt - 4600)/100); 

      data = (pm_register_data_type)((override_output_volt << 7) | (0xF & volt_millivolt));

      err_flag = pm_comm_write_byte_mask(lab_ptr->comm_ptr->slave_id, reg, 0x8F, data,0);
   }

   return err_flag;
}


static  pm_err_flag_type pm_lab_unlock_perph_write(pm_lab_data_type *lab_ptr)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
   pm_register_address_type sec_access;

   if (NULL == lab_ptr)
   {
      err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
   }
   else
   {
      sec_access = lab_ptr->lab_reg_table->base_address + lab_ptr->lab_reg_table->sec_access;
      err_flag = pm_comm_write_byte(lab_ptr->comm_ptr->slave_id, sec_access, 0xA5, 0);
   }

   return err_flag;
}
