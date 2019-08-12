/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  LocationReport

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_LOCATIONREPORT_H__
#define __IZAT_MANAGER_LOCATIONREPORT_H__

#include "IzatLocation.h"
#include "WiperData.h"

namespace izat_manager
{

class LocationReport : public IzatLocation {
public:

    typedef enum ProviderSource {

        PROVIDER_SOURCE_FUSED     = 0x00000001,
        PROVIDER_SOURCE_NETWORK   = 0x00000002,
        PROVIDER_SOURCE_GNSS      = 0x00000004,
    } ProviderSource;

    typedef enum ProcessorSrc {
        ProcessorSrc_AP,
        ProcessorSrc_Modem
    } ProcessorSrc;

    // mProviderSource is a bitmask of providers. It indicates to the upper layer
    // which requested framework provider this location is fit for.
    // The possible valid values for this field are as follows
    // IZAT_PROVIDER_SOURCE_FUSED,
    // IZAT_PROVIDER_SOURCE_NETWORK,
    // IZAT_PROVIDER_SOURCE_GNSS,
    // IZAT_PROVIDER_SOURCE_FUSED | IZAT_PROVIDER_SOURCE_NETWORK,
    // IZAT_PROVIDER_SOURCE_FUSED | IZAT_PROVIDER_SOURCE_GNSS
    // The following are invalid
    // IZAT_PROVIDER_SOURCE_INVALID,
    // IZAT_PROVIDER_SOURCE_GNSS | IZAT_PROVIDER_SOURCE_NETWORK
    bool mHasProviderSource;
    ProviderSource mProviderSource;

    bool mHasHorizontalConfidence;
    uint8 mHorizontalConfidence;

    bool mHasHorizontalReliability;
    ReliabilityValue mHorizontalReliability;

    bool mHasVerticalConfidence;
    uint8 mVerticalConfidence;

    bool mHasVerticalReliability;
    ReliabilityValue mVerticalReliability;

    bool mHasPositionSource;
    PositionSourceType mPositionSource;

    bool mHasProcessorSource;
    ProcessorSrc mProcessorSource;

    bool mHasPropagatedFix;
    bool mIsPropagatedFix;

    bool mIsIntermediateFix;

    LocationReport() {
        resetInternal();
    }

    LocationReport(const LocationReport &locReport) :
        IzatLocation(locReport) {
        makeCopy(locReport);
    }

    LocationReport(const IzatLocation &izatLoc) :
        IzatLocation(izatLoc) {
        resetInternal();
    }

    LocationReport & operator=(const LocationReport &locReport) {
        IzatLocation::operator=(locReport);
        makeCopy(locReport);
        return *this;
    }

    virtual ~LocationReport() {
        resetInternal();
    };

    void reset()
    {
        IzatLocation::reset();
        resetInternal();
    }

    void stringify (string & valueStr) const {
        if (isValid ()) {
            IzatLocation :: stringify (valueStr);
            if (mHasProviderSource) {
                char t[10];
                snprintf (t, 10, "%d", mProviderSource);
                string providerSource (t);
                valueStr += ", Provider Source (Fused - 0x00000001, Network - 0x00000002, GNSS - 0x00000004): ";
                valueStr += providerSource;
            }

            if (mHasPositionSource) {
                char t[10];
                snprintf (t, 10, "%d", mPositionSource);
                string positionSource (t);
                valueStr += ", Position Source (GNSS-0, CELLID-1, ENH_CELLID-2, WIFI-3, TERRESTRIAL-4, GNSS_TERRESTRIAL_HYBRID-5, OTHER-6): ";
                valueStr += positionSource;
            }

            if (mHasProcessorSource) {
                char t[10];
                snprintf (t, 10, "%d", mProcessorSource);
                string processorSource (t);
                valueStr += ", Processor Source (AP - 0, Modem - 1): ";
                valueStr += processorSource;
            }

            if (mHasPropagatedFix) {
                valueStr += ", Propagated Fix: ";
                valueStr += mIsPropagatedFix ? "True" : "False";
            }
        } else {
            valueStr += "Not Valid";
        }
    }
protected:

private:
    void makeCopy(const LocationReport & locReport) {
        mHasProviderSource = locReport.mProviderSource;
        mProviderSource = locReport.mProviderSource;

        mHasHorizontalConfidence = locReport.mHasHorizontalConfidence;
        mHorizontalConfidence = locReport.mHorizontalConfidence;

        mHasHorizontalReliability = locReport.mHasHorizontalConfidence;
        mHorizontalReliability = locReport.mHorizontalReliability;

        mHasVerticalConfidence = locReport.mHasVerticalConfidence;
        mVerticalConfidence = locReport.mVerticalConfidence;

        mHasVerticalReliability = locReport.mHasVerticalReliability;
        mVerticalReliability = locReport.mVerticalReliability;

        mHasPositionSource = locReport.mHasPositionSource;
        mPositionSource = locReport.mPositionSource;

        mHasProcessorSource = locReport.mHasProcessorSource;
        mProcessorSource = locReport.mProcessorSource;

        mHasPropagatedFix = locReport.mHasPropagatedFix;
        mIsPropagatedFix = locReport.mIsPropagatedFix;

        mIsIntermediateFix = locReport.mIsIntermediateFix;
    }

    void resetInternal() {
        mHasProviderSource = false;
        mHasHorizontalConfidence = false;
        mHasHorizontalReliability = false;
        mHasVerticalConfidence = false;
        mHasVerticalReliability = false;
        mHasPositionSource = false;
        mHasProcessorSource = false;
        mHasPropagatedFix = false;
        mIsIntermediateFix = false;
    }
};

}// namespace izatmanager

#endif // #ifndef __IZAT_MANAGER_LOCATIONREPORT_H__
