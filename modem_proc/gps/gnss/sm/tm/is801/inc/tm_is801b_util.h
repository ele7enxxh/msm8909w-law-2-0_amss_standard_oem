#ifndef TM_IS801B_UTIL_H
#define TM_IS801B_UTIL_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         TM IS801 Utility Module

GENERAL DESCRIPTION
  This file contains TM's IS801 Utility Functions

  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/is801/inc/tm_is801b_util.h#1 $
 
  when       who     what, where, why
  --------   ---     ----------------------------------------------------------

===========================================================================*/

#include "gps_variation.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "customer.h"
#include "gnss_common.h"
#include "tm_is801.h"
#include "tm_is801b.h"
#include "tm_is801_i.h"
#include "msg.h"
#include "sm_nv.h"
#include "pdapibuf.h"
#include "cai.h"
#include "mccdma.h"
#include "qw.h"
#include "math.h"
#include "sm_dm_event.h"

#include "tm_asn1util.h"

#if ((defined FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE) || \
     (defined FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE ))
#error code not present
#endif
#endif /* ((defined FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE) || (defined FEATURE_GNSS_SM_TM_1X_UP_IS801B_ENABLE )) */
