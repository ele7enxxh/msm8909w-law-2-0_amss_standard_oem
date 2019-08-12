#ifndef PM_CLK_DRIVER__H
#define PM_CLK_DRIVER__H

/*! \file pm_clk_driver.h
 *  \n
 *  \brief This file contains CLOCK peripheral driver related function prototypes,
 *         enums and driver data structure type.   
 *  \n  
 *  \n &copy; Copyright 2013-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/drivers/clk/src/pm_clk_driver.h#1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
04/12/13    aks      Created as part of Code refactoring.
========================================================================== */
/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "pm_clk.h"
#include "pm_err_flags.h"
#include "hw_module_type.h"
#include "CoreVerify.h"
#include "pm_comm.h"
#include "pm_utils.h"

/*===========================================================================

                     STRUCTURE TYPE AND ENUM

===========================================================================*/

#define PM_CLK_INVALID_DATA   99

/* Please make sure to change the clk_info and clk_dep info arrays in
   config folder when the below macro is changed */
#define PM_MAX_NUM_CLKS       14

typedef struct 
{ 
    pm_register_address_type    base_address;  
    pm_register_address_type    peripheral_offset;
    pm_register_address_type    DRV_CTL1;   //0x44
    pm_register_address_type    EN_CTL;     //0x46
    pm_register_address_type    PD_CTL;    //0x48
}pm_clk_register_info_type;

typedef struct
{
    pm_register_address_type    SLPSRC_CTRL1;       
}pm_clk_sleep_register_info_type;

typedef struct
{
    pm_register_address_type    TIMER1; 
}pm_clk_xo_register_info_type;

typedef struct 
{
    pm_comm_info_type               *comm_ptr;
    uint32                          num_of_peripherals;
    uint8                           periph_subtype[PM_MAX_NUM_CLKS];
    //array of clk peripheral index  to get to right offset from base
    uint8                           clk_perph_index[PM_ALL_CLKS];
    pm_clk_type                     *clk_common;
    pm_clk_register_info_type       *clk_reg_table;
    pm_clk_sleep_register_info_type *clk_sleep_reg_table;
    pm_clk_xo_register_info_type    *clk_xo_reg_table;
}pm_clk_data_type;

/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/
void pm_clk_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_clk_data_type* pm_clk_get_data(uint8 pmic_index);

uint8 pm_clk_get_num_peripherals(uint8 pmic_index);


#endif /* PM_CLK_DRIVER_H */


