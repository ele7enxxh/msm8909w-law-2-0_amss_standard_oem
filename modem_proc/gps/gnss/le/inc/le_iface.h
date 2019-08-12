/*============================================================================
  FILE:         le_iface.h

  OVERVIEW:     Input API structure definitions and function 
                prototypes for LE  module, the place where NF task is 
                migrating.

  DEPENDENCIES: NA
 
                Copyright (c) 2015 QUALCOMM Atheros, Inc.
                All Rights Reserved.
                Qualcomm Atheros Confidential and Proprietary
============================================================================*/


/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/le/inc/le_iface.h#1 $ 
   $DateTime: 2016/12/13 07:59:45 $ 
   $Author: mplcsds1 $ 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-03-16  wtr  new file for LE module (transition from nf_iface.h)

============================================================================*/

#ifndef _LE_IFACE_H
#define _LE_IFACE_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "com_dtypes.h" // for boolean
#include "le_types.h" // for structs 
#include "gnss_common.h" // for gnss_PdrCtrlSrcEnumType

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/


/*======================================================================

  FUNCTION 		  le_WifiCovMeasAdPut()

  DESCRIPTION 	 Send to LE that a new WiFi Measurement & Assistance 
                 Data package for consumption to update location 
                 estimation, and send out (if reqeusted.)

  DEPENDENCIES  No use of file or regional statics.
                No sequencing requirements.

  PARAMETERS      Description of the parameters to the functions

    pz_WifiCovFix - data to pack into IPC to send

  RETURN VALUE   True if input message was successfully enqueued

  SIDE EFFECTS   None

======================================================================*/
boolean le_WifiCovMeasAdPut(const le_WifiCovFixRequestType *pz_WifiCovFix);

/*==============================================================================

  FUNCTION      le_InjectTlePosition

  DESCRIPTION 	Inject TLE position to ALE (via MGP, & PE to NF for now)

  DEPENDENCIES  None

  PARAMETERS    pz_TlePos: input, pointer to a TLE position

  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void le_InjectTlePosition( const ale_CellDBInputPosStructType *pz_TlePos );

/*==============================================================================

  FUNCTION      le_InjectPdrControl

  DESCRIPTION 	Inject PDR control to NF(via MGP & PE per modularity req's)

  DEPENDENCIES  None

  PARAMETERS    
    b_PdrDisableFlag: PDR control config, TRUE if feature is 
                          disabled, FALSE otherwise
    e_PdrCtrlSrc: Source of the config, build-config, QMI or NV

  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
boolean le_InjectPdrControl
(
  boolean b_PdrDisableFlag,
  gnss_PdrCtrlSrcEnumType e_PdrCtrlSrc
);


#endif  /* _LE_IFACE_H */
