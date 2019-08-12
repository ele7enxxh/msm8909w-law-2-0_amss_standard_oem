/*==============================================================================

FILE:      adsppm_utils.h

DESCRIPTION: ADSPPM internal utils API

This is the internal utils API for ADSPPM. This is not distributed to any clients that uses
ADSPPM. 

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A


* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.

==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //components/rel/core.mpss/3.5.c12.3/power/acpm/inc/adsppm_utils.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
07/20/11   CK      Initial version
==============================================================================*/

#ifndef ADSPPM_UTILS_H
#define ADSPPM_UTILS_H

#include "DALSys.h"
#include "adsppm.h"
#include "core_internal.h"
#include "asic.h"
#include "ULog.h"
//#include "msg_mask.h" //TODO: 08/29/2013, compilation complains about repeated definition for DIAG_MSG_DISABLE_LVL_MASK
#include "tracer.h"
#include "acpm_tracer_event_ids.h"



//#define ADSPPM_NO_ULOG 1

extern __inline void adsppm_lock( DALSYSSyncHandle lock );
extern __inline void adsppm_unlock( DALSYSSyncHandle lock );

extern __inline ULogHandle GetUlogHandle(void);

#define ADSPPM_ARRAY_SIZE( array ) (sizeof(array)/sizeof(array[0]))
#define ADSPPM_ARRAY( array ) ADSPPM_ARRAY_SIZE(array), &array[0]

#define DIVIDE_ROUND_UP(val, base) ((val+base-1)/base)

#if (ADSPPM_NO_MUTEX==1)
#define ADSPPM_LOCK(hLock)
#define ADSPPM_UNLOCK(hLock)
#else
#define ADSPPM_LOCK(hLock) adsppm_lock(hLock)
#define ADSPPM_UNLOCK(hLock) adsppm_unlock(hLock)
#endif

#define ADSPPM_LOCK_RESOURCE(resourceId) adsppm_lock(GetGlobalContext()->RMCtxLock[resourceId-1])
#define ADSPPM_UNLOCK_RESOURCE(resourceId) adsppm_unlock(GetGlobalContext()->RMCtxLock[resourceId-1])

//Logging levels
#define ADSPPM_LOG_LEVEL_PROFILE 0
#define ADSPPM_LOG_LEVEL_ERROR 1 /** Error log messages*/
#define ADSPPM_LOG_LEVEL_WARNING 2 /** Warnings */
#define ADSPPM_LOG_LEVEL_INFO 3 /** Critical info messages */
#define ADSPPM_LOG_LEVEL_INFO_EXT 4 /** Extended info messages */
#define ADSPPM_LOG_LEVEL_FN_ENTRY 5 /** Function entry/exit messages */
#define ADSPPM_LOG_LEVEL_DEBUG 6 /** All debug messages */
#define ADSPPM_LOG_LEVEL_MAX 7


//Turn on/off QDSS software event log
#if(ADSPPM_NO_QDSS_LOG == 1)
#define ADSPPM_QDSS_EVENT_0(event)
#define ADSPPM_QDSS_EVENT_1(event, arg_1)
#define ADSPPM_QDSS_EVENT_2(event, arg_1, arg_2)
#define ADSPPM_QDSS_EVENT_3(event, arg_1, arg_2, arg_3)
#define ADSPPM_QDSS_EVENT_4(event, arg_1, arg_2, arg_3, arg_4)
#define ADSPPM_QDSS_EVENT_5(event, arg_1, arg_2, arg_3, arg_4, arg_5)

#else
#define ADSPPM_QDSS_EVENT_0(event) tracer_event_simple_vargs((tracer_event_id_t)event,0)
#define ADSPPM_QDSS_EVENT_1(event, arg_1) tracer_event_simple_vargs((tracer_event_id_t)event, 1, arg_1)
#define ADSPPM_QDSS_EVENT_2(event, arg_1, arg_2) tracer_event_simple_vargs((tracer_event_id_t)event, 2, arg_1, arg_2)
#define ADSPPM_QDSS_EVENT_3(event, arg_1, arg_2, arg_3)  tracer_event_simple_vargs((tracer_event_id_t)event, 3, arg_1, arg_2, arg_3)
#define ADSPPM_QDSS_EVENT_4(event, arg_1, arg_2, arg_3, arg_4) tracer_event_simple_vargs((tracer_event_id_t)event, 4, arg_1, arg_2, arg_3, arg_4)
#define ADSPPM_QDSS_EVENT_5(event, arg_1, arg_2, arg_3, arg_4, arg_5) tracer_event_simple_vargs((tracer_event_id_t)event, 5, arg_1, arg_2, arg_3, arg_4, arg_5)

#endif

//Turn on/off DIAG messages
#if (Q6ADSPPM_NO_DIAG_MSG==1)
#define ADSPPM_DBG_MSG(prio, message)
#define ADSPPM_DBG_MSG_1(prio, message, arg_1)
#define ADSPPM_DBG_MSG_2(prio, message, arg_1, arg_2)
#define ADSPPM_DBG_MSG_3(prio, message, arg_1, arg_2,arg_3)
#define ADSPPM_DBG_MSG_4(prio, message, arg_1, arg_2,arg_3,arg_4)
#define ADSPPM_DBG_MSG_5(prio, message, arg_1, arg_2,arg_3,arg_4,arg_5)
#else
#define ADSPPM_DBG_MSG(prio, message)  MSG(MSG_SSID_QDSP6, prio ,message )
#define ADSPPM_DBG_MSG_1(prio, message, arg_1)  MSG_1(MSG_SSID_ADSP_PWRDEM, prio , message, arg_1 )
#define ADSPPM_DBG_MSG_2(prio, message, arg_1, arg_2)  MSG_2(MSG_SSID_ADSP_PWRDEM, prio , message, arg_1, arg_2 )
#define ADSPPM_DBG_MSG_3(prio, message, arg_1, arg_2,arg_3)  MSG_3(MSG_SSID_ADSP_PWRDEM, prio , message, arg_1 ,arg_2, arg_3)
#define ADSPPM_DBG_MSG_4(prio, message, arg_1, arg_2,arg_3,arg_4)  MSG_4(MSG_SSID_ADSP_PWRDEM, prio , message, arg_1 ,arg_2, arg_3,arg_4)
#define ADSPPM_DBG_MSG_5(prio, message, arg_1, arg_2,arg_3,arg_4,arg_5)  MSG_5(MSG_SSID_ADSP_PWRDEM, prio , message, arg_1 ,arg_2, arg_3,arg_4, arg_5)
#endif

//TODO: 08/29/2013, compilation complains about repeated definition for DIAG_MSG_DISABLE_LVL_MASK
//Re-direct ULOG output to DIAG messages
/*#if (ADSPPM_ULOG_TO_DIAG==1)
#undef  ULOG_RT_PRINTF_0
#undef  ULOG_RT_PRINTF_1
#undef  ULOG_RT_PRINTF_2
#undef  ULOG_RT_PRINTF_3
#undef  ULOG_RT_PRINTF_4
#undef  ULOG_RT_PRINTF_5
#define ULOG_RT_PRINTF_0(h, formatStr) UNUSED(h); ADSPPM_DBG_MSG(MSG_LEGACY_HIGH,formatStr)
#define ULOG_RT_PRINTF_1(h, formatStr, p1)   UNUSED(h); ADSPPM_DBG_MSG_1(MSG_LEGACY_HIGH,formatStr,p1)
#define ULOG_RT_PRINTF_2(h, formatStr, p1,p2)   UNUSED(h); ADSPPM_DBG_MSG_2(MSG_LEGACY_HIGH,formatStr,p1,p2)
#define ULOG_RT_PRINTF_3(h, formatStr, p1,p2,p3)   UNUSED(h); ADSPPM_DBG_MSG_3(MSG_LEGACY_HIGH,formatStr,p1,p2,p3)
#define ULOG_RT_PRINTF_4(h, formatStr, p1,p2,p3,p4)   UNUSED(h); ADSPPM_DBG_MSG_4(MSG_LEGACY_HIGH,formatStr,p1,p2,p3,p4)
#define ULOG_RT_PRINTF_5(h, formatStr, p1,p2,p3,p4,p5)   UNUSED(h); ADSPPM_DBG_MSG_4(MSG_LEGACY_HIGH,formatStr,p1,p2,p3,p4,p5)
#endif*/


#if (ADSPPM_NO_ULOG==1)
#define ULOG_RT_PRINTF_0(h, formatStr)
#define ULOG_RT_PRINTF_1(h, formatStr, p1)
#define ULOG_RT_PRINTF_2(h, formatStr, p1,p2)
#define ULOG_RT_PRINTF_3(h, formatStr, p1,p2,p3)
#define ULOG_RT_PRINTF_4(h, formatStr, p1,p2,p3,p4)
#define ULOG_RT_PRINTF_5(h, formatStr, p1,p2,p3,p4,p5)
#endif

#define ADSPPM_LOG_PRINTF_0(level, formatStr) \
        if(ACM_GetDebugLevel() >= level) {ULOG_RT_PRINTF_1(GetUlogHandle(), "%s: "formatStr, __FUNCTION__ );}
#define ADSPPM_LOG_PRINTF_1(level, formatStr, p1) \
        if(ACM_GetDebugLevel() >= level) {ULOG_RT_PRINTF_2(GetUlogHandle(), "%s: "formatStr, __FUNCTION__, p1);}
#define ADSPPM_LOG_PRINTF_2(level, formatStr, p1,p2) \
        if(ACM_GetDebugLevel() >= level) {ULOG_RT_PRINTF_3(GetUlogHandle(), "%s: "formatStr, __FUNCTION__, p1,p2);}
#define ADSPPM_LOG_PRINTF_3(level, formatStr, p1,p2,p3) \
        if(ACM_GetDebugLevel() >= level) {ULOG_RT_PRINTF_4(GetUlogHandle(), "%s: "formatStr, __FUNCTION__, p1,p2,p3);}
#define ADSPPM_LOG_PRINTF_4(level, formatStr, p1,p2,p3,p4) \
        if(ACM_GetDebugLevel() >= level) {ULOG_RT_PRINTF_5(GetUlogHandle(), "%s: "formatStr, __FUNCTION__, p1,p2,p3,p4);}
#define ADSPPM_LOG_PRINTF_5(level, formatStr, p1,p2,p3,p4,p5) \
        if(ACM_GetDebugLevel() >= level) {ULOG_RT_PRINTF_6(GetUlogHandle(), "%s: "formatStr, __FUNCTION__, p1,p2,p3,p4,p5);}
#define ADSPPM_LOG_PRINTF_6(level, formatStr, p1,p2,p3,p4,p5,p6) \
        if(ACM_GetDebugLevel() >= level) {ULOG_RT_PRINTF_7(GetUlogHandle(), "%s: "formatStr, __FUNCTION__, p1,p2,p3,p4,p5,p6);}



#define ADSPPM_LOG_FUNC_ENTER \
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_FN_ENTRY, "Enter\n")

#define ADSPPM_LOG_FUNC_EXIT(code) \
        ADSPPM_LOG_PRINTF_1(ADSPPM_LOG_LEVEL_FN_ENTRY, "Exit(%u)\n", code)

#define ADSPPM_LOG_FUNC_PROFILESTART \
        if(ACM_GetDebugLevel() == ADSPPM_LOG_LEVEL_PROFILE) {ULOG_RT_PRINTF_1(GetUlogHandle(), "%s: ProfileStart\n", __FUNCTION__ );}

#define ADSPPM_LOG_FUNC_PROFILEEND \
        if(ACM_GetDebugLevel() == ADSPPM_LOG_LEVEL_PROFILE) {ULOG_RT_PRINTF_1(GetUlogHandle(), "%s: ProfileEnd\n", __FUNCTION__ );}


#endif

