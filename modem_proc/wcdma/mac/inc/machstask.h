#ifndef MAC_HS_DL_TASK_H
#define MAC_HS_DL_TASK_H
/*============================================================================================

            M A C   H S D P A   D L   T A S K    H E A D E R    F I L E

DESCRIPTION
  This file contains all of the definitions necessary to interface with downlink mac hs task.

Copyright (c) 1991- 2004 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2005, 2008-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================================*/

/*============================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/inc/machstask.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who   what, where, why
--------   ---   ----------------------------------------------------------------------------
01/29/14   kc    Fix not to request new p-t-c until TSNs from prev p-t-c are all delivered
01/04/13   kc    Removing duplicate/redundant code
07/13/12   grk   MBMS feature cleanup at L2(MBMS_DEBUG_1, FEATURE_MODEM_MBMS)
01/02/12   grk   Feature cleanup
09/19/10   av    HS_FACH and Enhanced 0x4220 log changes
05/07/09   ssg   Updated Copyright Information
04/29/09   av    Fixed compiler warnings in L2 
03/01/09   rm    Added new featurization for FEATURE_CMI.
12/18/08   ksr   L2 Mutex changes to avoid concurrency issues
01/12/05   gsc   Added 1 sec timer for reporting status log pkt.
10/11/04   gsc   Initial revision

============================================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"

#ifdef CUST_H
#include "customer.h"
#endif

/* Public portion of the API exists in wcdmatmcif.h */
#include "wcdmatmcif.h"

/* ----------------------------------------------------------------------- */
/* Private Section.                                                        */
/* ----------------------------------------------------------------------- */

#include  "queue.h"

/*============================================================================================
                        DOWNLINK LAYER 2 SIGNAL DEFINITIONS
============================================================================================*/

/* This signal is set when an item is placed on mac_hs_cmd_q */
#define     MAC_HS_DL_DATA_IND_SIG         0x0001
#define     MAC_HS_DL_DOG_RPT_TIMER_SIG    0x0002
#define     MAC_HS_QUEUE_1_TIMER_SIG       0x0004
#define     MAC_HS_QUEUE_2_TIMER_SIG       0x0008
#define     MAC_HS_QUEUE_3_TIMER_SIG       0x0010
#define     MAC_HS_QUEUE_4_TIMER_SIG       0x0020
#define     MAC_HS_QUEUE_5_TIMER_SIG       0x0040
#define     MAC_HS_QUEUE_6_TIMER_SIG       0x0080
#define     MAC_HS_QUEUE_7_TIMER_SIG       0x0100
#define     MAC_HS_QUEUE_8_TIMER_SIG       0x0200
#define     MAC_HS_DL_CONFIG_REQ_SIG       0x0400

#ifdef FEATURE_MAC_HS_QXDM_LOGGING 
#define     MAC_HS_DL_STATUS_LOG_TIMER_SIG 0x0800
#endif

#define     MAC_HS_P_TO_C_DATA_IND_SIG_RESERVED     0x1000

#ifdef FEATURE_WCDMA_HS_FACH
#define MAC_HS_RESET_BASE_TIMER_SIG        0x2000
#endif

/*=============================================================================================

FUNCTION  wcdma_mac_hs_dl_wait

DESCRIPTION
	This function waits for any signal in a specified signal mask to be set.
	If this procedure detects that any signal in the specified signal mask is
	set, it will return the entire mask of signals set in the task's tcb.

DEPENDENCIES
	None.

RETURN VALUE
	Mask of signals returned by the rex_wait call.

SIDE EFFECTS
	None.

=============================================================================================*/
uint32  wcdma_mac_hs_dl_wait
	 (
	 /* signals to wait on */
	 uint32  wait_sigs
	 );

#endif /* MAC_HS_DL_TASK_H */
