/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef IZAT_API_V02_H
#define IZAT_API_V02_H

#include <LocApiV02.h>
#include <IzatApiBase.h>
#include <geofence.h>
#include <vector>

namespace lbs_core {
    class LocApiProxyV02;
};

using namespace loc_core;
using namespace lbs_core;

namespace izat_core {

typedef std::vector<uint32_t> HwIdsOfBatchingOnFull;

class IzatApiV02 : public IzatApiBase {
    uint32_t mBatchSize;
    uint32_t mDesiredBatchSize;
    uint32_t mSupportedLBFeatures;
    HwIdsOfBatchingOnFull mHwIdsOfBatchingOnFull;
    bool mBatchingSupported;
    bool mUpdatingTBFOnTheFlySupported;
    bool mTimeBasedTrackingInProgress;
    bool mDistanceBasedTrackingInProgress;
    bool mFlpSessionInProgress;
    int updateTimeBasedTracking(int64_t period_ns);
    int startTimeBasedTracking(int64_t period_ns,
                               int32_t accuracy);
    int startDistanceBasedTracking(const FlpExtBatchOptions& options,
                                   uint32_t hwId = FLP_INVALID_HW_ID);
    int startModemBatching(const FlpExtBatchOptions& options,
                           int32_t accuracy,
                           int32_t timeout,
                           uint32_t hwId = FLP_INVALID_HW_ID);
    int stopTimeBasedTracking();
    int stopDistanceBasedTracking(uint32_t hwId = FLP_INVALID_HW_ID);
    int stopModemBatching(uint32_t hwId = FLP_INVALID_HW_ID);
    int queryBatchBufferOnModem(int32_t size);

    void fillGeneralAssistData(GnssAidingData flags,
                               qmiLocDeleteAssistDataReqMsgT_v02 &req);
    void fillSvInfoAssistData(GnssAidingData flags,
                              qmiLocDeleteAssistDataReqMsgT_v02 &req);
public:
    IzatApiV02(LocApiProxyV02* locApiProxy);
    inline virtual ~IzatApiV02() {}

    inline virtual void* getSibling() { return (IzatApiBase*)this; }

    /* event callback registered with the loc_api v02 interface */
    virtual int eventCb(locClientHandleType client_handle,
                         uint32_t loc_event_id,
                         locClientEventIndUnionType loc_event_payload);
    // For Geofence
    void GeofenceBreach(const qmiLocEventGeofenceBreachIndMsgT_v02* breachInfo);
    void batchGeofenceBreach(const qmiLocEventGeofenceBatchedBreachIndMsgT_v02* batchedBreachInfo);
    void GeofenceStatus(const qmiLocEventGeofenceGenAlertIndMsgT_v02* alertInfo);
    void geofenceDwell(const qmiLocEventGeofenceBatchedDwellIndMsgT_v02 *dwellEvent);
    virtual int addGeofence(GeoFenceData geofenceData, bool needsResponse);
    virtual int removeGeofence(uint32_t hwId, int32_t afwId);
    virtual int pauseGeofence(uint32_t hwId, int32_t afwId);
    virtual int resumeGeofence(uint32_t hwId, int32_t afwId, uint32_t breachMask);
    virtual int modifyGeofence(uint32_t hwId, GeoFenceData geofenceData);
    // set user preference
    virtual int setUserPref(bool user_pref);
    virtual int injectApCache(APCacheData data);
    virtual int injectApCacheBlacklist(APCacheBlacklistData data);
    virtual int sendBestAvailablePosReq(GpsLocation &zppLoc);
    virtual int sendBestAvailablePosReq(GpsLocation &zppLoc, LocPosTechMask &tech_mask);

    virtual void setBatchingSupport(uint32_t supportedLBFeatures);
    virtual void setBatchSize(int32_t size, bool hasSSR = false);
    virtual int cleanUpFlpSession();
    virtual int startFlpSession(const FlpExtBatchOptions& options,
                                int32_t accuracy,
                                int32_t timeout,
                                uint32_t hwId = FLP_INVALID_HW_ID);
    virtual int updateFlpSession(const FlpExtBatchOptions& options,
                                 int32_t accuracy,
                                 int32_t timeout,
                                 uint32_t hwId = FLP_INVALID_HW_ID);
    virtual int stopFlpSession(uint32_t hwId = FLP_INVALID_HW_ID,
                               LocApiSelectionType apiSelected = LOC_API_SELECTION_UN_SELECTED);
    virtual int getBatchedLocation(int32_t lastNlocations,
                                   LocBatchingReportedType reportType,
                                   void* cbForOnQueryRequest = NULL);
    virtual int injectLocation(FlpExtLocation location);
    virtual bool isMessageSupported(LocCheckingMessagesID msgID);
    void readModemLocations(FlpExtLocation* pLocationPiece,
                            int32_t lastNlocations,
                            int32_t& numbOfEntries);

    // for TDP
    virtual int sendGtpEnhancedCellConfigBlocking(e_premium_mode mode);
    virtual int sendGtpCellConfigurationsBlocking(const char* data, uint32_t len);

    virtual int sendGdtUploadBeginResponse(int32_t service, int32_t session, int32_t status);
    virtual int sendGdtUploadEndResponse(int32_t service, int32_t session, int32_t status);

    virtual int sendGtpApStatus(int32_t gtpApDbStatus, uint64_t gtpApPcid64, int8_t gtpApPcid64_valid,
                                const char* oemId, int8_t oemId_valid,
                                const char* modelId, int8_t modelId_valid);

    virtual int sendGdtDownloadBeginResponse(int32_t service, uint32_t session, int32_t status,
                                             const uint8_t* respLocInfo, int8_t respLocInfo_valid,
                                             uint32_t respLocInfo_len);
    virtual int sendGdtDownloadReadyStatus(int32_t service, uint32_t session, int32_t status,
                                           const char* filePath);
    virtual int sendGdtReceiveDoneResponse(int32_t service, uint32_t session, int32_t status);
    virtual int sendGdtDownloadEndResponse(int32_t service, uint32_t session, int32_t status);

    // Delete aiding data
    virtual int deleteAidingData(GnssAidingData flags);

    // XTRA Client 2.0
    virtual int setXtraVersionCheck(enum XtraVersionCheck check);
    virtual int injectIzatPcid(uint64_t pcid);
    virtual int injectXtraData_legacy(const char* data, uint32_t len);
    virtual int injectXtraData(const char* data, uint32_t len);
    virtual int injectNtpTime(const int64_t time, const int64_t reference, const int64_t uncertainty);
    virtual int requestXtraServers();

private:
    void onDbtPosReportEvent(const qmiLocEventDbtPositionReportIndMsgT_v02* pDbtPosReport);
    void onGdtUploadBeginEvent(const qmiLocEventGdtUploadBeginStatusReqIndMsgT_v02* pInfo);
    void onGdtUploadEndEvent(const qmiLocEventGdtUploadEndReqIndMsgT_v02* pInfo);
    void onGdtDownloadBeginEvent(const qmiLocEventGdtDownloadBeginReqIndMsgT_v02* pInfo);
    void onGdtReceiveDoneEvent(const qmiLocEventGdtReceiveDoneIndMsgT_v02* pInfo);
    void onGdtDownloadEndEvent(const qmiLocEventGdtDownloadEndReqIndMsgT_v02* pInfo);
};

}  // namespace izat_core

#endif //IZAT_API_V02_H
