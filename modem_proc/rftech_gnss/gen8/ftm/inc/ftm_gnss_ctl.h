/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



                            FTM 1x CTL

GENERAL DESCRIPTION
  This is the header file for the embedded FTM RF Commands 

Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gnss/gen8/ftm/inc/ftm_gnss_ctl.h#1 $ 
  $DateTime: 2016/12/13 07:59:49 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/01/09   hp      Created. Added support for External LNA

===========================================================================*/

#ifndef FTM_GNSS_CTL_H
#define FTM_GNSS_CTL_H

#include "rfa_variation.h"
#include "customer.h"
#include "target.h"
#include "rfcom.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftm.h"




/*===========================================================================

                     FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

FUNCTION FTM_SET_GNSS_EXTERNAL_LNA

DESCRIPTION
   This function will set the External LNA ON or OFF.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ftm_set_gnss_external_lna(boolean on_off); 


#endif /* FEATURE_FACTORY_TESTMODE */
#endif  // FTM_RF_H
