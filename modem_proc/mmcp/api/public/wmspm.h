#ifndef WMSPM_H
#define WMSPM_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           W i r e l e s s     M e s s a g i n g  S e r v i c e s
              -- API definitions for Power Management

DESCRIPTION
  This module defines the data types and functions for the Short Message
  Services (SMS) including EMS and Broadcast SMS in CDMA 1x and GSM/GPRS/UMTS
  systems which are used by the client software and are part of the
  UAPI (User Application Programming Interface).

Copyright (c) 1998,1999,2000,2001,2002,2003,2004,2005,2006,2007,2008
              by Qualcomm Technologies, Incorporated.
All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wms.h_v   1.22   19 Jul 2002 08:59:16   hqu  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/wmspm.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/11/08   PMD     Removed wms_bc_*** APIs for CDMA Broadcast
01/31/08   PMD     Updated Copyright Information to include 2008
04/18/07   HQ      Added guards against C++ style linking.
04/09/07   HQ      Re-org API header files.
03/29/07   HQ      Initial version for supporting Power Management
                   for Dual CPU chipsets.
===========================================================================*/


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/
#include "wms.h"


#ifdef __cplusplus
extern "C" {
#endif

/* <EJECT> */
/*===========================================================================

                         DATA TYPE DECLARATIONS

===========================================================================*/



#ifdef __cplusplus
}
#endif


#endif /* WMSPM_H */
