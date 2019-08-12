/*! \file pm_mpp.c
*  
*  \brief Implementation file for MPP public APIs.
*  \n  
*  &copy; Copyright 2010-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/pmic/drivers/mpp/src/pm_mpp.c#1 $ 

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture   
02/27/13   al      Adding IRQ related APIs  
01/24/13   al      Added get real time interrupt status
04/10/12   hs      Changed the type for internalResourceIndex from int to uint8.
03/02/12   hs	   Removed deprecated APIs.

===========================================================================*/

/*===========================================================================

INCLUDE FILES 

===========================================================================*/
#include "pm_mpp.h"
#include "pm_mpp_driver.h"
#include "CoreVerify.h"
#include "pm_comm.h"

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
static pm_err_flag_type pm_mpp_configure_type(pm_drv_mpp_config_type type, uint8  perph_index);
static pm_err_flag_type pm_mpp_config_dvin(pm_mpp_dlogic_lvl_type  level, uint8  perph_index);
static pm_err_flag_type pm_mpp_is_dbus_valid(pm_mpp_dlogic_in_dbus_type  dbus, uint8  perph_index);
static pm_err_flag_type pm_mpp_abus_pulldown(uint8  which_ch, pm_drv_mpp_config_type  mpp_type, uint8  perph_index);
/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
pm_err_flag_type pm_mpp_config_digital_input(unsigned pmic_chip, pm_mpp_perph_index mpp, pm_mpp_dlogic_lvl_type  level, pm_mpp_dlogic_in_dbus_type  dbus)
{
    /* Use this variable to keep track if the operation was successful or not */
    pm_err_flag_type         errFlag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type dig_in_ctl;
    pm_mpp_data_type *mpp_ptr = pm_mpp_get_data(pmic_chip);
    pm_register_address_type perph_index = (pm_register_address_type)mpp;
    

	if (mpp_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ((uint8)perph_index >= mpp_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__INVALID_MPP_INDEXED;
    }
    if (level >= PM_MPP__DLOGIC__LVL_INVALID )
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
    if (dbus >= PM_MPP__DLOGIC_IN__DBUS_INVALID )
    {
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }

	dig_in_ctl = mpp_ptr->mpp_reg_table->base_address + (mpp_ptr->mpp_reg_table->peripheral_offset*perph_index) + mpp_ptr->mpp_reg_table->dig_in_ctl;
			  
    /* Make sure that the user made a valid dbus selection. */
    errFlag = pm_mpp_is_dbus_valid(dbus, (uint8)perph_index);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    /* Check whether this config will enable or disable ABUS pull down */ 
    errFlag = pm_mpp_abus_pulldown((uint8)level, PM_MPP_DIG_IN, (uint8)perph_index);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    // Set MPP_TYPE to DIGITAL INPUT
    errFlag = pm_mpp_configure_type(PM_MPP_DIG_IN, (uint8)perph_index);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    // Configure the VOLTAGE_SEL 
    errFlag = pm_mpp_config_dvin(level, (uint8)perph_index);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    // Enable Digital Input buffer
    //DTEST4	Route to DTEST4
    //DTEST3	Route to DTEST3
    //DTEST2	Route to DTEST2
    //DTEST1	Route to DTEST1
    // DTEST lines can be used in parallel
    errFlag = pm_comm_write_byte_mask(mpp_ptr->comm_ptr->slave_id, dig_in_ctl,(pm_register_mask_type)0x0F, (pm_register_data_type)(1<<dbus), 0);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    // Enable the MPP
    errFlag = pm_mpp_enable(pmic_chip, mpp, TRUE);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    return errFlag;
}


pm_err_flag_type pm_mpp_config_digital_output(unsigned pmic_chip, pm_mpp_perph_index mpp, pm_mpp_dlogic_lvl_type  level, pm_mpp_dlogic_out_ctrl_type  output_ctrl)
{
    /* Use this variable to keep track if the operation was successful or not */
    pm_err_flag_type         errFlag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type mode_ctl;
    pm_mpp_data_type *mpp_ptr = pm_mpp_get_data(pmic_chip);
    pm_register_address_type perph_index = (pm_register_address_type)mpp;
	
	if (mpp_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ((uint8)perph_index >= mpp_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__INVALID_MPP_INDEXED;
    }
    if (level >= PM_MPP__DLOGIC__LVL_INVALID )
    {
        /* Parm 1 is out of range */
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
    if (output_ctrl >= PM_MPP__DLOGIC_OUT__CTRL_INVALID)
    {
        /* Parm 2 is out of range */
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
	
	mode_ctl = mpp_ptr->mpp_reg_table->base_address + (mpp_ptr->mpp_reg_table->peripheral_offset*perph_index) + mpp_ptr->mpp_reg_table->mode_ctl;
		
    /* Check whether this config will enable or disable ABUS pulldown */ 
    errFlag = pm_mpp_abus_pulldown((uint8)level, PM_MPP_DIG_OUT, (uint8)perph_index);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    // Set MPP_TYPE to DIGITAL OUTPUT
    errFlag = pm_mpp_configure_type(PM_MPP_DIG_OUT, (uint8)perph_index);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    // Configure the VOLTAGE_SEL 
    errFlag = pm_mpp_config_dvin(level, (uint8)perph_index);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    // Configure output source
    //Source select:
    //When configured as a digital output Source select:
    //0000 = 0
    //0001 = 1
    //0010 = paired MPP
    //0011 = inverted paired MPP
    errFlag = pm_comm_write_byte_mask(mpp_ptr->comm_ptr->slave_id, mode_ctl, 0x03,(pm_register_data_type)output_ctrl, 0);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    // Enable the MPP
    errFlag = pm_mpp_enable(pmic_chip, mpp, TRUE);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    return errFlag;
}

pm_err_flag_type pm_mpp_config_digital_inout(unsigned pmic_chip, pm_mpp_perph_index mpp, pm_mpp_dlogic_lvl_type  level, pm_mpp_dlogic_inout_pup_type  pull_up)
{
    /* Use this variable to keep track if the operation was successful or not */
    pm_err_flag_type         errFlag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type dig_pull_ctl;
    pm_mpp_data_type *mpp_ptr = pm_mpp_get_data(pmic_chip);
    pm_register_address_type perph_index = (pm_register_address_type)mpp;

	if (mpp_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ((uint8)perph_index >= mpp_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__INVALID_MPP_INDEXED;
    }
    if (level >= PM_MPP__DLOGIC__LVL_INVALID )
    {
        /* Parm 1 is out of range */
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
    if (pull_up >= PM_MPP__DLOGIC_INOUT__PUP_INVALID)
    {
        /* Parm 2 is out of range */
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
	
	dig_pull_ctl = mpp_ptr->mpp_reg_table->base_address + (mpp_ptr->mpp_reg_table->peripheral_offset*perph_index) + mpp_ptr->mpp_reg_table->dig_pull_ctl;
	
    /* Check whether this config will enable or disable ABUS pulldown */ 
    errFlag = pm_mpp_abus_pulldown((uint8)level, PM_MPP_BI_DIR, (uint8)perph_index);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    // Set MPP_TYPE to PM_MPP_BI_DIR
    errFlag = pm_mpp_configure_type(PM_MPP_BI_DIR, (uint8)perph_index);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    // Configure the VOLTAGE_SEL 
    errFlag = pm_mpp_config_dvin(level, (uint8)perph_index);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    // Config the Pullup Resistor Control
    errFlag = pm_comm_write_byte_mask(mpp_ptr->comm_ptr->slave_id, dig_pull_ctl, 0x07,(pm_register_data_type)pull_up, 0);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    // Enable the MPP
    errFlag = pm_mpp_enable(pmic_chip, mpp, TRUE);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    return errFlag;
}

pm_err_flag_type pm_mpp_config_analog_input(unsigned pmic_chip, pm_mpp_perph_index mpp, pm_mpp_ain_ch_type  ch_select)
{
    /* Use this variable to keep track if the operation was successful or not */
    pm_err_flag_type       errFlag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type ana_in_ctl;
    pm_register_address_type mode_ctl;
    pm_mpp_data_type *mpp_ptr = pm_mpp_get_data(pmic_chip);
    pm_register_address_type perph_index = (pm_register_address_type)mpp;

	if (mpp_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ((uint8)perph_index >= mpp_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__INVALID_MPP_INDEXED;
    }
    if (ch_select >= PM_MPP__AIN__CH_INVALID)
    {
        /* Parm 1 is out of range */
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
	
	ana_in_ctl = mpp_ptr->mpp_reg_table->base_address + (mpp_ptr->mpp_reg_table->peripheral_offset*perph_index) + mpp_ptr->mpp_reg_table->ana_in_ctl;
	mode_ctl   = mpp_ptr->mpp_reg_table->base_address + (mpp_ptr->mpp_reg_table->peripheral_offset*perph_index) + mpp_ptr->mpp_reg_table->mode_ctl;
			  
    /* Check whether this config will enable or disable ABUS pulldown */ 
    errFlag = pm_mpp_abus_pulldown((uint8)ch_select, PM_MPP_ANALOG_IN, (uint8)perph_index);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    // Set MPP_TYPE to PM_MPP_ANALOG_IN
    errFlag = pm_mpp_configure_type(PM_MPP_ANALOG_IN, (uint8)perph_index);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    // Configure the channel control
    //AMUX Channel Control
    //0: Route to AMUX5
    //1: Route to AMUX6
    //2: Route to AMUX7
    //3: Route to AMUX8
    //4: Route to ATEST1 (aka ABUS1) (internal use only)
    //5: Route to ATEST2 (aka ABUS2) (internal use only)
    //6: Route to ATEST3 (aka ABUS3) (internal use only)
    //7: Route to ATEST4 (aka ABUS4) (internal use only)

    errFlag = pm_comm_write_byte_mask(mpp_ptr->comm_ptr->slave_id, ana_in_ctl, 0x07,(pm_register_data_type)ch_select, 0);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    // Note: When the MPP is configured as AIN, AOUT, or Current Sink, the LSB in mode_ctl needs to be set to keep the MPP enabled; otherwise, the MPP
    // will alwways be disabled.
    errFlag = pm_comm_write_byte_mask(mpp_ptr->comm_ptr->slave_id, mode_ctl, 0x01, 0x01, 0);

    // Enable the MPP
    errFlag = pm_mpp_enable(pmic_chip, mpp, TRUE);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    return errFlag;
}


pm_err_flag_type pm_mpp_config_analog_output(unsigned pmic_chip, pm_mpp_perph_index mpp, pm_mpp_aout_level_type  level, pm_mpp_aout_switch_type  OnOff)
{
    /* Use this variable to keep track if the operation was successful or not */
    pm_err_flag_type       errFlag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type mode_ctl = 0;
    pm_register_address_type ana_out_ctl;
    pm_mpp_data_type *mpp_ptr = pm_mpp_get_data(pmic_chip);
    pm_register_address_type perph_index = (pm_register_address_type)mpp;

	if (mpp_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ((uint8)perph_index >= mpp_ptr->num_of_peripherals)
    {
        return PM_ERR_FLAG__INVALID_MPP_INDEXED;
    }
    if (level != PM_MPP__AOUT__LEVEL_VREF_1p25_Volts)
    {
        /* Parm 1 is out of range */
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
    if (OnOff>= PM_MPP__AOUT__SWITCH_INVALID)
    {
        /* Parm 2 is out of range */
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
	
	mode_ctl    = mpp_ptr->mpp_reg_table->base_address + (mpp_ptr->mpp_reg_table->peripheral_offset*perph_index) + mpp_ptr->mpp_reg_table->mode_ctl;
	ana_out_ctl = mpp_ptr->mpp_reg_table->base_address + (mpp_ptr->mpp_reg_table->peripheral_offset*perph_index) + mpp_ptr->mpp_reg_table->ana_out_ctl;
			  
    /* Check whether this config will enable or disable ABUS pulldown */ 
    errFlag = pm_mpp_abus_pulldown((uint8)level, PM_MPP_ANALOG_OUT, (uint8)perph_index);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    // Set MPP_TYPE to PM_MPP_ANALOG_OUT
    errFlag = pm_mpp_configure_type(PM_MPP_ANALOG_OUT, (uint8)perph_index);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    // Config Analog Output Control
    //Analog Output Control
    //0: Output = vref_1V25 = REF_BYP pin, typically 1.25 Volts
    //1: Output = vref_V625 = 0.625 Volts (internal use only)
    //2: Output = vref_V3125 = 0.3125 Volts (internal use only)
    //3: Output = paired MPP input (internal use only)
    //4: Output = buffered ATEST1 (aka ABUS1) (internal use only)
    //5: Output = buffered ATEST2 (aka ABUS2) (internal use only)
    //6: Output = buffered ATEST3 (aka ABUS3) (internal use only)
    //7: Output = buffered ATEST4 (aka ABUS4) (internal use only)
    errFlag = pm_comm_write_byte_mask(mpp_ptr->comm_ptr->slave_id, ana_out_ctl, 0x07, (pm_register_data_type)level, 0);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    // Config analog output
    //Enable control when configured as Bidirectional, AIN, AOUT, or Current Sink.  MPP is enable whenever the selected condition is true.
    //0000 = 0 (mpp is always disabled)
    //0001 = 1 (mpp is always Enabled)
    //0010 = paired MPP
    //0011 = inverted paired MPP
    errFlag = pm_comm_write_byte_mask(mpp_ptr->comm_ptr->slave_id, mode_ctl, 0x03, (pm_register_data_type)OnOff, 0);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    // Enable the MPP
    errFlag = pm_mpp_enable(pmic_chip, mpp, TRUE);

    return errFlag;
}

pm_err_flag_type pm_mpp_config_i_sink(unsigned pmic_chip, pm_mpp_perph_index mpp, pm_mpp_i_sink_level_type  level, pm_mpp_i_sink_switch_type  OnOff)
{
    /* Use this variable to keep track if the operation was successful or not */
    pm_err_flag_type       errFlag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type sink_ctl;
    pm_register_address_type mode_ctl;
    pm_mpp_data_type *mpp_ptr = pm_mpp_get_data(pmic_chip);
    pm_register_address_type perph_index = (pm_register_address_type)mpp;

	if (mpp_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ((uint8)perph_index >= mpp_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__INVALID_MPP_INDEXED;
    }
    if (level >= PM_MPP__I_SINK__LEVEL_INVALID)
    {
        /* Parm 1 is out of range */
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }
    if (OnOff >= PM_MPP__I_SINK__SWITCH_INVALID)
    {
        /* Parm 2 is out of range */
        return PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
    }
	
	sink_ctl = mpp_ptr->mpp_reg_table->base_address + (mpp_ptr->mpp_reg_table->peripheral_offset*(uint8)perph_index) + mpp_ptr->mpp_reg_table->sink_ctl;
	mode_ctl = mpp_ptr->mpp_reg_table->base_address + (mpp_ptr->mpp_reg_table->peripheral_offset*(uint8)perph_index) + mpp_ptr->mpp_reg_table->mode_ctl;
	
    /* Check whether this config will enable or disable ABUS pulldown */ 
    errFlag = pm_mpp_abus_pulldown((uint8)level, PM_MPP_I_SINK, (uint8)perph_index);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    // Set MPP_TYPE to PM_MPP_I_SINK
    errFlag = pm_mpp_configure_type(PM_MPP_I_SINK, (uint8)perph_index);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    // Config Current Sink Output Control
    //Current Sink Output Control
    //0: Output = 5 mA
    //1: Output = 10 mA
    //2: Output = 15 mA
    //3: Output = 20 mA
    //4: Output = 25 mA
    //5: Output = 30 mA
    //6: Output = 35 mA
    //7: Output = 40 mA
    errFlag = pm_comm_write_byte_mask(mpp_ptr->comm_ptr->slave_id, sink_ctl, 0x07, (pm_register_data_type)level, 0);    
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    // Config I_SINK output
    //Enable control when configured as Bidirectional, AIN, AOUT, or Current Sink.  MPP is enable whenever the selected condition is true.
    //0000 = 0 (mpp is always disabled)
    //0001 = 1 (mpp is always Enabled)
    //0010 = paired MPP
    //0011 = inverted paired MPP
    errFlag = pm_comm_write_byte_mask(mpp_ptr->comm_ptr->slave_id, mode_ctl, 0x03, (pm_register_data_type)OnOff, 0);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    // Enable the MPP
    errFlag = pm_mpp_enable(pmic_chip, mpp, TRUE);

    return errFlag;
}


pm_err_flag_type pm_mpp_config_atest(unsigned pmic_chip, pm_mpp_perph_index mpp, pm_mpp_ch_atest_type  atest_select)
{
    /* Use this variable to keep track if the operation was successful or not */
    pm_err_flag_type    errFlag = PM_ERR_FLAG__SUCCESS;
    pm_register_address_type ana_in_ctl;
    pm_register_address_type mode_ctl;
    pm_mpp_data_type *mpp_ptr = pm_mpp_get_data(pmic_chip);
    pm_register_address_type perph_index = (pm_register_address_type)mpp;
	
	if (mpp_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if ((uint8)perph_index >= mpp_ptr->num_of_peripherals)
    {
        return PM_ERR_FLAG__INVALID_MPP_INDEXED;
    }
    if (atest_select >= PM_MPP__CH_ATEST_INVALID)
    {
        /* Parm 1 is out of range */
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }

	ana_in_ctl = mpp_ptr->mpp_reg_table->base_address + (mpp_ptr->mpp_reg_table->peripheral_offset*perph_index) + mpp_ptr->mpp_reg_table->ana_in_ctl;
	mode_ctl   = mpp_ptr->mpp_reg_table->base_address + (mpp_ptr->mpp_reg_table->peripheral_offset*perph_index) + mpp_ptr->mpp_reg_table->mode_ctl;	
			  
    // Set MPP_TYPE to PM_MPP_ANALOG_IN
    errFlag = pm_mpp_configure_type(PM_MPP_ANALOG_IN, (uint8)perph_index);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    // Configure teh channel control
    //AMUX Channel Control
    //4: Route to ATEST1 (aka ABUS1) (internal use only)
    //5: Route to ATEST2 (aka ABUS2) (internal use only)
    //6: Route to ATEST3 (aka ABUS3) (internal use only)
    //7: Route to ATEST4 (aka ABUS4) (internal use only)
    errFlag = pm_comm_write_byte_mask(mpp_ptr->comm_ptr->slave_id, ana_in_ctl, 0x07,(pm_register_data_type)(atest_select+4), 0);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    // Note: When the MPP is configured as AIN, AOUT, or Current Sink, the LSB in mode_ctl needs to be set to keep the MPP enabled; otherwise, the MPP
    // will alwways be disabled.
    errFlag = pm_comm_write_byte_mask(mpp_ptr->comm_ptr->slave_id, mode_ctl, 0x01, 0x01, 0);

    // Enable the MPP
    errFlag = pm_mpp_enable(pmic_chip, mpp, TRUE);
    if(errFlag != PM_ERR_FLAG__SUCCESS) return errFlag ;

    return errFlag;
}

pm_err_flag_type pm_mpp_config_dtest_output(unsigned pmic_chip, pm_mpp_perph_index mpp, pm_mpp_dlogic_lvl_type  level, pm_mpp_dlogic_out_dbus_type  dbus)
{
    (void)level;
    (void)dbus;
    (void)mpp;

    //DTESTOUT is not supported on PM8941 anymore. Need to verify for 8084
    return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
}


pm_err_flag_type pm_mpp_set_list_mpp_with_shunt_cap(unsigned pmic_chip, pm_mpp_perph_index mpp)
{
	pm_mpp_data_type *mpp_ptr = pm_mpp_get_data(pmic_chip);
    pm_register_address_type perph_index = (pm_register_address_type)mpp;
	
	CORE_VERIFY(mpp_ptr);
    
    if ((uint8)perph_index >= mpp_ptr->num_of_peripherals)
    {
        return PM_ERR_FLAG__INVALID_MPP_INDEXED;
    }

    (mpp_ptr)->m_mpp_with_shunt_cap_list_status |= (1L << (uint8)perph_index); 

    return PM_ERR_FLAG__SUCCESS;
}



pm_err_flag_type pm_mpp_get_mpp_with_shunt_cap_list_status_for_device(unsigned pmic_chip, pm_mpp_perph_index mpp, uint32  *shuntList)
{
	pm_mpp_data_type *mpp_ptr = pm_mpp_get_data(pmic_chip);
    pm_register_address_type perph_index = (pm_register_address_type)mpp;
	
	CORE_VERIFY(mpp_ptr);

    if ((uint8)perph_index >= mpp_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__INVALID_MPP_INDEXED;
    }
    if (NULL == shuntList )
    {
        /* Param is out of range */
        return PM_ERR_FLAG__INVALID_POINTER;
    }

    *shuntList = (mpp_ptr)->m_mpp_with_shunt_cap_list_status;
    return PM_ERR_FLAG__SUCCESS;
}



pm_err_flag_type pm_mpp_status_get(unsigned pmic_chip, pm_mpp_perph_index mpp, pm_mpp_status_type  *mpp_status)
{
	pm_register_data_type	 data		  = 0;
	pm_err_flag_type		 errFlag	  = PM_ERR_FLAG__SUCCESS;   
	uint8					 index		  = 0;
	
    pm_register_address_type base_address;
    pm_register_address_type mode_ctl;
    pm_register_address_type dig_pull_ctl;
    pm_register_address_type dig_in_ctl;
    pm_register_address_type dig_vin_ctl;
    pm_register_address_type sink_ctl;
    pm_register_address_type ana_in_ctl;
    pm_register_address_type ana_out_ctl;

	pm_mpp_data_type *mpp_ptr = pm_mpp_get_data(pmic_chip);
    pm_register_address_type perph_index = (pm_register_address_type)mpp;

	if (mpp_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (NULL == mpp_status){
        return PM_ERR_FLAG__INVALID_POINTER;
    }
	if ((uint8)perph_index >= mpp_ptr->num_of_peripherals){
        return PM_ERR_FLAG__INVALID_MPP_INDEXED;
    }
	
	base_address = mpp_ptr->mpp_reg_table->base_address + (mpp_ptr->mpp_reg_table->peripheral_offset*perph_index);
    mode_ctl	  =	base_address + mpp_ptr->mpp_reg_table->mode_ctl;
    dig_pull_ctl =	base_address + mpp_ptr->mpp_reg_table->dig_pull_ctl;
    dig_in_ctl	  =	base_address + mpp_ptr->mpp_reg_table->dig_in_ctl;
    dig_vin_ctl  =	base_address + mpp_ptr->mpp_reg_table->dig_vin_ctl;
    sink_ctl     =	base_address + mpp_ptr->mpp_reg_table->sink_ctl;
    ana_in_ctl   =	base_address + mpp_ptr->mpp_reg_table->ana_in_ctl;
    ana_out_ctl  =	base_address + mpp_ptr->mpp_reg_table->ana_out_ctl;

	//Determine configuration of MPP
    errFlag = pm_comm_read_byte_mask(mpp_ptr->comm_ptr->slave_id, mode_ctl, 0x70, &data,0);
	
	if(PM_ERR_FLAG__SUCCESS == errFlag){
		mpp_status->mpp_config = (pm_mpp_config_type)(data>>4);
	
		//Based on configuration fill out appropriate struct
		switch(mpp_status->mpp_config){
			case PM_MPP_DIG_IN:
				
				errFlag = pm_comm_read_byte_mask(mpp_ptr->comm_ptr->slave_id, dig_vin_ctl, 0x07, &data,0);
				if(PM_ERR_FLAG__SUCCESS == errFlag)
					mpp_status->mpp_config_settings.mpp_digital_input_status.mpp_dlogic_lvl = (pm_mpp_dlogic_lvl_type)data;
				
				errFlag = pm_comm_read_byte_mask(mpp_ptr->comm_ptr->slave_id, dig_in_ctl, 0xF, &data,0);
				if(PM_ERR_FLAG__SUCCESS == errFlag){
					
					//Check which bit is set to determine DBUS index, break if index goes beyond number of DBUS lines
					while(data >>= 1){
						if(index >= (mpp_ptr)->mpp_specific->num_of_dbus)
							break;
						index++;
					}

					mpp_status->mpp_config_settings.mpp_digital_input_status.mpp_dlogic_in_dbus = (pm_mpp_dlogic_in_dbus_type)index;
				}
				break;
			case PM_MPP_DIG_OUT:
				
				errFlag = pm_comm_read_byte_mask(mpp_ptr->comm_ptr->slave_id, dig_vin_ctl, 0x07, &data,0);
				if(PM_ERR_FLAG__SUCCESS == errFlag)
					mpp_status->mpp_config_settings.mpp_digital_output_status.mpp_dlogic_lvl = (pm_mpp_dlogic_lvl_type) data;
			    
				errFlag = pm_comm_read_byte_mask(mpp_ptr->comm_ptr->slave_id, mode_ctl, 0x03, &data,0);
				if(PM_ERR_FLAG__SUCCESS == errFlag)
					mpp_status->mpp_config_settings.mpp_digital_output_status.mpp_dlogic_out_ctrl = (pm_mpp_dlogic_out_ctrl_type)data;
			
				break;
			case PM_MPP_BI_DIR:
				
				errFlag = pm_comm_read_byte_mask(mpp_ptr->comm_ptr->slave_id, dig_pull_ctl, 0x07, &data,0);
				if(PM_ERR_FLAG__SUCCESS == errFlag)
					mpp_status->mpp_config_settings.mpp_bidirectional_status.mpp_dlogic_inout_pup = (pm_mpp_dlogic_inout_pup_type)data;
				
				errFlag = pm_comm_read_byte_mask(mpp_ptr->comm_ptr->slave_id, dig_vin_ctl, 0x07, &data,0);
				if(PM_ERR_FLAG__SUCCESS == errFlag)
					mpp_status->mpp_config_settings.mpp_bidirectional_status.mpp_dlogic_lvl = (pm_mpp_dlogic_lvl_type)data;
				
				break;
			case PM_MPP_ANALOG_IN:	
				
				errFlag = pm_comm_read_byte_mask(mpp_ptr->comm_ptr->slave_id, ana_in_ctl, 0x07, &data,0);
				if(PM_ERR_FLAG__SUCCESS == errFlag)
					mpp_status->mpp_config_settings.mpp_analog_input_status.mpp_ain_ch = (pm_mpp_ain_ch_type)data;
				
				break;
			case PM_MPP_ANALOG_OUT:
				
				errFlag = pm_comm_read_byte_mask(mpp_ptr->comm_ptr->slave_id, ana_out_ctl, 0x07, &data,0);
				if(PM_ERR_FLAG__SUCCESS == errFlag)
					mpp_status->mpp_config_settings.mpp_analog_output_status.mpp_aout_level = (pm_mpp_aout_level_type)data;

				errFlag = pm_comm_read_byte_mask(mpp_ptr->comm_ptr->slave_id, mode_ctl, 0x03, &data,0);
				if(PM_ERR_FLAG__SUCCESS == errFlag)
					mpp_status->mpp_config_settings.mpp_analog_output_status.mpp_aout_switch = (pm_mpp_aout_switch_type)data;
				
				break;
			case PM_MPP_I_SINK:
				
				errFlag = pm_comm_read_byte_mask(mpp_ptr->comm_ptr->slave_id, sink_ctl, 0x07, &data,0);
				if(PM_ERR_FLAG__SUCCESS == errFlag)
					mpp_status->mpp_config_settings.mpp_current_sink_status.mpp_i_sink_level = (pm_mpp_i_sink_level_type)data;

				errFlag = pm_comm_read_byte_mask(mpp_ptr->comm_ptr->slave_id, mode_ctl, 0x03, &data,0);
				if(PM_ERR_FLAG__SUCCESS == errFlag)
					mpp_status->mpp_config_settings.mpp_current_sink_status.mpp_i_sink_switch = (pm_mpp_i_sink_switch_type)data;
				
				break;
			case PM_MPP_TYPE_INVALID:
				errFlag = PM_ERR_FLAG__INVALID;
				break;
			default:
				errFlag = PM_ERR_FLAG__INVALID;
				break;
		}
	}

    return errFlag;
}


pm_err_flag_type pm_mpp_enable(unsigned pmic_chip, pm_mpp_perph_index mpp, boolean enable)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;   
    pm_register_data_type data = 0;
    pm_register_address_type en_ctl;
    pm_mpp_data_type *mpp_ptr = pm_mpp_get_data(pmic_chip);
    pm_register_address_type perph_index = (pm_register_address_type)mpp;


	if (mpp_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
	
    en_ctl = mpp_ptr->mpp_reg_table->base_address + (mpp_ptr->mpp_reg_table->peripheral_offset*perph_index) + mpp_ptr->mpp_reg_table->en_ctl;
			  
    // MPP_EN	0	"MPP enable
    //              0 = puts MPP_PAD at high Z and disables the block
    //              1 = MPP is enabled"
    if(TRUE == enable)
    {
        data = 0x80;
    }
    errFlag = pm_comm_write_byte_mask(mpp_ptr->comm_ptr->slave_id, en_ctl , 0x80, data, 0);

    return errFlag;
}


/*===========================================================================

                     INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
pm_err_flag_type pm_mpp_is_dbus_valid(pm_mpp_dlogic_in_dbus_type  dbus, uint8  perph_index)
{
    pm_err_flag_type    errFlag   = PM_ERR_FLAG__SUCCESS;
    uint8               index     = 0;
	pm_mpp_data_type *mpp_ptr = pm_mpp_get_data(0);
	
	if (mpp_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
    if (perph_index >= mpp_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__INVALID_MPP_INDEXED;
    }

    switch (dbus)
    {
    //case PM_MPP__DLOGIC_IN__DBUS_NONE:
    //    /* Only the MPP that reserved the DBUS/DTEST can release it.*/
    //    for (index = 0; index < mpp_ptr->mpp_specific->num_of_dbus; index++)
    //    {
    //        if (mpp_ptr->m_dbus_busy[index] == perph_index)
    //        {
    //            mpp_ptr->m_dbus_busy[index] = mpp_ptr->num_of_peripherals;
    //        }
    //    }
    //    break;

    case PM_MPP__DLOGIC_IN__DBUS1:
    case PM_MPP__DLOGIC_IN__DBUS2:
    case PM_MPP__DLOGIC_IN__DBUS3:                                         
    case PM_MPP__DLOGIC_IN__DBUS4:                                         
        /* Is the DBUS/DTEST already in use by another MPP?*/
        if ((mpp_ptr->m_dbus_busy[dbus] == mpp_ptr->num_of_peripherals) ||
            (mpp_ptr->m_dbus_busy[dbus] == perph_index))
        {
            /* Before we reserve the DBUS/DTEST, check if the MPP 
            has another DBUS/DTEST reserved.*/  
            for (index = 0; index < mpp_ptr->mpp_specific->num_of_dbus; index++)
            {
                if (mpp_ptr->m_dbus_busy[index] == perph_index)
                {
                    /* the MPP has a DBUS/DTEST already reserved */
                    break;
                }
            }

            /* Assign the bus to the MPP. */
            mpp_ptr->m_dbus_busy[dbus] = perph_index;

            /* If the MPP has another DBUS/DTEST reserved then free it */
            if ((index != mpp_ptr->mpp_specific->num_of_dbus) && (index != dbus))
            {
                mpp_ptr->m_dbus_busy[index] = mpp_ptr->num_of_peripherals;
            }
        }
        else
        {
            /* The DBUS/DTEST is busy, return an ERROR. */
            errFlag = PM_ERR_FLAG__DBUS_IS_BUSY_MODE;
        }

        break;
    default:

        /* The user passed an invalid data bus selection. */
        errFlag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }

    return errFlag;
}

pm_err_flag_type pm_mpp_abus_pulldown(uint8  which_ch, pm_drv_mpp_config_type  mpp_type, uint8  perph_index)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;   
	pm_mpp_data_type *mpp_ptr = pm_mpp_get_data(0);
	
    if (perph_index >= mpp_ptr->num_of_peripherals )
    {
        return PM_ERR_FLAG__INVALID_MPP_INDEXED;
    }

    /* If the config type is Analog Input and the MPP is routed to one of
    * the ABUS lines, then check further to see if it can really use it 
    */ 

    if(PM_MPP_ANALOG_IN == mpp_type)
    {
        if (which_ch >= (uint8)PM_MPP__AIN__CH_ABUS1 &&
            which_ch <= (uint8)PM_MPP__AIN__CH_ABUS4)
        {
            /* if m_abus_in_use indicates the ABUS line in question is being
            * used but this MPP is not using it, then someone else is using
            * it. The MPP can't have it since it's considered to be busy
            */ 
            if ( (mpp_ptr->m_abus_in_use & (1 << which_ch)) && 
                mpp_ptr->m_mpp_using_abus[perph_index] != which_ch )
            {
                errFlag = PM_ERR_FLAG__ABUS_IS_BUSY_MODE;
            }
            else
            {
                /* Before setting any bit in m_abus_in_use, check whether any of
                * the ABUS lines are already in use. If true, don't need to 
                * set bit 7 of register 0x7F since it should have been set already
                */
                // in MISC module
                if (!mpp_ptr->m_abus_in_use) (void)pm_comm_write_byte_mask(mpp_ptr->comm_ptr->slave_id, mpp_ptr->mpp_specific->pm_drv_test_acc_rw, 0x80, 0x80, 0);
                /* If this MPP is already using an ABUS line which might be
                * different from the one being configured now, clear it.
                * That means the MPP will be re-routed to a different ABUS line
                */
                if (mpp_ptr->m_mpp_using_abus[perph_index])
                {
                    mpp_ptr->m_abus_in_use &= ~(uint32)(1 << mpp_ptr->m_mpp_using_abus[perph_index]);
                }
                /* Set the global flags to indicate the ABUS line is now 
                * being used by this MPP
                */
                mpp_ptr->m_mpp_using_abus[perph_index] = which_ch;
                mpp_ptr->m_abus_in_use |= (1 << which_ch);
            }
        }
    }
    else
    {
        /* The MPP is being configured to a type that won't use the ABUS
        * line. Check if the MPP has been using any ABUS line. If so,
        * clear it.
        */ 
        if (mpp_ptr->m_mpp_using_abus[perph_index])
        {
            mpp_ptr->m_abus_in_use &= ~(uint32)(1 << mpp_ptr->m_mpp_using_abus[perph_index]);
            mpp_ptr->m_mpp_using_abus[perph_index] = 0;
            /* Clear bit 7 of register 0x7F if none of the ABUS lines are
            * being used
            */
            //in MISC module
            if (!mpp_ptr->m_abus_in_use) (void)pm_comm_write_byte_mask(mpp_ptr->comm_ptr->slave_id, mpp_ptr->mpp_specific->pm_drv_test_acc_rw, 0x80, 0x00, 0);
        }
    }

    return errFlag;
}

pm_err_flag_type pm_mpp_configure_type(pm_drv_mpp_config_type type, uint8  perph_index)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;   
	pm_mpp_data_type *mpp_ptr = pm_mpp_get_data(0);
    pm_register_address_type mode_ctl;


	if (mpp_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
		
    mode_ctl = mpp_ptr->mpp_reg_table->base_address + (pm_register_address_type)(mpp_ptr->mpp_reg_table->peripheral_offset*perph_index) + mpp_ptr->mpp_reg_table->mode_ctl;
			  
    // Set MPP_TYPE to DIGITAL INPUT
    //MPP Type:
    //0: Digital Input
    //1: Digital Output
    //2: Digital Input and Digital Output
    //3: Bidirectional Logic
    //4: Analog Input
    //5: Analog Output
    //6: Current Sink
    //7: Reserved
    errFlag = pm_comm_write_byte_mask(mpp_ptr->comm_ptr->slave_id, mode_ctl , 0x70, (pm_register_data_type)(type<<4), 0);

    return errFlag;
}





pm_err_flag_type pm_mpp_config_dvin(pm_mpp_dlogic_lvl_type  level, uint8  perph_index)
{
    pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;   
	pm_mpp_data_type *mpp_ptr = pm_mpp_get_data(0);
    pm_register_address_type dig_vin_ctl;
	
	if (mpp_ptr == NULL)
    {
        return PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    }
	
    dig_vin_ctl = mpp_ptr->mpp_reg_table->base_address + (pm_register_address_type)(mpp_ptr->mpp_reg_table->peripheral_offset*perph_index) + mpp_ptr->mpp_reg_table->dig_vin_ctl;
			  
    // Configure the VOLTAGE_SEL 
    // "Select Voltage source:
    //   000 = VIN0 (refer to the objective spec.)
    //   001 = VIN1 (refer to the objective spec.)
    //   010 = VIN2 (refer to the objective spec.)*
    //   011 = VIN3 (refer to the objective spec.)
    //   100 = VIN4 (refer to the objective spec.)*
    //   101 = VIN5 (refer to the objective spec.)
    //   110 = VIN6 (refer to the objective spec.)
    //   111 = VIN7 (refer to the objective spec.)

    errFlag = pm_comm_write_byte_mask(mpp_ptr->comm_ptr->slave_id, dig_vin_ctl, 0x07, (pm_register_data_type)level, 0);

    return errFlag;
}
