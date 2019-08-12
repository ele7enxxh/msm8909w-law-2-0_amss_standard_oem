/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 Receive Telephony / Data connection Info from upper frameworks

 Copyright (c) 2012-2014 Qualcomm Atheros, Inc.
 All Rights Reserved.
 Qualcomm Atheros Confidential and Proprietary.
 =============================================================================*/

#ifndef RILINFO_H
#define RILINFO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define LOC_RIL_VERSION 1
#define LOC_RIL_SERVER "Loc-Ril-Server"

typedef enum
{
  LOC_NW_ROAMING = 1,
  LOC_NW_HOME = 2,
  LOC_NW_OOO = 3,
  LOC_NW_MAX = 0x10000000
} LOC_NWstatus;

typedef enum
{
  LOC_RIL_TECH_CDMA = 0x1,
  LOC_RIL_TECH_GSM = 0x2,
  LOC_RIL_TECH_WCDMA = 0x4,
  LOC_RIL_TECH_LTE = 0x8,
  LOC_RIL_TECH_TD_SCDMA = 0x16,
  LOC_RIL_TECH_MAX = 0x10000000
} LOC_RilTechType;

typedef struct
{
  // Integer (0..999)
  uint16_t mcc;

  // Integer (0..999)
  uint16_t mnc;

  // Integer (0..65535)
  uint16_t lac;

  //Integer (0..268435455)
  uint32_t cid;
} LOC_RilTechWcdmaCinfo;

typedef struct
{
  // Integer (0..999)
  uint16_t mcc;

  // Integer (0..999)
  uint16_t mnc;

  // Integer (0..65535)
  uint16_t lac;

  // Integer (0..65535)
  uint32_t cid;
} LOC_RilTechGsmCinfo;

typedef struct
{
  // Integer (0..999)
  // Set to 0 if mcc is not available
  uint16_t mcc;

  // Integer (0..32767)
  uint16_t sid;

  // Integer (0..65535)
  uint16_t nid;

  // Integer (0..65535)
  uint32_t bsid;

  // Latitude is a decimal number as specified in 3GPP2 C.S0005-A v6.0.
  // It is represented in units of 0.25 seconds and ranges from -1296000
  // to 1296000, both values inclusive (corresponding to a range of -90
  // to +90 degrees). Integer.MAX_VALUE is considered invalid value.
  int32_t bslat;

  // Longitude is a decimal number as specified in 3GPP2 C.S0005-A v6.0.
  // It is represented in units of 0.25 seconds and ranges from -2592000
  // to 2592000, both values inclusive (corresponding to a range of -180
  // to +180 degrees). Integer.MAX_VALUE is considered invalid value.
  int32_t bslon;

  // Integer (-24..24). Short.MAX_VALUE is considered as
  // timezone information not present.
  int16_t local_timezone_offset_from_utc;

  //Boolean
  bool local_timezone_on_daylight_savings;
} LOC_RilTechCdmaCinfo;

typedef struct
{
  // Integer (0..999)
  uint16_t mcc;

  // Integer (0..999)
  uint16_t mnc;

  // Integer (0..65535)
  // 0 = tac unavailable
  uint16_t tac;

  // Integer (0..503)
  // SHRT_MAX = pci unavailable
  // Optional value
  uint16_t pci;

  // Integer (0..268435455)
  uint32_t cid;
} LOC_RilTechLteCinfo;

// The following defines are for valid_mask in RilCellInfo
#define LOC_RIL_CELLINFO_HAS_NW_STATUS 0x00000001
#define LOC_RIL_CELLINFO_HAS_TECH_TYPE 0x00000002
#define LOC_RIL_CELLINFO_HAS_CELL_INFO 0x00000004

// The following defines are for valid mask for Loc_RilTechGsmCinfo
// and Loc_RilTechWcdmaCinfo fields.
#define LOC_RIL_TECH_GW_HAS_MCC 0x00000008
#define LOC_RIL_TECH_GW_HAS_MNC 0x00000010
#define LOC_RIL_TECH_GW_HAS_LAC 0x00000020
#define LOC_RIL_TECH_GW_HAS_CID 0x00000040

// The following defines are for valid_mask in LOC_RilTechCdmaCinfo
#define LOC_RIL_TECH_CDMA_HAS_MCC              0x00000008
#define LOC_RIL_TECH_CDMA_HAS_SID              0x00000010
#define LOC_RIL_TECH_CDMA_HAS_NID              0x00000020
#define LOC_RIL_TECH_CDMA_HAS_BSID             0x00000040
#define LOC_RIL_TECH_CDMA_HAS_BSLAT            0x00000080
#define LOC_RIL_TECH_CDMA_HAS_BSLONG           0x00000100
#define LOC_RIL_TECH_CDMA_HAS_TIMEZONE         0x00000200
#define LOC_RIL_TECH_CDMA_HAS_DAYLIGHT_SAVING  0x00000400

// The following defines are for valid_mask in LOC_RilTechLteCinfo
#define LOC_RIL_TECH_LTE_HAS_MCC  0x00000008
#define LOC_RIL_TECH_LTE_HAS_MNC  0x00000010
#define LOC_RIL_TECH_LTE_HAS_TAC  0x00000020
#define LOC_RIL_TECH_LTE_HAS_PCI  0x00000040
#define LOC_RIL_TECH_LTE_HAS_CID  0x00000080

typedef struct LOC_RilCellInfo
{
  uint32_t valid_mask;
  LOC_NWstatus nwStatus;
  LOC_RilTechType rtType;
  union
  {
    LOC_RilTechCdmaCinfo cdmaCinfo;
    LOC_RilTechGsmCinfo gsmCinfo;
    LOC_RilTechWcdmaCinfo wcdmaCinfo;
    LOC_RilTechLteCinfo lteCinfo;
  } u;
} LOC_RilCellInfo;

#define LOC_RILAIRIF_CDMA  0x01
#define LOC_RILAIRIF_GSM   0x02
#define LOC_RILAIRIF_WCDMA 0x04
#define LOC_RILAIRIF_LTE   0x08
#define LOC_RILAIRIF_EVDO  0x10
#define LOC_RILAIRIF_WIFI  0x20
typedef uint32_t LOC_RilAirIf_mask;
typedef uint32_t LOC_RilAirIf_type;

#define LOC_RIL_SERVICE_INFO_HAS_AIR_IF_TYPE          0x00000001
#define LOC_RIL_SERVICE_INFO_HAS_CARRIER_AIR_IF_TYPE  0x00000002
#define LOC_RIL_SERVICE_INFO_HAS_CARRIER_MCC          0x00000004
#define LOC_RIL_SERVICE_INFO_HAS_CARRIER_MNC          0x00000008
#define LOC_RIL_SERVICE_INFO_HAS_CARRIER_NAME         0x00000010
#define LOC_RIL_CARRIER_NAME_MAX_LEN                  32

typedef struct LOC_RilServiceInfo
{
  uint32_t valid_mask;        // Set to 0 in case of failure
  LOC_RilAirIf_mask airIf_type;        // Air interface types supported by mobile
  LOC_RilAirIf_type carrierAirIf_type; // home carrier air interface type is either:
                                       // LOC_RILAIRIF_GSM, LOC_RILAIRIF_WCDMA or
                                       // LOC_RILAIRIF_CDMA
  uint16_t carrierMcc;        // home carrier MCC
  uint16_t carrierMnc;        // home carrier MNC
  uint16_t carrierNameLen;    // home carrier name length
  char carrierName[LOC_RIL_CARRIER_NAME_MAX_LEN]; // home carrier name
} LOC_RilServiceInfo;

typedef uint32_t LOC_Ril_version;

typedef struct LOC_RilVoiceCallInfo
{
  bool isStateValid;
  bool isInVoiceCall;
  time_t voiceCallStartTime;
  time_t cellUpdateTimeInVoiceCall;
} LOC_RilVoiceCallInfo;

//
// Response message payload
// +--------------------------------------------------------------------+
// | Payload Length in bytes |  Payload      |
// +--------------------------------------------------------------------+
// <<------- 32 bits -------> <-- variable -->
//
typedef struct
{
  uint32_t msgLength;
  // It is caller's responsibility to allocate enough memory to hold the msg paylaod
  uint8_t msgPayload[1];
} LOC_RilRespMsgBuf;

typedef enum
{
  // Request version number for this service
  LOC_RIL_REQUEST_VERSION = 1,

  // Register for cell update, update will be sent when
  // there is update in RilCellInfo changes.
  // Response will be LOC_RIL_RESPONSE_CELL_UPDATE.
  LOC_RIL_REGISTER_CELL_UPDATE = 2,

  // De-register for cell update
  // No response is sent back.
  LOC_RIL_DEREISTER_CELL_UPDATE = 3,

  // Request for one-time RilServiceInfo
  // Response will be LOC_RIL_RESPONSE_SERVICE_INFO.
  LOC_RIL_REQUEST_SERVICE_INFO = 4,

  // Request for voice call update
  LOC_RIL_REGISTER_VOICE_CALL_UPDATE = 5,

  // Deregister for voice call update
  // No reponse is sent back
  LOC_RIL_DEREGISTER_VOICE_CALL_UPDATE = 6,

  // Force it to 32-bit
  LOC_RIL_REQUEST_TYPE_MAX = 0x10000000
} LOC_RilRequestType;

typedef enum
{
  // Response version number for this service.
  // Payload length will be sizeof (uint32_t)
  // Payload will be 32-bit version number
  LOC_RIL_RESPONSE_VERSION = 1,

  // Update when there is a change in RilCellInfo changes.
  // Payload length will be sizeof (LOC_RilCellInfo)
  // Payload will be LOC_RilCellInfo.
  LOC_RIL_RESPONSE_CELL_UPDATE = 2,

  // Update when phone is OOO.
  // Payload length will be 0 and there will be no payload.
  LOC_RIL_RESPONSE_PHONE_OOO = 3,

  // Reponse for RilServiceInfo request
  // Payload length will be sizeof(LOC_RilServiceInfo) and
  // payload will be LOC_RilServiceInfo
  LOC_RIL_RESPONSE_SERVICE_INFO = 4,

  // Force it to 32-bit
  LOC_RIL_RESPONSE_TYPE_MAX = 0x10000000
} LOC_RilResponseType;

#ifdef __cplusplus
}
#endif

#endif // RILINFO_H
