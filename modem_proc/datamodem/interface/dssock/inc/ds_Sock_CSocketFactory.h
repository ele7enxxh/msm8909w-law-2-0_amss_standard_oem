#ifndef DS_SOCK_CSOCKETFACTORY_H
#define DS_SOCK_CSOCKETFACTORY_H

/*
  ===============================================================================
  FILE: ds_Sock_CSocketFactory.idl

  DESCRIPTION: This file defines the class id that implements the 
  ds_Sock_ISocketFactory interface

  Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  ===============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#if !defined(AEEINTERFACE_CPLUSPLUS)
#include "ds_Utils_StdDef.h"
#define ds_Sock_AEECLSID_CSocketFactory 0x10751e7
#else /* C++ */
#include "ds_Utils_StdDef.h"
namespace ds
{
   namespace Sock
   {
      const ::AEECLSID AEECLSID_CSocketFactory = 0x10751e7;
   };
};
#endif /* !defined(AEEINTERFACE_CPLUSPLUS) */
/*
  ================================================================================
  Class DOCUMENTATION
  ================================================================================
  
  AEECLSID_DSSockSocketFactory
  
  Description:
  Allows clients to create an instance of ds_Sock_ISocketFactory interface,
  which can be used to create ds_Sock_ISocket objects.
  
  Default Interfaces:
  ds_Sock_ISocketFactory
  
  Other Supported Interfaces (directly):
  None
  
  Other Supported Interfaces (indirectly):
  ds_Sock_ISocket - Can be obtained via ISocketFactory_CreateSocket* methods.
  Other Interfaces supported from ds_Sock_ISocket (via IQI):
  ISocketExt.
  
  See Also:
  None
  ================================================================================*/

#endif //DS_SOCK_CSOCKETFACTORY_H
