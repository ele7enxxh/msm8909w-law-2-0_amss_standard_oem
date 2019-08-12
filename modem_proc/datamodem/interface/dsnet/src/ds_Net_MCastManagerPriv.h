#ifndef DS_NET_MCAST_MANAGER_PRIV_H
#define DS_NET_MCAST_MANAGER_PRIV_H
#ifdef FEATUTE_DATA_PS_MCAST
/*==========================================================================*/
/*!
  @file
  ds_Net_MCastManagerPriv.h

  @brief
  This file defines the class that implements the IMCastManagerPriv 
  interface.

  @details
  The MCastManagerPriv class (ds::Net::MCastManagerPriv) 
  implements the following interfaces:
  IQI
  IMCastManagerPriv

  @todo
  Write details

  Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_MCastManagerPriv.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-07-29 vm  Created module.

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
#include "ds_Net_IMCastManagerPriv.h"

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
/*lint -esym(1510, IMCastManagerPriv) */
/*lint -esym(1510, IQI) */
class MCastManagerPriv : public IMCastManagerPriv
{

private:
  int32                 mIfaceHandle;
  ICritSect*            mpICritSect;

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
  virtual ~MCastManagerPriv
  (
    void
  )
  throw();


public:
  MCastManagerPriv
  (
    int32      ifaceHandle
  );

  /*-------------------------------------------------------------------------
    Inherited functions from IMCastManagerPriv.
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
  associated with the MCastManagerPriv object.

  @param[in]  addr - Socket address for the Multicast group.
  @param[in]  info - Info on Multicast group to join.
  @param[out] session - Out param for created MCast session.

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
    const SockAddrStorageType      addr,
    IQI*                           info,
    IMCastSessionPriv **           session
  );

  /*!
  @brief
  This function issues a bundled request to join several multicast groups
  at once.

  This function issues a bundled request to join several multicast groups
  at once. This function creates MCastSession objects for each of the
  multicast handles that are created for the bundle. This function only
  issues request to join the multicast groups, but it does not guarantee
  that the join requests would all be successful.

  @param[in]  addrSeq - Socket address list for the Multicast groups
  @param[in]  addrSeqLen - Number of multicast socket addresses
  @param[in]  mcastSpecInfoSeq - Multicast Join info list.
  @param[in]  mcastSpecInfoSeqLen - Number of multicast join infos in the list.
  @param[in]  mcastJoinFlagsSeq - MCast register flags list
  @param[in]  mcastJoinFlagsSeqLen - number of MCast register flags in the list
  @param[out] sessions - Out param for created MCast sessions.
  @param[in]  sessionLen - Max space in sessions array.
  @param[out] sessionLenReq - Out param for number of Mcast sessions created.

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
    IQI **                            mcastSpecInfoSeq,
    int                               mcastSpecInfoSeqLen,
    const MCastJoinFlagsType *        mcastJoinFlagsSeq,
    int                               mcastJoinFlagsSeqLen,
    IMCastSessionPriv **              sessions,
    int                               sessionsLen,
    int *                             sessionsLenReq
  );

  /*!
  @brief
  This function issues a bundled request to leave several multicast groups
  at once.

  @details
  This function issues a bundled request to leave several multicast groups
  at once. This function performs an extended Iface IOCTL to leave all the
  groups which are given as arguments. The MCast sessions should already
  have been created through Join or JoinBundle methods of MCastManagerPriv.

  @param[in]  sessions - Out param for created MCast sessions.
  @param[in]  sessionLen - Number of Mcast sessions to leave.

  @see        No external dependencies

  @see        ds::Sock::SockAddrStorageType
  @see        IMCastSession
  @see        IMCastManager::Join()
  @see        IMCastManager::JoinBundle()

  @return     AEE_SUCCESS - if the request is issued to leave the
              MCast group successfully. This does not mean that Mcast group
              is leaved already.
  @return     QDS_EFAULT - Invalid arguments.
  @return     Other ds Error codes may be returned.
  */
  virtual ds::ErrorType CDECL LeaveBundle
  (
    IMCastSessionPriv **  sessions,
    int                   sessionsLen
  );

  /*!
  @brief
  This function issues a request for registering multiple multicast

  @details
  This function performs an extended Iface IOCTL to register all the
  groups which are given as arguments. The MCast sessions should already
  have been created through Join or JoinBundle methods of MCastManagerPriv.

  @param[in]  sessions - Out param for created MCast sessions.
  @param[in]  sessionLen - Number of Mcast sessions to leave.

  @return     AEE_SUCCESS - if the request is issued to register the
              MCast group successfully. This does not mean that Mcast group
              is registered already.
  @return     QDS_EFAULT - Invalid arguments.
  @return     Other ds Error codes may be returned.

  */
  virtual ds::ErrorType CDECL RegisterBundle
  (
    IMCastSessionPriv **  sessions,
    int                   sessionsLen
  );

  /*!
  @brief
  This function creates an instance of IMBMSSpecPriv. 
  
  @details
  IMBMSSpecPriv creation is supported only via IMCastManagerPriv.
  IMBMSSpecPriv instance can be used to specify MBMS MCast Join information
  in IMCastManagerPriv::Join and IMCastManagerPriv::JoinBundle.

  @param[out] ppIMBMSSpec  - Output The newly created IMBMSSpecPriv instance.

  @retval AEE_SUCCESS  - IMBMSSpecPriv created successfully.
  @retval Other ds designated error codes might be returned.

  @see IMCastManagerPriv::Join
  @see ds_Errors_Def.idl.
  */
  virtual ds::ErrorType CDECL CreateMBMSSpecPriv
  (
    IMBMSSpecPriv**         ppIMBMSSpec
  );

  /*-------------------------------------------------------------------------
    IQI interface Methods
  -------------------------------------------------------------------------*/
  DSIQI_IMPL_DEFAULTS_NO_QI()

  virtual int CDECL QueryInterface (AEEIID iid, void **ppo);

  /*-------------------------------------------------------------------------
    Overload new/delete operators.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS(PS_MEM_DS_NET_MCAST_MANAGER_PRIV)

};/* class MCastManagerPriv */
} /* namespace Net */
} /* namespace ds */
#endif // FEATUTE_DATA_PS_MCAST
#endif /* DS_NET_MCAST_MANAGER_PRIV_H */

