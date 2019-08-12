#ifndef PM_APP_CHG__H
#define PM_APP_CHG__H

/*! \file pm_app_chg_alg.h
 *  \n
 *  \brief  PMIC sbl charger algorithms
 *  \details  This file contains functions and variable declarations to support
 *   the PMIC SBL charging
 *
 *
 *  \n &copy; Copyright (c) 2012-2014  by QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================
                                Edit History
This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

 
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/03/14   pxm     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_smbb.h"
#include "pm_lbc.h"
#include "pm_pon.h"
#include "pm_rgb.h"
#include "pm_target_information.h" 

#include "qhsusb_al_charger.h"
#include "HALhwio.h"
#include "DALSys.h"      
#include "AdcBoot.h"
#include "boot_api.h"


/*=========================================================================== 
 
                     GLOBAL DEFINES
 
===========================================================================*/



/*=========================================================================== 
 
                     GLOBAL TYPE DEFINITIONS
 
===========================================================================*/
typedef enum
{
    PM_CHG_ENTRY_STATE,
    PM_CHG_BATTERY_AND_DEBUG_BOARD_DETECT_STATE,
    PM_CHG_CHARGER_TYPE_DETECT_STATE,
    PM_CHG_USB_CHARGER_PORT_DETECT_STATE,
    PM_CHG_DC_CHARGING_STATE,
    PM_CHG_WIRELESS_CHARGING_STATE,
    PM_CHG_CDP_CHARGING_STATE,
    PM_CHG_DCP_CHARGING_STATE,
    PM_CHG_FLOAT_CHARGING_STATE,
    PM_CHG_SDP_CHARGING_STATE,
    PM_CHG_BOOTUP_STATE,
    PM_CHG_SHUTDOWN_STATE,
    PM_CHG_INVALID_STATE,
    PM_CHG_UNKNOWN_STATE
}pm_chg_state_type;

typedef struct
{
    pm_chg_state_type previous_state;
    pm_chg_state_type current_state;
    uint32 batt_level;
}pm_chg_status_type;



/*===========================================================================

                     PROTOTYPES 

===========================================================================*/

/**
 * @name 
 *     pm_chg_get_battery_volt_level
 *
 * @description
 *    This function gets the current battery voltage level
 * 
 * @param 
 *     uint32* battery_voltage
 * 
 * @return 
 *     pm_err_flag_type
 * 
 * @dependency
 *     The following function must have been called:
 *        pm_driver_init()
 */
pm_err_flag_type pm_chg_get_battery_volt_level(uint32* battery_voltage);


/**
 * @name 
 *     pm_chg_sbl_charging_state_entry
 *
 * @description
 *    This function initializes sbl charging state machine.
 * 
 * @param 
 *     None
 * 
 * @return 
 *     pm_err_flag_type
 * 
 * @dependency
 *     The following function must have been called:
 *        pm_driver_init()
 */
pm_err_flag_type pm_chg_sbl_charging_state_entry(void);

/**
 * @name 
 *     pm_chg_is_battery_present
 *
 * @description
 *    This function checks for Battery precence
 * 
 * @param 
 *     boolean  battery_present
 *        TRUE:  Battery is present
 *        FALSE: Battery is NOT present
 * 
 * @return 
 *     pm_err_flag_type
 * 
 * @dependency
 *     The following function must have been called:
 *        pm_driver_init()
 */
pm_err_flag_type pm_chg_is_battery_present(boolean  *battery_present);

/**
 * @name 
 *     pm_chg_is_debug_board_present
 *
 * @description
 *    This function checks for Debug Board precence
 * 
 * @param 
 *     boolean  debug_board_present
 *        TRUE:  Debug Board is present
 *        FALSE: Debug Board is NOT present
 * 
 * @return 
 *     pm_err_flag_type
 * 
 * @dependency
 *     The following function must have been called:
 *        pm_driver_init()
 */
pm_err_flag_type pm_chg_is_debug_board_present(boolean  *debug_board_present);


/**
 * @name 
 *     pm_chg_is_battery_weak
 *
 * @description
 *    This function checks for Battery charge status
 * 
 * @param 
 *     boolean  *battery_weak
 *        TRUE:  Batteery is Weak
 *               ( Vbatt < PM_CHG_FLCB_OS_BOOT_UP_THRESHOLD )
 *        FALSE: Batteery is NOT Weak
 *               ( Vbatt >= PM_CHG_FLCB_OS_BOOT_UP_THRESHOLD )
 * 
 * @return 
 *     pm_err_flag_type
 * 
 * @dependency
 *     The following function must have been called:
 *        pm_driver_init()
 */
pm_err_flag_type pm_chg_is_battery_weak(boolean  *battery_weak);



/**
 * @name 
 *     battery_voltage
 *
 * @description
 *    This function returns current Battery voltage level
 * 
 * @param 
 *     uint32* battery_voltage
 * 
 * @return 
 *     pm_err_flag_type
 * 
 * @dependency
 *     The following function must have been called:
 *        pm_driver_init()
 */
pm_err_flag_type pm_chg_get_battery_voltage(uint32* battery_voltage);

/**
 * @name 
 *     pm_device_index
 *
 * @description
 *    This function returns the pmic device index
 *  
 * @return 
 *     pmic device index
 * 
 * @dependency
 *     The following function must have been called:
 *        pm_driver_init()
 */
uint8 pm_cfg_device_index(void);

/**
 * @brief This function initiate smbb or lbc charger. 
 * 
 *  \return Error flag.
 *  <b>Dependencies</b>
 *  \li The following function must have been called: pm_driver_init()
 
*/
void
pm_chg_driver_init(
    pm_comm_info_type *comm_ptr,
    peripheral_info_type * peripheral_info,
    uint8 pmic_index
);



#endif  //PM_APP_CHG__H

