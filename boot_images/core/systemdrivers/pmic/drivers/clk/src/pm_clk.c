/*! \file pm_clk.c
 *  \n 
 *  \brief Implementation file for CLOCK public APIs. 
 *  \n  
 *  \n &copy; Copyright 2013-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/clk/src/pm_clk.c#1 $ 
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/2014 akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture 
09/25/13   kt      Updated clk module driver.
02/01/13   hw      Rearchitecting clk module driver to peripheral driver
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_clk.h"
#include "pm_clk_driver.h"
#include "pm_comm.h"

/*===========================================================================

                     API IMPLEMENTATION 

===========================================================================*/

pm_err_flag_type pm_clk_drv_strength(uint8 pmic_chip, pm_clk_type periph, pm_clk_drv_strength_type drive_strength )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_clk_data_type *clk_ptr = pm_clk_get_data(pmic_chip);
    pm_register_address_type reg = 0;
    uint32 periph_index = 0;

    if ((clk_ptr == NULL) || (periph >= PM_ALL_CLKS) || 
        (drive_strength >= PM_CLK_DRV_STRENGTH_INVALID))
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
    err_flag = pm_comm_write_byte_mask(clk_ptr->comm_ptr->slave_id, reg, 0x03, (pm_register_data_type)(drive_strength), 0);  
    
    /* Let the user know if we were successful or not */
    return err_flag;
}

pm_err_flag_type pm_clk_sw_enable(uint8 pmic_chip, pm_clk_type periph, pm_on_off_type on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_clk_data_type *clk_ptr = pm_clk_get_data(pmic_chip);
    pm_register_address_type reg = 0;
    pm_register_data_type data = 0;
    uint32 periph_index = 0;

    if ((clk_ptr == NULL) || (periph >= PM_ALL_CLKS))
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    periph_index = clk_ptr->clk_perph_index[periph];

    if((periph_index == PM_CLK_INVALID_DATA) || (periph_index >= PM_MAX_NUM_CLKS) ||
       (clk_ptr->periph_subtype[periph_index] == 0))
    {
        return PM_ERR_FLAG__INVALID_CLK_INDEXED;
    }

    reg = clk_ptr->clk_reg_table->base_address + (pm_register_address_type)(clk_ptr->clk_reg_table->peripheral_offset*periph_index) + 
          clk_ptr->clk_reg_table->EN_CTL;

    if (PM_ON == on_off)
    {
        data = 0xFF;
    }
    else
    {
        data = 0x00;
    }

    err_flag =  pm_comm_write_byte_mask(clk_ptr->comm_ptr->slave_id, reg , 0x80, data, 0); // 7

    return err_flag;
}

pm_err_flag_type pm_clk_pin_ctrled( uint8 pmic_chip, pm_clk_type periph, pm_on_off_type on_off )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_clk_data_type *clk_ptr = pm_clk_get_data(pmic_chip);
    pm_register_address_type reg = 0;
    pm_register_data_type data = 0;
    uint32 periph_index = 0;

    if ((clk_ptr == NULL) || (periph >= PM_ALL_CLKS))
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    periph_index = clk_ptr->clk_perph_index[periph];

    if((periph_index == PM_CLK_INVALID_DATA) || (periph_index >= PM_MAX_NUM_CLKS) ||
       (clk_ptr->periph_subtype[periph_index] == 0))
    {
        return PM_ERR_FLAG__INVALID_CLK_INDEXED;
    }

    reg = clk_ptr->clk_reg_table->base_address + (pm_register_address_type)(clk_ptr->clk_reg_table->peripheral_offset*periph_index) + 
          clk_ptr->clk_reg_table->EN_CTL;

    if (PM_ON == on_off)
    {
        data = 0xFF;
    }
    else
    {
        data = 0x00;
    }

    err_flag = pm_comm_write_byte_mask(clk_ptr->comm_ptr->slave_id, reg , 0x01, data, 0); // 0

    return err_flag;
}



pm_err_flag_type pm_clk_pull_down( uint8 pmic_chip, pm_clk_type periph, pm_on_off_type on_off )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_clk_data_type *clk_ptr = pm_clk_get_data(pmic_chip);
    pm_register_address_type reg = 0;
    pm_register_data_type    data = 0;
    uint8 periph_index = 0;

    if ((clk_ptr == NULL) || (periph >= PM_ALL_CLKS))
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
        }

    periph_index = clk_ptr->clk_perph_index[periph];

    if((periph_index == PM_CLK_INVALID_DATA) || (periph_index >= PM_MAX_NUM_CLKS) ||
       (clk_ptr->periph_subtype[periph_index] == 0))
    {
        return PM_ERR_FLAG__INVALID_CLK_INDEXED;
    }

    reg = clk_ptr->clk_reg_table->base_address + (pm_register_address_type)(clk_ptr->clk_reg_table->peripheral_offset*periph_index) + 
          clk_ptr->clk_reg_table->PD_CTL;

    if (PM_ON == on_off)
    {
        data = 0xFF;
    }
    else
    {
        data = 0x00;
    }

    err_flag = pm_comm_write_byte_mask(clk_ptr->comm_ptr->slave_id, reg , 0x80, data, 0); // 7

    return err_flag;
}

