/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemConcreteTypes

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_DATAITEMCONCRETETYPES_H__
#define __IZAT_MANAGER_DATAITEMCONCRETETYPES_H__

#include <string>
#include <comdef.h>
#include <DataItemBase.h>
#include <DataItemConcreteTypeDefaultValues.h>

#define MAC_ADDRESS_LENGTH 6

struct LOC_RilServiceInfo;
struct LOC_RilCellInfo;

// Add Concrete DataItem definitions here. All concrete DataItems
// must implement DataItemBase.

namespace izat_manager
{
class IDataItemSerializer;
class IDataItemDeSerializer;
class IDataItemCopier;
class IDataItemStringifier;
class IDataItem;

class AirplaneModeDataItem : public DataItemBase  {

public:
    AirplaneModeDataItem(bool mode = AIRPLANEMODE_DEFAULT_MODE);
    ~AirplaneModeDataItem();
    virtual DataItemId getId();
    virtual IDataItemSerializer * getSerializer();
    virtual IDataItemDeSerializer * getDeSerializer();
    virtual IDataItemCopier * getCopier();
    virtual IDataItemStringifier * getStringifier();
// Data members
    bool mMode;
};

class ENHDataItem : public DataItemBase {

public:
    ENHDataItem(bool enabled = ENH_DEFAULT_ENABLED);
    ~ENHDataItem();
    virtual DataItemId getId();
    virtual IDataItemSerializer * getSerializer();
    virtual IDataItemDeSerializer * getDeSerializer();
    virtual IDataItemCopier * getCopier();
    virtual IDataItemStringifier * getStringifier();

// Data members
    bool mEnabled;
};

class GPSStateDataItem : public DataItemBase {

public:
    GPSStateDataItem(bool enabled = GPSSTATE_DEFAULT_ENABLED);
    ~GPSStateDataItem();
    virtual DataItemId getId();
    virtual IDataItemSerializer * getSerializer();
    virtual IDataItemDeSerializer * getDeSerializer();
    virtual IDataItemCopier * getCopier();
    virtual IDataItemStringifier * getStringifier();
// Data members
    bool mEnabled;
};

class NLPStatusDataItem : public DataItemBase {

public:
    NLPStatusDataItem(bool enabled = NLPSTATUS_DEFAULT_ENABLED);
    ~NLPStatusDataItem();
    virtual DataItemId getId();
    virtual IDataItemSerializer * getSerializer();
    virtual IDataItemDeSerializer * getDeSerializer();
    virtual IDataItemCopier * getCopier();
    virtual IDataItemStringifier * getStringifier();
// Data members
    bool mEnabled;
};

class WifiHardwareStateDataItem : public DataItemBase {

public:
    WifiHardwareStateDataItem(bool enabled = WIFIHARDWARESTATE_DEFAULT_ENABLED);
    ~WifiHardwareStateDataItem();
    virtual DataItemId getId();
    virtual IDataItemSerializer * getSerializer();
    virtual IDataItemDeSerializer * getDeSerializer();
    virtual IDataItemCopier * getCopier();
    virtual IDataItemStringifier * getStringifier();
// Data members
    bool mEnabled;
};

class ScreenStateDataItem : public DataItemBase {

public:
    ScreenStateDataItem(bool enabled = SCREEN_STATE_DEFAULT_ENABLED);
    ~ScreenStateDataItem();
    virtual DataItemId getId();
    virtual IDataItemSerializer * getSerializer();
    virtual IDataItemDeSerializer * getDeSerializer();
    virtual IDataItemCopier * getCopier();
    virtual IDataItemStringifier * getStringifier();
// Data members
    bool mState;
};

class PowerConnectStateDataItem : public DataItemBase {

public:
    PowerConnectStateDataItem(bool enabled = POWER_CONNECT_STATE_DEFAULT_ENABLED);
    ~PowerConnectStateDataItem();
    virtual DataItemId getId();
    virtual IDataItemSerializer * getSerializer();
    virtual IDataItemDeSerializer * getDeSerializer();
    virtual IDataItemCopier * getCopier();
    virtual IDataItemStringifier * getStringifier();
// Data members
    bool mState;
};

class TimeZoneChangeDataItem : public DataItemBase {

public:
    TimeZoneChangeDataItem(
        int64 currTimeMillis = TIME_DEFAULT_CURRTIME,
        int32 rawOffset = TIMEZONE_DEFAULT_RAWOFFSET,
        int32 dstOffset = TIMEZONE_DEFAULT_DSTOFFSET
    );
    ~TimeZoneChangeDataItem();
    virtual DataItemId getId();
    virtual IDataItemSerializer * getSerializer();
    virtual IDataItemDeSerializer * getDeSerializer();
    virtual IDataItemCopier * getCopier();
    virtual IDataItemStringifier * getStringifier();
// Data members
    int64 mCurrTimeMillis;
    int32 mRawOffsetTZ;
    int32 mDstOffsetTZ;
};

class TimeChangeDataItem : public DataItemBase {

public:
    TimeChangeDataItem(
        int64 currTimeMillis = TIME_DEFAULT_CURRTIME,
        int32 rawOffset = TIMEZONE_DEFAULT_RAWOFFSET,
        int32 dstOffset = TIMEZONE_DEFAULT_DSTOFFSET
    );
    ~TimeChangeDataItem();
    virtual DataItemId getId();
    virtual IDataItemSerializer * getSerializer();
    virtual IDataItemDeSerializer * getDeSerializer();
    virtual IDataItemCopier * getCopier();
    virtual IDataItemStringifier * getStringifier();
// Data members
    int64 mCurrTimeMillis;
    int32 mRawOffsetTZ;
    int32 mDstOffsetTZ;
};

class ShutdownStateDataItem : public DataItemBase {

public:
    ShutdownStateDataItem(bool state = SHUTDOWN_DEFAULT_STATE);
    ~ShutdownStateDataItem();
    virtual DataItemId getId();
    virtual IDataItemSerializer * getSerializer();
    virtual IDataItemDeSerializer * getDeSerializer();
    virtual IDataItemCopier * getCopier();
    virtual IDataItemStringifier * getStringifier();
// Data members
    bool mState;
};


class AssistedGpsDataItem : public DataItemBase {

public:
    AssistedGpsDataItem(bool enabled = ASSISTED_GPS_DEFAULT_ENABLED);
    ~AssistedGpsDataItem();
    virtual DataItemId getId();
    virtual IDataItemSerializer * getSerializer();
    virtual IDataItemDeSerializer * getDeSerializer();
    virtual IDataItemCopier * getCopier();
    virtual IDataItemStringifier * getStringifier();
// Data members
    bool mEnabled;
};

class NetworkInfoDataItem : public DataItemBase {

public:
    NetworkInfoDataItem(
        int32 type = NETWORKINFO_DEFAULT_TYPE,
        std::string typeName = NETWORKINFO_DEFAULT_TYPENAME,
        std::string subTypeName = NETWORKINFO_DEFAULT_SUBTYPENAME,
        bool available = NETWORKINFO_DEFAULT_AVAILABLE,
        bool connected = NETWORKINFO_DEFAULT_CONNECTED,
        bool roaming = NETWORKINFO_DEFAULT_ROAMING
    );
    ~NetworkInfoDataItem();
    virtual DataItemId getId();
    virtual IDataItemSerializer * getSerializer();
    virtual IDataItemDeSerializer * getDeSerializer();
    virtual IDataItemCopier * getCopier();
    virtual IDataItemStringifier * getStringifier();
// Data members
    int32 mType;
    std::string mTypeName;
    std::string mSubTypeName;
    bool mAvailable;
    bool mConnected;
    bool mRoaming;
};

class ServiceStatusDataItem : public DataItemBase {

public:
    ServiceStatusDataItem(int32 serviceState = SERVICESTATUS_DEFAULT_STATE);
    ~ServiceStatusDataItem();
    virtual DataItemId getId();
    virtual IDataItemSerializer * getSerializer();
    virtual IDataItemDeSerializer * getDeSerializer();
    virtual IDataItemCopier * getCopier();
    virtual IDataItemStringifier * getStringifier();
// Data members
    int32 mServiceState;
};

class ModelDataItem : public DataItemBase {

public:
    ModelDataItem(const std::string & state = MODEL_DEFAULT_NAME);
    ~ModelDataItem();
    virtual DataItemId getId();
    virtual IDataItemSerializer * getSerializer();
    virtual IDataItemDeSerializer * getDeSerializer();
    virtual IDataItemCopier * getCopier();
    virtual IDataItemStringifier * getStringifier();
// Data members
    std::string mModel;
};

class ManufacturerDataItem : public DataItemBase {

public:
    ManufacturerDataItem(const std::string & state = MANUFACTURER_DEFAULT_NAME);
    ~ManufacturerDataItem();
    virtual DataItemId getId();
    virtual IDataItemSerializer * getSerializer();
    virtual IDataItemDeSerializer * getDeSerializer();
    virtual IDataItemCopier * getCopier();
    virtual IDataItemStringifier * getStringifier();
// Data members
    std::string mManufacturer;
};

class RilServiceInfoDataItem : public DataItemBase {

public:
    RilServiceInfoDataItem(struct LOC_RilServiceInfo * serviceInfo = NULL);
    ~RilServiceInfoDataItem();
    virtual DataItemId getId();
    virtual IDataItemSerializer * getSerializer();
    virtual IDataItemDeSerializer * getDeSerializer();
    virtual IDataItemCopier * getCopier();
    virtual IDataItemStringifier * getStringifier();
// Data members
    struct LOC_RilServiceInfo * mServiceInfo;
private:
    bool mDestroy;
};

class RilCellInfoDataItem : public DataItemBase {

public:
    RilCellInfoDataItem(struct LOC_RilCellInfo * cellInfo = NULL);
    ~RilCellInfoDataItem();
    virtual DataItemId getId();
    virtual IDataItemSerializer * getSerializer();
    virtual IDataItemDeSerializer * getDeSerializer();
    virtual IDataItemCopier * getCopier();
    virtual IDataItemStringifier * getStringifier();
// Data members
    struct LOC_RilCellInfo * mCellInfo;
private:
    bool mDestroy;
};

class WifiSupplicantStatusDataItem : public DataItemBase {

public:
    WifiSupplicantStatusDataItem();
    ~WifiSupplicantStatusDataItem();
    virtual DataItemId getId();
    virtual IDataItemSerializer * getSerializer();
    virtual IDataItemDeSerializer * getDeSerializer();
    virtual IDataItemCopier * getCopier();

    // Data members
    typedef enum WifiSupplicantState {
        DISCONNECTED,
        INTERFACE_DISABLED,
        INACTIVE,
        SCANNING,
        AUTHENTICATING,
        ASSOCIATING,
        ASSOCIATED,
        FOUR_WAY_HANDSHAKE,
        GROUP_HANDSHAKE,
        COMPLETED,
        DORMANT,
        UNINITIALIZED,
        INVALID
    } WifiSupplicantState;

    /* Represents whether access point attach state*/
    WifiSupplicantState mState;
    /* Represents info on whether ap mac address is valid */
    bool mApMacAddressValid;
    /* Represents mac address of the wifi access point*/
    uint8 mApMacAddress[MAC_ADDRESS_LENGTH];
    /* Represents info on whether ap SSID is valid */
    bool mWifiApSsidValid;
    /* Represents Wifi SSID string*/
    std::string mWifiApSsid;
    virtual IDataItemStringifier * getStringifier();
};

class TacDataItem : public DataItemBase {

public:
    TacDataItem(const std::string & state = TAC_DEFAULT_NAME);
    ~TacDataItem();
    virtual DataItemId getId();
    virtual IDataItemSerializer * getSerializer();
    virtual IDataItemDeSerializer * getDeSerializer();
    virtual IDataItemCopier * getCopier();
    virtual IDataItemStringifier * getStringifier();
// Data members
    std::string mValue;
};

class MccmncDataItem : public DataItemBase {

public:
    MccmncDataItem(const std::string & state = MCCMNC_DEFAULT_NAME);
    ~MccmncDataItem();
    virtual DataItemId getId();
    virtual IDataItemSerializer * getSerializer();
    virtual IDataItemDeSerializer * getDeSerializer();
    virtual IDataItemCopier * getCopier();
    virtual IDataItemStringifier * getStringifier();
// Data members
    std::string mValue;
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_DATAITEMCONCRETETYPES_H__
