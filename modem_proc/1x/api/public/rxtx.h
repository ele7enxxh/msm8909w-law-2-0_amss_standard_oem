#ifndef RXTX_H
#define RXTX_H
/*===========================================================================

                   R X T X   H E A D E R    F I L E

DESCRIPTION
  This file contains all of the definitions necessary to interface with
  the rxtx (layer 2) task.

  Copyright (c) 1991 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/rxtx.h_v   1.2.2.0   30 Nov 2001 17:48:54   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/rxtx.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/04/09   adw     Moved task start functionality to public header.
03/04/09   adw     Removed the temporary _v.h include.
12/02/08   adw     Removed duplicate declarations already included in rxtx_v.h.
12/01/08   adw     Temporarily include the internal _v.h file.
03/09/06   fc      Mainlined FEATURE_IS2000.
01/20/06   fc      Merged the following :
           fh      Added support for AKA and MI.
10/29/04   pg      Added support for bypass 2 good frames at TCI state.
03/02/01   va      Merged the following from the common archive:
  02/26/01 mai     Changed FEATURE_GPSONE to FEATURE_GPS_MODE to move code needed
                   only for GPS part of position location to a new #define.
02/28/01   lh      Changed length field from word to dword.
10/23/00   yll     Added RXTX_REJECT_S to reject the MC's START_GPS_F request
06/19/00   yll     Added support for GPS: RXTX_START_GPS_F
03/06/00   ry      Added IS2000 featurization
11/15/99   va      Added new interface for HO RXTX_HO_F with reset_l2 and 
                   encrypt mode as payload. 
10/02/98   pms     Backed out nearing of variables
10/02/98   pms     Neared variables to save ROM space
08/03/95   dna     Support for message encryption
12/22/93   jca     Modified for IS-95 Phase 1 upgrade.
07/23/92   jai     Modified file for LINT and linking.
07/09/92   jai     First DMSS release
09/11/91   jai     Created file. 

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"


/* <EJECT> */
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION RXTX_TASK

DESCRIPTION
  This task performs layer 2 processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void rxtx_task
(
  dword dummy
    /* Required for REX, ignore */
);


#endif /* RXTX_H */
