/*! \file pm_bua.c
*  \n
*  \brief This file contains the implementation of the public APIs for BUA module.
*  \n
*  \details Implementation file for BUA module. Each of the APIs checks for
*           access and then if necessary directs the call to Driver implementation.
*  \n
*  &copy; Copyright 2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/drivers/bua/src/pm_bua.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/16/13   kt      Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_bua_driver.h"
#include "pm_bua.h"


/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
pm_err_flag_type pm_bua_enable(uint8 pmic_chip, boolean enable)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 data = 0;
    uint8 mask = 0x80;
    pm_bua_data_type *bua_ptr = pm_bua_get_data(pmic_chip);

    if (bua_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_register_address_type en_ctl1 = bua_ptr->bua_common->base_address + bua_ptr->bua_reg_table->en_ctl1;

    if (enable == TRUE)
    {
        /* Enable BUA module */
        data = 0xFF;
    }
    else
    {
        /* Disable BUA module */
        data = 0x00;
    }

    err_flag = pm_comm_write_byte_mask(bua_ptr->comm_ptr->slave_id, en_ctl1, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_bua_enable_status(uint8 pmic_chip, boolean *status)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_bua_data_type *bua_ptr = pm_bua_get_data(pmic_chip);
    uint8 mask = 0x80;
    uint8 data = 0;

    if (bua_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_register_address_type STATUS = bua_ptr->bua_common->base_address + bua_ptr->bua_reg_table->status1;

    /* Read BUA_OK bit from status register */
    err_flag = pm_comm_read_byte_mask(bua_ptr->comm_ptr->slave_id, STATUS, mask, &data,0);

    *status = data ? TRUE : FALSE;

    return err_flag;
}

pm_err_flag_type pm_bua_alarm_detect_status(uint8 pmic_chip, pm_bua_alarm_type alarm, boolean *status)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_bua_data_type *bua_ptr = pm_bua_get_data(pmic_chip);
    pm_register_address_type STATUS;
    uint8 mask = 0;
    uint8 data = 0;

    if (bua_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ((alarm >= PM_BUA_INVALID_ALARM) || (alarm >= bua_ptr->bua_specific->num_alarms_supported))
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED ;
    }

    switch (alarm)
    {
    case PM_BUA_BATT_ALARM:
        STATUS = bua_ptr->bua_common->base_address + bua_ptr->bua_reg_table->status1;
        mask = 0x40;
        break;
    default:
        STATUS = bua_ptr->bua_common->base_address + bua_ptr->bua_reg_table->status2;
        mask = 1<<(((uint8)alarm) - 1);
        break;
    }

    err_flag = pm_comm_read_byte_mask(bua_ptr->comm_ptr->slave_id, STATUS, mask, &data,0);
    *status = data ? TRUE : FALSE;

    return err_flag;
}

pm_err_flag_type pm_bua_irq_enable(uint8 pmic_chip, pm_bua_alarm_type alarm, boolean enable)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 data = 1 << alarm;
    pm_bua_data_type *bua_ptr = pm_bua_get_data(pmic_chip);

    if (bua_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_register_address_type INT_EN_SET = bua_ptr->bua_common->base_address + bua_ptr->bua_reg_table->int_en_set;
    pm_register_address_type INT_EN_CLR = bua_ptr->bua_common->base_address + bua_ptr->bua_reg_table->int_en_clr;

    if ((alarm >= PM_BUA_INVALID_ALARM) || (alarm >= bua_ptr->bua_specific->num_alarms_supported))
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED ;
    }

    if (enable == TRUE)
    {
        err_flag = pm_comm_write_byte(bua_ptr->comm_ptr->slave_id, INT_EN_SET, data, 0);
    }
    else
    {
        err_flag = pm_comm_write_byte(bua_ptr->comm_ptr->slave_id, INT_EN_CLR, data, 0);
    }

    return err_flag;
}

pm_err_flag_type pm_bua_irq_clear(uint8 pmic_chip, pm_bua_alarm_type alarm)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 data = 1 << alarm;
    pm_bua_data_type *bua_ptr = pm_bua_get_data(pmic_chip);

    if (bua_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_register_address_type INT_LATCHED_CLR = bua_ptr->bua_common->base_address + bua_ptr->bua_reg_table->int_latched_clr;

    if ((alarm >= PM_BUA_INVALID_ALARM) || (alarm >= bua_ptr->bua_specific->num_alarms_supported))
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED ;
    }

    err_flag = pm_comm_write_byte(bua_ptr->comm_ptr->slave_id, INT_LATCHED_CLR, data, 0);

    return err_flag;
}

pm_err_flag_type pm_bua_irq_set_trigger(uint8 pmic_chip, pm_bua_alarm_type alarm, pm_irq_trigger_type trigger)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 1 << alarm;
    uint8 set_type = 0;
    uint8 polarity_high = 0;
    uint8 polarity_low = 0;
    pm_bua_data_type *bua_ptr = pm_bua_get_data(pmic_chip);

    if (bua_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_register_address_type INT_SET_TYPE = bua_ptr->bua_common->base_address + bua_ptr->bua_reg_table->int_set_type;
    pm_register_address_type INT_POLARITY_HIGH = bua_ptr->bua_common->base_address + bua_ptr->bua_reg_table->int_polarity_high;
    pm_register_address_type INT_POLARITY_LOW = bua_ptr->bua_common->base_address + bua_ptr->bua_reg_table->int_polarity_low;

    if ((alarm >= PM_BUA_INVALID_ALARM) || (alarm >= bua_ptr->bua_specific->num_alarms_supported))
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED ;
    }

    switch (trigger)
    {
    case PM_IRQ_TRIGGER_ACTIVE_LOW:
      set_type = 0x00;
      polarity_high = 0x00;
      polarity_low = 0xFF;
      break;
    case PM_IRQ_TRIGGER_ACTIVE_HIGH:
      set_type = 0x00;
      polarity_high = 0xFF;
      polarity_low = 0x00;
      break;
    case PM_IRQ_TRIGGER_RISING_EDGE:
      set_type = 0xFF;
      polarity_high = 0xFF;
      polarity_low = 0x00;
      break;
    case PM_IRQ_TRIGGER_FALLING_EDGE:
      set_type = 0xFF;
      polarity_high = 0x00;
      polarity_low = 0xFF;
      break;
    case PM_IRQ_TRIGGER_DUAL_EDGE:
      set_type = 0xFF;
      polarity_high = 0xFF;
      polarity_low = 0xFF;
      break;
    default:
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    err_flag = pm_comm_write_byte_mask(bua_ptr->comm_ptr->slave_id, INT_SET_TYPE, mask, set_type, 0);
    err_flag |= pm_comm_write_byte_mask(bua_ptr->comm_ptr->slave_id, INT_POLARITY_HIGH, mask, polarity_high, 0);
    err_flag |= pm_comm_write_byte_mask(bua_ptr->comm_ptr->slave_id, INT_POLARITY_LOW, mask, polarity_low, 0);

    return err_flag;
}

pm_err_flag_type pm_bua_irq_get_trigger(uint8 pmic_chip, pm_bua_alarm_type alarm, pm_irq_trigger_type *trigger)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 mask = 1 << alarm;
    uint8 set_type = 0;
    uint8 polarity_high = 0;
    uint8 polarity_low = 0;
    pm_bua_data_type *bua_ptr = pm_bua_get_data(pmic_chip);

    if (bua_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    pm_register_address_type INT_SET_TYPE = bua_ptr->bua_common->base_address + bua_ptr->bua_reg_table->int_set_type;
    pm_register_address_type INT_POLARITY_HIGH = bua_ptr->bua_common->base_address + bua_ptr->bua_reg_table->int_polarity_high;
    pm_register_address_type INT_POLARITY_LOW = bua_ptr->bua_common->base_address + bua_ptr->bua_reg_table->int_polarity_low;

    if ((alarm >= PM_BUA_INVALID_ALARM) || (alarm >= bua_ptr->bua_specific->num_alarms_supported))
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED ;
    }

    err_flag = pm_comm_read_byte_mask(bua_ptr->comm_ptr->slave_id, INT_SET_TYPE, mask, &set_type,0);
    err_flag |= pm_comm_read_byte_mask(bua_ptr->comm_ptr->slave_id, INT_POLARITY_HIGH, mask, &polarity_high,0);
    err_flag |= pm_comm_read_byte_mask(bua_ptr->comm_ptr->slave_id, INT_POLARITY_LOW, mask, &polarity_low,0);

    if ((set_type == 0) && (polarity_high == 0) && (polarity_low == mask))
    {
        *trigger = PM_IRQ_TRIGGER_ACTIVE_LOW;
    }
    else if ((set_type == 0) && (polarity_high == mask) && (polarity_low == 0))
    {
        *trigger = PM_IRQ_TRIGGER_ACTIVE_HIGH;
    }
    else if ((set_type == mask) && (polarity_high == mask) && (polarity_low == 0))
    {
        *trigger = PM_IRQ_TRIGGER_RISING_EDGE;
    }
    else if ((set_type == mask) && (polarity_high == 0) && (polarity_low == mask))
    {
        *trigger = PM_IRQ_TRIGGER_FALLING_EDGE;
    }
    else if ((set_type == mask) && (polarity_high == mask) && (polarity_low == mask))
    {
        *trigger = PM_IRQ_TRIGGER_DUAL_EDGE;
    }
    else
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    return err_flag;
}

pm_err_flag_type pm_bua_irq_status(uint8 pmic_chip, pm_bua_alarm_type alarm, pm_irq_status_type type, boolean *status)
{
    pm_err_flag_type err_flag   = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type INT_STS;
    uint8 mask = 1 << alarm;
    uint8 data = 0;
    pm_bua_data_type *bua_ptr = pm_bua_get_data(pmic_chip);

    if (bua_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ((alarm >= PM_BUA_INVALID_ALARM) || (alarm >= bua_ptr->bua_specific->num_alarms_supported))
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    switch (type)
    {
    case PM_IRQ_STATUS_RT:
        INT_STS = bua_ptr->bua_common->base_address + bua_ptr->bua_reg_table->int_rt_status;
        break;
    case PM_IRQ_STATUS_LATCHED:
        INT_STS = bua_ptr->bua_common->base_address + bua_ptr->bua_reg_table->int_latched_status;
        break;
    case PM_IRQ_STATUS_PENDING:
        INT_STS = bua_ptr->bua_common->base_address + bua_ptr->bua_reg_table->int_pending_status;
        break;
    case PM_IRQ_STATUS_ENABLE:
        INT_STS = bua_ptr->bua_common->base_address + bua_ptr->bua_reg_table->int_en_set;
        break;
    default:
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    err_flag = pm_comm_read_byte_mask(bua_ptr->comm_ptr->slave_id, INT_STS, mask, &data,0);
    *status = data ? TRUE : FALSE;

    return err_flag;
}
