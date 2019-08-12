/*! \file pm_clk_xo.c
*  \n
*  \brief Implementation file for CLK XO public APIs.
*  \n  
*  &copy; Copyright 2013-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/drivers/clk/src/pm_clk_xo.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/20/13   aks      Created as a part of code re-architecture
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_clk_xo.h"
#include "pm_clk_driver.h"
#include "pm_comm.h"


/*===========================================================================

                     STRUCTURE TYPE AND ENUM

===========================================================================*/

/* Max TCXO Warm up timer value (also Mask for the same) */
#define PM_XO_MAX_TCXO_WARMUP_TIMER            0x7FF

/*===========================================================================

                     API IMPLEMENTATION 

===========================================================================*/
pm_err_flag_type pm_clk_xo_set_warmup_time(uint8 pmic_chip, pm_clk_type periph, uint32 timer_value)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_clk_data_type *clk_ptr = pm_clk_get_data(pmic_chip);
    pm_register_address_type reg = 0;
    uint32 periph_index = 0;

    if ((clk_ptr == NULL) || (clk_ptr->comm_ptr == NULL) || (periph != PM_CLK_XO))
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    periph_index = clk_ptr->clk_perph_index[periph];

    if((periph_index == PM_CLK_INVALID_DATA) || (periph_index >= PM_MAX_NUM_CLKS) || (periph_index >= clk_ptr->num_of_peripherals) || 
       (clk_ptr->periph_subtype[periph_index] != PM_HW_MODULE_CLOCK_XO_CORE))
    {
        return PM_ERR_FLAG__INVALID_CLK_INDEXED;
    }

    reg = clk_ptr->clk_reg_table->base_address + (pm_register_address_type)(clk_ptr->clk_reg_table->peripheral_offset*periph_index) + 
          clk_ptr->clk_xo_reg_table->TIMER1;

    /* If the user entered TCXO Warm up delay timer value is greater
       than max allowed, set the max value and return error */
    if (timer_value > PM_XO_MAX_TCXO_WARMUP_TIMER)
    {
        timer_value = (uint32)PM_XO_MAX_TCXO_WARMUP_TIMER;
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    err_flag |= pm_comm_write_byte_array(clk_ptr->comm_ptr->slave_id, reg, sizeof(timer_value), (uint8*)&timer_value, 0);

    return err_flag;
}

