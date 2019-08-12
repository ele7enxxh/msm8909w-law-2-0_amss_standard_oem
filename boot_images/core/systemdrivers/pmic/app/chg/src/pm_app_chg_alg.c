/*! \file
*  
*  \brief  pm_app_chg_alg.c
*  \details Implementation file for pmic sbl charging algorithm
*    
*  &copy; Copyright 2013-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================
                                Edit History
This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.
 
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/03/15   pxm     Set ibat_max to 500ma before setting boot_done bit. CR827060
01/06/15   pxm     Add generic PMIC LED API
11/21/14   pxm     CR758680. IBAT_ATC_B could be configured from 90ma to 1440ma from PM8916 v2.0
11/03/14   pxm     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_app_chg.h"
#include "boothw_target.h"
#include "pm_utils.h"

#include "pm_smb.h"
#include "pm_led.h"
#include "boot_logger.h"
#include "boot_logger_ram.h"
#include "boot_logger_uart.h"
#include "pm_smb_driver.h"
#include "DDIPlatformInfo.h"

/*===========================================================================

                     PROTOTYPES 

===========================================================================*/
//SBL charger state algorithms
pm_err_flag_type  pm_chg_battery_and_debug_board_detect_state(void);
pm_err_flag_type  pm_chg_charger_detect_state(void);
//pm_err_flag_type  pm_chg_wireless_charging_state(void);
pm_err_flag_type  pm_chg_cdp_charging_state(void);
pm_err_flag_type  pm_chg_dcp_charging_state(void);
pm_err_flag_type  pm_chg_float_charging_state(void);
pm_err_flag_type  pm_chg_sdp_charging_state(void);
pm_err_flag_type  pm_chg_bootup_ok_state(void);
pm_err_flag_type  pm_chg_shutdown_state(void);

//Common helper functions
pm_err_flag_type  pm_chg_process_sbl_charger_states(void);  //Process next charger state
pm_err_flag_type  pm_chg_enable_usb_charging(void);

//Test functions
pm_err_flag_type  pm_chg_get_sbl_charging_status(pm_chg_status_type *chg_state);
pm_err_flag_type  pm_chg_get_batt_volt_level(uint32 *batt_level);

/*=========================================================================== 
 
                     GLOBAL TYPE DEFINITIONS
 
===========================================================================*/
#define PLATFORMINFO_BYPASS_SMB_CFG_MASK 0x02

typedef pm_err_flag_type (*pm_chg_state_alg_ptr_type) (void);
typedef struct 
{
   pm_chg_state_type           current_chg_state;
   pm_chg_state_alg_ptr_type   next_chg_state_alg;
}pm_chg_state_alg_type;
static pm_chg_state_alg_type *next_state_ptr = NULL;
static boolean boot_with_charger = FALSE;
static pm_chg_type chg_chip_type = PM_CHG_INVAILD;
static boolean use_internal_charger = TRUE;
static boolean enum_timer_run = FALSE;
static boolean enum_expired = FALSE; // if enum_expired, hw will set ibat_max to 90mA
                                     // So don't set ibat_max in sbl after enum timer expired
static boolean smb_bypass_config = FALSE;

static pm_chg_state_alg_type pm_chg_state__battery_and_debug_board_detect =  
{
   PM_CHG_BATTERY_AND_DEBUG_BOARD_DETECT_STATE,
   pm_chg_battery_and_debug_board_detect_state
};

static pm_chg_state_alg_type pm_chg_state_charger_detect =  
{
   PM_CHG_USB_CHARGER_PORT_DETECT_STATE,
   pm_chg_charger_detect_state
};

static pm_chg_state_alg_type pm_chg_state__cdp_charging =  
{
   PM_CHG_CDP_CHARGING_STATE,
   pm_chg_cdp_charging_state
};

static pm_chg_state_alg_type pm_chg_state__dcp_charging =  
{
   PM_CHG_DCP_CHARGING_STATE,
   pm_chg_dcp_charging_state
};

static pm_chg_state_alg_type pm_chg_state__float_charging =  
{
   PM_CHG_FLOAT_CHARGING_STATE,
   pm_chg_float_charging_state
};

static pm_chg_state_alg_type pm_chg_state__sdp_charging =  
{
   PM_CHG_SDP_CHARGING_STATE,
   pm_chg_sdp_charging_state
};

static pm_chg_state_alg_type pm_chg_state__bootup_ok =  
{
   PM_CHG_BOOTUP_STATE,
   pm_chg_bootup_ok_state
};

static pm_chg_state_alg_type pm_chg_state__shutdown =  
{
   PM_CHG_SHUTDOWN_STATE,
   pm_chg_shutdown_state
};



static uint16 *sbl_chg_app_ds = NULL; 
static pm_chg_status_type pm_chg_status;
static qhsusb_charger_port_type s_charger_port = QHSUSB_CHARGER_PORT_UNKNOWN;


/*===========================================================================

                     FUNCTION IMPLEMENTATION 

===========================================================================*/

static pm_err_flag_type pm_get_smb_by_pass_config(boolean* bypass)
{
    DalDeviceHandle *phPlatformInfoDevice = NULL;
    DalPlatformInfoPlatformInfoType platformInfo = {DALPLATFORMINFO_TYPE_UNKNOWN,};
    DALResult result = 0;

    /* Get the platform info */
    result = DAL_PlatformInfoDeviceAttach(DALDEVICEID_PLATFORMINFO, &phPlatformInfoDevice);
    if(DAL_SUCCESS != result)
    {
        boot_log_message("Error during device attach");
        return (pm_err_flag_type) result;
    }

    result = DalPlatformInfo_GetPlatformInfo(phPlatformInfoDevice, &platformInfo);
    if(DAL_SUCCESS != result)
    {
        boot_log_message("Error during get platinfo");
        return (pm_err_flag_type) result;
    }

    // If this bit is set, it means it's non-1360 SMB charger and didn't have driver. So by pass I2C operation
    *bypass = PLATFORMINFO_BYPASS_SMB_CFG_MASK == (platformInfo.version & PLATFORMINFO_BYPASS_SMB_CFG_MASK);

    return PM_ERR_FLAG__SUCCESS;
}

static pm_err_flag_type pm_chg_enter_hw_trickle_charging(uint8 device_index)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    boot_log_message("-->hw ATC");
    if(TRUE == enum_expired)
    {
        // if enum expired, don't set ibat_max by sw, hw would do that
        err_flag |= pm_chg_set_ibat_max(device_index, sbl_chg_app_ds[PM_CHG_USB_IUNIT]);
    }
    else
    {
        // Force ATC on for SMBB so sw would run until VBAT reach VBAT_WEAK.
        // It's NOT necessary since VPH_PWR is short to VBAT so that software couldn't run till VBAT_WEAK
        if(PM_CHG_SMBB == chg_chip_type)
        {
            err_flag |= pm_chg_set_itrkl(device_index, sbl_chg_app_ds[PM_CHG_USB_IUNIT]);
            err_flag |= pm_chg_set_mode(device_index, PM_CHG_MODE__ATC_ON);
        }
        else if(PM_CHG_LBC == chg_chip_type)
        {
            err_flag |= pm_chg_set_ibat_atc_b(device_index, sbl_chg_app_ds[PM_CHG_USB_IUNIT]);
        }
    }

    err_flag |= pm_chg_set_battery_threshold(device_index, 
            sbl_chg_app_ds[PM_CHG_FLCB_DEAD_BATTERY_THRESHOLD],
            sbl_chg_app_ds[PM_CHG_FLCB_OS_BOOTUP_AFTER_USB_SUSPEND_THRESHOLD]); 

    next_state_ptr = &pm_chg_state__shutdown;

    return err_flag;
}

static pm_err_flag_type pm_chg_sbl_charging_initialize(void)
{
    pm_err_flag_type err = PM_ERR_FLAG__SUCCESS;    
    uint8 device_index = pm_cfg_device_index();

    boot_log_message("--> Charge init");
    chg_chip_type = pm_chg_get_peripheral_sub_type(device_index);

    err |= pm_chg_set_battery_threshold(device_index, sbl_chg_app_ds[PM_CHG_FLCB_DEAD_BATTERY_THRESHOLD], sbl_chg_app_ds[PM_CHG_FLCB_WEAK_BATTERY_THRESHOLD]);
    err |= pm_chg_set_ibat_atc_b(device_index, sbl_chg_app_ds[PM_CHG_USB_IBAT_ATC_B_NORMAL]);
    err |= pm_chg_set_vin_min(device_index, sbl_chg_app_ds[PM_CHG_VIN_MIN]);
    err |= pm_chg_set_ibat_safe(device_index, sbl_chg_app_ds[PM_CHG_IBAT_SAFE]);
    if(PM_CHG_LBC == chg_chip_type)
    {
        err |= pm_chg_get_enum_timer_status(device_index, &enum_timer_run);

        // Set VDD_MAX, VDD_SAFE
        // err |= pm_lbc_chg_set_vdd_max(device_index, PM_LBC_1, vdd_max);
        // err |= pm_lbc_chg_set_vdd_safe(device_index, PM_LBC_1, vdd_safe);

        // Read CHG_OPTION
        err |= pm_chg_get_chg_option(device_index, &use_internal_charger);
        
        // Enable charger watch dog
        // err |= pm_lbc_chg_wdog_enable(device_index, PM_LBC_1, TRUE);
    }

    // set Vtrickle and Vweak explicitly. Have seen the case that Vweak is changed but didn't find the code in sbl.
    if(PM_CHG_SMBB == chg_chip_type)
    {
        pm_chg_set_mode(device_index, PM_CHG_MODE__ATC_OFF);
        // TODO: explicitly set iTrickle here.
    }

    return err;
}

/** 
 * @name: pm_chg_sbl_charging_state_entry() 
 *
 * @description : This function is the main entry function to 
 *              the charger state machine.  It is called at the
 *              end of PMIC driver initialization:
 *              pm_driver_init()
 * 
 */
pm_err_flag_type  pm_chg_sbl_charging_state_entry(void)   //called at the end of pm_driver_init
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    pm_chg_status.previous_state = PM_CHG_ENTRY_STATE;
    pm_chg_status.current_state  = PM_CHG_ENTRY_STATE;
    pm_chg_status.batt_level  = 0;

    //Get handle for charger algorithm specific data (from Dal config)
    sbl_chg_app_ds = (uint16*)pm_target_information_get_specific_info((uint32)PM_PROP_CHG_APP_LUT);

    //Check Battery/Debug board presence
    next_state_ptr = &pm_chg_state__battery_and_debug_board_detect;

    err_flag |= pm_chg_sbl_charging_initialize();
    
    err_flag |= pm_chg_process_sbl_charger_states();  //Process next sbl charging state
    
    if( err_flag != PM_ERR_FLAG__SUCCESS)  
    {//Handle All SBL charger algorithm errors
       BOOT_ERR_FATAL();      
    }
   
    return err_flag;
}


/** 
 * @name: pm_chg_process_sbl_charger_states() 
 *
 * @description: This function procesing the next SBL charging 
 *           state.  Each charging state configures the next
 *           state by updating the global static var
 *           next_state_ptr and returns completely.
 *  
 *           The while loop inside this function ensure that the
 *           next state pointed by next_state_ptr gets called.
 *  
 *           This function only returns if there is an error or
 *           if we just finished executing Bootup OK state.
 * 
 */
 static pm_err_flag_type  pm_chg_process_sbl_charger_states(void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    pm_chg_state_alg_ptr_type next_state = NULL;

    //Process SBL charging states transitions
    while( (next_state_ptr != NULL)  )
    {
        pm_chg_status.previous_state = pm_chg_status.current_state;
        pm_chg_status.current_state  = next_state_ptr->current_chg_state;

        next_state = next_state_ptr->next_chg_state_alg;
        if (next_state)
        {
           err_flag = next_state();  //transition to next state
        }
                
        if ( ( err_flag                     != PM_ERR_FLAG__SUCCESS  )  ||
             ( pm_chg_status.current_state == PM_CHG_BOOTUP_STATE   )  ||
             ( pm_chg_status.current_state == PM_CHG_SHUTDOWN_STATE )      //Shutdown state condition will never happen but we have it for sake of being complete
           )
        {
            boot_log_message_uart("Current status",pm_chg_status.current_state,LOG_MSG_TYPE_BOOT, NULL);
            break;  
        }
    }
    return err_flag;
}



/** 
 * @name: pm_chg_battery_and_debug_board_detect_state() 
 *
 * @description : This function detects detection of battery and 
 *              debug board presence
 *             If debug board is detected, it setus up Bootup OK
 *             state as a next state and returns.
 *  
 *             If battery is detected and the battery charge
 *              level is > PM_CHG_FLCB_OS_BOOT_UP_THRESHOLD,
 *              then it sets up Bootup_OK as a next state and
 *              returns.
 *  
 *              If battery is detected and battery level is <
 *              PM_CHG_FLCB_OS_BOOT_UP_THRESHOLD, then it sets
 *              up charger_type_detect as a next state and
 *              returns.
 * 
 */
pm_err_flag_type  pm_chg_battery_and_debug_board_detect_state(void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    boolean battery_present = FALSE;
    boolean debug_board_present = FALSE;
    boolean battery_weak = TRUE;
    uint8 device_index = pm_cfg_device_index();
    pm_model_type primary_pmic, secondary_pmic;


    //Check for debug board presence
    primary_pmic = pm_get_pmic_model(0);
    secondary_pmic = pm_get_pmic_model(1);
    if( (primary_pmic == PMIC_IS_PMA8084) && (secondary_pmic == PMIC_IS_PM8941) ) //If target is 8974Pro AC, skip debug board presence detection
    {
       debug_board_present = 0; 
    }
    else
    {
       err_flag = pm_chg_is_debug_board_present(&debug_board_present);
    }
    
    if(err_flag == PM_ERR_FLAG__SUCCESS)
    {
       if( debug_board_present == TRUE)
       {
          boot_log_message("debug board present");
          next_state_ptr = &pm_chg_state__bootup_ok;  //Bootup OK
       }
       else
       {
          //Check for battery presence
          err_flag = pm_chg_is_battery_present(&battery_present);
          if(err_flag == PM_ERR_FLAG__SUCCESS)
          {
             if( battery_present == FALSE)
             { //No battery presence is detected
                boot_log_message("battery good, boot up");
                err_flag |= pm_chg_set_ibat_max(device_index, sbl_chg_app_ds[PM_CHG_USB_IDEV_DCHG]);
                next_state_ptr = &pm_chg_state__bootup_ok; 
             }
             else //Battery is present
             {
                //Check for battery weak state
                err_flag = pm_chg_is_battery_weak(&battery_weak);
                if(err_flag == PM_ERR_FLAG__SUCCESS)
                {
                   if( battery_weak == TRUE)//Battery weak
                   {
                      next_state_ptr = &pm_chg_state_charger_detect;
                   }
                   else //Battery is in good charge level
                   {
                      next_state_ptr = &pm_chg_state__bootup_ok;   //Bootup OK
                   }
                }
             }
          }
       }
    }

    return err_flag;
}

static void pm_set_boot_done(void)
{
    uint8 device_index = pm_cfg_device_index();
    pm_chg_set_boot_done(device_index);
}

/** 
 * @name: pm_chg_charger_detect_state() 
 *
 * @description : This function detects the type of USB charger 
 *              connected to the device. This function is called
 *              when Battery is dead/weak.
 *  
 *             The following USB chargers may be connected:
 *             SDP = Standard Downstream Port
 *             CDP = Charger Downstream Port
 *             DCP = Dedicated Charger Port
 *  
 */
pm_err_flag_type  pm_chg_charger_detect_state(void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    boolean  usb_driver_init_status;
    uint8 device_index = pm_cfg_device_index();

    err_flag = pm_chg_is_charger_present(&boot_with_charger);
    if( (err_flag == PM_ERR_FLAG__SUCCESS) && (TRUE == boot_with_charger) )
    {
        if(FALSE == use_internal_charger)
        {
            // For external charger on LCB charger, just configure IBAT to the max and then enable charging
            err_flag |= pm_get_smb_by_pass_config(&smb_bypass_config);
            if(PM_ERR_FLAG__SUCCESS != err_flag)
            {
                return err_flag;
            }
            else if(!smb_bypass_config)
            {
                err_flag  = pm_smb_init();
                if(PM_ERR_FLAG__SUCCESS != err_flag)
                {
                    if(NULL == pm_smb_get_info())
                    {
                        smb_bypass_config = TRUE;
                        boot_log_message("Couldn't get smb_hw_info, skip enum and smb config");
                    }
                    else
                    {
                        return err_flag;
                    }
                }
            }
            if(smb_bypass_config)
            {
                return pm_chg_enable_usb_charging();
            }
        }
        else
        {
            // Set ibat_max to 500ma. it's allowed at first 2 minutes regardless of enumeration.
            // Once we determine doing weak charging, raise ibat_max to 500ma before charger type detection.
            err_flag |= pm_chg_set_ibat_max(device_index, sbl_chg_app_ds[PM_CHG_USB_IDEV_SDP]);
            if(PM_ERR_FLAG__SUCCESS != err_flag)
            {
                return err_flag;
            }
            pm_set_boot_done(); // set boot done bit to enable fast charging
        }

        //Detect charger port type
        usb_driver_init_status = qhsusb_al_chg_init();  // init 
        if( usb_driver_init_status )
        {
             s_charger_port = (qhsusb_charger_port_type)qhsusb_al_chg_get_charger_port_type(); 
             switch(s_charger_port)
             {
             case QHSUSB_CHARGER_PORT_SDP:
                 boot_log_message("SDP");
                 next_state_ptr = &pm_chg_state__sdp_charging; 
                 break;
             case QHSUSB_CHARGER_PORT_CDP:
                 boot_log_message("CDP");
                 next_state_ptr = &pm_chg_state__cdp_charging; 
                 break;
             case QHSUSB_CHARGER_PORT_DCP:
                 boot_log_message("DCP");
                 next_state_ptr = &pm_chg_state__dcp_charging; 
                 break;
             case QHSUSB_CHARGER_PORT_DCP_NON_COMPLIANT:
                 boot_log_message("Float");
                 next_state_ptr = &pm_chg_state__float_charging;
                 break;
             default:
                 boot_log_message("unknown-->shutdown");
                 next_state_ptr = &pm_chg_state__shutdown;
                 // err_flag = PM_ERR_FLAG__INVALID_CHG_INDEXED;
                 break;
             }
        }
        else
        {
            err_flag = PM_ERR_FLAG__INVALID_CHG_INDEXED;
        }
    }
    else if( (err_flag == PM_ERR_FLAG__SUCCESS) && (FALSE == boot_with_charger) )
    {
        boot_log_message("boot but no charger, shutdown");
        next_state_ptr = &pm_chg_state__shutdown;
        err_flag = PM_ERR_FLAG__SUCCESS;
    }
    else
    {
        boot_log_message("error in charger detectiong");
        err_flag = PM_ERR_FLAG__INVALID_CHG_INDEXED;  //USB charger type detection error
    }

    return err_flag;
}

static boolean pm_chg_enum_timer_expire(void)
{
    boolean expired = FALSE;
    uint8 device_index = pm_cfg_device_index();

    if(PM_ERR_FLAG__SUCCESS != pm_chg_get_enum_timer_expired(device_index, &expired))
    {
        return FALSE;
    }
    else
    {
        return expired;
    }
}

/*
static pm_err_flag_type pm_pet_wdog(void)
{
    return pm_lbc_chg_wdog_pet(pm_cfg_device_index(), PM_LBC_1);
}
*/
/** 
 * @name:  pm_chg_sdp_charging_state()
 *
 * @description : This function is called when SDP(Standard 
 *              Downstream Port) type USB charger is connected.
 *              If the battery charge level is weak(Vbatt<
 *              FLCB_OS_BOOT_UP_THRESHOLD), it charges it until
 *              the battery level is good.
 * 
 */
pm_err_flag_type  pm_chg_sdp_charging_state(void)  
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 device_index = pm_cfg_device_index();
    qhsusb_enumeration_result enum_status = QHSUSB_ENUMERATION_FAILURE;

    if(FALSE == use_internal_charger)
    {
        // Set USB500 mode for SDP before enumeration is started.
        // 500ma is allowed at first 2 minutes before enumerated fail.
        boot_log_message("Set USB500 mode");
        err_flag |= pm_smb_set_icl_mode(IL_USB500);
    }

    // qhsusb_al_chg_enumerate_usb has sw delay inside and would include disconnect
    // detection so it's not necessary to add sw delay and charger absent detect here.
    do
    {
        enum_status = qhsusb_al_chg_enumerate_usb();

        if(FALSE == use_internal_charger)
        {
            // For external charger, only enum for 30s (sw timer in qhsusb_al_chg_enumerate_usb)
            break;
        }
        else if(PM_CHG_LBC == chg_chip_type && TRUE == enum_timer_run)
        {
            // TODO: since qhsusb_al_chg_enumerate_usb will excute for 30s if enum not success,
            // we need to check enum timer expire situation in the function and then check return value,
            // otherwise we need to implement the enum in this file instead of using usb enum API.
            enum_expired = pm_chg_enum_timer_expire();
        }
    } while ((FALSE == enum_expired)
            && (enum_status != QHSUSB_ENUMERATION_SUCCESS)
            && (enum_status != QHSUSB_DEVICE_DISCONNECTED));

    if(QHSUSB_ENUMERATION_SUCCESS == enum_status)
    {
        boot_log_message("===== enum success =====");
        if(TRUE == use_internal_charger)
        {
            if(PM_CHG_SMBB == chg_chip_type)
            {
                err_flag |= pm_chg_set_iusb_max(device_index, sbl_chg_app_ds[PM_CHG_USB_IDEV_SDP]);
            }
            else if(PM_CHG_LBC == chg_chip_type)
            {
                err_flag |= pm_chg_stop_enum_timer(device_index);
            }
        }

        err_flag |= pm_chg_enable_usb_charging();   //Common USB charging routine
    }
    else if(QHSUSB_DEVICE_DISCONNECTED == enum_status)
    {
        boot_log_message("Disconnected during enum");
        next_state_ptr = &pm_chg_state__shutdown;
    }
    else
    {
        boot_log_message("Enum fail-->HW ATC");
        err_flag |= pm_chg_enter_hw_trickle_charging(device_index);
    }

    return err_flag;
}


/** 
 * @name:  pm_chg_cdp_charging_state()
 *
 * @description : This function is called when CDP(Charger 
 *              Downstream Port) type USB charger is connected.
 *              If the battery charge level is weak(Vbatt<
 *              FLCB_OS_BOOT_UP_THRESHOLD), it charges it until
 *              the battery level is good.
 * 
 */
pm_err_flag_type  pm_chg_cdp_charging_state(void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 device_index = pm_cfg_device_index();

    //Set Ibat_Max = 1500mA for CDP/DCP and Ibat_max = 500mA for SDP
    if(TRUE == use_internal_charger)
    {
        err_flag  = pm_chg_set_ibat_max(device_index, sbl_chg_app_ds[PM_CHG_USB_IDEV_DCHG]);
    }
    else
    {
        boot_log_message("Set AC mode and set to 1350ma");
        // To avoid some scenario that raising ICL couldn't take effect
        // change the input current limit setting to higher value
        // --> enable USB500
        // --> enable AC mode
        err_flag |= pm_smb_set_ibat_fast(sbl_chg_app_ds[PM_CHG_USB_IDEV_DCHG]);
        err_flag |= pm_smb_set_icl(sbl_chg_app_ds[PM_CHG_USB_IDEV_DCHG]);
        err_flag |= pm_smb_set_icl_mode(IL_USB500);
        err_flag |= pm_smb_set_icl_mode(IL_AC);
    }
    err_flag |= pm_chg_enable_usb_charging();  //Common USB charging routine 

    return err_flag;
}





/** 
 * @name:  pm_chg_dcp_charging_state()
 *
 * @description : This function is called when DCP(Dedicated 
 *              Charger Port) type USB charger is connected.  If
 *              the battery charge level is weak (Vbatt <
 *              FLCB_OS_BOOT_UP_THRESHOLD), it charges it until
 *              the battery level is good.
 * 
 */
pm_err_flag_type  pm_chg_dcp_charging_state(void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 device_index = pm_cfg_device_index();

    //Set Ibat_Max = 1500mA for CDP/DCP and Ibat_max = 500mA for SDP
    if(TRUE == use_internal_charger)
    {
        boot_log_message("Internal charger");
        err_flag  = pm_chg_set_ibat_max(device_index, sbl_chg_app_ds[PM_CHG_USB_IDEV_DCHG]);
    }
    else
    {
        boot_log_message("Set AC mode and set to 1350ma");
        // To avoid some scenario that raising ICL couldn't take effect
        // change the input current limit setting to higher value
        // --> enable USB500
        // --> enable AC mode
        err_flag |= pm_smb_set_ibat_fast(sbl_chg_app_ds[PM_CHG_USB_IDEV_DCHG]);
        err_flag |= pm_smb_set_icl(sbl_chg_app_ds[PM_CHG_USB_IDEV_DCHG]);
        err_flag |= pm_smb_set_icl_mode(IL_USB500);
        err_flag |= pm_smb_set_icl_mode(IL_AC);
    }
    err_flag |= pm_chg_enable_usb_charging();  //Common USB charging routine 

    return err_flag;
}

/** 
 * @name:  pm_chg_float_charging_state()
 *
 * @description : This function is called when floating (non-compliant 
 *              Charger Port) type USB charger is connected.  If
 *              the battery charge level is weak (Vbatt <
 *              FLCB_OS_BOOT_UP_THRESHOLD), it charges it until
 *              the battery level is good.
 * 
 */
pm_err_flag_type  pm_chg_float_charging_state(void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 device_index = pm_cfg_device_index();

    // Set the upper limit for float charger. 
    if(TRUE == use_internal_charger)
    {
        err_flag  = pm_chg_set_ibat_max(device_index, sbl_chg_app_ds[PM_CHG_USB_IDEV_FLOAT]);
    }
    else
    {
        boot_log_message("Set AC mode and set to 1350ma");
        // To avoid some scenario that raising ICL couldn't take effect
        // change the input current limit setting to higher value
        // --> enable USB500
        // --> enable AC mode 
        err_flag |= pm_smb_set_ibat_fast(sbl_chg_app_ds[PM_CHG_USB_IDEV_FLOAT]);
        err_flag |= pm_smb_set_icl(sbl_chg_app_ds[PM_CHG_USB_IDEV_FLOAT]);
        err_flag |= pm_smb_set_icl_mode(IL_USB500);
        err_flag |= pm_smb_set_icl_mode(IL_AC);
    }

    err_flag |= pm_chg_enable_usb_charging();  //Common USB charging routine 

    return err_flag;
}

/** 
 * @name:  pm_chg_enable_usb_charging()
 *
 * @description : This function is common USB charging 
 *              algorithm. It is called by SDP, CDP and DCP USB
 *              charging states.  If the battery charge level is
 *              weak(Vbatt< FLCB_OS_BOOT_UP_THRESHOLD), it
 *              charges it until the battery level is good.
 * 
 */
pm_err_flag_type  pm_chg_enable_usb_charging(void)  
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    boolean charger_present =  FALSE;
    boolean toggle_led = FALSE;
    boolean battery_weak = TRUE;
    boolean battery_present = FALSE;
    pm_led_type led_default = PM_LED_TYPE_INVALID;
    qhsusb_boot_poll_result usb_poll_status = HSU_BOOT_POLL_RESULT_ERROR; 
    uint8 device_index = pm_cfg_device_index();

    if(TRUE == use_internal_charger && 
        PM_CHG_LBC == chg_chip_type && 
        TRUE == enum_timer_run && 
        FALSE == enum_expired)
    {
        err_flag = pm_chg_stop_enum_timer(device_index);
    }

    err_flag |= pm_chg_is_battery_weak(&battery_weak);
    if (err_flag == PM_ERR_FLAG__SUCCESS)
    {
        if( battery_weak == FALSE )
        {
            next_state_ptr = &pm_chg_state__bootup_ok; 
        }
        else
        {
            if(TRUE == use_internal_charger)
            {
                err_flag |= pm_chg_set_mode(device_index, PM_CHG_MODE__CHARGE_ON);
            }
            led_default = pm_led_get_default_led();
            while( battery_weak )  //While battery is in weak state
            {
                //Check for battery disconnection
                err_flag |= pm_chg_is_battery_present(&battery_present);
                if(err_flag == PM_ERR_FLAG__SUCCESS)
                {
                    if( battery_present == FALSE) //Battery is disconnected
                    {
                       next_state_ptr = &pm_chg_state__bootup_ok; //boot up without battery 
                       break;
                    }
                 }

                //Check USB charger presence
                err_flag |= pm_chg_is_charger_present(&charger_present);
                if( (charger_present == FALSE) || (err_flag != PM_ERR_FLAG__SUCCESS) )//Charger disconnected
                {
                    next_state_ptr = &pm_chg_state__shutdown;
                    battery_weak = TRUE;
                    break;
                }

                if(use_internal_charger || !smb_bypass_config)
                {
                    //Check if USB is in SUSPEND/RESET state
                    usb_poll_status = (qhsusb_boot_poll_result)qhsusb_al_chg_poll();
                    if( (usb_poll_status ==  HSU_BOOT_POLL_RESULT_SUSPENDED ) || 
                        (usb_poll_status ==  HSU_BOOT_POLL_RESULT_RESUMED_NOT_ENUMERATED) )
                    {
                        boot_log_message_uart("UBS poll status", usb_poll_status, LOG_MSG_TYPE_BOOT, NULL);
                        if(TRUE == use_internal_charger)
                        {
                            err_flag |= pm_chg_enter_hw_trickle_charging(device_index);
                        }
                        else
                        {
                            boot_log_message("Set USB100 mode");
                            err_flag |= pm_smb_set_icl_mode(IL_USB100);
                            next_state_ptr = &pm_chg_state__shutdown;
                        }
                        battery_weak = TRUE;
                        break;
                    }
                }

                //Enable charging and LED blinking
                toggle_led = !toggle_led;
                if(PM_LED_TYPE_INVALID != led_default)
                {
                    pm_led_enable(device_index, led_default, toggle_led);
                }

                // pm_pet_wdog();
                (void)DALSYS_BusyWait(1000*500); // Poll every 500ms   

                //Check battery weak state
                err_flag |= pm_chg_is_battery_weak(&battery_weak);
                if( err_flag != PM_ERR_FLAG__SUCCESS){break;} //Check for ADC battery level read error
            }//while

            if((battery_weak == FALSE) && (battery_present == TRUE) && (err_flag == PM_ERR_FLAG__SUCCESS))
            {
                if(!qhsusb_al_chg_should_chg_be_on())
                {
                    qhsusb_al_chg_disconnect_from_host();
                    if(use_internal_charger || !smb_bypass_config)
                    {
                        // For the conditions that usb operation is required.
                        // In thiw package warehouse, the pmic charger always need enumeration
                        // For some low tier smb chip such as smb1360, enumeration is required
                        // so we can set current limit accordingly.
                        qhsusb_al_chg_deinit();
                    }

                    if(use_internal_charger)
                    {
                        err_flag |= pm_chg_set_mode(device_index, PM_CHG_MODE__CHARGE_OFF);
                    }
                }
                next_state_ptr = &pm_chg_state__bootup_ok;    //Boot up OK
            }
        }
    }
    return err_flag;
}



/** 
 * @name:  pm_chg_bootup_ok_state()
 *
 * @description : This function implements the the bootup OK 
 *              charging algorithm. It is called when the
 *              battery state is good to boot up or when
 *              dedicated charger is detected. It does needed
 *              configuration before returning and let the
 *              system to boot up.
 * 
 */
pm_err_flag_type  pm_chg_bootup_ok_state(void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 device_index = pm_cfg_device_index();

    if(s_charger_port == QHSUSB_CHARGER_PORT_SDP)
    {
        qhsusb_al_chg_disconnect_from_host();
    } 
    boot_log_message("boot up");
    if(boot_with_charger && (FALSE == use_internal_charger))
    {
        boot_log_message("smb deinit");
        err_flag |= pm_smb_deinit();
    }
    else
    {
        if(!qhsusb_al_chg_should_chg_be_on())
        {
           err_flag |= pm_chg_set_mode(device_index, PM_CHG_MODE__CHARGE_OFF);  //disable boot charging
        }

        err_flag |= pm_chg_set_comp_override(device_index, FALSE); //Disable VBAT_WEAK override
    }
    return err_flag;
}




/** 
 * @name:  pm_chg_shutdown_state()
 *
 * @description : This function implements shutdown charging 
 *              algorithm.  It is called when charger is
 *              disconnected while the battery is in a weak
 *              state and also when USB chanrger is in
 *              Suspend/reset state.  This function does needed
 *              configuration before shutting down the system.
 * 
 */
pm_err_flag_type  pm_chg_shutdown_state(void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint8 device_index = pm_cfg_device_index();

    boot_log_message("shut down");
    err_flag = pm_chg_set_comp_override(device_index, FALSE); //Disable VBAT_WEAK override
    if(boot_with_charger)
    {
        if(FALSE == use_internal_charger)
        {
            boot_log_message("smb deinit and shutdown");
            err_flag |= pm_smb_set_icl_mode(IL_USB500);
            err_flag |= pm_smb_deinit();
            boot_log_message("smb deinit end");
        }
        if(use_internal_charger || !smb_bypass_config)
        {
            qhsusb_al_chg_deinit(); //deinit usb driver
            boot_log_message("qhsusb_al_chg_deinit");
        }
    }
    boot_hw_powerdown();

    return err_flag;  
}




/** 
 * @name:  pm_chg_get_sbl_charging_status()
 *
 * @description : This function returns the current SBL charging
 *              state.  Used for testing/debugging
 *  
 */
pm_err_flag_type  pm_chg_get_sbl_charging_status(pm_chg_status_type *chg_state)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    *chg_state = pm_chg_status;  
    return err_flag;
}



/** 
 * @name:  pm_chg_get_batt_volt_level()
 *
 * @description : This function returns the current battery 
 *              voltage level.  Used for testing/debugging.
 * 
 */
pm_err_flag_type  pm_chg_get_batt_volt_level(uint32 *batt_level)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    *batt_level = pm_chg_status.batt_level;
    return err_flag;
}


/**
 * @name : pm_chg_is_battery_weak ()
 *
 * @description : This function checks for Battery charge status
 */
pm_err_flag_type pm_chg_is_battery_weak(boolean  *battery_weak)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint32 batt_voltage = 0x0; 

    err_flag = pm_chg_get_battery_volt_level(&batt_voltage);
    if(err_flag == PM_ERR_FLAG__SUCCESS)
    {
        pm_chg_status.batt_level = batt_voltage;  //update global var
        boot_log_message_uart("battery voltage", batt_voltage, LOG_MSG_TYPE_BOOT, NULL);

        if(batt_voltage < sbl_chg_app_ds[PM_CHG_FLCB_OS_BOOT_UP_THRESHOLD])
        {
            *battery_weak = TRUE;  //battery is weak
        }
        else
        {
            *battery_weak = FALSE;  //battery is good
        }
    }

    return err_flag;
}

uint8 pm_cfg_device_index(void)
{
    return sbl_chg_app_ds[PM_CHG_PMIC_CFG_DEVICE_INDEX];
}


