#ifndef WCDMADIAGIF_H
#define WCDMADIAGIF_H
/*===========================================================================
                 WCDMA DIAG Interface

GENERAL DESCRIPTION
  This file contains the code for exporting DIAG apis outside of modem.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/public/wcdmadiagif.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/01/09   scm     Remove nested comment.
10/07/09   rm      Initial Release

===========================================================================*/

/*===========================================================================
FUNCTION        WL1_GET_FRAME_NUMBER_FOR_DIAG

DESCRIPTION     This function returns the frame number for diag

PARAMETER       NONE

RETURN VALUE    uint32 value.
===========================================================================*/
extern uint32 wl1_get_frame_number_for_diag(void);


#endif /* WCDMADIAGIF_H */
