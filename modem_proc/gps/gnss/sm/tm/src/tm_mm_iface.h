#ifndef TM_MM_IFACE_H
#define TM_MM_IFACE_H


/*===========================================================================
  
                            TM_MM_IFACE
                   
DESCRIPTION:
  This header file contains TM MM interface related constants, variables
  and function protocols.

  
  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_mm_iface.h#1 $

  when        who     what, where, why
  --------    ---     ----------------------------------------------------------
  05/30/07    lt      Mods. to make TM's RRC and MM iface symmetrical to CM and RR. 
  05/30/07    lt      Initial check in.
===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"

#ifdef FEATURE_CGPS_UMTS_CP


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                               DEFINES                                   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                              DATA TYPES                                 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                              FUNCTION DECLARATIONS                      */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*===========================================================================

FUNCTION tm_mm_iface_proc_event

DESCRIPTION
  This function is used by TM to process an MM Event Message.  This function
  runs within the context of the SM_TM task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void tm_mm_iface_proc_event( void *p_msg );


/*===========================================================================

FUNCTION tm_mm_iface_init

DESCRIPTION
  This function initializes the TM-MM IFACE module.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void tm_mm_iface_init( void );

#endif /* FEATURE_CGPS_UMTS_CP */

#endif /* TM_MM_IFACE_H */

