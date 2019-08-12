#ifndef mm_mm_h
#define mm_mm_h
/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mm_mm.h_v   1.6   21 Oct 2001 11:19:42   skoganti  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/mm_mm.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/04/01   jca     Added missing includes to quiet LINT.
08/14/01   ab      Renamed CM to CNM.
08/22/01   jca     Removed changes from version 1.3 and 1.4.
11/14/02   mks     Added Message type for MM_INFORMATION message
06/06/05   abe     Added code for CCBS support
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "sys_cnst.h"
#include "sys_stru.h"
#include "com_iei.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*
 * Local  Defines
 * --------------
 */

/* 
 * Message Types for mm to mm peer messages - These are as defined in
 * GSM 04.08
 */

#define IMSI_DETACH_INDICATION      0x01
#define LOCATION_UPDATING_ACCEPT    0x02
#define LOCATION_UPDATING_REJECT    0x04
#define LOCATION_UPDATING_REQUEST   0x08
#define AUTHENTICATION_REJECT       0x11
#define AUTHENTICATION_REQUEST      0x12
#define AUTHENTICATION_RESPONSE     0x14
#define IDENTITY_REQUEST            0x18
#define IDENTITY_RESPONSE           0x19
#define TMSI_REALLOCATION_COMMAND   0x1a
#define TMSI_REALLOCATION_COMPLETE  0x1b
#define CM_SERVICE_ACCEPT           0x21
#define CM_SERVICE_REJECT           0x22
#define CM_SERVICE_ABORT            0x23
#define CM_SERVICE_REQUEST          0x24
#define CM_REESTABLISHMENT_REQUEST  0x28
#define ABORT                       0x29
#define MM_STATUS                   0x31
#define MM_INFORMATION              0x32

#define AUTHENTICATION_FAILURE      0x1C

#define PAGING_RESPONSE             0x27




#endif /* mm_mm_h */
