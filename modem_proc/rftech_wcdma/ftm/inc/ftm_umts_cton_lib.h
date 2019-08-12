#ifndef FTM_UMTS_CTON_H
#define FTM_UMTS_CTON_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                           F T M  C - T O - N

GENERAL DESCRIPTION
  This is the header file for the embedded FTM CTON command comming from the diag service 

Copyright (c) 1994, 1995, 1996 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2000-2013        by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/ftm/inc/ftm_umts_cton_lib.h#1 $ 
  $DateTime: 2016/12/13 07:59:54 $ 
  $Author: mplcsds1 $

when       who       what, where, why
--------   ---       ----------------------------------------------------------
07/25/04   bhaskar   Created
===========================================================================*/

#include "rfa_variation.h"
#ifdef FEATURE_FACTORY_TESTMODE

/*------------------------------------------------------------------------*/
/*                     FUNCTION PROTOTYPES                                */
/*------------------------------------------------------------------------*/

extern int ftm_umts_get_CtoN( void);


/*------------------------------------------------------------------------*/
/*                     MACRO DEFINES                                      */
/*------------------------------------------------------------------------*/

#define FFT_DC_INDEX 3
#define FFTLEN	2048
#define FFTBITS 11

#define FTM_GSM_MAX_BUFFER  1024

#define CTON_NO_AVERAGES  10

#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* FTM_UMTS_CTON_H */
