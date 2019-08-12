/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



                            F T M  R F

GENERAL DESCRIPTION
  This is the header file for the embedded FTM 1x Dispatch commands comming 
  from the diag service 

Copyright (c) 2009       by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gnss/api/ftm_gnss_dispatch.h#1 $ 
  $DateTime: 2016/12/13 07:59:49 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/01/09   hp      Created. Added support for External LNA 
===========================================================================*/

#ifndef FTM_GNSS_DISPATCH_H
#define FTM_GNSS_DISPATCH_H

#include "rfa_variation.h"
#include "comdef.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "target.h"
#include "ftm.h"
#include "rfcom.h"
#include "ftmdiag.h"


//PROTOTYPES
ftm_rsp_pkt_type ftm_gnss_dispatch(ftm_pkt_type  *);


#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* FTM_GNSS_DISPATCH_H */
