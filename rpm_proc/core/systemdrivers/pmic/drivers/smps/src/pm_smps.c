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

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/drivers/smps/src/pm_smps.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/26/14   vtw     SW workaround for GFX rail multiphase support.
04/25/13   aks     Code Refactoring: Removing PMIC_SubRsc from pwr algs
04/02/13   hs      Added API pm_ipwr_sw_mode_status_lite()
02/27/13   hs      Code refactoring.
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

                     API IMPLEMENTATION

===========================================================================*/

pm_err_flag_type pm_smps_sw_mode(uint8 pmic_chip, uint8 smps_peripheral_index, pm_sw_mode_type mode)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);
    uint8 periph_type = 0;

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

        periph_type = pwr_data->pwr_specific_info[smps_peripheral_index].periph_type;

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

    if (PM_ERR_FLAG__SUCCESS == err_flag)
    {
        PM_LOG_MSG_DEBUG(PMIC_DRV_PWR_MODE, pmic_chip, periph_type, smps_peripheral_index, mode);
    }
    else
    {
        PM_LOG_MSG_ERROR(PMIC_DRV_PWR_MODE_ERROR, pmic_chip, periph_type, smps_peripheral_index, err_flag);
    }

    return err_flag;
}

pm_err_flag_type pm_smps_sw_mode_status(uint8 pmic_chip, uint8 smps_peripheral_index, pm_sw_mode_type *sw_mode)
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

pm_err_flag_type pm_smps_sw_mode_status_raw(uint8 pmic_chip, uint8 smps_peripheral_index, uint8 *mode)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_sw_mode_status_raw_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_peripheral_index, mode);
    }
    return err_flag;
}

pm_err_flag_type pm_smps_sw_enable(uint8 pmic_chip, uint8 smps_peripheral_index, pm_on_off_type on_off)
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

pm_err_flag_type pm_smps_sw_enable_status(uint8 pmic_chip, uint8 smps_peripheral_index, pm_on_off_type *on_off)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_sw_enable_status_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_peripheral_index, on_off);
    }
    return err_flag;
}

pm_err_flag_type pm_smps_vreg_ok_status(uint8 pmic_chip, uint8 smps_peripheral_index, boolean* vreg_ok)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_sw_enable_vreg_ok_status_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_peripheral_index, vreg_ok);
    }
    return err_flag;
}

pm_err_flag_type pm_smps_volt_level(uint8 pmic_chip, uint8 smps_peripheral_index, pm_volt_level_type volt_level)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if (smps_ptr == NULL)
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_volt_level_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_peripheral_index, volt_level);
    }
    return err_flag;
}

pm_err_flag_type pm_smps_volt_level_status(uint8 pmic_chip, uint8 smps_peripheral_index, pm_volt_level_type *volt_level)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);

    if(smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_volt_level_status_alg(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_peripheral_index, volt_level);
    }
    return err_flag;
}

pm_err_flag_type pm_smps_calculate_vset(uint8 pmic_chip, uint8 smps_peripheral_index, pm_volt_level_type volt_level, uint32 *vset)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);
    pm_volt_level_type  dummy_ceiling = 0;
    
    if(smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else
    {
        err_flag = pm_pwr_volt_calculate_vset_celing_uv(&(smps_ptr->pm_pwr_data), smps_ptr->comm_ptr, smps_peripheral_index, volt_level, vset, &dummy_ceiling);
    }
    return err_flag;
}

pm_err_flag_type pm_smps_quiet_mode(uint8 pmic_chip, uint8 smps_peripheral_index, pm_quiet_mode_type  mode, uint16  voltage_mv)
{
    pm_err_flag_type  err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);
    const uint16      vrst_min = 250; // 250mV
    const uint16      vrst_max = 800; // 800mV
    uint8 periph_type = 0;

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

        periph_type = pwr_data->pwr_specific_info[smps_peripheral_index].periph_type;

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

    if (PM_ERR_FLAG__SUCCESS != err_flag)
    {
        PM_LOG_MSG_ERROR(PMIC_DRV_SMPS_QM_ERROR, pmic_chip, periph_type, smps_peripheral_index, err_flag);
    }

    return err_flag;
}

pm_err_flag_type pm_smps_switching_freq(uint8 pmic_chip, uint8 smps_peripheral_index, pm_smps_switching_freq_type freq)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);
    uint8 periph_type = 0;

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
        pm_pwr_data_type            *pwr_data = &(smps_ptr->pm_pwr_data);

        periph_type = pwr_data->pwr_specific_info[smps_peripheral_index].periph_type;

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

    if (PM_ERR_FLAG__SUCCESS != err_flag)
    {
        PM_LOG_MSG_ERROR(PMIC_DRV_SMPS_SWITCH_FREQ_ERROR, pmic_chip, periph_type, smps_peripheral_index, err_flag);
    }

    return err_flag;
}

pm_err_flag_type pm_smps_switching_freq_status(uint8 pmic_chip, uint8 smps_peripheral_index, pm_smps_switching_freq_type *freq)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);
    uint8 periph_type = 0;

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

        periph_type = pwr_data->pwr_specific_info[smps_peripheral_index].periph_type;
        
        /* We can read back the frequency from SMPSx_FREQ peripheral CLK_DIV reg for both FTS and HF bucks */
        reg = pwr_data->pwr_specific_info[smps_peripheral_index].periph_base_address + pwr_data->pwr_reg_table->CLK_DIV + smps_freq_periph_offset;

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

    if (PM_ERR_FLAG__SUCCESS != err_flag)
    {
        PM_LOG_MSG_ERROR(PMIC_DRV_SMPS_SWITCH_FREQ_ERROR, pmic_chip, periph_type, smps_peripheral_index, err_flag);
    }

    return err_flag;
}

pm_err_flag_type pm_smps_set_softstart
(
  uint8 pmic_chip,
  uint8 periph_index,
  uint8 ss_cfg
)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);
  uint8 periph_type = 0;

  if (smps_ptr == NULL)
  {
    err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else if(periph_index >= smps_ptr->pm_pwr_data.num_of_peripherals)
  {
    err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
  }
  else
  {
    pm_register_address_type reg = 0xFF;
    pm_pwr_data_type *pwr_data = &(smps_ptr->pm_pwr_data);

    periph_type = pwr_data->pwr_specific_info[periph_index].periph_type;

    reg = (pm_register_address_type)(pwr_data->pwr_specific_info[periph_index].periph_base_address +
                                     pwr_data->pwr_reg_table->SOFT_START_CTL);

    err_flag = pm_comm_write_byte_mask(smps_ptr->comm_ptr->slave_id, reg, 0x1F, (pm_register_data_type)ss_cfg, 0); // bit<4:0>
  }

  if(PM_ERR_FLAG__SUCCESS != err_flag)
  {
    PM_LOG_MSG_ERROR(PMIC_DRV_SMPS_SS_ERROR, pmic_chip, periph_type, periph_index, err_flag);
  }

  return err_flag;
} /* pm_smps_set_softstart */

pm_err_flag_type pm_smps_get_softstart
(
  uint8 pmic_chip,
  uint8  periph_index,
  uint8* ss_ptr
)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);
  uint8 periph_type = 0;

  if (smps_ptr == NULL) 
  {
    err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else if(periph_index >= smps_ptr->pm_pwr_data.num_of_peripherals)
  {
    err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
  }
  else if (ss_ptr == NULL)
  {
    err_flag = PM_ERR_FLAG__INVALID_POINTER;
  }
  else
  {
    pm_register_address_type reg = 0xFF;
    pm_pwr_data_type *pwr_data = &(smps_ptr->pm_pwr_data);

    periph_type = pwr_data->pwr_specific_info[periph_index].periph_type;


    reg = (pm_register_address_type)(pwr_data->pwr_specific_info[periph_index].periph_base_address +
                                     pwr_data->pwr_reg_table->SOFT_START_CTL);

    err_flag = pm_comm_read_byte(smps_ptr->comm_ptr->slave_id, reg, ss_ptr, 0);
  }

  if(PM_ERR_FLAG__SUCCESS != err_flag)
  {
    PM_LOG_MSG_ERROR(PMIC_DRV_SMPS_SS_ERROR, pmic_chip, periph_type, periph_index, err_flag);
  }

  return err_flag;
} /* pm_smps_get_softstart */

pm_err_flag_type pm_smps_get_vstep
(
  uint8 pmic_chip,
  uint8  periph_index,
  uint8* vstep_ptr
)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);
  uint8 periph_type = 0;

  if (smps_ptr == NULL)
  {
    err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else if(periph_index >= smps_ptr->pm_pwr_data.num_of_peripherals)
  {
    err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
  }
  else if (vstep_ptr == NULL)
  {
    err_flag = PM_ERR_FLAG__INVALID_POINTER;
  }
  else
  {
    pm_register_address_type reg = 0xFF;
    pm_pwr_data_type *pwr_data = &(smps_ptr->pm_pwr_data);

    periph_type = pwr_data->pwr_specific_info[periph_index].periph_type;

    reg = (pm_register_address_type)(pwr_data->pwr_specific_info[periph_index].periph_base_address +
                                     pwr_data->pwr_reg_table->VOLTAGE_STEP_CTL);

    err_flag = pm_comm_read_byte(smps_ptr->comm_ptr->slave_id, reg, vstep_ptr, 0);

  }

  if(PM_ERR_FLAG__SUCCESS != err_flag)
  {
    PM_LOG_MSG_ERROR(PMIC_DRV_SMPS_VSTEP_ERROR, pmic_chip, periph_type, periph_index, err_flag);
  }

  return err_flag;
} /* pm_smps_get_vstep */

pm_err_flag_type pm_smps_ocp_enable(uint8 pmic_chip, uint8 smps_peripheral_index, boolean enable)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_smps_data_type *smps_ptr = pm_smps_get_data(pmic_chip);
    uint8 periph_type = 0;

    if(smps_ptr == NULL) 
    {
        err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if(smps_peripheral_index >= smps_ptr->pm_pwr_data.num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED;
    }
    else
    {
        pm_register_address_type    reg = 0;
        pm_register_address_type    base_addr = 0;
        pm_register_data_type       data = 0;
        pm_pwr_data_type            *pwr_data = &(smps_ptr->pm_pwr_data);

        periph_type = pwr_data->pwr_specific_info[smps_peripheral_index].periph_type;

        /* SMPSx_CTRL peripheral base address */
        base_addr = pwr_data->pwr_specific_info[smps_peripheral_index].periph_base_address;

        /* OCP feature is present on HF bucks only */
        if (periph_type == PM_HW_MODULE_HF_BUCK)
        {
            /* Enable/Disable OCP for HF Bucks */
            if (enable == TRUE)
            {
                data = 0x80;
            }
            else
            {
                data = 0x00;
            }

            /* OCP reg needs protected access. Write to SEC_ACCESS register */
            reg =  base_addr + pwr_data->pwr_reg_table->SEC_ACCESS;

            err_flag = pm_comm_write_byte(smps_ptr->comm_ptr->slave_id, reg, 0xA5, 0);

            if (err_flag == PM_ERR_FLAG__SUCCESS)
            {
                /* Write to OCP register */
                reg = base_addr + pwr_data->pwr_reg_table->OCP;

                err_flag = pm_comm_write_byte_mask(smps_ptr->comm_ptr->slave_id, reg, 0x80, data, 0);
            }
        }
        else
        {
            err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
        }
    }

    if (PM_ERR_FLAG__SUCCESS != err_flag)
    {
        PM_LOG_MSG_ERROR(PMIC_DRV_SMPS_OCP_ERROR, pmic_chip, periph_type, smps_peripheral_index, err_flag);
    }

    return err_flag;
}

