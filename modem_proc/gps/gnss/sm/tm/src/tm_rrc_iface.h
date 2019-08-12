#ifndef TM_RRC_IFACE_H
#define TM_RRC_IFACE_H


/*===========================================================================
  
                            TM_RRC_IFACE
                   
DESCRIPTION:
  This header file contains TM RRC interface related constants, variables
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_rrc_iface.h#1 $

  when        who     what, where, why
  --------    ---     ----------------------------------------------------------
  05/30/07    lt      Mods. to make TM's RRC and MM iface symmetrical to CM and RR. 
  05/30/07    lt      Initial check in.
===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"
 
#ifdef FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH /* this would be defined in custwcdma.h */
#undef FEATURE_WCDMA_GPS_CELL_ID_READING /* this would be defined in custcgps.h */
#endif
	 


#if defined (FEATURE_CGPS_UMTS_CP_WCDMA) || defined ( FEATURE_WCDMA_GPS_CELL_ID_READING ) || defined (FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH)
#include "rrclsmif.h"

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

FUNCTION tm_rrc_iface_proc_event

DESCRIPTION
  This function is used by TM to process an RRC Event Message.  This function
  runs within the context of the SM_TM task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void tm_rrc_iface_proc_event( void *p_msg );


/*===========================================================================

FUNCTION tm_rrc_iface_init

DESCRIPTION
  This function initializes the TM-RRC IFACE module.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void tm_rrc_iface_init( void );

/*===========================================================================

FUNCTION
  tm_rrc_iface_cell_info_dch_report

DESCRIPTION
  This function receives RRC response to cell id request sent by TM.

   Parameters:
    Cell ID information: 
    rrcmeas_plmn_cell_info_struct_type cell_id_info
	Status: (RRC_NEW_CELL_ID_AVAIL/RRC_NO_CELL_ID_AVAIL)  
    rrc_cell_id_status_type            status 

DEPENDENCIES
   None.

RETURN VALUE
   None

SIDE EFFECTS
   None.

===========================================================================*/
void tm_rrc_iface_cell_info_dch_report(
    rrcmeas_plmn_cell_info_struct_type cell_id_info,
    rrc_cell_id_status_type            status );

#endif /* FEATURE_CGPS_UMTS_CP_WCDMA */

#endif /* TM_RRC_IFACE_H */

