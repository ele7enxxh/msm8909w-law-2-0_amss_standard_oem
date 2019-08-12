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
  time_fn_map_is_rtc_valid
  time_fn_map_poll_time_for_boundary

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  time_fn_map_initialize_fp() must be called after the PMIC initializes.


Copyright (c) 2009 - 2015 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/time/src/time_fn_map.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/15   abh     Changes for correctly handling ATS_PRIVATE bases.
02/14/11   ab      Featurizing the missing apis for 8660 target currently under
                   FEATURE_TIME_ALARM

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "dog.h"
#include "ats_v.h"
#include "time_fn_map.h"
#include <stringl/stringl.h>
#include "time_dal_v.h"

#ifdef FEATURE_QMI_TIME_REMOTE
#include "qmi_time_server.h"
#endif

/** Static definition for function pointers */
#ifdef FEATURE_PMIC_RTC
static time_fn_map_fp_type time_fn_map_fps;
#endif /*FEATURE_PMIC_RTC*/

/** 
@ingroup Time Function Pointers
         
@brief  
*Initializes the Time Function Pointers                                 \n\n

@param[in]                                                               
 None                                                                   \n\n  
 
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
)
{
  #ifdef FEATURE_PMIC_RTC
  time_fn_map_fps.time_fn_map_set_rtc_display_mode_fp= NULL;
  time_fn_map_fps.time_fn_map_get_rtc_display_mode_fp= NULL;
  time_fn_map_fps.time_fn_map_rtc_rw_cmd_fp= NULL;
  time_fn_map_fps.time_fn_map_is_rtc_valid_fp= NULL;
  #endif


  /*Only if PMIC is available, intialize all the fps*/
  #ifndef FEATURE_BU_WITHOUT_PMIC

  #ifdef FEATURE_PMIC_RTC
  if (time_defines.use_pmic_rtc)
    {
      time_fn_map_fps.time_fn_map_set_rtc_display_mode_fp = pm_set_rtc_display_mode;
      time_fn_map_fps.time_fn_map_get_rtc_display_mode_fp = pm_get_rtc_display_mode;
      time_fn_map_fps.time_fn_map_rtc_rw_cmd_fp = pm_rtc_rw_cmd;
      time_fn_map_fps.time_fn_map_is_rtc_valid_fp = pm_is_rtc_valid;
    }
  #endif /*FEATURE_PMIC_RTC*/

  #endif /*feature FEATURE_BU_WITHOUT_PMIC*/

  time_fn_map_fps.efs_get_fp = NULL;
  time_fn_map_fps.efs_put_fp = NULL;

  #ifndef FEATURE_TIME_NO_EFS
  if (time_defines.use_pmic_rtc)
    {
	  time_fn_map_fps.efs_get_fp = efs_get;
	  time_fn_map_fps.efs_put_fp = efs_put;
	  time_fn_map_fps.efs_deltree_fp = efs_deltree;
	}
  #endif /*FEATURE_TIME_NO_EFS*/

  #ifdef FEATURE_QMI_TIME_REMOTE
  time_fn_map_fps.time_server_call_ind_fp = time_server_call_ind;
  #endif /*FEATURE_QMI*/
}

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
)
{
  /* Return the address of the static structure */
  return (&time_fn_map_fps);
}


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
)
{
  boolean success = FALSE;
  #ifdef FEATURE_PMIC_RTC
  if(time_fn_map_fps.time_fn_map_rtc_rw_cmd_fp)
  {
    if(time_fn_map_fps.time_fn_map_rtc_rw_cmd_fp
       (PM_RTC_GET_CMD, (pm_rtc_julian_type*)(julian)) 
        == ATS_PM_ERR_FLAG_SUCCESS)
    {
      success = TRUE;
    }  
  }
  #endif

  return success;
}

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
)
{
  boolean success = FALSE;
  #ifdef FEATURE_PMIC_RTC
  if(time_fn_map_fps.time_fn_map_rtc_rw_cmd_fp)
  {
    if(time_fn_map_fps.time_fn_map_rtc_rw_cmd_fp
       (PM_RTC_SET_CMD,(pm_rtc_julian_type*)(julian)) 
        == ATS_PM_ERR_FLAG_SUCCESS)
    {
      success = TRUE;
    }  
  }
  #endif

  return success;
}

/** 
@ingroup Time Function Pointers
         
@brief  
*This function sets RTC display mode                                   \n\n 

@param[in]                                                               
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
)
{
  boolean success = FALSE;

  #ifdef FEATURE_PMIC_RTC
  if(time_fn_map_fps.time_fn_map_set_rtc_display_mode_fp)
  {
    if (time_fn_map_fps.time_fn_map_set_rtc_display_mode_fp
        ((pm_rtc_display_type)rtc_display_mode.display_mode) == ATS_PM_ERR_FLAG_SUCCESS)
    {
      success = TRUE;
    }
  }
  #endif
  return success;
}

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
)
{
   boolean success = FALSE;

  #ifdef FEATURE_PMIC_RTC
  if(time_fn_map_fps.time_fn_map_get_rtc_display_mode_fp)
  {
    if (time_fn_map_fps.time_fn_map_get_rtc_display_mode_fp
        ((pm_rtc_display_type *) &(rtc_display_mode->display_mode)) == ATS_PM_ERR_FLAG_SUCCESS)
    {
      success = TRUE;
    }
  }
  #endif
  return success;
}

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
)
{
   boolean success = FALSE;

  #ifdef FEATURE_PMIC_RTC
  if(time_fn_map_fps.time_fn_map_is_rtc_valid_fp)
  {
    success = time_fn_map_fps.time_fn_map_is_rtc_valid_fp
              ( PM_RTC_24HR_MODE, (pm_rtc_julian_type*)julian_ptr);  
    
  }
  #endif
  return success;
}

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
)
{
  /*for feature pmic rtc*/
  #ifdef FEATURE_PMIC_RTC

  /* Julian time/date from the PMIC's Real Time Clock, temp holder */
  //time_julian_type                temp_julian;
  
  if (is_time_valid == TRUE)
  {

/* Removing this part of polling code since its delaying the init routines. 
   We will need to move this part to some other task context than rcinit. 
   For now since there is not stringent requirement, we can ignore polling
   completely. If there comes a requirement we can move to some other task
   context or use some timers to do the polling.
*/
#if 0
    /* RTC has 1 sec resolution, so we could be off by a second.
       Some apps like GPS require RTC accuracy be to within 50 ms.
       So need to poll the PMIC RTC to catch the second boundary */

    /* Copy the first RTC returned time into a temporary structure */
    memscpy(&temp_julian, sizeof(time_julian_type), julian, sizeof(time_julian_type));

    /* Loop while the current RTC time is the same as the first
       capture. */
    while (memcmp(julian, &temp_julian, sizeof(time_julian_type)) == 0)
    {
      /* Retrieve the real time clock's Julian time/date from the PMIC */
      if (FALSE == time_fn_map_get_pmic_time(julian))
      {
         ATS_ERR("ATS_RTC: time_fn_map_get_pmic_time failed");
      }

       /* Wait 1 ms */
       DALSYS_BusyWait(1000);

       /* Kick dog */
       dog_kick();
     }
#endif /* #if 0 */     
  }  

  /* Set the RTC status to Valid */
  *pmic_rtc_status = TRUE;

  /* Check if time is valid. If not set default time */
  if ( !is_time_valid )
  {
    julian->year = 1980;
    julian->month = 1;
    julian->day = 6;
    julian->hour = 0;
    julian->minute = 0;
    julian->second = 0;
    julian->day_of_week = 6;

    /*looks like they want to try one last time*/
    if (FALSE == time_fn_map_set_pmic_time(julian))
    {
       ATS_ERR("RTC: Call to pm_rtc_rw_cmd failed");
    }

    /* Set the RTC status to Invalid */
    *pmic_rtc_status = FALSE;
    is_time_valid = TRUE;
  }

  return is_time_valid;

  /*in case of FEATURE_PM1000_RTC or an unsupported pmic, 
   don't modify pmic_rtc_status (since it should be
   already invalid and return whatever time valid flag
   was passed to us. This approach is adopted so that
   this function can be called irrespective of which
   PMIC feature is defined.*/
  #else 
  return is_time_valid;
  #endif /* FEATURE_PM1000_RTC | FEATURE_PMIC_RTC */
}

