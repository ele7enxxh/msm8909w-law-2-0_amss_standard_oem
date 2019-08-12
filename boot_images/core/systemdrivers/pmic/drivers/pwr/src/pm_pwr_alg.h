#ifndef PM_PWR_ALG__H
#define PM_PWR_ALG__H

/*! \file pm_pwr_alg.h
 *  \n
 *  \brief   
 *  \details  
 *  \n &copy; Copyright 2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/pwr/src/pm_pwr_alg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/14   akm     Comm change Updates
14/11/13   sv      Added Enum "SEC_ACCESS" to struct pm_pwr_register_info_typ (CR-567633)
12/06/12   hw      Rearchitecturing module driver to peripheral driver
========================================================================== */

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_utils.h"
#include "bare_metal_info.h"
#include "pm_resources_and_types.h"
#include "hw_module_type.h"
#include "CoreVerify.h"
#include "pm_comm.h"


/*===========================================================================

                      TYPES AND STRUCTURES 

===========================================================================*/

typedef struct
{
    unsigned    start:16;
    unsigned    end:16;
}pm_pwr_range_info_type;

typedef struct
{
    uint32    RangeMin;
    uint32    RangeMax;
    uint32    VStep;			
}pm_pwr_volt_info_type;

/* These fields are mapped to registers that are needed by power algorithms */
typedef struct 
{
    pm_register_address_type    base_address;
    pm_register_address_type    peripheral_offset;
    pm_register_address_type    STATUS;      // STATUS 0x08 
    pm_register_address_type    MODE_CTL;      // MODE_CTL 0x45 
    pm_register_address_type    PD_CTL;        // PD_CTL 0x48 
    pm_register_address_type    EN_CTL;        // EN_CTL 0x46 
    pm_register_address_type    VOLTAGE_CTRL1; // VOLTAGE_CTL1 0x40 
    pm_register_address_type    VOLTAGE_CTRL2; // VOLTAGE_CTL2 0x41 
    pm_register_address_type    PHASE_CNT_MAX; // PHASE_CNT_MAX 0x54 (S_CTRL)
    pm_register_address_type    FTS_PWM_CURR_LIM_CTL; // FTS_PWM_CURR_LIM_CTL 0x60 (S_PS)
    pm_register_address_type    HF_PWM_CURR_LIM_CTL; // HF_PWM_CURR_LIM_CTL 0x4A (S_PS)
    pm_register_address_type    CLK_DIV;  //CLK_DIV 0x50 (S_CTRL, S_FREQ)
    pm_register_address_type    QMODE_SHADOW; // QMODE_SHADOW 0x63
    pm_register_address_type    SOFT_START_CTL; //SOFT_START_CTL 0x4C
    pm_register_address_type    QM_MODE; // QM_MODE 0x51 (BCLK_GEN_MAIN)
    pm_register_address_type    SEC_ACCESS; // SEC_ACCESS 0xD0
    pm_register_address_type    PERIPH_SUBTYPE; // PERIPH_SUBTYPE 0x005
    pm_register_address_type    PERIPH_TYPE; // PERIPH_SUBTYPE 0x004
}pm_pwr_register_info_type;

typedef struct
{
    pm_pwr_volt_info_type         *pwr_vset;
    pm_pwr_range_info_type        *pwr_range;
    pm_register_address_type      periph_base_address;
    uint8                         periph_type;
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

pm_err_flag_type pm_pwr_pull_down_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_on_off_type on_off);

pm_err_flag_type pm_pwr_sw_mode_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_sw_mode_type *mode);

pm_err_flag_type pm_pwr_pin_ctrl_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, uint8 select_pin);

pm_err_flag_type pm_pwr_sw_enable_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_on_off_type on_off);

pm_err_flag_type pm_pwr_volt_level_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_volt_level_type volt_level);

pm_err_flag_type pm_pwr_volt_level_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_volt_level_type *volt_level);

pm_err_flag_type pm_pwr_vreg_ok_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, boolean *status);


#endif /* PM_PWR_ALG__H */
