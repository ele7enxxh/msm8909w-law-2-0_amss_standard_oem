#ifndef APWRX_H
#define APWRX_H

#include "target.h"


/*===========================================================================

DESCRIPTION
  This file contains defines for integer size independence

  Copyright (c) 1999 - 2013 Qualcomm Technologies, Inc. 
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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/apwrx.h_v   1.0.2.0   30 Nov 2001 16:44:34   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/apwrx.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/03/13   nee     Changing the function signature of apwrxN() for inclusion
                   of memscpy() changes Phase 2
01/06/12   ppr     1X CP - Feature clean up: Mainline Always On features,
                   Remove Always Off Features
04/13/11   ag      Renamed type WORD to APWRX_WORD.
09/30/00   ks      Added casts to LOW and MSG macros to get rid of compiler
                   warnings.
01/20/99   ck      Checked in the file.

===========================================================================*/

/*=========================================================================*/


#define APWRX_WORD long

#define BITCT (8*sizeof(unsigned APWRX_WORD))
#define LOW ((unsigned APWRX_WORD)(((unsigned APWRX_WORD)1L << (BITCT/2)) - 1))
#define MSB ((unsigned APWRX_WORD)((unsigned APWRX_WORD)1L << (BITCT-1)))

/* set Nmax equal to the maximum number of bits divided by BITCT */

#define MaxBits 512
#define Nmax ((MaxBits+BITCT-1)/BITCT)

int apwrxN(int n, unsigned APWRX_WORD *a, unsigned APWRX_WORD *x, unsigned APWRX_WORD *ax,
       size_t size_of_ax, unsigned APWRX_WORD *p);

void InitMont(unsigned APWRX_WORD *m,   /* storage for -1/p */
        unsigned APWRX_WORD *p);  /* modulus */


#endif /* APWRX_H */
