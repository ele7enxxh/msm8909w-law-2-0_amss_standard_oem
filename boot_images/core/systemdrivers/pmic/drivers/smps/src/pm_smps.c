/*! \file pm_smps.c
*  
*  \brief Implementation file for SMPS public APIs.
*  \n  
*  &copy; Copyright 2010-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/smps/src/pm_smps.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/14   akm     Comm change Updates
05/15/14   akm     Updated SMPS ilim and freq algorithm
03/31/14   akm     Updated to the latest PMIC driver architecture   
12/06/12   hw      Rearchitecturing module driver to peripheral driver
04/09/12   hs      Changed the type for internalResourceIndex from int to uint8.
04/04/12   hs      Removed the buck switch size alg.
03/30/12   hs      Removed the dirver size alg.
04/10/11   wra     Removed the ifdef RPM declaration. RPM has it's own routing layer now
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_smps.h"
#include "pm_smps_driver.h"
#include "pm_comm.h"

/*===========================================================================

                     LOCAL FUNCTION PROTOTYPE

===========================================================================*/

/* Internal function to calculate ILIM reg data and mask based on ilim_level for HF bucks */
static pm_err_flag_type pm_smps_hf_calculate_ilim_data(uint8 smps_ps_subtype, uint32 ilim_level_milli_amps, pm_smps_ilim_mode_type smps_mode, 
                                                       pm_register_data_type *ilim_data_ptr, pm_register_mask_type *ilim_mask_ptr);

/* Internal function to calculate ILIM reg data and mask based on ilim_level for FTS bucks */
static pm_err_flag_type pm_smps_fts_calculate_ilim_data(uint8 smps_ps_subtype, uint32 ilim_level_milli_amps, pm_smps_ilim_mode_type smps_mode, 
                                                        pm_register_data_type *ilim_data_ptr, pm_register_mask_type *ilim_mask_ptr);

/*===========================================================================

                     API IMPLEMENTATION 

===========================================================================*/

pm_err_flag_type 
pm_smps_sw_mode(uint8 pmic_chip, uint8 smps_peripheral_index, pm_sw_mode_type mode) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(smps_peripheral_index >= smps_ptr->pm_pwr_data.num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }
    else if (mode >= PM_SW_MODE_INVALID)
    {
        err_flag = PM_ERR_FLAG__MACRO_NOT_RECOGNIZED;
    }
    else
    {
        pm_register_address_type          reg = 0;
        pm_register_mask_type             mask = 0;
        pm_register_data_type             data = 0;
        pm_pwr_data_type                  *pwr_data = &(smps_ptr->pm_pwr_data);
        
        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[smps_peripheral_index].periph_base_address + pwr_data->pwr_reg_table->MODE_CTL);

        switch(mode)
        {
        case PM_SW_MODE_LPM: // Low power mode
          {
              mask = 0xFF;
              data = 0x00;
          }
          break;
        case PM_SW_MODE_NPM: // normal power mode
          {
              mask = 0xC0; //bit 7 NPM and bit 6 AUTO_MODE
              data = 0x80;
          }
          break;
        case PM_SW_MODE_AUTO: // auto mode
          {
              mask = 0xC0; //bit 7 NPM and bit 6 AUTO_MODE
              data = 0x40;
          }
          break;
        default:
          err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
          break;
        }

        if (PM_ERR_FLAG__SUCCESS == err_flag)
        {
            err_flag = pm_comm_write_byte_mask(smps_ptr->comm_ptr->slave_id, reg, mask, data, 0);
        }
    }

    return err_flag;
}

pm_err_flag_type 
pm_smps_sw_mode_status(uint8 pmic_chip, uint8 smps_peripheral_index, pm_sw_mode_type* sw_mode) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_sw_mode_status_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_peripheral_index, sw_mode);
    }

    return err_flag;
}

pm_err_flag_type 
pm_smps_sw_enable(uint8 pmic_chip, uint8 smps_peripheral_index, pm_on_off_type on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_sw_enable_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_peripheral_index, on_off);
    }

    return err_flag;
}
pm_err_flag_type 
pm_smps_volt_level_status(uint8 pmic_chip, uint8 perph_index, pm_volt_level_type *volt_level)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_volt_level_status_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, perph_index, volt_level);
    }
    return err_flag;
}

pm_err_flag_type 
pm_smps_volt_level(uint8 pmic_chip, uint8 perph_index, pm_volt_level_type volt_level)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_volt_level_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, perph_index, volt_level);
    }
    return err_flag;
}

pm_err_flag_type 
pm_smps_inductor_ilim(uint8 pmic_chip, uint8 smps_peripheral_index, uint32 ilim_level_milli_amps, pm_smps_ilim_mode_type smps_mode)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(smps_peripheral_index >= smps_ptr->pm_pwr_data.num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }
	else if (smps_mode >= PM_ILIM_SMPS_MODE_INVALID)
    {
        err_flag = PM_ERR_FLAG__MACRO_NOT_RECOGNIZED;
    }
    else
    { 
        pm_register_address_type    ilim_reg = 0;
        pm_register_address_type    sec_reg = 0;
        pm_register_address_type    subtype_reg = 0;
        pm_register_address_type    smps_ctrl_base_addr = 0;
        pm_register_address_type    smps_ps_base_addr = 0;
        pm_register_address_type    smps_ps_periph_offset = 0x100;
        pm_pwr_data_type            *pwr_data = &(smps_ptr->pm_pwr_data);
        pm_register_data_type       ilim_data = 0;
        pm_register_data_type       ps_subtype_data = 0;
        pm_register_mask_type       ilim_mask = 0;

        /* SMPSx_CTRL peripheral base address */
        smps_ctrl_base_addr = pwr_data->pwr_specific_info[smps_peripheral_index].periph_base_address;

        /* SMPSx_PS peripheral base address */
        smps_ps_base_addr =  smps_ctrl_base_addr + smps_ps_periph_offset;

        /* Get SMPSx_PS peripheral subtype value */
        subtype_reg =  smps_ps_base_addr + pwr_data->pwr_reg_table->PERIPH_SUBTYPE;
        
        err_flag = pm_comm_read_byte(smps_ptr->comm_ptr->slave_id, subtype_reg, &ps_subtype_data, 0);

        if (err_flag == PM_ERR_FLAG__SUCCESS)
        {
            /* We need to set the CURRENT_LIMIT in SMPSx_PS peripheral */

            /* Register offset and current limit values are different on FTS and HF bucks */
            if (pwr_data->pwr_specific_info[smps_peripheral_index].periph_type == PM_HW_MODULE_HF_BUCK)
        {
                /* For HF, calculate ILIM reg value and mask based on the PS periph subtype
                   for the requested ILIM level */
                err_flag = pm_smps_hf_calculate_ilim_data(ps_subtype_data, ilim_level_milli_amps, smps_mode, &ilim_data, &ilim_mask);

                /* For HF, PWM_CURR_LIM_CTL reg offset is 0x4A */
                ilim_reg =  smps_ps_base_addr + pwr_data->pwr_reg_table->HF_PWM_CURR_LIM_CTL;

                /* SMPSx_PS peripheral SEC_ACCESS register address */
                sec_reg =  smps_ctrl_base_addr + pwr_data->pwr_reg_table->SEC_ACCESS;
            }
            else if (pwr_data->pwr_specific_info[smps_peripheral_index].periph_type == PM_HW_MODULE_FTS)
             {
                /* For FTS, calculate ILIM reg value and mask based on the PS periph subtype
                   for the requested ILIM level */
                err_flag = pm_smps_fts_calculate_ilim_data(ps_subtype_data, ilim_level_milli_amps, smps_mode, &ilim_data, &ilim_mask);

                /* For FTS, PWM_CURR_LIM_CTL reg offset is 0x60 */
                ilim_reg =  smps_ps_base_addr + pwr_data->pwr_reg_table->FTS_PWM_CURR_LIM_CTL;

                /* We need to write to SMPSx_CTRL peripheral SEC_ACCESS register
                   to open access to SMPSx_PS PWM_CURR_LIM_CTL reg in case of FTS */
                sec_reg =  smps_ctrl_base_addr + pwr_data->pwr_reg_table->SEC_ACCESS;
             }
             else
             {
                err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
            }
        }
           
            if (err_flag == PM_ERR_FLAG__SUCCESS)
            {
            /* PWM_CURR_LIM_CTL needs protected access. Write to SEC_ACCESS register */
            err_flag = pm_comm_write_byte(smps_ptr->comm_ptr->slave_id, sec_reg, 0xA5, 0);

            /* Write to PWM_CURR_LIM_CTL register */
            err_flag |= pm_comm_write_byte_mask(smps_ptr->comm_ptr->slave_id, ilim_reg, ilim_mask, ilim_data, 0);
             }
        }

    return err_flag;
}

pm_err_flag_type 
pm_smps_inductor_ilim_status(uint8 pmic_chip, uint8 smps_peripheral_index, uint32 *ilim_level_milli_amps, pm_smps_ilim_mode_type smps_mode)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(smps_peripheral_index >= smps_ptr->pm_pwr_data.num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }
    else if (smps_mode >= PM_ILIM_SMPS_MODE_INVALID)
    {
        err_flag = PM_ERR_FLAG__MACRO_NOT_RECOGNIZED;
    }
    else if (ilim_level_milli_amps == NULL)
    {
        err_flag = PM_ERR_FLAG__INVALID_POINTER;
    }
	else
    {
        pm_register_address_type    ilim_reg = 0;
        pm_register_address_type    subtype_reg = 0;
        pm_register_address_type    smps_ctrl_base_addr = 0;
        pm_register_address_type    smps_ps_base_addr = 0;
        pm_register_address_type    smps_ps_periph_offset = 0x100;
        pm_pwr_data_type            *pwr_data = &(smps_ptr->pm_pwr_data);
        pm_register_data_type       ilim_data = 0;
        pm_register_data_type       ps_subtype_data = 0;
        pm_register_mask_type       ilim_mask = 0;

        /* SMPSx_CTRL peripheral base address */
        smps_ctrl_base_addr = pwr_data->pwr_specific_info[smps_peripheral_index].periph_base_address;
        
        /* SMPSx_PS peripheral base address */
        smps_ps_base_addr =  smps_ctrl_base_addr + smps_ps_periph_offset;

        /* We need to set the CURRENT_LIMIT in SMPSx_PS peripheral */

        /* Register offset and current limit values are different on FTS and HF bucks */
        if (pwr_data->pwr_specific_info[smps_peripheral_index].periph_type == PM_HW_MODULE_HF_BUCK)
        {
            /* For HF, PWM_CURR_LIM_CTL reg offset is 0x4A */
            ilim_reg =  smps_ps_base_addr + pwr_data->pwr_reg_table->HF_PWM_CURR_LIM_CTL;

            /* For HF, mask is different for AUTO_MODE and PWM_MODE CURR_LIM setting */
            if (smps_mode == PM_ILIM_SMPS_AUTO_MODE)
            {
                ilim_mask = 0x38;
        }
        else
        {
                ilim_mask = 0x07;
            }

            subtype_reg =  smps_ps_base_addr + pwr_data->pwr_reg_table->PERIPH_SUBTYPE;

            /* Read PERIPH_SUBTYPE register */
            err_flag = pm_comm_read_byte(smps_ptr->comm_ptr->slave_id, subtype_reg, &ps_subtype_data, 0);

            /* Read PWM_CURR_LIM_CTL register */
            err_flag |= pm_comm_read_byte_mask(smps_ptr->comm_ptr->slave_id, ilim_reg, ilim_mask, &ilim_data,0);

            if (err_flag == PM_ERR_FLAG__SUCCESS)
            {
                if (smps_mode == PM_ILIM_SMPS_AUTO_MODE)
            {
                    ilim_data = ilim_data>>3;
            }
                /* Formula for calculating current limit on different HF SMPS_PS subtypes
                   6X --> Iplimit = 5400 mA - m*650 mA
                   3X --> Iplimit = 3500 mA - m*400 mA
                   2X --> Iplimit = 2700 mA - m*300 mA
                   where m is the bit value of iplimit_sel<2:0> */

                if (ps_subtype_data == PM_HW_MODULE_HF_BUCK_PS6X)
            {
                    *ilim_level_milli_amps = 5400 - (ilim_data * 650);
            }
                else if (ps_subtype_data == PM_HW_MODULE_HF_BUCK_PS3X)
            {
                    *ilim_level_milli_amps = 3500 - (ilim_data * 400);
            }
                else if (ps_subtype_data == PM_HW_MODULE_HF_BUCK_PS2X)
            {
                    *ilim_level_milli_amps = 2700 - (ilim_data * 300);
            }
            else
            {
                err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
            }
        }

    }
        else if (pwr_data->pwr_specific_info[smps_peripheral_index].periph_type == PM_HW_MODULE_FTS)
{
            /* For FTS, PWM_CURR_LIM_CTL reg offset is 0x60 */
            ilim_reg =  smps_ps_base_addr + pwr_data->pwr_reg_table->FTS_PWM_CURR_LIM_CTL;

            /* For FTS, no AUTO_MODE CURR_LIM setting */
            if (smps_mode == PM_ILIM_SMPS_AUTO_MODE)
    {
                err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
        }
            else
        {
                subtype_reg =  smps_ps_base_addr + pwr_data->pwr_reg_table->PERIPH_SUBTYPE;

                err_flag = pm_comm_read_byte(smps_ptr->comm_ptr->slave_id, subtype_reg, &ps_subtype_data, 0);

                if (err_flag == PM_ERR_FLAG__SUCCESS)
        {
                    /* Formula for calculating current limit on different FTS SMPS_PS subtypes
                       2.5X    --> Iplimit = m * 312 mA + 468 mA
                                   where m is the bit value of iplimit_sel<3:0>
                       1X,0.5X --> Iplimit = m * 500 mA + 1000 mA
                                   where m is the bit value of iplimit_sel<2:0>
                    */

                    if (ps_subtype_data == PM_HW_MODULE_FTS__FTS2p5_PS)
            {
                        ilim_mask = 0x0F;

                        /* Read PWM_CURR_LIM_CTL register */
                        err_flag = pm_comm_read_byte_mask(smps_ptr->comm_ptr->slave_id, ilim_reg, ilim_mask, &ilim_data,0);

                        if (err_flag == PM_ERR_FLAG__SUCCESS)
                {
                            *ilim_level_milli_amps = (ilim_data * 312) + 468;
        }
    }
                    else if ((ps_subtype_data == PM_HW_MODULE_FTS__PS1X) || (ps_subtype_data == PM_HW_MODULE_FTS__PS0p5X))
    {
                        ilim_mask = 0x07;

                        /* Read PWM_CURR_LIM_CTL register */
                        err_flag = pm_comm_read_byte_mask(smps_ptr->comm_ptr->slave_id, ilim_reg, ilim_mask, &ilim_data,0);

                        if (err_flag == PM_ERR_FLAG__SUCCESS)
        {
                            *ilim_level_milli_amps = (ilim_data * 500) + 1000;
        }
        }
        else
        {
                        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
            }    
        }
    }
    }
    else
    {
            err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
        }
    }

    return err_flag;
}

pm_err_flag_type 
pm_smps_quiet_mode(uint8 pmic_chip, uint8 smps_peripheral_index, pm_quiet_mode_type mode, uint16 voltage_mv)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);
    const uint16      vrst_min = 250; // 250mV
    const uint16      vrst_max = 800; // 800mV

    if(smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(smps_peripheral_index >= smps_ptr->pm_pwr_data.num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }
    else if (mode >= PM_QUIET_MODE__INVALID)
    {
        err_flag = PM_ERR_FLAG__MACRO_NOT_RECOGNIZED;
    }
    else if ( (voltage_mv > vrst_max) || (voltage_mv < vrst_min) )
    {
        err_flag = PM_ERR_FLAG__VLEVEL_OUT_OF_RANGE;
    }
    else
    {
        pm_register_address_type    reg = 0xFF;
        pm_register_data_type       data_vrst = 0xFF;
        pm_register_address_type    reg_comm = 0xFF;
        const uint16                vrst_step = 50; // 50mV
        pm_pwr_data_type            *pwr_data = &(smps_ptr->pm_pwr_data);
        
        reg_comm = 0x1000 + pwr_data->pwr_reg_table->QM_MODE;        
        
        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[smps_peripheral_index].periph_base_address + pwr_data->pwr_reg_table->QMODE_SHADOW);
        
        data_vrst = (pm_register_data_type)((voltage_mv - vrst_min)/vrst_step);

        switch(mode)
        {
        case PM_QUIET_MODE__DISABLE:
            {
                //QM_MODE: QM_EN bit<7> = 0
                err_flag = pm_comm_write_byte_mask(smps_ptr->comm_ptr->slave_id, reg_comm, 0x80, 0, 0); // bit<7>

                //QM_SHADOW: FOLLOW_QM bit<5> = 0 &&QMODE_PS_METHOD  bit<4> = 0
                err_flag = pm_comm_write_byte_mask(smps_ptr->comm_ptr->slave_id, reg, 0x30, 0, 0); // bit<5:4>
            }
            break;
        case PM_QUIET_MODE__QUIET:
            {
                //QM_MODE: QM_EN bit<7> = 1
                err_flag = pm_comm_write_byte_mask(smps_ptr->comm_ptr->slave_id, reg_comm, 0x80, 0x80, 0); // bit<7>

                //QM_SHADOW: FOLLOW_QM bit<5> = 1 &&QMODE_PS_METHOD  bit<4> = 0 && QMODE_PS_VRST = XXXX
                err_flag = pm_comm_write_byte_mask(smps_ptr->comm_ptr->slave_id, reg, 0x3F, (0x20|data_vrst), 0); // bit<5:0>
            }
            break;
        case PM_QUIET_MODE__SUPER_QUIET: //TODO do we need to set bit 5 as well?
            {
                //QM_MODE: QM_EN bit<7> = 1
                err_flag = pm_comm_write_byte_mask(smps_ptr->comm_ptr->slave_id, reg_comm, 0x80, 0x80, 0); // bit<7>

                //QM_SHADOW: FOLLOW_QM bit<5> = 1 &&QMODE_PS_METHOD  bit<4> = 1 && QMODE_PS_VRST = 0000
                err_flag = pm_comm_write_byte_mask(smps_ptr->comm_ptr->slave_id, reg, 0x3F, 0x30, 0); // bit<5:0>
            }
            break;
        default:
            err_flag = PM_ERR_FLAG__MACRO_NOT_RECOGNIZED;
            break;
        }
    }

    return err_flag;
}

pm_err_flag_type 
pm_smps_switching_freq(uint8 pmic_chip, uint8 smps_peripheral_index, pm_smps_switching_freq_type freq)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(smps_peripheral_index >= smps_ptr->pm_pwr_data.num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }
    else if ( freq >= PM_SWITCHING_FREQ_INVALID )
    {
        err_flag = PM_ERR_FLAG__MACRO_NOT_RECOGNIZED;
    }
    else
    {
        pm_register_address_type    reg = 0;
        pm_register_address_type    smps_ctrl_base_addr = 0;
        pm_register_address_type    smps_freq_base_addr = 0;
        pm_register_address_type    smps_freq_periph_offset = 0x200;
        pm_pwr_data_type                  *pwr_data = &(smps_ptr->pm_pwr_data);
                                                                        
        /* SMPSx_CTRL peripheral base address */
        smps_ctrl_base_addr = pwr_data->pwr_specific_info[smps_peripheral_index].periph_base_address;

        /* SMPSx_FREQ peripheral base address */
        smps_freq_base_addr =  smps_ctrl_base_addr + smps_freq_periph_offset;

        /* Implementation is different on FTS and HF bucks */
        if (pwr_data->pwr_specific_info[smps_peripheral_index].periph_type == PM_HW_MODULE_HF_BUCK)
        {
            /* We need to set the freq in SMPSx_FREQ peripheral for HF Bucks */

            /* CLK_DIV needs protected access. Write to SEC_ACCESS register */
            reg =  smps_freq_base_addr + pwr_data->pwr_reg_table->SEC_ACCESS;
		
            err_flag = pm_comm_write_byte(smps_ptr->comm_ptr->slave_id, reg, 0xA5, 0);

            if (err_flag == PM_ERR_FLAG__SUCCESS)
            {
                /* Write to CLK_DIV register */
                reg =  smps_freq_base_addr + pwr_data->pwr_reg_table->CLK_DIV;

                err_flag = pm_comm_write_byte_mask(smps_ptr->comm_ptr->slave_id, reg, 0x1F, (pm_register_data_type)freq, 0); // <0:4>
            }
    }
        else if (pwr_data->pwr_specific_info[smps_peripheral_index].periph_type == PM_HW_MODULE_FTS)
        {
            /* We need to set the freq in SMPSx_CTRL peripheral for FTS Bucks */

            /* CLK_DIV needs protected access. Write to SEC_ACCESS register */
            reg =  smps_ctrl_base_addr + pwr_data->pwr_reg_table->SEC_ACCESS;

            err_flag = pm_comm_write_byte(smps_ptr->comm_ptr->slave_id, reg, 0xA5, 0);

            if (err_flag == PM_ERR_FLAG__SUCCESS)
            {
                /* Write to CLK_DIV register */
                reg =  smps_ctrl_base_addr + pwr_data->pwr_reg_table->CLK_DIV;

                err_flag = pm_comm_write_byte_mask(smps_ptr->comm_ptr->slave_id, reg, 0x1F, (pm_register_data_type)freq, 0); // <0:4>
            }
        }
        else
        {
            err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
        }
    }
    return err_flag;
}

pm_err_flag_type 
pm_smps_switching_freq_status(uint8 pmic_chip, uint8 smps_peripheral_index, pm_smps_switching_freq_type *freq)
{
     pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
     pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if (smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(smps_peripheral_index >= smps_ptr->pm_pwr_data.num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }
    else if (freq == NULL)
    {
        err_flag = PM_ERR_FLAG__INVALID_POINTER;
    }
    else
    {
        pm_register_address_type    reg = 0;
        pm_register_address_type    smps_freq_periph_offset = 0x200;
        pm_register_data_type       data_read = 0;
        pm_pwr_data_type                      *pwr_data = &(smps_ptr->pm_pwr_data);
                                                                        
        /* We can read back the frequency from SMPSx_FREQ peripheral CLK_DIV reg for both FTS and HF bucks */
        reg = pwr_data->pwr_specific_info[smps_peripheral_index].periph_base_address + pwr_data->pwr_reg_table->CLK_DIV + smps_freq_periph_offset;

        err_flag = pm_comm_read_byte_mask( smps_ptr->comm_ptr->slave_id, reg, 0x1F, &data_read,0 );  // <0:4>

        if (err_flag == PM_ERR_FLAG__SUCCESS)
        {
            *freq = (pm_smps_switching_freq_type)data_read;
        }
        else
        {
            *freq = PM_SWITCHING_FREQ_INVALID;
        }
    }

    return err_flag;
}

pm_err_flag_type 
pm_smps_set_phase(uint8 pmic_chip, uint8 smps_peripheral_index, pm_phase_cnt_type phase)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if (smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(smps_peripheral_index >= smps_ptr->pm_pwr_data.num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }
    else if(phase >= PM_PHASE_CNT__INVALID)
    {
        err_flag = PM_ERR_FLAG__MACRO_NOT_RECOGNIZED;
    }
    else
    {
        pm_register_address_type    reg = 0xFF;
        pm_pwr_data_type            *pwr_data = &(smps_ptr->pm_pwr_data);
        
        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[smps_peripheral_index].periph_base_address + pwr_data->pwr_reg_table->PHASE_CNT_MAX);
        
        err_flag = pm_comm_write_byte_mask(smps_ptr->comm_ptr->slave_id, reg, 0x03, (pm_register_data_type)phase, 0); // bit<2:0>
    }

    return err_flag;
}

pm_err_flag_type 
pm_smps_get_phase(uint8 pmic_chip, uint8 smps_peripheral_index, pm_phase_cnt_type *phase)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if (smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(smps_peripheral_index >= smps_ptr->pm_pwr_data.num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }
    else if(phase == NULL)
    {
        err_flag = PM_ERR_FLAG__INVALID_POINTER;
    }
    else
    {
        pm_register_address_type    reg = 0xFF;
        pm_register_data_type       data = 0;
        pm_pwr_data_type            *pwr_data = &(smps_ptr->pm_pwr_data);
        
        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[smps_peripheral_index].periph_base_address + pwr_data->pwr_reg_table->PHASE_CNT_MAX);
        
        err_flag = pm_comm_read_byte(smps_ptr->comm_ptr->slave_id, reg, &data, 0);
        if(err_flag != PM_ERR_FLAG__SUCCESS)
        {
            *phase = PM_PHASE_CNT__INVALID;
            return err_flag;
        }
        
        *phase = (pm_phase_cnt_type) (data & 0x03);
    }

    return err_flag;
}


pm_err_flag_type 
pm_smps_pull_down(uint8 pmic_chip, uint8 smps_peripheral_index, pm_on_off_type on_off) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

  if (smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_pull_down_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_peripheral_index, on_off);
    }

    return err_flag;
}


pm_err_flag_type 
pm_smps_pin_ctrled(uint8 pmic_chip, uint8 smps_peripheral_index, uint8 select_pin)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

  if (smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_pin_ctrl_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_peripheral_index, select_pin);
    }

    return err_flag;
}


pm_err_flag_type 
pm_smps_vreg_ok_status(uint8 pmic_chip, uint8 smps_peripheral_index, boolean *vreg_ok_status)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if (smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    { 
        err_flag = pm_pwr_vreg_ok_status_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_peripheral_index, vreg_ok_status);
    }

    return err_flag;
}

/*===========================================================================

                     INTERNAL DRIVER FUNCTIONS 

===========================================================================*/

pm_err_flag_type 
pm_smps_hf_calculate_ilim_data(uint8 smps_ps_subtype, uint32 ilim_level_milli_amps, pm_smps_ilim_mode_type smps_mode, 
                                                pm_register_data_type *ilim_data_ptr, pm_register_mask_type *ilim_mask_ptr)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type ilim_reg_data = 0;
    pm_register_mask_type ilim_reg_mask = 0;

    if ((ilim_data_ptr == NULL) || (ilim_mask_ptr == NULL))
    {
        err_flag = PM_ERR_FLAG__INVALID_POINTER;
    }
    else
    {
        /* Formula for calculating current limit on different HF SMPS_PS subtypes
           6X --> Iplimit = 5400 mA - m*650 mA
           3X --> Iplimit = 3500 mA - m*400 mA
           2X --> Iplimit = 2700 mA - m*300 mA
           where m is the bit value of iplimit_sel<2:0> */

        if (smps_ps_subtype == PM_HW_MODULE_HF_BUCK_PS6X)
        {
            /* Maximum current is 5400mA */
            if (ilim_level_milli_amps <= 5400)
            {
                ilim_reg_data = ((5400 - ilim_level_milli_amps)/650);
            }
        }
        else if (smps_ps_subtype == PM_HW_MODULE_HF_BUCK_PS3X)
        {
            /* Maximum current is 3500mA */
            if (ilim_level_milli_amps <= 3500)
            {
                ilim_reg_data = ((3500 - ilim_level_milli_amps)/400);
            }
        }
        else if (smps_ps_subtype == PM_HW_MODULE_HF_BUCK_PS2X)
        {
            /* Maximum current is 2700mA */
            if (ilim_level_milli_amps <= 2700)
            {
                ilim_reg_data = ((2700 - ilim_level_milli_amps)/300);
            }
        }
        else
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

        /* For HF, mask is different for AUTO_MODE and PWM_MODE CURR_LIM setting */
        if (smps_mode == PM_ILIM_SMPS_AUTO_MODE)
        {
            ilim_reg_data = ilim_reg_data<<3;
            ilim_reg_mask = 0x38;
        }
    else
    {
            ilim_reg_mask = 0x07;
        }
    }

    if (err_flag == PM_ERR_FLAG__SUCCESS)
    {
        *ilim_data_ptr = ilim_reg_data;
        *ilim_mask_ptr = ilim_reg_mask;
    }

    return err_flag;
}

pm_err_flag_type 
pm_smps_fts_calculate_ilim_data(uint8 smps_ps_subtype, uint32 ilim_level_milli_amps, pm_smps_ilim_mode_type smps_mode, 
                                                 pm_register_data_type *ilim_data_ptr, pm_register_mask_type *ilim_mask_ptr)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type ilim_reg_data = 0;
    pm_register_mask_type ilim_reg_mask = 0;

    if (smps_mode == PM_ILIM_SMPS_AUTO_MODE)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if ((ilim_data_ptr == NULL) || (ilim_mask_ptr == NULL))
    {
        err_flag = PM_ERR_FLAG__INVALID_POINTER;
    }
    else
    {
        /* Formula for calculating current limit on different FTS SMPS_PS subtypes
           2.5X    --> Iplimit = m * 312 mA + 468 mA
           where m is the bit value of iplimit_sel<3:0>
           1X,0.5X --> Iplimit = m * 500 mA + 1000 mA
           where m is the bit value of iplimit_sel<2:0> */

        if (smps_ps_subtype == PM_HW_MODULE_FTS__FTS2p5_PS)
        {
            ilim_reg_mask = 0x0F;

            /* Minimum current is 468mA */
            if (ilim_level_milli_amps >= 468)
            {
                /* Added 0.5 and simplified the formula to round off to upper bound value */
                ilim_reg_data = ((ilim_level_milli_amps - 312)/312);
            }
        }
        else if ((smps_ps_subtype == PM_HW_MODULE_FTS__PS1X) || (smps_ps_subtype == PM_HW_MODULE_FTS__PS0p5X))
{
            ilim_reg_mask = 0x07;

            /* Minimum current is 1000mA */
            if (ilim_level_milli_amps >= 1000)
    {
                /* Added 0.5 and simplified the formula to round off to upper bound value */
                ilim_reg_data = ((ilim_level_milli_amps - 750)/500);
            }
    }
    else
    {
            err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

        /* ilim_reg_data shouldn't exceed the ilim_reg_mask value */
        if (ilim_reg_data > ilim_reg_mask)
        {
            ilim_reg_data = ilim_reg_mask;
        }
}

    if (err_flag == PM_ERR_FLAG__SUCCESS)
    {
        *ilim_data_ptr = ilim_reg_data;
        *ilim_mask_ptr = ilim_reg_mask;
    }

    return err_flag;
}
