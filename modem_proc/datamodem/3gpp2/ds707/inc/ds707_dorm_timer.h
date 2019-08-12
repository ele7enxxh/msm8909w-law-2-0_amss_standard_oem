#ifndef DS707_DORM_TIMER_H
#define DS707_DORM_TIMER_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                  D O R M A N T  T I M E R  C O N T R O L

GENERAL DESCRIPTION
  This file contains functions used to handle the dormant timer.  This 
  functionality is also known as the holddown timer.  It prevents a pkt call
  from re-originiating out of dormancy for a time specified by the base
  station.

EXTERNALIZED FUNCTIONS
  DS707_DORM_TIMER_PARSE_SOCM
    Called when a service-option control message is received, requiring the
    dormant timer value to be changed.
    
  DS707_DORM_TIMER_RESET_VAL
    Called when a data session goes null in JCDMA or the SID has changed.
    The holddown timer is set back to it's default value.
    
  DS707_DORM_TIMER_GET_TIMER
    Returns the dormant timer value.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2002 - 2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_dorm_timer.h_v   1.1   15 Nov 2002 21:36:16   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_dorm_timer.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/02/11   msh     Support for posting SOCM DORM TIMER CMD 
03/09/09   sa      AU level CMI modifications.
02/04/04   ak      Updated to use pkt mgr event mechanisms.
11/15/02   ak      Updated file header comments.

===========================================================================*/

/*===========================================================================
                       INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "dstask_v.h"
#include "snm.h"

/*===========================================================================
                        TYPEDEFS AND VARIABLES
===========================================================================*/
/*---------------------------------------------------------------------------
  Type for the Service Option Control Message for Packet Data Dormant Timer
  Control.
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  byte  cntl;          /* Dormant Timer Control & Field Type               */
  byte  dorm_time;     /* Packet Data Dormant Timer value (in 1/10 sec)    */
} ds707_dorm_timer_socm_msg_type;

/*===========================================================================
                                 EXTERNS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_DORM_TIMER_PARSE_SOCM

DESCRIPTION   This function parses the service option control message.

DEPENDENCIES  None

RETURN VALUE  Returns indication if message was okay or not okay.

SIDE EFFECTS  None
===========================================================================*/
snm_socm_return_type ds707_dorm_timer_parse_socm
(
  ds707_dorm_timer_socm_msg_type  *dorm_socm_msg   /* SOCM with Dorm Timer info  */
);

/*===========================================================================
FUNCTION      DS707_DORM_TIMER_ENABLE

DESCRIPTION   Enable or disable dormancy timer

DEPENDENCIES  If multiple calls of this function are made, principle used
              is last caller wins.

              Input TRUE  - use the holddown timer when going dormant.
                    FALSE - do not use the holddown timer when dormant.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds707_dorm_timer_enable
(
  boolean is_enabled
);

/*===========================================================================
FUNCTION      DS707_DORM_TIMER_INIT

DESCRIPTION   Called at startup

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_dorm_timer_init(void);

/*===========================================================================
FUNCTION      DS707_DORM_TIMER_PROCESS_CMD

DESCRIPTION   Processes the dormant timer commands.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_dorm_timer_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
);


#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_DORM_TIMER_H */

