/*! \file 
*  \n
*  \brief  pm_coinchg.h ---- PMIC clock driver 
*  \n
*  \n This header file contains enums and API definitions for PMIC coin
*  cell charger driver.
*  \n
*  \n &copy; Copyright 2009 - 2012 Qualcomm Technologies Incorporated, All Rights Reserved
*/
/* ======================================================================= */

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pm_coinchg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
02/08/11   hw      Added support for 2.5V coin cell charging enum
10/20/09   jtn     Move init function prototype to pm_device_init_i.h
06/30/09   jtn     Updated file documentation
=============================================================================*/
#ifndef PM_COINCHG_H

#define PM_COINCHG_H

#include "pm_lib_err.h"
/**
* Coin Cell Charger Voltage Settings.
*/
typedef enum
{
    /**
    * Set to 3.0 volts
    */
    PM_COIN_CELL_VSET_3p0V,
    /**
    * Set to 3.1 volts
    */
    PM_COIN_CELL_VSET_3p1V,
    /**
    * Set to 3.2 volts
    */
    PM_COIN_CELL_VSET_3p2V,
    /**
    * Set to 2.5 volts
    */
    PM_COIN_CELL_VSET_2p5V,
    /**
    * Invalid selection, used for PMIC API Range checking
    */
    PM_COIN_CELL_VSET_INVALID
}pm_coin_cell_vset_type;

/**
* Coin Cell Charger Current Limiting Resistor Settings.
*/
typedef enum
{
    /**
    * Set to 2100 ohms
    */
    PM_COIN_CELL_RSET_2100_OHMS,
    /**
    * Set to 1700 ohms
    */
    PM_COIN_CELL_RSET_1700_OHMS,
    /**
    * Set to 1200 ohms
    */
    PM_COIN_CELL_RSET_1200_OHMS,
    /**
    * Set to 800 ohms
    */
    PM_COIN_CELL_RSET_800_OHMS,
    /**
    * Invalid selection, used for range checking in PMIC API
    */
    PM_COIN_CELL_RSET_INVALID
}pm_coin_cell_rset_type;


/**
* 
* 
* @brief Driver function pm_coin_cell_chg_switch
* @detailed This function turns ON/OFF charging of the coin cell.
* @param pmic_device_index Selects the device in which the coin cell charger being controlled are located. Device index starts with zero
* @param cmd    PM_ON_CMD  = Enables charging of the coin cell.  By default
*               (without calling pm_coin_cell_chg_config()),
*               turning on the coin cell charger will configure
*               the charger voltage to 3.2 Volts and the
*               current resistor setting to 2.1K Ohms.
*               
*               PM_OFF_CMD = Disables charging of the coin cell. (default)
* 
* @return PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = The input
*         parameter is invalid.
*         
*         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not
*         available on this version of the PMIC.
*         
*         PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver
*         failed to communicate with the PMIC.
*         
*         PM_ERR_FLAG__SUCCESS               = Operation was
*         successful.
* 
* example //Configure the coin cell charger voltage to 3.1 Volts and
* //Current Limiting resistor setting to 1.2K Ohms.
*      err = pm_coin_cell_chg_config(PM_COIN_CELL_VSET_3p1V,
*                                    PM_COIN_CELL_RSET_1200_OHMS);
*      // Enable coin cell charging.
*      err = pm_coin_cell_chg_switch(PM_ON_CMD);
*/
extern pm_err_flag_type pm_dev_coin_cell_chg_switch(unsigned pmic_device_index, pm_switch_cmd_type cmd);
//#pragma message("Do not use pm_coin_cell_chg_switch(), which is deprecated. Use pm_dev_coin_cell_chg_switch() instead.")
extern pm_err_flag_type pm_coin_cell_chg_switch(pm_switch_cmd_type cmd);

/**
* @brief function pm_coin_cell_chg_config
* 
* @detailed This function configures the coin cell charger voltage and current
*   limiting resistor value.
* @param pmic_device_index Selects the device in which the coin cell charger being controlled are located. Device index starts with zero
*
* @param vset   Configures the coin cell charger voltage
*               
*               PM_COIN_CELL_VSET_3p0V  = 3.0 Volts.
*               PM_COIN_CELL_VSET_3p1V  = 3.1 Volts.
*               PM_COIN_CELL_VSET_3p2V  = 3.2 Volts. (Default)
* 
* @param rset   Configures the coin cell charger current limiting resistor setting
*               
*               PM_COIN_CELL_RSET_2100_OHMS  = 2.1K Ohms. (Default)
*               PM_COIN_CELL_RSET_1700_OHMS  = 1.7K Ohms.
*               PM_COIN_CELL_RSET_1200_OHMS  = 1.2K Ohms.
*               PM_COIN_CELL_RSET_800_OHMS   = 800  Ohms.
* 
* @return      PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = The input parameter 1 is invalid.
*         
*         PM_ERR_FLAG__PAR2_OUT_OF_RANGE     = The input parameter 2 is invalid.
*         
*         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
*         version of the PMIC.
*         
*         PM_ERR_FLAG__SBI_OPT_ERR           = The SBI driver failed to
*         communicate with the PMIC.
*         
*         PM_ERR_FLAG__SUCCESS               = Operation was successful.
* 
* Example      // Configure the coin cell charger voltage to 3.1 Volts and
*      // Current Limiting resistor setting to 1.2K Ohms.
*      err = pm_coin_cell_chg_config(PM_COIN_CELL_VSET_3p1V,
*                                    PM_COIN_CELL_RSET_1200_OHMS);
*      // Enable coin cell charging.
*      pm_coin_cell_chg_switch(PM_ON_CMD);
*/
extern pm_err_flag_type pm_dev_coin_cell_chg_config(unsigned pmic_device_index, 
                                                    pm_coin_cell_vset_type vset,
                                                    pm_coin_cell_rset_type rset);
//#pragma message("Do not use pm_coin_cell_chg_config(), which is deprecated. Use pm_dev_coin_cell_chg_config() instead.")
extern pm_err_flag_type pm_coin_cell_chg_config(pm_coin_cell_vset_type vset,
                                                pm_coin_cell_rset_type rset);


#endif
