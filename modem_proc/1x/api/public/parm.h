#ifndef PARM_H
#define PARM_H
/*===========================================================================

         M O B I L E    C A I    P A R A M E T E R    S E R V I C E S
                        H E A D E R    F I L E

DESCRIPTION
  This module contains variables necessary to interface with the
  Mobile CAI parameter services.

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

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/parm.h_v   1.1   01 Oct 2002 18:47:18   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/parm.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/09   jj      CMI phase2 changes.
03/04/09   adw     Removed the temporary _v.h include.
12/01/08   adw     Temporarily include the internal _v.h file.
11/21/08   adw     Merge from CMI sandbox, based on VU_MODEM_1X_CP.07.02.09
08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
10/01/02   fc      Added BCCH, FCCCH and EACH BA mode parameter support.
06/28/00   lad     Introduce backward compatibility flag that, when enabled,
                   forces PARM DIAG support to use the old DIAG V7 method.
                   The feature is called FEATURE_DIAG_V7_PKT_IFACE.
06/19/00   lad     Removed unused parm_type and legacy DIAG support prototypes
03/30/99   ram     Verified and check-ed in changes made for MDR
                   DM support by aaj. parm_diag_get now always requires
                   10 parameters to accomodate the changes for MDR
                   support.
02/26/99   aaj     Modified parm_fer_type struct to include MDR sup channels
07/16/96   jmk     Split count of TC parameters into MUX1 and MUX2 counts.
02/08/96   jca     Put ()s around macros to make Lint happy.
08/16/95   jmk     Modified parm_diag_get for MUX2 parms.
08/14/95   gb      Added MUX2 parameter support.
01/02/92   jai     Created file.

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "cai.h"

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Frame error rate get type */
typedef struct
{
  uint32 total_frames;
    /* total number of frames received on the forward traffic channel */
  uint32 bad_frames;
    /* total number of category 9 and 10 frames received */
  dword total_sup_frames[CAI_MAX_NUM_SUP]; /* total frames rcd */
  dword bad_sup_frames[CAI_MAX_NUM_SUP]; /* erasure sup frames */
} parm_fer_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* <EJECT> */
/*===========================================================================

FUNCTION PARM_GET_FER

DESCRIPTION
  This function retrieves the total number of forward traffic channel frames
  received and the number of "bad" frames ( number of category 9 and 10
  frames ).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

extern void parm_get_fer
(
  parm_fer_type *data_ptr
    /* pointer to structure to put statistics in */
);


#endif /* PARM_H */
