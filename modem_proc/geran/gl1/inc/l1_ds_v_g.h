#ifndef L1_DS_V_G_H
#define L1_DS_V_G_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        L1   D A T A  S E R V I C E S   P R O C E S S I N G
                       H E A D E R   F I L E

DESCRIPTION
   This module contains definitions for the interface between L1 and
   Data Services.


Copyright (c) 2001-2013 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gl1/inc/l1_ds_v_g.h#1 $

when       who      what, where, why
--------   -------- ---------------------------------------------
10-04-01   tjw      Add customer.h and comdef.h to all SU API files
10/28/03   tb       Changed calculation of GCSD_BUF_MAX_OFFSET.
09/22/03   tb       Added prototype for gl1_get_current_ded_chan_type().
09/13/01   JC       Initial revision. Added QCT headers and some cleanup.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "sys_stru.h"
#include "gl1_defs.h"
#include "gl1_defs_g.h"

/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/
#ifdef FEATURE_DATA_GCSD

/* Cross-check the value copied from "gl1_defs.h"
#include "gl1_defs_g.h" by api\geran\l1_ds.h */
#if APICOPY__GL1_DEFS_FRAMES_IN_BLK_PERIOD != GL1_DEFS_FRAMES_IN_BLK_PERIOD
  #error APICOPY__GL1_DEFS_FRAMES_IN_BLK_PERIOD != GL1_DEFS_FRAMES_IN_BLK_PERIOD
#endif

/* Cross-check the size of the copy of gl1_defs_rx_hdr_struct in api\geran\l1_ds.h */
/*lint --emacro((514), COMPILE_TIME_ASSERT)*/
#define COMPILE_TIME_ASSERT(e) enum { COMPILE_TIME_ASSERT_value = 1/(!!(e)) };
COMPILE_TIME_ASSERT(sizeof(APICOPY__gl1_defs_rx_hdr_struct) ==
                    sizeof(gl1_defs_rx_hdr_struct))

#endif /* FEATURE_DATA_GCSD */

#endif /* L1_DS_V_G_H */
