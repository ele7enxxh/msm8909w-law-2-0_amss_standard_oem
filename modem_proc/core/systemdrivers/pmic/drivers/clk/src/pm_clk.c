/*! \file pm_clk.c
*  \n 
*  \brief Implementation file for CLK resource type.
*  \n  
*  &copy; Copyright 2013-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/drivers/clk/src/pm_clk.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/10/13   rh      File created
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_clk_driver.h"
#include "pm_clk.h"


/*===========================================================================

                     API IMPLEMENTATION

===========================================================================*/

pm_err_flag_type pm_clk_drv_strength(uint8 pmic_chip, pm_clk_type periph, pm_clk_drv_strength_type drive_strength)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_clk_data_type *clk_ptr = pm_clk_get_data(pmic_chip);
    pm_register_address_type reg = 0;
    uint32 periph_index = 0;

    if ((clk_ptr == NULL) || (periph > PM_SLEEP_CLK1) || (drive_strength >= PM_CLK_DRV_STRENGTH_INVALID))
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    periph_index = clk_ptr->clk_perph_index[periph];

    if((periph_index == PM_CLK_INVALID_DATA) || (periph_index >= PM_MAX_NUM_CLKS) ||
       (clk_ptr->periph_subtype[periph_index] == 0))
    {
        return PM_ERR_FLAG__INVALID_CLK_INDEXED;
    }

    if (clk_ptr->periph_subtype[periph_index] == PM_HW_MODULE_CLOCK_XO_CORE)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    reg = clk_ptr->clk_reg_table->base_address + (pm_register_address_type)(clk_ptr->clk_reg_table->peripheral_offset*periph_index) +
          clk_ptr->clk_reg_table->DRV_CTL1;

    /* Set strength */
    err_flag =  pm_comm_write_byte_mask(clk_ptr->comm_ptr->slave_id, reg,
                                        0x03, (pm_register_data_type)(drive_strength), 0);  // 1:0

    return err_flag;
}

pm_err_flag_type pm_dev_clk_buff_set_output_drive_strength(uint8 pmic_chip,
                                                           pm_clk_type periph,
                                                           pm_clk_drv_strength_type drive_strength)
{
	return pm_clk_drv_strength(pmic_chip, periph, drive_strength);
}

pm_err_flag_type pm_clk_get_status(uint8 pmic_chip, pm_clk_type clk, pm_clk_status_type *clk_status)
{
    pm_err_flag_type         err_flag   = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type reg = 0;
    pm_register_address_type clk_base_addr = 0;
    pm_register_data_type    data = 0;
    
    pm_clk_data_type *clk_ptr = pm_clk_get_data(pmic_chip);

    if ((clk_ptr == NULL) || (clk >= PM_ALL_CLKS))
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    uint8 periph_index = clk_ptr->clk_perph_index[clk];

    clk_base_addr = clk_ptr->clk_reg_table->base_address + (pm_register_address_type)(clk_ptr->clk_reg_table->peripheral_offset * periph_index);

    if ((clk_ptr->periph_subtype[periph_index] != PM_HW_MODULE_CLOCK_XO_CORE) &&
        (clk_ptr->periph_subtype[periph_index] != PM_HW_MODULE_CLOCK_DIV_CLK))
    {
        reg = clk_base_addr + clk_ptr->clk_reg_table->DRV_CTL1;

        /* Get strength */
        err_flag |= pm_comm_read_byte_mask(clk_ptr->comm_ptr->slave_id, reg, 0x03, &data, 0);

        clk_status->clk_drv_strength = (pm_clk_drv_strength_type)data;
    }

    reg = clk_base_addr + clk_ptr->clk_reg_table->STATUS1;

    err_flag |= pm_comm_read_byte(clk_ptr->comm_ptr->slave_id, reg, &data, 0);

    clk_status->clk_ok = (data & 0x80) ? PM_ON : PM_OFF;

    reg = clk_base_addr + clk_ptr->clk_reg_table->EN_CTL;

    err_flag |= pm_comm_read_byte(clk_ptr->comm_ptr->slave_id, reg, &data, 0);

    clk_status->clk_sw_enable = (data & 0x80) ? PM_ON : PM_OFF;

    /* Pin Control and Pull Down are applicable for specific clock subtypes */
    if ((clk_ptr->periph_subtype[periph_index] == PM_HW_MODULE_CLOCK_BB_CLK) || 
        (clk_ptr->periph_subtype[periph_index] == PM_HW_MODULE_CLOCK_RF_CLK) ||
        (clk_ptr->periph_subtype[periph_index] == PM_HW_MODULE_CLOCK_CLK_DIST))
    {
        clk_status->clk_pin_ctrled = (data & 0x01) ? PM_ON : PM_OFF;

        reg = clk_base_addr + clk_ptr->clk_reg_table->PD_CTL;

        err_flag |= pm_comm_read_byte(clk_ptr->comm_ptr->slave_id, reg, &data, 0);

        clk_status->clk_pull_down = (data & 0x80) ? PM_ON : PM_OFF;
    }

    if (clk_ptr->periph_subtype[periph_index] == PM_HW_MODULE_CLOCK_DIV_CLK)
    {
        clk_status->clk_pin_ctrled = (data & 0x01) ? PM_ON : PM_OFF;

        reg = clk_base_addr + clk_ptr->clk_reg_table->DIV_CTL1;

        err_flag |= pm_comm_read_byte(clk_ptr->comm_ptr->slave_id, reg, &data, 0);

        clk_status->clk_out_div = (data & 0x7);
    }

    return err_flag;
}

