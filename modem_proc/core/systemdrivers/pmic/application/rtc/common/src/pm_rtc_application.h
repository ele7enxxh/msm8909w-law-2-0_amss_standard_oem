#ifndef PM_RTC_APPLICATION_H
#define PM_RTC_APPLICATION_H

/*! \file 
*  \n
*  \brief  pm_rtc_application.c ---- 
*  \details This header file contains class declarations of the RTCAPP class.
*  \n
*  \n &copy; 2010 Qualcomm Technologies Incorporated, All Rights Reserved
*/
/*===========================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/application/rtc/common/src/pm_rtc_application.h#1 $

when		who			what, where, why
--------	---			----------------------------------------------------------
07/15/2010   henryw     Creation of RTC Application
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_rtc.h"
#include "pmapp_rtc.h"
#include "pm_err_flags.h"
#include "pm_malloc.h"

/*===========================================================================

                     STRUCTURE TYPE AND ENUM

===========================================================================*/
typedef struct pm_rtc_app_data_type
{
	boolean init_complete;

    pm_rtc_display_type pm_app_rtc_current_display_mode;
    boolean pm_rtc_power_reset_status;
    word PM_RTC_MIN_YEAR;
    word PM_RTC_MAX_YEAR;
    word PM_RTC_MAX_MONTH;
    word PM_RTC_MAX_DOW;
    word PM_RTC_MAX_DAY;
    word PM_RTC_MAX_24HR_HOUR;
    word PM_RTC_MAX_12HR_HOUR;
    word PM_RTC_MIN_12HR_HOUR;
    word PM_RTC_MIDL_12HR_HOUR;
    word PM_RTC_MIDH_12HR_HOUR;
    word PM_RTC_MAX_MINUTES;
    word PM_RTC_MAX_SECONDS;
    uint16 PM_RTC_BASE_YEAR;
    uint16 PM_RTC_QUAD_YEAR;
    uint32 RM_RTC_CLK_OFFSET_S;
    uint16* PM_RTC_LEAP_MONTH_TAB;
    uint16* PM_RTC_NORMAL_MONTH_TAB;
    uint16* PM_RTC_YEAR_TAB;
}pm_rtc_app_data_type;

/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/
void pm_rtc_app_init(void);

pm_rtc_app_data_type* pm_rtc_app_get_data(void);

#endif //PM_RTC_APPLICATION_H

