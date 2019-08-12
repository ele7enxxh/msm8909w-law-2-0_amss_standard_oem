/*! \file pm_smps.c
*  
*  \brief Implementation file for SMPS resource type.  
*  
*  \details Each of the APIs checks for access and then if necessary directs
*  the call to Driver implementation.
*  
*  &copy; Copyright 2010-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/drivers/smps/src/pm_smps.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/26/13   rk      remove pin controlled  and quiet mode APIs.
12/06/12   hw      Rearchitecturing module driver to peripheral driver
04/09/12   hs      Changed the type for internalResourceIndex from int to uint8.
04/04/12   hs      Removed the buck switch size alg.
03/30/12   hs      Removed the dirver size alg.
04/10/11   wra     Removed the ifdef RPM declaration. RPM has it's own routing layer now
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_smps_driver.h"
#include "pm_smps.h"


/*===========================================================================

                     FUNCTION DECLARATIONS

===========================================================================*/

/* Internal helper function */
static pm_err_flag_type pm_smps_util_get_ilim_level(uint16 ilim_level, SmpsILimDS *ilimDS, pm_smps_perph_subtype_type smps_perph_subtype, pm_register_data_type *ilim_data);

/*===========================================================================

                     API IMPLEMENTATION

===========================================================================*/

pm_err_flag_type pm_smps_sw_mode_status(uint8 pmic_chip, uint8 perph_index, pm_sw_mode_type* sw_mode)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_sw_mode_status_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, perph_index, sw_mode);
    }

    return err_flag;
}

pm_err_flag_type pm_smps_pin_ctrled_status(uint8 pmic_chip, uint8 perph_index, pm_on_off_type *on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);
    uint8 select_pin = 0;

    if(smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_pin_ctrl_status_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, perph_index, on_off, &select_pin);
    }
    return err_flag;
}

pm_err_flag_type pm_smps_pin_ctrl_status(uint8 pmic_chip, uint8 perph_index, uint8 *select_pin)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);
    pm_on_off_type on_off = PM_INVALID;

    if(smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_pin_ctrl_status_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, perph_index, &on_off, select_pin);
    }
    return err_flag;
}

pm_err_flag_type pm_smps_volt_level_status(uint8 pmic_chip, uint8 perph_index, pm_volt_level_type *volt_level)
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

pm_err_flag_type pm_smps_sw_enable_status(uint8 pmic_chip, uint8 perph_index, pm_on_off_type *on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_sw_enable_status_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, perph_index, on_off);
    }
    return err_flag;
}

pm_err_flag_type pm_smps_vreg_ok_status(uint8 pmic_chip, uint8 perph_index, boolean *on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_sw_enable_vreg_ok_status_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, perph_index, on_off);
    }
    return err_flag;
}

pm_err_flag_type pm_smps_clk_source(uint8 pmic_chip, pm_clk_src_type  clk)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if ( clk >= PM_CLK_SOURCE_INVALID )
    {
      err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE ;
    }
    else
    {
        switch ( clk )
        {
        case PM_CLK_TCXO:
          //TODO temp commented out until the clk buff interface is done
          //errFlag = clk->ClkSelectRCorXO(true, internalResourceIndex); // This function sets the Buck Clock Source.
          break;
        case PM_CLK_RC:
          //TODO temp commented out until the clk buff interface is done
          //errFlag = clk->ClkSelectRCorXO(false, internalResourceIndex);
          break;
        default:
          err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
        }
    }

    return err_flag;
}

pm_err_flag_type pm_smps_switching_freq(uint8 pmic_chip, uint8 perph_index, pm_smps_switching_freq_type freq)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if (smps_ptr == NULL)
    {
       err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if ( perph_index >= smps_ptr->pm_pwr_data.num_of_peripherals )
    {
       err_flag = PM_ERR_FLAG__VREG_ID_OUT_OF_RANGE ;
    }
    else if ( freq >= PM_SWITCHING_FREQ_INVALID )
    {
        err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE ;
    }
    else
    {
        pm_register_address_type reg = 0xFF;
        pm_register_address_type smps_freq_periph_offset = 0x200;
        pm_pwr_data_type         *pwr_data = &(smps_ptr->pm_pwr_data);

        reg = (pm_register_address_type) (pwr_data->pwr_reg_table->base_address +
                                          (perph_index * pwr_data->pwr_reg_table->peripheral_offset) +
                                          smps_freq_periph_offset + pwr_data->pwr_reg_table->SEC_ACCESS);

        err_flag = pm_comm_write_byte(smps_ptr->comm_ptr->slave_id, reg, 0xA5, 0);

        if (PM_ERR_FLAG__SUCCESS == err_flag)
        {
            /* Write to CLK_DIV register */
            reg = (pm_register_address_type) (pwr_data->pwr_reg_table->base_address +
                                              (perph_index * pwr_data->pwr_reg_table->peripheral_offset) +
                                              smps_freq_periph_offset + pwr_data->pwr_reg_table->CLK_DIV);

            err_flag = pm_comm_write_byte_mask(smps_ptr->comm_ptr->slave_id, reg, 0x1F, (pm_register_data_type)freq, 0);   // <0:4>
        }
    }

    return err_flag;
}

pm_err_flag_type pm_smps_switching_freq_status(uint8 pmic_chip, uint8 perph_index, pm_smps_switching_freq_type *freq)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if (smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(perph_index >= smps_ptr->pm_pwr_data.num_of_peripherals)
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
        pm_register_data_type       data_read = 0;
        pm_register_address_type    smps_freq_periph_offset = 0x200;
        pm_pwr_data_type            *pwr_data = &(smps_ptr->pm_pwr_data);

        reg = (pm_register_address_type) (pwr_data->pwr_reg_table->base_address +
                                          (perph_index * pwr_data->pwr_reg_table->peripheral_offset) +
                                          smps_freq_periph_offset + pwr_data->pwr_reg_table->CLK_DIV);

        err_flag = pm_comm_read_byte_mask(smps_ptr->comm_ptr->slave_id, reg, 0x1F, &data_read,0);  // <0:4>

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

pm_err_flag_type pm_smps_inductor_ilim(uint8 pmic_chip, uint8 perph_index, uint16 ilim_level, pm_smps_ilim_mode_type smps_mode)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if (smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if ( perph_index >= smps_ptr->pm_pwr_data.num_of_peripherals )
    {
        err_flag = PM_ERR_FLAG__VREG_ID_OUT_OF_RANGE ;
    }
    else if (smps_mode >= PM_ILIM_SMPS_MODE_INVALID)
    {
        err_flag = PM_ERR_FLAG__PAR4_OUT_OF_RANGE ;
    }
    else
    {
        pm_register_data_type  ilim_data = 0x00;
        pm_register_data_type  ilim_data_mask = 0xFF;
        pm_register_data_type  perph_subtype_data_read = 0x00;
        pm_pwr_data_type       *pwr_data = &(smps_ptr->pm_pwr_data);

        pm_register_address_type smps_base_address  = (pm_register_address_type) pwr_data->pwr_reg_table->base_address +
                                                          0x0100 +  (perph_index * pwr_data->pwr_reg_table->peripheral_offset);
        pm_register_address_type smps_periph_subtype_reg = smps_base_address + pwr_data->pwr_reg_table->PERPH_SUBTYPE;
        pm_register_address_type smps_ilim_reg          = smps_base_address + pwr_data->pwr_reg_table->PWM_CURRENT_LIM_CTL;

        SmpsILimDS* ilimDS = (SmpsILimDS*)pm_target_information_get_common_info(PM_PROP_SMPS_ILIMIT_LUT);

        if(NULL == ilimDS)
        {
            err_flag = PM_ERR_FLAG__INVALID_POINTER;
        }
        else
        {
            //Read SMPS PERPH_SUBTYPE
            err_flag = pm_comm_read_byte_mask( smps_ptr->comm_ptr->slave_id, smps_periph_subtype_reg,
                                               0xFF, &perph_subtype_data_read, 0 );

            if ( (pm_smps_perph_subtype_type)perph_subtype_data_read == SMPS_PERPH_SUBTYPE_2X)
            {
                err_flag = pm_smps_util_get_ilim_level(ilim_level, ilimDS, SMPS_PERPH_SUBTYPE_2X, &ilim_data);
            }
            else if ( (pm_smps_perph_subtype_type)perph_subtype_data_read == SMPS_PERPH_SUBTYPE_3X)
            {
                err_flag = pm_smps_util_get_ilim_level(ilim_level, ilimDS, SMPS_PERPH_SUBTYPE_3X, &ilim_data);
            }
            else
            {
                err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
            }

            //Check for for smps operating mode
            if (err_flag == PM_ERR_FLAG__SUCCESS)
            {
                if(smps_mode == PM_ILIM_SMPS_PWM_MODE)
                {
                     ilim_data_mask = 0x07;
                }
                else if(smps_mode == PM_ILIM_SMPS_AUTO_MODE )
                {
                    ilim_data = ilim_data << 3;
                    ilim_data_mask = 0x38;
                }

                err_flag = pm_comm_write_byte_mask(smps_ptr->comm_ptr->slave_id, smps_ilim_reg, ilim_data_mask, ilim_data, 0 ) ;
             }
        }
    }

    return err_flag;
}

pm_err_flag_type pm_smps_inductor_ilim_status( uint8 pmic_chip, uint8 perph_index, uint16* ilim_level, pm_smps_ilim_mode_type smps_mode )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if (smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if ( perph_index >= smps_ptr->pm_pwr_data.num_of_peripherals )
    {
     err_flag = PM_ERR_FLAG__VREG_ID_OUT_OF_RANGE ;
    }
    else if (smps_mode >= PM_ILIM_SMPS_MODE_INVALID)
    {
        err_flag = PM_ERR_FLAG__PAR4_OUT_OF_RANGE ;
    }
    else
    {
        pm_register_data_type ilim_data_mask = 0xFF;
        pm_register_data_type data_read = 0x00;
        pm_register_data_type perph_subtype_data_read = 0x00;
        pm_pwr_data_type      *pwr_data = &(smps_ptr->pm_pwr_data);

        pm_register_address_type smps_base_address  = (pm_register_address_type)pwr_data->pwr_reg_table[perph_index].base_address +
                                                       0x0100 +  (perph_index* pwr_data->pwr_reg_table->peripheral_offset);
        pm_register_address_type smps_periph_subtype_reg = smps_base_address + pwr_data->pwr_reg_table->PERPH_SUBTYPE;
        pm_register_address_type smps_ilim_reg          = smps_base_address + pwr_data->pwr_reg_table->PWM_CURRENT_LIM_CTL;

        //Get handle for smps ilimit configuration data (lookup table)
        SmpsILimDS* ilimDS = (SmpsILimDS*)pm_target_information_get_specific_info(PM_PROP_SMPS_ILIMIT_LUT);

        if(NULL == ilimDS)
        {
            err_flag = PM_ERR_FLAG__INVALID_POINTER;
        }
        else
        {
            //Read SMPS PERPH_SUBTYPE
            err_flag = pm_comm_read_byte_mask(smps_ptr->comm_ptr->slave_id,
                               smps_periph_subtype_reg, 0xFF, &perph_subtype_data_read ,0);

            if(smps_mode == PM_ILIM_SMPS_PWM_MODE)
            {
                ilim_data_mask = 0x07;
            }
            else if(smps_mode == PM_ILIM_SMPS_AUTO_MODE )
            {
                ilim_data_mask = 0x38;
            }
            err_flag = pm_comm_read_byte_mask(smps_ptr->comm_ptr->slave_id, smps_ilim_reg, ilim_data_mask, &data_read,0 );

            if(smps_mode == PM_ILIM_SMPS_AUTO_MODE )
            {
                data_read = data_read >>3;
            }

            //Get actual ilim data from DS
            if (perph_subtype_data_read == 0x02)  //2x smps sub type
            {
                *ilim_level = ilimDS[(uint8)data_read].smps_ilimit_2x_lut;

            }
            else if (perph_subtype_data_read == 0x03)  //3x smps sub type)
            {
                *ilim_level = ilimDS[(uint8)data_read].smps_ilimit_3x_lut;
            }
            else
            {
                err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
            }
        }
    }

    return err_flag;
}

pm_err_flag_type pm_smps_quiet_mode(uint8 pmic_chip, uint8 perph_index, pm_quiet_mode_type mode, uint16 voltage_mv)
{
    pm_err_flag_type  err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);
    const uint16      vrst_min = 250; // 250mV
    const uint16      vrst_max = 800; // 800mV

    if (smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(perph_index >= smps_ptr->pm_pwr_data.num_of_peripherals)
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

        reg = (pm_register_address_type)(pwr_data->pwr_specific_info[perph_index].periph_base_address + pwr_data->pwr_reg_table->QMODE_SHADOW);

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

pm_err_flag_type pm_smps_volt_calculate_vset(uint8 pmic_chip, uint8 perph_index, pm_volt_level_type  volt_level, uint32* vset)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if (smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_volt_calculate_vset_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, perph_index, volt_level, vset);
    }

    return err_flag;
}

pm_err_flag_type pm_smps_volt_level_stepper_done_status(uint8 pmic_chip, uint8 perph_index, boolean *stepper_done)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if (smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_volt_level_stepper_done_status_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, perph_index, stepper_done);
    }

    return err_flag;
}

pm_err_flag_type pm_smps_volt_level(uint8 pmic_chip, uint8 perph_index, pm_volt_level_type volt_level)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if (smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_volt_level_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, perph_index, volt_level);
    }

    return err_flag;
}

pm_err_flag_type pm_smps_util_get_ilim_level(uint16 ilim_level, SmpsILimDS *ilimDS, pm_smps_perph_subtype_type smps_perph_subtype, pm_register_data_type *ilim_data)
{
    uint8 i;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    //ToDo:  This Alg can be optimized more but requires data structure change on pm_config_register.c
    if ((pm_smps_perph_subtype_type)smps_perph_subtype == SMPS_PERPH_SUBTYPE_2X)  //2x smps sub type
    {
        //check if I limit requested is within limit
        if (ilim_level > ilimDS[0].smps_ilimit_2x_lut)
        {
            *ilim_data = ilimDS[0].smps_ilimit_reg_data_lut; //set the Max limit
        }
        else if(ilim_level < ilimDS[7].smps_ilimit_2x_lut)
        {
            *ilim_data = ilimDS[7].smps_ilimit_reg_data_lut; //set the Min limit
        }
        else
        {
            for (i = 0; i < 8; i++)  //search for closest upper limit
            {
                if ( (ilim_level <= ilimDS[i].smps_ilimit_2x_lut) && (ilim_level > ilimDS[i+1].smps_ilimit_2x_lut) )
                {
                    *ilim_data = ilimDS[i].smps_ilimit_reg_data_lut;
                    break;
                }
            }
        }
    }
    else if ((pm_smps_perph_subtype_type)smps_perph_subtype == SMPS_PERPH_SUBTYPE_3X ) //3X smps sub type
    {
        if (ilim_level > ilimDS[0].smps_ilimit_3x_lut)
        {
            *ilim_data = ilimDS[0].smps_ilimit_reg_data_lut;
        }
        else if(ilim_level < ilimDS[7].smps_ilimit_3x_lut)
        {
            *ilim_data = ilimDS[7].smps_ilimit_reg_data_lut;
        }
        else
        {
            for (i = 0; i < 8; i++)
            {
                if ( (ilim_level <= ilimDS[i].smps_ilimit_3x_lut) && (ilim_level > ilimDS[i+1].smps_ilimit_3x_lut) )
                {
                    *ilim_data = ilimDS[i].smps_ilimit_reg_data_lut;
                    break;
                }
            }
        }
    }
    else
    {
        err_flag = PM_ERR_FLAG__PAR4_OUT_OF_RANGE ;;
    }

    return err_flag;
}
