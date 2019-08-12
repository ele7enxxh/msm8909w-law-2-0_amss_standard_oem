#ifndef DS_NET_MCAST_SESSION_PRIV_H
#define DS_NET_MCAST_SESSION_PRIV_H
#ifdef FEATUTE_DATA_PS_MCAST
/*===========================================================================
  @file ds_Net_MCastSessionPriv.h

  This file defines the class that implements ds_Net_IMCastSessionPriv interface.

  The MCastSessionPriv class (ds::Net::MCastSessionPriv) implements the following 
  interfaces: IQI and IMCastSessionPriv.

  Copyright (c) 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_MCastSessionPriv.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-07-29 vm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Net_IMCastSessionPriv.h"
#include "ds_Net_Handle.h"
#include "ds_Net_Platform.h"
#include "ds_Net_MemManager.h"
#include "ds_Net_MCastManager.h"

/*Suppress lint error because of namespace collisions */
/*lint -save -e578 */
/*lint -esym(1510, IMCastSessionPriv) */
/*lint -esym(1510, IEventManager) */
/*lint -esym(1510, IQI) */
namespace ds
{
  namespace Net
  {
    class MCastSessionPriv: public  IMCastSessionPriv, 
                            public  Handle
    {
      private:
        int32                   mMCastHandle;
        ISignalBus *            mpSigBusRegStatus1_0;
        ISignalBus *            mpSigBusRegStatus2_0;
        ISignalBus *            mpSigBusTechStatus;
        MCastStateChangedType   mRegState_1_0;
        MCastStateChangedType   mRegState_2_0;
        MCastInfoCodeType       mStatusInfoCode;
        MCastJoinFlagsType      mJoinFlag;

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

        virtual void Destructor
        (
          void 
        )
        throw();
  
        /*!
        @brief
        Dummy destructor. Only used to free memory. 
        
        @details
        For objects that support weak references, destruction of the object and
        freeing up of memory can happen at different times. Hence Destructor()
        is the one which performs actual destruction of the object, where as 
       ~Object() would perform memory cleanup. 
        @params     None.
        @return     None.
       */
        virtual ~MCastSessionPriv
        (
          void 
        ) 
        throw();

      public:
        MCastSessionPriv
        (
          int32 ifaceHandle,
          int32 mcastHandle,
          MCastJoinFlagsType regFlag
        );

        /*---------------------------------------------------------------------
          Inherited functions from IMCastSessionPriv
        ---------------------------------------------------------------------*/
        /**
        @brief
        This function issues a request for leaving a multicast group.

        @retval AEE_SUCCESS Request received successfully.
        @retval Other ds designated error codes might be returned.
        @see ds_Errors_Def.idl.
        */
        virtual ds::ErrorType CDECL Leave
        (
          void
        );

        /**
        @brief
        This function provides the MCast session registration state and
        information code. DSS fetch this information once 
        QDS_EV_REGISTRATION_STATE_PRIV_BCMCS1_0 events occurs.

        @retval AEE_SUCCESS Request received successfully.
        @retval Other ds designated error codes might be returned.
        @see ds_Errors_Def.idl.
        */
        virtual ds::ErrorType CDECL GetRegStateAndInfoCodeBCMCS1_0
        (
          MCastStateChangedType* RegStateAndInfoCodeBCMCS1_0
        );

        /**
        @brief
        This function provides the MCast session registration state and
        information code. DSS fetch this information once 
        QDS_EV_REGISTRATION_STATE_PRIV_BCMCS2_0 events occurs.

        @retval AEE_SUCCESS Request received successfully.
        @retval Other ds designated error codes might be returned.
        @see ds_Errors_Def.idl.
        */
        virtual ds::ErrorType CDECL GetRegStateAndInfoCodeBCMCS2_0
        (
          MCastStateChangedType* RegStateAndInfoCodeBCMCS2_0
        );

        /**
        @brief
        This function provides the information code for the status of the
        MCast technology session.
        DSS fetch this information onceQDS_EV_TECHNOLOGY_STATUS_PRIV event
        occurs.

        @retval AEE_SUCCESS Request received successfully.
        @retval Other ds designated error codes might be returned.
        @see ds_Errors_Def.idl.
        */
        virtual ds::ErrorType CDECL GetTechStatusInfoCode
        (
          MCastInfoCodeType* TechStatusInfoCode
        );

        /*-------------------------------------------------------------------------
          IEventManager interface forwarders:
        -------------------------------------------------------------------------*/
        virtual ds::ErrorType CDECL OnStateChange
        (
          ISignal             *pISignal,
          ds::Net::EventType   eventName,
          IQI **               regObj
        )
        {
          return Handle::OnStateChange(pISignal, eventName, regObj);
        }

        virtual ds::ErrorType GetSignalBus
        (
          ds::Net::EventType  eventID,
          ISignalBus **       ppISigBus
        );

        /* Process() function derived from INode->Handle->Here */
        virtual boolean Process (void* userDataPtr);

        /*-------------------------------------------------------------------------
          Methods to overload new/delete operators.
        -------------------------------------------------------------------------*/
        DSNET_OVERLOAD_OPERATORS (PS_MEM_DS_NET_MCAST_SESSION_PRIV)

        /*-------------------------------------------------------------------------
          Macro to define IWeakRef/IQI methods.
        -------------------------------------------------------------------------*/
        DS_UTILS_IWEAKREF_IMPL_DEFAULTS_NO_QI()
        DSIQI_QUERY_INTERFACE(IMCastSessionPriv)

        /*-------------------------------------------------------------------------
          Internal methods.
        -------------------------------------------------------------------------*/
        inline int32 GetMCastHandle
        (
          void
        )
        {
          return mMCastHandle;
        }

        inline MCastJoinFlagsType GetRegFlag
        (
          void
        )
        {
          return mJoinFlag;
        }

    }; /* class MCastSessionPriv */

  }  /* Namespace Net */
}  /* Namespace ds */

/*lint -restore */
#endif // FEATUTE_DATA_PS_MCAST
#endif /* DS_NET_MCAST_SESSION_PRIV_H */

