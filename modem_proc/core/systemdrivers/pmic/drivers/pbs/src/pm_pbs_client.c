/*! \file pm_pbs_client.c
*  \n
*  \brief This file contains the implementation of the public APIs for PBS Client module.
*  \n
*  \details Implementation file for PBS Client module. Each of the APIs checks for
*           access and then if necessary directs the call to Driver implementation.
*  \n
*  &copy; Copyright 2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/*===========================================================================
EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/drivers/pbs/src/pm_pbs_client.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/16/13   kt      Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_pbs_client_driver.h"
#include "pm_pbs_client.h"
#include "pm_irq.h"


/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
pm_err_flag_type pm_pbs_client_enable(uint8 pmic_chip, uint8 pbs_client_index, boolean enable)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_pbs_client_data_type *pbs_client_ptr = pm_pbs_client_get_data(pmic_chip);
    uint8 data = 0;
    uint8 mask = 0x80;

    if (pbs_client_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (pbs_client_index >= pbs_client_ptr->pbs_client_common->num_peripherals)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED ;
    }

    pm_register_address_type en_ctl1 = pbs_client_ptr->pbs_client_common->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->en_ctl1;

    if (enable == TRUE)
    {
        /* Enable PBS Client module */
        data = 0xFF;
    }
    else
    {
        /* Disable PBS Client module */
        data = 0x00;
    }

    err_flag = pm_comm_write_byte_mask(pbs_client_ptr->comm_ptr->slave_id, en_ctl1, mask, data, 0);

    return err_flag;
}

pm_err_flag_type pm_pbs_client_enable_status(uint8 pmic_chip, uint8 pbs_client_index, boolean *status)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_pbs_client_data_type *pbs_client_ptr = pm_pbs_client_get_data(pmic_chip);
    uint8 mask = 0x80;
    uint8 data = 0;

    if (pbs_client_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (pbs_client_index >= pbs_client_ptr->pbs_client_common->num_peripherals)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED ;
    }

    pm_register_address_type en_ctl1 = pbs_client_ptr->pbs_client_common->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->en_ctl1;

    /* Read PBS Client module enable status */
    err_flag = pm_comm_read_byte_mask(pbs_client_ptr->comm_ptr->slave_id, en_ctl1, mask, &data,0);

    *status = data ? TRUE : FALSE;

    return err_flag;
}

pm_err_flag_type pm_pbs_client_trigger_cfg(uint8 pmic_chip, uint8 pbs_client_index, pm_pbs_client_trigger_type trigger)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_pbs_client_data_type *pbs_client_ptr = pm_pbs_client_get_data(pmic_chip);
    uint8 data = 0;

    if (pbs_client_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (pbs_client_index >= pbs_client_ptr->pbs_client_common->num_peripherals)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED ;
    }

    pm_register_address_type TRIG_CFG = pbs_client_ptr->pbs_client_common->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->trig_cfg;

    switch (trigger)
    {
    case PM_PBS_CLIENT_TRIGGER_RISING_EDGE:
      data = 0x80;
      break;
    case PM_PBS_CLIENT_TRIGGER_FALLING_EDGE:
      data = 0x40;
      break;
    case PM_PBS_CLIENT_TRIGGER_DUAL_EDGE:
      data = 0xC0;
      break;
    default:
      return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    err_flag = pm_comm_write_byte(pbs_client_ptr->comm_ptr->slave_id, TRIG_CFG, data, 0);

    return err_flag;
}

pm_err_flag_type pm_pbs_client_irq_enable(uint8 pmic_chip, uint8 pbs_client_index, pm_pbs_client_irq_type irq, boolean enable)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_pbs_client_data_type *pbs_client_ptr = pm_pbs_client_get_data(pmic_chip);
    uint8 mask = 1<<irq;

    if (pbs_client_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (pbs_client_index >= pbs_client_ptr->pbs_client_common->num_peripherals)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED ;
    }

    pm_register_address_type INT_EN_SET = pbs_client_ptr->pbs_client_common->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_en_set;
    pm_register_address_type INT_EN_CLR = pbs_client_ptr->pbs_client_common->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_en_clr;

    if (enable == TRUE)
    {
        err_flag = pm_comm_write_byte(pbs_client_ptr->comm_ptr->slave_id, INT_EN_SET, mask, 0);
    }
    else
    {
        err_flag = pm_comm_write_byte(pbs_client_ptr->comm_ptr->slave_id, INT_EN_CLR, mask, 0);
    }

    return err_flag;
}

pm_err_flag_type pm_pbs_client_irq_clear(uint8 pmic_chip, uint8 pbs_client_index, pm_pbs_client_irq_type irq)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_pbs_client_data_type *pbs_client_ptr = pm_pbs_client_get_data(pmic_chip);
    uint8 mask = 1<<irq;

    if (pbs_client_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (pbs_client_index >= pbs_client_ptr->pbs_client_common->num_peripherals)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED ;
    }

    pm_register_address_type INT_LATCHED_CLR = pbs_client_ptr->pbs_client_common->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_latched_clr;

    err_flag = pm_comm_write_byte(pbs_client_ptr->comm_ptr->slave_id, INT_LATCHED_CLR, mask, 0);

    return err_flag;
}

pm_err_flag_type pm_pbs_client_irq_set_trigger(uint8 pmic_chip, uint8 pbs_client_index, pm_pbs_client_irq_type irq, pm_irq_trigger_type trigger)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_pbs_client_data_type *pbs_client_ptr = pm_pbs_client_get_data(pmic_chip);
    uint8 mask = 1<<irq;
    uint8 set_type = 0;
    uint8 polarity_high = 0;
    uint8 polarity_low = 0;

    if (pbs_client_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (pbs_client_index >= pbs_client_ptr->pbs_client_common->num_peripherals)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED ;
    }

    pm_register_address_type INT_SET_TYPE = pbs_client_ptr->pbs_client_common->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_set_type;
    pm_register_address_type INT_POLARITY_HIGH = pbs_client_ptr->pbs_client_common->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_polarity_high;
    pm_register_address_type INT_POLARITY_LOW = pbs_client_ptr->pbs_client_common->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_polarity_low;

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

    err_flag = pm_comm_write_byte_mask(pbs_client_ptr->comm_ptr->slave_id, INT_SET_TYPE, mask, set_type, 0);
    err_flag |= pm_comm_write_byte_mask(pbs_client_ptr->comm_ptr->slave_id, INT_POLARITY_HIGH, mask, polarity_high, 0);
    err_flag |= pm_comm_write_byte_mask(pbs_client_ptr->comm_ptr->slave_id, INT_POLARITY_LOW, mask, polarity_low, 0);

    return err_flag;
}

pm_err_flag_type pm_pbs_client_irq_get_trigger(uint8 pmic_chip, uint8 pbs_client_index, pm_pbs_client_irq_type irq, pm_irq_trigger_type *trigger)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_pbs_client_data_type *pbs_client_ptr = pm_pbs_client_get_data(pmic_chip);
    uint8 mask = 1<<irq;
    uint8 set_type = 0;
    uint8 polarity_high = 0;
    uint8 polarity_low = 0;

    if (pbs_client_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (pbs_client_index >= pbs_client_ptr->pbs_client_common->num_peripherals)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED ;
    }

    pm_register_address_type INT_SET_TYPE = pbs_client_ptr->pbs_client_common->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_set_type;
    pm_register_address_type INT_POLARITY_HIGH = pbs_client_ptr->pbs_client_common->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_polarity_high;
    pm_register_address_type INT_POLARITY_LOW = pbs_client_ptr->pbs_client_common->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_polarity_low;

    err_flag = pm_comm_read_byte_mask(pbs_client_ptr->comm_ptr->slave_id, INT_SET_TYPE, mask, &set_type,0);
    err_flag |= pm_comm_read_byte_mask(pbs_client_ptr->comm_ptr->slave_id, INT_POLARITY_HIGH, mask, &polarity_high,0);
    err_flag |= pm_comm_read_byte_mask(pbs_client_ptr->comm_ptr->slave_id, INT_POLARITY_LOW, mask, &polarity_low,0);

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

pm_err_flag_type pm_pbs_client_irq_status(uint8 pmic_chip, uint8 pbs_client_index, pm_pbs_client_irq_type irq, pm_irq_status_type type, boolean *status)
{
    pm_err_flag_type err_flag   = PM_ERR_FLAG__SUCCESS;
    pm_pbs_client_data_type *pbs_client_ptr = pm_pbs_client_get_data(pmic_chip);
    pm_register_address_type INT_STS;
    uint8 mask = 1<<irq;
    uint8 data = 0;

    if (pbs_client_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (pbs_client_index >= pbs_client_ptr->pbs_client_common->num_peripherals)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED ;
    }

    switch (type)
    {
    case PM_IRQ_STATUS_RT:
        INT_STS = pbs_client_ptr->pbs_client_common->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_rt_status;
        break;
    case PM_IRQ_STATUS_LATCHED:
        INT_STS = pbs_client_ptr->pbs_client_common->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_latched_status;
        break;
    case PM_IRQ_STATUS_PENDING:
        INT_STS = pbs_client_ptr->pbs_client_common->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_pending_status;
        break;
    case PM_IRQ_STATUS_ENABLE:
        INT_STS = pbs_client_ptr->pbs_client_common->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->int_en_set;
        break;
    default:
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }

    err_flag = pm_comm_read_byte_mask(pbs_client_ptr->comm_ptr->slave_id, INT_STS, mask, &data,0);
    *status = data ? TRUE : FALSE;

    return err_flag;
}

pm_err_flag_type pm_pbs_client_sw_trigger(uint8 pmic_chip, uint8 pbs_client_index)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_pbs_client_data_type *pbs_client_ptr = pm_pbs_client_get_data(pmic_chip);
    uint8 data = 1;

    if (pbs_client_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (pbs_client_index >= pbs_client_ptr->pbs_client_common->num_peripherals)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED ;
    }

    pm_register_address_type trig_ctl = pbs_client_ptr->pbs_client_common->base_address + (pbs_client_index*0x0100) + pbs_client_ptr->pbs_client_reg_table->trig_ctl;

    err_flag = pm_comm_write_byte(pbs_client_ptr->comm_ptr->slave_id, trig_ctl, data, 0);

    return err_flag;
}
