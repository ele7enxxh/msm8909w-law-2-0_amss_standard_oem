/*! \file pm_clk_sleep.c
*  \n
*  \brief Implementation file for CLK SLEEP public APIs.
*  \n  
*  &copy; Copyright 2013-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/drivers/clk/src/pm_clk_sleep.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/20/13   aks      Created as part of code re-architecture.
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_clk_sleep.h"
#include "pm_clk_driver.h"
#include "pm_comm.h"

/*===========================================================================

                     API IMPLEMENTATION 

===========================================================================*/
pm_err_flag_type pm_clk_sleep_src_status(uint8 pmic_chip, pm_clk_type periph, pm_on_off_type *on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_clk_data_type *clk_ptr = pm_clk_get_data(pmic_chip);
    pm_register_address_type reg = 0;
    pm_register_data_type data = 0;
    uint32 periph_index = 0;
    uint8 mask = 0x80;

    if ((clk_ptr == NULL) || (clk_ptr->comm_ptr == NULL) || (periph != PM_CLK_SLEEP))
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if (on_off == NULL)
    {
        return PM_ERR_FLAG__INVALID_POINTER;
    }

    periph_index = clk_ptr->clk_perph_index[periph];

    if((periph_index == PM_CLK_INVALID_DATA) || (periph_index >= PM_MAX_NUM_CLKS) || (periph_index >= clk_ptr->num_of_peripherals) || 
       (clk_ptr->periph_subtype[periph_index] != PM_HW_MODULE_CLOCK_SLP_CLK))
    {
        return PM_ERR_FLAG__INVALID_CLK_INDEXED;
    }

    reg = clk_ptr->clk_reg_table->base_address + (pm_register_address_type)(clk_ptr->clk_reg_table->peripheral_offset*periph_index) + 
          clk_ptr->clk_sleep_reg_table->SLPSRC_CTRL1;

    err_flag = pm_comm_read_byte(clk_ptr->comm_ptr->slave_id, reg, &data, 0);

    if (err_flag != PM_ERR_FLAG__SUCCESS)
    {
        *on_off = PM_INVALID;
        return err_flag;
    }

    if (data & mask)
    {
        *on_off = PM_ON;
    }
    else
    {
        *on_off = PM_OFF;
    }

    return err_flag;
}

