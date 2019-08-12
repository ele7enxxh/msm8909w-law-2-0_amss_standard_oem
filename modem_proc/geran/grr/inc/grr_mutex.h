#ifndef GRR_MUTEX_H
#define GRR_MUTEX_H
/*==========================================================================*/
/*!
  @file GRR_MUTEX.H

  GERAN RR Mutex definitions

  Provides mutex definitions shared within GRR mdule

  Copyright (c) 2008-2013 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*==========================================================================*/
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/inc/grr_mutex.h#1 $ */

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      ----------------------------------------------------------
14/03/12   pg       Add support for QuRT 
29/07/98   hv       Created to support KxMutex
===========================================================================*/

/*----------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */ 

/* HV: to investigate replacing FEATURE_WINCE with FEATURE_NV_SUPPORTS_KXMUTEX ..
*/
#include "gmutex.h"


/*----------------------------------------------
** Critical section to protect shared vars in RR
**----------------------------------------------
*/

extern rex_crit_sect_type grr_general_crit_sec;
extern rex_crit_sect_type grr_sysinfo_crit_sec;


#endif /* GRR_MUTEX_H */
