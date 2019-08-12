#ifndef DS_NET_MCAST_DEF_H
#define DS_NET_MCAST_DEF_H

#ifdef FEATUTE_DATA_PS_MCAST
/*============================================================================
  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Addr_Def.h"
namespace ds
{
   namespace Net
   {
      
      /** 
        * MCastRegStateType: Information on registration state of the MCast
        * Session.
        * Registration to the MCast service is triggered by the applicable API of
        * IMcastManager.
        * Once registration is triggered, the application should register for 
        * QDS_EV_REGISTRATION_STATE event on each applicable MCast Session object
        * in order to be notified on changes to the registration state.
        */
      typedef int MCastRegStateType;
      namespace MCastRegState
      {
         
         /**
           * SUCCESS: Registration to the MCast service was successful. The
           * application must have a socket bound to the same MCast IP and port
           * specified in the Join operation in order to receive the MCast data.
           * When MCast data arrives on the socket the application will be notified
           * via READ event on the socket.
           */
         const ::ds::Net::MCastRegStateType MCAST_REGISTER_SUCCESS = 1;
         
         /**
           * FAILURE: Registration to the MCast service has failed. This may
           * indicate a temporary failure based on the info code that may be
           * obtained via IMCastSession::TechStatusInfoCode. If the failure
           * is permanent, the application should call IMCastSession::Leave.
           */
         const ::ds::Net::MCastRegStateType MCAST_REGISTER_FAILURE = 2;
         
         /**
           * DEREGISTERED: the application has been de-registered from the MCast
           * service. The reason is indicated by the info code that may be
           * obtained via IMCastSession::TechStatusInfoCode. There is no need to
           * call IMCastSession::Leave as the application has been automatically
           * de-registered. The MCastSession object is not usable anymore and
           * should be released.
           */
         const ::ds::Net::MCastRegStateType MCAST_DEREGISTERED = 3;
      };
      typedef int MCastInfoCodeType;
      namespace MCastInfoCode
      {
         const ::ds::Net::MCastInfoCodeType IC_NOT_SPECIFIED = 0;
         const ::ds::Net::MCastInfoCodeType BCMCS_FLOW_STATUS_CANCELLED = 600;
         const ::ds::Net::MCastInfoCodeType BCMCS_FLOW_STATUS_UNABLE_TO_MONITOR = 601;
         const ::ds::Net::MCastInfoCodeType BCMCS_FLOW_STATUS_REQUESTED = 611;
         const ::ds::Net::MCastInfoCodeType BCMCS_FLOW_STATUS_TIMEOUT = 602;
         const ::ds::Net::MCastInfoCodeType BCMCS_FLOW_STATUS_LOST = 603;
         const ::ds::Net::MCastInfoCodeType BCMCS_FLOW_STATUS_SYS_UNAVAILABLE = 604;
         const ::ds::Net::MCastInfoCodeType BCMCS_FLOW_STATUS_AN_REJECT_NOT_AVAILABLE = 606;
         const ::ds::Net::MCastInfoCodeType BCMCS_FLOW_STATUS_AN_REJECT_NOT_TRANSMITTED = 607;
         const ::ds::Net::MCastInfoCodeType BCMCS_FLOW_STATUS_AN_REJECT_INVALID_AUTH_SIG = 608;
         const ::ds::Net::MCastInfoCodeType BCMCS_FLOW_STATUS_UNAVAILABLE = 605;
         const ::ds::Net::MCastInfoCodeType BCMCS_FLOW_STATUS_NO_MAPPING = 609;
         const ::ds::Net::MCastInfoCodeType BCMCS_FLOW_STATUS_ID_NOT_FOUND_FOR_GIVEN_MULTICAST_IP = 610;
         const ::ds::Net::MCastInfoCodeType BCMCS_MAX_FLOWS_REACHED = 612;
         const ::ds::Net::MCastInfoCodeType BCMCS_JOIN_REQ_IN_PROGRESS = 613;
         const ::ds::Net::MCastInfoCodeType BCMCS_FLOW_REQUEST_SENT = 614;
         const ::ds::Net::MCastInfoCodeType BCMCS_FLOW_STATUS_MAX_MONITORED_FLOWS = 615;
         const ::ds::Net::MCastInfoCodeType BCMCS_FLOW_STATUS_MONITORED = 616;
         const ::ds::Net::MCastInfoCodeType BCMCS_REGISTRATION_SUCCESS = 617;
         const ::ds::Net::MCastInfoCodeType BCMCS_REGISTRATION_NOT_ALLOWED = 618;
         const ::ds::Net::MCastInfoCodeType BCMCS_REGISTRATION_FAILED = 619;
         const ::ds::Net::MCastInfoCodeType BCMCS_FLOW_DEREGISTERED = 620;
         const ::ds::Net::MCastInfoCodeType FLO_IP_OR_PORT_NOT_SUPPORTED = 1051;
         const ::ds::Net::MCastInfoCodeType FLO_NO_AUTHORIZATION = 1052;
         const ::ds::Net::MCastInfoCodeType FLO_NO_SYSTEM_COVERAGE = 1053;
         const ::ds::Net::MCastInfoCodeType FLO_MAX_FLOW_REACHED = 1054;
         const ::ds::Net::MCastInfoCodeType DVBH_IP_OR_PORT_NOT_FOUND = 1101;
         const ::ds::Net::MCastInfoCodeType DVBH_SYSTEM_UNAVAILABLE = 1102;
         const ::ds::Net::MCastInfoCodeType DVBH_BAD_REQUEST = 1103;
         const ::ds::Net::MCastInfoCodeType DVBH_REQUEST_CONFLICT = 1104;
         const ::ds::Net::MCastInfoCodeType DVBH_DUP_REQUEST = 1105;
         const ::ds::Net::MCastInfoCodeType DVBH_MAX_FLOWS_REACHED = 1106;
         const ::ds::Net::MCastInfoCodeType MCAST_INFO_CODE_RESERVED1 = 1151;
      };
      struct MCastStateChangedType {
         MCastRegStateType regState;
         MCastInfoCodeType infoCode;
      };
      
      /**
        * MCastJoinFlagsType: flags specifying whether MCast Join operation 
        * should also perform registration or not.
        */
      typedef int MCastJoinFlagsType;
      namespace MCastJoinFlags
      {
         
         /**
           * REG_SETUP_ALLOWED: The system shall trigger registration process
           * as part of JoinBundle API processing
           */
         const ::ds::Net::MCastJoinFlagsType REG_SETUP_ALLOWED = 0;
         
         /**
           * REG_SETUP_NOT_ALLOWED: Do not perform registration as part of the 
           * Join request. The application shall take care to call the
           * RegisterBundle API when it is ready to triger the registration
           * process for a session..
           */
         const ::ds::Net::MCastJoinFlagsType REG_SETUP_NOT_ALLOWED = 1;
      };
   };
};
#endif // FEATUTE_DATA_PS_MCAST
#endif //DS_NET_MCAST_DEF_H
