/*=============================================================================
  @file sns_pd.c

  This file contains sensors PD initialization code.

*******************************************************************************
* Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved
* Qualcomm Technologies Confidential and Proprietary.
********************************************************************************/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/dsps/src/sns_pd.c#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
  EDIT HISTORY FOR FILE

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-09-18   SM   Removed main().  Inserted pd_start.
  2013-10-18  JHH   Remove timer and wait forever
  2013-08-22   PK   Increased SNS heap size for Hammerhead
  2013-07-26   VH   Eliminated compiler warnings
  2012-12-02   AG   Initial version
  2013-02-05   AG   Use RCInit, remove previous workaround
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

/* This used to be define here: */
/* Heap size to be defined by each User PD Image owner */
/*
#ifdef ADSP_HWCONFIG_L
    unsigned int heapSize=0x100000;
#else
    unsigned int heapSize=0xC0000;
#endif
*/
#include "pd_mon_qurt.h"

void pd_mon_sensors(void)
{
   PD_MON_RESTART restart = PD_MON_RESTART_ALWAYS;
   pd_mon_spawn(&restart, SENSOR_IMG_NAME);
}

