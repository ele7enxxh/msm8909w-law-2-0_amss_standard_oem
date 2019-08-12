#ifndef TM_RR_IFACE_H
#define TM_RR_IFACE_H


/*===========================================================================
  
                            TM_RR_IFACE
                   
DESCRIPTION:
  This header file describes the API used by GSM RR to send events and data
  to CGPS.

  
  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_rr_iface.h#1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
05/08/07    kgj     Initial Release
===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"

#ifdef FEATURE_CGPS_UMTS_CP_GSM

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

FUNCTION tm_rr_iface_init

DESCRIPTION
  This function initializes the TM-RR IFACE module.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tm_rr_iface_init( void );

/*===========================================================================

FUNCTION tm_rr_iface_proc_event

DESCRIPTION
  This function is called by TM Core when it receives an IPC message
  from RR.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tm_rr_iface_proc_event( void * p_msg );

#endif /* FEATURE_CGPS_UMTS_CP_GSM */

#endif /* TM_RR_IFACE_H */

