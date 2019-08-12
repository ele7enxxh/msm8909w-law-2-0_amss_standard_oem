/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemStringifierConcreteTypes Implementation

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#include <string>
#include <stdio.h>
#include <comdef.h>
#include <DataItemId.h>
#include <IDataItem.h>
#include <DataItemConcreteTypes.h>
#include <DataItemStringifierBase.h>
#include <DataItemStringifierConcreteTypes.h>
#include <DataItemConcreteTypeFieldNames.h>
#include <log_util.h>
#include <loc_ril.h>
#include <WiperData.h>

using namespace izat_manager;
using namespace std;

#define DATA_ITEM_STRINGIFIER_CTOR(C) \
C::C(IDataItem * d) \
:DataItemStringifierBase(d) \
{}

#define DATA_ITEM_STRINGIFIER_DTOR(C) \
C::~C() {}

#define DATA_ITEM_STRINGIFY(T) \
void T :: stringify (string & valueStr)

#define ERROR_CHECK_AND_DOWN_CAST(T,ID) \
if(mD == NULL) { result = 1; break; } \
if(mD->getId() != ID) { result = 2; break; } \
T * d = reinterpret_cast<T *>(mD);


// Ctors
DATA_ITEM_STRINGIFIER_CTOR(AirplaneModeDataItemStringifier)
DATA_ITEM_STRINGIFIER_CTOR(ENHDataItemStringifier)
DATA_ITEM_STRINGIFIER_CTOR(GPSStateDataItemStringifier)
DATA_ITEM_STRINGIFIER_CTOR(NLPStatusDataItemStringifier)
DATA_ITEM_STRINGIFIER_CTOR(WifiHardwareStateDataItemStringifier)
DATA_ITEM_STRINGIFIER_CTOR(ScreenStateDataItemStringifier)
DATA_ITEM_STRINGIFIER_CTOR(PowerConnectStateDataItemStringifier)
DATA_ITEM_STRINGIFIER_CTOR(TimeZoneChangeDataItemStringifier)
DATA_ITEM_STRINGIFIER_CTOR(TimeChangeDataItemStringifier)
DATA_ITEM_STRINGIFIER_CTOR(ShutdownStateDataItemStringifier)
DATA_ITEM_STRINGIFIER_CTOR(AssistedGpsDataItemStringifier)
DATA_ITEM_STRINGIFIER_CTOR(NetworkInfoDataItemStringifier)
DATA_ITEM_STRINGIFIER_CTOR(ServiceStatusDataItemStringifier)
DATA_ITEM_STRINGIFIER_CTOR(ModelDataItemStringifier)
DATA_ITEM_STRINGIFIER_CTOR(ManufacturerDataItemStringifier)
DATA_ITEM_STRINGIFIER_CTOR(RilServiceInfoDataItemStringifier)
DATA_ITEM_STRINGIFIER_CTOR(RilCellInfoDataItemStringifier)
DATA_ITEM_STRINGIFIER_CTOR(WifiSupplicantStatusDataItemStringifier)
DATA_ITEM_STRINGIFIER_CTOR(TacDataItemStringifier)
DATA_ITEM_STRINGIFIER_CTOR(MccmncDataItemStringifier)

// Dtors
DATA_ITEM_STRINGIFIER_DTOR(AirplaneModeDataItemStringifier)
DATA_ITEM_STRINGIFIER_DTOR(ENHDataItemStringifier)
DATA_ITEM_STRINGIFIER_DTOR(GPSStateDataItemStringifier)
DATA_ITEM_STRINGIFIER_DTOR(NLPStatusDataItemStringifier)
DATA_ITEM_STRINGIFIER_DTOR(WifiHardwareStateDataItemStringifier)
DATA_ITEM_STRINGIFIER_DTOR(ScreenStateDataItemStringifier)
DATA_ITEM_STRINGIFIER_DTOR(PowerConnectStateDataItemStringifier)
DATA_ITEM_STRINGIFIER_DTOR(TimeZoneChangeDataItemStringifier)
DATA_ITEM_STRINGIFIER_DTOR(TimeChangeDataItemStringifier)
DATA_ITEM_STRINGIFIER_DTOR(ShutdownStateDataItemStringifier)
DATA_ITEM_STRINGIFIER_DTOR(AssistedGpsDataItemStringifier)
DATA_ITEM_STRINGIFIER_DTOR(NetworkInfoDataItemStringifier)
DATA_ITEM_STRINGIFIER_DTOR(ServiceStatusDataItemStringifier)
DATA_ITEM_STRINGIFIER_DTOR(ModelDataItemStringifier)
DATA_ITEM_STRINGIFIER_DTOR(ManufacturerDataItemStringifier)
DATA_ITEM_STRINGIFIER_DTOR(RilServiceInfoDataItemStringifier)
DATA_ITEM_STRINGIFIER_DTOR(RilCellInfoDataItemStringifier)
DATA_ITEM_STRINGIFIER_DTOR(WifiSupplicantStatusDataItemStringifier)
DATA_ITEM_STRINGIFIER_DTOR(TacDataItemStringifier)
DATA_ITEM_STRINGIFIER_DTOR(MccmncDataItemStringifier)


// stringify
DATA_ITEM_STRINGIFY(AirplaneModeDataItemStringifier) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(AirplaneModeDataItem, AIRPLANEMODE_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr = AIRPLANEMODE_FIELD_MODE;
        valueStr += ": ";
        valueStr += (d->mMode) ? ("true") : ("false");
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
}

DATA_ITEM_STRINGIFY(ENHDataItemStringifier) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(ENHDataItem, ENH_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr = ENH_FIELD_ENABLED;
        valueStr += ": ";
        valueStr += (d->mEnabled) ? ("true") : ("false");
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(GPSStateDataItemStringifier) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(GPSStateDataItem, GPSSTATE_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr = GPSSTATE_FIELD_ENABLED;
        valueStr += ": ";
        valueStr += (d->mEnabled) ? ("true") : ("false");
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(NLPStatusDataItemStringifier) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(NLPStatusDataItem,NLPSTATUS_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr = NLPSTATUS_FIELD_ENABLED;
        valueStr += ": ";
        valueStr += (d->mEnabled) ? ("true") : ("false");
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(WifiHardwareStateDataItemStringifier) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(WifiHardwareStateDataItem,WIFIHARDWARESTATE_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr = WIFIHARDWARESTATE_FIELD_ENABLED;
        valueStr += ": ";
        valueStr += (d->mEnabled) ? ("true") : ("false");
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(ScreenStateDataItemStringifier) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(ScreenStateDataItem,SCREEN_STATE_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr = SCREENSTATE_FIELD_ENABLED;
        valueStr += ": ";
        valueStr += (d->mState) ? ("true") : ("false");
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(PowerConnectStateDataItemStringifier) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(PowerConnectStateDataItem,POWER_CONNECTED_STATE_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr = POWERCONNECTSTATE_FIELD_ENABLED;
        valueStr += ": ";
        valueStr += (d->mState) ? ("true") : ("false");
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(TimeZoneChangeDataItemStringifier) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(TimeZoneChangeDataItem,TIMEZONE_CHANGE_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr = TIMEZONECHANGE_FIELD_ENABLED;
        valueStr += ": ";
        valueStr += "true";
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(TimeChangeDataItemStringifier) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(TimeChangeDataItem,TIME_CHANGE_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr = TIMECHANGE_FIELD_ENABLED;
        valueStr += ": ";
        valueStr += "true";
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(ShutdownStateDataItemStringifier) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(ShutdownStateDataItem,SHUTDOWN_STATE_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr = SHUTDOWN_FIELD_ENABLED;
        valueStr += ": ";
        valueStr += (d->mState) ? ("true") : ("false");
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(AssistedGpsDataItemStringifier) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(AssistedGpsDataItem,ASSISTED_GPS_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr = ASSISTEDGPS_FIELD_ENABLED;
        valueStr += ": ";
        valueStr += (d->mEnabled) ? ("true") : ("false");
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(NetworkInfoDataItemStringifier) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(NetworkInfoDataItem,NETWORKINFO_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr = NETWORKINFO_CARD;
        valueStr += "::";
        valueStr += NETWORKINFO_FIELD_TYPE;
        valueStr += ": ";
        char type [12];
        snprintf (type, 12, "%d", d->mType);
        valueStr += string (type);
        valueStr += ", ";
        valueStr += NETWORKINFO_FIELD_TYPENAME;
        valueStr += ": ";
        valueStr += d->mTypeName;
        valueStr += ", ";
        valueStr += NETWORKINFO_FIELD_SUBTYPENAME;
        valueStr += ": ";
        valueStr += d->mSubTypeName;
        valueStr += ", ";
        valueStr += NETWORKINFO_FIELD_AVAILABLE;
        valueStr += ": ";
        valueStr += (d->mAvailable) ? ("true") : ("false");
        valueStr += ", ";
        valueStr += NETWORKINFO_FIELD_CONNECTED;
        valueStr += ": ";
        valueStr += (d->mConnected) ? ("true") : ("false");
        valueStr += ", ";
        valueStr += NETWORKINFO_FIELD_ROAMING;
        valueStr += ": ";
        valueStr += (d->mRoaming) ? ("true") : ("false");
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(ServiceStatusDataItemStringifier) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(ServiceStatusDataItem,SERVICESTATUS_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr += SERVICESTATUS_FIELD_STATE;
        valueStr += ": ";
        char state [12];
        snprintf (state, 12, "%d", d->mServiceState);
        valueStr += string (state);
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(ModelDataItemStringifier) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(ModelDataItem,MODEL_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr += MODEL_FIELD_NAME;
        valueStr += ": ";
        valueStr += d->mModel;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(ManufacturerDataItemStringifier) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(ManufacturerDataItem,MANUFACTURER_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr += MANUFACTURER_FIELD_NAME;
        valueStr += ": ";
        valueStr += d->mManufacturer;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(RilServiceInfoDataItemStringifier) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(RilServiceInfoDataItem,RILSERVICEINFO_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr += RILSERVICEINFO_CARD;
        if (d->mServiceInfo->valid_mask) {
            valueStr += ":";

            valueStr += RILSERVICEINFO_FIELD_ARIF_TYPE_MASK;
            valueStr += ": ";
            char t[10];
            memset (t, '\0', 10);
            snprintf (t, 10, "%d", d->mServiceInfo->airIf_type);
            valueStr += t;


            valueStr += ", ";
            valueStr += RILSERVICEINFO_FIELD_CARRIER_ARIF_TYPE;
            valueStr += "(CDMA-1, GSM -2, WCDMA-4, LTA-8, EVDO-16, WIFI-32):";
            memset (t, '\0', 10);
            snprintf (t, 10, "%d", d->mServiceInfo->carrierAirIf_type);
            valueStr += t;

            valueStr += ", ";
            valueStr += RILSERVICEINFO_FIELD_CARRIER_MCC;
            valueStr += ": ";
            memset (t, '\0', 10);
            snprintf (t, 10, "%d", d->mServiceInfo->carrierMcc);
            valueStr += t;

            valueStr += ", ";
            valueStr += RILSERVICEINFO_FIELD_CARRIER_MNC;
            valueStr += ": ";
            memset (t, '\0', 10);
            snprintf (t, 10, "%d", d->mServiceInfo->carrierMcc);
            valueStr += t;

            valueStr += ", ";
            valueStr += RILSERVICEINFO_FIELD_CARRIER_NAME;
            valueStr += ": ";
            valueStr += d->mServiceInfo->carrierName;
        } else {
            valueStr += "Invalid";
        }
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(RilCellInfoDataItemStringifier) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(RilCellInfoDataItem,RILCELLINFO_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr += RILCELLINFO_CARD;
        if(d->mCellInfo->valid_mask) {
            if((d->mCellInfo->valid_mask & LOC_RIL_CELLINFO_HAS_NW_STATUS) ==
                LOC_RIL_CELLINFO_HAS_NW_STATUS) {
                valueStr += ": ";
                valueStr += RILCELLINFO_FIELD_NETWORK_STATUS;
                valueStr += ": ";
                if (d->mCellInfo->nwStatus == LOC_NW_ROAMING) {
                    valueStr += "ROAMING";
                } else if (d->mCellInfo->nwStatus == LOC_NW_HOME) {
                    valueStr += "ROAMING";
                } else {
                    valueStr += "OOO";
                }
            }
            if((d->mCellInfo->valid_mask & LOC_RIL_CELLINFO_HAS_CELL_INFO) ==
                LOC_RIL_CELLINFO_HAS_CELL_INFO) {
                if((d->mCellInfo->valid_mask & LOC_RIL_CELLINFO_HAS_TECH_TYPE) ==
                    LOC_RIL_CELLINFO_HAS_TECH_TYPE) {

                        valueStr += ", ";
                        valueStr += RILCELLINFO_FIELD_RIL_TECH_TYPE;
                        valueStr += ": ";

                    switch(d->mCellInfo->rtType) {
                        case LOC_RIL_TECH_CDMA:
                        valueStr += "CDMA";
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_MCC) == LOC_RIL_TECH_CDMA_HAS_MCC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_MCC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.cdmaCinfo.mcc);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_SID) == LOC_RIL_TECH_CDMA_HAS_SID) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_SID;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.cdmaCinfo.sid);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_NID) == LOC_RIL_TECH_CDMA_HAS_NID) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_NID;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.cdmaCinfo.nid);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_BSID) == LOC_RIL_TECH_CDMA_HAS_BSID) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_BSID;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.cdmaCinfo.bsid);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_BSLAT) == LOC_RIL_TECH_CDMA_HAS_BSLAT) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_BSLAT;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.cdmaCinfo.bslat);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_BSLONG) == LOC_RIL_TECH_CDMA_HAS_BSLONG) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_BSLON;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.cdmaCinfo.bslon);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_TIMEZONE) == LOC_RIL_TECH_CDMA_HAS_TIMEZONE) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_UTC_TIME_OFFSET;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.cdmaCinfo.local_timezone_offset_from_utc);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_CDMA_HAS_DAYLIGHT_SAVING) == LOC_RIL_TECH_CDMA_HAS_DAYLIGHT_SAVING) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_DAYLIGHT_TIMEZONE;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.cdmaCinfo.local_timezone_on_daylight_savings);
                            valueStr += t;
                        }
                        break;
                        case LOC_RIL_TECH_GSM:
                        valueStr += "GSM";
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_MCC) == LOC_RIL_TECH_GW_HAS_MCC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_MCC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.gsmCinfo.mcc);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_MNC) == LOC_RIL_TECH_GW_HAS_MNC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_MNC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.gsmCinfo.mnc);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_LAC) == LOC_RIL_TECH_GW_HAS_LAC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_LAC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.gsmCinfo.lac);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_CID) == LOC_RIL_TECH_GW_HAS_CID) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_CID;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.gsmCinfo.cid);
                            valueStr += t;
                        }
                        break;
                        case LOC_RIL_TECH_WCDMA:
                        valueStr += "WCDMA";
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_MCC) == LOC_RIL_TECH_GW_HAS_MCC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_MCC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.wcdmaCinfo.mcc);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_MNC) == LOC_RIL_TECH_GW_HAS_MNC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_MNC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.wcdmaCinfo.mnc);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_LAC) == LOC_RIL_TECH_GW_HAS_LAC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_LAC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.wcdmaCinfo.lac);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_GW_HAS_CID) == LOC_RIL_TECH_GW_HAS_CID) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_CID;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.wcdmaCinfo.cid);
                            valueStr += t;
                        }
                        break;
                        case LOC_RIL_TECH_LTE:
                        valueStr += "LTE";
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_LTE_HAS_MCC) == LOC_RIL_TECH_LTE_HAS_MCC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_MCC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.lteCinfo.mcc);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_LTE_HAS_MNC) == LOC_RIL_TECH_LTE_HAS_MNC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_MNC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.lteCinfo.mnc);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_LTE_HAS_TAC) == LOC_RIL_TECH_LTE_HAS_TAC) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_TAC;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.lteCinfo.tac);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_LTE_HAS_PCI) == LOC_RIL_TECH_LTE_HAS_PCI) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_PCI;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.lteCinfo.pci);
                            valueStr += t;
                        }
                        if((d->mCellInfo->valid_mask & LOC_RIL_TECH_LTE_HAS_CID) == LOC_RIL_TECH_LTE_HAS_CID) {
                            valueStr += ", ";
                            valueStr += RILLCELLINFO_FIELD_CID;
                            valueStr += ": ";
                            char t[20];
                            memset (t, '\0', 20);
                            snprintf (t, 20, "%d", d->mCellInfo->u.lteCinfo.cid);
                            valueStr += t;
                        }
                        break;
                        case LOC_RIL_TECH_TD_SCDMA:
                        case LOC_RIL_TECH_MAX:
                        default:
                        break;
                    }; // end switch
                }
            }
        }
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}

DATA_ITEM_STRINGIFY(WifiSupplicantStatusDataItemStringifier) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(WifiSupplicantStatusDataItem, WIFI_SUPPLICANT_STATUS_DATA_ITEM_ID);
        valueStr += "Attach state: ";
        char t[50];
        memset (t, '\0', 50);
        snprintf (t, 50, "%d", d->mState);
        valueStr += t;

        valueStr += ", Mac address valid: ";
        valueStr += (d->mApMacAddressValid) ? ("true") : ("false");

        valueStr += ", AP MAC address: ";
        memset (t, '\0', 50);
        snprintf
        (
            t,
            50,
            "[%02x:%02x:%02x:%02x:%02x:%02x]",
            d->mApMacAddress[0],
            d->mApMacAddress[1],
            d->mApMacAddress[2],
            d->mApMacAddress[3],
            d->mApMacAddress[4],
            d->mApMacAddress[5]
        );
        valueStr += t;

        valueStr += ", Wifi-Ap SSID Valid: ";
        valueStr += (d->mWifiApSsidValid) ? ("true") : ("false");

        valueStr += ", SSID: ";
        valueStr += d->mWifiApSsid;

    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
}
DATA_ITEM_STRINGIFY(TacDataItemStringifier) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(TacDataItem,TAC_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr += TAC_FIELD_NAME;
        valueStr += ": ";
        valueStr += d->mValue;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
DATA_ITEM_STRINGIFY(MccmncDataItemStringifier) {
    int32 result = 0;
    ENTRY_LOG();
    do {
        ERROR_CHECK_AND_DOWN_CAST(MccmncDataItem,MCCMNC_DATA_ITEM_ID);
        valueStr.clear ();
        valueStr += MCCMNC_FIELD_NAME;
        valueStr += ": ";
        valueStr += d->mValue;
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);

}
