/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemConcreteTypes Implementation

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#include <string>
#include <comdef.h>
#include <DataItemId.h>
#include <IDataItem.h>
#include <DataItemBase.h>
#include <DataItemConcreteTypeDefaultValues.h>
#include <DataItemConcreteTypes.h>
#include <DataItemSerializerConcreteTypes.h>
#include <DataItemDeSerializerConcreteTypes.h>
#include <DataItemCopierConcreteTypes.h>
#include <DataItemStringifierConcreteTypes.h>
#include <loc_ril.h>
#include <postcard.h>
#include <WiperData.h>

using namespace izat_manager;

using namespace qc_loc_fw;

// Ctors
AirplaneModeDataItem :: AirplaneModeDataItem
 (bool mode)
:
DataItemBase
 (
AIRPLANEMODE_DATA_ITEM_ID
),
mMode (mode)
{}

ENHDataItem :: ENHDataItem
 (bool enabled)
:
DataItemBase
 (
ENH_DATA_ITEM_ID
),
mEnabled (enabled)
{}

GPSStateDataItem :: GPSStateDataItem
 (bool enabled)
:
DataItemBase
 (
GPSSTATE_DATA_ITEM_ID
),
mEnabled (enabled)
{}

NLPStatusDataItem :: NLPStatusDataItem
 (bool enabled)
:
DataItemBase
 (
NLPSTATUS_DATA_ITEM_ID
),
mEnabled (enabled)
{}

WifiHardwareStateDataItem :: WifiHardwareStateDataItem
 (bool enabled)
:
DataItemBase
 (
WIFIHARDWARESTATE_DATA_ITEM_ID
),
mEnabled (enabled)
{}

ScreenStateDataItem :: ScreenStateDataItem
 (bool state)
:
DataItemBase
 (
SCREEN_STATE_DATA_ITEM_ID
),
mState (state)
{}

PowerConnectStateDataItem :: PowerConnectStateDataItem
 (bool state)
:
DataItemBase
 (
POWER_CONNECTED_STATE_DATA_ITEM_ID
),
mState (state)
{}

TimeZoneChangeDataItem :: TimeZoneChangeDataItem
 (
    int64 currTimeMillis,
    int32 rawOffset,
    int32 dstOffset
)
:
DataItemBase
 (
TIMEZONE_CHANGE_DATA_ITEM_ID
),
mCurrTimeMillis (currTimeMillis),
mRawOffsetTZ (rawOffset),
mDstOffsetTZ (dstOffset)
{}

TimeChangeDataItem :: TimeChangeDataItem
 (
    int64 currTimeMillis,
    int32 rawOffset,
    int32 dstOffset
)
:
DataItemBase
 (
TIME_CHANGE_DATA_ITEM_ID
),
mCurrTimeMillis (currTimeMillis),
mRawOffsetTZ (rawOffset),
mDstOffsetTZ (dstOffset)
{}

ShutdownStateDataItem :: ShutdownStateDataItem
 (bool state)
:
DataItemBase
 (
SHUTDOWN_STATE_DATA_ITEM_ID
),
mState (state)
{}

AssistedGpsDataItem :: AssistedGpsDataItem
 (bool enabled)
:
DataItemBase
 (
ASSISTED_GPS_DATA_ITEM_ID
),
mEnabled (enabled)
{}

NetworkInfoDataItem :: NetworkInfoDataItem
 (
    int32 type,
    std :: string typeName,
    std :: string subTypeName,
    bool available,
    bool connected,
    bool roaming
)
:
DataItemBase
 (
NETWORKINFO_DATA_ITEM_ID
),
mType (type),
mTypeName (typeName),
mSubTypeName (subTypeName),
mAvailable (available),
mConnected (connected),
mRoaming (roaming)
{}

ServiceStatusDataItem :: ServiceStatusDataItem
 (int32 serviceState)
:
DataItemBase
 (
SERVICESTATUS_DATA_ITEM_ID
),
mServiceState (serviceState)
{}

ModelDataItem :: ModelDataItem
 (const std :: string & name)
:
DataItemBase
 (
MODEL_DATA_ITEM_ID
),
mModel (name)
{}

ManufacturerDataItem :: ManufacturerDataItem
 (const std :: string & name)
:
DataItemBase
 (
MANUFACTURER_DATA_ITEM_ID
),
mManufacturer (name)
{}

RilServiceInfoDataItem :: RilServiceInfoDataItem
 (LOC_RilServiceInfo * serviceInfo)
:
DataItemBase
 (
RILSERVICEINFO_DATA_ITEM_ID
),
mServiceInfo (serviceInfo),
mDestroy (false)
{
    if (mServiceInfo == NULL) {
        mServiceInfo = new  (std :: nothrow) LOC_RilServiceInfo;
        if (mServiceInfo) {
            mDestroy = true;
            // Initialize to default values
            mServiceInfo->valid_mask = RTLLSERVICEINFO_DEFAULT_VALID_MASK;
            mServiceInfo->airIf_type = RTLLSERVICEINFO_DEFAULT_AIRIF_TYPE;
            mServiceInfo->carrierAirIf_type = RTLLSERVICEINFO_DEFAULT_CARRIER_AIRIF_TYPE;
            mServiceInfo->carrierMcc = RTLLSERVICEINFO_DEFAULT_CARRIER_MCC;
            mServiceInfo->carrierMnc = RTLLSERVICEINFO_DEFAULT_CARRIER_MNC;
            mServiceInfo->carrierNameLen = RTLLSERVICEINFO_DEFAULT_CARRIER_NAMELEN;
            strlcpy (mServiceInfo->carrierName, RTLLSERVICEINFO_DEFAULT_CARRIER_NAME, LOC_RIL_CARRIER_NAME_MAX_LEN);
        }
    }
}


RilCellInfoDataItem :: RilCellInfoDataItem
 (LOC_RilCellInfo * cellInfo)
:
DataItemBase
 (
RILCELLINFO_DATA_ITEM_ID
),
mCellInfo (cellInfo),
mDestroy (false)
{
    if (mCellInfo == NULL) {
        mCellInfo = new  (std :: nothrow) LOC_RilCellInfo;
        if (mCellInfo) {
            mDestroy = true;
            // Initialize to default values
            mCellInfo->valid_mask = RILLCELLINFO_DEFAULT_VALID_MASK;
            mCellInfo->nwStatus = static_cast<LOC_NWstatus> (RILLCELLINFO_DEFAULT_NETWORK_STATUS);
            mCellInfo->rtType = static_cast<LOC_RilTechType> (RILLCELLINFO_DEFAULT_RIL_TECH_TYPE);
            mCellInfo->u.cdmaCinfo.mcc = RILLCELLINFO_DEFAULT_CDMA_CINFO_MCC;
            mCellInfo->u.cdmaCinfo.sid = RILLCELLINFO_DEFAULT_CDMA_CINFO_SID;
            mCellInfo->u.cdmaCinfo.nid = RILLCELLINFO_DEFAULT_CDMA_CINFO_NID;
            mCellInfo->u.cdmaCinfo.bsid = RILLCELLINFO_DEFAULT_CDMA_CINFO_BSID;
            mCellInfo->u.cdmaCinfo.bslat = RILLCELLINFO_DEFAULT_CDMA_CINFO_BSLAT;
            mCellInfo->u.cdmaCinfo.bslon = RILLCELLINFO_DEFAULT_CDMA_CINFO_BSLON;
            mCellInfo->u.cdmaCinfo.local_timezone_offset_from_utc =
                RILLCELLINFO_DEFAULT_CDMA_CINFO_LOCAL_TIME_ZONE_OFFSET_FROM_UTC;
            mCellInfo->u.cdmaCinfo.local_timezone_on_daylight_savings =
                RILLCELLINFO_DEFAULT_CDMA_CINFO_LOCAL_TIME_ZONE_ON_DAYLIGHT_SAVINGS;
        }
    }
}


WifiSupplicantStatusDataItem :: WifiSupplicantStatusDataItem()
:
DataItemBase
 (
WIFI_SUPPLICANT_STATUS_DATA_ITEM_ID
),
mState((WifiSupplicantState)WIFI_SUPPLICANT_DEFAULT_STATE),
mApMacAddressValid(false),
mWifiApSsidValid(false)
{
    memset (&mApMacAddress, 0, sizeof (mApMacAddress));
    mWifiApSsid.clear();
}

TacDataItem :: TacDataItem
 (const std :: string & name)
:
DataItemBase
 (
TAC_DATA_ITEM_ID
),
mValue (name)
{}

MccmncDataItem :: MccmncDataItem
 (const std :: string & name)
:
DataItemBase
 (
MCCMNC_DATA_ITEM_ID
),
mValue (name)
{}

// Dtors
AirplaneModeDataItem :: ~AirplaneModeDataItem () {}
ENHDataItem :: ~ENHDataItem () {}
GPSStateDataItem :: ~GPSStateDataItem () {}
NLPStatusDataItem :: ~NLPStatusDataItem () {}
WifiHardwareStateDataItem :: ~WifiHardwareStateDataItem () {}
ScreenStateDataItem :: ~ScreenStateDataItem () {}
PowerConnectStateDataItem :: ~PowerConnectStateDataItem () {}
TimeZoneChangeDataItem :: ~TimeZoneChangeDataItem () {}
TimeChangeDataItem :: ~TimeChangeDataItem () {}
ShutdownStateDataItem :: ~ShutdownStateDataItem () {}
AssistedGpsDataItem :: ~AssistedGpsDataItem () {}
NetworkInfoDataItem :: ~NetworkInfoDataItem () {}
ServiceStatusDataItem :: ~ServiceStatusDataItem () {}
ModelDataItem :: ~ModelDataItem () {}
ManufacturerDataItem :: ~ManufacturerDataItem () {}
RilServiceInfoDataItem :: ~RilServiceInfoDataItem () {
    if (  (mDestroy) &&  (mServiceInfo) ) { delete mServiceInfo; }
}
RilCellInfoDataItem :: ~RilCellInfoDataItem () {
    if (  (mDestroy) &&  (mCellInfo) ) { delete mCellInfo; }
}
WifiSupplicantStatusDataItem :: ~WifiSupplicantStatusDataItem () {}
TacDataItem :: ~TacDataItem () {}
MccmncDataItem :: ~MccmncDataItem () {}

// getId
DataItemId AirplaneModeDataItem :: getId () { return DataItemBase :: getId (); }
DataItemId ENHDataItem :: getId () { return DataItemBase :: getId (); }
DataItemId GPSStateDataItem :: getId () { return DataItemBase :: getId (); }
DataItemId NLPStatusDataItem :: getId () { return DataItemBase :: getId (); }
DataItemId WifiHardwareStateDataItem :: getId () { return DataItemBase :: getId (); }
DataItemId ScreenStateDataItem :: getId () { return DataItemBase :: getId (); }
DataItemId PowerConnectStateDataItem :: getId () { return DataItemBase :: getId (); }
DataItemId TimeZoneChangeDataItem :: getId () { return DataItemBase :: getId (); }
DataItemId TimeChangeDataItem :: getId () { return DataItemBase :: getId (); }
DataItemId ShutdownStateDataItem :: getId () { return DataItemBase :: getId (); }
DataItemId AssistedGpsDataItem :: getId () { return DataItemBase :: getId (); }
DataItemId NetworkInfoDataItem :: getId () { return DataItemBase :: getId (); }
DataItemId ServiceStatusDataItem :: getId () { return DataItemBase :: getId (); }
DataItemId ModelDataItem :: getId () { return DataItemBase :: getId (); }
DataItemId ManufacturerDataItem :: getId () { return DataItemBase :: getId (); }
DataItemId RilServiceInfoDataItem :: getId () { return DataItemBase :: getId (); }
DataItemId RilCellInfoDataItem :: getId () { return DataItemBase :: getId (); }
DataItemId WifiSupplicantStatusDataItem :: getId () {return DataItemBase :: getId ();}
DataItemId TacDataItem :: getId () { return DataItemBase :: getId (); }
DataItemId MccmncDataItem :: getId () { return DataItemBase :: getId (); }

// getSerializer
IDataItemSerializer * AirplaneModeDataItem :: getSerializer () { if (mSerializer == NULL) mSerializer = new  (std :: nothrow) AirplaneModeDataItemSerializer (this); return mSerializer; }
IDataItemSerializer * ENHDataItem :: getSerializer () { if (mSerializer == NULL) mSerializer = new  (std :: nothrow) ENHDataItemSerializer (this); return mSerializer; }
IDataItemSerializer * GPSStateDataItem :: getSerializer () { if (mSerializer == NULL) mSerializer = new  (std :: nothrow) GPSStateDataItemSerializer (this); return mSerializer; }
IDataItemSerializer * NLPStatusDataItem :: getSerializer () { if (mSerializer == NULL) mSerializer = new  (std :: nothrow) NLPStatusDataItemSerializer (this); return mSerializer; }
IDataItemSerializer * WifiHardwareStateDataItem :: getSerializer () { if (mSerializer == NULL) mSerializer = new  (std :: nothrow) WifiHardwareStateDataItemSerializer (this); return mSerializer; }
IDataItemSerializer * ScreenStateDataItem :: getSerializer () { if (mSerializer == NULL) mSerializer = new  (std :: nothrow) ScreenStateDataItemSerializer (this); return mSerializer; }
IDataItemSerializer * PowerConnectStateDataItem :: getSerializer () { if (mSerializer == NULL) mSerializer = new  (std :: nothrow) PowerConnectStateDataItemSerializer (this); return mSerializer; }
IDataItemSerializer * TimeZoneChangeDataItem :: getSerializer () { if (mSerializer == NULL) mSerializer = new  (std :: nothrow) TimeZoneChangeDataItemSerializer (this); return mSerializer; }
IDataItemSerializer * TimeChangeDataItem :: getSerializer () { if (mSerializer == NULL) mSerializer = new  (std :: nothrow) TimeChangeDataItemSerializer (this); return mSerializer; }
IDataItemSerializer * ShutdownStateDataItem :: getSerializer () { if (mSerializer == NULL) mSerializer = new  (std :: nothrow) ShutdownStateDataItemSerializer (this); return mSerializer; }
IDataItemSerializer * AssistedGpsDataItem :: getSerializer () { if (mSerializer == NULL) mSerializer = new  (std :: nothrow) AssistedGpsDataItemSerializer (this); return mSerializer; }
IDataItemSerializer * NetworkInfoDataItem :: getSerializer () { if (mSerializer == NULL) mSerializer = new  (std :: nothrow) NetworkInfoDataItemSerializer (this); return mSerializer; }
IDataItemSerializer * ServiceStatusDataItem :: getSerializer () { if (mSerializer == NULL) mSerializer = new  (std :: nothrow) ServiceStatusDataItemSerializer (this); return mSerializer; }
IDataItemSerializer * ModelDataItem :: getSerializer () { if (mSerializer == NULL) mSerializer = new  (std :: nothrow) ModelDataItemSerializer (this); return mSerializer; }
IDataItemSerializer * ManufacturerDataItem :: getSerializer () { if (mSerializer == NULL) mSerializer = new  (std :: nothrow) ManufacturerDataItemSerializer (this); return mSerializer; }
IDataItemSerializer * RilServiceInfoDataItem :: getSerializer () { if (mSerializer == NULL) mSerializer = new  (std :: nothrow) RilServiceInfoDataItemSerializer (this); return mSerializer; }
IDataItemSerializer * RilCellInfoDataItem :: getSerializer () { if (mSerializer == NULL) mSerializer = new  (std :: nothrow) RilCellInfoDataItemSerializer (this); return mSerializer; }
IDataItemSerializer * WifiSupplicantStatusDataItem :: getSerializer () { if (mSerializer == NULL) mSerializer = new  (std :: nothrow) WifiSupplicantStatusDataItemSerializer (this); return mSerializer; }
IDataItemSerializer * TacDataItem :: getSerializer () { if (mSerializer == NULL) mSerializer = new  (std :: nothrow) TacDataItemSerializer (this); return mSerializer; }
IDataItemSerializer * MccmncDataItem :: getSerializer () { if (mSerializer == NULL) mSerializer = new  (std :: nothrow) MccmncDataItemSerializer (this); return mSerializer; }

// getDeSerializer
IDataItemDeSerializer * AirplaneModeDataItem :: getDeSerializer () { if (mDeSerializer == NULL) mDeSerializer = new  (std :: nothrow) AirplaneModeDataItemDeSerializer (this); return mDeSerializer; }
IDataItemDeSerializer * ENHDataItem :: getDeSerializer () { if (mDeSerializer == NULL) mDeSerializer = new  (std :: nothrow) ENHDataItemDeSerializer (this); return mDeSerializer; }
IDataItemDeSerializer * GPSStateDataItem :: getDeSerializer () { if (mDeSerializer == NULL) mDeSerializer = new  (std :: nothrow) GPSStateDataItemDeSerializer (this); return mDeSerializer; }
IDataItemDeSerializer * NLPStatusDataItem :: getDeSerializer () { if (mDeSerializer == NULL) mDeSerializer = new  (std :: nothrow) NLPStatusDataItemDeSerializer (this); return mDeSerializer; }
IDataItemDeSerializer * WifiHardwareStateDataItem :: getDeSerializer () { if (mDeSerializer == NULL) mDeSerializer = new  (std :: nothrow) WifiHardwareStateDataItemDeSerializer (this); return mDeSerializer; }
IDataItemDeSerializer * ScreenStateDataItem :: getDeSerializer () { if (mDeSerializer == NULL) mDeSerializer = new  (std :: nothrow) ScreenStateDataItemDeSerializer (this); return mDeSerializer; }
IDataItemDeSerializer * PowerConnectStateDataItem :: getDeSerializer () { if (mDeSerializer == NULL) mDeSerializer = new  (std :: nothrow) PowerConnectStateDataItemDeSerializer (this); return mDeSerializer; }
IDataItemDeSerializer * TimeZoneChangeDataItem :: getDeSerializer () { if (mDeSerializer == NULL) mDeSerializer = new  (std :: nothrow) TimeZoneChangeDataItemDeSerializer (this); return mDeSerializer; }
IDataItemDeSerializer * TimeChangeDataItem :: getDeSerializer () { if (mDeSerializer == NULL) mDeSerializer = new  (std :: nothrow) TimeChangeDataItemDeSerializer (this); return mDeSerializer; }
IDataItemDeSerializer * ShutdownStateDataItem :: getDeSerializer () { if (mDeSerializer == NULL) mDeSerializer = new  (std :: nothrow) ShutdownStateDataItemDeSerializer (this); return mDeSerializer; }
IDataItemDeSerializer * AssistedGpsDataItem :: getDeSerializer () { if (mDeSerializer == NULL) mDeSerializer = new  (std :: nothrow) AssistedGpsDataItemDeSerializer (this); return mDeSerializer; }
IDataItemDeSerializer * NetworkInfoDataItem :: getDeSerializer () { if (mDeSerializer == NULL) mDeSerializer = new  (std :: nothrow) NetworkInfoDataItemDeSerializer (this); return mDeSerializer; }
IDataItemDeSerializer * ServiceStatusDataItem :: getDeSerializer () { if (mDeSerializer == NULL) mDeSerializer = new  (std :: nothrow) ServiceStatusDataItemDeSerializer (this); return mDeSerializer; }
IDataItemDeSerializer * ModelDataItem :: getDeSerializer () { if (mDeSerializer == NULL) mDeSerializer = new  (std :: nothrow) ModelDataItemDeSerializer (this); return mDeSerializer; }
IDataItemDeSerializer * ManufacturerDataItem :: getDeSerializer () { if (mDeSerializer == NULL) mDeSerializer = new  (std :: nothrow) ManufacturerDataItemDeSerializer (this); return mDeSerializer; }
IDataItemDeSerializer * RilServiceInfoDataItem :: getDeSerializer () { if (mDeSerializer == NULL) mDeSerializer = new  (std :: nothrow) RilServiceInfoDataItemDeSerializer (this); return mDeSerializer; }
IDataItemDeSerializer * RilCellInfoDataItem :: getDeSerializer () { if (mDeSerializer == NULL) mDeSerializer = new  (std :: nothrow) RilCellInfoDataItemDeSerializer (this); return mDeSerializer; }
IDataItemDeSerializer * WifiSupplicantStatusDataItem :: getDeSerializer () { if (mDeSerializer == NULL) mDeSerializer = new  (std :: nothrow) WifiSupplicantStatusDataItemDeSerializer (this); return mDeSerializer; }
IDataItemDeSerializer * TacDataItem :: getDeSerializer () { if (mDeSerializer == NULL) mDeSerializer = new  (std :: nothrow) TacDataItemDeSerializer (this); return mDeSerializer; }
IDataItemDeSerializer * MccmncDataItem :: getDeSerializer () { if (mDeSerializer == NULL) mDeSerializer = new  (std :: nothrow) MccmncDataItemDeSerializer (this); return mDeSerializer; }

// getCopier
IDataItemCopier * AirplaneModeDataItem :: getCopier () { if (mCopier == NULL) mCopier = new  (std :: nothrow) AirplaneModeDataItemCopier (this); return mCopier; }
IDataItemCopier * ENHDataItem :: getCopier () { if (mCopier == NULL) mCopier = new  (std :: nothrow) ENHDataItemCopier (this); return mCopier; }
IDataItemCopier * GPSStateDataItem :: getCopier () { if (mCopier == NULL) mCopier = new  (std :: nothrow) GPSStateDataItemCopier (this); return mCopier; }
IDataItemCopier * NLPStatusDataItem :: getCopier () { if (mCopier == NULL) mCopier = new  (std :: nothrow) NLPStatusDataItemCopier (this); return mCopier; }
IDataItemCopier * WifiHardwareStateDataItem :: getCopier () { if (mCopier == NULL) mCopier = new  (std :: nothrow) WifiHardwareStateDataItemCopier (this); return mCopier; }
IDataItemCopier * ScreenStateDataItem :: getCopier () { if (mCopier == NULL) mCopier = new  (std :: nothrow) ScreenStateDataItemCopier (this); return mCopier; }
IDataItemCopier * PowerConnectStateDataItem :: getCopier () { if (mCopier == NULL) mCopier = new  (std :: nothrow) PowerConnectStateDataItemCopier (this); return mCopier; }
IDataItemCopier * TimeZoneChangeDataItem :: getCopier () { if (mCopier == NULL) mCopier = new  (std :: nothrow) TimeZoneChangeDataItemCopier (this); return mCopier; }
IDataItemCopier * TimeChangeDataItem :: getCopier () { if (mCopier == NULL) mCopier = new  (std :: nothrow) TimeChangeDataItemCopier (this); return mCopier; }
IDataItemCopier * ShutdownStateDataItem :: getCopier () { if (mCopier == NULL) mCopier = new  (std :: nothrow) ShutdownStateDataItemCopier (this); return mCopier; }
IDataItemCopier * AssistedGpsDataItem :: getCopier () { if (mCopier == NULL) mCopier = new  (std :: nothrow) AssistedGpsDataItemCopier (this); return mCopier; }
IDataItemCopier * NetworkInfoDataItem :: getCopier () { if (mCopier == NULL) mCopier = new  (std :: nothrow) NetworkInfoDataItemCopier (this); return mCopier; }
IDataItemCopier * ServiceStatusDataItem :: getCopier () { if (mCopier == NULL) mCopier = new  (std :: nothrow) ServiceStatusDataItemCopier (this); return mCopier; }
IDataItemCopier * ModelDataItem :: getCopier () { if (mCopier == NULL) mCopier = new  (std :: nothrow) ModelDataItemCopier (this); return mCopier; }
IDataItemCopier * ManufacturerDataItem :: getCopier () { if (mCopier == NULL) mCopier = new  (std :: nothrow) ManufacturerDataItemCopier (this); return mCopier; }
IDataItemCopier * RilServiceInfoDataItem :: getCopier () { if (mCopier == NULL) mCopier = new  (std :: nothrow) RilServiceInfoDataItemCopier (this); return mCopier; }
IDataItemCopier * RilCellInfoDataItem :: getCopier () { if (mCopier == NULL) mCopier = new  (std :: nothrow) RilCellInfoDataItemCopier (this); return mCopier; }
IDataItemCopier * WifiSupplicantStatusDataItem :: getCopier () { if (mCopier == NULL) mCopier = new  (std :: nothrow) WifiSupplicantStatusDataItemCopier (this); return mCopier; }
IDataItemCopier * TacDataItem :: getCopier () { if (mCopier == NULL) mCopier = new  (std :: nothrow) TacDataItemCopier (this); return mCopier; }
IDataItemCopier * MccmncDataItem :: getCopier () { if (mCopier == NULL) mCopier = new  (std :: nothrow) MccmncDataItemCopier (this); return mCopier; }


// getStringifier
IDataItemStringifier * AirplaneModeDataItem :: getStringifier () { if (mStringifier == NULL) mStringifier = new  (std :: nothrow) AirplaneModeDataItemStringifier (this); return mStringifier; }
IDataItemStringifier * ENHDataItem :: getStringifier () { if (mStringifier == NULL) mStringifier = new  (std :: nothrow) ENHDataItemStringifier (this); return mStringifier; }
IDataItemStringifier * GPSStateDataItem :: getStringifier () { if (mStringifier == NULL) mStringifier = new  (std :: nothrow) GPSStateDataItemStringifier (this); return mStringifier; }
IDataItemStringifier * NLPStatusDataItem :: getStringifier () { if (mStringifier == NULL) mStringifier = new  (std :: nothrow) NLPStatusDataItemStringifier (this); return mStringifier; }
IDataItemStringifier * WifiHardwareStateDataItem :: getStringifier () { if (mStringifier == NULL) mStringifier = new  (std :: nothrow) WifiHardwareStateDataItemStringifier (this); return mStringifier; }
IDataItemStringifier * ScreenStateDataItem :: getStringifier () { if (mStringifier == NULL) mStringifier = new  (std :: nothrow) ScreenStateDataItemStringifier (this); return mStringifier; }
IDataItemStringifier * PowerConnectStateDataItem :: getStringifier () { if (mStringifier == NULL) mStringifier = new  (std :: nothrow) PowerConnectStateDataItemStringifier (this); return mStringifier; }
IDataItemStringifier * TimeZoneChangeDataItem :: getStringifier () { if (mStringifier == NULL) mStringifier = new  (std :: nothrow) TimeZoneChangeDataItemStringifier (this); return mStringifier; }
IDataItemStringifier * TimeChangeDataItem :: getStringifier () { if (mStringifier == NULL) mStringifier = new  (std :: nothrow) TimeChangeDataItemStringifier (this); return mStringifier; }
IDataItemStringifier * ShutdownStateDataItem :: getStringifier () { if (mStringifier == NULL) mStringifier = new  (std :: nothrow) ShutdownStateDataItemStringifier (this); return mStringifier; }
IDataItemStringifier * AssistedGpsDataItem :: getStringifier () { if (mStringifier == NULL) mStringifier = new  (std :: nothrow) AssistedGpsDataItemStringifier (this); return mStringifier; }
IDataItemStringifier * NetworkInfoDataItem :: getStringifier () { if (mStringifier == NULL) mStringifier = new  (std :: nothrow) NetworkInfoDataItemStringifier (this); return mStringifier; }
IDataItemStringifier * ServiceStatusDataItem :: getStringifier () { if (mStringifier == NULL) mStringifier = new  (std :: nothrow) ServiceStatusDataItemStringifier (this); return mStringifier; }
IDataItemStringifier * ModelDataItem :: getStringifier () { if (mStringifier == NULL) mStringifier = new  (std :: nothrow) ModelDataItemStringifier (this); return mStringifier; }
IDataItemStringifier * ManufacturerDataItem :: getStringifier () { if (mStringifier == NULL) mStringifier = new  (std :: nothrow) ManufacturerDataItemStringifier (this); return mStringifier; }
IDataItemStringifier * RilServiceInfoDataItem :: getStringifier () { if (mStringifier == NULL) mStringifier = new  (std :: nothrow) RilServiceInfoDataItemStringifier (this); return mStringifier; }
IDataItemStringifier * RilCellInfoDataItem :: getStringifier () { if (mStringifier == NULL) mStringifier = new  (std :: nothrow) RilCellInfoDataItemStringifier (this); return mStringifier; }
IDataItemStringifier * WifiSupplicantStatusDataItem :: getStringifier () { if (mStringifier == NULL) mStringifier = new  (std :: nothrow) WifiSupplicantStatusDataItemStringifier (this); return mStringifier; }
IDataItemStringifier * TacDataItem :: getStringifier () { if (mStringifier == NULL) mStringifier = new  (std :: nothrow) TacDataItemStringifier (this); return mStringifier; }
IDataItemStringifier * MccmncDataItem :: getStringifier () { if (mStringifier == NULL) mStringifier = new  (std :: nothrow) MccmncDataItemStringifier (this); return mStringifier; }
