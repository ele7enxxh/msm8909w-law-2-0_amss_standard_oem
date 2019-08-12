/*! \file pm_rgb.c
*  
*  \brief  pm_rgb.c
*  \details Implementation file for CLK resourece type.
*    
*  &copy; Copyright 2012 - 2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/rgb/src/pm_rgb.c#1 $ 
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture   
02/27/13   aab     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_rgb.h"
#include "pm_rgb_driver.h"
#include "pm_comm.h"

pm_err_flag_type pm_rgb_set_voltage_source
(
  uint8                       pmic_chip,
  pm_rgb_which_type           rgb,
  pm_rgb_voltage_source_type  source
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type reg = 0;
    pm_register_address_type periph_index = (pm_register_address_type)rgb;
    pm_rgb_data_type *rgb_ptr = pm_rgb_get_data(pmic_chip);

    if (rgb_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if (periph_index >= rgb_ptr->num_of_peripherals)
    {
      return PM_ERR_FLAG__INVALID_SMBB_INDEXED;
    }

    reg = rgb_ptr->rgb_reg_table->base_address + (rgb_ptr->rgb_reg_table->peripheral_offset*periph_index) + rgb_ptr->rgb_reg_table->src_sel;
    
    switch (source)
    {
      //ToDo: This require update (remove casting enums that doesn't not guarantee sequencing)
      case PM_RGB_VOLTAGE_SOURCE_GND:         source = (pm_rgb_voltage_source_type)0; break;
      case PM_RGB_VOLTAGE_SOURCE_VPH:         source = (pm_rgb_voltage_source_type)1; break;
      case PM_RGB_VOLTAGE_SOURCE_5V:          source = (pm_rgb_voltage_source_type)2; break;
      case PM_RGB_VOLTAGE_SOURCE_MIN_VCHG_5V: source = (pm_rgb_voltage_source_type)3; break;
      default:
        return PM_ERR_FLAG__PAR3_OUT_OF_RANGE;
    }

    err_flag = pm_comm_write_byte_mask(rgb_ptr->comm_ptr->slave_id, reg, 0x03, (pm_register_data_type)(source), 0);  // 1:0

    return err_flag;
}


pm_err_flag_type pm_rgb_enable
(
  uint8               pmic_chip,
  pm_rgb_which_type   rgb,
  uint32              rgb_mask,
  boolean             enable,
  boolean             atc
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_rgb_data_type *rgb_ptr = pm_rgb_get_data(pmic_chip);
    pm_register_address_type reg = 0;
    pm_register_address_type periph_index = (pm_register_address_type)rgb;
    pm_register_data_type rgb_en_ctl_val;

    if (rgb_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (periph_index >= rgb_ptr->num_of_peripherals)
    {
      return PM_ERR_FLAG__INVALID_SMBB_INDEXED;
    }

    if (!atc)
    {
        reg = rgb_ptr->rgb_reg_table->base_address + (rgb_ptr->rgb_reg_table->peripheral_offset*periph_index) + rgb_ptr->rgb_reg_table->en_ctl;
    }
    else
    {
        reg = rgb_ptr->rgb_reg_table->base_address + (rgb_ptr->rgb_reg_table->peripheral_offset*periph_index) + rgb_ptr->rgb_reg_table->en_ctl_atc;
    }

    err_flag  = pm_comm_read_byte_mask(rgb_ptr->comm_ptr->slave_id, reg, 0xFF, &rgb_en_ctl_val,0);

    if (rgb_mask & PM_RGB_SEGMENT_R)
    {
      rgb_en_ctl_val = (rgb_en_ctl_val & ~(1 << 7)) | (enable << 7);
    }
    if (rgb_mask & PM_RGB_SEGMENT_G)
    {
      rgb_en_ctl_val = (rgb_en_ctl_val & ~(1 << 6)) | (enable << 6);
    }
    if (rgb_mask & PM_RGB_SEGMENT_B)
    {
      rgb_en_ctl_val = (rgb_en_ctl_val & ~(1 << 5)) | (enable << 5);
    }

    err_flag |= pm_comm_write_byte(rgb_ptr->comm_ptr->slave_id, reg, rgb_en_ctl_val, 0);

    
    return err_flag;
}


pm_err_flag_type pm_rgb_get_status
(
  uint8               pmic_chip,
  pm_rgb_which_type   rgb,
  pm_rgb_status_type  *status
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_rgb_data_type *rgb_ptr = pm_rgb_get_data(pmic_chip);
    pm_register_address_type periph_index = (pm_register_address_type)rgb;

    pm_register_address_type reg_rgb_en_ctl = 0;
    pm_register_address_type reg_rgb_en_ctl_atc = 0;
    pm_register_address_type reg_rgb_src_sel = 0;

    pm_register_data_type en_ctl_val = 0;
    pm_register_data_type en_ctl_atc_val = 0;
    pm_register_data_type src_sel_val = 0;

    if (rgb_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (periph_index >= rgb_ptr->num_of_peripherals)
    {
      return PM_ERR_FLAG__INVALID_SMBB_INDEXED;
    }


    reg_rgb_en_ctl     = rgb_ptr->rgb_reg_table->base_address + (rgb_ptr->rgb_reg_table->peripheral_offset*periph_index) + rgb_ptr->rgb_reg_table->en_ctl;
    reg_rgb_en_ctl_atc = rgb_ptr->rgb_reg_table->base_address + (rgb_ptr->rgb_reg_table->peripheral_offset*periph_index) + rgb_ptr->rgb_reg_table->en_ctl_atc;
    reg_rgb_src_sel    = rgb_ptr->rgb_reg_table->base_address + (rgb_ptr->rgb_reg_table->peripheral_offset*periph_index) + rgb_ptr->rgb_reg_table->src_sel;

    err_flag   = pm_comm_read_byte(rgb_ptr->comm_ptr->slave_id, reg_rgb_en_ctl,     &en_ctl_val, 0);
    err_flag  |= pm_comm_read_byte(rgb_ptr->comm_ptr->slave_id, reg_rgb_en_ctl_atc, &en_ctl_atc_val, 0);
    err_flag  |= pm_comm_read_byte(rgb_ptr->comm_ptr->slave_id, reg_rgb_src_sel,    &src_sel_val, 0);

    status->enabled_mask = (en_ctl_val & (1 << 7)) ? PM_RGB_SEGMENT_R : 0;
    status->enabled_mask |= (en_ctl_val & (1 << 6)) ? PM_RGB_SEGMENT_G : 0;
    status->enabled_mask |= (en_ctl_val & (1 << 5)) ? PM_RGB_SEGMENT_B : 0;
    status->enabled_atc_mask = (en_ctl_atc_val & (1 << 7)) ? PM_RGB_SEGMENT_R : 0;
    status->enabled_atc_mask |= (en_ctl_atc_val & (1 << 6)) ? PM_RGB_SEGMENT_G : 0;
    status->enabled_atc_mask |= (en_ctl_atc_val & (1 << 5)) ? PM_RGB_SEGMENT_B : 0;

    switch (src_sel_val & 0x3)
    {
      case 0: status->voltage_source = PM_RGB_VOLTAGE_SOURCE_GND; break;
      case 1: status->voltage_source = PM_RGB_VOLTAGE_SOURCE_VPH; break;
      case 2: status->voltage_source = PM_RGB_VOLTAGE_SOURCE_5V; break;
      case 3: status->voltage_source = PM_RGB_VOLTAGE_SOURCE_MIN_VCHG_5V; break;
      default:
        status->voltage_source = PM_RGB_VOLTAGE_SOURCE_INVALID;
    }
    
    return err_flag;
}

