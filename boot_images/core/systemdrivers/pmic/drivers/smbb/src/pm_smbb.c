/*! \file
*
*  \brief  pm_smbb.c
*  \details Implementation file for SMBB resourece type.
*
*  &copy; Copyright 2013-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/smbb/src/pm_smbb.c#1 $
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/12/14   mr      Removed KW errors (CR-678310)
08/06/13   aab     Added pm_smbb_chg_get_chg_gone_rt_status()
06/17/13   aab     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/

#include "pm_resource_manager.h"
#include "pm_smbb.h"
#include "pm_smbb_driver.h"
#include "pm_comm.h"

pm_err_flag_type
pm_smbb_chg_set_battery_threshold
(
   uint8 pmic_chip,
   pm_smbb_which_type     smbb,
   uint32                 dead_battery_threshold,
   uint32                 weak_battery_threshold
)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint16 *sbl_chg_app_ds = NULL;
  pm_register_address_type reg_smbb_vbat_trkl;
  pm_register_address_type reg_smbb_vbat_weak;
  uint8 data_weak = 0, data_dead = 0;
  uint8 perph_index = (pm_smbb_which_type)smbb;
  pm_smbb_data_type *smbb_ptr = pm_smbb_get_data(pmic_chip);

  if (smbb_ptr == NULL)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  reg_smbb_vbat_trkl = smbb_ptr->smbb_reg_table->base_address + (pm_register_address_type)(perph_index*smbb_ptr->smbb_reg_table->peripheral_offset) + smbb_ptr->smbb_reg_table->vbat_trkl;
  reg_smbb_vbat_weak = smbb_ptr->smbb_reg_table->base_address + (pm_register_address_type)(perph_index*smbb_ptr->smbb_reg_table->peripheral_offset) + smbb_ptr->smbb_reg_table->vbat_weak;

  if (perph_index >= smbb_ptr->num_of_peripherals)
  {
    err_flag = PM_ERR_FLAG__INVALID_SMBB_INDEXED ;
  }
  else if (NULL != (sbl_chg_app_ds = (uint16*)pm_target_information_get_specific_info(PM_PROP_CHG_APP_LUT)))
  {

    if(dead_battery_threshold < sbl_chg_app_ds[PM_CHG_VBAT_TRKL_MIN_THRESHOLD] )  //2050: PM_CHG_VBAT_TRKL_MIN_THRESHOLD
    {
        /* Set error flag */
        err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;

        /* Round to boundary */
        dead_battery_threshold = sbl_chg_app_ds[ PM_CHG_VBAT_TRKL_MIN_THRESHOLD ]; // minimum setting is 2050mV in SMBB_CHGR_VBAT_TRKL //PM_CHG_VBAT_TRKL_MIN_THRESHOLD
    }
    else if(dead_battery_threshold > sbl_chg_app_ds[PM_CHG_FLCB_DEAD_BATTERY_THRESHOLD])  //2800: PM_CHG_FLCB_DEAD_BATTERY_THRESHOLD
    {
        /* Set error flag */
        err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;

        /* Round to boundary */
        dead_battery_threshold = sbl_chg_app_ds[PM_CHG_FLCB_DEAD_BATTERY_THRESHOLD];  //maximum setting is 2800mV in SMBB_CHGR_VBAT_TRKL //PM_CHG_FLCB_DEAD_BATTERY_THRESHOLD
    }

    if(weak_battery_threshold < sbl_chg_app_ds[PM_CHG_VBAT_WEAK_MIN_THRESHOLD])   //2100: PM_CHG_VBAT_WEAK_MIN_THRESHOLD
    {
        /* Set error flag */
        err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;

        /* Round to boundary */
        weak_battery_threshold = sbl_chg_app_ds[PM_CHG_VBAT_WEAK_MIN_THRESHOLD]; //minimal setting is 2100mV in SMBB_CHGR_VBAT_WEAK  //PM_CHG_VBAT_WEAK_MIN_THRESHOLD
    }

    if(weak_battery_threshold > sbl_chg_app_ds[ PM_CHG_VBAT_WEAK_MAX_THRESHOLD ])  //3600: PM_CHG_VBAT_WEAK_MAX_THRESHOLD
    {
        /* Set error flag */
        err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;

        /* Round to boundary */
        weak_battery_threshold = sbl_chg_app_ds[PM_CHG_VBAT_WEAK_MAX_THRESHOLD]; //maximum setting is 3600 in SMBB_CHGR_VBAT_WEAK  //PM_CHG_VBAT_WEAK_MAX_THRESHOLD
    }

    if(weak_battery_threshold > dead_battery_threshold)
    {
       /* Set dead battery threshold in SMBB_CHGR_VBAT_TRKL register */
        data_dead = (uint8)((dead_battery_threshold - sbl_chg_app_ds[PM_CHG_VBAT_TRKL_MIN_THRESHOLD])/50);  //2050:PM_CHG_VBAT_TRKL_MIN_THRESHOLD/50

        /* Set the new data */
        err_flag |= pm_comm_write_byte_mask(smbb_ptr->comm_ptr->slave_id, reg_smbb_vbat_trkl, 0x0F, data_dead, 0);

        /* Set weak battery threshold in SMBB_CHGR_VBAT_WEAK register */
        data_weak = (uint8)((weak_battery_threshold - sbl_chg_app_ds[PM_CHG_VBAT_WEAK_MIN_THRESHOLD])/100);

        /* Set the new data */
        err_flag |= pm_comm_write_byte_mask(smbb_ptr->comm_ptr->slave_id, reg_smbb_vbat_weak, 0x0F, data_weak, 0);
    }
    else
    {
        err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
  }
  else
  {
    err_flag = PM_ERR_FLAG__INVALID_POINTER;
  }

  return err_flag;
}

pm_err_flag_type
pm_smbb_chg_get_weak_battery_status
(
   uint8 pmic_chip,
   pm_smbb_which_type     smbb,
   boolean                *weak
)
{
  uint8 data = 0x00;
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  uint8 perph_index = (pm_smbb_which_type)smbb;
  pm_smbb_data_type *smbb_ptr = pm_smbb_get_data(pmic_chip);
  pm_register_address_type reg_smbb_vbat_status = smbb_ptr->smbb_reg_table->base_address + (pm_register_address_type)(perph_index*smbb_ptr->smbb_reg_table->peripheral_offset) + smbb_ptr->smbb_reg_table->vbat_status;
  pm_register_address_type reg_smbb_wdog_pet    = smbb_ptr->smbb_reg_table->base_address + (pm_register_address_type)(perph_index*smbb_ptr->smbb_reg_table->peripheral_offset) + smbb_ptr->smbb_reg_table->wdog_pet;


  if (smbb_ptr == NULL)
  {
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }

  if (perph_index >= smbb_ptr->num_of_peripherals)
  {
    err_flag = PM_ERR_FLAG__INVALID_SMBB_INDEXED ;
  }
  else
  {
    if( weak == NULL )
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }

    /* Pet the dog SMBB_CHGR_CHG_WDOG_PET */
    err_flag |= pm_comm_write_byte_mask(smbb_ptr->comm_ptr->slave_id, reg_smbb_wdog_pet, 0x80, 0x80, 0);

    /* Read SMBB_CHGR_VBAT_STATUS bit 1*/
    err_flag |= pm_comm_read_byte(smbb_ptr->comm_ptr->slave_id, reg_smbb_vbat_status, &data, 0);

    /* bit 1: BAT_UPR_STATUS: 0 = Vbatt below threshold, 1 = Vbatt above threshold */
    if( (data & 0x02) == 0x02)
    {
        *weak = FALSE;
    }
    else
    {
        *weak = TRUE;
    }
  }
  return err_flag;
}

pm_err_flag_type
pm_smbb_chg_set_ibat_max
(
   uint8 pmic_chip,
   pm_smbb_which_type     smbb,
   uint32 value_ma
)
{
    uint8        data = 0x00;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 perph_index = (pm_smbb_which_type)smbb;
    pm_smbb_data_type *smbb_ptr = pm_smbb_get_data(pmic_chip);
    pm_register_address_type reg_smbb_ibat_max = smbb_ptr->smbb_reg_table->base_address + (pm_register_address_type)(perph_index*smbb_ptr->smbb_reg_table->peripheral_offset) + smbb_ptr->smbb_reg_table->ibat_max;


    if (smbb_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (perph_index >= smbb_ptr->num_of_peripherals)
    {
      err_flag = PM_ERR_FLAG__INVALID_SMBB_INDEXED ;
    }
    else
    {
        if(value_ma > 3250)
        {
            err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;

            /* Round the value to boundary */
            value_ma = 3250;
        }
        else if(value_ma < 100)
        {
            err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;

            /* Round the value to boundary */
            value_ma = 100;
        }

        data = (uint8) ((value_ma - 100)/50) ;

        err_flag |= pm_comm_write_byte_mask(smbb_ptr->comm_ptr->slave_id, reg_smbb_ibat_max, 0x3F, data, 0);
    }

    return err_flag;
}

pm_err_flag_type
pm_smbb_chg_set_itrkl
(
	uint8 pmic_chip,
    pm_smbb_which_type     smbb,
    uint32                 value_ma
)
{
    uint8        data = 0x00;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 perph_index = (pm_smbb_which_type)smbb;
    pm_smbb_data_type *smbb_ptr = pm_smbb_get_data(pmic_chip);
    pm_register_address_type reg_smbb_ibat_atc_a = smbb_ptr->smbb_reg_table->base_address + (pm_register_address_type)(perph_index*smbb_ptr->smbb_reg_table->peripheral_offset) + smbb_ptr->smbb_reg_table->ibat_atc_a;


    if (smbb_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (perph_index >= smbb_ptr->num_of_peripherals)
    {
      err_flag = PM_ERR_FLAG__INVALID_SMBB_INDEXED ;
    }
    else
    {

        if(value_ma > 200)
        {
            err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;

            /* Round the value to boundary */
            value_ma = 200;
        }

        else if(value_ma < 50)
        {
            err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;

            /* Round the value to boundary */
            value_ma = 50;
        }

        data = (uint8) ((value_ma - 50)/10) ;

        /* Set the new data in SMBB_CHGR_IBAT_ATC_A */
        err_flag |= pm_comm_write_byte_mask(smbb_ptr->comm_ptr->slave_id, reg_smbb_ibat_atc_a, 0x0F, data, 0);
    }

    return err_flag;
}

pm_err_flag_type
pm_smbb_chg_set_mode
(
	uint8 pmic_chip,
    pm_smbb_which_type     smbb,
    pm_chg_mode_type  mode
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 perph_index = (pm_smbb_which_type)smbb;
    pm_smbb_data_type *smbb_ptr = pm_smbb_get_data(pmic_chip);
    pm_register_address_type reg_smbb_chg_ctrl  = smbb_ptr->smbb_reg_table->base_address + (pm_register_address_type)(perph_index*smbb_ptr->smbb_reg_table->peripheral_offset) + smbb_ptr->smbb_reg_table->chg_ctrl;
    pm_register_address_type reg_smbb_atc_ctrl  = smbb_ptr->smbb_reg_table->base_address + (pm_register_address_type)(perph_index*smbb_ptr->smbb_reg_table->peripheral_offset) + smbb_ptr->smbb_reg_table->atc_ctrl;
    pm_register_address_type reg_smbb_boot_done = smbb_ptr->smbb_reg_table->base_address + (pm_register_address_type)(perph_index*smbb_ptr->smbb_reg_table->peripheral_offset) + (pm_register_address_type)(PM_SMBB_MISC*0x0100) + smbb_ptr->smbb_reg_table->boot_done;

    if (smbb_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (perph_index >= smbb_ptr->num_of_peripherals)
    {
        err_flag = PM_ERR_FLAG__INVALID_SMBB_INDEXED ;
    }
    else
    {
        if( mode > PM_CHG_MODE__ATC_ON )
        {
            return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
        }

        switch( mode )
        {
        case PM_CHG_MODE__CHARGE_OFF:
            {
                /*************Disable charging ********/
                /* Clear SMBB_CHGR_CHG_CTRL bit 7 */
                err_flag |= pm_comm_write_byte_mask(smbb_ptr->comm_ptr->slave_id, reg_smbb_chg_ctrl, 0x80, 0, 0);
            }
            break;
        case PM_CHG_MODE__CHARGE_ON:
            {
                /*************Enable charging ********/
                /* Set SMBB_MISC_BOOT_DONE bit 7 */
                err_flag |= pm_comm_write_byte_mask(smbb_ptr->comm_ptr->slave_id, reg_smbb_boot_done, 0x80, 0x80, 0);

                /* Set SMBB_CHGR_CHG_CTRL bit 7 */
                err_flag |= pm_comm_write_byte_mask(smbb_ptr->comm_ptr->slave_id, reg_smbb_chg_ctrl, 0x80, 0x80, 0);
            }
            break;
        case PM_CHG_MODE__ATC_OFF:
            {
                /* Clear SMBB_CHGR_ATC_CTRL bit 7 */
                err_flag |= pm_comm_write_byte_mask(smbb_ptr->comm_ptr->slave_id, reg_smbb_atc_ctrl, 0x80, 0, 0);
            }
            break;
        case PM_CHG_MODE__ATC_ON:
            {
                /* Set SMBB_CHGR_ATC_CTRL bit 7 */
                err_flag |= pm_comm_write_byte_mask(smbb_ptr->comm_ptr->slave_id, reg_smbb_atc_ctrl, 0x80, 0x80, 0);
            }
            break;
        default:
            err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
        }

    }
    return err_flag;
}

pm_err_flag_type
pm_smbb_disable_chg
(
	uint8 pmic_chip,
    pm_smbb_which_type     smbb
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 perph_index = (pm_smbb_which_type)smbb;
    pm_smbb_data_type *smbb_ptr = pm_smbb_get_data(pmic_chip);
    pm_register_address_type reg_smbb_chg_ctrl = smbb_ptr->smbb_reg_table->base_address + (pm_register_address_type)(perph_index*smbb_ptr->smbb_reg_table->peripheral_offset) + smbb_ptr->smbb_reg_table->chg_ctrl;

    if (smbb_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (perph_index >= smbb_ptr->num_of_peripherals)
    {
      err_flag = PM_ERR_FLAG__INVALID_SMBB_INDEXED ;
    }
    else
    {
        err_flag |= pm_comm_write_byte_mask(smbb_ptr->comm_ptr->slave_id, reg_smbb_chg_ctrl, 0x01, 0x01, 0);
    }

    return err_flag;
}

pm_err_flag_type
pm_smbb_chg_get_battery_present
(
	uint8 pmic_chip,
    pm_smbb_which_type   smbb,
    boolean             *batt_present,
    boolean             *batt_therm,
    boolean             *batt_id
)
{
    uint8 data = 0x00;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 perph_index = (pm_smbb_which_type)smbb;
    pm_smbb_data_type *smbb_ptr = pm_smbb_get_data(pmic_chip);
    pm_register_address_type reg_smbb_bat_pres_status = smbb_ptr->smbb_reg_table->base_address + (pm_register_address_type)(perph_index*smbb_ptr->smbb_reg_table->peripheral_offset) + (pm_register_address_type)(PM_SMBB_BAT_IF*0x0100) + smbb_ptr->smbb_reg_table->bat_pres_status;

    if (smbb_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    if (perph_index >= smbb_ptr->num_of_peripherals)
    {
      err_flag = PM_ERR_FLAG__INVALID_SMBB_INDEXED ;
    }
    else
    {
        if( NULL == batt_present || NULL == batt_therm || NULL == batt_id)
        {
            return PM_ERR_FLAG__INVALID_POINTER;
        }

        /* Read SMBB_BAT_IF_BAT_PRES_STATUS bit 7 */
        err_flag |= pm_comm_read_byte(smbb_ptr->comm_ptr->slave_id, reg_smbb_bat_pres_status, &data, 0);

        /* bit 7: BATTERY_PRESENT: 0 = battery not present, 1 = battery present */
        if( (data & 0x80) == 0x80)
        {
            *batt_present = TRUE;
        }
        else
        {
            *batt_present = FALSE;
        }

        if( (data & 0x02) == 0x02)
        {
            *batt_therm = TRUE;
        }
        else
        {
            *batt_therm = FALSE;
        }

        if( (data & 0x01) == 0x01)
        {
            *batt_id = TRUE;
        }
        else
        {
            *batt_id = FALSE;
        }
    }
    return err_flag;
}

pm_err_flag_type
pm_smbb_chg_set_iusb_max
(
	uint8 pmic_chip,
    pm_smbb_which_type    smbb,
    uint32                value_ma
)
{
    uint8 data = 0x00;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 perph_index = (pm_smbb_which_type)smbb;
    pm_smbb_data_type *smbb_ptr = pm_smbb_get_data(pmic_chip);
    pm_register_address_type reg_smbb_iusb_max_en       = smbb_ptr->smbb_reg_table->base_address + (pm_register_address_type)(perph_index*smbb_ptr->smbb_reg_table->peripheral_offset)+ (pm_register_address_type)(PM_SMBB_USB_CHGPTH*0x0100) + smbb_ptr->smbb_reg_table->iusb_max_en;
    pm_register_address_type reg_smbb_iusb_max          = smbb_ptr->smbb_reg_table->base_address + (pm_register_address_type)(perph_index*smbb_ptr->smbb_reg_table->peripheral_offset)+ (pm_register_address_type)(PM_SMBB_USB_CHGPTH*0x0100) + smbb_ptr->smbb_reg_table->iusb_max;
    pm_register_address_type reg_smbb_flcb_iusb_max_lim = smbb_ptr->smbb_reg_table->base_address + (pm_register_address_type)(perph_index*smbb_ptr->smbb_reg_table->peripheral_offset)+ (pm_register_address_type)(PM_SMBB_USB_CHGPTH*0x0100) + smbb_ptr->smbb_reg_table->flcb_iusb_max_lim;

    if (smbb_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (perph_index >= smbb_ptr->num_of_peripherals)
    {
      err_flag = PM_ERR_FLAG__INVALID_SMBB_INDEXED ;
    }
    else
    {
        if(value_ma > 1500)
        {
            err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;

            /* Round the value to boundary */
            value_ma = 1500;
        }
        else if(value_ma < 100)
        {
            err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;

            /* Round the value to boundary */
            value_ma = 100;
        }

        if(value_ma == 100)
        {
            data = 0;
        }
        else if(value_ma == 150)
        {
            data = 1;
        }
        else
        {
            data = (uint8)(value_ma/100);
        }

        /* Set the USB IMAX in SMBB_USB_IUSB_MAX */
        err_flag |= pm_comm_write_byte_mask(smbb_ptr->comm_ptr->slave_id, reg_smbb_iusb_max, 0x1F, data, 0);

        /* Enable the current limiting in SMBB_USB_IUSB_MAX_EN */
        err_flag |= pm_comm_write_byte_mask(smbb_ptr->comm_ptr->slave_id, reg_smbb_iusb_max_en, 0x80, 0x80, 0);

        /* Enable the current limiting in SMBB_USB_FLCB_IUSB_MAX_LIM */
        err_flag |= pm_comm_write_byte_mask(smbb_ptr->comm_ptr->slave_id, reg_smbb_flcb_iusb_max_lim, 0x80, 0x80, 0);
    }
    return err_flag;
}

pm_err_flag_type
pm_smbb_chg_get_attached_charger
(
    uint8 pmic_chip,
    pm_smbb_which_type              smbb,
    pm_chg_attached_chgr_type *type
)
{
    uint8 data1 = 0x00;
    uint8 data2 = 0x00;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 perph_index = (pm_smbb_which_type)smbb;
    pm_smbb_data_type *smbb_ptr = pm_smbb_get_data(pmic_chip);
    pm_register_address_type reg_smbb_usb_chg_pth_sts    = smbb_ptr->smbb_reg_table->base_address + (pm_register_address_type)(perph_index*smbb_ptr->smbb_reg_table->peripheral_offset)+ (pm_register_address_type)(PM_SMBB_USB_CHGPTH*0x0100) + smbb_ptr->smbb_reg_table->usb_chg_pth_sts;
    pm_register_address_type reg_smbb_usb_dc_chg_pth_sts = smbb_ptr->smbb_reg_table->base_address + (pm_register_address_type)(perph_index*smbb_ptr->smbb_reg_table->peripheral_offset)+ (pm_register_address_type)(PM_SMBB_DC_CHGPATH*0x0100) + smbb_ptr->smbb_reg_table->dc_chg_pth_sts;

    if (smbb_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (perph_index >= smbb_ptr->num_of_peripherals)
    {
      err_flag = PM_ERR_FLAG__INVALID_SMBB_INDEXED ;
    }
    else
    {
        if( type == NULL )
        {
            return PM_ERR_FLAG__INVALID_POINTER;
        }

        /* Read SMBB_USB_CHG_PTH_STS bit <7:6> */
        err_flag |= pm_comm_read_byte(smbb_ptr->comm_ptr->slave_id, reg_smbb_usb_chg_pth_sts, &data1, 0);

        /* Read SMBB_DC_DC_CHG_PTH_STS bit <7:6> */
        err_flag |= pm_comm_read_byte(smbb_ptr->comm_ptr->slave_id, reg_smbb_usb_dc_chg_pth_sts, &data2, 0);

        if (data1 & 0x80)
        {
            *type = PM_CHG_ATTACHED_CHGR__USB;

            if (data2 & 0x80)
            {
               *type = PM_CHG_ATTACHED_CHGR__BOTH;
               return PM_ERR_FLAG__SUCCESS;
            }
        }
        else if (data2 & 0x80)
        {
            *type = PM_CHG_ATTACHED_CHGR__DCIN;
        }
        else
        {
            *type = PM_CHG_ATTACHED_CHGR__NONE;
        }
    }
    return err_flag;
}

pm_err_flag_type
pm_smbb_chg_set_atc_led
(
	uint8 pmic_chip,
    pm_smbb_which_type              smbb,
    boolean                         atc_led_state
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 perph_index = (pm_smbb_which_type)smbb;
    pm_smbb_data_type *smbb_ptr = pm_smbb_get_data(pmic_chip);
    pm_register_address_type reg_smbb_atc_led = smbb_ptr->smbb_reg_table->base_address + (pm_register_address_type)(perph_index*smbb_ptr->smbb_reg_table->peripheral_offset) + smbb_ptr->smbb_reg_table->atc_led;


    if (smbb_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (atc_led_state)
    {
       err_flag |= pm_comm_write_byte_mask(smbb_ptr->comm_ptr->slave_id, reg_smbb_atc_led, 0x01, 0x01, 0); //ATC LED ON
    }
    else
    {
       err_flag |= pm_comm_write_byte_mask(smbb_ptr->comm_ptr->slave_id, reg_smbb_atc_led, 0x01, 0x00, 0); //ATC LED OFF
    }

    return err_flag;
}

pm_err_flag_type
pm_smbb_chg_get_chg_gone_rt_status
(
	uint8 pmic_chip,
    pm_smbb_which_type              smbb,
    boolean                         *chg_gone_rt_status
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 perph_index = (pm_smbb_which_type)smbb;
    uint8 data = 0x00;
    pm_smbb_data_type *smbb_ptr = pm_smbb_get_data(pmic_chip);
    pm_register_address_type reg_smbb_chg_gone_rt_status  = smbb_ptr->smbb_reg_table->base_address + (pm_register_address_type)(perph_index*smbb_ptr->smbb_reg_table->peripheral_offset)+ (pm_register_address_type)(PM_SMBB_USB_CHGPTH*0x0100) + smbb_ptr->smbb_reg_table->chg_gone_rt_status;


    if (smbb_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (perph_index >= smbb_ptr->num_of_peripherals)
    {
      err_flag = PM_ERR_FLAG__INVALID_SMBB_INDEXED ;
    }
    else
    {
        if(chg_gone_rt_status == NULL)
        {
            return PM_ERR_FLAG__INVALID_POINTER;
        }

        /* Read CHG_GONE RT STatus bit 2 */
        err_flag |= pm_comm_read_byte(smbb_ptr->comm_ptr->slave_id, reg_smbb_chg_gone_rt_status, &data, 0);

        /* bit 2: CHG GONE RT Status*/
        if( (data & 0x04) == 0x04)
        {
            *chg_gone_rt_status = TRUE;
        }
        else
        {
            *chg_gone_rt_status = FALSE;
        }
    }
    return err_flag;
}




pm_err_flag_type
pm_smbb_chg_set_comp_override
(
	uint8 pmic_chip,
    pm_smbb_which_type              smbb,
    boolean                         comp_override_status
)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 perph_index = (pm_smbb_which_type)smbb;
    pm_smbb_data_type *smbb_ptr = pm_smbb_get_data(pmic_chip);
    pm_register_address_type reg_smbb_sec_access = smbb_ptr->smbb_reg_table->base_address + (pm_register_address_type)(perph_index*smbb_ptr->smbb_reg_table->peripheral_offset) + smbb_ptr->smbb_reg_table->sec_access;
    pm_register_address_type reg_smbb_comp_over1 = smbb_ptr->smbb_reg_table->base_address + (pm_register_address_type)(perph_index*smbb_ptr->smbb_reg_table->peripheral_offset) + smbb_ptr->smbb_reg_table->comp_over1;

    if (smbb_ptr == NULL)
    {
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (comp_override_status)
    {
       err_flag  = pm_comm_write_byte_mask(smbb_ptr->comm_ptr->slave_id, reg_smbb_sec_access, 0xFF, 0xA5, 0);
       err_flag |= pm_comm_write_byte_mask(smbb_ptr->comm_ptr->slave_id, reg_smbb_comp_over1, 0xFF, 0x20, 0); //Enable COMP_OVERR1
    }
    else
    {
       err_flag  = pm_comm_write_byte_mask(smbb_ptr->comm_ptr->slave_id, reg_smbb_sec_access, 0xFF, 0xA5, 0);
       err_flag |= pm_comm_write_byte_mask(smbb_ptr->comm_ptr->slave_id, reg_smbb_comp_over1, 0xFF, 0x00, 0); //Disable COMP_OVERR1
    }

    return err_flag;
}

