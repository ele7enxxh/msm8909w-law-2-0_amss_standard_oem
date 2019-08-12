#ifndef TM_L1_IFACE_H
#define TM_L1_IFACE_H


/*===========================================================================
  
                            TM_L1_IFACE
                   
DESCRIPTION:
  This header file contains TM L1 interface related constants, variables
  and function protocols.

  
  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_l1_iface.h#1 $

  when        who     what, where, why
  --------    ---     ------------------------------------------------------
  06/09/15    mj      Add support for W+W feature
04/22/14      ssu     API to request WCDMA cell measurements.
  07/17/07    lt      Initial check-in.

===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"

#ifdef FEATURE_USER_PLANE_MRL 


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

FUNCTION tm_l1_iface_proc_event

DESCRIPTION
  This function is used by TM to process an L1 Event Message.  This function
  runs within the context of the SM_TM task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void tm_l1_iface_proc_event( void *p_msg );


#ifdef FEATURE_CGPS_LTE_CELLDB

/*===========================================================================

FUNCTION tm_lte_l1_iface_proc_event

DESCRIPTION
  This function is used by TM to process an LTE L1 Event Message. This function
  runs within the context of the SM_TM task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void tm_lte_l1_iface_proc_event( void *p_msg );

#endif /* SUPL2 + LTE_CELL_DB */


/*===========================================================================

FUNCTION tm_l1_iface_init

DESCRIPTION
  This function initializes the TM-L1 IFACE module.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void tm_l1_iface_init( void );


#ifdef FEATURE_CGPS_WCDMA
#ifdef FEATURE_CGPS_DUAL_WCDMA
/*===========================================================================

FUNCTION
  tm_l1_iface_send_wcdma_cell_meas_req

DESCRIPTION
  This function requests cell measurement data from WCDMA L1.

PARAMETERS
   sys_modem_as_id_e_type. Specifies the sub id to which the request must be sent

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None.

===========================================================================*/

void tm_l1_iface_send_wcdma_cell_meas_req(sys_modem_as_id_e_type as_id);
#else
/*===========================================================================

FUNCTION
  tm_l1_iface_send_wcdma_cell_meas_req

DESCRIPTION
  This function requests cell measurement data from WCDMA L1.

PARAMETERS
   none

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None.

===========================================================================*/

void tm_l1_iface_send_wcdma_cell_meas_req(void);
#endif // FEATURE_CGPS_DUAL_WCDMA

#endif //FEATURE_CGPS_WCDMA


#endif /* FEATURE_USER_PLANE_MRL  */

#endif /* TM_L1_IFACE_H */

