/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#ifndef GEOFENCE_ADAPTER_H
#define GEOFENCE_ADAPTER_H

#include <IzatAdapterBase.h>
#include <IzatApiBase.h>
#include <geofence.h>
#include <LocDualContext.h>
#include <map>

using namespace loc_core;
using namespace izat_core;

class GeoFencer;
class GeofenceAdapter;

struct GeofenceKey {
    uint32_t afwId;
    GeoFencer* client;
    inline GeofenceKey(uint32_t id, void* cl) :
        afwId(id), client((GeoFencer*)cl) {}
};
inline bool operator <(GeofenceKey const& left, GeofenceKey const& right) {
    return left.afwId < right.afwId || (left.afwId == right.afwId && left.client < right.client);
}
typedef std::map<uint32_t, GeoFenceData> GeoFencesMap;
typedef std::map<GeofenceKey, uint32_t> GeoFenceIdMap;

struct MsgGeofenceBreachEvent : public LocMsg {
    GeoFencer* mClient;
    int32_t mAfwId;
    FlpExtLocation mLocation;
    int32_t mTransition;
    inline MsgGeofenceBreachEvent(GeoFencer* client, int32_t afwId, const FlpExtLocation& gpsLocation, int32_t transition) :
        LocMsg(), mClient(client), mAfwId(afwId), mLocation(gpsLocation), mTransition(transition) {}
    virtual void proc() const;
};

struct MsgGeofenceStatusEvent : public LocMsg {
    uint64_t mStatus;
    GeoFencer* mClient;
    inline MsgGeofenceStatusEvent(uint64_t status, GeoFencer* client) :
        LocMsg(), mStatus(status), mClient(client){}
    virtual void proc() const;
};

struct MsgGeofenceResp : public LocMsg {
    GeofenceResp mResp;
    int32_t mStatus;
    GeoFenceData mData;
    GeofenceAdapter* mAdapter;
    inline MsgGeofenceResp(GeofenceResp resp,
                          int32_t status,
                          GeoFenceData& data,
                          GeofenceAdapter* adapter) :
        LocMsg(),
        mResp(resp),
        mStatus(status),
        mData(data),
        mAdapter(adapter){}
    virtual void proc() const;
};

class GeofenceAdapter : public IzatAdapterBase {

    static GeofenceAdapter* mMe;
    static const LOC_API_ADAPTER_EVENT_MASK_T mMask;
    GeofenceAdapter(LocThread::tCreate tCreator);
    GeofenceAdapter(LocMsg* associateMsg);
    inline virtual ~GeofenceAdapter() {}

public:

    bool getHwIdFromAfwId(int32_t afwId, GeoFencer* client,  uint32_t& hwId);

    GeoFencer* mClients[3];
    static GeofenceAdapter* get(GeoFencer* client, LocThread::tCreate tCreator);
    static GeofenceAdapter* get(GeoFencer* client, LocMsg* assoMsg);
    static pthread_mutex_t mGetAdapterMutex;

    void updateRegisteredEvents();
    void addGfClients(GeoFencer* client);
    bool gfBreachEvent(int32_t hwId, FlpExtLocation& gpsLocation, int32_t transition);
    bool gfStatusEvent(uint64_t status);
    bool gfResponse(GeofenceResp resp, int32_t status, uint32_t hwId);

    bool addGeofence(GeoFenceData& geofenceData, bool needsResponse);
    bool removeGeofence(GeoFencer* client, int32_t afwId);
    bool pauseGeofence(GeoFencer* client, int32_t afwId);
    bool resumeGeofence(GeoFencer* client, int32_t afwId, uint32_t breachMask);
    bool modifyGeofence(GeoFenceData& geofenceData);

    bool saveGeofenceItem(uint32_t hwId, GeoFenceData& geofenceData);
    bool removeGeofenceItem(uint32_t hwId);
    bool pauseGeofenceItem(uint32_t hwId);
    bool resumeGeofenceItem(uint32_t hwId);
    void modifyGeoFenceItem(const GeoFenceData& data, uint32_t hwId);
    bool getGeofenceDataFromHwId(uint32_t hwId, GeoFenceData& data);
    void dump();
    virtual void handleEngineUpEvent();
    virtual bool updateGfBreachLocation(FlpExtLocation& gpsLocation);

private:

    GeoFencesMap mGeoFences; //map hwId to GeoFenceData
    GeoFenceIdMap mGeoFenceIds; //map of GeofenceKey to hwId
};

#endif /* GEOFENCE_ADAPTER_H */
