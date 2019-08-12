#ifndef DS_NET_MCAST_MANAGER_H
#define DS_NET_MCAST_MANAGER_H
#ifdef FEATUTE_DATA_PS_MCAST
/*==========================================================================*/
/*!
  @file
  ds_Net_MCastManager.h

  @brief
  This file defines the class that implements the IMCastManager 
  interface.

  @details
  The MCastManager class (ds::Net::MCastManager) implements the following
  interfaces:
  IQI
  IEventManager
  IMCastManager
  IMCastManagerExt
  IMCastManagerBCMCS
  IMCastManagerMBMSPriv

  @todo
  Write details

  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_MCastManager.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-12-22 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_StdErr.h"
#include "ds_Utils_StdErr.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Utils_Factory.h"
#include "ds_Utils_ICritSect.h"
#include "ds_Net_Utils.h"
#include "ds_Net_MemManager.h"
#include "ds_Net_IMCastManager.h"
#include "ds_Net_IMCastManagerExt.h"
#include "ds_Net_IMCastManagerBCMCS.h"
#include "ds_Net_IMCastManagerMBMSPriv.h"
#include "ds_Net_IMBMSSpecPriv.h"
#include "ds_Net_IMCastManagerPriv.h"
#include "ds_Net_INetworkFactory.h"

/*===========================================================================
                     FORWARD DECLERATION
===========================================================================*/

/*===========================================================================

                     PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
namespace Net
{
/*lint -esym(1510, IMCastManager) */
/*lint -esym(1510, IMCastManagerExt) */
/*lint -esym(1510, IMCastManagerBCMCS) */
/*lint -esym(1510, IMCastManagerMBMSPriv) */
/*lint -esym(1510, IQI) */
class MCastManager : public IMCastManager,
                     public IMCastManagerExt,
                     public IMCastManagerBCMCS,
                     public IMCastManagerMBMSPriv
{

private:
  int32                 mIfaceHandle;
  ICritSect*            mpICritSect;
  IMCastManagerPriv *   mpMCastManagerPriv;
  NetworkModeType       mNetworkMode;

  /*!
  @brief
  Private destructor.

  @details
  The destructor of this object is private. Use Release() method of the
  IQI interface to free the object.

  @param      None.
  @return     None.
  @see        IQI::Release()
  */
  virtual ~MCastManager
  (
    void
  )
  throw();

public:
  MCastManager
  (
    int32      ifaceHandle,
    NetworkModeType networkMode
  );

  /*-------------------------------------------------------------------------
    Inherited functions from IMCastManager.
  -------------------------------------------------------------------------*/
  /*!
  @function
  GetTechObject()

  @brief
  This function allows object extensibility.
  For supported interfaces, objects implementing those interfaces may be
  fetched via this function.

  @details
  The supported interfaces are documented in
  the DS_NET_NetworkFactory.bid file. GetTechObject-supported interface
  does not imply any similar contract in regard to QueryInterface for the
  respective interface.
  Unlike IQI, the availability of a specific interface depends on some
  factors, e.g. current network type. Moreover, there is no guaranty
  for transitivity or symmetry.

  @param[in]  iid  - The interface that should be retrieved.
  @param[out] ppo  - On success, will contain the requested
  interface instance.

  @return     AEE_SUCCESS on success
  @return     QDS_EFAULT - Invalid arguments
  @return     AEE_ECLASSNOTSUPPORT - Class ID is not supported.
  */
  virtual ds::ErrorType CDECL GetTechObject
  (
    AEEIID                                  iid,
    void**                                   ppo
  );

  /*!
  @brief
  This function issues a request to join a Multicast group.

  @details
  This function issues a request to join a Multicast group on the iface
  associated with the MCastManager object.

  @param[in]  addr - Socket address for the Multicast group.
  @param[in]  mcastSpec - MCast spec in JSON format.
  @param[out] session - Out param for created MCast session.
  @param[out] errSpec - MCast error spec in JSON format.
  @param[in]  errSpecLen - length of provided MCast spec in JSON format.
  @param[out] errSpecLenReq - length required for storing 
                              MCast error spec in JSON format.

  @see        ds::Sock::SockAddrStorageType
  @see        IMCastSession

  @return     AEE_SUCCESS - if the request is issued to join the
              MCast group successfully. This does not mean that Mcast group
              is joined already.
  @return     QDS_EFAULT - Invalid arguments.
  @return     Other ds Error codes may be returned.
  */
  virtual ds::ErrorType CDECL Join
  (
    const ::ds::SockAddrStorageType addr,
    const char *                    mcastSpec,
    IMCastSession **                session,
    char *                          errSpec,
    int                             errSpecLen,
    int *                           errSpecLenReq
  );

  /*---------------------------------------------------------------------
    Inherited functions from IMCastManagerExt
  ---------------------------------------------------------------------*/
  /*!
  @brief
  This function issues a request for joining multiple multicast
  groups.

  @details
  In order to receive the MCast data, the application must also 
  register to the MCast groups using the RegisterBundle API.
  Applying JoinBundle allows applications to commit the "Join" part
  of the MCast setup at an early stage (e.g. by Joining all available
  MCast groups) and apply only the RegisterBundle API once the User
  is requesting service, thus minimizing the setup time required from
  the moment the User requests the service.

  @param[in]  addrSeq  - Multicast group addresses.
  @param[in]  addrSeqLen - Number of multicast socket addresses
  @param[in]  mcastSpecs - A JSON string specifying extra Multicast join
                           information. The extra Multicast join information
                           is supported only for certain MCast technologies.
                           When using this API to join other MCast
                           technologies, mcastSpecs can be empty.          
                           The application should add the Mcast Specs info
                           to the JSON string in the same order of the 
                           addresses in addrSeq.          
                           Currently extra Join information is not applicable
                           for supported technologies hence no JSON schema
                           is currently specified for this information.
  @param[out]  sessions - Output IMCastSessionsOutput containing IMCastSession 
                          objects opened for handling the requests.
                          The order of sessions shall correspond to the
                          order of addresses in addrSeq.
  @param[out]  errSpec - MCast error spec in JSON format.
  @param[in]   errSpecLen - length of provided MCast spec in JSON format.
  @param[out]  errSpecLenReq - length required for storing 
                               MCast error spec in JSON format.

  @See        RegisterBundle.
  @see        ds::Sock::SockAddrStorageType
  @see        IMCastSession

  @return     AEE_SUCCESS - if the request is issued to join the
              MCast group successfully. This does not mean that Mcast group
              is joined already.
  @return     QDS_EFAULT - Invalid arguments.
  @return     Other ds Error codes may be returned.
  */
  virtual ds::ErrorType CDECL JoinBundle
  (
    const ::ds::SockAddrStorageType * addrSeq,
    int                               addrSeqLen,
    const char *                      mcastSpecs,
    IMCastSessionsOutput **           sessions,
    const MCastJoinFlagsType *        mcastJoinFlagsSeq,
    int                               mcastJoinFlagsSeqLen,
    char *                            errSpec,
    int                               errSpecLen,
    int *                             errSpecLenReq
  );

  /*!
  @brief
  This function issues a request for registering multiple multicast
  groups.

  @details
  This function performs an extended Iface IOCTL to register all the
  groups which are given as arguments. The IMCastSessionsInput
  should already has been created through JoinBundle method.

  @param[in] sessions - A MCastSessions group with which all
                        IMcastSessions objects to register are associated.

  @see IMCastSessionsInput, IMcastSession.
  @see ds_Errors_Def.idl.

  @retval AEE_SUCCESS Request received successfully.
  @retval Other ds designated error codes might be returned.
  */
  virtual ds::ErrorType CDECL RegisterBundle
  (
    IMCastSessionsInput* sessions
  );

  /*!
  @brief
  This function issues a request for leaving multiple multicast
  groups.

  @details
  This function issues a bundled request to leave several multicast groups
  at once. This function performs an extended Iface IOCTL to leave all the
  groups which are given as arguments. The IMCastSessionsInput
  should already have been created through JoinBundle method of MCastManager.

  @param[in] sessions  - A MCastSessions group with which all
                         IMcastSessions objects to leave are associated.

  @see IMCastSessionsInput, IMcastSession.
  @see ds_Errors_Def.idl.

  @retval AEE_SUCCESS Request received successfully.
  @retval Other ds designated error codes might be returned.
  */
  virtual ds::ErrorType CDECL LeaveBundle
  (
    IMCastSessionsInput * sessions
  );

  /**
  @brief
  This function creates an instance of IMCastSessionsInput.

  @details
  Creates container for IMCastSessions
  IMCastSessionsInput creation is supported only via IMCastManagerExt.

  @param[out] newMCastSessionsInput  - Output The newly created 
                                       IMCastSessionsInput instance.

  @retval AEE_SUCCESS IMCastSessionsInput created successfully.
  @retval Other ds designated error codes might be returned.
  
  @see ds_Errors_Def.idl.
  */
  virtual ds::ErrorType CDECL CreateMCastSessionsInput
  (
    IMCastSessionsInput ** newMCastSessionsInput
  );

  /*-------------------------------------------------------------------------
    Inherited functions from IMCastManagerMBMSPriv.
  -------------------------------------------------------------------------*/
  /*!
  @function
  Activate()

  @brief
  Activates the MBMS Mcast context.

  @details
  TODO

  @param[in]  addr - MBMS address to join
  @param[in]  pdpNumber - PDP context number.
  @param[out] ppNetMCastMBMSCtr - Returned IMCastMBMSCtrlPriv interface.

  @return     AEE_SUCCESS - on success
  @return     QDS_EFAULT - Invalid arguments

  @see        ds::IPAddrType
  @see        IMCastMBMSCtrlPriv
  */
  virtual ds::ErrorType CDECL Activate
  (
    const ds::IPAddrType     *pAddr,
    int                      pdpNumber,
    IMCastMBMSCtrlPriv       **ppIMCastCastMBMSCtrl
  );

  /*---------------------------------------------------------------------
    Inherited functions from IMCastManagerBCMCS.
  ---------------------------------------------------------------------*/
  /**
  @brief
  Appends the given spec to the device's BCMCS database.

  @details
  This function appends the given spec to the device's BCMCS database.
  If the BCMCS DB already contains a BCMCS record with the same IP
  Address, BCMCSPort and Zone, it shall be overwritten by the
  BCMCS specification provided in this API.

  @param[in] bcmcsDBSpec  - Structure that holds all the information to
                            be updated.
  @see IBCMCSDBSpec.
  @retval ds::SUCCESS Request received successfully.
  @retval Other ds designated error codes might be returned.
  @see ds_Errors_Def.idl.
  */

  virtual ds::ErrorType CDECL UpdateDB
  (
    const ds::Net::BCMCSDBSpecType* updateParam
  );

  /**
  @brief
  Issues a request for registering multiple multicast groups using
  handoff optimization

  @details
  This function issues a request for registering multiple multicast groups using
  handoff optimization.

  @param[in] addrSeq    - Multicast group addresses.
  @param[in] addrSeqLen - Multicast group addresses length.

  @see IMCastSession
  @retval ds::SUCCESS Request received successfully.
  @retval Other ds designated error codes might be returned.
  @see ds_Errors_Def.idl.
  */
  virtual ds::ErrorType CDECL RegisterUsingHandoffOpt
  (
    const ::ds::SockAddrStorageType* addrSeq,
    int addrSeqLen
  );

  /*-------------------------------------------------------------------------
    IQI interface Methods
  -------------------------------------------------------------------------*/
  DSIQI_IMPL_DEFAULTS_NO_QI()

  virtual int CDECL QueryInterface (AEEIID iid, void **ppo);

  /*-------------------------------------------------------------------------
    Overload new/delete operators.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS(PS_MEM_DS_NET_MCAST_MANAGER)

};/* class MCastManager */
} /* namespace Net */
} /* namespace ds */
#endif // FEATUTE_DATA_PS_MCAST
#endif /* DS_NET_MCAST_MANAGER_H */

