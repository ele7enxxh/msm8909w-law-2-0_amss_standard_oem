#ifndef __DSS_GLOBALS_H__
#define __DSS_GLOBALS_H__

/*======================================================

FILE:  DSS_Globals.h

SERVICES:
Global definitions of Backward Compatibility Layer set up in
a Singleton class

GENERAL DESCRIPTION:

=====================================================

Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_Globals.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-11-01 rk  Removed feature mobileap.
  2010-04-18 en  History added.

===========================================================================*/


//===================================================================
//   Includes and Public Data Declarations
//===================================================================

//-------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------
#include "comdef.h"
#include "dssocket.h"
#include "dserrno.h"

#include "ds_Utils_ISignalFactory.h"

#include "customer.h"

#include "ds_Utils_StdErr.h"

#include "DSS_NetActiveScope.h"
#include "DSS_NetMonitoredScope.h"
#include "ds_Net_INetworkFactory.h"
#include "ds_Sock_ISocketFactory.h"
#include "DSS_MapObject2Descriptor.h"
#include "DSS_NetApp.h"
#include "DSS_Socket.h"
#include "ps_mem.h"
#include "dss_config.h"
#include "ds_Utils_Singleton.h"

//-------------------------------------------------------------------
// Constant / Define Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Type Declarations (typedef, struct, enum, etc.)
//-------------------------------------------------------------------
typedef struct FilterRegInfo
{
   ds::Net::IIPFilterRegPriv* filterReg;
   dss_iface_id_type iface_id;
   sint15 sockfd;
   struct FilterRegInfo* next;

   void * operator new
   (
      unsigned int numBytes
   )  throw()
   {
      return ps_mem_get_buf( PS_MEM_DSAL_FILTER_REG_INFO_TYPE);
   } 

   void operator delete
   (
      void *  bufPtr
   )
   {
      if(NULL != bufPtr)
      {
         PS_MEM_FREE(bufPtr);
         return;
      }    
   } 
} FilterRegInfo;

//-------------------------------------------------------------------
// Global Constant Data Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Data Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Forward Declarations
//-------------------------------------------------------------------

sint15 GetNetHandleFromQoSRequestHandle(dss_iface_ioctl_type ioctl_name,void* argval_ptr);

//===================================================================
//              Macro Definitions
//===================================================================
#define DSS_MAX_SINT15 32767

// NetApp max descriptor (handle) is max of its type - sint15
#define DSS_GLOBALS_BASE_DESCRIPTOR_MIN_NETAPP (1)
#define DSS_GLOBALS_BASE_DESCRIPTOR_MAX_NETAPP (DSS_MAX_SINT15)

// limitation for maximal handle comes from
// prototype of DS API socket related functions,
// see dss_socket() for example
// limitation for minimal handle comes from
// definition that 0 is not a valid socket handle
#define DSS_GLOBALS_BASE_DESCRIPTOR_MIN_SOCKETS (1)
#define DSS_GLOBALS_BASE_DESCRIPTOR_MAX_SOCKETS (DSS_MAX_SINT15)

//===================================================================
//              Class Definitions
//===================================================================

//===================================================================
//  CLASS:      DSSGlobals
//
//  DESCRIPTION:
//
//  HOW TO USE:
//
//===================================================================
enum
{
   NUM_OF_FLOW_IDS = 256,
   NUM_OF_QOS_NET_INITIATED_REQUESTS = 64
};

class DSSGlobals : public ds::Utils::Singleton<DSSGlobals>
{
  friend class ds::Utils::Singleton<DSSGlobals>;
//-------------------------------------------------------------------
//  Constructors/Destructors
//-------------------------------------------------------------------
  private:
     DSSGlobals();
  public:
     virtual ~DSSGlobals();

//-------------------------------------------------------------------
//  Interface members
//-------------------------------------------------------------------
  public:
    static DSSGlobals* Instance();
    static void Init();
    static void Deinit();
    AEEResult  GetCritSect(ICritSect** ppIcritSect);
    AEEResult  GetNetworkFactory(ds::Net::INetworkFactory **ppNetworkFactory);
    // TODO: the parameter should be of type that implements IDSSockFactory
    void      GetSockFactory(ds::Sock::ISocketFactory **ppFactory);
    AEEResult GetSignalFactory(ISignalFactory **ppFactory);
    // TODO: allow ppNetApp to be NULL if we only want to check if there's such handle
    AEEResult GetNetApp(sint15 netHandle, DSSNetApp **ppNetApp);
    AEEResult GetNetObjects(sint15 netHandle,
                            dss_iface_id_type iface_id,
                            bool bAllowSecondary,
                            bool bAllowToCreateSecondary,
                            bool bForbidTemporary,
                            DSSNetApp** pNetApp,
                            DSSNetActiveScope* pNetActiveScope,
                            DSSNetMonitoredScope* pNetMonitoredScope,
                            dss_iface_ioctl_type ioctl_name,
                            void* argval_ptr);
    bool      AreFlowIDsReleased(void);
    bool      IsValidNetApp(sint15 handle);
    sint15    InsertNetApp(DSSNetApp *pNetApp);
    AEEResult RemoveNetApp(sint15 netHandle);
    AEEResult RemoveSocket(sint15 sockFd);
    AEEResult GetSocketById(sint15 sockfd, DSSSocket **ppDSSSocket);
    AEEResult FindSocketWithEvents(sint15 dss_nethandle, sint15 *sockfd_ptr, uint32 *eventOccurred, uint32 *sigOnOff);
    // Creates a DSSSocket and inserts it into the global list. kind can be 1 or 2
    AEEResult CreateSocket(dss_sock_cb_fcn_type sockCb, sint15 sNetApp,
                           sint15* psSockFd, DSSSocket** ppDSSSocket, int kind,
                           byte protocol);
    AEEResult GetFreeQoSFlowID(uint8 *pFlowID);
#ifdef FEATUTE_DATA_PS_MCAST
    AEEResult GetFreeMCastFlowID(uint8 *pFlowID);
#endif // FEATUTE_DATA_PS_MCAST
    AEEResult GetFreeQoSNetInitiatedReqHandle(dss_qos_net_initiated_req_handle_type* handle);
    void ReleaseQoSFlowID(uint8 flowID) throw();
#ifdef FEATUTE_DATA_PS_MCAST
    void ReleaseMCastFlowID(uint8 flowID) throw();
#endif // FEATUTE_DATA_PS_MCAST
    void ReleaseQoSNetInitiatedReqHandle(dss_qos_net_initiated_req_handle_type handle) throw();

    // in this function we AddRef to the filterReg that is being stored inside the DSSGlobals
    AEEResult AddFilterRegObjectToList(ds::Net::IIPFilterRegPriv* filterReg, dss_iface_id_type iface_id, sint15 sockfd);
    AEEResult RemoveFilterRegObjectFromList(dss_iface_id_type iface_id, sint15 sockfd);
    AEEResult RemoveFilterRegObjectFromList(sint15 sockfd);
    AEEResult RemoveFilterRegObjectFromListByHandle(uint32 filterHandle);
    bool      SocketListFull();
    bool      NetAppListFull();

//-------------------------------------------------------------------
//  Get/Set functions for protected members access
//
//-------------------------------------------------------------------

//-------------------------------------------------------------------
//  Protected members
//-------------------------------------------------------------------

//-------------------------------------------------------------------
//  Private members
//-------------------------------------------------------------------
  private:
     AEEResult CreateTempDSSNetMonitored(DSSNetMonitored** ppNetMonitored, dss_iface_id_type iface_id);
     sint15    InsertSocket(DSSSocket *pSocket);
     AEEResult RemoveAllMatchingFilterRegObjectFromList(dss_iface_id_type iface_id, sint15 sockfd);

//-------------------------------------------------------------------
//  Data members
//-------------------------------------------------------------------
   private:
      ds::Net::INetworkFactory* mpNetFactory;
      ds::Sock::ISocketFactory* mpSockFactory;
      ISignalFactory* mpSignalFactory;
      ICritSect* mpCritSect;
      bool mQoSFlowIDs[NUM_OF_FLOW_IDS];
#ifdef FEATUTE_DATA_PS_MCAST
      bool mMCastFlowIDs[NUM_OF_FLOW_IDS];
#endif // FEATUTE_DATA_PS_MCAST
      bool mQoSNetInitiatedHandles[NUM_OF_QOS_NET_INITIATED_REQUESTS];
      int mLastProvidedQoSNetInitiatedHandle; 

      DSSMapObject2Descriptor<DSSNetApp, DSS_MAX_APPS> mMapNetApp2Descriptor;
      DSSMapObject2Descriptor<DSSSocket, DSS_MAX_SOCKS> mMapSocket2Descriptor;

      FilterRegInfo* mFilterRegInfoList;

      int mTcpSocketsNum;
      int mUdpSocketsNum;      
//-------------------------------------------------------------------
//  Memory management
//-------------------------------------------------------------------

   public :
      void * operator new
      (
         unsigned int numBytes
      )  throw();

      void operator delete
      (
         void *  bufPtr
      );
};

//===================================================================

#endif // __DSS_GLOBALS_H__
