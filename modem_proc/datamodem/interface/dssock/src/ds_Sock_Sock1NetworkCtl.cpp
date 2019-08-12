/*===========================================================================
FILE: ds_Sock_Sock1NetworkCtl.cpp

OVERVIEW: 
This file provides implementation of the Sock1NetworkCtl class.

DEPENDENCIES: None

Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
EDIT HISTORY FOR MODULE

Please notice that the changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_Sock1NetworkCtl.cpp#1 $
$DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

when       who what, where, why
---------- --- ------------------------------------------------------------
2010-01-05 vm Created module

===========================================================================*/

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "ds_Sock_Sock1NetworkCtl.h"
#include "ds_Net_INetwork.h"
#include "ps_mem.h"

/*===========================================================================

LOCAL DATA DEFINITIONS

===========================================================================*/
using namespace ds::Net;
using namespace ds::Sock;
using namespace ds::Error;

/*===========================================================================

PUBLIC CLASS METHODS

===========================================================================*/

Sock1NetworkCtl::Sock1NetworkCtl
(
  void
)  : NetworkCtl()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

}/* Sock1NetworkCtl::Sock1NetworkCtl() */

ds::ErrorType Sock1NetworkCtl::Clone
(
  ICritSect       *critSect,
  NetworkCtl    **networkCtlPtr
)
{
  ds::ErrorType    result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(0 == networkCtlPtr)
  {
    result = QDS_EFAULT;
    goto bail;
  }

  *networkCtlPtr = new Sock1NetworkCtl();
  
  if(0 == *networkCtlPtr)
  {
    result = AEE_ENOMEMORY;
    goto bail;
  }

  result = (*networkCtlPtr)->Init(critSect);
  if(AEE_SUCCESS != result)
  {
    result = AEE_ENOMEMORY;
    goto bail;
  }

  result = NetworkCtl::CloneMembers(networkCtlPtr);
  if(AEE_SUCCESS != result)
  {
    goto bail;
  }

  return AEE_SUCCESS;

bail:
  if (0 != networkCtlPtr) 
  {
    if(0 != *networkCtlPtr)
    {
      (void) (*networkCtlPtr)->Release();
    }
  }

  return result;
}/* Sock1NetworkCtl::Clone() */

Sock1NetworkCtl::~Sock1NetworkCtl
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

}/* Sock1NetworkCtl::~Sock1NetworkCtl() */

boolean Sock1NetworkCtl::IsPolicyUpdateEnabled
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return false;
}/* Sock1NetworkCtl::IsPolicyUpdateEnabled() */

void * Sock1NetworkCtl::operator new
(
  unsigned int numBytes
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ps_mem_get_buf( PS_MEM_SOCK_1_NET_CTL_TYPE);
} /* Sock1NetworkCtl::operator new() */

void Sock1NetworkCtl::operator delete
(
  void *  bufPtr
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == bufPtr)
  {
    ASSERT( 0);
    return;
  }

  PS_MEM_FREE( bufPtr);
  return;

} /* Sock1NetworkCtl::operator delete() */

Sock1NetworkCtl * Sock1NetworkCtl::CreateInstance
(
  void
)
{
  Sock1NetworkCtl *  newNetCtl = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
  Create a Network Controller
  -------------------------------------------------------------------------*/
  newNetCtl = new Sock1NetworkCtl();
  if (0 == newNetCtl)
  {
    LOG_MSG_ERROR_0("Sock1NetworkCtl::CreateInstance(): "
                    "No mem for Sock1NetworkCtl");
    goto bail;
  }

  return newNetCtl;

bail:

  return 0;
}/* Sock1NetworkCtl::CreateInstance() */

