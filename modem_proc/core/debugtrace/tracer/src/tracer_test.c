/*===========================================================================
  FILE: tracer_test.c

  OVERVIEW:     QUALCOMM Debug Subsystem (QDSS) Tracer - Test

  DEPENDENCIES:

               Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/debugtrace/tracer/src/tracer_test.c#1 $
===========================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "comdef.h"
#include "tracer_test.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/

// Test types
#define TRACER_TEST_TYPE_API         1  // Basic sanity test.
#define TRACER_TEST_TYPE_PERF        2  // Performance measures.

/*---------------------------------------------------------------------------
 * Function Definitions
 * ------------------------------------------------------------------------*/

extern int tracer_test_api(uint8 subtype);

/*-------------------------------------------------------------------------*/
int tracer_test_perf(uint8 type, uint16 option, uint32 mask,
                     uint32 count1, uint32 count2)
{
   return TRACER_TEST_RVAL_UNSUPPORTED;
}

/*-------------------------------------------------------------------------*/
int tracer_test(uint8 type, uint8 subtype,
                uint16 option, uint32 mask,
                uint32 count1, uint32 count2)
{
   int rval;
   switch (type)
   {
      case TRACER_TEST_TYPE_API:
         rval = tracer_test_api(subtype);
         break;
      case TRACER_TEST_TYPE_PERF:
         rval = tracer_test_perf(subtype, option, mask, count1, count2);
         break;
      default:
         rval = TRACER_TEST_RVAL_UNSUPPORTED;
         break;
   }
   return rval;
}

