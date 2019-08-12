/******************************************************************************
  @file    RadioConfigService.java

  ---------------------------------------------------------------------------
  Copyright (c) 2014,2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

package com.qualcomm.qti.radioconfiginterface;

import com.qualcomm.qti.radioconfiginterface.RadioConfigSocket;
import com.google.protobuf.micro.ByteStringMicro;
import com.qualcomm.qti.radioconfiginterface.RadioConfigMsgPacker;
import com.qualcomm.qti.radioconfiginterface.RadioConfigSyncRequest;

import com.qualcomm.qti.radioconfiginterface.RadioConfig;
import com.qualcomm.qti.radioconfiginterface.RadioConfigItem;
import com.qualcomm.qti.radioconfiginterface.IRadioConfig;
import com.qualcomm.qti.radioconfiginterface.RadioConfigIntegerList;
import com.qualcomm.qti.radioconfiginterface.LtedAuthManager;

//proto
import com.qualcomm.qti.radioconfiginterface.RadioConfigInterface;

import android.app.Service;
import android.content.Intent;
import android.os.Handler;
import android.os.Binder;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.telephony.TelephonyManager;
import android.util.Log;

import android.content.Context;
import android.content.pm.Signature;
import android.content.pm.PackageManager;
import java.util.Map;
import java.util.List;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.TreeMap;
import java.util.Collections;
import java.util.UUID;
import java.io.IOException;
import java.lang.Byte;


public class RadioConfigService extends Service {
    private final String LOG_TAG = "RadioConfigService";
    private final int RADIO_CONFIG_SERVICE_MAX_TOKEN = 50; //reset after 50 req

    public static HashMap<Integer, RadioConfigSyncRequest> reqTable;

    public static final int EVENT_REQ = 1;
    public static final int EVENT_RESP = 2;

    private Context context;

    private final int DEFAULT_SLOT = 0;

    private static int mToken = 0;

    private RadioConfigSocket[] mSocket = null;
    private int mNumPhones = 0;

    private static final String OS_PLATFORM = "ANDROID";

    private UUID mOsId = UUID.nameUUIDFromBytes(OS_PLATFORM.getBytes());

    private LtedAuthManager mLtedAuthManager;

    private ArrayList<ILtedProvisioningListener> mListeners = new ArrayList<ILtedProvisioningListener>();

    private Handler mRespHdlr = new Handler() {
        public void handleMessage (Message msg) {
            Log.i(LOG_TAG, "handleMessage()");

            try {
                byte[] bytes = (byte[])msg.obj;
                RadioConfigInterface.MessageTag tag = RadioConfigInterface.MessageTag.parseFrom(bytes);

                int token = tag.getToken();
                Log.d( LOG_TAG, "handleMessage() - token: " + token +
                       ", type: " + tag.getType() +
                       ", Id: " + tag.getId() +
                       ", error: " + tag.getError() );

                if (tag.getType() == RadioConfigInterface.RADIO_CONFIG_MSG_RESPONSE) {

                    if( tag.getId() == RadioConfigInterface.RADIO_CONFIG_SET_CONFIG ||
                        tag.getId() == RadioConfigInterface.RADIO_CONFIG_GET_CONFIG ) {

                        RadioConfigSyncRequest reqMsgObj = null;

                        if(reqTable.containsKey(token)) {
                            reqMsgObj = reqTable.get(token);

                            if(reqMsgObj.getMsgId() != tag.getId()) {
                                Log.e(LOG_TAG, "TOKEN MISMATCH!!!! ERROR");
                                return;
                            }
                        }
                        else{
                            Log.e(LOG_TAG, "ENTRY NOT PRESENT IN THE TABLE");
                            return;
                        }

                        reqMsgObj.setRespBytes(tag.getPayload().toByteArray());
//                      reqTable.put(token, reqMsgObj);
                        reqMsgObj.notifyResp();
                    }
                } else if (tag.getType() == RadioConfigInterface.RADIO_CONFIG_MSG_INDICATION) {
                    byte []payload = null;
                    if (tag.hasPayload() && tag.getPayload() != null) {
                        payload = tag.getPayload().toByteArray();
                    }
                    handleUnsolIndication(tag.getId(), payload);
                } else {
                    Log.e(LOG_TAG, "unexpected msg type");
                }
            } catch (Exception e) {
                Log.e(LOG_TAG, "error occured when parsing the resp/ind");
            }
        }
    };

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(LOG_TAG, "onCreate()");
        context = this;

        //Table
        reqTable = new HashMap<Integer, RadioConfigSyncRequest>();

        TelephonyManager tm = (TelephonyManager)context.getSystemService(Context.TELEPHONY_SERVICE);
        mNumPhones = tm.getPhoneCount();
        mSocket = new RadioConfigSocket[mNumPhones];
        for (int i = 0; i < mNumPhones; i++) {
            mSocket[i] = new RadioConfigSocket(mRespHdlr, i);

            new Thread(mSocket[i]).start();
        }
        mLtedAuthManager = new LtedAuthManager(context);
    }

    @Override
    public IBinder onBind(Intent intent) {
        Log.i(LOG_TAG, "onBind()");
        return mBinder;
    }

    @Override
    public void onDestroy() {
        Log.i(LOG_TAG, "onDestroy()");
        for (int i = 0; i < mNumPhones; i++) {
            mSocket[i].toDestroy();
        }
        stopSelf();
        super.onDestroy();
    }

    private RadioConfigIntegerList generateErrorResponse(List<RadioConfigItem<Integer>> config, int error) {

        List<RadioConfigItem<Integer>> errorList = new ArrayList<RadioConfigItem<Integer>>();

        RadioConfigItem<Integer> item;
        for(RadioConfigItem<Integer> temp : config) {
            item = new RadioConfigItem<Integer>();
            item.setItem(temp.getItem());
            item.setError(error);
            errorList.add(item);
        }
        RadioConfigIntegerList ret = new RadioConfigIntegerList();
        ret.setList(errorList);
        return ret;
    }

    private synchronized int getToken() {
        mToken++;
        if(mToken > RADIO_CONFIG_SERVICE_MAX_TOKEN){
            mToken = 0;
        }
        return mToken;
    }

    private void handleUnsolIndication(int id, byte []payload) {
        switch (id) {
            case RadioConfigInterface.RADIO_CONFIG_UNSOL_PSK_EXPIRED:
                Log.d(LOG_TAG, "RADIO_CONFIG_UNSOL_PSK_EXPIRED");
                Log.d(LOG_TAG, "mListeners.size = " + mListeners.size());
                for (int i = 0; i < mListeners.size(); i++) {
                    ILtedProvisioningListener listener = mListeners.get(i);
                    try {
                        listener.onPskExpired();
                    } catch (Exception e) {
                        Log.d(LOG_TAG, "error occured onPskExpired: " + e);
                    }
                }
                break;

            default:
                Log.e(LOG_TAG, "unknown indication");
                break;
        }
    }

    private RadioConfigInterface.ConfigMsg sendRadioConfigInterfaceRequestSync(int slotId,
            int msgId, Object payload) {
        int token = getToken();
        Log.d(LOG_TAG, "sendRadioConfigInterfaceRequestSync");

        // get tag;
        RadioConfigInterface.MessageTag tag = new RadioConfigInterface.MessageTag();
        tag.setToken(token);
        tag.setType(RadioConfigInterface.RADIO_CONFIG_MSG_REQUEST);
        tag.setId(msgId);
        tag.setError(RadioConfigInterface.RADIO_CONFIG_ERR_SUCCESS);

        tag.setPayload(ByteStringMicro.copyFrom(
                    RadioConfigMsgPacker.packMsg(
                        msgId,
                        RadioConfigInterface.RADIO_CONFIG_MSG_REQUEST,
                        payload)));

        RadioConfigSyncRequest reqMsgObj = new RadioConfigSyncRequest(token, msgId);
        reqMsgObj.setReqBytes(RadioConfigMsgPacker.packTag(tag));
        Log.i(LOG_TAG, "packed information into a syncreqobj");
        reqTable.put(token, reqMsgObj);
        Log.i(LOG_TAG, "put the token and obj into the hashtable!");
        Message msg = mSocket[slotId].obtainMessage(EVENT_REQ, reqMsgObj);
        Log.i(LOG_TAG, "obtained msg");
        msg.sendToTarget();
        Log.i(LOG_TAG, "sent to target, will wait for resp; token: " + token);
        reqMsgObj.waitForResp();
        Log.i(LOG_TAG, "notified of response!! token: " + token);

        //clean up the table
        reqTable.remove(token);

        RadioConfigInterface.ConfigMsg resp = null;

        //processing the response
        try {
            byte[] respBytes = reqMsgObj.getRespBytes();
            if (respBytes != null) {
                resp = RadioConfigInterface.ConfigMsg.parseFrom(respBytes);
            } else {
                Log.e(LOG_TAG, "respBytes is null!!");
            }
        }catch( Exception e){
            e.printStackTrace();
        }

        return resp;
    }

    private int setLtedConfig(int item, RadioConfigParcelable config, int slotId)
            throws RemoteException {
        LtedConfig req = null;

        Log.d(LOG_TAG, "setLtedConfig: item = " + item);

        if(!mLtedAuthManager.verifyAuthenticity(Binder.getCallingUid())) {
            Log.e(LOG_TAG, "Authentication Failure; return error");
            return RadioConfig.ErrorConstants.RADIO_CONFIG_AUTH_FAILURE;
        }

        if (item == RadioConfig.ConfigConstants.CONFIG_ITEM_SET_LTED_CONFIG) {
            req = (LtedConfig)config.getData();
            Log.d(LOG_TAG, "setLtedConfig: req = " + req);
        }

        if (req == null) {
            Log.e(LOG_TAG, "setLtedConfig: Invalid input");
            return RadioConfig.ErrorConstants.RADIO_CONFIG_ERROR_GENERIC;
        }

        // get request
        RadioConfigInterface.ConfigMsg configMsg = new RadioConfigInterface.ConfigMsg();

        RadioConfigInterface.ConfigItemMsg itemMsg = new RadioConfigInterface.ConfigItemMsg();
        itemMsg.setItem(mapRadioConfigItemToProtoItem(item));

        RadioConfigInterface.LtedConfig ltedConfig = new RadioConfigInterface.LtedConfig();
        RadioConfigInterface.Value128 osId = new RadioConfigInterface.Value128();

        if (req.getOsId() != null) {
            Log.d(LOG_TAG, "OS ID from the request");
            osId.setLsb(req.getOsId().getLeastSignificantBits());
            osId.setMsb(req.getOsId().getMostSignificantBits());
        } else {
            Log.d(LOG_TAG, "Use default OS ID");
            osId.setLsb(mOsId.getLeastSignificantBits());
            osId.setMsb(mOsId.getMostSignificantBits());
        }

        Log.d(LOG_TAG, "set osId: msb : " + osId.getMsb() + " lsb : " + osId.getLsb());
        ltedConfig.setOsId(osId);

        Log.d(LOG_TAG, "set apn: " + req.getApn());
        if (req.getApn() != null) {
            ltedConfig.setApn(req.getApn());
        } else {
            Log.i(LOG_TAG, "APN is null");
        }

        Log.d(LOG_TAG, "req.getAnnouncingPolicyCount = " + req.getAnnouncingPolicyCount());
        if (req.getAnnouncingPolicyCount() > 0) {
            for (LtedConfig.AnnouncingPolicy annPolicy : req.getAnnouncingPolicyList()) {
                RadioConfigInterface.AnnouncingPolicy value =
                        new RadioConfigInterface.AnnouncingPolicy();
                RadioConfigInterface.Plmn plmn = new RadioConfigInterface.Plmn();
                plmn.setMcc(annPolicy.getPlmn().getMcc());
                plmn.setMnc(annPolicy.getPlmn().getMnc());
                value.setPlmn(plmn);
                value.setValidityTime(annPolicy.getValidityTime());
                value.setRange(annPolicy.getRange().getRange());

                Log.e(LOG_TAG, "add : " + value);
                ltedConfig.addAnnouncingPolicy(value);
            }
        }

        Log.d(LOG_TAG, "req.getMonitoringPolicyCount = " + req.getMonitoringPolicyCount());
        if (req.getMonitoringPolicyCount() > 0) {
            for (LtedConfig.MonitoringPolicy monPolicy : req.getMonitoringPolicyList()) {
                RadioConfigInterface.MonitoringPolicy value =
                        new RadioConfigInterface.MonitoringPolicy();
                RadioConfigInterface.Plmn plmn = new RadioConfigInterface.Plmn();
                plmn.setMcc(monPolicy.getPlmn().getMcc());
                plmn.setMnc(monPolicy.getPlmn().getMnc());
                value.setPlmn(plmn);
                value.setValidityTime(monPolicy.getValidityTime());
                value.setRemainingTime(monPolicy.getRemainingTime());

                Log.e(LOG_TAG, "add : " + value);
                ltedConfig.addMonitoringPolicy(value);
            }
        }

        byte []bakPassword = req.getBakPassword();
        if (bakPassword != null) {
            Log.d(LOG_TAG, "setting bakPassword");
            ltedConfig.setBakPassword(ByteStringMicro.copyFrom(bakPassword));
        } else {
            Log.d(LOG_TAG, "bakPassword is null!!");
        }

        itemMsg.setByteArrayValue(ByteStringMicro.copyFrom(ltedConfig.toByteArray()));
        configMsg.addSettings(itemMsg);

        RadioConfigInterface.ConfigMsg resp = sendRadioConfigInterfaceRequestSync(
                slotId,
                RadioConfigInterface.RADIO_CONFIG_SET_CONFIG,
                configMsg);

        int ret = RadioConfig.ErrorConstants.RADIO_CONFIG_ERROR_GENERIC;

        if (resp != null) {
            List<RadioConfigInterface.ConfigItemMsg> settingsListResp = resp.getSettingsList();

            for (RadioConfigInterface.ConfigItemMsg entry : settingsListResp) {
                Log.e(LOG_TAG, "entry.getItem : " + entry.getItem());
                Log.e(LOG_TAG, "entry.getErrorCause : " + entry.getErrorCause());
                if (entry.getItem() == item) {
                    ret = mapProtoErrortoRadioConfigError(entry.getErrorCause());
                    break;  // Expected only one item!!
                }
            }
        } else {
            ret = RadioConfig.ErrorConstants.RADIO_CONFIG_ERROR_GENERIC;
        }

        Log.d(LOG_TAG, "setLtedConfig: ret = " + ret);

        return ret;
    }

    private int setLtedCategory(int item, RadioConfigParcelable config, int slotId)
            throws RemoteException {
        LtedCategory req = null;

        Log.d(LOG_TAG, "setLtedCategory: item = " + item);

        if(!mLtedAuthManager.verifyAuthenticity(Binder.getCallingUid())) {
            Log.e(LOG_TAG, "Authentication Failure; return error");
            return RadioConfig.ErrorConstants.RADIO_CONFIG_AUTH_FAILURE;
        }

        if (item == RadioConfig.ConfigConstants.CONFIG_ITEM_SET_LTED_CATEGORY) {
            req = (LtedCategory)config.getData();
            Log.d(LOG_TAG, "setLtedCategory: req = " + req);
        }

        if (req == null) {
            Log.e(LOG_TAG, "setLtedCategory: Invalid input");
            return RadioConfig.ErrorConstants.RADIO_CONFIG_ERROR_GENERIC;
        }

        // get request
        RadioConfigInterface.ConfigMsg configMsg = new RadioConfigInterface.ConfigMsg();

        RadioConfigInterface.ConfigItemMsg itemMsg = new RadioConfigInterface.ConfigItemMsg();
        itemMsg.setItem(mapRadioConfigItemToProtoItem(item));

        RadioConfigInterface.LtedCategory ltedCategory = new RadioConfigInterface.LtedCategory();
        ltedCategory.setOsAppId(req.getOsAppId());
        ltedCategory.setCategory(req.getCategory());

        itemMsg.setByteArrayValue(ByteStringMicro.copyFrom(ltedCategory.toByteArray()));
        configMsg.addSettings(itemMsg);

        RadioConfigInterface.ConfigMsg resp = sendRadioConfigInterfaceRequestSync(
                slotId,
                RadioConfigInterface.RADIO_CONFIG_SET_CONFIG,
                configMsg);

        int ret = RadioConfig.ErrorConstants.RADIO_CONFIG_ERROR_GENERIC;

        if (resp != null) {
            List<RadioConfigInterface.ConfigItemMsg> settingsListResp = resp.getSettingsList();

            for(RadioConfigInterface.ConfigItemMsg entry : settingsListResp) {
                Log.e(LOG_TAG, "entry.getItem : " + entry.getItem());
                Log.e(LOG_TAG, "entry.getErrorCause : " + entry.getErrorCause());
                if (entry.getItem() == item) {
                    ret = mapProtoErrortoRadioConfigError(entry.getErrorCause());
                    break;  // Expected only one item!!
                }
            }
        } else {
            ret = RadioConfig.ErrorConstants.RADIO_CONFIG_ERROR_GENERIC;
        }

        Log.d(LOG_TAG, "setLtedCategory: ret = " + ret);

        return ret;
    }

    private int getLtedConfig(int item, int slotId, RadioConfigParcelable result)
            throws RemoteException  {
        LtedConfig config = null;

        Log.d(LOG_TAG, "getLtedConfig: item = " + item);

        if(!mLtedAuthManager.verifyAuthenticity(Binder.getCallingUid())) {
            Log.e(LOG_TAG, "Authentication Failure; return error");
            return RadioConfig.ErrorConstants.RADIO_CONFIG_AUTH_FAILURE;
        }

        if (item != RadioConfig.ConfigConstants.CONFIG_ITEM_GET_LTED_CONFIG) {
            Log.e(LOG_TAG, "invalid item = " + item);
            return RadioConfig.ErrorConstants.RADIO_CONFIG_ERROR_GENERIC;
        }

        // get request
        RadioConfigInterface.ConfigMsg configMsg = new RadioConfigInterface.ConfigMsg();
        RadioConfigInterface.ConfigItemMsg itemMsg = new RadioConfigInterface.ConfigItemMsg();
        itemMsg.setItem(mapRadioConfigItemToProtoItem(item));
        configMsg.addSettings(itemMsg);

        RadioConfigInterface.ConfigMsg resp = sendRadioConfigInterfaceRequestSync(
                slotId,
                RadioConfigInterface.RADIO_CONFIG_GET_CONFIG,
                configMsg);

        int ret = RadioConfig.ErrorConstants.RADIO_CONFIG_ERROR_GENERIC;

        if (resp != null) {
            List<RadioConfigInterface.ConfigItemMsg> settingsListResp = resp.getSettingsList();

            for(RadioConfigInterface.ConfigItemMsg entry : settingsListResp) {
                Log.e(LOG_TAG, "entry.getItem : " + entry.getItem());
                if (entry.getItem() == item) {
                    ret = mapProtoErrortoRadioConfigError(entry.getErrorCause());

                    if (ret != RadioConfig.ErrorConstants.RADIO_CONFIG_SUCCESS) {
                        break; // failure
                    }

                    RadioConfigInterface.LtedConfig ltedConfig = null;
                    try {
                        byte[] dbg_bytes = entry.getByteArrayValue().toByteArray();
                        ltedConfig = RadioConfigInterface.LtedConfig.parseFrom(dbg_bytes);
                    } catch (Exception e) {
                        ret = RadioConfig.ErrorConstants.RADIO_CONFIG_ERROR_GENERIC;
                        Log.e(LOG_TAG, "Exception in parseFrom: " + e);
                        break;
                    }
                    config = getLtedConfigFrom(ltedConfig);

                    if (config == null) {
                        ret = RadioConfig.ErrorConstants.RADIO_CONFIG_ERROR_GENERIC;
                        break;
                    }

                    if (result != null) {
                        result.setItem(item);
                        result.setData(config);
                    }
                    break; // only one config expected - CONFIG_ITEM_GET_LTED_CONFIG
                }
            }
        } else {
            Log.e(LOG_TAG, "resp is null!!");
            ret = RadioConfig.ErrorConstants.RADIO_CONFIG_ERROR_GENERIC;
        }

        Log.d(LOG_TAG, "getLtedConfig: ret = " + ret);
        return ret;
    }

    private int getLtedCategory(int item, RadioConfigParcelable extras,
            int slotId, RadioConfigParcelable result) throws RemoteException  {
        LtedCategory req = (LtedCategory)extras.getData();
        Log.d(LOG_TAG, "getLtedCategory: " + req);

        if(!mLtedAuthManager.verifyAuthenticity(Binder.getCallingUid())) {
            Log.e(LOG_TAG, "Authentication Failure; return error");
            return RadioConfig.ErrorConstants.RADIO_CONFIG_AUTH_FAILURE;
        }

        if (req == null) {
            Log.e(LOG_TAG, "getLtedCategory: Invalid input");
            return RadioConfig.ErrorConstants.RADIO_CONFIG_ERROR_GENERIC;
        }

        // get request
        RadioConfigInterface.ConfigMsg configMsg = new RadioConfigInterface.ConfigMsg();

        RadioConfigInterface.ConfigItemMsg itemMsg = new RadioConfigInterface.ConfigItemMsg();
        itemMsg.setItem(mapRadioConfigItemToProtoItem(item));

        RadioConfigInterface.LtedCategory ltedCategory = new RadioConfigInterface.LtedCategory();
        ltedCategory.setOsAppId(req.getOsAppId());

        itemMsg.setByteArrayValue(ByteStringMicro.copyFrom(ltedCategory.toByteArray()));
        configMsg.addSettings(itemMsg);

        RadioConfigInterface.ConfigMsg resp = sendRadioConfigInterfaceRequestSync(
                slotId,
                RadioConfigInterface.RADIO_CONFIG_GET_CONFIG,
                configMsg);

        int ret = RadioConfig.ErrorConstants.RADIO_CONFIG_ERROR_GENERIC;

        if (resp != null) {
            List<RadioConfigInterface.ConfigItemMsg> settingsListResp = resp.getSettingsList();

            // TODO; expected only one entry in the ConfigItemMsg
            for(RadioConfigInterface.ConfigItemMsg entry : settingsListResp) {
                Log.e(LOG_TAG, "entry.getItem : " + entry.getItem());
                if (entry.getItem() == RadioConfig.ConfigConstants.CONFIG_ITEM_GET_LTED_CATEGORY) {
                    ret = mapProtoErrortoRadioConfigError(entry.getErrorCause());

                    if (ret != RadioConfig.ErrorConstants.RADIO_CONFIG_SUCCESS) {
                        break;
                    }

                    try {
                        ltedCategory = RadioConfigInterface.LtedCategory.parseFrom(
                                entry.getByteArrayValue().toByteArray());
                    } catch (Exception e) {
                        ret = RadioConfig.ErrorConstants.RADIO_CONFIG_ERROR_GENERIC;
                        Log.e(LOG_TAG, "Exception in parseFrom: " + e);
                        break;
                    }

                    LtedCategory category = getLtedCategoryFrom(ltedCategory);

                    if (category == null) {
                        ret = RadioConfig.ErrorConstants.RADIO_CONFIG_ERROR_GENERIC;
                        break;
                    }

                    if (result != null) {
                        result.setItem(item);
                        result.setData(category);
                    }
                    break; // only one config expected - CONFIG_ITEM_GET_LTED_CATEGORY
                }
            }
        } else {
            ret = RadioConfig.ErrorConstants.RADIO_CONFIG_ERROR_GENERIC;
        }
        LtedCategory lc = (LtedCategory)result.getData();
        Log.d(LOG_TAG, "getLtedCategory: lc = " + lc);
        return ret;
    }

    private LtedConfig getLtedConfigFrom(RadioConfigInterface.LtedConfig data) {
        LtedConfig config = new LtedConfig();

        if (data == null) {
            return null;
        }

        Log.e(LOG_TAG, "data.getApn() = " + data.getApn());
        config.setApn(data.getApn());

        ArrayList<LtedConfig.AnnouncingPolicy> annList =
                new ArrayList<LtedConfig.AnnouncingPolicy>();

        Log.e(LOG_TAG, "data.getAnnouncingPolicyCount() = " + data.getAnnouncingPolicyCount());
        if (data.getAnnouncingPolicyCount() > 0) {
            for (RadioConfigInterface.AnnouncingPolicy annPol : data.getAnnouncingPolicyList()) {
                LtedConfig.AnnouncingPolicy recvPolicy =
                    new LtedConfig.AnnouncingPolicy();
                LtedConfig.Plmn plmn = new LtedConfig.Plmn(
                        annPol.getPlmn().getMcc(),
                        annPol.getPlmn().getMnc());
                recvPolicy.setPlmn(plmn);
                recvPolicy.setValidityTime(annPol.getValidityTime());
                LtedConfig.Range range = new LtedConfig.Range(annPol.getRange());
                recvPolicy.setRange(range);

                Log.e(LOG_TAG, "Annoouncing Policy: plmn: " + recvPolicy.getPlmn() +
                        " validityTime: " + recvPolicy.getValidityTime() + " range: " +
                        recvPolicy.getRange());

                annList.add(recvPolicy);
            }
            config.setAnnouncingPolicyList(annList);
        }

        ArrayList<LtedConfig.MonitoringPolicy> monList =
                new ArrayList<LtedConfig.MonitoringPolicy>();

        Log.e(LOG_TAG, "data.getMonitoringPolicyCount() = " + data.getMonitoringPolicyCount());
        if (data.getMonitoringPolicyCount() > 0) {
            for (RadioConfigInterface.MonitoringPolicy monPol : data.getMonitoringPolicyList()) {
                LtedConfig.MonitoringPolicy recvPolicy =
                    new LtedConfig.MonitoringPolicy();
                LtedConfig.Plmn plmn = new LtedConfig.Plmn(
                        monPol.getPlmn().getMcc(),
                        monPol.getPlmn().getMnc());
                recvPolicy.setPlmn(plmn);
                recvPolicy.setValidityTime(monPol.getValidityTime());
                recvPolicy.setRemainingTime(monPol.getRemainingTime());

                Log.e(LOG_TAG, "Monitoring Policy: plmn: " + recvPolicy.getPlmn() +
                        " validityTime: " + recvPolicy.getValidityTime() + " remainingTime: " +
                        recvPolicy.getRemainingTime());

                monList.add(recvPolicy);
            }
            config.setMonitoringPolicyList(monList);
        }

        return config;
    }

    private LtedCategory getLtedCategoryFrom(RadioConfigInterface.LtedCategory data) {
        LtedCategory category = new LtedCategory();
        if (data == null) {
            return null;
        }

        Log.e(LOG_TAG, "data.getOsAppId() = " + data.getOsAppId());
        Log.e(LOG_TAG, "data.getCategory() = " + data.getCategory());
        category.setOsAppId(data.getOsAppId());
        category.setCategory(data.getCategory());

        return category;
    }



    private final IRadioConfig.Stub mBinder = new IRadioConfig.Stub() {

        public RadioConfigIntegerList setIntConfig(RadioConfigIntegerList config, int slotId) throws RemoteException{
            if (mSocket == null || mSocket[slotId] == null) {
                Log.e(LOG_TAG, "socket is not connected");
                return generateErrorResponse(config.getList(), RadioConfig.ErrorConstants.RADIO_CONFIG_INIT_FAILURE);
            }

            Log.i(LOG_TAG, "setIntConfig()");

            int token = getToken();

            // get tag;
            RadioConfigInterface.MessageTag tag = new RadioConfigInterface.MessageTag();
            tag.setToken(token);
            tag.setType(RadioConfigInterface.RADIO_CONFIG_MSG_REQUEST);
            tag.setId(RadioConfigInterface.RADIO_CONFIG_SET_CONFIG);
            tag.setError(RadioConfigInterface.RADIO_CONFIG_ERR_SUCCESS);

            // get request
            List<RadioConfigItem<Integer>> configList = config.getList();
            RadioConfigInterface.ConfigMsg configMsg = new RadioConfigInterface.ConfigMsg();

            for(RadioConfigItem<Integer> entry : configList) {

              RadioConfigInterface.ConfigItemMsg itemMsg = new RadioConfigInterface.ConfigItemMsg();
              itemMsg.setItem(mapRadioConfigItemToProtoItem(entry.getItem()));
              if(entry.hasValue()){
                  itemMsg.setIntValue(entry.getValue());
              }
              configMsg.addSettings(itemMsg);

              Log.i(LOG_TAG, "ConfigItem: " + entry.getItem() + "  Value: " + entry.getValue());
            }

            tag.setPayload(ByteStringMicro.copyFrom(
                                RadioConfigMsgPacker.packMsg( RadioConfigInterface.RADIO_CONFIG_SET_CONFIG,
                                                             RadioConfigInterface.RADIO_CONFIG_MSG_REQUEST,
                                                             configMsg) ));

            RadioConfigSyncRequest reqMsgObj = new RadioConfigSyncRequest(token, RadioConfigInterface.RADIO_CONFIG_SET_CONFIG);
            reqMsgObj.setReqBytes(RadioConfigMsgPacker.packTag(tag));
            Log.i(LOG_TAG, "packed information into a syncreqobj");
            reqTable.put(token, reqMsgObj);
            Log.i(LOG_TAG, "put the token and obj into the hashtable!");
            Message msg = mSocket[slotId].obtainMessage(EVENT_REQ, reqMsgObj);
            Log.i(LOG_TAG, "obtained msg");
            msg.sendToTarget();
            Log.i(LOG_TAG, "sent to target, will wait for resp; token: " + token);
            reqMsgObj.waitForResp();
            Log.i(LOG_TAG, "notified of response!! token: " + token);


            //processing the response
            byte[] resp = reqMsgObj.getRespBytes();
            if(resp == null){
                return generateErrorResponse(config.getList(), RadioConfig.ErrorConstants.RADIO_CONFIG_ERROR_GENERIC);
            }

            try {
                RadioConfigInterface.ConfigMsg configMsgResp = RadioConfigInterface.ConfigMsg.parseFrom(resp);
                List<RadioConfigInterface.ConfigItemMsg> settingsListResp = configMsgResp.getSettingsList();

                List<RadioConfigItem<Integer>> respList = new ArrayList<RadioConfigItem<Integer>>();
                RadioConfigItem<Integer> item;

                for(RadioConfigInterface.ConfigItemMsg entry : settingsListResp) {
                    item = new RadioConfigItem<Integer>();
                    //need to check has
                    item.setItem(entry.getItem());
                    item.setError(mapProtoErrortoRadioConfigError(entry.getErrorCause()));
                    respList.add(item);
                }
                //clean up the table
                reqTable.remove(token);

                RadioConfigIntegerList respObj = new RadioConfigIntegerList();
                respObj.setList(respList);
                return respObj;
            }catch( Exception e){
                e.printStackTrace();
                return generateErrorResponse(config.getList(), RadioConfig.ErrorConstants.RADIO_CONFIG_ERROR_GENERIC);
            }
        }

        public RadioConfigIntegerList getIntConfig(RadioConfigIntegerList config, int slotId) throws RemoteException{
            return null;
        }

        public RadioConfigStringList setStringConfig(RadioConfigStringList config, int slotId) throws RemoteException{
            return null;
        }
        public RadioConfigStringList getStringConfig(RadioConfigStringList config, int slotId) throws RemoteException{
            return null;
        }

        public int setParcelableConfig(int item, RadioConfigParcelable config, int slotId) throws RemoteException {
            int ret = RadioConfig.ErrorConstants.RADIO_CONFIG_ERROR_GENERIC;

            Log.d(LOG_TAG, "setParcelableConfig: slot id = " + slotId);
            if (slotId < 0 || slotId >= mNumPhones) {
                Log.e(LOG_TAG, "setParcelableConfig: Invalid slot id");
                return RadioConfig.ErrorConstants.RADIO_CONFIG_INVALID_SLOT_ID;
            }

            if (mSocket == null || mSocket[slotId] == null) {
                Log.e(LOG_TAG, "setParcelableConfig: socket is not connected");
                return RadioConfig.ErrorConstants.RADIO_CONFIG_INIT_FAILURE;
            }

            Log.d(LOG_TAG, "setParcelableConfig: item = " + item);

            switch (item) {
                case RadioConfig.ConfigConstants.CONFIG_ITEM_SET_LTED_CONFIG:
                    Log.d(LOG_TAG, "setParcelableConfig: CONFIG_ITEM_SET_LTED_CONFIG");
                    ret = setLtedConfig(item, config, slotId);
                    break;

                case RadioConfig.ConfigConstants.CONFIG_ITEM_SET_LTED_CATEGORY:
                    Log.d(LOG_TAG, "setParcelableConfig: CONFIG_ITEM_SET_LTED_CATEGORY");
                    ret = setLtedCategory(item, config, slotId);
                    break;

                default:
                    Log.e(LOG_TAG, "setParcelableConfig: Invalid item");
                    break;
            }

            Log.d(LOG_TAG, "setParcelableConfig: ret = " + ret);

            return ret;
        }

        public int getParcelableConfig(int item, RadioConfigParcelable extras, int slotId,
                RadioConfigParcelable config) throws RemoteException {
            int ret = RadioConfig.ErrorConstants.RADIO_CONFIG_ERROR_GENERIC;

            Log.d(LOG_TAG, "getParcelableConfig: slot id = " + slotId);
            if (slotId < 0 || slotId >= mNumPhones) {
                Log.e(LOG_TAG, "getParcelableConfig: Invalid slot id");
                return RadioConfig.ErrorConstants.RADIO_CONFIG_INVALID_SLOT_ID;
            }

            if (mSocket == null || mSocket[slotId] == null) {
                Log.e(LOG_TAG, "getParcelableConfig: socket is not connected");
                return RadioConfig.ErrorConstants.RADIO_CONFIG_INIT_FAILURE;
            }

            Log.d(LOG_TAG, "getParcelableConfig: item = " + item);

            switch (item) {
                case RadioConfig.ConfigConstants.CONFIG_ITEM_GET_LTED_CONFIG:
                    Log.e(LOG_TAG, "getParcelableConfig: CONFIG_ITEM_GET_LTED_CONFIG");
                    ret = getLtedConfig(item, slotId, config);
                    if (ret == RadioConfig.ErrorConstants.RADIO_CONFIG_SUCCESS) {
                        LtedConfig ltedConfig = (LtedConfig)config.getData();
                        Log.d(LOG_TAG, "getParcelableConfig: ltedConfig = " + ltedConfig);
                    }
                    break;

                case RadioConfig.ConfigConstants.CONFIG_ITEM_GET_LTED_CATEGORY:
                    Log.e(LOG_TAG, "getParcelableConfig: CONFIG_ITEM_GET_LTED_CATEGORY");
                    ret = getLtedCategory(item, extras, slotId, config);
                    if (ret == RadioConfig.ErrorConstants.RADIO_CONFIG_SUCCESS) {
                        LtedCategory result = (LtedCategory)config.getData();
                        Log.d(LOG_TAG, "getParcelableConfig: result = " + result);
                    }
                    break;

                default:
                    Log.e(LOG_TAG, "getParcelableConfig: Invalid item");
                    break;
            }

            Log.d(LOG_TAG, "getParcelableConfig: ret = " + ret);

            return ret;
        }

        public boolean registerLtedProvisioningListener(ILtedProvisioningListener listener) {
            Log.d(LOG_TAG, "registerLtedProvisioningListener");
            synchronized (mListeners) {
                if (mListeners.contains(listener)) {
                    return true;
                }
                return mListeners.add(listener);
            }
        }

        public boolean deregisterLtedProvisioningListener(ILtedProvisioningListener listener) {
            Log.d(LOG_TAG, "deregisterLtedProvisioningListener");
            synchronized (mListeners) {
                return mListeners.remove(listener);
            }
        }
    };

    int mapRadioConfigItemToProtoItem(int item) {
        int ret = RadioConfigInterface.CONFIG_ITEM_NONE;

        switch(item) {
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IN_CALL_LTE_RSRP_LOW:
            ret = RadioConfigInterface.CONFIG_ITEM_IN_CALL_LTE_RSRP_LOW;
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IN_CALL_LTE_RSRP_MID:
            ret = RadioConfigInterface.CONFIG_ITEM_IN_CALL_LTE_RSRP_MID;
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IN_CALL_LTE_RSRP_HIGH:
            ret = RadioConfigInterface.CONFIG_ITEM_IN_CALL_LTE_RSRP_HIGH;
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IN_CALL_WIFI_RSSI_THRESHOLD_LOW:
            ret = RadioConfigInterface.CONFIG_ITEM_IN_CALL_WIFI_RSSI_THRESHOLD_LOW;
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IN_CALL_WIFI_RSSI_THRESHOLD_HIGH:
            ret = RadioConfigInterface.CONFIG_ITEM_IN_CALL_WIFI_RSSI_THRESHOLD_HIGH;
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IN_CALL_WIFI_SINR_THRESHOLD_LOW:
            ret = RadioConfigInterface.CONFIG_ITEM_IN_CALL_WIFI_SINR_THRESHOLD_LOW;
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IN_CALL_WIFI_SINR_THRESHOLD_HIGH:
            ret = RadioConfigInterface.CONFIG_ITEM_IN_CALL_WIFI_SINR_THRESHOLD_HIGH;
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IDLE_LTE_RSRP_LOW:
            ret = RadioConfigInterface.CONFIG_ITEM_IDLE_LTE_RSRP_LOW;
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IDLE_LTE_RSRP_MID:
            ret = RadioConfigInterface.CONFIG_ITEM_IDLE_LTE_RSRP_MID;
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IDLE_LTE_RSRP_HIGH:
            ret = RadioConfigInterface.CONFIG_ITEM_IDLE_LTE_RSRP_HIGH;
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IDLE_WIFI_RSSI_THRESHOLD_LOW:
            ret = RadioConfigInterface.CONFIG_ITEM_IDLE_WIFI_RSSI_THRESHOLD_LOW;
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IDLE_WIFI_RSSI_THRESHOLD_HIGH:
            ret = RadioConfigInterface.CONFIG_ITEM_IDLE_WIFI_RSSI_THRESHOLD_HIGH;
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IDLE_WIFI_SINR_THRESHOLD_LOW:
            ret = RadioConfigInterface.CONFIG_ITEM_IDLE_WIFI_SINR_THRESHOLD_LOW;
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_IDLE_WIFI_SINR_THRESHOLD_HIGH:
            ret = RadioConfigInterface.CONFIG_ITEM_IDLE_WIFI_SINR_THRESHOLD_HIGH;
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_ECIO_1X_THRESHOLD_LOW:
            ret = RadioConfigInterface.CONFIG_ITEM_ECIO_1X_THRESHOLD_LOW;
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_ECIO_1X_THRESHOLD_HIGH:
            ret = RadioConfigInterface.CONFIG_ITEM_ECIO_1X_THRESHOLD_HIGH;
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_SET_LTED_CONFIG:
            ret = RadioConfigInterface.CONFIG_ITEM_SET_LTED_CONFIG;
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_GET_LTED_CONFIG:
            ret = RadioConfigInterface.CONFIG_ITEM_GET_LTED_CONFIG;
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_SET_LTED_CATEGORY:
            ret = RadioConfigInterface.CONFIG_ITEM_SET_LTED_CATEGORY;
            break;
        case RadioConfig.ConfigConstants.CONFIG_ITEM_GET_LTED_CATEGORY:
            ret = RadioConfigInterface.CONFIG_ITEM_GET_LTED_CATEGORY;
            break;
        default:
            ret = RadioConfigInterface.CONFIG_ITEM_NONE;
            break;
        }
        return ret;
    }

    int mapProtoItemToRadioConfigItem(int item) {

        int ret = RadioConfig.ConfigConstants.CONFIG_UNKNOWN;

        switch(item) {
        case RadioConfigInterface.CONFIG_ITEM_IN_CALL_LTE_RSRP_LOW:
            ret = RadioConfig.ConfigConstants.CONFIG_ITEM_IN_CALL_LTE_RSRP_LOW;
            break;
        case RadioConfigInterface.CONFIG_ITEM_IN_CALL_LTE_RSRP_MID:
            ret = RadioConfig.ConfigConstants.CONFIG_ITEM_IN_CALL_LTE_RSRP_MID;
            break;
        case RadioConfigInterface.CONFIG_ITEM_IN_CALL_LTE_RSRP_HIGH:
            ret = RadioConfig.ConfigConstants.CONFIG_ITEM_IN_CALL_LTE_RSRP_HIGH;
            break;
        case RadioConfigInterface.CONFIG_ITEM_IN_CALL_WIFI_RSSI_THRESHOLD_LOW:
            ret = RadioConfig.ConfigConstants.CONFIG_ITEM_IN_CALL_WIFI_RSSI_THRESHOLD_LOW;
            break;
        case RadioConfigInterface.CONFIG_ITEM_IN_CALL_WIFI_RSSI_THRESHOLD_HIGH:
            ret = RadioConfig.ConfigConstants.CONFIG_ITEM_IN_CALL_WIFI_RSSI_THRESHOLD_HIGH;
            break;
        case RadioConfigInterface.CONFIG_ITEM_IN_CALL_WIFI_SINR_THRESHOLD_LOW:
            ret = RadioConfig.ConfigConstants.CONFIG_ITEM_IN_CALL_WIFI_SINR_THRESHOLD_LOW;
            break;
        case RadioConfigInterface.CONFIG_ITEM_IN_CALL_WIFI_SINR_THRESHOLD_HIGH:
            ret = RadioConfig.ConfigConstants.CONFIG_ITEM_IN_CALL_WIFI_SINR_THRESHOLD_HIGH;
            break;
        case RadioConfigInterface.CONFIG_ITEM_IDLE_LTE_RSRP_LOW:
            ret = RadioConfig.ConfigConstants.CONFIG_ITEM_IDLE_LTE_RSRP_LOW;
            break;
        case RadioConfigInterface.CONFIG_ITEM_IDLE_LTE_RSRP_MID:
            ret = RadioConfig.ConfigConstants.CONFIG_ITEM_IDLE_LTE_RSRP_MID;
            break;
        case RadioConfigInterface.CONFIG_ITEM_IDLE_LTE_RSRP_HIGH:
            ret = RadioConfig.ConfigConstants.CONFIG_ITEM_IDLE_LTE_RSRP_HIGH;
            break;
        case RadioConfigInterface.CONFIG_ITEM_IDLE_WIFI_RSSI_THRESHOLD_LOW:
            ret = RadioConfig.ConfigConstants.CONFIG_ITEM_IDLE_WIFI_RSSI_THRESHOLD_LOW;
            break;
        case RadioConfigInterface.CONFIG_ITEM_IDLE_WIFI_RSSI_THRESHOLD_HIGH:
            ret = RadioConfig.ConfigConstants.CONFIG_ITEM_IDLE_WIFI_RSSI_THRESHOLD_HIGH;
            break;
        case RadioConfigInterface.CONFIG_ITEM_IDLE_WIFI_SINR_THRESHOLD_LOW:
            ret = RadioConfig.ConfigConstants.CONFIG_ITEM_IDLE_WIFI_SINR_THRESHOLD_LOW;
            break;
        case RadioConfigInterface.CONFIG_ITEM_IDLE_WIFI_SINR_THRESHOLD_HIGH:
            ret = RadioConfig.ConfigConstants.CONFIG_ITEM_IDLE_WIFI_SINR_THRESHOLD_HIGH;
            break;
        case RadioConfigInterface.CONFIG_ITEM_ECIO_1X_THRESHOLD_LOW:
            ret = RadioConfig.ConfigConstants.CONFIG_ITEM_ECIO_1X_THRESHOLD_LOW;
            break;
        case RadioConfigInterface.CONFIG_ITEM_ECIO_1X_THRESHOLD_HIGH:
            ret = RadioConfig.ConfigConstants.CONFIG_ITEM_ECIO_1X_THRESHOLD_HIGH;
            break;
        case RadioConfigInterface.CONFIG_ITEM_SET_LTED_CONFIG:
            ret = RadioConfig.ConfigConstants.CONFIG_ITEM_SET_LTED_CONFIG;
            break;
        case RadioConfigInterface.CONFIG_ITEM_GET_LTED_CONFIG:
            ret = RadioConfig.ConfigConstants.CONFIG_ITEM_GET_LTED_CONFIG;
            break;
        case RadioConfigInterface.CONFIG_ITEM_SET_LTED_CATEGORY:
            ret = RadioConfig.ConfigConstants.CONFIG_ITEM_SET_LTED_CATEGORY;
            break;
        case RadioConfigInterface.CONFIG_ITEM_GET_LTED_CATEGORY:
            ret = RadioConfig.ConfigConstants.CONFIG_ITEM_GET_LTED_CATEGORY;
            break;
        default:
            ret = RadioConfig.ConfigConstants.CONFIG_UNKNOWN;
            break;
        }
        return ret;
    }

    int mapProtoErrortoRadioConfigError(int error){
        int ret = RadioConfig.ErrorConstants.RADIO_CONFIG_ERROR_GENERIC;

        switch(error){
        case RadioConfigInterface.CONFIG_NO_ERR:
            ret = RadioConfig.ErrorConstants.RADIO_CONFIG_SUCCESS;
            break;
        case RadioConfigInterface.CONFIG_GENERIC_ERR:
            ret = RadioConfig.ErrorConstants.RADIO_CONFIG_ERROR_GENERIC;
            break;
        }
        return ret;
    }
}
