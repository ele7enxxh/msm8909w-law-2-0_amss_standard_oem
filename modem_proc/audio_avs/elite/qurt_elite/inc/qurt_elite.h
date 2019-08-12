/*========================================================================
qurt_elite.h

This is the top-level include file for the qurt_elite utilities.

Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/elite/qurt_elite/inc/qurt_elite.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/04/10   mwc      Created file.

========================================================================== */
/**
@file qurt_elite.h

@brief This file includes all header files required for using qurt_elite functions.
The user of qurt_elite should only include this file to call the qurt_elite functions.
*/

#ifndef QURT_ELITE_H
#define QURT_ELITE_H
#define assert ASSERT
/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#if defined(__qdsp6__)
#include "qurt.h"
#include "qube.h"
#include "q6protos.h"
#endif


#include "qurt_elite_diag.h"
#include "qurt_elite_bufmgr.h"
#include "qurt_elite_channel.h"
#include "qurt_elite_globalstate.h"
#include "qurt_elite_memory.h"
#include "qurt_elite_memorymap.h"
#include "qurt_elite_mutex.h"
#include "qurt_elite_profile.h"
#include "qurt_elite_queue.h"
#include "qurt_elite_signal.h"
#include "qurt_elite_thread.h"
#include "qurt_elite_timer.h"
#include "qurt_elite_types.h"
#include "qurt_elite_interrupt.h"
#include "qurt_elite_condvar.h"
#include "qurt_elite_std.h"
#include "qurt_elite_signal_group.h"

#include "adsp_error_codes.h"

#include <string.h>
#include "stringl.h"
#include <stdio.h>


#endif // #ifndef QURT_ELITE_H

