/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IzatLocation

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_IZATLOCATION_H__
#define __IZAT_MANAGER_IZATLOCATION_H__

#include <comdef.h>
#include <stdio.h>
#include <string>

namespace izat_manager {

using namespace std;

/**
 * @brief IzatLocation class
 * @details IzatLocation class containing location related information
 *
 */
class IzatLocation {
public:

    /**
     * @brief Default contructor
     * @details Default constructor
     */
    IzatLocation () {
        reset ();
    }

    /**
     * @brief Constructor
     * @details Constructor
     *
     * @param rhs Reference to IzatLocation
     */
    IzatLocation (const IzatLocation & rhs) {
        makeCopy (rhs);
    }

    /**
     * @brief assignment operator
     * @details assignment operator
     *
     * @param rhs Reference to IzatLocation
     * @returs reference to IzatLocation
     */
    IzatLocation & operator= (const IzatLocation & rhs) {
        makeCopy (rhs);
        return *this;
    }

    /**
     * @brief Destructor
     * @details Destructor
     */
    virtual ~IzatLocation () {
        reset ();
    };

    /**
     * @brief Reset
     * @details Reset
     */
    void reset () {

        mHasUtcTimestampInMsec = false;
        mHasElapsedRealTimeInNanoSecs = false;
        mHasLatitude = false;
        mHasLongitude = false;
        mHasHorizontalAccuracy = false;
        mHasAltitudeWrtEllipsoid = false;
        mHasAltitudeWrtMeanSeaLevel = false;
        mHasVerticalUncertainity = false;
        mHasBearing = false;
        mHasSpeed = false;
    }

    /**
     * @brief Check if location is valid
     * @details Check if location is valid
     * @return true if valid or false if not valid
     */
    bool isValid () const {
        return  (mHasLatitude && mHasLongitude && mHasHorizontalAccuracy);
    }

    /**
     * @brief Convert contents to string
     * @details Convert contents to string
     *
     * @param valueStr reference to string where the final string value is stored.
     */
    void stringify (string & valueStr) const {
        valueStr.clear ();
        if (isValid ()) {
            valueStr += "Latitude: ";
            char t[50];
            memset (t, '\0', 50);
            snprintf (t, 50, "%f", mLatitude);
            string latitude (t);
            memset (t, '\0', 50);
            valueStr += latitude;
            valueStr += ", Longitude: ";
            snprintf (t, 50, "%f", mLongitude);
            string longitude (t);
            memset (t, '\0', 50);
            valueStr += longitude;
            valueStr += ", Horizontal Acc: ";
            snprintf (t, 50, "%f", mHorizontalAccuracy);
            string horizontalAccuracy (t);
            memset (t, '\0', 50);
            valueStr += horizontalAccuracy;

            if (mHasBearing) {
                valueStr += ", Bearing: ";
                snprintf (t, 50, "%f", mBearing);
                string bearing (t);
                memset (t, '\0', 50);
                valueStr += bearing;
            }

            if (mHasSpeed) {
                valueStr += ", Speed: ";
                snprintf (t, 50, "%f", mSpeed);
                string speed (t);
                memset(t, '\0', 50);
                valueStr += speed;
            }
        } else {
            valueStr += "Position Invalid";
        }
    }

    /**
     * Boolean flag to indicate the presence of UTC time stamp
     */
    bool mHasUtcTimestampInMsec;

    /**
     * UTC time stamp in milliseconds
     */
    int64 mUtcTimestampInMsec;

    /**
     * Boolean flag to indicate the presence of elapsed real time nano seconds
     */
    bool mHasElapsedRealTimeInNanoSecs;

    /**
     * Elapsed real time in nano seconds
     */
    int64 mElapsedRealTimeInNanoSecs;

    /**
     * Boolean flag to indicate the presence of latitude
     */
    bool mHasLatitude;
    /**
     * Latitude
     */
    double mLatitude;

    /**
     * Boolean flag to indicate the presence of longitude
     */
    bool mHasLongitude;

    /**
     * Longitude
     */
    double mLongitude;

    /**
     * Boolean flag to indicate the presence of horizontal accuracy
     */
    bool mHasHorizontalAccuracy;

    /**
     * Horizontal accuracy
     */
    float mHorizontalAccuracy;

    /**
     * Boolean flag to indicate the presence of Altitude with respect to ellipsoid
     */
    bool mHasAltitudeWrtEllipsoid;

    /**
     * Altitude with respect to ellipsoid
     */
    double mAltitudeWrtEllipsoid;

    /**
     * Boolean flag to indicate the presence of Altitude with respect to sea level
     */
    bool mHasAltitudeWrtMeanSeaLevel;

    /**
     * Altitude with respect to sea level
     */
    double mAltitudeWrtMeanSeaLevel;

    /**
     * Boolean flag to indicate the presence of vertical uncertainty
     */
    bool mHasVerticalUncertainity;

    /**
     * Vertical uncertainty
     */
    float mVerticalUncertainity;

    /**
     * Boolean flag to indicate the presence of Bearing
     */
    bool mHasBearing;

    /**
     * Bearing
     */
    float mBearing;

    /**
     * Boolean flag to indicate the presence of Speed
     */
    bool mHasSpeed;

    /**
     * Speed
     */
    float mSpeed;

private:
    /**
     * @brief Copy method
     * @details Copy method
     *
     * @param rhs Reference to IzatLocation indcating where to copy from.
     */
    void makeCopy  (const IzatLocation & rhs) {
        mHasUtcTimestampInMsec = rhs.mHasUtcTimestampInMsec;
        mUtcTimestampInMsec = rhs.mUtcTimestampInMsec;

        mHasElapsedRealTimeInNanoSecs = rhs.mHasElapsedRealTimeInNanoSecs;
        mElapsedRealTimeInNanoSecs = rhs.mElapsedRealTimeInNanoSecs;

        mHasLatitude = rhs.mHasLatitude;
        mLatitude = rhs.mLatitude;

        mHasLongitude= rhs.mHasLongitude;
        mLongitude = rhs.mLongitude;

        mHasHorizontalAccuracy = rhs.mHasHorizontalAccuracy;
        mHorizontalAccuracy = rhs.mHorizontalAccuracy;

        mHasAltitudeWrtEllipsoid = rhs.mHasAltitudeWrtEllipsoid;
        mAltitudeWrtEllipsoid = rhs.mAltitudeWrtEllipsoid;

        mHasAltitudeWrtMeanSeaLevel = rhs.mHasAltitudeWrtMeanSeaLevel;
        mAltitudeWrtMeanSeaLevel = rhs.mAltitudeWrtMeanSeaLevel;

        mHasVerticalUncertainity = rhs.mHasVerticalUncertainity;
        mVerticalUncertainity = rhs.mVerticalUncertainity;

        mHasBearing = rhs.mHasBearing;
        mBearing = rhs.mBearing;

        mHasSpeed = rhs.mHasSpeed;
        mSpeed = rhs.mSpeed;
    }
};

}// namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IZATLOCATION_H__
