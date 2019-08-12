/* =========================================================================

DESCRIPTION
  Implementation of stubs for memheap to compile the memheap and amssheap on mba
  image

Copyright (c) 2014      by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================ */

/* =========================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/services/utils/src/memheap_mba_stub.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
09/15/14  rks    Added stubs for err_SaveFatal2 and err_SaveFatal1
09/19/12  mg     Created original file
============================================================================ */
#include "comdef.h"  
#include "msg_diag_service.h"
#include "qurt.h"

#if defined (FEATURE_SAVE_DEBUG_TRACE)
void err_Fatal_internal3 (const msg_const_type* const_blk, uint32 code1, uint32 code2, uint32 code3)
{
   return;
}

void err_Fatal_internal2 (const msg_const_type* const_blk, uint32 code1, uint32 code2)
{
   return;
}

void err_Fatal_internal1 (const msg_const_type* const_blk, uint32 code1)
{
  return;
}

void err_Fatal_internal0 (const msg_const_type* const_blk)
{
	return;
}
#else

void err_Fatal3 (const err_FatalParam_type* const_blk, uint32 code1, uint32 code2, uint32 code3)
{
   return;
}

void err_Fatal2 (const msg_const_type* const_blk, uint32 code1, uint32 code2)
{
   return;
}

void err_Fatal1 (const msg_const_type* const_blk, uint32 code1)
{
   return;
}

void err_Fatal0 (const err_FatalParam_type* const_blk)
{
	return;
}
#endif

void msg_v2_send_3 ( const msg_v2_const_type * xx_msg_const_ptr, uint32 xx_arg1,
    uint32 xx_arg2, uint32 xx_arg3)
{
	return;
}

void msg_v3_send_3 ( const msg_v3_const_type * xx_msg_const_ptr, uint32 xx_arg1,
    uint32 xx_arg2, uint32 xx_arg3)
{
  return;
}

int qurt_thread_get_priority (qurt_thread_t threadid)
{
	return 0;
}

int qurt_thread_set_priority (qurt_thread_t threadid, unsigned short newprio)
{
	return 0;
}
