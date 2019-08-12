/******************************************************************************
  @file    RadioConfig.java

  ---------------------------------------------------------------------------
  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

package com.qualcomm.qti.radioconfiginterface;

import com.qualcomm.qti.radioconfiginterface.IRadioConfig;
import com.qualcomm.qti.radioconfiginterface.RadioConfigItem;
import android.util.Log;
import java.util.List;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.TreeMap;
import android.os.Handler;
import android.os.Parcel;
import android.os.Parcelable;
import android.os.Message;
import android.content.Context;
import android.os.IBinder;
import android.os.RemoteException;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.ComponentName;

/**
 * This class provides Radio Config Interface
 * for the clients to set/get the config settings.
 *
 * Client needs to instantiate this class to
 * use the IRadioConfig.aidl functionality
 */

public class RadioConfig {
    private final String LOG_TAG = "RadioConfig";

    private Context context;


    /* used internally to check the status of serviceConnection */
    private Boolean serviceConnected;

    private RadioConfigServiceConnection mConnection;
    private IRadioConfig radioConfigService = null;

    //List of handlers to store with ids
    private Handler serviceConnectionStatusHandler = null;
    private int serviceConnectionStatusId;

    /**
     * Error constants.
     * returned by all the functions.
     */
    public static class ErrorConstants {
        public static final int RADIO_CONFIG_SUCCESS = 0;
        public static final int RADIO_CONFIG_ERROR_GENERIC = 1;
        public static final int RADIO_CONFIG_INIT_FAILURE = 2;
        public static final int RADIO_CONFIG_SERVICE_NOT_CONNECTED = 3;
        public static final int RADIO_CONFIG_INVALID_SLOT_ID = 4;
        public static final int RADIO_CONFIG_INTERNAL_FAILURE = 5;
        public static final int RADIO_CONFIG_AUTH_FAILURE = 5;

    }

    /**
     * Defines the type of ConfigConstant.
     * To be used in the requests.
     */
    public static class ConfigConstants {
        public static final int CONFIG_UNKNOWN = -1;
        public static final int CONFIG_START = 1;
        public static final int CONFIG_ITEM_IN_CALL_LTE_RSRP_LOW = CONFIG_START;
        public static final int CONFIG_ITEM_IN_CALL_LTE_RSRP_MID = 2;
        public static final int CONFIG_ITEM_IN_CALL_LTE_RSRP_HIGH = 3;
        public static final int CONFIG_ITEM_IN_CALL_WIFI_RSSI_THRESHOLD_LOW = 4;
        public static final int CONFIG_ITEM_IN_CALL_WIFI_RSSI_THRESHOLD_HIGH = 5;
        public static final int CONFIG_ITEM_IN_CALL_WIFI_SINR_THRESHOLD_LOW = 6;
        public static final int CONFIG_ITEM_IN_CALL_WIFI_SINR_THRESHOLD_HIGH = 7;
        public static final int CONFIG_ITEM_IDLE_LTE_RSRP_LOW = 8;
        public static final int CONFIG_ITEM_IDLE_LTE_RSRP_MID = 9;
        public static final int CONFIG_ITEM_IDLE_LTE_RSRP_HIGH = 10;
        public static final int CONFIG_ITEM_IDLE_WIFI_RSSI_THRESHOLD_LOW = 11;
        public static final int CONFIG_ITEM_IDLE_WIFI_RSSI_THRESHOLD_HIGH = 12;
        public static final int CONFIG_ITEM_IDLE_WIFI_SINR_THRESHOLD_LOW = 13;
        public static final int CONFIG_ITEM_IDLE_WIFI_SINR_THRESHOLD_HIGH = 14;
        public static final int CONFIG_ITEM_ECIO_1X_THRESHOLD_LOW = 15;
        public static final int CONFIG_ITEM_ECIO_1X_THRESHOLD_HIGH = 16;

        // LTED provisioning related ConfigItems
        // LtedConfig message will be packed in the byteArrayValue
        public static final int CONFIG_ITEM_SET_LTED_CONFIG   = RadioConfigParcelable.CONFIG_ITEM_SET_LTED_CONFIG;   //100;
        public static final int CONFIG_ITEM_GET_LTED_CONFIG   = RadioConfigParcelable.CONFIG_ITEM_GET_LTED_CONFIG;   //101;
        public static final int CONFIG_ITEM_SET_LTED_CATEGORY = RadioConfigParcelable.CONFIG_ITEM_SET_LTED_CATEGORY; //102;
        public static final int CONFIG_ITEM_GET_LTED_CATEGORY = RadioConfigParcelable.CONFIG_ITEM_GET_LTED_CATEGORY; //103;

        /* Add the config items here
          */
        public static final int CONFIG_END = CONFIG_ITEM_GET_LTED_CATEGORY;
    }

    /**
     * Constructor
     * @param context context in which the bindService will be
     *                initiated.
     */
    public RadioConfig(Context context){
        this.context = context;
        serviceConnected = false;
    }

    /**
     * To check if the service is connected or not
     * @return boolean true if service is connected, false oterwise
     */
    public boolean isServiceConnected() {
        return serviceConnected;
    }


    /**
     * Initiate connection with the service.
     *
     * @param callbackHandler Handler to which the result message
     *                        needs to be sent. The message will
     *                        contain a integer value.
     *                        0 - service is not connected
     *                        1 - service is connected
     *
     * @param serviceConnectionStatusId This interger id will be
     *                                  put in the "what" variable
     *                                  of the message that is sent
     *                                  to the handler. Can be used
     *                                  as an identifier.
     *
     * @return boolean Immediate result of the operation. true if
     *                 successful.
     *                 NOTE: This does not garuntee a successful
     *                 connection. The client needs to use handler
     *                 to listen to the Result.
     *
     */
    public boolean connectService(Handler callbackHandler, int serviceConnectionStatusId){
        serviceConnectionStatusHandler = callbackHandler;
        this.serviceConnectionStatusId = serviceConnectionStatusId;

        Log.e(LOG_TAG, "Creating RadioConfig Service, if not started start");
        Intent intent = new Intent();
        intent.setComponent(new ComponentName("com.qualcomm.qti.radioconfiginterface", "com.qualcomm.qti.radioconfiginterface.RadioConfigService"));
        mConnection = new RadioConfigServiceConnection();
        boolean sucess = context.bindService(intent, mConnection, Context.BIND_AUTO_CREATE);
        Log.e(LOG_TAG, "binService result: " + sucess);

        return sucess;
    }

    /**
     * Disconnect the connection with the Service.
     *
     */
    public void disconnectService() {
        Log.i(LOG_TAG, "releaseRadioConfigService");
        if (mConnection != null) {
            context.unbindService(mConnection);
            mConnection = null;
        }
    }

    /**
     * Set int value of list of Radio Config items
     *
     * @param List<RadioConfigItem<Integer>>
     *     Should be a list of RadioConfigItem<Integer> Objects
     *     Each object should have item, and value filled in.
     *     Error field will be ignored.
     *     NOTE: Currently the order in the list is not preserved.
     *
     * @param slotId Currently not supported.
     *              For multisim, the slot on which the settings
     *              needs to be propagated.
     *
     * @return List<RadioConfigItem<Integer>>
     *    Will be a list of RadioConfigItem<Integer>
     *    Each object will contain item, and error fields
     *    value field can be ignored.
     *
     *    For Error field refer to
     *    com.qualcomm.qti.RadioConfigInterface.RadioConfig#ErrorConstants
     */
    public List<RadioConfigItem<Integer>> setIntConfig(List<RadioConfigItem<Integer>> config, int slotId){

        //Return a list with appropriate error filled in if any of the below conditions
        if(!serviceConnected){
            Log.e(LOG_TAG, "service not connected!");
            return formulateErrorList(config, ErrorConstants.RADIO_CONFIG_SERVICE_NOT_CONNECTED);
        }
        if(slotId != 0) {
            //TODO: Add the right slot check for multisim
            Log.e(LOG_TAG, "Invalid slot id!");
            return formulateErrorList(config, ErrorConstants.RADIO_CONFIG_INVALID_SLOT_ID);
        }

        Log.i(LOG_TAG, "setIntConfig");

        RadioConfigIntegerList configList = new RadioConfigIntegerList();
        configList.setList(config);

        RadioConfigIntegerList errorList;
        List<RadioConfigItem<Integer>> ret = null;
        RadioConfigItem<Integer> item;

        try {
            errorList = radioConfigService.setIntConfig(configList, slotId);
            ret = new ArrayList<RadioConfigItem<Integer>>();
            List<RadioConfigItem<Integer>> tempList = errorList.getList();

            for(RadioConfigItem<Integer> temp : tempList){
                item = new RadioConfigItem<Integer>();
                if(temp.hasItem()){
                    item.setItem(temp.getItem());
                }
                if(temp.hasError()){
                    item.setError(temp.getError());
                }
                ret.add(item);
            }
        } catch(RemoteException e){
            Log.e(LOG_TAG, "setIntconfig, remoteexception");
            e.printStackTrace();
            //populate ret to return all errors!!
            ret = formulateErrorList(config, ErrorConstants.RADIO_CONFIG_INTERNAL_FAILURE);
        }

        return ret;
    }

    /**
     * CURRENTLY NOT SUPPORTED
     *
     * Get int value of a list of Radio Config items
     *
     * @param List<Integer>
     *     List of config items for which you want the value
     *
     * @param slotId Currently not supported.
     *              For multisim, the slot on which the settings
     *              needs to be read.
     *
     * @return List<RadioConfigItem<Integer>>
     *    Will be a list of RadioConfigItem<Integer> Objects.
     *    Each object will contain item, value and error fields
     *    filled in.
     *    Error field must be checked before using the value.
     *
     *    For Error field refer to
     *    com.qualcomm.qti.RadioConfigInterface.RadioConfig#ErrorConstants
     */
    public List<RadioConfigItem<Integer>> getIntConfig(List<Integer> config, int slotId){
        return null;
    }

    /**
     * CURRENTLY NOT SUPPORTED
     *
     * Set String value of list of Radio Config items
     *
     * @param List<RadioConfigItem<String>>
     *     Should be a list of RadioConfigItem<String> Objects. Each
     *     object should have item, and value filled in. Error field
     *     will be ignored.
     *
     * @param slotId Currently not supported.
     *              For multisim, the slot on which the settings
     *              needs to be propagated.
     *
     * @return List<RadioConfigItem<String>>
     *    Will be a list of RadioConfigItem<String> Objects. Each
     *    object will contain item, and error fields value field can
     *    be ignored.
     *
     *    For Error field refer to
     *    com.qualcomm.qti.RadioConfigInterface.RadioConfig#ErrorConstants
     */
    public List<RadioConfigItem<String>> setStringConfig(List<RadioConfigItem<String>> config, int slotId){
        return null;
    }

    /**
     * CURRENTLY NOT SUPPORTED
     *
     * Get String value of list of Radio Config items
     *
     * @param List<Integer>
     *     List of config items for which you want the value
     *
     * @param slotId Currently not supported.
     *              For multisim, the slot on which the settings
     *              needs to be read.
     *
     * @return List<RadioConfigItem<String>>
     *    Will be a list of RadioConfigItem<String> Objects.
     *    Each object will contain item, value and error fields
     *    filled in.
     *    Error field must be checked before using the value.
     *
     *    For Error field refer to
     *    com.qualcomm.qti.RadioConfigInterface.RadioConfig#ErrorConstants
     */
    public List<RadioConfigItem<String>> getStringConfig(List<Integer> config, int slotId){
        return null;
    }

    /**
     * Set parcelable value of Radio Config item
     *
     * @param item Radio Config Item
     *
     * @param config Input parcelable object.
     *
     * @param slotId For multisim, the slot on which the settings
     *              needs to be set.
     *
     * @return operation result code
     *    For Error field refer to
     *    com.qualcomm.qti.RadioConfigInterface.RadioConfig#ErrorConstants
     */
    public int setParcelableConfig(int item, Parcelable config, int slotId) {
        int ret = ErrorConstants.RADIO_CONFIG_SUCCESS;
        //Return a list with appropriate error filled in if any of the below conditions
        if(!serviceConnected){
            Log.e(LOG_TAG, "service not connected!");
            return ErrorConstants.RADIO_CONFIG_SERVICE_NOT_CONNECTED;
        }

        Log.i(LOG_TAG, "setParcelableConfig");

        RadioConfigParcelable rcConfig = new RadioConfigParcelable(config, item);

        try {
            ret = radioConfigService.setParcelableConfig(item, rcConfig, slotId);
        } catch(RemoteException e){
            Log.e(LOG_TAG, "setParcelableConfig, remoteexception");
            e.printStackTrace();
            ret = ErrorConstants.RADIO_CONFIG_INTERNAL_FAILURE;
        }

        return ret;
    }

    /**
     * Get parcelable value of Radio Config item
     *
     * @param item Radio Config Item
     *
     * @param extras Input parcelable object.
     *
     * @param slotId For multisim, the slot on which the settings
     *              needs to be set.
     *
     * @param config Output parcelable object. Will contains the parcelable settings
     *              value read from the requested Radio Config Item if the API returns
     *              success.
     *
     * @return operation result code
     *    For Error field refer to
     *    com.qualcomm.qti.RadioConfigInterface.RadioConfig#ErrorConstants
     */
    public int getParcelableConfig(int item, Parcelable extras, int slotId, Parcelable config) {
        int ret = ErrorConstants.RADIO_CONFIG_SUCCESS;
        //Return a list with appropriate error filled in if any of the below conditions
        if(!serviceConnected){
            Log.e(LOG_TAG, "service not connected!");
            return ErrorConstants.RADIO_CONFIG_SERVICE_NOT_CONNECTED;
        }

        Log.i(LOG_TAG, "getParcelableConfig");

        RadioConfigParcelable req  = new RadioConfigParcelable(extras, item);
        RadioConfigParcelable resp = new RadioConfigParcelable(config, item);

        try {
            ret = radioConfigService.getParcelableConfig(item, req, slotId, resp);
        } catch(RemoteException e){
            Log.e(LOG_TAG, "getParcelableConfig, remoteexception");
            e.printStackTrace();
            ret = ErrorConstants.RADIO_CONFIG_INTERNAL_FAILURE;
        }

        Log.i(LOG_TAG, "getParcelableConfig: pack the out parameter");

        Log.i(LOG_TAG, "getParcelableConfig: item = " + item);

        if (item == ConfigConstants.CONFIG_ITEM_GET_LTED_CONFIG) {
            LtedConfig outConfig = (LtedConfig)config;
            LtedConfig resultConfig = (LtedConfig)resp.getData();
            if (outConfig != null) {
                outConfig.copyFrom(resultConfig);
            }
            Log.i(LOG_TAG, "getParcelableConfig: out config = " + outConfig);
        } else if (item == ConfigConstants.CONFIG_ITEM_GET_LTED_CATEGORY) {
            LtedCategory outConfig = (LtedCategory)config;
            LtedCategory resultConfig = (LtedCategory)resp.getData();
            if (outConfig != null) {
                outConfig.copyFrom(resultConfig);
            }
            Log.i(LOG_TAG, "getParcelableConfig: out config = " + outConfig);
        }

        return ret;
    }

    private class ListenerTransport extends ILtedProvisioningListener.Stub {
        private LtedProvisioningListener mListener;

        ListenerTransport(LtedProvisioningListener listener) {
            mListener = listener;
        }

        @Override
        public void onPskExpired() {
            mListener.onPskExpired();
        }
    }

    private HashMap<LtedProvisioningListener,ListenerTransport> mListeners =
        new HashMap<LtedProvisioningListener,ListenerTransport>();

    private ListenerTransport wrapListener(LtedProvisioningListener listener) {
        if (listener == null) return null;
        synchronized (mListeners) {
            ListenerTransport transport = mListeners.get(listener);
            if (transport == null) {
                transport = new ListenerTransport(listener);
            }
            mListeners.put(listener, transport);
            return transport;
        }
    }

    /**
     * Register for LTED Provisioning Listener.
     *
     * @param listener Listener object
     *
     * @return true on success, false on failure
     */
    public boolean registerLtedProvisioningListener(LtedProvisioningListener listener) {
        boolean ret = false;
        ListenerTransport transport = wrapListener(listener);

        Log.d(LOG_TAG, "registerLtedProvisioningListener");

        try {
          ret = radioConfigService.registerLtedProvisioningListener(transport);
        } catch(RemoteException e){
            Log.e(LOG_TAG, "registerLtedProvisioningListener, remoteexception");
            e.printStackTrace();
        }
        return ret;
    }

    /**
     * deregister for LTED Provisioning Listener.
     *
     * @param listener Listener object
     *
     * @return true on success, false on failure
     */
    public boolean deregisterLtedProvisioningListener(LtedProvisioningListener listener) {
        boolean ret = false;
        ListenerTransport transport;

        Log.d(LOG_TAG, "deregisterLtedProvisioningListener");

        synchronized (mListeners) {
            transport = mListeners.remove(listener);
        }
        if (transport == null) return ret;

        try {
          ret = radioConfigService.deregisterLtedProvisioningListener(transport);
        } catch(RemoteException e){
            Log.e(LOG_TAG, "registerLtedProvisioningListener, remoteexception");
            e.printStackTrace();
        }
        return ret;
    }


    /*************************************************************************************
     *                     INTERNAL HELPER FUNCTIONS/VARIABLES                           *
     *************************************************************************************/

    private List<RadioConfigItem<Integer>> formulateErrorList(List<RadioConfigItem<Integer>> config, int error){
        List<RadioConfigItem<Integer>> ret = new ArrayList<RadioConfigItem<Integer>>();
        RadioConfigItem<Integer> item;
        for(RadioConfigItem<Integer> temp : config){
            item = new RadioConfigItem<Integer>();
            item.setItem(temp.getItem());
            item.setError(error);
            ret.add(item);
        }
        return ret;
    }

    private class RadioConfigServiceConnection implements ServiceConnection {

        public void onServiceConnected(ComponentName name, IBinder boundService) {

            Log.e(LOG_TAG, "Onservice connected, get the binder object");
            radioConfigService = IRadioConfig.Stub.asInterface((IBinder) boundService);
            Log.e(LOG_TAG, "radioconfigservice obtained from the binber!!!!");
            serviceConnected = true;
            if(serviceConnectionStatusHandler == null){
                Log.e(LOG_TAG, "ServiceStatusHandler is NULL");
            }else {
                Message msg = serviceConnectionStatusHandler.obtainMessage(serviceConnectionStatusId, serviceConnected);
                msg.sendToTarget();
            }
        }

        public void onServiceDisconnected(ComponentName name) {
            Log.i(LOG_TAG, "onServiceDisconnected, release the binder object");
            radioConfigService = null;
            serviceConnected = false;
            if(serviceConnectionStatusHandler == null){
                Log.e(LOG_TAG, "ServiceStatusHandler is NULL");
            }else {
                Message msg = serviceConnectionStatusHandler.obtainMessage(serviceConnectionStatusId, serviceConnected);
                msg.sendToTarget();
            }
        }
    }

}
