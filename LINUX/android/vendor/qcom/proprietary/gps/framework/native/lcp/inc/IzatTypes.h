/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Izat Types

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_IZATTYPES_H__
#define __IZAT_MANAGER_IZATTYPES_H__

namespace izat_manager {

/**
 * Bit Mask for location listeners.
 */
typedef unsigned short int IzatLocationListenerMask;

/**
 * Enum of supported location streams.
 * Each elements of enum represet a bit mask value.
 * Do not add values with multiple bits set.
 */
typedef enum IzatLocationStreamType {

    /**
     * Fused location stream.
     */
    IZAT_STREAM_FUSED = 0x1,

    /**
     * Network location stream.
     */
    IZAT_STREAM_NETWORK = 0x2,

    /**
     * GNSS location stream.
     */
    IZAT_STREAM_GNSS  = 0x4

} IzatLocationStreamType;

typedef enum IzatLocationStatus {
    /**
     * Final location
     */
    IZAT_LOCATION_STATUS_FINAL = 0x0,

    /**
      * Intermediate location
      */
    IZAT_LOCATION_STATUS_INTERMEDIATE = 0x1
} IzatLocationStatus;

/**
 * Izat Provider status Enumeration
 */
typedef enum IzatProviderStatus {

    /**
     * Out of service.
     */
    IZAT_PROVIDER_OUT_OF_SERVICE = 0,

    /**
     * Temporarily unavailable.
     */
    IZAT_PROVIDER_TEMPORARILY_UNAVAILABLE,

    /**
     * Available.
     */
    IZAT_PROVIDER_AVAILABLE,

    /**
     * GNSS status none
     */
    IZAT_PROVIDER_GNSS_STATUS_NONE,

    /**
     * GNSS Session BEGIN
     */
    IZAT_PROVIDER_GNSS_STATUS_SESSION_BEGIN,

    /**
     * GNSS Session END
     */
    IZAT_PROVIDER_GNSS_STATUS_SESSION_END,

    /**
     * GNSS Engine ON
     */
    IZAT_PROVIDER_GNSS_STATUS_ENGINE_ON,

    /**
     * GNSS Engine OFF
     */
    IZAT_PROVIDER_GNSS_STATUS_ENGINE_OFF,



} IzatProviderStatus;

/**
 * Enumeration of Horizontal accuracy to be used for specifying accuracy of Latitude and Longitudes
 */
typedef enum IzatHorizontalAccuracy {

    /**
     * High power consumption.
     */
    IZAT_HORIZONTAL_FINE = 0,

    /**
     * Medium power consumption.
     */
    IZAT_HORIZONTAL_BLOCK,

    /**
     * Low power consumption.
     */
    IZAT_HORIZONTAL_CITY,

    /**
     * Zero power consumption.
     */
    IZAT_HORIZONTAL_NONE

} IzatHorizontalAccuracy;

/**
 * Enumeration of accuracy to be used for specifying accuracy of Bearing and or Altitude.
 */
typedef enum IzatOtherAccuracy {

    /**
     * High accuracy.
     */
    IZAT_OTHER_HIGH = 0,

    /**
     * Low accuracy.
     */
    IZAT_OTHER_LOW,

    /**
     * No Accuracy requirement.
     */
    IZAT_OTHER_NONE

} IzatOtherAccuracy;

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IZATTYPES_H__
