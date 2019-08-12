#ifndef PM_PWR_ALG__H
#define PM_PWR_ALG__H

/*! \file pm_pwr_alg.h
 *  \n
 *  \brief   
 *  \details  
 *  \n &copy; Copyright 2011-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/drivers/pwr/src/pm_pwr_alg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/06/12   hw      Rearchitecturing module driver to peripheral driver
========================================================================== */

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_err_flags.h"
#include "pmapp_npa.h"
#include "bare_metal_info.h"
#include "pm_resources_and_types.h"
#include "hw_module_type.h"
#include "CoreVerify.h"
#include "pm_comm.h"


/*===========================================================================

                      TYPES AND STRUCTURES

===========================================================================*/
#define PM_VOLT_INVALID_RANGE 255

typedef struct
{
    uint32    RangeMin;
    uint32    RangeMax;
    uint32    VStep;
}pm_pwr_volt_info_type;

typedef struct
{
    uint32                 vreg_ok_us;   // settling time for off-to-on, in us.
    uint32                 stepper_us;   // settling time per stepper, in us
    uint32                 stepper_uv;   // voltage size per stepper, in uv
}pm_pwr_settling_time_info_type;

typedef struct
{
    unsigned    start:16;
    unsigned    end:16;
}pm_pwr_range_info_type;


/* These fields are mapped to registers
   that are needed by power algorithms */
typedef struct
{
    pm_register_address_type    base_address;
    pm_register_address_type    peripheral_offset;
    pm_register_address_type    STATUS;               // STATUS 0x08
    pm_register_address_type    MODE_CTL;             // MODE_CTL 0x45 (CTRL)
    pm_register_address_type    PD_CTL;               // PD_CTL 0x48 (CTRL)
    pm_register_address_type    EN_CTL;               // EN_CTL 0x46 (CTRL)
    pm_register_address_type    VOLTAGE_CTRL1;        // VOLTAGE_CTL1 0x40 (CTRL)
    pm_register_address_type    VOLTAGE_CTRL2;        // VOLTAGE_CTL1 0x41 (CTRL)
    pm_register_address_type    VOLTAGE_CTRL3;        // VOLTAGE_CTL3 0x42 (CTRL)
    pm_register_address_type    PHASE_CNT_MAX;        // PHASE_CNT_MAX 0x54 (CTRL)
    pm_register_address_type    PWM_CURRENT_LIM_CTL;  // PWM_CURRENT_LIM_CTL 0x4A (PS)
    pm_register_address_type    CLK_DIV;              //CLK_DIV 0x50
    pm_register_address_type    QMODE_SHADOW;         // QMODE_SHADOW 0x63
    pm_register_address_type    SOFT_START_CTL;       // SOFT_START_CTL 0x60
    pm_register_address_type    VOLTAGE_STEP_CTL;     // VOLTAGE_STEP_CTL 0x61
    pm_register_address_type    QM_MODE;              // QM_MODE 0x51
    pm_register_address_type    SEC_ACCESS;           // SEC_ACCESS 0xD0
    pm_register_address_type    PERPH_SUBTYPE;        // PERPH_SUBTYPE 0x0005
    pm_register_address_type    PERPH_RESET_CTL4;     // PERPH_RESET_CTL4 0xDB
} pm_pwr_register_info_type;




typedef struct
{
    pm_pwr_volt_info_type         *pwr_vset;
    pm_register_address_type      periph_base_address;
    uint8                         periph_type;
    uint8                         pwr_range;
}pm_pwr_specific_info_type;

typedef struct
{
    pm_pwr_register_info_type     *pwr_reg_table;
    uint32                        num_of_peripherals;
    pm_pwr_specific_info_type     *pwr_specific_info;
}pm_pwr_data_type;

/*===========================================================================

                     FUNCTION DECLARATIONS

===========================================================================*/

pm_err_flag_type pm_pwr_sw_mode_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_sw_mode_type *sw_mode);

pm_err_flag_type pm_pwr_sw_enable_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_on_off_type on_off);

pm_err_flag_type pm_pwr_sw_enable_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_on_off_type *on_off);

pm_err_flag_type pm_pwr_sw_enable_vreg_ok_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, boolean *vreg_ok);

pm_err_flag_type pm_pwr_pin_ctrl_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_on_off_type *on_off, uint8 *select_pin);

pm_err_flag_type pm_pwr_volt_level_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_volt_level_type *volt_level);

pm_err_flag_type pm_pwr_soft_reset_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, boolean *status);

pm_err_flag_type pm_pwr_soft_reset_exit_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index);

pm_err_flag_type pm_pwr_volt_level_stepper_done_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, boolean *stepper_done);

pm_err_flag_type pm_pwr_volt_calculate_vset_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8  peripheral_index, pm_volt_level_type  volt_level, uint32* vset);

pm_err_flag_type pm_pwr_volt_level_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8  peripheral_index, pm_volt_level_type volt_level);

#endif /* PM_PWR_ALG__H */
