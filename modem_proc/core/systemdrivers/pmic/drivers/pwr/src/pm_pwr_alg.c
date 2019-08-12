/*! \file pm_pwr_alg.c 
*  \n
*  \brief  
*  \n  
*  \n &copy; Copyright 2012-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/drivers/pwr/src/pm_pwr_alg.c#1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
12/06/12   hw      Rearchitecturing module driver to peripheral driver
========================================================================== */

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_pwr_alg.h"


/*===========================================================================

                     FUNCTION IMPLEMENTATION

===========================================================================*/

pm_err_flag_type pm_pwr_sw_mode_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_sw_mode_type *sw_mode)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    if ( (peripheral_index >= pwr_data->num_of_peripherals) ||
         (0 == pwr_data->pwr_specific_info[peripheral_index].periph_type) )
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }
    else if (sw_mode == NULL)
    {
        err_flag = PM_ERR_FLAG__INVALID_POINTER;
    }
    else
    {
        pm_register_address_type           reg = 0x0;
        pm_register_data_type              data = 0xFF;

        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->MODE_CTL);

        err_flag = pm_comm_read_byte(comm_ptr->slave_id, reg, &data, 0);

        if (err_flag != PM_ERR_FLAG__SUCCESS)
        {
            *sw_mode = PM_SW_MODE_INVALID;
            return err_flag;
        }

        /* BYPASS supercedes NPM & PFM */
        /* Only valid for LDO's */
        if (data & 0x20)
        {
            *sw_mode = PM_SW_MODE_BYPASS;
        }
        // NPM supercedes AUTO &  PFM
        else if (data & 0x80)
        {
            *sw_mode = PM_SW_MODE_NPM;
        }
        // AUTO supercedes  PFM
        // only valid for SMPS
        else if (data & 0x40)
        {
            *sw_mode = PM_SW_MODE_AUTO;
        }
        else // IT MIGHT BE PFM DEPENDING ON pc_mode bits
        {
            *sw_mode = PM_SW_MODE_LPM;
        }
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_sw_enable_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_on_off_type on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    if((pwr_data == NULL) || (comm_ptr == NULL) || (on_off >= PM_INVALID))
    {
        return PM_ERR_FLAG__INVALID_POINTER;
    }
    if ( (peripheral_index >= pwr_data->num_of_peripherals) ||
         (0 == pwr_data->pwr_specific_info[peripheral_index].periph_type) )
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }
    else
    {
        pm_register_address_type reg = 0x0;

        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->EN_CTL);

        if (PM_OFF == on_off)
        {
            err_flag = pm_comm_write_byte_mask(comm_ptr->slave_id, reg, 0x80, 0, 0);
        }
        else
        {
            err_flag = pm_comm_write_byte_mask(comm_ptr->slave_id, reg, 0x80, 0x80, 0);
        }
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_sw_enable_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_on_off_type *on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

     if((pwr_data == NULL) || (comm_ptr == NULL) || (on_off == NULL))
    {
        return PM_ERR_FLAG__INVALID_POINTER;
    }
    if ( (peripheral_index >= pwr_data->num_of_peripherals) ||
         (0 == pwr_data->pwr_specific_info[peripheral_index].periph_type) )
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED ;
    }
    else
    {
        pm_register_address_type           reg = 0x0;
        pm_register_data_type              data = 0;

        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->EN_CTL);

        err_flag = pm_comm_read_byte(comm_ptr->slave_id, reg, &data, 0);
        if (err_flag != PM_ERR_FLAG__SUCCESS)
        {
            *on_off = PM_INVALID;
            return err_flag;
        }

        if (data & 0x80)
        {
            *on_off = PM_ON;
        }
        else
        {
            *on_off = PM_OFF;
        }
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_sw_enable_vreg_ok_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, boolean *vreg_ok)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    if ((peripheral_index >= pwr_data->num_of_peripherals) || (pwr_data->pwr_specific_info[peripheral_index].periph_type == 0))
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }
    else if (vreg_ok == NULL)
    {
        err_flag = PM_ERR_FLAG__INVALID_POINTER;
    }
    else
    {
        pm_register_address_type reg = 0x0;
        pm_register_data_type data = 0;

        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->STATUS);

        err_flag = pm_comm_read_byte(comm_ptr->slave_id, reg, &data, 0);
        if (err_flag != PM_ERR_FLAG__SUCCESS)
        {
            *vreg_ok = FALSE;
            return err_flag;
        }

        if (data & 0x80)
        {
            *vreg_ok = TRUE;
        }
        else
        {
            *vreg_ok = FALSE;
        }
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_pin_ctrl_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_on_off_type *on_off, uint8 *select_pin)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    if((pwr_data == NULL) || (comm_ptr == NULL) || (on_off == NULL) || (select_pin == NULL))
    {
        return PM_ERR_FLAG__INVALID_POINTER;
    }
    if ( (peripheral_index >= pwr_data->num_of_peripherals) ||
         (0 == pwr_data->pwr_specific_info[peripheral_index].periph_type) )
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type           reg = 0x0;
        pm_register_data_type              data = 0;

        if(pwr_data->pwr_reg_table == NULL)
        {
            return PM_ERR_FLAG__INVALID_POINTER;
        }

        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->EN_CTL);

        err_flag = pm_comm_read_byte(comm_ptr->slave_id, reg, &data, 0);
        if(err_flag != PM_ERR_FLAG__SUCCESS)
        {
            *on_off = PM_INVALID;
            return err_flag;
        }

        *select_pin = (uint8)(data & 0x0F);

        if(*select_pin != 0)
        {
            *on_off = PM_ON;
        }
        else
        {
            *on_off = PM_OFF;
        }
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_volt_level_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8  peripheral_index, pm_volt_level_type *volt_level)
{
    pm_err_flag_type                 err_flag = PM_ERR_FLAG__SUCCESS;
    uint32                           vmin = 0; // microvolt
    uint32                           vStep = 0; //microvolt
    uint32                           range = 0; // 0 to 4
    pm_register_data_type            vset = 0;
    pm_register_address_type         reg = 0;
    pm_register_data_type            reg_data[2];

    if ( (peripheral_index >= pwr_data->num_of_peripherals) ||
         (0 == pwr_data->pwr_specific_info[peripheral_index].periph_type) )
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }
    else if (volt_level == NULL)
    {
        err_flag = PM_ERR_FLAG__INVALID_POINTER;
    }
    else
    {
        // Read VOLTAGE_CTRLx to calculate Voltage set
        reg =  (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->VOLTAGE_CTRL1);

        err_flag = pm_comm_read_byte_array(comm_ptr->slave_id, reg, 2, reg_data, 0);

        if ( err_flag == PM_ERR_FLAG__SUCCESS )
        {
            vset = reg_data[1];

            if ( (PM_HW_MODULE_ULT_BUCK == pwr_data->pwr_specific_info[peripheral_index].periph_type) ||
                 (PM_HW_MODULE_ULT_LDO == pwr_data->pwr_specific_info[peripheral_index].periph_type) )
            {
                range = pwr_data->pwr_specific_info[peripheral_index].pwr_range;
                if (1 == range)
                {
                    vset &= 0x1F;
                }
            }
            else
            {
                range = reg_data[0];
            }

            vmin = pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].RangeMin; // microvolt
            vStep = pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].VStep; //microvolt

            *volt_level = vmin + vset * vStep;
        }
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_soft_reset_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, boolean*  status)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    if((pwr_data == NULL) || (comm_ptr == NULL) || (status == NULL))
    {
        return PM_ERR_FLAG__INVALID_POINTER;
    }

    if ( (peripheral_index >= pwr_data->num_of_peripherals) ||
         (0 == pwr_data->pwr_specific_info[peripheral_index].periph_type) )
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type           reg = 0x0;
        pm_register_data_type              data = 0;

        if(pwr_data->pwr_reg_table == NULL)
        {
            return PM_ERR_FLAG__INVALID_POINTER;
        }

        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->PERPH_RESET_CTL4);

        err_flag = pm_comm_read_byte(comm_ptr->slave_id, reg, &data, 0);
        if(err_flag != PM_ERR_FLAG__SUCCESS)
        {
            return err_flag;
        }

        if(data & 0x01)
        {
            *status = TRUE;
        }
        else
        {
            *status = FALSE;
        }
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_soft_reset_exit_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    if((pwr_data == NULL) || (comm_ptr == NULL))
    {
        return PM_ERR_FLAG__INVALID_POINTER;
    }

    if ( (peripheral_index >= pwr_data->num_of_peripherals) ||
         (0 == pwr_data->pwr_specific_info[peripheral_index].periph_type) )
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        pm_register_address_type           sec_reg = 0x0;
        pm_register_address_type           perph_reg = 0x0;
        pm_register_data_type              sec_data = 0xA5;
        pm_register_data_type              perph_data = 0;

        if(pwr_data->pwr_reg_table == NULL)
        {
            return PM_ERR_FLAG__INVALID_POINTER;
        }

        sec_reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->SEC_ACCESS);
        perph_reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->PERPH_RESET_CTL4);

        /* Write 0xA5 to sec_access register to get write access to protected registers */
        err_flag = pm_comm_write_byte(comm_ptr->slave_id, sec_reg, sec_data, 0);
        if(err_flag != PM_ERR_FLAG__SUCCESS)
        {
            return err_flag;
        }

        /* Write 0 to perph_ctl4 register to get power rail out of soft reset */
        err_flag = pm_comm_write_byte(comm_ptr->slave_id, perph_reg, perph_data, 0);
        if(err_flag != PM_ERR_FLAG__SUCCESS)
        {
            return err_flag;
        }
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_volt_level_stepper_done_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8  peripheral_index, boolean *stepper_done)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    if ( (peripheral_index >= pwr_data->num_of_peripherals) ||
         (0 == pwr_data->pwr_specific_info[peripheral_index].periph_type) )
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }
    else if (stepper_done == NULL)
    {
        err_flag = PM_ERR_FLAG__INVALID_POINTER;
    }
    else
    {
        pm_register_address_type          reg = 0;
        pm_register_data_type             data = 0;

        // get the voltage level LUT
        reg =  (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->STATUS);

        err_flag = pm_comm_read_byte(comm_ptr->slave_id, reg, &data, 0);
        if (err_flag != PM_ERR_FLAG__SUCCESS)
        {
            *stepper_done = FALSE;
            return err_flag;
        }

        if (data & 0x01)
        {
            *stepper_done = TRUE;
        }
        else
        {
            *stepper_done = FALSE;
        }
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_volt_calculate_vset_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8  peripheral_index, pm_volt_level_type volt_level, uint32 *vset)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint32           vmin = 0; // microvolt
    uint32           vStep = 0; //microvolt
    uint32           range = 0; //microvolt

    if ( (peripheral_index >= pwr_data->num_of_peripherals) ||
         (0 == pwr_data->pwr_specific_info[peripheral_index].periph_type) )
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }
    else if (vset == NULL)
    {
        err_flag = PM_ERR_FLAG__INVALID_POINTER;
    }
    else
    {
        range = pwr_data->pwr_specific_info[peripheral_index].pwr_range;
        //If the range read failed in init , return error
        if (range == PM_VOLT_INVALID_RANGE)
        {
            //Think about proper error code, this error is due
            //to SPMI read failure but that happened at init
            err_flag = PM_ERR_FLAG__VLEVEL_OUT_OF_RANGE;
            return err_flag;
        }

        if ((volt_level <= pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].RangeMax) &&
            (volt_level >= pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].RangeMin))
        {
            vmin = pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].RangeMin;
            vStep = pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].VStep;

        }

        if (vStep > 0)
        {
            // Calculate VSET
            *vset = (pm_register_data_type)((volt_level - vmin)/vStep);

            if ((1 == range) && (PM_HW_MODULE_ULT_BUCK == pwr_data->pwr_specific_info[peripheral_index].periph_type))
            {
                *vset |= 0x60;
            }

        }
        else
        {
            err_flag = PM_ERR_FLAG__VLEVEL_OUT_OF_RANGE;
        }
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_volt_level_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_volt_level_type  volt_level)
{
    pm_err_flag_type                  err_flag = PM_ERR_FLAG__SUCCESS;
    uint32                            vmin = 0;  // microvolt
    uint32                            vStep = 0; // microvolt
    uint32                            range = 0; // 0 to 4
    pm_register_data_type             vset = 0;
    pm_register_address_type          reg = 0;
    uint8                             mask = 0xFF;

    if ( (peripheral_index >= pwr_data->num_of_peripherals) ||
         (0 == pwr_data->pwr_specific_info[peripheral_index].periph_type) )
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED ;
    }
    else
    {
        range = pwr_data->pwr_specific_info[peripheral_index].pwr_range;
           //If the range read failed in init , return error
        if (range == PM_VOLT_INVALID_RANGE )
        {
          //Think about proper error code, this error is due
          //to SPMI read failure but that happened at init
           err_flag = PM_ERR_FLAG__VLEVEL_OUT_OF_RANGE;
           return err_flag;
        }

        if ((volt_level <= pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].RangeMax) &&
            (volt_level >= pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].RangeMin) )
        {
            vmin = pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].RangeMin;
            vStep = pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].VStep;
        }

        if (vStep > 0)
        {
            // Calculate VSET
            vset = (pm_register_data_type)((volt_level - vmin)/vStep);

             if ((1 == range) && (PM_HW_MODULE_ULT_BUCK == pwr_data->pwr_specific_info[peripheral_index].periph_type))
             {
                 mask = 0x1F;    // since bit<5> and bit<6> implies range which shouldn’t be changed, bit <7> is unused.
             }

             // Set VSET
             reg =  (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->VOLTAGE_CTRL2);
             // Set the vset then
             err_flag = pm_comm_write_byte_mask(comm_ptr->slave_id, reg, mask, vset , 0) ; // 0:7
        }
        else
        {
            err_flag = PM_ERR_FLAG__VLEVEL_OUT_OF_RANGE;
        }
    }

    return err_flag;
}