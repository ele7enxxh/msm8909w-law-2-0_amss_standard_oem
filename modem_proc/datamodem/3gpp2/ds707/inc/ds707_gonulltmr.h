#ifndef DS707_GONULL_TMR_H
#define DS707_GONULL_TMR_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                  G O  N U L L  T I M E R  C O N T R O L
                       
GENERAL DESCRIPTION
  This file contains functions used to handle the abort dormant ppp timer. This 
  functionality is also known as the go null timer.  It aborts the dormant ppp 
  as soon as the go null timer expires. 

EXTERNALIZED FUNCTIONS
    
  DS707_GONULLTMR_SET_TIMER
    This function takes a GO NULL Timer value in minutes and sets the value
    of the actual timer in milli seconds

  DS707_GONULLTMR_GET_TIMER
    Returns the go null timer value in minutes.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 2004-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_gonulltmr.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/15/11   mg      Global variable cleanup
11/10/09   as      Cleared Windows Compiler warnings for MOB builds
11/18/08   sn      Set DS707_GONULLTIMER_VALUE_FOR_VOICE_MOSMS_INTERRUPTION to 
                   zero.
07/02/07   sc      Moved the def of macro GONULL_TIMERI_TIMER_DEFAULT from
                   ds707_gonulltmri.c to this file.
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
#include "customer.h"

#ifdef FEATURE_DATA_IS707

/*===========================================================================
                        TYPEDEFS AND VARIABLES
===========================================================================*/

#define DS707_GONULLTIMER_VALUE_FOR_VOICE_MOSMS_INTERRUPTION 0

#define MILLISECS_PER_SECOND (1000)


/* Enumerated type to define config parameters for timer. */
typedef enum
{
  DS707_GONULLTMRI_DO=1,
  DS707_GONULLTMRI_1X=2,
  DS707_GONULLTMRI_1X_AND_DO
} ds707_gonulltmri_config_enum_type;


/*===========================================================================
                         EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_GONULLTMR_SET_TIMER

DESCRIPTION   This function sets the go null timer value in minutes.

PARAMETERS    ds707_gonulltmri_config_enum_type: Use this enumerated type to
              configure either the 1x, DO or both timers with a specified value.

              int16: The new timer value to be set (in minutes) 

DEPENDENCIES  None

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
int ds707_gonulltmr_set_timer
(
  ds707_gonulltmri_config_enum_type,
  int32 
);

/*===========================================================================
FUNCTION      DS707_GONULLTMR_GET_TIMER

DESCRIPTION   This function returns the go null timer value in minutes.

PARAMETERS    ds707_gonulltmri_config_enum_type: Use this type to select
              which timer value to return(1x, DO or both). If both is selected
              then an error is returned. 

DEPENDENCIES  None

RETURN VALUE  Returns the go null timer value.

SIDE EFFECTS  None
===========================================================================*/
int16 ds707_gonulltmr_get_timer
(
  ds707_gonulltmri_config_enum_type
);

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
void ds707_gonulltmr_set_voice_mosms_interruption_timer_values(void);

/*===========================================================================
FUNCTION      DS707_GONULLTMR_RESTORE_TIMER_VALUES

DESCRIPTION   Restoring the values of go null timer that existed prior to
              the interruption of the voice or SMS call.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_gonulltmr_restore_timer_values(void);

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
);

#endif
#endif /* DS707_GONULL_TMRI_H */

