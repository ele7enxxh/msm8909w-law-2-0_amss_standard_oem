/*============================================================================
  FILE:         le_from_nf_trans.h

  OVERVIEW:     Temporary file to allow NF module to call LE-internal functions,
                used during the transition of most NF functionality to LE module
                outside MGP.
                To be obsolete once NF module fully integrated into LE module.

                This file should only be included by NF task modules, no other
                modules external to LE.

                Transition phase: GNSS/MGP/PE/NF -> GNSS/LE/inc/this_file
                Future: GNSS/MGP/PE/NF files will be moved to GNSS/LE directly
                  so they can directly access LE/src/*.h

  DEPENDENCIES: NA
 
                Copyright (c) 2015 QUALCOMM Atheros, Inc.
                All Rights Reserved.
                Qualcomm Atheros Confidential and Proprietary
               
                Copyright (c) 2015 Qualcomm Technologies, Inc. 
                All Rights Reserved.
                Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/


/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/le/inc/le_from_nf_trans.h#1 $ 
   $DateTime: 2016/12/13 07:59:45 $ 
   $Author: mplcsds1 $ 

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-03-24  wtr  new file for LE module (transition from nf to le)

============================================================================*/

#ifndef _LE_FROM_NF_TRANS_H
#define _LE_FROM_NF_TRANS_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "le_types.h" /* for le_WifiCovFixRequestType */

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*==============================================================================

  FUNCTION      le_InitWifiCovStatics()

  DESCRIPTION   Init all file-statics - on boot-up, and delete position

  DEPENDENCIES  No use of file or regional statics.
                No sequencing requirements.

  PARAMETERS    Description of the parameters to the functions

    none

  RETURN VALUE  None

  SIDE EFFECTS  lez_WifCovStatics is initialized

==============================================================================*/
void le_InitWifiCovStatics();

/*==============================================================================

  FUNCTION      le_InitWifiCovPosition()

  DESCRIPTION   Init all file-statics - on delete position (and called at boot 
                indirectly)

  DEPENDENCIES  No use of file or regional statics.
                No sequencing requirements.

  PARAMETERS    Description of the parameters to the functions

    none

  RETURN VALUE  None

  SIDE EFFECTS  lez_WifCovStatics.z_Wch is initialized

==============================================================================*/
void le_InitWifiCovPosition();

/*======================================================================

  FUNCTION 		  le_HandleWifiCovFix()

  DESCRIPTION 	Process the incoming IPC measurement with Wifi Coverage area
                fix request information, including measurements, assistance
                data, and controls including a possible request to send out
                and update to SM after doing the calculations.

  DEPENDENCIES  No use of file or regional statics.
                No sequencing requirements.

  PARAMETERS    Description of the parameters to the functions

    pz_WifiCovFix - incoming information to use

  RETURN VALUE  None

  SIDE EFFECTS  None

======================================================================*/
void le_HandleWifiCovFix(
    const le_WifiCovFixRequestType *pz_WifiCovFix);

#endif  /* _LE_FROM_NF_TRANS_H */
