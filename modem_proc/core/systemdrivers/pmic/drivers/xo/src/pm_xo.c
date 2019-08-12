/*! \file
*
*  \brief  mpp_app.cpp ----File contains the implementation of the public APIs for XO resource type.
*  \details Implementation file for XO resource type.
*
*  &copy; Copyright 2013 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/drivers/xo/src/pm_xo.c#1 $
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/10/13   rh      File created

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_xo_core.h"
#include "pm_xo_driver.h"


/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
pm_err_flag_type pm_dev_xo_core_set_power_mode(uint8 pmic_chip, int internalResourceIndex, pm_xo_core_power_mode_type  mode)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type reg = 0;
    pm_xo_core_data_type *xo_ptr = pm_xo_core_get_data(pmic_chip);

    if (xo_ptr == NULL)
    {
        errFlag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    else if  ( internalResourceIndex >= xo_ptr->num_of_peripherals )
    {
        errFlag = PM_ERR_FLAG__INVALID_TCXO_INDEXED ;
    }
    else if( mode > PM_XO_CORE_PWR_MODE__HPM )
    {
        errFlag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
    else
    {
        reg = xo_ptr->xo_core_register->base_address + (pm_register_address_type)(xo_ptr->xo_core_register->peripheral_offset*internalResourceIndex) +
              xo_ptr->xo_core_register->mode_ctl1;

        errFlag = pm_comm_write_byte_mask(xo_ptr->comm_ptr->slave_id,  reg, 0x03, (pm_register_data_type)(mode), 0);   // 1:0
    }

    /* Let the user know if we were successful or not */
    return errFlag;
}

pm_err_flag_type pm_dev_mega_xo_set_xo_trim(uint8 pmic_chip, int internalResourceIndex, uint8 trim_value)
{
    pm_err_flag_type      errFlag   = PM_ERR_FLAG__SUCCESS;
    pm_err_flag_type      err       = PM_ERR_FLAG__SUCCESS;
    pm_xo_core_data_type  *xo_ptr = pm_xo_core_get_data(pmic_chip);

    if (xo_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if  ( internalResourceIndex >= xo_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__INVALID_TCXO_INDEXED ;
    }

    pm_register_address_type reg = xo_ptr->xo_core_register->base_address + (pm_register_address_type)(xo_ptr->xo_core_register->peripheral_offset*internalResourceIndex) + xo_ptr->xo_core_register->xo_adj;

    if(trim_value > (xo_ptr->xo_core_common->trim_max))
    {
        /* Round the value to the boundary */
        trim_value = xo_ptr->xo_core_common->trim_max;

        /* Set error flag */
        err = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

    errFlag = pm_comm_write_byte_mask(xo_ptr->comm_ptr->slave_id,  reg, 0x3F, (pm_register_data_type)trim_value, 0 ); // 5:0

    if(err == PM_ERR_FLAG__SUCCESS)
    {
        return errFlag;
    }
    else
    {
        return err;
    }
}

pm_err_flag_type pm_dev_mega_xo_get_xo_trim(uint8 pmic_chip, int internalResourceIndex, uint8*  trim_value)
{
    pm_err_flag_type      errFlag   = PM_ERR_FLAG__SUCCESS;
    pm_register_data_type data = 0;
    pm_xo_core_data_type  *xo_ptr = pm_xo_core_get_data(pmic_chip);

    if (xo_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if  ( internalResourceIndex >= xo_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__INVALID_TCXO_INDEXED ;
    }
    if(NULL == trim_value)
    {
        return PM_ERR_FLAG__INVALID_POINTER ;
    }

    pm_register_address_type reg = xo_ptr->xo_core_register->base_address + (pm_register_address_type)(xo_ptr->xo_core_register->peripheral_offset*internalResourceIndex) + xo_ptr->xo_core_register->xo_adj;

    errFlag = pm_comm_read_byte(xo_ptr->comm_ptr->slave_id, reg, &data, 0);

    if (errFlag == PM_ERR_FLAG__SUCCESS)
    {
        *trim_value = (uint8)(data & 0x3F); // 5:0
    }

    return errFlag;
}

pm_err_flag_type pm_dev_mega_xo_set_xo_core_force_on(uint8 pmic_chip, int internalResourceIndex,boolean  xo_core_enable)
{
    pm_err_flag_type      errFlag   = PM_ERR_FLAG__SUCCESS;
    pm_xo_core_data_type  *xo_ptr = pm_xo_core_get_data(pmic_chip);

    if (xo_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ( internalResourceIndex >= xo_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__INVALID_TCXO_INDEXED ;
    }

    pm_register_data_type    data = 0;
    pm_register_address_type reg = xo_ptr->xo_core_register->base_address + (pm_register_address_type)(xo_ptr->xo_core_register->peripheral_offset*internalResourceIndex) + xo_ptr->xo_core_register->en_ctl;

    if(TRUE == xo_core_enable)
    {
        data = 0xFF;
    }
    else
    {
        data = 0;
    }

    errFlag = pm_comm_write_byte_mask(xo_ptr->comm_ptr->slave_id,  reg, 0x80, data, 0 ); // 7

    return errFlag;
}

/*===========================================================================

                        DEPRECATED FUNCTIONS

===========================================================================*/
pm_err_flag_type pm_xo_core_set_power_mode(int externalResourceIndex, pm_xo_core_power_mode_type mode)
{
    return pm_dev_xo_core_set_power_mode(0, externalResourceIndex, mode);
}
pm_err_flag_type pm_mega_xo_set_xo_trim(int externalResourceIndex, uint8 trim_value)
{
    return pm_dev_mega_xo_set_xo_trim(0, externalResourceIndex, trim_value);
}
pm_err_flag_type pm_xo_core_get_xo_trim(int externalResourceIndex, uint8* trim_value)
{
    return pm_dev_mega_xo_get_xo_trim(0, externalResourceIndex, trim_value);
}
pm_err_flag_type pm_xo_core_set_xo_core_force_on(int externalResourceIndex, boolean xo_core_enable)
{
    return pm_dev_mega_xo_set_xo_core_force_on(0, externalResourceIndex, xo_core_enable);
}
