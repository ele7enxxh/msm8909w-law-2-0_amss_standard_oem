#ifndef PM_GPIO_DRIVER__H
#define PM_GPIO_DRIVER__H

/*! \file
 *  \n
 *  \brief  pm_gpio_driver.h
 *  \details  This file contains functions prototypes and variable/type/constant
 *  declarations for supporting GPIO pin services for the Qualcomm
 *  PMIC chip set.
 *  \n &copy; Copyright 2010-2013 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/drivers/gpio/src/pm_gpio_driver.h#1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
07/05/12    hs      Updated the driver.
04/10/12    hs      Changed the type for internalResourceIndex from int to uint8.
03/01/12    hs      Updated the driver.
08/18/11    wra     Fixing Klocwork Warnings
01/12/11    wra     Added structure for common settings and changed the name of
                    the structure containing mask data gpioDS --> gpioMaskDS
11/01/10    APU     Porting code over from 7x30
========================================================================== */

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "CoreVerify.h"

#include "pm_err_flags.h"
#include "pm_dal_prop_ids.h"
#include "pm_target_information.h"
#include "pm_malloc.h"


/*===========================================================================

                     TYPE DEFINITIONS

===========================================================================*/
typedef struct pm_gpio_register_info_type
{
    pm_register_address_type    base_address;
    pm_register_address_type    peripheral_offset;
    pm_register_address_type    status;                //0x08
    pm_register_address_type    int_rt_sts;            //0x10
    pm_register_address_type    int_set_type;          //0x11
    pm_register_address_type    int_polarity_high;     //0x12
    pm_register_address_type    int_polarity_low;      //0x13
    pm_register_address_type    int_latched_clr;       //0x14
    pm_register_address_type    int_en_set;            //0x15
    pm_register_address_type    int_en_clr;            //0x16
    pm_register_address_type    int_latched_sts;       //0x18
    pm_register_address_type    int_pending_sts;       //0x19
    pm_register_address_type    int_mid_sel;           //0x1A
    pm_register_address_type    int_priority;          //0x1B
    pm_register_address_type    mode_ctl;              //0x40
    pm_register_address_type    dig_pull_ctl;          //0x42
    pm_register_address_type    dig_in_ctl;            //0x43
    pm_register_address_type    dig_out_ctl;           //0x45
    pm_register_address_type    dig_vin_ctl;           //0x41
    pm_register_address_type    en_ctl;                //0x46
}pm_gpio_register_info_type;

/* GPIO config type */
typedef enum pm_drv_gpio_config_type
{
    PM_GPIO_DIG_IN, // - 0: Digital Input
    PM_GPIO_DIG_OUT, //- 1: Digital Output
    PM_GPIO_DIG_IN_DIG_OUT, //- 2: Digital Input and Digital Output
    PM_GPIO_TYPE_INVALID
} pm_drv_gpio_config_type;

typedef struct pm_gpio_data_type
{
    pm_comm_info_type        *comm_ptr;
    pm_gpio_register_info_type* gpio_register;
    uint8             num_of_peripherals;
}pm_gpio_data_type;

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
void pm_gpio_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_gpio_data_type* pm_gpio_get_data(uint8 pmic_index);

uint8 pm_gpio_get_num_peripherals(uint8 pmic_index);

#endif //PM_GPIO_DRIVER__H
