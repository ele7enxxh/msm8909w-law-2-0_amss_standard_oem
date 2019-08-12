/*======================================================================

               OEM DRE DRSync file

 GENERAL DESCRIPTION
  This file contains the implementation of OEM DR Enablement.
  This task enables OEMs to access following positioning information
  provided from the SM layer of GPS software (on modem processor).

 EXTERNALIZED FUNCTIONS


 INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

 Export of this technology or software is regulated by the U.S. Government.
 Diversion contrary to U.S. law prohibited.
======================================================================*/

/*=====================================================================

 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/oem/src/sm_oemdrsync.c#1 $$DateTime: 2016/12/13 07:59:45 $$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/10/10   adi      Initial version.
======================================================================*/

#include "sm_oemdrsync.h"
#include "msg.h"
#include "sm_log.h"

/****************** Variables ************************************************/

/****************** Function Definitions**************************************/

/*===========================================================================

FUNCTION oemDrsyncConfig()

DESCRIPTION


DEPENDENCIES
None.

PARAMETERS


RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void oemDrSyncConfig(const gnss_DRSyncConfigType* pOemDrSyncConfig)
{
  (void) pOemDrSyncConfig;

  if (NULL != pOemDrSyncConfig) {
    sm_log_oemdrsync_config(pOemDrSyncConfig);

    MSG_9(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "DRSync GetCFG %d %d %f %d %d %f %f %d %d",
      pOemDrSyncConfig->e_Mode,
      pOemDrSyncConfig->e_Polarity,
      pOemDrSyncConfig->f_BiasNs,
      pOemDrSyncConfig->l_PhaseSec,
      pOemDrSyncConfig->l_PeriodSec,
      pOemDrSyncConfig->f_TuncStartThresholdMs,
      pOemDrSyncConfig->f_TuncStopThresholdMs,
      pOemDrSyncConfig->b_TuncIgnore,
      pOemDrSyncConfig->b_Armed);
  }
  else {
    MSG_3(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "NULL pOemDrSyncConfig", 0, 0, 0);
  }
}

/*===========================================================================

FUNCTION oemDrSyncPulseDesc()

DESCRIPTION


DEPENDENCIES
None.

PARAMETERS
None.

RETURN VALUE
oemdre status type

SIDE EFFECTS
None.

===========================================================================*/
void oemDrSyncPulseDescriptor(const gnss_DRSyncPulseDescType *pOemDrSyncPulseDesc)
{
  (void) pOemDrSyncPulseDesc;

  if (NULL != pOemDrSyncPulseDesc) {
    sm_log_oemdrsync_pulse_desc(pOemDrSyncPulseDesc);

    MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"DRSync PulseDesc %f %d %f %d", 
      pOemDrSyncPulseDesc->f_GpsMsec,
      pOemDrSyncPulseDesc->w_GpsWeek,
      pOemDrSyncPulseDesc->f_ClkTimeUncMs, 
      pOemDrSyncPulseDesc->q_Count);
  }
  else {
    MSG_3(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "NULL pOemDrSyncPulseDesc", 0, 0, 0);
  }
}
