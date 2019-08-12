/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     MBMS SERVICE MANAGER TASK MODULE (MSMTASK.C)

GENERAL DESCRIPTION

 This file contains types, function definitions and data definitions for
 MBMS Service Manager Task

Copyright (c) 2001,2002,2003 Qualcomm Technologies, Incorporated and its licensors.  All 
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mbmsm/src/msmtask.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/17/06   vr      Initial version
03/18/08   rk      Added Support to merge MBMS SM in CB task
09/12/11   HC      Replacing the #include "ms_timer.h" with #include "ms_timer_v.h" 
                   as a part of CMI activity
                   Replacing the #include "gs.h" with #include "gs_v.h" 
                   as a part of CMI activity

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "task.h"
#include "dog_hb_rex.h"
#include "queue.h"
#include "err.h"
#include "gs_v.h"
#include "ms_timer_v.h"
#ifndef FEATURE_MODEM_RCINIT_PHASE2
#include "tmc.h"
#endif /* FEATURE_MODEM_RCINIT_PHASE2 */

#if defined(FEATURE_MBMS) || defined (FEATURE_MODEM_MBMS)
#error code not present
#endif

#ifdef FEATURE_MBMS
#error code not present
#endif

