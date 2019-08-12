/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemsFactory Implementation

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/


#include <IDataItem.h>
#include <DataItemsFactory.h>
#include <DataItemConcreteTypes.h>

using namespace izat_manager;

IDataItem *
DataItemsFactory :: createNewDataItem (DataItemId id) {

    IDataItem * dataItem;
    switch (id) {
    case AIRPLANEMODE_DATA_ITEM_ID:
        dataItem = new AirplaneModeDataItem ();
        break;
    case ENH_DATA_ITEM_ID:
        dataItem = new ENHDataItem ();
        break;
    case GPSSTATE_DATA_ITEM_ID:
        dataItem = new GPSStateDataItem ();
        break;
    case NLPSTATUS_DATA_ITEM_ID:
        dataItem = new NLPStatusDataItem ();
        break;
    case WIFIHARDWARESTATE_DATA_ITEM_ID:
        dataItem = new WifiHardwareStateDataItem ();
        break;
    case NETWORKINFO_DATA_ITEM_ID:
        dataItem = new NetworkInfoDataItem ();
        break;
    case SERVICESTATUS_DATA_ITEM_ID:
        dataItem = new ServiceStatusDataItem ();
        break;
    case RILCELLINFO_DATA_ITEM_ID:
        dataItem = new RilCellInfoDataItem ();
        break;
    case RILSERVICEINFO_DATA_ITEM_ID:
        dataItem = new RilServiceInfoDataItem ();
        break;
    case MODEL_DATA_ITEM_ID:
        dataItem = new ModelDataItem ();
    break;
    case MANUFACTURER_DATA_ITEM_ID:
        dataItem = new ManufacturerDataItem ();
    break;
    case ASSISTED_GPS_DATA_ITEM_ID:
        dataItem = new AssistedGpsDataItem ();
    break;
    case SCREEN_STATE_DATA_ITEM_ID:
        dataItem = new ScreenStateDataItem ();
    break;
    case POWER_CONNECTED_STATE_DATA_ITEM_ID:
        dataItem = new PowerConnectStateDataItem ();
    break;
    case TIMEZONE_CHANGE_DATA_ITEM_ID:
        dataItem = new TimeZoneChangeDataItem ();
    break;
    case TIME_CHANGE_DATA_ITEM_ID:
        dataItem = new TimeChangeDataItem ();
    break;
    case WIFI_SUPPLICANT_STATUS_DATA_ITEM_ID:
        dataItem = new WifiSupplicantStatusDataItem ();
    break;
    case SHUTDOWN_STATE_DATA_ITEM_ID:
        dataItem = new ShutdownStateDataItem ();
    break;
    case TAC_DATA_ITEM_ID:
        dataItem = new TacDataItem ();
    break;
    case MCCMNC_DATA_ITEM_ID:
        dataItem = new MccmncDataItem ();
    break;
    case INVALID_DATA_ITEM_ID:
    case MAX_DATA_ITEM_ID:
    default:
        dataItem = NULL;
        break;
    };
    return dataItem;
}

