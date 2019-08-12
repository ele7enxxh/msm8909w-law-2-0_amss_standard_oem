/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemCopierConcreteTypes Implementation

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#include <string>
#include <comdef.h>
#include <DataItemId.h>
#include <IDataItem.h>
#include <DataItemConcreteTypes.h>
#include <DataItemCopierBase.h>
#include <DataItemCopierConcreteTypes.h>
#include <log_util.h>
#include <loc_ril.h>

using namespace izat_manager;
using namespace std;

#define DATA_ITEM_COPIER_CTOR(C) \
C::C(IDataItem * d) \
:DataItemCopierBase(d) \
{}

#define DATA_ITEM_COPIER_DTOR(C) \
C::~C() {}

#define DATA_ITEM_COPY(T) \
int32 T::copy (IDataItem * src, bool * dataItemCopied)

#define ERROR_CHECK_AND_DOWN_CAST(T,ID) \
   if( (mD == NULL) || (src == NULL) ) { result = 1; break; } \
   if(mD->getId() != src->getId()) { result = 2; break; } \
   if(mD->getId() != ID) { result = 3; break; } \
   T * s = reinterpret_cast<T *>(mD); \
   T * d = reinterpret_cast<T *>(src);

#define SET_COPIED(B) \
    if (NULL != B) { *B = true;}

// Ctors
DATA_ITEM_COPIER_CTOR(AirplaneModeDataItemCopier)
DATA_ITEM_COPIER_CTOR(ENHDataItemCopier)
DATA_ITEM_COPIER_CTOR(GPSStateDataItemCopier)
DATA_ITEM_COPIER_CTOR(NLPStatusDataItemCopier)
DATA_ITEM_COPIER_CTOR(WifiHardwareStateDataItemCopier)
DATA_ITEM_COPIER_CTOR(ScreenStateDataItemCopier)
DATA_ITEM_COPIER_CTOR(PowerConnectStateDataItemCopier)
DATA_ITEM_COPIER_CTOR(TimeZoneChangeDataItemCopier)
DATA_ITEM_COPIER_CTOR(TimeChangeDataItemCopier)
DATA_ITEM_COPIER_CTOR(ShutdownStateDataItemCopier)
DATA_ITEM_COPIER_CTOR(AssistedGpsDataItemCopier)
DATA_ITEM_COPIER_CTOR(NetworkInfoDataItemCopier)
DATA_ITEM_COPIER_CTOR(ServiceStatusDataItemCopier)
DATA_ITEM_COPIER_CTOR(ModelDataItemCopier)
DATA_ITEM_COPIER_CTOR(ManufacturerDataItemCopier)
DATA_ITEM_COPIER_CTOR(RilServiceInfoDataItemCopier)
DATA_ITEM_COPIER_CTOR(RilCellInfoDataItemCopier)
DATA_ITEM_COPIER_CTOR(WifiSupplicantStatusDataItemCopier)
DATA_ITEM_COPIER_CTOR(TacDataItemCopier)
DATA_ITEM_COPIER_CTOR(MccmncDataItemCopier)

// Dtors
DATA_ITEM_COPIER_DTOR(AirplaneModeDataItemCopier)
DATA_ITEM_COPIER_DTOR(ENHDataItemCopier)
DATA_ITEM_COPIER_DTOR(GPSStateDataItemCopier)
DATA_ITEM_COPIER_DTOR(NLPStatusDataItemCopier)
DATA_ITEM_COPIER_DTOR(WifiHardwareStateDataItemCopier)
DATA_ITEM_COPIER_DTOR(ScreenStateDataItemCopier)
DATA_ITEM_COPIER_DTOR(PowerConnectStateDataItemCopier)
DATA_ITEM_COPIER_DTOR(TimeZoneChangeDataItemCopier)
DATA_ITEM_COPIER_DTOR(TimeChangeDataItemCopier)
DATA_ITEM_COPIER_DTOR(ShutdownStateDataItemCopier)
DATA_ITEM_COPIER_DTOR(AssistedGpsDataItemCopier)
DATA_ITEM_COPIER_DTOR(NetworkInfoDataItemCopier)
DATA_ITEM_COPIER_DTOR(ServiceStatusDataItemCopier)
DATA_ITEM_COPIER_DTOR(ModelDataItemCopier)
DATA_ITEM_COPIER_DTOR(ManufacturerDataItemCopier)
DATA_ITEM_COPIER_DTOR(RilServiceInfoDataItemCopier)
DATA_ITEM_COPIER_DTOR(RilCellInfoDataItemCopier)
DATA_ITEM_COPIER_DTOR(WifiSupplicantStatusDataItemCopier)
DATA_ITEM_COPIER_DTOR(TacDataItemCopier)
DATA_ITEM_COPIER_DTOR(MccmncDataItemCopier)


// copy
DATA_ITEM_COPY(AirplaneModeDataItemCopier) {
   int32 result = -1;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(AirplaneModeDataItem, AIRPLANEMODE_DATA_ITEM_ID);
        if(s->mMode == d->mMode) { result = 0; break; }
         s->mMode = d->mMode;
         SET_COPIED(dataItemCopied);
         result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;;
}
DATA_ITEM_COPY(ENHDataItemCopier) {
    int32 result = -1;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(ENHDataItem, ENH_DATA_ITEM_ID);
        if(s->mEnabled == d->mEnabled) { result =0; break; }
        s->mEnabled = d->mEnabled;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;;
}
DATA_ITEM_COPY(GPSStateDataItemCopier) {
    int32 result = -1;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(GPSStateDataItem, GPSSTATE_DATA_ITEM_ID);
        if(s->mEnabled == d->mEnabled) { result = 0; break; }
        s->mEnabled = d->mEnabled;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;;
 }
DATA_ITEM_COPY(NLPStatusDataItemCopier) {
    int32 result = -1;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(NLPStatusDataItem,NLPSTATUS_DATA_ITEM_ID);
        if(s->mEnabled == d->mEnabled) { result = 0; break; }
         s->mEnabled = d->mEnabled;
         SET_COPIED(dataItemCopied);
         result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;;
}
DATA_ITEM_COPY(WifiHardwareStateDataItemCopier) {
    int32 result = -1;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(WifiHardwareStateDataItem,WIFIHARDWARESTATE_DATA_ITEM_ID);
        if(s->mEnabled == d->mEnabled) { result = 0; break; }
        s->mEnabled = d->mEnabled;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;;
}
DATA_ITEM_COPY(ScreenStateDataItemCopier) {
    int32 result = -1;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(ScreenStateDataItem,SCREEN_STATE_DATA_ITEM_ID);
        if(s->mState == d->mState) { result = 0; break; }
        s->mState = d->mState;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;;
}
DATA_ITEM_COPY(PowerConnectStateDataItemCopier) {
    int32 result = -1;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(PowerConnectStateDataItem,POWER_CONNECTED_STATE_DATA_ITEM_ID);
        if(s->mState == d->mState) { result = 0; break; }
        s->mState = d->mState;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;;
}
DATA_ITEM_COPY(TimeZoneChangeDataItemCopier) {
    int32 result = -1;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(TimeZoneChangeDataItem,TIMEZONE_CHANGE_DATA_ITEM_ID);
        if(s->mCurrTimeMillis == d->mCurrTimeMillis &&
           s->mRawOffsetTZ == d->mRawOffsetTZ &&
           s->mDstOffsetTZ == d->mDstOffsetTZ) {
            result = 0;
            break;
        }
        s->mCurrTimeMillis = d->mCurrTimeMillis;
        s->mRawOffsetTZ = d->mRawOffsetTZ;
        s->mDstOffsetTZ = d->mDstOffsetTZ;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;;
}
DATA_ITEM_COPY(TimeChangeDataItemCopier) {
    int32 result = -1;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(TimeChangeDataItem,TIME_CHANGE_DATA_ITEM_ID);
        if(s->mCurrTimeMillis == d->mCurrTimeMillis &&
           s->mRawOffsetTZ == d->mRawOffsetTZ &&
           s->mDstOffsetTZ == d->mDstOffsetTZ) {
            result = 0;
            break;
        }
        s->mCurrTimeMillis = d->mCurrTimeMillis;
        s->mRawOffsetTZ = d->mRawOffsetTZ;
        s->mDstOffsetTZ = d->mDstOffsetTZ;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;;
}
DATA_ITEM_COPY(ShutdownStateDataItemCopier) {
    int32 result = -1;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(ShutdownStateDataItem,SHUTDOWN_STATE_DATA_ITEM_ID);
        if(s->mState == d->mState) { result = 0; break; }
        s->mState = d->mState;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;;
}
DATA_ITEM_COPY(AssistedGpsDataItemCopier) {
    int32 result = -1;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(AssistedGpsDataItem,ASSISTED_GPS_DATA_ITEM_ID);
        if(s->mEnabled == d->mEnabled) { result = 0; break; }
        s->mEnabled = d->mEnabled;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;;
}
DATA_ITEM_COPY(NetworkInfoDataItemCopier) {
    int32 result = -1;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(NetworkInfoDataItem,NETWORKINFO_DATA_ITEM_ID);
        if( (s->mType == d->mType) && (0 == s->mTypeName.compare(d->mTypeName)) &&
            (0 == s->mSubTypeName.compare(d->mSubTypeName)) &&
            (s->mAvailable == d->mAvailable) &&
            (s->mConnected == d->mConnected) &&
            (s->mRoaming == d->mRoaming) ) {
            result = 0;
            break;
        }

        if(s->mType != d->mType) { s->mType = d->mType;}
        if(0 != s->mTypeName.compare(d->mTypeName)) { s->mTypeName = d->mTypeName;}
        if(0 != s->mSubTypeName.compare(d->mSubTypeName)) {s->mSubTypeName = d->mSubTypeName;}
        if(s->mAvailable != d->mAvailable) {s->mAvailable = d->mAvailable;}
        if(s->mConnected != d->mConnected) {s->mConnected = d->mConnected;}
        if(s->mRoaming != d->mRoaming) {s->mRoaming = d->mRoaming;}

        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;;
}
DATA_ITEM_COPY(ServiceStatusDataItemCopier) {
    int32 result = -1;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(ServiceStatusDataItem,SERVICESTATUS_DATA_ITEM_ID);
        if(s->mServiceState == d->mServiceState) { result = 0; break; }
        s->mServiceState = d->mServiceState;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG("%d",result);
    return result;
}
DATA_ITEM_COPY(ModelDataItemCopier) {
    int32 result = -1;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(ModelDataItem,MODEL_DATA_ITEM_ID);
        if(0 == s->mModel.compare(d->mModel)) { result = 0; break; }
        s->mModel = d->mModel;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_COPY(ManufacturerDataItemCopier) {
    int32 result = -1;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(ManufacturerDataItem,MANUFACTURER_DATA_ITEM_ID);
        if(0 == s->mManufacturer.compare(d->mManufacturer)) { result = 0; break; }
        s->mManufacturer = d->mManufacturer;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while (0);
    EXIT_LOG("%d",result);
    return result;
}
DATA_ITEM_COPY(RilServiceInfoDataItemCopier) {
    int32 result = -1;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(RilServiceInfoDataItem,RILSERVICEINFO_DATA_ITEM_ID);
        string sCName(s->mServiceInfo->carrierName);
        string dCName(d->mServiceInfo->carrierName);
        if( (s->mServiceInfo->valid_mask == d->mServiceInfo->valid_mask) &&
            (s->mServiceInfo->airIf_type == d->mServiceInfo->airIf_type) &&
            (s->mServiceInfo->carrierAirIf_type == d->mServiceInfo->carrierAirIf_type) &&
            (s->mServiceInfo->carrierMcc == d->mServiceInfo->carrierMcc) &&
            (s->mServiceInfo->carrierMnc == d->mServiceInfo->carrierMnc) &&
            (s->mServiceInfo->carrierNameLen == d->mServiceInfo->carrierNameLen) &&
            (0 == sCName.compare(dCName)) ) {
            result =0;
            break;
        }
        s->mServiceInfo->valid_mask = d->mServiceInfo->valid_mask;
        s->mServiceInfo->airIf_type = d->mServiceInfo->airIf_type;
        s->mServiceInfo->carrierAirIf_type = d->mServiceInfo->carrierAirIf_type;
        s->mServiceInfo->carrierMcc = d->mServiceInfo->carrierMcc;
        s->mServiceInfo->carrierMnc = d->mServiceInfo->carrierMnc;
        s->mServiceInfo->carrierNameLen = d->mServiceInfo->carrierNameLen;
        strlcpy(s->mServiceInfo->carrierName, d->mServiceInfo->carrierName, LOC_RIL_CARRIER_NAME_MAX_LEN);
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}
DATA_ITEM_COPY(RilCellInfoDataItemCopier) {
    int32 result = -1;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(RilCellInfoDataItem,RILCELLINFO_DATA_ITEM_ID);
        if( (s->mCellInfo->valid_mask == d->mCellInfo->valid_mask) &&
            (s->mCellInfo->nwStatus == d->mCellInfo->nwStatus) &&
            (s->mCellInfo->rtType == d->mCellInfo->rtType) ) {
            if(s->mCellInfo->rtType == LOC_RIL_TECH_CDMA) {
                if( (s->mCellInfo->u.cdmaCinfo.mcc == d->mCellInfo->u.cdmaCinfo.mcc) &&
                    (s->mCellInfo->u.cdmaCinfo.sid == d->mCellInfo->u.cdmaCinfo.sid) &&
                    (s->mCellInfo->u.cdmaCinfo.nid == d->mCellInfo->u.cdmaCinfo.nid) &&
                    (s->mCellInfo->u.cdmaCinfo.bsid == d->mCellInfo->u.cdmaCinfo.bsid) &&
                    (s->mCellInfo->u.cdmaCinfo.bslat == d->mCellInfo->u.cdmaCinfo.bslat) &&
                    (s->mCellInfo->u.cdmaCinfo.bslon == d->mCellInfo->u.cdmaCinfo.bslon) &&
                    (s->mCellInfo->u.cdmaCinfo.local_timezone_offset_from_utc == d->mCellInfo->u.cdmaCinfo.local_timezone_offset_from_utc) &&
                    (s->mCellInfo->u.cdmaCinfo.local_timezone_on_daylight_savings == d->mCellInfo->u.cdmaCinfo.local_timezone_on_daylight_savings) )  {
                    result = 0;
                    break;
                }
            }
            else if (s->mCellInfo->rtType == LOC_RIL_TECH_GSM) {
                if( (s->mCellInfo->u.gsmCinfo.mcc == d->mCellInfo->u.gsmCinfo.mcc) &&
                    (s->mCellInfo->u.gsmCinfo.mnc == d->mCellInfo->u.gsmCinfo.mnc) &&
                    (s->mCellInfo->u.gsmCinfo.lac == d->mCellInfo->u.gsmCinfo.lac) &&
                    (s->mCellInfo->u.gsmCinfo.cid == d->mCellInfo->u.gsmCinfo.cid) ) {
                    result = 0;
                    break;
                }
            }
            else if (s->mCellInfo->rtType == LOC_RIL_TECH_WCDMA) {
                if( (s->mCellInfo->u.wcdmaCinfo.mcc == d->mCellInfo->u.wcdmaCinfo.mcc) &&
                    (s->mCellInfo->u.wcdmaCinfo.mnc == d->mCellInfo->u.wcdmaCinfo.mnc) &&
                    (s->mCellInfo->u.wcdmaCinfo.lac == d->mCellInfo->u.wcdmaCinfo.lac) &&
                    (s->mCellInfo->u.wcdmaCinfo.cid == d->mCellInfo->u.wcdmaCinfo.cid) ) {
                    result = 0;
                    break;
                }
            }
            else if (s->mCellInfo->rtType == LOC_RIL_TECH_LTE) {
                if( (s->mCellInfo->u.lteCinfo.mcc == d->mCellInfo->u.lteCinfo.mcc) &&
                    (s->mCellInfo->u.lteCinfo.mnc == d->mCellInfo->u.lteCinfo.mnc) &&
                    (s->mCellInfo->u.lteCinfo.tac == d->mCellInfo->u.lteCinfo.tac) &&
                    (s->mCellInfo->u.lteCinfo.pci == d->mCellInfo->u.lteCinfo.pci) &&
                    (s->mCellInfo->u.lteCinfo.cid == d->mCellInfo->u.lteCinfo.cid) ) {
                    result = 0;
                    break;
                }
            }
            else if (s->mCellInfo->rtType == LOC_RIL_TECH_TD_SCDMA) {
                result = 0;
                break;
            }
            else if (s->mCellInfo->rtType == LOC_RIL_TECH_MAX) {
                result = 0;
                break;
            }
        }
        memcpy(static_cast<void*>(s->mCellInfo), static_cast<void *>(d->mCellInfo), sizeof(LOC_RilCellInfo));
        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

DATA_ITEM_COPY(WifiSupplicantStatusDataItemCopier) {
    int32 result = -1;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(WifiSupplicantStatusDataItem, WIFI_SUPPLICANT_STATUS_DATA_ITEM_ID);
        if( (s->mState == d->mState) &&
            (s->mApMacAddressValid == d->mApMacAddressValid) &&
            (s->mWifiApSsidValid == d->mWifiApSsidValid)) {

            // compare mac address
            if (memcmp(s->mApMacAddress, d->mApMacAddress, sizeof(s->mApMacAddress)) == 0) {

                // compare ssid
                if (s->mWifiApSsid.compare(d->mWifiApSsid) == 0) {
                    result = 0;
                    break;
                }
            }
        }

        if (s->mState != d->mState) { s->mState = d->mState;}
        if (s->mApMacAddressValid != d->mApMacAddressValid) {s->mApMacAddressValid = d->mApMacAddressValid;}
        if (s->mWifiApSsidValid != d->mWifiApSsidValid) {s->mWifiApSsidValid = d->mWifiApSsidValid;}
        if (memcmp(s->mApMacAddress, d->mApMacAddress, sizeof(s->mApMacAddress)) != 0) {
            memcpy(static_cast<void*>(s->mApMacAddress), static_cast<void *>(d->mApMacAddress), sizeof(s->mApMacAddress));
        }
        if (s->mWifiApSsid.compare(d->mWifiApSsid) != 0) {
            s->mWifiApSsid = d->mWifiApSsid;
        }

        SET_COPIED(dataItemCopied);
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
    return result;
}

DATA_ITEM_COPY(TacDataItemCopier) {
    int32 result = -1;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(TacDataItem,TAC_DATA_ITEM_ID);
        if(0 == s->mValue.compare(d->mValue)) { result = 0; break; }
        s->mValue = d->mValue;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while (0);
    EXIT_LOG("%d",result);
    return result;
}

DATA_ITEM_COPY(MccmncDataItemCopier) {
    int32 result = -1;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(MccmncDataItem,MCCMNC_DATA_ITEM_ID);
        if(0 == s->mValue.compare(d->mValue)) { result = 0; break; }
        s->mValue= d->mValue;
        SET_COPIED(dataItemCopied);
        result = 0;
    } while (0);
    EXIT_LOG("%d",result);
    return result;
}

