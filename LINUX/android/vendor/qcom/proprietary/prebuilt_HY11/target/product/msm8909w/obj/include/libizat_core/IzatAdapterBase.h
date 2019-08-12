/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef IZAT_ADAPTER_BASE_H
#define IZAT_ADAPTER_BASE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <string.h>
#include <LocAdapterBase.h>
#include <geofence.h>
#include <LocAdapterProxyBase.h>

using namespace loc_core;

namespace izat_core {

class IzatApiBase;

class IzatAdapterBase : public LocAdapterProxyBase {
protected:
    IzatApiBase* mIzatApi;
    IzatAdapterBase(const LOC_API_ADAPTER_EVENT_MASK_T mask,
                    ContextBase* context);
    virtual ~IzatAdapterBase();
    const MsgTask* mMsgTask;
public:
    virtual bool gfBreachEvent(int32_t hwId, FlpExtLocation& gpsLocation, int32_t transition);
    virtual bool gfStatusEvent(uint64_t status);
    virtual bool gfResponse(GeofenceResp resp, int32_t status, uint32_t hwId);
    virtual bool saveGeofenceItem(uint32_t hwId, GeoFenceData& geofenceData);
    virtual bool removeGeofenceItem(uint32_t hwId);
    virtual bool pauseGeofenceItem(uint32_t hwId);
    virtual bool resumeGeofenceItem(uint32_t hwId);

    inline void sendMsg(const LocMsg* msg) const {
        mMsgTask->sendMsg(msg);
    }
    inline void sendMsg(const LocMsg* msg) {
        mMsgTask->sendMsg(msg);
    }
    virtual bool handleReportedLocations(const FlpExtLocation* location,
                                         int32_t number_query,
                                         int32_t number_read,
                                         LocBatchingReportedType reportType,
                                         void* cbForOnQueryRequest = NULL);
    virtual bool getLocations(int32_t last_n_locations,
                              LocBatchingReportedType reportType,
                              void* cbForOnQueryRequest = NULL);
    virtual bool setUserPreference(bool user_pref);
    virtual void handleEngineUpEvent();
    virtual void handleEngineDownEvent();
    virtual bool updateGfBreachLocation(FlpExtLocation& gpsLocation);
    virtual bool reportPosition(UlpLocation &location,
                                GpsLocationExtended &locationExtended,
                                enum loc_sess_status status,
                                LocPosTechMask loc_technology_mask);

    // for GDT AP
    virtual bool onGdtUploadBeginEvent(int32_t service,
                                       int32_t session,
                                       const char* filePath,
                                       uint32_t filePath_len);
    virtual bool onGdtUploadEndEvent(int32_t service, int32_t session, int32_t status);
    virtual bool handleGtpApStatusResp(int32_t status, uint16_t clientSwVer,
                                       uint8_t asnMajVer, uint8_t asnMinVer, uint8_t asnPtVer);
    virtual bool onGdtDownloadBeginEvent(int32_t service, uint32_t session,
                                         uint32_t respTimeoutInterval,
                                         const uint8_t* clientInfo, uint32_t clientInfo_len,
                                         const uint8_t* mobileStatusData, uint32_t mobileStatusData_len,
                                         const char* filePath, uint32_t filePath_len,
                                         uint32_t powerBudgetInfo, int8_t powerBudgetInfo_valid,
                                         uint32_t powerBudgetAllowance, int8_t powerBudgetAllowance_valid);
    virtual bool onGdtReceiveDoneEvent(int32_t service, uint32_t session, int32_t status);
    virtual bool onGdtDownloadEndEvent(int32_t service, uint32_t session, int32_t status);

    // XTRA Client 2.0
    virtual bool onReceiveXtraServers(const char* server1, const char* server2, const char* server3);
    virtual bool onRequestXtraData();
    virtual bool onRequestNtpTime();

#ifdef ON_TARGET_TEST
#endif
};

}  // namespace izat_core

#ifdef __cplusplus
}
#endif

#endif /* IZAT_ADAPTER_BASE_H */
