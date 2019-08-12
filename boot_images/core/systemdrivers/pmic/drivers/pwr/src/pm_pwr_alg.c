/*! \file pm_pwr_alg.c 
*  \n
*  \brief  
*  \n  
*  \n &copy; Copyright 2012-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/pwr/src/pm_pwr_alg.c#1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
02/06/15    akt     Updated the mask to read proper VREG_OK bit (CR-792283)
07/16/14    akm     Comm change Updates
03/31/14    akm     Updated to the latest PMIC driver architecture   
12/06/12    hw      Rearchitecturing module driver to peripheral driver
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_pwr_alg.h"
#include "pm_comm.h"

/*===========================================================================

                     FUNCTION IMPLEMENTATION 

===========================================================================*/

pm_err_flag_type pm_pwr_pull_down_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_on_off_type on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    
    if (peripheral_index >= pwr_data->num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }
    else if (on_off >= PM_INVALID)
    {
        err_flag = PM_ERR_FLAG__MACRO_NOT_RECOGNIZED;
    }
    else
    {
        pm_register_address_type          reg = 0xFF;
        pm_register_data_type             data = 0xFF;
    
        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->PD_CTL);
    
        data = (PM_OFF == on_off) ? 0 : 0x80;

        err_flag = pm_comm_write_byte_mask(comm_ptr->slave_id, reg, 0x80, data, 0);
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_sw_mode_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_sw_mode_type *sw_mode)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
                                                          
    if (peripheral_index >= pwr_data->num_of_peripherals)
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
        
        if (err_flag == PM_ERR_FLAG__SUCCESS)
        {
            /* BYPASS supercedes NPM & PFM */
            /* Only valid for LDO's */
            if (data & 0x20)
            {
                *sw_mode = PM_SW_MODE_BYPASS;
            }
            /* NPM supercedes AUTO & PFM */
            else if (data & 0x80)
            {
                *sw_mode = PM_SW_MODE_NPM;
            }
            /* AUTO supercedes PFM */
            /* only valid for SMPS */
            else if (data & 0x40)
            {
                *sw_mode = PM_SW_MODE_AUTO;
            }
            else /* IT MIGHT BE PFM DEPENDING ON pc_mode bits */
            {
                *sw_mode = PM_SW_MODE_LPM;
            }
        }
        else
        {
            *sw_mode = PM_SW_MODE_INVALID;
        }
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_pin_ctrl_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, uint8 select_pin)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    if (peripheral_index < pwr_data->num_of_peripherals)
    {
        pm_register_address_type           reg = 0x0;

        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->EN_CTL);

        err_flag = pm_comm_write_byte_mask(comm_ptr->slave_id, reg, 0x0F, (pm_register_data_type)select_pin, 1); // bit<0:3>
    }
    else
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }

    return err_flag;
}

pm_err_flag_type pm_pwr_sw_enable_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_on_off_type on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    if (peripheral_index >= pwr_data->num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }
    else if (on_off >= PM_INVALID)
    {
        err_flag = PM_ERR_FLAG__MACRO_NOT_RECOGNIZED;
    }
    else
    {
        pm_register_address_type           reg = 0x0;
            
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

pm_err_flag_type pm_pwr_volt_level_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, pm_volt_level_type  volt_level)
{
    pm_err_flag_type                  err_flag = PM_ERR_FLAG__SUCCESS;
    uint32                            vmin = 0;  // microvolt
    uint32                            vStep = 0; // microvolt
    uint32                            range = 0; // 0 to 4
    pm_register_data_type             vset = 0;
    pm_register_data_type             data = 0;
    pm_register_data_type             curr_range = 0;
    pm_register_address_type          status_reg = 0;
    pm_register_address_type          reg = 0;
    uint8                             periph_type = 0;
    

    if (peripheral_index >= (pwr_data->num_of_peripherals))
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED ;
    }
    else
    {
        periph_type = pwr_data->pwr_specific_info[peripheral_index].periph_type;

        // The range register VOLT_CTL1 is not applicable for ULT rails
        if ((PM_HW_MODULE_ULT_BUCK != periph_type) && (PM_HW_MODULE_ULT_LDO != periph_type))
        {
            // Set Range reg
            reg = pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->VOLTAGE_CTRL1;

            // Get the current range
            err_flag = pm_comm_read_byte(comm_ptr->slave_id, reg, &curr_range, 0);
        }
        else if (PM_HW_MODULE_ULT_BUCK == periph_type)
        {
            // Set Range reg
            reg = pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->VOLTAGE_CTRL2;

            // Get the current range
            err_flag = pm_comm_read_byte(comm_ptr->slave_id, reg, &data, 0);

            // Extracting bit 6 and bit 5 of VOLTAGE_CTRL2 Register to determine the range set
            data &=  0x60;
            if (0x60 == data)
            {
                curr_range = 1;
            }
        }

        if(PM_ERR_FLAG__SUCCESS != err_flag)
        {
            return err_flag;
        }

        if( (volt_level <= pwr_data->pwr_specific_info[peripheral_index].pwr_vset[curr_range].RangeMax) &&
            (volt_level >= pwr_data->pwr_specific_info[peripheral_index].pwr_vset[curr_range].RangeMin) )
        {
            vmin = pwr_data->pwr_specific_info[peripheral_index].pwr_vset[curr_range].RangeMin;
            vStep = pwr_data->pwr_specific_info[peripheral_index].pwr_vset[curr_range].VStep;
            range = curr_range;
        }
        else
        {
            for(range = pwr_data->pwr_specific_info[peripheral_index].pwr_range->start; range <= pwr_data->pwr_specific_info[peripheral_index].pwr_range->end; range++)
            {
                if( (volt_level <= pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].RangeMax) && 
                    (volt_level >= pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].RangeMin) )
                {
                    vmin = pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].RangeMin;
                    vStep = pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].VStep;
                    break;
                }
            }
        }
       
        if (vStep > 0)
        {
            //calculate VSET
            vset = (pm_register_data_type)((volt_level - vmin)/vStep);

            // Set the range first, only if the current range is not equal to requested range
            if(curr_range != ((pm_register_data_type)range))
            {
                status_reg = pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->STATUS;

                data = 0;

                //Read the Vreg OK status
                err_flag = pm_comm_read_byte(comm_ptr->slave_id, status_reg, &data, 0);

                if(PM_ERR_FLAG__SUCCESS != err_flag)
                {
                    return err_flag;
                }

                //New range cannot be set if Power rail is ON
                if(data & 0x80)
                {
                    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
                }

                // The range register VOLT_CTL1 update is not applicable for ULT rails
                if ((PM_HW_MODULE_ULT_BUCK != periph_type) && (PM_HW_MODULE_ULT_LDO != periph_type))
                {
                    // Set Range reg
                    reg = pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->VOLTAGE_CTRL1;

                    //Set the new range
                    err_flag = pm_comm_write_byte_mask(comm_ptr->slave_id, reg, 0x07, (pm_register_data_type)range, 0) ; // 0:2

                    if(PM_ERR_FLAG__SUCCESS != err_flag)
                    {
                        return err_flag;
                    }
                }
            }

             // Set VSET
             reg =  (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->VOLTAGE_CTRL2);

             // Update vset value for ULT buck range 1 cases
             if ((PM_HW_MODULE_ULT_BUCK == periph_type) && (1 == range))
             {
                 // since bit<5> and bit<6> are used for range
                 vset |= 0x60; 
             }

             // Set vset
             err_flag = pm_comm_write_byte(comm_ptr->slave_id, reg, vset, 0);
        }
        else
        {
            err_flag = PM_ERR_FLAG__VLEVEL_OUT_OF_RANGE;
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
    uint8                            periph_type = 0;

    if (peripheral_index >= (pwr_data->num_of_peripherals))
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }
    else if (volt_level == NULL)
    {
        err_flag = PM_ERR_FLAG__INVALID_POINTER;
    }
    else
    {
        periph_type = pwr_data->pwr_specific_info[peripheral_index].periph_type;

        // get the voltage level LUT
        reg =  (pm_register_address_type)(pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->VOLTAGE_CTRL1);

        err_flag = pm_comm_read_byte_array(comm_ptr->slave_id, reg, 2, reg_data, 0);

        if ( err_flag == PM_ERR_FLAG__SUCCESS )
        {
            // The range register VOLT_CTL1 is not applicable for ULT rails
            if ((PM_HW_MODULE_ULT_BUCK != periph_type) && (PM_HW_MODULE_ULT_LDO != periph_type))
            {
                range = reg_data[0];
            }
            else if (PM_HW_MODULE_ULT_BUCK == periph_type)
            {
                // Extracting bit 6 and bit 5 of VOLTAGE_CTRL2 Register to determine the range set
                if (0x60 == (reg_data[1] & 0x60))
                {
                    range = 1;
                }
            }

            vset = reg_data[1];
            if ((1 == range) && (PM_HW_MODULE_ULT_BUCK == periph_type))
            {
                vset &= 0x1F;
            }
            vmin = pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].RangeMin; // microvolt
            vStep = pwr_data->pwr_specific_info[peripheral_index].pwr_vset[range].VStep; //microvolt

            *volt_level = vmin + vset * vStep;
        }
    }

    return err_flag;
}

pm_err_flag_type 
pm_pwr_vreg_ok_status_alg(pm_pwr_data_type *pwr_data, pm_comm_info_type *comm_ptr, uint8 peripheral_index, boolean *status)
{
    pm_err_flag_type                 err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type         status_reg = 0;
    pm_register_data_type            stepper_done_data = 0;
    pm_register_data_type            vreg_done_data = 0;
    pm_register_data_type            data = 0;

    if (peripheral_index >= (pwr_data->num_of_peripherals))
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED ;
    }

    status_reg = pwr_data->pwr_specific_info[peripheral_index].periph_base_address + pwr_data->pwr_reg_table->STATUS;

    //Read the Vreg OK status
    err_flag = pm_comm_read_byte(comm_ptr->slave_id, status_reg, &data, 0);

    vreg_done_data = data & 0x80;  //7-bit of STATUS-1 registers is VREG_OK

    // LDOs with stepper done 
    if ((0x07 == pwr_data->pwr_reg_table->PERIPH_SUBTYPE) && (0x04 == pwr_data->pwr_reg_table->PERIPH_TYPE))
    {
        stepper_done_data = data & 0x01; //0-bit of STATUS-1 registers is STEPPER_DONE for NMOS LDOs only
        if ((0 !=  vreg_done_data) && (0 !=  stepper_done_data))
        {
           *status = TRUE;
        }
        else
        {
            *status = FALSE;
        }
    }
    else
    {
        if (0 != vreg_done_data)
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

