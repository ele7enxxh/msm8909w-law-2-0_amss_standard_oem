#ifndef DBG_UTIL_H
#define DBG_UTIL_H
/*============================================================================
dbg_util

ABSTRACT
   debug utilities.

DESCRIPTION
   Provides debug functions


Copyright (c) 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
=========================================================================== */

/*=========================================================================
                              Edit History

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/smecom/ixutil/environment/amss/inc/dbg_util.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
06/26/06   jay    Added a few more utility functions
06/21/06   ssm    Enable DEBUG_PRINT_ENABLE
03/02/06   gr     Initial Revision

=========================================================================== */


#include "comdef.h"
#include "msg.h"
#ifdef UIONE_SDK
#include "AEEStdlib.h"
#endif
#ifndef DEBUG_PRINT_ENABLE
#define DEBUG_PRINT_ENABLE
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEBUG_PRINT_ENABLE
  #define DBGPRINT_METADATA_FORMAT  "dbgprint %s:%d"
  #ifdef UIONE_SDK
    #define DBGPRINT DBGPRINTF
  #else
   #define DBGPRINT\
      dbgprint_format( msg_file, __LINE__), dbgprint
  #endif /* UIONE_SDK */
#else
  #define DBGPRINT dbgprint_dummy
#endif

#ifdef FEATURE_SME_DEBUG
    #define SMECOM_DBG_MSG0 MSG
    #define SMECOM_DBG_MSG1 MSG_SPRINTF_1
    #define SMECOM_DBG_MSG2 MSG_SPRINTF_2
    #define SMECOM_DBG_MSG3 MSG_SPRINTF_3
#else
    #define SMECOM_DBG_MSG0(a, b, c)
    #define SMECOM_DBG_MSG1(a, b, c, d)   
    #define SMECOM_DBG_MSG2(a, b, c, d, e)
    #define SMECOM_DBG_MSG3(a, b, c, d, e, f)
#endif

#ifdef FEATURE_DRM_NO_BREW
  #ifndef DBGPRINTF
  #define DBGPRINTF DBGPRINT
  #endif
#endif

extern void dbgprint(const char * pFormat, ...);

extern void dbgprint_dummy(const char * pszFormat, ...);

extern void dbgprint_format( const char * pFilePath, int nLineNum);

extern void PrintBinary( uint8*  data, uint32 len  );

extern void PrintLargeData( uint8* data, uint32 len );

#ifdef __cplusplus
}
#endif

#endif /* DBG_UTIL_H */
