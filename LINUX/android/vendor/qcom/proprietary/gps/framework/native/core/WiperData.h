 /*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#ifndef WIPERDATA_H
#define WIPERDATA_H

#include "comdef.h"

namespace izat_manager
{

#ifdef __cplusplus
extern "C"
{
#endif


#define MAX_REPORTED_APS 50
#define MAC_ADDRESS_LENGTH 6
#define SSID_LENGTH 32

#define WIFI_APDATA_MASK_AP_RSSI 0x10
#define WIFI_APDATA_MASK_AP_CHANNEL 0x20

typedef enum ReliabilityValue
{
    RELIABILITY_NOT_SET = 0,
    RELIABILITY_VERY_LOW = 1,
    RELIABILITY_LOW = 2,
    RELIABILITY_MEDIUM = 3,
    RELIABILITY_HIGH = 4
} ReliabilityValue;

typedef enum PositionSourceType
{
    GNSS = 0,
    CELLID = 1,
    ENH_CELLID = 2,
    WIFI = 3,
    TERRESTRIAL = 4,
    GNSS_TERRESTRIAL_HYBRID = 5,
    OTHER = 6
} PositionSourceType;


typedef struct CoarsePositionInfo {

    uint8 latitudeValid;  /* Must be set to true if latitude is being passed */
    double latitude;

    uint8 longitudeValid;  /* Must be set to true if longitude is being passed */
    double longitude;

    uint8 horUncCircularValid;  /* Must be set to true if horUncCircular is being passed */
    float horUncCircular;

    uint8 horConfidenceValid;  /* Must be set to true if horConfidence is being passed */
    uint8 horConfidence;

    uint8 horReliabilityValid;  /* Must be set to true if horReliability is being passed */
    ReliabilityValue horReliability;

    uint8 altitudeWrtEllipsoidValid;  /* Must be set to true if altitudeWrtEllipsoid is being passed */
    float altitudeWrtEllipsoid;

    uint8 altitudeWrtMeanSeaLevelValid;  /* Must be set to true if altitudeWrtMeanSeaLevel is being passed */
    float altitudeWrtMeanSeaLevel;

    uint8 vertUncValid;  /* Must be set to true if vertUnc is being passed */
    float vertUnc;

    uint8 vertConfidenceValid;  /* Must be set to true if vertConfidence is being passed */
    uint8 vertConfidence;

    uint8 vertReliabilityValid;  /* Must be set to true if vertReliability is being passed */
    ReliabilityValue vertReliability;

    uint8 timestampUtcValid;  /* Must be set to true if timestampUtc is being passed */
    uint64 timestampUtc;

    uint8 timestampAgeValid;  /* Must be set to true if timestampAge is being passed */
    int32 timestampAge;

    uint8 positionSrcValid;  /* Must be set to true if positionSrc is being passed */
    PositionSourceType positionSrc;

} CoarsePositionInfo;

typedef struct WifiApSsidInfo {

  /*  Wifi SSID string */
  char ssid[SSID_LENGTH+1];
} WifiApSsidInfo;

typedef struct WifiApInfo {

    int apLen;
    /* Represents whether access point len*/
    uint8 mac_address[MAX_REPORTED_APS * MAC_ADDRESS_LENGTH];
    /*  Represents mac address of the wifi access point*/
    int16 rssi[MAX_REPORTED_APS];
    /*  Represents received signal strength indicator of the wifi access point*/
    uint16 channel[MAX_REPORTED_APS];
    /*  Represents  wiFi channel on which a beacon was received of the wifi access point */

} WifiApInfo;

typedef struct WifiLocation {

    unsigned char positionValid;
    /* Represents info on whether position is valid */
    double latitude;
    /* Represents latitude in degrees */
    double longitude;
    /* Represents longitude in degrees */
    float accuracy;
    /* Represents expected accuracy in meters */
    uint8 horConfidence;
    /* Represents expected hor confidence in percentage */
    int fixError;
    /* Represents Wifi position error code */
    unsigned char numApsUsed;
    /* Represents  number of Access Points (AP) used to generate a fix */
    unsigned char apInfoValid;
    /* Represents whether access point info is valid*/
    WifiApInfo apInfo;
    /* Represents access point information */
    uint8  wifiApSsidValid;
    /* Represents info on whether ap SSID is valid */
    uint32 wifiApSsidInfoLen;
    /* Represents info on ap SSID length */
    WifiApSsidInfo wifiApSsidInfo[MAX_REPORTED_APS];
    /* Represent Wifi SSID string */
}WifiLocation;

typedef struct WifiSupplicantInfo {

    int attachState;
    /* Represents whether access point attach state*/
    unsigned char apMacAddressValid;
    /* Represents info on whether ap mac address is valid */
    uint8 apMacAddress[MAC_ADDRESS_LENGTH];
    /* Represents mac address of the wifi access point*/
    uint8 wifiApSsidValid;
    /* Represents info on whether ap SSID is valid */
    char ssid[SSID_LENGTH+1];
    /* Represents Wifi SSID string*/
} WifiSupplicantInfo;

/* Wifi request types from modem*/
enum WifiRequestType
{
    HIGH = 0,
    LOW = 1,
    STOP = 2,
    UNKNOWN
};

/*Wifi attachment status */
enum WifiAttachmentStatus
{
    WIFI_ACCESS_POINT_ATTACHED = 0,
    WIFI_ACCESS_POINT_DETACHED = 1,
    WIFI_ACCESS_POINT_HANDOVER = 2
};

#ifdef __cplusplus
}
#endif
}

#endif /* WIPERDATA_H */
