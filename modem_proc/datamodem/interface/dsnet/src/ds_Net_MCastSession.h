#ifndef DS_NET_MCAST_SESSION_H
#define DS_NET_MCAST_SESSION_H
#ifdef FEATUTE_DATA_PS_MCAST
/*===========================================================================
  @file ds_Net_MCastSession.h

  This file defines the class that implements ds_Net_IMCastSession interface.

  The MCastSession class (ds::Net::MCastSession) implements the following 
  interfaces: IQI and IMCastSession.

  Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_MCastSession.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-03-10 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Net_IMCastSession.h"
#include "ds_Net_Handle.h"
#include "ds_Net_Platform.h"
#include "ds_Net_MemManager.h"
#include "ds_Net_MCastManager.h"

/*Suppress lint error because of namespace collisions */
/*lint -save -e578 */
/*lint -esym(1510, IMCastSession) */
/*lint -esym(1510, IEventManager) */
/*lint -esym(1510, IQI) */
namespace ds
{
  namespace Net
  {

    typedef int32 MCastSessionCreationType;
    namespace MCastSessionCreation
    {
      const MCastSessionCreationType MCAST_SESSION_CREATED_BY_JOIN = 0;
      const MCastSessionCreationType MCAST_SESSION_CREATED_BY_JOIN_BUNDLE = 1;
    };

    class MCastSession: public IMCastSession, 
                        public Handle
    {
      public:
        MCastSession
        (
          int32 ifaceHandle,
          int32 mcastHandle,
          MCastSessionCreationType creationType,
          MCastJoinFlagsType regFlag
        );

        /*---------------------------------------------------------------------
          Inherited functions from IMCastSession
        ---------------------------------------------------------------------*/
        /**
          @brief
          This function issues a request for leaving a multicast session.
          
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
        information code

        @retval AEE_SUCCESS Request received successfully.
        @retval Other ds designated error codes might be returned.
        @see ds_Errors_Def.idl.
        */
        virtual ds::ErrorType CDECL GetRegStateAndInfoCode
        (
          MCastStateChangedType* RegStateAndInfoCode
        );

        /**
        @brief
        This function provides the information code for the status of the
        MCast technology session

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
        DSNET_OVERLOAD_OPERATORS (PS_MEM_DS_NET_MCAST_SESSION)

        /*-------------------------------------------------------------------------
          Macro to define IWeakRef/IQI methods.
        -------------------------------------------------------------------------*/
        DS_UTILS_IWEAKREF_IMPL_DEFAULTS_NO_QI()
        DSIQI_QUERY_INTERFACE(IMCastSession)

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


    private:
      int32                    mMCastHandle;
      ISignalBus *             mpSigBusRegStatus;
      ISignalBus *             mpSigBusTechStatus;
      MCastStateChangedType    mRegState;
      MCastInfoCodeType        mStatusInfoCode;
      MCastSessionCreationType mSessionCreationType;
      MCastJoinFlagsType       mJoinFlag;

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
      virtual ~MCastSession
      (
        void 
      ) 
      throw();

    }; /* class MCastSession */

  }  /* Namespace Net */
}  /* Namespace ds */

/*lint -restore */
#endif // FEATUTE_DATA_PS_MCAST
#endif

