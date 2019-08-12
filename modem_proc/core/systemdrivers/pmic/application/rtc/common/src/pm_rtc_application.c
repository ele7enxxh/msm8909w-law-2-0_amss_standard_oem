/*! \file
*  
*  \brief  pm_rtc_application.c ----File contains the implementation of the public APIs for RTCAPP resource type.
*  \details Implementation file for RTCAPP resourece type.
*  Each of the APIs checks for access and then if necessary directs
*  the call to Driver implementation or to RPC function for the master processor.
*  
*    PMIC code generation Version: 2.0.0.22
*    This file contains code for Target specific settings and modes.
*  
*  &copy; Copyright 2010 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/application/rtc/common/src/pm_rtc_application.c#1 $ 

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_rtc_application.h"
#include "pmapp_rtc.h"

/*===========================================================================

                        STATIC VARIABLES 

===========================================================================*/
/* Static global variable to store the RTC APP data */
static pm_rtc_app_data_type pm_rtc_app_data;

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/
void pm_rtc_app_init()
{	
    uint16* tempTable_leap;  
    uint16* tempTable_normal;   
    uint16* tempTable_year;   

    /* The leap_month_tab table holds the number of cumulative days that have
       elapsed as of the end of each month during a leap year. */
    pm_malloc(sizeof(uint16)*13, (void**)&tempTable_leap);

/* The norm_month_tab table holds the number of cumulative days that have
   elapsed as of the end of each month during a non-leap year. */
    pm_malloc(sizeof(uint16)*13, (void**)&tempTable_normal);

/* The year_tab table is used for determining the number of days which
   have elapsed since the start of each year of a leap year set. It has
   1 extra entry which is used when trying to find a 'bracketing' year. */
    pm_malloc(sizeof(uint16)*5, (void**)&tempTable_year);
   
    tempTable_leap[0] =  0;    
    tempTable_leap[1] =  31;                                      /* Jan */
    tempTable_leap[2] =  31+29;                                   /* Feb */
    tempTable_leap[3] =  31+29+31;                                /* Mar */
    tempTable_leap[4] =  31+29+31+30;                             /* Apr */
    tempTable_leap[5] =  31+29+31+30+31;                          /* May */
    tempTable_leap[6] =  31+29+31+30+31+30;                       /* Jun */
    tempTable_leap[7] =  31+29+31+30+31+30+31;                    /* Jul */
    tempTable_leap[8] =  31+29+31+30+31+30+31+31;                 /* Aug */
    tempTable_leap[9] =  31+29+31+30+31+30+31+31+30;              /* Sep */
    tempTable_leap[10] = 31+29+31+30+31+30+31+31+30+31;           /* Oct */
    tempTable_leap[11] = 31+29+31+30+31+30+31+31+30+31+30;        /* Nov */
    tempTable_leap[12] = 31+29+31+30+31+30+31+31+30+31+30+31;     /* Dec */

    pm_rtc_app_data.PM_RTC_LEAP_MONTH_TAB = tempTable_leap;

    tempTable_normal[0] =  0;    
    tempTable_normal[1] =  31;                                      /* Jan */
    tempTable_normal[2] =  31+28;                                   /* Feb */
    tempTable_normal[3] =  31+28+31;                                /* Mar */
    tempTable_normal[4] =  31+28+31+30;                             /* Apr */
    tempTable_normal[5] =  31+28+31+30+31;                          /* May */
    tempTable_normal[6] =  31+28+31+30+31+30;                       /* Jun */
    tempTable_normal[7] =  31+28+31+30+31+30+31;                    /* Jul */
    tempTable_normal[8] =  31+28+31+30+31+30+31+31;                 /* Aug */
    tempTable_normal[9] =  31+28+31+30+31+30+31+31+30;              /* Sep */
    tempTable_normal[10] = 31+28+31+30+31+30+31+31+30+31;           /* Oct */
    tempTable_normal[11] = 31+28+31+30+31+30+31+31+30+31+30;        /* Nov */
    tempTable_normal[12] = 31+28+31+30+31+30+31+31+30+31+30+31;     /* Dec */

    pm_rtc_app_data.PM_RTC_NORMAL_MONTH_TAB = tempTable_normal;

    tempTable_year[0] =  0;    
    tempTable_year[1] =  366;                                      
    tempTable_year[2] =  366+365;                                  
    tempTable_year[3] =  366+365+365;                                
    tempTable_year[4] =  366+365+365+365;       

    pm_rtc_app_data.PM_RTC_YEAR_TAB = tempTable_year;


    pm_rtc_app_data.pm_app_rtc_current_display_mode = PM_RTC_INVALID_MODE;
    pm_rtc_app_data.pm_rtc_power_reset_status		= FALSE;

    pm_rtc_app_data.PM_RTC_MIN_YEAR			= 1980;  // Lowest valid year
    pm_rtc_app_data.PM_RTC_MAX_YEAR			= 2100;  // Highest valid year
    pm_rtc_app_data.PM_RTC_MAX_MONTH		= 12;    // Highest valid number of months
    pm_rtc_app_data.PM_RTC_MAX_DOW			= 6;     // Highest valid number of day-of-week
    pm_rtc_app_data.PM_RTC_MAX_DAY			= 31;   // Highest valid number of days
    pm_rtc_app_data.PM_RTC_MAX_24HR_HOUR	= 23;    // Highest valid number of hours (24HR mode)
    pm_rtc_app_data.PM_RTC_MAX_12HR_HOUR	= 32;    // Highest valid number of hours (12HR mode)
    pm_rtc_app_data.PM_RTC_MIN_12HR_HOUR	= 01;    // Lowest valid number of hours  (12HR mode)
    pm_rtc_app_data.PM_RTC_MIDL_12HR_HOUR	= 12;    // In 12HR mode, 13 to 20 are
    pm_rtc_app_data.PM_RTC_MIDH_12HR_HOUR	= 21;    //    not valid values
    pm_rtc_app_data.PM_RTC_MAX_MINUTES		= 59;    // Highest valid number of minutes
    pm_rtc_app_data.PM_RTC_MAX_SECONDS		= 59;    // Highest valid number of seconds

    pm_rtc_app_data.PM_RTC_BASE_YEAR        = 1980; 
    pm_rtc_app_data.PM_RTC_QUAD_YEAR        = 366+(3*365);

    pm_rtc_app_data.RM_RTC_CLK_OFFSET_S     = 432000;

    /*  During INIT if no one has started RTC [i.e. after a xvDD reset event], then SEC_RTC_CNTRL[7] 
        should be 0 and pm_hal_rtc_get_time() should return the error and we set the flag accordingly.
        If you boot and RTC_EN is zero, your RTC isn't running, and it can't be trusted. 
        i.e. SEC_RTC_CNTRL[7] is RTC_EN.  If it is zero then an RTC reset has occurred. */
    {
        pm_hal_rtc_time_type verifyTime = { 0 } ;
        pm_err_flag_type err = pm_hal_rtc_get_time ( &verifyTime ) ;
        
        /* If RTC is enabled (ON), rtcONOFF should return "1". And "0" if it is disabled (OFF). */
        if ( PM_ERR_FLAG__RTC_HALTED == err )
        {
            pm_rtc_app_data.pm_rtc_power_reset_status = TRUE ;    
        }
    }
	
	pm_rtc_app_data.init_complete = TRUE;
}

pm_rtc_app_data_type* pm_rtc_app_get_data()
{
	return &pm_rtc_app_data;
}