#ifndef DH_H
#define DH_H
/*===========================================================================

               E X T E R N A L   D I F F I E - H E L M A N
                      T A S K   D E F I N I T I O N S

DESCRIPTION
  This contains all the declarations for the Authentication Task.

  Copyright (c) 1998 - 2013 Qualcomm Technologies, Inc. 
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
                      
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/dh.h_v   1.0.2.0   30 Nov 2001 16:53:36   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/dh.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $


when       who    what, where, why
--------   ---    ----------------------------------------------------------
08/31/09   adw    Removed unnecessary target.h include.
07/01/09   jj     Moved a symbol from internal dh_v.h to this file.
04/09/09   jj     CMI Phase2 changes
03/05/09   adw    Moved task start functionality to public header.
03/04/09   adw    Removed the temporary _v.h include.
12/01/08   adw    Temporarily include the internal _v.h file.
11/21/08   adw    Merge from CMI sandbox, based on VU_MODEM_1X_CP.07.02.09
12/10/04   sb     Fixed lint issue.
10/20/04   an     Replaced inclusion of srch.h with srchmc.h
09/23/04   fc     Fixed lint errors.
01/20/99   ck     Removed set_a_key_temp as it was not being used and also
                  removed extern from functions generate_rand and dh_wait
08/25/98   ck     Featurised the interface routines for the exponentiation 
                  module under FEATURE_DH_EXP
06/25/98   ck     Created Module

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
  /* Number of bytes of Diffie-Helman result  */

/* <EJECT> */

#define DH_RAND                   20
  /* Number of bytes of Random number generated in DH by the random number generator */

/*===========================================================================

FUNCTION DH_TASK

DESCRIPTION
  This procedure is the entrance procedure for the DH task.

DEPENDENCIES
  None

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None

===========================================================================*/

extern void dh_task
(
  dword dummy
    /* Parameter required for REX.  Tell lint to ignore it. */
    /*lint -esym(715,dummy) */
);

/* <EJECT> */
/*===========================================================================

FUNCTION GET_RAND_FROM_DH

DESCRIPTION
      This function returns a random number that is generated and stored in DH

DEPENDENCIES
      None.

RETURN VALUE
      None.

SIDE EFFECTS
      None.

===========================================================================*/
extern byte* get_rand_from_dh(void);



#endif /* DH_H */

