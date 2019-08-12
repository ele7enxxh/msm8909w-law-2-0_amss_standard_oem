/*
 * Copyright (c) 2003-2013 Objective Systems, Inc.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by Objective Systems, Inc.
 *
 * PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 *****************************************************************************/
/**
 * @file rtxMemLeakCheck.h
 */
#ifndef _RTXMEMLEAKCHECK_H_
#define _RTXMEMLEAKCHECK_H_

/*
DBG_NEW will be defined here.  In files where the new operator is not
defined, you may add this to enable collection of location information for
memory locations using new:
   #define new DBG_NEW
When memory debugging is not enabled, DBG_NEW will simply be defined as "new",
so this can be used without consideration of any other flags.
*/

/* VC++ and _DEBUG/_TRACE only */
#if defined (_MSC_VER) && defined (_DEBUG) && defined (_TRACE) && !defined (_WIN32_WCE)
#include <crtdbg.h>
#include "rtxsrc/osSysTypes.h"
#include "rtxsrc/rtxExternDefs.h"

#define DECLARE_MEMLEAK_DETECTOR0 { \
int LeakTmpFlag; \
_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG); \
_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT); \
_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG); \
_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT); \
_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG); \
_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT); \
LeakTmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); \
LeakTmpFlag |= _CRTDBG_CHECK_ALWAYS_DF; \
LeakTmpFlag |= _CRTDBG_LEAK_CHECK_DF;   \
LeakTmpFlag |= _CRTDBG_DELAY_FREE_MEM_DF; \
_CrtSetDbgFlag(LeakTmpFlag); \
}

#ifdef __cplusplus
#define DECLARE_MEMLEAK_DETECTOR static class ___MemLeakDetector___ { \
public: \
___MemLeakDetector___ () DECLARE_MEMLEAK_DETECTOR0 \
} ___memleakdetector__
#else
#define DECLARE_MEMLEAK_DETECTOR DECLARE_MEMLEAK_DETECTOR0
#endif /* __cplusplus */

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )

#else /* NOT VC++ OR NOT _DEBUG OR NOT _TRACE */
#define DECLARE_MEMLEAK_DETECTOR
#define DBG_NEW new
#endif /* _MSC_VER */

#endif /* _RTXMEMLEAKCHECK_H_ */
