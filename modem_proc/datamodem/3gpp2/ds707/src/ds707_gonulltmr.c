/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                  G O  N U L L  T I M E R  C O N T R O L
                       
GENERAL DESCRIPTION
  This file contains functions used to handle the abort dormant ppp timer. This 
  functionality is also known as the go null timer.  It aborts the dormant ppp 
  as soon as the go null timer expires. 

EXTERNALIZED FUNCTIONS

  DS707_GONULLTMR_SET_TIMER
    This function sets the value of go null timer to the input value
    
  DS707_GONULLTMR_GET_TIMER
    Returns the go null timer value in minutes.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 2004-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_gonulltmr.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/18/12   jz      Seperate old JCDMA and Simplified JCDMA 
08/11/11   var     Resolve Compiler warnings & KW errors.
03/15/11   mg      Global variable cleanup
11/10/09   as      Cleared Windows Compiler warnings for MOB builds
11/18/08   sn      Moved DS707_GONULLTIMER_VALUE_FOR_VOICE_MOSMS_INTERRUPTION 
                   to header file, added comments and some cleanup.
03/08/07   ara     Changed FEATURE_JCDMA_2 to FEATURE_JCDMA_1X 
11/14/06   rsj     - Created a new configuration enumerated type 
                   ds707_gonulltmri_config_enum_type to select
                   either 1x or DO timer values to be configured.
                   - Modified Timer primitives for set and get to allow
                   two sepparate values for 1x and DO to be used.
03/18/04   gr      Initial creation of the module
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "ds3gtimer.h"
#include "msg.h"
#include "data_msg.h"
#include "ds707_gonulltmri.h"
#include "ds707_gonulltmr.h"
#include "ds707_roaming.h"
#include "ds707_extif.h"

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/* These variables track the old values of DO and 1X go null timer during 
   interruption by voice call or MO SMS.

   Scenario: Data call is going on on HDR.
             Mobile originated(MO) Voice call or MO SMS comes in.
             This interrupts the Data call because we need to stop and 
             release HDR session and go on 1X.
             At this point we want different Go NULL timer values for HDR.
             Hence these variables are used to track previous values that 
             existed prior to this interruption so that we can restore
             back those original values once voice call or MO sms is over.
*/
#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
static int16 ds707_gonulltmri_saved_do_val;
static int16 ds707_gonulltmri_saved_1x_val;
#endif

static uint32 ds707_gonulltmri_do_val;
static uint32 ds707_gonulltmri_1x_val;

/*
   This variable indicates if VOICE MOSMS interruption handling is enabled or 
   disabled. TRUE indicates enabled and GO NULL TIMER value will be set to 
   VOICE MOSMS interruption timer value which is a larger value compared to 
   default GO NULL timer value. FALSE indicates disabled and GO NULL TIMER 
   value will not be changed. Currently, handling of VOICE MOSMS interruption 
   is disabled and so this variable is set to FALSE.
*/
#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
static boolean ds707_voice_mosms_interruption_mode_enabled = FALSE;
#endif

/*===========================================================================
                        INTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================
                         EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_GONULLTMR_SET_TIMER_BY_AN

DESCRIPTION   This is a function to set the ABORT_DORMANT_PPP timer 
              dynamically. gonull_timer must be specified in minutes
             

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_gonulltmr_set_timer_by_an
(
  uint32 gonull_timer /* timer value in ms */
)
{ 
  uint32 gonull_timer_seconds;

  /* Convert the gonull_timer into minutes to set the global timer values.
   * This conversion is approximate
   */
  gonull_timer_seconds = (gonull_timer / MILLISECS_PER_SECOND);  
  
  if(ds707_extif_is_pkt_data_sess_on_cdma())
  {
	/*Simplified JCDMA:
	  5.1.6 Use PPP close timer for socket calls  [D1-No.90]*/
    if(ds707_roaming_is_curr_mode_jcdma() ||
	   ds707_roaming_is_simplified_jcdma_mode())
    {
      ds707_gonulltmri_1x_val = gonull_timer_seconds;
    }
    else
    {
      ds707_set_gonulltmri_val(gonull_timer_seconds);
    }
  }
  else if(ds707_extif_is_pkt_data_sess_on_hdr())
  {
	/*Simplified JCDMA:
	  5.1.6 Use PPP close timer for socket calls  [D1-No.90]*/
    if(ds707_roaming_is_curr_mode_jcdma() ||
	   ds707_roaming_is_simplified_jcdma_mode())
    {
      ds707_gonulltmri_do_val = gonull_timer_seconds;
    }
    else
    {
      ds707_set_gonulltmri_val(gonull_timer_seconds);
    }
  }
  else 
  {
    /* We should not get here */
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, 
              "The Current Data Session is not HDR or 1X. Not setting timer.");
  }  

  /* If timer is already running, need to restart it with the new value */
  if (ds707_get_gonulltmri_is_running_flag() && (gonull_timer > 0))
  {
    ds3gi_start_timer(DS3G_TIMER_ABORT_DORMANT_PPP, gonull_timer);    
  }
}

/*===========================================================================
FUNCTION      DS707_GONULLTMR_SET_TIMER

DESCRIPTION   This function sets the go null timer value in seconds. 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
int ds707_gonulltmr_set_timer
(
  /*Specifies which timer values to set(1x, DO or both)*/
  ds707_gonulltmri_config_enum_type gonull_config, 
  /*Specified the time in seconds to set the specified timer to*/
  int32 gonull_timer
)
{

  /* Set the gonull timer value */
  if(gonull_config == DS707_GONULLTMRI_1X)
  {
    ds707_gonulltmri_1x_val = gonull_timer;

    #ifdef FEATURE_DEDICATED_C2K_MODE_DATA
    ds707_gonulltmri_saved_1x_val = gonull_timer;
    #endif
  }
  else if(gonull_config == DS707_GONULLTMRI_DO)
  {
    ds707_gonulltmri_do_val = gonull_timer;

    #ifdef FEATURE_DEDICATED_C2K_MODE_DATA
    ds707_gonulltmri_saved_do_val = gonull_timer;
    #endif
  }
  else if(gonull_config == DS707_GONULLTMRI_1X_AND_DO)
  {
    ds707_gonulltmri_do_val = gonull_timer;
    ds707_gonulltmri_1x_val = gonull_timer;

    #ifdef FEATURE_DEDICATED_C2K_MODE_DATA
    ds707_gonulltmri_saved_do_val = gonull_timer;
    ds707_gonulltmri_saved_1x_val = gonull_timer;
    #endif
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_MED, "Invalid GO NULL Timer configuration specified : %d", gonull_config);
    return -1;
  }

  DATA_IS707_MSG2 (MSG_LEGACY_MED, "GO NULL Timer values (in seconds): DO = %d, 1x = %d ", 
                   ds707_gonulltmri_do_val, ds707_gonulltmri_1x_val);
  return 0;
  
}

/*===========================================================================
FUNCTION      DS707_GONULLTMR_GET_TIMER

DESCRIPTION   This function returns the go null timer value in minutes.

DEPENDENCIES  None

RETURN VALUE  Returns the gonull timer value in seconds.
              Returns -1 on error

SIDE EFFECTS  None
===========================================================================*/
int16 ds707_gonulltmr_get_timer
( 
  /*Specifies which timer values to get(1x, DO)*/
  ds707_gonulltmri_config_enum_type gonull_config
)
{
  if(gonull_config == DS707_GONULLTMRI_1X)
  {
    DATA_IS707_MSG1(MSG_LEGACY_MED, 
        "Getting 1X GO NULL Timer value: %d seconds", ds707_gonulltmri_1x_val);
    return (int16)ds707_gonulltmri_1x_val;
  }
  else if(gonull_config == DS707_GONULLTMRI_DO)
  {
    DATA_IS707_MSG1(MSG_LEGACY_MED, 
            "Getting DO GO NULL Timer as %d seconds", ds707_gonulltmri_do_val);
    return (int16)ds707_gonulltmri_do_val;
  }
  else
  {
    DATA_IS707_MSG1(MSG_LEGACY_MED, 
             "Bad GO NULL Timer config parameter specified %d", gonull_config);
    return -1;
  }
}

/*===========================================================================
FUNCTION      DS707_GONULLTMR_SET_VOICE_MOSMS_INTERRUPTION_TIMER_VALUES

DESCRIPTION   When Data call on HDR is interrupted by a voice call or SMS,
              the go null timer value is changed to larger values.

              Later when the SMS or voice call is over, the go null timer
              values are restored back.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_gonulltmr_set_voice_mosms_interruption_timer_values(void)
{
#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
  if (!ds707_voice_mosms_interruption_mode_enabled)
  {

    ds707_gonulltmri_saved_do_val = ds707_gonulltmri_do_val;
    ds707_gonulltmri_saved_1x_val = ds707_gonulltmri_1x_val;

    ds707_gonulltmri_do_val = DS707_GONULLTIMER_VALUE_FOR_VOICE_MOSMS_INTERRUPTION;
    ds707_gonulltmri_1x_val = DS707_GONULLTIMER_VALUE_FOR_VOICE_MOSMS_INTERRUPTION;

    ds707_voice_mosms_interruption_mode_enabled = TRUE;

    DATA_IS707_MSG2(MSG_LEGACY_MED, "Archiving previous Go Null timers DO %d, 1X %d to restore later",
               ds707_gonulltmri_saved_do_val, ds707_gonulltmri_saved_1x_val);
    DATA_IS707_MSG2(MSG_LEGACY_MED, "GO NULL timers now set to DO %d, 1X %d",
               ds707_gonulltmri_do_val,
               ds707_gonulltmri_1x_val);
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "No need to SET Go Null Timers. Already in Voice, MO SMS interruption mode");
  }
#endif
 
}

/*===========================================================================
FUNCTION      DS707_GONULLTMR_RESTORE_TIMER_VALUES

DESCRIPTION   Restoring the values of go null timer that existed prior to
              the interruption of the voice or SMS call.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_gonulltmr_restore_timer_values(void)
{
#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
  if (ds707_voice_mosms_interruption_mode_enabled)
  {
    DATA_IS707_MSG2(MSG_LEGACY_MED, "Restore Go Null timers to DO %d, 1X %d",
               ds707_gonulltmri_saved_do_val,
               ds707_gonulltmri_saved_1x_val);

    DATA_IS707_MSG2(MSG_LEGACY_MED, "Go Null timers before restoration were DO %d, 1X %d",
               ds707_gonulltmri_do_val,
               ds707_gonulltmri_1x_val);

    ds707_gonulltmri_do_val = ds707_gonulltmri_saved_do_val;
    ds707_gonulltmri_1x_val = ds707_gonulltmri_saved_1x_val;

    ds707_voice_mosms_interruption_mode_enabled = FALSE;
    DATA_IS707_MSG0(MSG_LEGACY_MED, "Voice or MO SMS interruption mode disabled");
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_MED, "No need to restore Go Null Timers. Not in Voice, MO SMS interruption mode");
  }
#endif
}
