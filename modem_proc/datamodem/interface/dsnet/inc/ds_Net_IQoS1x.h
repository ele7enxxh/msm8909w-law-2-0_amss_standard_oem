#ifndef DS_NET_IQOS1X_H
#define DS_NET_IQOS1X_H

/*============================================================================
  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
namespace ds
{
   namespace Net
   {
      const ::AEEIID AEEIID_IQoS1x = 0x106d74b;
      
      /** @interface IQoS1x
        * 
        * ds Net 1x QoS interface.
        * This interface provides 1x-specific operations on a QoS link.
        * This Object is received from IQoSSecondary/IQoS via
        * GetTechObject.
        */
      struct IQoS1x : public ::IQI
      {
         
         /**
           * This struct defines the type for RMAC3 information.
           */
         struct RMAC3InfoType {
            int headroomPayloadSize; /**< PA headroom limited payload size in bytes. */
            int bucketLevelPayloadSize; /**< BucketLevel equivalent payload size in bytes. */
            int t2pInflowPayloadSize; /**< T2PInflow equivalent payload size in bytes. */
         };
         
         /**
           * This attribute represents the MAC layer information for the QoS flow.          
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetRMAC3Info(RMAC3InfoType* value) = 0;
         
         /**
           * This attribute represents the status of the data transmission for the QoS flow.
           * If there is an error in transmission of data because of the two reasons listed below,
           * then the attribute will be FALSE, otherwise it will be TRUE.
           * Reasons for transmission error:
           *  - Physical Layer Transmission error (M-ARQ)
           *  - Data has become stale (by remaining in transmit watermark for too long) and thus
           *    discarded before a transmission is even attempted.          
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetTXStatus(boolean* value) = 0;
         
         /**
           * This attribute represents the value of the inactivity timer of the associated QoS instance.
           * If data is not transmitted on this QoS instance for the specified inactivity timer value,
           * the traffic channel is released in order to conserve resources.
           * Although the attribute's name implies that there is a timer per every QoS instance, there
           * is only a timer per traffic channel. Since more than one QoS instance can be multiplexed
           * on to the same traffic channel, the largest value of all inactivity timer values is used
           * to start the inactivity timer.          
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetInactivityTimer(int* value) = 0;
         
         /**
           * This attribute represents the value of the inactivity timer of the associated QoS instance.
           * If data is not transmitted on this QoS instance for the specified inactivity timer value,
           * the traffic channel is released in order to conserve resources.
           * Although the attribute's name implies that there is a timer per every QoS instance, there
           * is only a timer per traffic channel. Since more than one QoS instance can be multiplexed
           * on to the same traffic channel, the largest value of all inactivity timer values is used
           * to start the inactivity timer.          
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetInactivityTimer(int value) = 0;
      };
   };
};
#endif //DS_NET_IQOS1X_H
