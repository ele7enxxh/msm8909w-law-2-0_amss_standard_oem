/*!
   @file
   rfa.cpp

   @brief
   This file contains implementation the RFA class.
*/

/*===========================================================================

Copyright (c) 2011-12 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/rfc/src/rfa.cpp#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
05/09/12   sr      Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfa.h"
#include "rfcommon_msg.h"
#include "modem_mem.h"

// Constructor 
rfa::rfa() 
{

}

// Destructor
rfa::~rfa()
{

}

void* rfa::operator new(size_t size)
{
  return modem_mem_alloc(size,MODEM_MEM_CLIENT_RFA);
}

void rfa::operator delete(void* ptr)
{
  modem_mem_free(ptr,MODEM_MEM_CLIENT_RFA);
}
