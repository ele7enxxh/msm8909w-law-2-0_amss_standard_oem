/*! \file
*
*  \brief  pm_lbc.c
*  \details Implementation file for LBC resourece type.
*
*  &copy; Copyright 2013-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/lbc/src/pm_lbc.c#1 $
when       who     what, where, why
--------   ---     ----------------------------------------------------------
16/09/14   pxm     Support the condition charger is noticed by CBLPWR_N. CR715635
11/21/14   pxm     CR758680. IBAT_ATC_B could be configured from 90ma to 1440ma from PM8916 v2.0
11/03/14   pxm     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/

#include "pm_resource_manager.h"
#include "pm_lbc.h"
#include "pm_lbc_driver.h"
#include "pm_comm.h"
#include "DALSys.h"

pm_err_flag_type
pm_lbc_chg_set_battery_threshold
(
    uint8 pmic_chip,
    pm_lbc_which_type lbc,
    uint32 dead_battery_threshold,
    uint32 weak_battery_threshold
)
{
    uint8 perph_index = (pm_lbc_which_type)lbc;
    uint16 *sbl_chg_app_ds = NULL;
    uint8 data_weak = 0, data_trkl = 0;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_lbc_data_type *lbc_ptr = pm_lbc_get_data(pmic_chip);
    pm_register_address_type reg_lbc_vbat_trkl = lbc_ptr->lbc_reg_table->base_address + (pm_register_address_type)(perph_index*lbc_ptr->lbc_reg_table->peripheral_offset) + lbc_ptr->lbc_reg_table->vbat_trkl;
    pm_register_address_type reg_lbc_vbat_weak = lbc_ptr->lbc_reg_table->base_address + (pm_register_address_type)(perph_index*lbc_ptr->lbc_reg_table->peripheral_offset) + lbc_ptr->lbc_reg_table->vbat_weak;

    if (lbc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (perph_index >= lbc_ptr->num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG__INVALID_LBC_INDEXED ;
    }
    else if( NULL != (sbl_chg_app_ds = (uint16*)pm_target_information_get_specific_info(PM_PROP_CHG_APP_LUT)) )
    {
        if(dead_battery_threshold < sbl_chg_app_ds[PM_CHG_VBAT_TRKL_MIN_THRESHOLD]
            || dead_battery_threshold > sbl_chg_app_ds[PM_CHG_VBAT_TRKL_MAX_THRESHOLD])
        {
            err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
        }
        else if(weak_battery_threshold < sbl_chg_app_ds[PM_CHG_VBAT_WEAK_MIN_THRESHOLD]
            || weak_battery_threshold > sbl_chg_app_ds[PM_CHG_VBAT_WEAK_MAX_THRESHOLD])
        {
            err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
        }
        else
        {
            // To avoid over flow of unit16 limit 65535
            // VBAT_WEAK = 3.0 + 0.01875 * bit[4:0]
            // VBAT_TRKL = 2.5 + 0.01562 * bit[4:0]
            data_weak = ((weak_battery_threshold - sbl_chg_app_ds[PM_CHG_VBAT_WEAK_MIN_THRESHOLD]) * 100 + 100)/ 1875;
            data_trkl = ((dead_battery_threshold - sbl_chg_app_ds[PM_CHG_VBAT_TRKL_MIN_THRESHOLD]) * 100 + 100) / 1562;
            err_flag |= pm_comm_write_byte_mask(lbc_ptr->comm_ptr->slave_id, reg_lbc_vbat_trkl, 0x1F, data_trkl, 0);
            err_flag |= pm_comm_write_byte_mask(lbc_ptr->comm_ptr->slave_id, reg_lbc_vbat_weak, 0x1F, data_weak, 0);
        }
    }
    else
    {
        err_flag = PM_ERR_FLAG__INVALID_POINTER ;
    }

    return err_flag;
}

pm_err_flag_type
pm_lbc_chg_get_chg_option
(
    uint8 pmic_chip,
    pm_lbc_which_type lbc,
    boolean           *internal_charger
)
{
    uint8 perph_index = (pm_lbc_which_type)lbc;
    uint8 data = 0x00;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_lbc_data_type *lbc_ptr = pm_lbc_get_data(pmic_chip);
    pm_register_address_type reg_lbc_chg_option = lbc_ptr->lbc_reg_table->base_address + (pm_register_address_type)(perph_index*lbc_ptr->lbc_reg_table->peripheral_offset) + lbc_ptr->lbc_reg_table->chg_option;

    if (lbc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (perph_index >= lbc_ptr->num_of_peripherals)
    {
      err_flag = PM_ERR_FLAG__INVALID_LBC_INDEXED ;
    }
    else
    {
        if( NULL == internal_charger)
        {
            return PM_ERR_FLAG__INVALID_POINTER;
        }

        /* Read CHG_OPTION */
        err_flag |= pm_comm_read_byte(lbc_ptr->comm_ptr->slave_id, reg_lbc_chg_option, &data, 0);

        /* bit <7:6>  0 = under voltage, 1 = over voltage, 2 = in valid range, 3 = in valid range 3 */
        *internal_charger = 0x80 == (data & 0x80);
    }

    return err_flag;
}

pm_err_flag_type
pm_lbc_chg_set_vdd_max
(
    uint8 pmic_chip,
    pm_lbc_which_type lbc,
    uint32            vdd_max
)
{
    uint8 data = 0x00;
    uint8 perph_index = (pm_lbc_which_type)lbc;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint16 *sbl_chg_app_ds = NULL;
    pm_lbc_data_type *lbc_ptr = pm_lbc_get_data(pmic_chip);
    pm_register_address_type reg_lbc_vdd_max = lbc_ptr->lbc_reg_table->base_address + (pm_register_address_type)(perph_index*lbc_ptr->lbc_reg_table->peripheral_offset) + lbc_ptr->lbc_reg_table->vdd_max;

    if (lbc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if (perph_index >= lbc_ptr->num_of_peripherals)
    {
      err_flag = PM_ERR_FLAG__INVALID_LBC_INDEXED ;
    }
    else if ( NULL != (sbl_chg_app_ds = (uint16*)pm_target_information_get_specific_info(PM_PROP_CHG_APP_LUT)) )
    {
        if(vdd_max > sbl_chg_app_ds[PM_CHG_VBAT_MAX_MAX] || vdd_max < sbl_chg_app_ds[PM_CHG_VBAT_MAX_MIN])
        {
            err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
        }
        else
        {
            data = (uint8) ((vdd_max - sbl_chg_app_ds[PM_CHG_VBAT_MAX_MIN]) / sbl_chg_app_ds[PM_CHG_VBAT_MAX_STEP]) ;
            err_flag |= pm_comm_write_byte_mask(lbc_ptr->comm_ptr->slave_id, reg_lbc_vdd_max, 0x1F, data, 0);
        }
    }
    else
    {
        err_flag = PM_ERR_FLAG__INVALID_POINTER ;
    }

    return err_flag;
}

pm_err_flag_type
pm_lbc_chg_set_vdd_safe
(
    uint8 pmic_chip,
    pm_lbc_which_type lbc,
    uint32            value_mv
)
{

    uint8 perph_index = (pm_lbc_which_type)lbc;
    uint16 *sbl_chg_app_ds = NULL;
    uint8 data = 0x00;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_lbc_data_type *lbc_ptr = pm_lbc_get_data(pmic_chip);
    pm_register_address_type reg_lbc_vdd_safe = lbc_ptr->lbc_reg_table->base_address + (pm_register_address_type)(perph_index*lbc_ptr->lbc_reg_table->peripheral_offset) + lbc_ptr->lbc_reg_table->vdd_safe;

    if (lbc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (perph_index >= lbc_ptr->num_of_peripherals)
    {
      err_flag = PM_ERR_FLAG__INVALID_LBC_INDEXED ;
    }
    else if( NULL != (sbl_chg_app_ds = (uint16*)pm_target_information_get_specific_info(PM_PROP_CHG_APP_LUT)) )
    {
        if(value_mv > sbl_chg_app_ds[PM_CHG_VBAT_MAX_MAX] || value_mv < sbl_chg_app_ds[PM_CHG_VBAT_MAX_MIN])
        {
            err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
        }
        else
        {
            data = (uint8) ((value_mv - sbl_chg_app_ds[PM_CHG_VBAT_MAX_MIN]) / sbl_chg_app_ds[PM_CHG_VBAT_MAX_STEP]) ;
            err_flag |= pm_comm_write_byte_mask(lbc_ptr->comm_ptr->slave_id, reg_lbc_vdd_safe, 0x1F, data, 0);
        }
    }
    else
    {
        err_flag = PM_ERR_FLAG__INVALID_POINTER ;
    }

    return err_flag;
}

pm_err_flag_type
pm_lbc_chg_set_ibat_max
(
   uint8 pmic_chip,
   pm_lbc_which_type      lbc,
   uint32 value_ma
)
{
  uint8 perph_index = (pm_lbc_which_type)lbc;
  uint8 data = 0x00;
  uint16 *sbl_chg_app_ds = NULL;
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  pm_lbc_data_type *lbc_ptr = pm_lbc_get_data(pmic_chip);
  pm_register_address_type reg_lbc_ibat_max = lbc_ptr->lbc_reg_table->base_address + (pm_register_address_type)(perph_index*lbc_ptr->lbc_reg_table->peripheral_offset) + lbc_ptr->lbc_reg_table->ibat_max;

  if (lbc_ptr == NULL)
  {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  if (perph_index >= lbc_ptr->num_of_peripherals)
  {
    err_flag = PM_ERR_FLAG__INVALID_LBC_INDEXED ;
  }
  else if( NULL != (sbl_chg_app_ds = (uint16*)pm_target_information_get_specific_info(PM_PROP_CHG_APP_LUT)) )
  {
      if(value_ma > sbl_chg_app_ds[PM_CHG_IBAT_MAX_MAX] || value_ma < sbl_chg_app_ds[PM_CHG_IBAT_MAX_MIN])
      {
          err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
      }
      else
      {
          data = (uint8) ((value_ma - sbl_chg_app_ds[PM_CHG_IBAT_MAX_MIN]) / sbl_chg_app_ds[PM_CHG_IBAT_MAX_STEP]) ;
          err_flag |= pm_comm_write_byte_mask(lbc_ptr->comm_ptr->slave_id, reg_lbc_ibat_max, 0x0F, data, 0);
      }
  }
  else
  {
      err_flag = PM_ERR_FLAG__INVALID_POINTER ;
  }

  return err_flag;
}

pm_err_flag_type
pm_lbc_chg_set_ibat_safe
(
   uint8 pmic_chip,
   pm_lbc_which_type      lbc,
   uint32 value_ma
)
{

    uint8 perph_index = (pm_lbc_which_type)lbc;
    uint8 data = 0x00;
    uint16 *sbl_chg_app_ds = NULL;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_lbc_data_type *lbc_ptr = pm_lbc_get_data(pmic_chip);
    pm_register_address_type reg_lbc_ibat_safe = lbc_ptr->lbc_reg_table->base_address + (pm_register_address_type)(perph_index*lbc_ptr->lbc_reg_table->peripheral_offset) + lbc_ptr->lbc_reg_table->ibat_safe;

    if (lbc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (perph_index >= lbc_ptr->num_of_peripherals)
    {
      err_flag = PM_ERR_FLAG__INVALID_LBC_INDEXED ;
    }
    else if( NULL != (sbl_chg_app_ds = (uint16*)pm_target_information_get_specific_info(PM_PROP_CHG_APP_LUT)) )
    {
        if(value_ma > sbl_chg_app_ds[PM_CHG_IBAT_MAX_MAX] || value_ma < sbl_chg_app_ds[PM_CHG_IBAT_MAX_MIN])
        {
            err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
        }
        else
        {
            data = (uint8) ((value_ma - sbl_chg_app_ds[PM_CHG_IBAT_MAX_MIN]) / sbl_chg_app_ds[PM_CHG_IBAT_MAX_STEP]) ;
            err_flag |= pm_comm_write_byte_mask(lbc_ptr->comm_ptr->slave_id, reg_lbc_ibat_safe, 0x0F, data, 0);
        }
    }
    else
    {
        err_flag = PM_ERR_FLAG__INVALID_POINTER ;
    }

    return err_flag;
}

pm_err_flag_type
pm_lbc_chg_set_vin_min
(
    uint8 pmic_chip,
    pm_lbc_which_type lbc,
    uint32 value_mv
)
{
    uint8        data = 0x00;
    uint8 perph_index = (pm_lbc_which_type)lbc;
    pm_lbc_data_type *lbc_ptr = pm_lbc_get_data(pmic_chip);
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type reg_lbc_vin_min = lbc_ptr->lbc_reg_table->base_address + (pm_register_address_type)(perph_index*lbc_ptr->lbc_reg_table->peripheral_offset) + lbc_ptr->lbc_reg_table->vin_min;

    if (perph_index >= lbc_ptr->num_of_peripherals)
    {
      err_flag = PM_ERR_FLAG__INVALID_LBC_INDEXED ;
    }
    else
    {
        if(value_mv > 5037 || value_mv < 4200)
        {
            return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
        }
        else
        {
            data = (value_mv - 4200) / 27;
        }

        err_flag |= pm_comm_write_byte_mask(lbc_ptr->comm_ptr->slave_id, reg_lbc_vin_min, 0x1F, data, 0);
    }

    return err_flag;
}

pm_err_flag_type
pm_lbc_disable_chg
(
   uint8 pmic_chip,
   pm_lbc_which_type      lbc
)
{
    uint8 perph_index = (pm_lbc_which_type)lbc;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_lbc_data_type *lbc_ptr = pm_lbc_get_data(pmic_chip);
    pm_register_address_type reg_lbc_chg_ctrl = lbc_ptr->lbc_reg_table->base_address + (pm_register_address_type)(perph_index*lbc_ptr->lbc_reg_table->peripheral_offset) + lbc_ptr->lbc_reg_table->chg_ctrl;

    if (lbc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if (perph_index >= lbc_ptr->num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG__INVALID_LBC_INDEXED ;
    }
    else
    {
        err_flag |= pm_comm_write_byte_mask(lbc_ptr->comm_ptr->slave_id, reg_lbc_chg_ctrl, 0x01, 0x01, 0);
    }

    return err_flag;
}

pm_err_flag_type
pm_lbc_chg_set_atc_led
(
   uint8 pmic_chip,
   pm_lbc_which_type               lbc,
   boolean                         atc_led_state
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 perph_index = (pm_lbc_which_type)lbc;
    pm_lbc_data_type *lbc_ptr = pm_lbc_get_data(pmic_chip);
    pm_register_address_type reg_lbc_atc_led = lbc_ptr->lbc_reg_table->base_address + (pm_register_address_type)(perph_index*lbc_ptr->lbc_reg_table->peripheral_offset) + lbc_ptr->lbc_reg_table->atc_led;

    if (lbc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if (atc_led_state)
    {
       err_flag |= pm_comm_write_byte_mask(lbc_ptr->comm_ptr->slave_id, reg_lbc_atc_led, 0x01, 0x01, 0); //ATC LED ON
    }
    else
    {
       err_flag |= pm_comm_write_byte_mask(lbc_ptr->comm_ptr->slave_id, reg_lbc_atc_led, 0x01, 0x00, 0); //ATC LED OFF
    }

    return err_flag;

}

pm_err_flag_type
pm_lbc_chg_set_ibat_atc_b
(
   uint8 pmic_chip,
   pm_lbc_which_type      lbc,
   uint32 value_ma
)
{
    uint8 data = 0x00;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 perph_index = (pm_lbc_which_type)lbc;
    pm_lbc_data_type *lbc_ptr = pm_lbc_get_data(pmic_chip);
    pm_register_address_type reg_lbc_atc_b = lbc_ptr->lbc_reg_table->base_address + (pm_register_address_type)(perph_index*lbc_ptr->lbc_reg_table->peripheral_offset) + lbc_ptr->lbc_reg_table->ibat_atc_b;

    if (lbc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if (perph_index >= lbc_ptr->num_of_peripherals)
    {
      err_flag = PM_ERR_FLAG__INVALID_LBC_INDEXED ;
    }
    else
    {
        // value_ma = (x+1)x90. [90,1440]
        // x=0x1055[0:3]
        if(value_ma < 90 && value_ma > 1440)
        {
            err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
        }
        else
        {
            data = value_ma / 90 - 1;

            err_flag |= pm_comm_write_byte_mask(lbc_ptr->comm_ptr->slave_id, reg_lbc_atc_b, 0x0F, data, 0);
        }
    }

    return err_flag;
}

pm_err_flag_type
pm_lbc_chg_wdog_pet
(
   uint8 pmic_chip,
   pm_lbc_which_type      lbc
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 perph_index = (pm_lbc_which_type)lbc;
    pm_lbc_data_type *lbc_ptr = pm_lbc_get_data(pmic_chip);
    pm_register_address_type reg_lbc_wdog_pet = lbc_ptr->lbc_reg_table->base_address + (pm_register_address_type)(perph_index*lbc_ptr->lbc_reg_table->peripheral_offset) + lbc_ptr->lbc_reg_table->wdog_pet;
    if (lbc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (perph_index >= lbc_ptr->num_of_peripherals)
    {
      err_flag = PM_ERR_FLAG__INVALID_LBC_INDEXED ;
    }
    else
    {
        err_flag |= pm_comm_write_byte_mask(lbc_ptr->comm_ptr->slave_id, reg_lbc_wdog_pet, 0x80, 0x80, 0);
    }

    return err_flag;
}

pm_err_flag_type
pm_lbc_chg_wdog_enable
(
   uint8 pmic_chip,
   pm_lbc_which_type      lbc,
   boolean                enable
)
{
    uint8 data = 0x00;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 perph_index = (pm_lbc_which_type)lbc;
    pm_lbc_data_type *lbc_ptr = pm_lbc_get_data(pmic_chip);
     pm_register_address_type reg_lbc_wdog_en = lbc_ptr->lbc_reg_table->base_address + (pm_register_address_type)(perph_index*lbc_ptr->lbc_reg_table->peripheral_offset) + lbc_ptr->lbc_reg_table->wdog_en;

    if (lbc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if (perph_index >= lbc_ptr->num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG__INVALID_LBC_INDEXED ;
    }
    else
    {
        if(enable == TRUE)
        {
            data = 0x80;
        }

        err_flag |= pm_comm_write_byte_mask(lbc_ptr->comm_ptr->slave_id, reg_lbc_wdog_en, 0x80, data, 0);
    }

    return err_flag;
}

pm_err_flag_type
pm_lbc_chg_set_comp_override
(
   uint8 pmic_chip,
   pm_lbc_which_type               lbc,
   boolean                         comp_override_status
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 perph_index = (pm_lbc_which_type)lbc;
    pm_lbc_data_type *lbc_ptr = pm_lbc_get_data(pmic_chip);
    pm_register_address_type reg_lbc_sec_access = lbc_ptr->lbc_reg_table->base_address + (pm_register_address_type)(perph_index*lbc_ptr->lbc_reg_table->peripheral_offset) + lbc_ptr->lbc_reg_table->sec_access;
    pm_register_address_type reg_lbc_comp_over1 = lbc_ptr->lbc_reg_table->base_address + (pm_register_address_type)(perph_index*lbc_ptr->lbc_reg_table->peripheral_offset) + lbc_ptr->lbc_reg_table->comp_over1;

    if (lbc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    err_flag  = pm_comm_write_byte_mask(lbc_ptr->comm_ptr->slave_id, reg_lbc_sec_access, 0xFF, 0xA5, 0);

    if (comp_override_status)
    {
        err_flag |= pm_comm_write_byte_mask(lbc_ptr->comm_ptr->slave_id, reg_lbc_comp_over1, 0xFF, 0x20, 0); //Enable COMP_OVERR1
    }
    else
    {
       err_flag |= pm_comm_write_byte_mask(lbc_ptr->comm_ptr->slave_id, reg_lbc_comp_over1, 0xFF, 0x00, 0); //Disable COMP_OVERR1
    }

    return err_flag;
}

pm_err_flag_type
pm_lbc_chg_set_mode
(
   uint8 pmic_chip,
   pm_lbc_which_type     lbc,
   pm_chg_mode_type      mode
)
{
    uint8 perph_index = (pm_lbc_which_type)lbc;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_lbc_data_type *lbc_ptr = pm_lbc_get_data(pmic_chip);
    pm_register_address_type reg_lbc_chg_ctrl  = lbc_ptr->lbc_reg_table->base_address + (pm_register_address_type)(perph_index*lbc_ptr->lbc_reg_table->peripheral_offset) + lbc_ptr->lbc_reg_table->chg_ctrl;

    if (lbc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (perph_index >= lbc_ptr->num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG__INVALID_LBC_INDEXED ;
    }
    else
    {
        if( mode > PM_CHG_MODE__CHARGE_ON )
        {
            return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
        }

        switch( mode )
        {
        case PM_CHG_MODE__CHARGE_OFF:
            {
                /*************Disable charging ********/
                /* Clear LBC_CHGR_CHG_CTRL bit 7 and set bit 0 */
                err_flag |= pm_comm_write_byte_mask(lbc_ptr->comm_ptr->slave_id, reg_lbc_chg_ctrl, 0x81, 0x01, 0);
            }
            break;
        case PM_CHG_MODE__CHARGE_ON:
            {
                /*************Enable charging ********/
                /* Set LBC_CHGR_CHG_CTRL bit 7 and clear bit 0 */
                err_flag |= pm_comm_write_byte_mask(lbc_ptr->comm_ptr->slave_id, reg_lbc_chg_ctrl, 0x81, 0x80, 0);
            }
            break;
        default:
            err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
            break;
        }

    }
    return err_flag;

}

pm_err_flag_type
pm_lbc_chg_get_battery_present
(
   uint8 pmic_chip,
   pm_lbc_which_type    lbc,
   boolean             *batt_present,
   boolean             *batt_id_present
)
{
    uint8 data = 0x00;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 perph_index = (pm_lbc_which_type)lbc;
    pm_lbc_data_type *lbc_ptr = pm_lbc_get_data(pmic_chip);
    pm_register_address_type reg_lbc_bat_pres_status = lbc_ptr->lbc_reg_table->base_address + (pm_register_address_type)(perph_index*lbc_ptr->lbc_reg_table->peripheral_offset) + (pm_register_address_type)(PM_LBC_BAT_IF*0x0100) + lbc_ptr->lbc_reg_table->bat_pres_status;

    if (lbc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (perph_index >= lbc_ptr->num_of_peripherals)
    {
      err_flag = PM_ERR_FLAG__INVALID_LBC_INDEXED ;
    }
    else
    {
        if( NULL == batt_present)
        {
            return PM_ERR_FLAG__INVALID_POINTER;
        }

        /* Read BAT_PRES_STATUS */
        err_flag |= pm_comm_read_byte(lbc_ptr->comm_ptr->slave_id, reg_lbc_bat_pres_status, &data, 0);

        /* bit 7: BAT_PRES: 0 = battery not present, 1 = battery present */
        *batt_present = 0x80 == (data & 0x80);

        /* bit 0: BAT_ID_PRES: 0 = BAT_ID_GONE, 1 = BAT_ID_PRESENT */
        *batt_id_present = 0x01 == (data & 0x01);
    }

    return err_flag;
}

pm_err_flag_type
pm_lbc_chg_get_charger_present
(
    uint8 pmic_chip,
    pm_lbc_which_type    lbc,
    boolean             *charger_present
)
{
    uint8 data = 0x00;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 perph_index = (pm_lbc_which_type)lbc;
    pm_lbc_data_type *lbc_ptr = pm_lbc_get_data(pmic_chip);
    pm_register_address_type reg_lbc_charger_pres_status = lbc_ptr->lbc_reg_table->base_address + (pm_register_address_type)(perph_index*lbc_ptr->lbc_reg_table->peripheral_offset) + (pm_register_address_type)(PM_LBC_CHGPTH*0x0100) + lbc_ptr->lbc_reg_table->usb_chg_pth_sts;

    if (lbc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (perph_index >= lbc_ptr->num_of_peripherals)
    {
      err_flag = PM_ERR_FLAG__INVALID_LBC_INDEXED ;
    }
    else
    {
        if( NULL == charger_present)
        {
            return PM_ERR_FLAG__INVALID_POINTER;
        }

        /* Read USB_CHG_PTH_STS */
        err_flag |= pm_comm_read_byte(lbc_ptr->comm_ptr->slave_id, reg_lbc_charger_pres_status, &data, 0);

        /* bit <7:6>  0 = under voltage, 1 = over voltage, 2 = in valid range, 3 = in valid range 3 */
        *charger_present = 0x80 == (data & 0x80);
    }

    // For the condition USB_IN is floated, CBLPWR_N is connected instead.
    // So if charger is NOT present by above detection, we need to detect CBLPWR_N for double check.
    // Check CBLPWR_ON bit, if this bit is 0, then charger is NOT there, if this bit is set 1, 
    // double check this bit after 50ms.
    // Here we use hard code for PON_INT_RT_STS(0x810) and bit CBLPWR_ON(bit2) since this register belongs to PON peripheral.
    // We don't want to add much effort in charger module for a PON register.
    if(FALSE == *charger_present)
    {
        err_flag |= pm_comm_read_byte(lbc_ptr->comm_ptr->slave_id, 0x810, &data, 0);
        if(err_flag == PM_ERR_FLAG__SUCCESS)
        {
            *charger_present = (data & 0x04) == 0x04;
        }

        if(TRUE == *charger_present)
        {
             // delay 50ms and then double check
             (void)DALSYS_BusyWait(1000*50);
             err_flag |= pm_comm_read_byte(lbc_ptr->comm_ptr->slave_id, 0x810, &data, 0);
             if(err_flag == PM_ERR_FLAG__SUCCESS)
             {
                 *charger_present = (data & 0x04) == 0x04;
             }
        }
    }

    return err_flag;
}

pm_err_flag_type
pm_lbc_chg_get_enum_timer_expired
(
    uint8 pmic_chip,
    pm_lbc_which_type    lbc,
    boolean             *enum_expired
)
{
    uint8 data = 0x00;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 perph_index = (pm_lbc_which_type)lbc;
    pm_lbc_data_type *lbc_ptr = pm_lbc_get_data(pmic_chip);
    pm_register_address_type reg_lbc_chg_path_sts = lbc_ptr->lbc_reg_table->base_address + (pm_register_address_type)(perph_index*lbc_ptr->lbc_reg_table->peripheral_offset) + (pm_register_address_type)(PM_LBC_CHGPTH*0x0100) + lbc_ptr->lbc_reg_table->usb_chg_pth_sts;

    if (lbc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if (perph_index >= lbc_ptr->num_of_peripherals)
    {
      err_flag = PM_ERR_FLAG__INVALID_LBC_INDEXED ;
    }
    else
    {
        if( NULL == enum_expired)
        {
            return PM_ERR_FLAG__INVALID_POINTER;
        }

        /* Read USB_CHG_PTH_STS */
        err_flag |= pm_comm_read_byte(lbc_ptr->comm_ptr->slave_id, reg_lbc_chg_path_sts, &data, 0);

        /* bit 5: 0 = hasn't expired, 1 = expired */
        *enum_expired = 0x20 == (data & 0x20);
    }

    return err_flag;
}

pm_err_flag_type
pm_lbc_chg_get_enum_timer_staus
(
   uint8 pmic_chip,
   pm_lbc_which_type      lbc,
   boolean                *run
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 perph_index = (pm_lbc_which_type)lbc;
    uint8 data = 0;
    pm_lbc_data_type *lbc_ptr = pm_lbc_get_data(pmic_chip);
    pm_register_address_type reg_lbc_enum_timer = lbc_ptr->lbc_reg_table->base_address
            + (pm_register_address_type)(perph_index*lbc_ptr->lbc_reg_table->peripheral_offset)
            + (pm_register_address_type)(PM_LBC_CHGPTH*0x0100) + lbc_ptr->lbc_reg_table->enum_timer_stop;

    if (lbc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if (perph_index >= lbc_ptr->num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG__INVALID_LBC_INDEXED ;
    }
    else
    {
        err_flag |= pm_comm_read_byte(lbc_ptr->comm_ptr->slave_id, reg_lbc_enum_timer, &data, 0);
        *run = ENUM_TIMER_STOP_RUN == (data & ENUM_TIMER_STOP_MASK);
    }

    return err_flag;
}

pm_err_flag_type
pm_lbc_chg_stop_enum_timer
(
   uint8 pmic_chip,
   pm_lbc_which_type      lbc
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 perph_index = (pm_lbc_which_type)lbc;
    pm_lbc_data_type *lbc_ptr = pm_lbc_get_data(pmic_chip);
    pm_register_address_type reg_lbc_stop_enum_timer = lbc_ptr->lbc_reg_table->base_address
            + (pm_register_address_type)(perph_index*lbc_ptr->lbc_reg_table->peripheral_offset)
            + (pm_register_address_type)(PM_LBC_CHGPTH*0x0100) + lbc_ptr->lbc_reg_table->enum_timer_stop;

    if (lbc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (perph_index >= lbc_ptr->num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG__INVALID_LBC_INDEXED ;
    }
    else
    {
        // 0x01 for stop. IPCAT shows 0x00 is stop which is wrong
        err_flag |= pm_comm_write_byte_mask(lbc_ptr->comm_ptr->slave_id, reg_lbc_stop_enum_timer, ENUM_TIMER_STOP_MASK, ENUM_TIMER_STOP_STOP, 0);
    }

    return err_flag;
}

pm_err_flag_type
pm_lbc_chg_set_boot_done
(
   uint8 pmic_chip,
   pm_lbc_which_type      lbc
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 perph_index = (pm_lbc_which_type)lbc;
    pm_lbc_data_type *lbc_ptr = pm_lbc_get_data(pmic_chip);
    pm_register_address_type reg_lbc_boot_done = lbc_ptr->lbc_reg_table->base_address
            + (pm_register_address_type)(perph_index*lbc_ptr->lbc_reg_table->peripheral_offset)
            + (pm_register_address_type)(PM_LBC_MISC*0x0100) + lbc_ptr->lbc_reg_table->boot_done;

    if (lbc_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (perph_index >= lbc_ptr->num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG__INVALID_LBC_INDEXED ;
    }
    else
    {
        err_flag |= pm_comm_write_byte_mask(lbc_ptr->comm_ptr->slave_id, reg_lbc_boot_done, 0x80, 0x80, 0);
    }

    return err_flag;
}


