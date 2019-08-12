#ifndef DS_NET_IBEARERINFO_H
#define DS_NET_IBEARERINFO_H

/*============================================================================
  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
#include "ds_Net_Def.h"
namespace ds
{
   namespace Net
   {
      namespace BearerTechCDMA
      {
         
         /**
           * CDMA sub-technologies. 
           * Since a mobile device may simultaneously support more than one
           * technology, the following may be ORed together to indicate all the
           * technologies supported by the bearer.
           */
         const unsigned int SUBTECH_1X         = 0x1;
         const unsigned int SUBTECH_EVDO_REV0  = 0x2;
         const unsigned int SUBTECH_EVDO_REVA  = 0x4;
         const unsigned int SUBTECH_EVDO_REVB  = 0x8;
         const unsigned int SUBTECH_EVDO_EHRPD = 0x10;
         const unsigned int SUBTECH_FMC        = 0x20;
         
         /**
           * SUBTECH_NULL shall be indicated if the application queries for the
           * the bearer technology but there is currently no active Network 
           * connection.
           */
         const unsigned int SUBTECH_NULL = 0x8000;
         
         /**
           * CDMA 1x specific service options.
           * Since a mobile device may simultaneously support more than one
           * service options, the following may be ORed together to indicate all
           * the service options supported by the bearer.
           */
         const unsigned int SO_1X_IS95 = 0x1;
         const unsigned int SO_1X_IS2000 = 0x2;
         const unsigned int SO_1X_IS2000_REL_A = 0x4;
         
         /**
           * CDMA EVDO (HDR) specific service options. Packet Applications are
           * not dependent on Rev0/RevA/RevB, therefore the constants are reused
           * for all revisions.
           * Since a mobile device may simultaneously support more than one
           * service options, the following may be ORed together to indicate all
           * the service options supported by the bearer.
           */
         const unsigned int SO_EVDO_DPA = 0x1;
         const unsigned int SO_EVDO_MFPA = 0x2;
         const unsigned int SO_EVDO_EMPA = 0x4;
         const unsigned int SO_EVDO_EMPA_EHRPD = 0x8;
         const unsigned int SO_EVDO_MMPA = 0x10;
         const unsigned int SO_EVDO_MMPA_EHRPD = 0x20;
      };
      namespace BearerTechUMTS
      {
         
         /**
           * UMTS sub-technologies. 
           * Since a mobile device may simultaneously support more than one
           * technology, the following may be ORed together to indicate all the
           * technologies supported by the bearer.
           * For example, in WCDMA the WCDMA subtechnology is set; in addition,
           * HSUPA and HSDPA flags may be set when the respective subtechnologies
           * are available.
           */
         const unsigned int SUBTECH_WCDMA = 0x1;
         const unsigned int SUBTECH_GPRS = 0x2;
         const unsigned int SUBTECH_HSDPA = 0x4;
         const unsigned int SUBTECH_HSUPA = 0x8;
         const unsigned int SUBTECH_EDGE = 0x10;
         const unsigned int SUBTECH_LTE = 0x20;
         
         /**
           * HSDPA+ bearer
           */
         const unsigned int SUBTECH_HSDPAPLUS = 0x40;
         
         /**
           * Dual Carrier HSDPA+ bearer
           */
         const unsigned int SUBTECH_DC_HSDPAPLUS = 0x80;
         
         /**
           * SUBTECH_NULL shall be indicated if the application queries for the
           * the bearer technology but there is currently no active Network 
           * connection.
           */
         const unsigned int SUBTECH_NULL = 0x8000;
      };
      
      /** Data bearer rate definitions */
      struct BearerTechRateType {
         int maxTxDataRate; /**< Max Tx bearer data rate */
         int maxRxDataRate; /**< Max Rx bearer data rate */
         int avgTxDataRate; /**< Average Tx bearer data rate */
         int avgRxDataRate; /**< Average Rx bearer data rate */
         int currentTxDataRate; /**< Current Tx bearer data rate */
         int currentRxDataRate; /**< Current Rx bearer data rate */
      };
      const ::AEEIID AEEIID_IBearerInfo = 0x106c6a5;
      struct IBearerInfo : public ::IQI
      {
         
         /**
           * This attribute indicates the network type of the data connection (bearer)
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetNetwork(::ds::Net::IfaceNameType* value) = 0;
         
         /**
           * This attribute indicates the CDMA sub technologies of the bearer.
           * It is relevant if the bearer Network type is CDMA.
           * @param value Attribute value
           * @see CDMA sub-technologies.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetCDMATypeMask(unsigned int* value) = 0;
         
         /**
           * This attribute indicates the CDMA service options of the bearer.
           * It is relevant if the bearer Network type is CDMA.
           * @param value Attribute value
           * @see CDMA service options.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetCDMAServiceOptionsMask(unsigned int* value) = 0;
         
         /**
           * This attribute indicates the UMTS sub technologies of the bearer.
           * It is relevant if the bearer Network type is UMTS.
           * @param value Attribute value
           * @see UMTS sub-technologies.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetUMTSTypeMask(unsigned int* value) = 0;
         
         /**
           * This attribute represents the bearer technology rate for this network.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetRate(::ds::Net::BearerTechRateType* value) = 0;
         
         /**
           * Indicates if the bearer (subtechnology) is the null bearer 
           * value for the technology described by this instance.
           * Bearer shall be NULL if there is currently no active Network
           * connection.
           * @param value Attribute value
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetBearerIsNull(boolean* value) = 0;
      };
   };
};
#endif //DS_NET_IBEARERINFO_H
