#ifndef DS707_SO_RETRYDEL
#define DS707_SO_RETRYDEL
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       S O  R E T R Y  D E L A Y  

GENERAL DESCRIPTION
  This file contains functions that are used to implement the retry delay
  processing that is part of IS2000.  Retry delays apply to all pkt 
  service options.
  
EXTERNALIZED FUNCTIONS
  DS707_SO_RETRYDEL_INIT
    Called at startup.  Initializes delay to zero (i.e., not
    delayed).
    
  DS707_SO_RETRYDEL_SET_DELAY
    Sets the delay for packet calls.  The delay is stored as the time in
    the future at which the phone is allowed to re-orig with a packet
    data calls.
    
  DS707_SO_RETRYDEL_CLR_DELAY
    Clears the retry delay for packet data calls.  The delay is set back
    to zero.
    
  DS707_SO_RETRYDEL_CLR_ALL_DELAYS
    Clears retry delay for packet data calls.
    
  DS707_SO_RETRYDEL_SO_IS_DELAYED
    Returns TRUE or FALSE, depending on if packet data calls are 
    delayed.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  Must call ds707_so_retrydel_init() at startup.

 Copyright (c) 2001 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_so_retrydel.h_v   1.2   21 Jan 2003 22:25:06   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_so_retrydel.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
01/13/03   ak         Updated to now have retry delay apply to all service
                      options, instead of being per service option.
11/17/02   ak         Updated file header comments.
03/30/01   ak         Initial revision.
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
/*---------------------------------------------------------------------------
  Struct is used for 707 data when receive a retry delay from BS
  for DS_707_RETRY_DELAY_ORIG_CMD.
---------------------------------------------------------------------------*/
typedef struct
{
  boolean delay_infinite;                  /*delay until cancel          */
  uint32 delay;                            /*delay in msec second         */
}ds707_retry_delay_orig_type;

/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_SO_RETRYDEL_INIT
 
DESCRIPTION   Initializes the retry delay.  Called only at powerup.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_retrydel_init(void);

/*===========================================================================
FUNCTION      DS707_SO_RETRYDEL_SET_DELAY
 
DESCRIPTION   Sets the delay for all pkt data calls.  If the delay is
              infinite, then the expiration_time is set to all F's (hex).

DEPENDENCIES  None.  

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_retrydel_set_delay
(
  boolean        delay_infinite,            /* is the delay infinite       */
  uint32         delay                      /* the delay in msec from now  */
);

/*===========================================================================
FUNCTION      DS707_SO_RETRYDEL_CLR_DELAY
 
DESCRIPTION   Clears the retry delay for pkt data so's.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_retrydel_clr_delay(void);

/*===========================================================================
FUNCTION      DS707_SO_RETRYDEL_IS_DELAYED
 
DESCRIPTION   Returns TRUE or FALSE, indicating if packet data calls are 
              being blocked/delayed by the base station.

DEPENDENCIES  None.

RETURN VALUE  TRUE = cannot originate pkt data right now, since the base
                     station has delayed it.
              FALSE= can originate pkt data now.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_so_retrydel_is_delayed(void);

#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_SO_RETRYDEL */

