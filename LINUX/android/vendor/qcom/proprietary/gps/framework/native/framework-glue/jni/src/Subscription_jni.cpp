/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright  (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#define LOG_TAG "Subscription_jni"
#define LOG_NDEBUG 0

#include "jni.h"
#include "JNIHelp.h"
#include "AndroidRuntime.h"
#include "utils/Log.h"
#include "Subscription_jni.h"
#include "utils_jni.h"
#include "Subscription.h"
#include "IzatDefines.h"
#include "AndroidRuntime.h"
#include "DataItemConcreteTypes.h"
#include "loc_ril.h"
#include "WiperData.h"

static jobject mOsAgentObj = NULL;
static jmethodID method_subscribe;
static jmethodID method_unsubscribe;
static jmethodID method_unsubscribeAll;
static jmethodID method_requestData;

using namespace android;

static void classInit(JNIEnv* env, jclass clazz)
{
    ENTRY_LOG();

    method_subscribe = env->GetMethodID(clazz, "subscribe", "([I)V");
    method_requestData = env->GetMethodID(clazz, "requestData", "([I)V");
    method_unsubscribe = env->GetMethodID(clazz, "unsubscribe", "([I)V");
    method_unsubscribeAll = env->GetMethodID(clazz, "unsubscribeAll", "()V");
}

static void instanceInit(JNIEnv *env, jobject obj)
{
    ENTRY_LOG();

    // cache the Java callback object
    if (!mOsAgentObj)
        mOsAgentObj = env->NewGlobalRef(obj);

    // create the subscription object.
    (void)Subscription::getSubscriptionObj();
}

static void instanceDeinit(JNIEnv *env, jobject obj)
{
    ENTRY_LOG();

    // clear Global References if any
    env->DeleteGlobalRef(mOsAgentObj);

    mOsAgentObj = NULL;

    // delete the subscription object.
    Subscription::destroyInstance();
}

void update_subscribeJNI(const std::list<DataItemId> & l, bool subscribe)
{
    ENTRY_LOG();

    int result = 0;

    jintArray dataItemsArray = NULL;

    do {
        BREAK_IF_ZERO(1, isValidCallbackThread());

        JNIEnv* env = getCallbackEnv();
        BREAK_IF_ZERO(2, env);

        int listSize = l.size();
        jintArray dataItemsArray = (jintArray)env->NewIntArray(listSize);
        BREAK_IF_ZERO(4, dataItemsArray);

        std::list<DataItemId>::const_iterator it = l.begin();
        for (int index = 0; it != l.end(); it++, index++) {
            jint dataItem = *it;
            env->SetIntArrayRegion(dataItemsArray, index, 1, &dataItem);
        }

        if (subscribe) {
            env->CallVoidMethod(mOsAgentObj, method_subscribe, dataItemsArray);
        } else {
            env->CallVoidMethod(mOsAgentObj, method_unsubscribe, dataItemsArray);
        }

        if (dataItemsArray) env->DeleteLocalRef(dataItemsArray);

        checkAndClearExceptionsFromCallback(env, __FUNCTION__);

        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void requestDataJNI(const std::list<DataItemId> & l)
{
    ENTRY_LOG();

    int result = 0;

    jintArray dataItemsArray = NULL;

    do {
        BREAK_IF_ZERO(1, isValidCallbackThread());

        JNIEnv* env = getCallbackEnv();
        BREAK_IF_ZERO(2, env);

        int listSize = l.size();
        jintArray dataItemsArray = (jintArray)env->NewIntArray(listSize);
        BREAK_IF_ZERO(4, dataItemsArray);

        std::list<DataItemId>::const_iterator it = l.begin();
        for (int index = 0; it != l.end(); it++, index++) {
            jint dataItem = *it;
            env->SetIntArrayRegion(dataItemsArray, index, 1, &dataItem);
        }

        env->CallVoidMethod(mOsAgentObj, method_requestData, dataItemsArray);

        if (dataItemsArray) env->DeleteLocalRef(dataItemsArray);

        checkAndClearExceptionsFromCallback(env, __FUNCTION__);

        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

void unsubscribeAllJNI()
{
    ENTRY_LOG();

    int result = 0;

    do {
        BREAK_IF_ZERO(1, isValidCallbackThread());

        JNIEnv* env = getCallbackEnv();
        BREAK_IF_ZERO(2, env);

        env->CallVoidMethod(mOsAgentObj, method_unsubscribeAll);
        checkAndClearExceptionsFromCallback(env, __FUNCTION__);

        result = 0;
    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

static void bool_dataitem_update(JNIEnv* env, jobject obj,
    jintArray dataItemIdArray, jbooleanArray updatedValueArray)
{
    int result = 0;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, Subscription::mObserverObj);

        jint * dataitem_arr = env->GetIntArrayElements(dataItemIdArray, 0);
        jboolean * updatedvalue_arr = env->GetBooleanArrayElements(updatedValueArray, 0);
        int array_len = env->GetArrayLength(dataItemIdArray);

        ENHDataItem enh_data_item;
        AirplaneModeDataItem airplaneMode_data_item;
        GPSStateDataItem gpsState_data_item;
        NLPStatusDataItem nlpStatus_data_item;
        WifiHardwareStateDataItem wifistate_data_item;
        AssistedGpsDataItem assistedgps_data_item;

        std::list<IDataItem *> dataItem_list;

        for (int idx = 0; idx < array_len; idx++) {
            switch (dataitem_arr[idx])
            {
                case ENH_DATA_ITEM_ID:
                    enh_data_item.mEnabled = updatedvalue_arr[idx];
                    dataItem_list.push_back(&enh_data_item);
                break;
                case AIRPLANEMODE_DATA_ITEM_ID:
                    airplaneMode_data_item.mMode = updatedvalue_arr[idx];
                    dataItem_list.push_back(&airplaneMode_data_item);
                break;
                case GPSSTATE_DATA_ITEM_ID:
                    gpsState_data_item.mEnabled = updatedvalue_arr[idx];
                    dataItem_list.push_back(&gpsState_data_item);
                break;
                case NLPSTATUS_DATA_ITEM_ID:
                    nlpStatus_data_item.mEnabled = updatedvalue_arr[idx];
                    dataItem_list.push_back(&nlpStatus_data_item);
                break;
                case WIFIHARDWARESTATE_DATA_ITEM_ID:
                    wifistate_data_item.mEnabled = updatedvalue_arr[idx];
                    dataItem_list.push_back(&wifistate_data_item);
                break;
                case ASSISTED_GPS_DATA_ITEM_ID:
                    assistedgps_data_item.mEnabled = updatedvalue_arr[idx];
                    dataItem_list.push_back(&assistedgps_data_item);
                break;
            }
        }

        if (dataItem_list.size() != 0) {
            Subscription::mObserverObj->notify(dataItem_list);
        }

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

static void string_dataitem_update(JNIEnv* env, jobject obj,
    jint dataItemId, jstring updatedValue)
{
    int result = 0;
    ENTRY_LOG();

    do {

        BREAK_IF_ZERO(1, Subscription::mObserverObj);

        IDataItem *dataItem = NULL;

        ModelDataItem modeldesc_data_item;
        ManufacturerDataItem manufacturerdesc_data_item;
        TacDataItem tacdesc_data_item;
        MccmncDataItem mccmncdesc_data_item;

        const char *dataItem_string_val = env->GetStringUTFChars(updatedValue, NULL);
        BREAK_IF_ZERO(2, dataItem_string_val);

        switch (dataItemId)
        {
            case MODEL_DATA_ITEM_ID:
                modeldesc_data_item.mModel.assign(dataItem_string_val);
                dataItem = &modeldesc_data_item;
            break;
            case MANUFACTURER_DATA_ITEM_ID:
                manufacturerdesc_data_item.mManufacturer.assign(dataItem_string_val);
                dataItem = &manufacturerdesc_data_item;
            break;
            case TAC_DATA_ITEM_ID:
                tacdesc_data_item.mValue.assign(dataItem_string_val);
                dataItem = &tacdesc_data_item;
            break;
            case MCCMNC_DATA_ITEM_ID:
                mccmncdesc_data_item.mValue.assign(dataItem_string_val);
                dataItem = &mccmncdesc_data_item;
            break;
        }

        if (dataItem != NULL) {
             std::list<IDataItem *> dataItem_list;
             dataItem_list.push_back(dataItem);
             Subscription::mObserverObj->notify(dataItem_list);
        }

        env->ReleaseStringUTFChars(updatedValue, dataItem_string_val);
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

static void networkinfo_update(JNIEnv* env, jobject obj,
    jboolean is_connected, jint type, jstring type_name, jstring subtype_name, jboolean is_available, jboolean is_roaming)
{
    int result = 0;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, Subscription::mObserverObj);

        NetworkInfoDataItem networkinfo_data_item;
        IDataItem *dataItem = NULL;

        networkinfo_data_item.mType = type;
        networkinfo_data_item.mAvailable = is_available;
        networkinfo_data_item.mConnected = is_connected;
        networkinfo_data_item.mRoaming = is_roaming;

        const char *type_name_string = env->GetStringUTFChars(type_name, NULL);
        if (type_name_string != NULL) {
            networkinfo_data_item.mTypeName.assign(type_name_string);
            env->ReleaseStringUTFChars(type_name, type_name_string);
        }

        const char *subtype_name_string = env->GetStringUTFChars(subtype_name, NULL);
        if (subtype_name_string != NULL) {
            networkinfo_data_item.mSubTypeName.assign(subtype_name_string);
            env->ReleaseStringUTFChars(subtype_name, subtype_name_string);
        }

        dataItem = &networkinfo_data_item;

        std::list<IDataItem *> dataItem_list;
        dataItem_list.push_back(dataItem);
        Subscription::mObserverObj->notify(dataItem_list);

    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

static void serviceinfo_update(JNIEnv* env, jobject obj,
    jint air_interface_type, jstring carrierName, jint Mcc, jint Mnc)
{
    int result = 0;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, Subscription::mObserverObj);


        LOC_RilServiceInfo serviceInfo;
        memset(&serviceInfo, 0, sizeof(LOC_RilServiceInfo));

        const char *carrier_name_string = NULL;

        switch (air_interface_type)
        {
            case LOC_RILAIRIF_CDMA:
            serviceInfo.valid_mask = LOC_RIL_SERVICE_INFO_HAS_CARRIER_AIR_IF_TYPE;
            serviceInfo.carrierAirIf_type = LOC_RILAIRIF_CDMA;

             carrier_name_string = env->GetStringUTFChars(carrierName, NULL);
             if (carrier_name_string != NULL) {
                serviceInfo.valid_mask |= LOC_RIL_SERVICE_INFO_HAS_CARRIER_NAME;

                if (strlcpy(serviceInfo.carrierName, carrier_name_string, sizeof(serviceInfo.carrierName)) >=
                    sizeof(serviceInfo.carrierName)) {
                        LOC_LOGD("Carrier name is longer than array size to hold it.");
                }
            }
            break;

            case LOC_RILAIRIF_GSM:
                serviceInfo.valid_mask = LOC_RIL_SERVICE_INFO_HAS_CARRIER_AIR_IF_TYPE;
                serviceInfo.carrierAirIf_type = LOC_RILAIRIF_GSM;

                // MCC cannot be zero
                if (Mcc != 0) {
                    serviceInfo.valid_mask |= LOC_RIL_SERVICE_INFO_HAS_CARRIER_MCC;
                    serviceInfo.carrierMcc = Mcc;
                }

                // Mnc can be 0
                serviceInfo.valid_mask |= LOC_RIL_SERVICE_INFO_HAS_CARRIER_MNC;
                serviceInfo.carrierMnc = Mnc;
            break;
        }

        RilServiceInfoDataItem serviceinfo_data_item(&serviceInfo);
        std::list<IDataItem *> dataItem_list;
        dataItem_list.push_back(&serviceinfo_data_item);
        Subscription::mObserverObj->notify(dataItem_list);

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

static void cell_update_gw(JNIEnv* env, jobject obj, jint air_interface_type, jint nwStatus, jint iMcc,
    jint iMnc, jint lac, jint cid, jint valid_mask)
{
    int result = 0;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, Subscription::mObserverObj);

        LOC_RilCellInfo cellInfo;
        memset(&cellInfo, 0, sizeof(LOC_RilCellInfo));

        cellInfo.valid_mask = valid_mask;
        cellInfo.nwStatus = (LOC_NWstatus)nwStatus;
        cellInfo.rtType = (LOC_RilTechType)air_interface_type;

        switch (air_interface_type)
        {
            case LOC_RILAIRIF_GSM:
                cellInfo.u.gsmCinfo.mcc = iMcc;
                cellInfo.u.gsmCinfo.mnc = iMnc;
                cellInfo.u.gsmCinfo.lac = lac;
                cellInfo.u.gsmCinfo.cid = cid;
            break;
            case LOC_RILAIRIF_WCDMA:
                cellInfo.u.wcdmaCinfo.mcc = iMcc;
                cellInfo.u.wcdmaCinfo.mnc = iMnc;
                cellInfo.u.wcdmaCinfo.lac = lac;
                cellInfo.u.wcdmaCinfo.cid = cid;
            break;
            default: result = 2;
        }

        if (result == 0) {
            cellInfo.valid_mask |= LOC_RIL_CELLINFO_HAS_CELL_INFO;

            RilCellInfoDataItem cellinfo_data_item(&cellInfo);
            std::list<IDataItem *> dataItem_list;
            dataItem_list.push_back(&cellinfo_data_item);

            Subscription::mObserverObj->notify(dataItem_list);
        }
        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

static void cell_update_lte(JNIEnv* env, jobject obj, jint air_interface_type, jint nwStatus,
    jint iMcc, jint iMnc, jint cid, jint pci, jint tac, jint valid_mask)
{
    int result = 0;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, Subscription::mObserverObj);


        LOC_RilCellInfo cellInfo;
        memset(&cellInfo, 0, sizeof(LOC_RilCellInfo));

        cellInfo.valid_mask = valid_mask;
        cellInfo.nwStatus = (LOC_NWstatus)nwStatus;
        cellInfo.rtType = (LOC_RilTechType)air_interface_type;

        cellInfo.u.lteCinfo.mcc = iMcc;
        cellInfo.u.lteCinfo.mnc = iMnc;
        cellInfo.u.lteCinfo.pci = pci;
        cellInfo.u.lteCinfo.tac = tac;
        cellInfo.u.lteCinfo.cid = cid;

        cellInfo.valid_mask |= LOC_RIL_CELLINFO_HAS_CELL_INFO;

        RilCellInfoDataItem cellinfo_data_item(&cellInfo);
        std::list<IDataItem *> dataItem_list;
        dataItem_list.push_back(&cellinfo_data_item);
        Subscription::mObserverObj->notify(dataItem_list);

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

static void cell_update_cdma(JNIEnv* env, jobject obj, jint air_interface_type, jint nwStatus,
    jint sid, jint nid, jint bsid, jint bslat, jint bslong, jboolean inDST, jint UTCTimeOffset, jint valid_mask)
{
    int result = 0;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, Subscription::mObserverObj);


        LOC_RilCellInfo cellInfo;
        memset(&cellInfo, 0, sizeof(LOC_RilCellInfo));

        cellInfo.valid_mask = valid_mask;
        cellInfo.nwStatus = (LOC_NWstatus)nwStatus;
        cellInfo.rtType = (LOC_RilTechType)air_interface_type;

        cellInfo.u.cdmaCinfo.sid = sid;
        cellInfo.u.cdmaCinfo.nid = nid;
        cellInfo.u.cdmaCinfo.bsid = bsid;
        cellInfo.u.cdmaCinfo.bslat = bslat;
        cellInfo.u.cdmaCinfo.bslon = bslong;
        cellInfo.u.cdmaCinfo.local_timezone_offset_from_utc = UTCTimeOffset;
        cellInfo.u.cdmaCinfo.local_timezone_on_daylight_savings = inDST;

        cellInfo.valid_mask |= LOC_RIL_CELLINFO_HAS_CELL_INFO;

        RilCellInfoDataItem cellinfo_data_item(&cellInfo);
        std::list<IDataItem *> dataItem_list;
        dataItem_list.push_back(&cellinfo_data_item);
        Subscription::mObserverObj->notify(dataItem_list);

        result = 0;
    } while(0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

static void cell_update_ooo(JNIEnv* env, jobject obj, jint nwStatus, jint valid_mask)
{
    int result = 0;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, Subscription::mObserverObj);

        LOC_RilCellInfo cellInfo;
        memset(&cellInfo, 0, sizeof(LOC_RilCellInfo));

        cellInfo.valid_mask = valid_mask;
        cellInfo.nwStatus = (LOC_NWstatus)nwStatus;

        RilCellInfoDataItem cellinfo_data_item(&cellInfo);

        std::list<IDataItem *> dataItem_list;
        dataItem_list.push_back(&cellinfo_data_item);

        Subscription::mObserverObj->notify(dataItem_list);

        result = 0;
    } while(0);
}

static void service_state_update(JNIEnv* env, jobject obj, jint nwStatus)
{
    int result = 0;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, Subscription::mObserverObj);

        ServiceStatusDataItem service_status_data_item;
        service_status_data_item.mServiceState = (LOC_NWstatus)nwStatus;

        IDataItem *dataItem = &service_status_data_item;

        std::list<IDataItem *> dataItem_list;
        dataItem_list.push_back(dataItem);

        Subscription::mObserverObj->notify(dataItem_list);

        result = 0;
    } while(0);
}

static void screen_status_update(JNIEnv* env, jobject obj, jboolean status)
{
    int result = 0;
    ENTRY_LOG();

    do {
        BREAK_IF_ZERO(1, Subscription::mObserverObj);

        ScreenStateDataItem screenstate_data_item;
        screenstate_data_item.mState = status;

        IDataItem *dataItem = &screenstate_data_item;

        std::list<IDataItem *> dataItem_list;
        dataItem_list.push_back(dataItem);

        Subscription::mObserverObj->notify(dataItem_list);

    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

static void power_connect_status_update(JNIEnv* env, jobject obj, jboolean status)
{
    int result = 0;
    ENTRY_LOG();

    do {
        PowerConnectStateDataItem powerconnectstate_data_item;
        powerconnectstate_data_item.mState = status;

        IDataItem *dataItem = &powerconnectstate_data_item;

        std::list<IDataItem *> dataItem_list;
        dataItem_list.push_back(dataItem);

        Subscription::mObserverObj->notify(dataItem_list);

    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

static void timezone_change_update(JNIEnv* env, jobject obj,
    jlong curTimeMillis, jint rawOffset, jint dstOffset)
{
    int result = 0;
    ENTRY_LOG();

    do {
        TimeZoneChangeDataItem timezonechange_data_item(curTimeMillis,
                                                        rawOffset, dstOffset);

        std::list<IDataItem *> dataItem_list;
        dataItem_list.push_back(&timezonechange_data_item);

        Subscription::mObserverObj->notify(dataItem_list);

    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

static void time_change_update(JNIEnv* env, jobject obj,
    jlong curTimeMillis, jint rawOffset, jint dstOffset)
{
    int result = 0;
    ENTRY_LOG();

    do {
        TimeChangeDataItem timechange_data_item(curTimeMillis,
                                                rawOffset, dstOffset);

        std::list<IDataItem *> dataItem_list;
        dataItem_list.push_back(&timechange_data_item);

        Subscription::mObserverObj->notify(dataItem_list);

    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

static void shutdown_update(JNIEnv* env, jobject obj)
{
    int result = 0;
    ENTRY_LOG();

    do {
        ShutdownStateDataItem shutdown_data_item(true);

        std::list<IDataItem *> dataItem_list;
        dataItem_list.push_back(&shutdown_data_item);

        Subscription::mObserverObj->notify(dataItem_list);

    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

static void wifi_supplicant_status_update(JNIEnv *env, jobject obj, jint state,
                                     jint ap_mac_valid,
                                     jbyteArray ap_mac_array,
                                     jint ssid_valid,
                                     jcharArray ssid_array)
{
    int result = 0;
    ENTRY_LOG();

    do {
            WifiSupplicantStatusDataItem wifisupplicantstatus_data_item;

            jbyte * mac_arr = 0;
            jchar * ssid_arr = 0;
            if(ap_mac_valid)
                mac_arr = env->GetByteArrayElements(ap_mac_array, 0);
            if(ssid_valid)
                ssid_arr = env->GetCharArrayElements(ssid_array, 0);

            wifisupplicantstatus_data_item.mState = (WifiSupplicantStatusDataItem::WifiSupplicantState)state;
            wifisupplicantstatus_data_item.mApMacAddressValid = ap_mac_valid;
            if((mac_arr != NULL) &&
               (wifisupplicantstatus_data_item.mApMacAddressValid == 1)) {
                for(int i=0;i<MAC_ADDRESS_LENGTH;i++) {
                    wifisupplicantstatus_data_item.mApMacAddress[i] = mac_arr[i];
                }
            }
            int i=0;
            wifisupplicantstatus_data_item.mWifiApSsidValid = ssid_valid;
            if((ssid_arr != NULL) &&
               (wifisupplicantstatus_data_item.mWifiApSsidValid == 1)) {
                for(i=0;i<SSID_LENGTH;i++) {
                    if(ssid_arr[i+1] != '"')
                    {
                        wifisupplicantstatus_data_item.mWifiApSsid.push_back(ssid_arr[i+1]);
                    }
                    else
                        break;
                }
            }

            if(NULL != mac_arr) {
                env->ReleaseByteArrayElements(ap_mac_array, mac_arr, 0);
            }

            if(NULL != ssid_arr) {
                env->ReleaseCharArrayElements(ssid_array, ssid_arr, 0);
            }


            IDataItem *dataItem = &wifisupplicantstatus_data_item;

            std::list<IDataItem *> dataItem_list;
            dataItem_list.push_back(dataItem);

            Subscription::mObserverObj->notify(dataItem_list);

    } while (0);

    EXIT_LOG_WITH_ERROR("%d", result);
}

static JNINativeMethod sMethods[] = {
     /* name, signature, funcPtr */
    {"native_subscription_class_init", "()V", (void *)classInit},
    {"native_subscription_init", "()V", (void *)instanceInit},
    {"native_subscription_deinit", "()V", (void *)instanceDeinit},
    {"native_bool_dataitem_update", "([I[Z)V", (void *)bool_dataitem_update},
    {"native_string_dataitem_update", "(ILjava/lang/String;)V", (void *)string_dataitem_update},
    {"native_networkinfo_update", "(ZILjava/lang/String;Ljava/lang/String;ZZ)V", (void *)networkinfo_update},
    {"native_serviceinfo_update", "(ILjava/lang/String;II)V", (void *)serviceinfo_update},
    {"native_cell_update_lte", "(IIIIIIII)V", (void *)cell_update_lte},
    {"native_cell_update_gw", "(IIIIIII)V", (void *)cell_update_gw},
    {"native_cell_update_cdma", "(IIIIIIIZII)V", (void *)cell_update_cdma},
    {"native_cell_update_ooo", "(II)V", (void *)cell_update_ooo},
    {"native_service_state_update", "(I)V", (void *)service_state_update},
    {"native_screen_status_update", "(Z)V", (void*) screen_status_update},
    {"native_power_connect_status_update", "(Z)V", (void*) power_connect_status_update},
    {"native_timezone_change_update", "(JII)V", (void*) timezone_change_update},
    {"native_time_change_update", "(JII)V", (void*) time_change_update},
    {"native_shutdown_update", "()V", (void*) shutdown_update},
    {"native_wifi_supplicant_status_update", "(II[BI[C)V", (void*) wifi_supplicant_status_update}
};

int register_Subscription(JNIEnv* env) {
    return jniRegisterNativeMethods(env, "com/qualcomm/location/osagent/OsAgent",
                                    sMethods, NELEM(sMethods));
}
