#ifndef time_fn_map_H
#define time_fn_map_H

/*=============================================================================

       T I M E   S E R V I C E   TIME FUNCTION MAP FILE

GENERAL DESCRIPTION
  Initializes and maintains function pointers used in time code. These 
  function pointers will be populated based on a specific device. For example, 
  depending on which PMIC driver is used, the function pointers will be 
  populated based on the pmic model used.

REGIONAL FUNCTIONS
  time_fn_map_initialize_fp
    Initialize the function pointers

  time_fn_map_get_pmic_time
  time_fn_map_set_pmic_time
  time_fn_map_set_rtc_display_mode
  time_fn_map_get_rtc_display_mode
  time_fn_map_get_rtc_alarm
  time_fn_map_reset_rtc_alarm
  time_fn_map_is_rtc_valid
  time_fn_map_set_irq_handle
  time_fn_map_clear_irq
  time_fn_map_poll_time_for_boundary
  time_fn_map_rw_alarm

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  time_fn_map_initialize_fp() must be called after the PMIC initializes.


Copyright (c) 2009 - 2015 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/time/src/time_fn_map.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/15   abh     Changes for correctly handling ATS_PRIVATE bases.
02/14/11   ab      Featurizing the missing apis for 8660 target currently under
                   FEATURE_TIME_ALARM

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "time_types.h"
#include "ats_rtc.h"
#include "msg.h"
#include "err.h"
#include "fs_public.h"
#ifdef FEATURE_PMIC_RTC
#include "pmic.h"
#endif /*FEATURE_PMIC_RTC*/

/**
@mainpage Time Services Mapping Functions
Function Pointers used in time code*/ 


/**
@defgroup Time Services Mapping Functions
This section contains function pointers used in time code.
*/

/** 
@ingroup Time Function Pointers
@file time_fn_map.h
This file contains unction pointers used in time code.
*/

#ifndef FEATURE_BU_WITHOUT_PMIC

#include "pm.h"

#define ATS_RTC_SYNC_PMIC_RTC
#define TIME_TOD_SYNC_PMIC_RTC

#endif /*FEATURE_BU_WITHOUT_PMIC*/

#include "time_efs.h"

/*=============================================================================
                           Enumeration types
=============================================================================*/

/** Abstraction for PMIC RTC display mode */

#ifdef FEATURE_PMIC_RTC

#define ATS_RTC_DISP_MODE_12HR PM_RTC_12HR_MODE
#define ATS_RTC_DISP_MODE_24HR PM_RTC_24HR_MODE
#define ATS_RTC_DISP_MODE_INVALID PM_RTC_INVALID_MODE

#else /* FEATURE_PMIC_RTC */

#define ATS_RTC_DISP_MODE_12HR 0
#define ATS_RTC_DISP_MODE_24HR 1
#define ATS_RTC_DISP_MODE_INVALID 2

#endif /* FEATURE_PMIC_RTC */

#ifdef FEATURE_PMIC_RTC

#define ATS_PM_ERR_FLAG_SUCCESS PM_ERR_FLAG__SUCCESS
#define ATS_PM_ERR_FLAG_FAILURE 0x1

#else /* FEATURE_PMIC_RTC */

#define ATS_PM_ERR_FLAG_SUCCESS 0x0
#define ATS_PM_ERR_FLAG_FAILURE 0x1

#endif /* FEATURE_PMIC_RTC */

#ifdef FEATURE_PMIC_RTC

#define ATS_PM_RTC_SET_CMD PM_RTC_SET_CMD

#else

#define ATS_PM_RTC_SET_CMD 1

#endif


/** Function pointer structure type */
typedef struct 
{

  #ifdef FEATURE_PMIC_RTC
  /** To set display moded for RTC time of PMIC   */
  pm_err_flag_type(*time_fn_map_set_rtc_display_mode_fp)(pm_rtc_display_type);
  /** To get display moded for RTC time of PMIC   */
  pm_err_flag_type(*time_fn_map_get_rtc_display_mode_fp)(pm_rtc_display_type*);
  /** To send rw command to PMIC   */
  pm_err_flag_type (*time_fn_map_rtc_rw_cmd_fp)(pm_rtc_cmd_type,  pm_rtc_julian_type *);
  /** To verify validity of RTC time got from the PMIC */
  boolean (*time_fn_map_is_rtc_valid_fp)(pm_rtc_display_type,  const pm_rtc_julian_type *);  
  #endif /*FEATURE_PMIC_RTC*/
  
  int (*efs_get_fp) (const char *, void *, uint32 );
  int (*efs_put_fp) (const char *, void *, uint32, int, int );
  int (*efs_deltree_fp) (const char *);

  void (*time_server_call_ind_fp) (uint16, uint64);

}time_fn_map_fp_type;

/** Data structure mapping for FEATURE_PMIC_RTC */
typedef union
{
   uint32   display_mode;
   uint32   rtc_cmd;
}time_fn_map_ds_type;

/** 
@ingroup Time Function Pointers
         
@brief  
*Initializes the Time Function Pointers                                 \n\n

@param[in]                                                               
 time_fn_map_fp_type - Pointer to structure containing function pointer \n
                       to be initialized                                \n\n
 
@dependencies  
 None                                                                   \n\n  
 
@param[out]
 None                                                                   \n\n
 
@sideffects 
 None                                                                   \n\n

*/
void time_fn_map_initialize_fp
(
  void
);

/** 
@ingroup Time Function Pointers
         
@brief  
*Returns the time function pointer address               \n\n

@param[in]                                                               
 None                                                                   \n\n  
 
@dependencies  
 None                                                                   \n\n  
 
@param[out]
 time_fn_map_fp_type - Pointer to structure containing function pointer \n
                                                                          \n\n
@sideffects 
 None                                                                   \n\n

*/
time_fn_map_fp_type * time_fn_map_get_fp
(
  void
);


/** 
@ingroup Time Function Pointers
         
@brief  
*This function reads the pmic time                                      \n

@param[in]                                                               
  time_julian_type * - pointer to julian time for getting time          \n
 
@dependencies  
 None                                                                   \n\n  
 
@param[out]
 boolean - Whether the pmic time was got or not                         \n\n
 
@sideffects 
 None                                                                   \n\n

*/
boolean time_fn_map_get_pmic_time
(
  time_julian_type *julian
);

/** 
@ingroup Time Function Pointers
         
@brief  
*This function sets the pmic time                                       \n\n 

@param[in]                                                               
  time_julian_type * - pointer to julian time to be set                 \n
 
@dependencies  
 None                                                                   \n\n  
 
@param[out]
 boolean - Whether the pmic time is set or not                          \n\n
 
@sideffects 
 None                                                                   \n\n

*/
boolean time_fn_map_set_pmic_time
(
  time_julian_type *julian
);

/** 
@ingroup Time Function Pointers
         
@brief  
*This function sets RTC display mode                                   \n\n 

@param[in]                                                               
  time_fn_map_ds_type.display_mode                                      \n
  pm_rtc_display_type - RTC display type to be set                      \n
 
@dependencies  
 None                                                                   \n\n  
 
@param[out]
 boolean - If the rtc time display type was set or not                  \n\n
 
@sideffects 
 None                                                                   \n\n

*/
boolean time_fn_map_set_rtc_display_mode
(
  time_fn_map_ds_type rtc_display_mode
);

/** 
@ingroup Time Function Pointers
         
@brief  
*This function gets RTC display mode 
*\n\n 

@param[in]                                                               
  time_fn_map_ds_type.display_mode                                      \n
* 
@dependencies  
 None                                                                   \n\n  
 
@param[out]
 boolean - If the rtc time display type was set or not                  \n\n
 
@sideffects 
 None                                                                   \n\n

*/
boolean time_fn_map_get_rtc_display_mode
(
  time_fn_map_ds_type* rtc_display_mode
);

/** 
@ingroup Time Function Pointers
         
@brief  
*This function checks if the rtc time is valid or not                  \n\n 

@param[in]                                                               
  time_julian_type * - Pointer to julian time to be verified            \n
 
@dependencies  
 None                                                                   \n\n  
 
@param[out]
 boolean - If the rtc time was valid or not                             \n\n
 
@sideffects 
 None                                                                   \n\n

*/
boolean time_fn_map_is_rtc_valid
(
  time_julian_type *julian_ptr
);

/** 
@ingroup Time Function Pointers
         
@brief  
*This function reads the pmic time in a loop till a boundary is 
detected.                                                               \n\n 

@param[in]                                                               
 ats_rtc_status_type* - To return the rtc status                        \n
  boolean             - Is the time provided was a valid time
  time_julian_type    - To return julian time                           \n\n
 
@dependencies  
 None                                                                   \n\n  
 
@param[out]
 boolean - Whether a valid time was set or not                          \n\n
 
@sideffects 
 None                                                                   \n\n

*/
boolean time_fn_map_poll_time_for_boundary
(
  boolean* pmic_rtc_status,
  boolean is_time_valid,
  time_julian_type* julian
);

#endif /*time_fn_map_H*/
