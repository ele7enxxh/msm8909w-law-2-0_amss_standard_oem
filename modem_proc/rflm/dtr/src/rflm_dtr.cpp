/*!
   @file
   dtr.cpp

   @brief
*/

/*===========================================================================

Copyright (c) 2010 - 2013 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/dtr/src/rflm_dtr.cpp#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
01/08/14   cvd     Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rflm_dtr.h"
#include "rflm_rw_lock.h"
#include "rxlm_intf.h"
#include <stringl/stringl.h>


/*===========================================================================
                           GLOBAL VARIABLES
===========================================================================*/
#if (defined(TEST_FRAMEWORK) && defined(FEATURE_BOLT_MODEM))
#error code not present
#endif
rflm_rw_lock_t rflm_dtr_handle_locks[RXLM_MAX_BUFFERS];
uint32 rflm_dtr_handle_locks_rf_locked[RXLM_MAX_BUFFERS];
#if (defined(TEST_FRAMEWORK) && defined(FEATURE_BOLT_MODEM))
#error code not present
#endif


/*----------------------------------------------------------------------------*/
/*!
   @brief
      Print out a Memory Chunk.
 
   @details
      Print out a Memory Chunk addr upto a length of size.
      Prints 5 words a message, therby reducing the number of messages.

   @retval
      void
*/

void dtr_print_memory_chunk(uint8* addr, uint32 size)
{
//Loop variable for addr
  uint32 i = 0;
//Variable that sets how far to print out.
  uint32 j = 0;
//Loop variable for addr_print
  uint32 k = 0;

  for (i = 0; i < size;)
  {
    j+=5;
    if (j > size)
    {
      j = size;
    }

    uint8 addr_print[5] = { 0 };
    
    // Go 5 more steps or until i = size
    for (k = 0; i < j; i++, k++)
    {
      addr_print[k] = addr[i];
    }

    RFLM_DIAG_TRACE_LOW(RFLM_DIAG_MSG_SSID_NONE, "addr -> 0x%X 0x%X 0x%X 0x%X 0x%X", addr_print[0], addr_print[1], addr_print[2], addr_print[3], addr_print[4]);
  }
}

