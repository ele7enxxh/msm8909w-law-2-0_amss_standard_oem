/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package org.codeaurora.ims;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.android.ims.ImsException;
import com.android.ims.ImsManager;
import com.android.ims.ImsReasonInfo;
import com.android.ims.ImsServiceClass;
import com.android.ims.ImsCallProfile;
import com.android.ims.internal.IImsCallSession;
import com.android.ims.internal.IImsCallSessionListener;
import com.android.ims.internal.IImsRegistrationListener;
import com.android.ims.internal.IImsEcbm;
import com.android.ims.internal.IImsMultiEndpoint;
import com.android.ims.internal.IImsService;
import com.android.ims.internal.IImsUt;
import com.android.ims.internal.IImsConfig;
import com.android.ims.internal.ImsCallSession;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.TelephonyIntents;
import com.qualcomm.ims.vt.ImsVideoGlobals;

import org.codeaurora.ims.QtiImsExtManager;
import com.qualcomm.ims.utils.Log;

import org.codeaurora.ims.utils.QtiImsExtUtils;

import android.Manifest;
import android.app.PendingIntent;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.ServiceManager;
import android.telephony.ServiceState;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;

public class ImsService extends Service {
    private static final int MAX_SUBSCRIPTIONS = 1;
    private ImsServiceSub mServiceSub[];
    // service id --> table
    private Map<Integer, ImsServiceSub> mServiceSubMap = new HashMap<Integer, ImsServiceSub>();

    private final String ACCESS_IMS_CALL_SERVICE = Manifest.permission.ACCESS_IMS_CALL_SERVICE;
    private final String MODIFY_PHONE_STATE = Manifest.permission.MODIFY_PHONE_STATE;
    private final String READ_PRIVILEGED_PHONE_STATE = Manifest.permission.READ_PRIVILEGED_PHONE_STATE;
    private final String READ_PHONE_STATE = Manifest.permission.READ_PHONE_STATE;

    private static final int INVALID_PHONE_ID = -1;
    private static final int DEFAULT_PHONE_ID = 0;
    protected int mImsPhoneId = INVALID_PHONE_ID;
    protected int mNumMultiModeStacks = 0;
    protected boolean mIsReceiverRegistered = false;
    private static final String ACTION_GET_RADIO_ACCESS_FAMILY_CHANGED
            = "org.codeaurora.intent.action.ACTION_RADIO_CAPABILITY_UPDATED";

    /**
     * Utility for getting number of subscriptions
     * @return int containing maximum number
     */
    private int getNumSubscriptions() {
        return MAX_SUBSCRIPTIONS; //One for now - plugin with msim util later
    }

    private void enforceReadPhoneState(String fn) {
        if (checkCallingOrSelfPermission(READ_PRIVILEGED_PHONE_STATE)
                != PackageManager.PERMISSION_GRANTED
                   && checkCallingOrSelfPermission(READ_PHONE_STATE)
                != PackageManager.PERMISSION_GRANTED) {
            enforceCallingOrSelfPermission(READ_PHONE_STATE, fn);
        }
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i (this, "ImsService created!");
        mServiceSub = new ImsServiceSub[getNumSubscriptions()];
        for (int i = 0; i < getNumSubscriptions(); i++) {
            mServiceSub[i] = new ImsServiceSub(i + 1, this);
        }
        ServiceManager.addService("ims", mBinder);

        // Adding QtiImsExtBinder to ServiceManager
        // For now using DEFAULT_PHONE_ID will migrate once mServiceSub
        // has multiple instances.
        ServiceManager.addService(QtiImsExtManager.SERVICE_ID,
                mServiceSub[DEFAULT_PHONE_ID].getQtiImsExtBinder());

        switchImsPhoneByPhoneId(DEFAULT_PHONE_ID);

        final int defaultSub = 1;
        ImsVideoGlobals.init(mServiceSub[defaultSub-1], this);

        /* Check if any change in socket communication is required */
        initSubscriptionStatus();

        /* Initialize Call Deflect support to not supported */
        initCallDeflectStatus();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onDestroy() {
        Log.i(this, "Ims Service Destroyed Successfully...");
        for (int i = 0; i < getNumSubscriptions(); i++) {
            mServiceSub[i].dispose();
        }
        super.onDestroy();
    }

    /* Receiver to handle RAF and DDS change event */
    private BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            Log.i(this, "mBroadcastReceiver - " + action);
            if (action.equals(TelephonyIntents.ACTION_DEFAULT_DATA_SUBSCRIPTION_CHANGED)) {
                int ddsSubId = intent.getIntExtra(PhoneConstants.SUBSCRIPTION_KEY,
                        SubscriptionManager.INVALID_SUBSCRIPTION_ID);
                Log.i(this, "got ACTION_DEFAULT_DATA_SUBSCRIPTION_CHANGED, new DDS = "
                        + ddsSubId);
                switchImsPhoneBySubId(ddsSubId);
            } else if (action.equals(ACTION_GET_RADIO_ACCESS_FAMILY_CHANGED)) {
                handleRafInfoChange();
            }
        }
    };

    /* Method to initialize call deflect status */
    private void initCallDeflectStatus() {
        android.provider.Settings.Global.putInt(
                  this.getContentResolver(),
                  QtiImsExtUtils.QTI_IMS_DEFLECT_ENABLED, 0);
    }

    /* Method to initialize the Phone Id */
    private void initSubscriptionStatus() {
        if (TelephonyManager.getDefault().isMultiSimEnabled()) {
            Log.i(this, "initSubscriptionStatus: multi-sim...");
            handleRafInfoChange();

            /* Check if Radio Access Family event change is to be registered */
            if (mNumMultiModeStacks == 0) {
                Log.i(this, "initSubscriptionStatus: registered for RAF info");

                /* Start listening to the RAF change event. */
                this.registerReceiver(mBroadcastReceiver,
                        new IntentFilter(ACTION_GET_RADIO_ACCESS_FAMILY_CHANGED));
                mIsReceiverRegistered = true;
            }
        } else {
            /* If not multi-sim, a change in socket communication is not required */
            Log.i(this, "initSubscriptionStatus: Not multi-sim...");
        }
    }

    /* Method to re-initialize the Ims Phone instances */
    private void switchImsPhoneByPhoneId(int phoneId) {
        if (!SubscriptionManager.isValidPhoneId(phoneId)) {
            Log.e(this, "switschImsPhone: Invalid phoneId: " + phoneId);
            return;
        }

        if (mImsPhoneId == phoneId) {
            Log.i(this, "switchImsPhone: ImsPhoneId: " + mImsPhoneId + " UNchanged");
            return;
        }

        Log.i(this, "switchImsPhone: ImsPhoneId:" + mImsPhoneId + " changed to " + phoneId);
        mImsPhoneId = phoneId;

        /* Change the socket communication */
        final int defaultSub = 1;
        mServiceSub[defaultSub-1].registerForPhoneId(mImsPhoneId);

        /* Shut-down the existing ims phone */
        int mNumPhones = TelephonyManager.getDefault().getPhoneCount();
        for (int i = 0; i < mNumPhones; i++) {
            if(mImsPhoneId != i) {
                Intent intent = new Intent(ImsManager.ACTION_IMS_SERVICE_DOWN);
                intent.addFlags(Intent.FLAG_RECEIVER_REPLACE_PENDING);
                intent.putExtra(ImsManager.EXTRA_PHONE_ID, i);
                this.sendBroadcast(new Intent(intent));
            }
        }

        /* Create the new IMS phone */
        Intent intent = new Intent(ImsManager.ACTION_IMS_SERVICE_UP);
        intent.addFlags(Intent.FLAG_RECEIVER_REPLACE_PENDING);
        intent.putExtra(ImsManager.EXTRA_PHONE_ID, mImsPhoneId);
        this.sendBroadcast(new Intent(intent));
    }

    /* Method to validate a subId and then check for IMSPhonechange */
    private void switchImsPhoneBySubId(int ddsSubId) {
        int phoneId = 0; /* By default, always assume dds as default phone ID */

        /*
         * If the subscription ID received via intent is valid,
         * then retrieve the corresponding phone ID.
         */
        if(SubscriptionManager.isValidSubscriptionId(ddsSubId)) {
            phoneId = SubscriptionManager.getPhoneId(ddsSubId);
        } else {
            /* Recheck the subId via API */
            ddsSubId = SubscriptionManager.getDefaultDataSubscriptionId();
            /* If the subscription ID is Valid, then retrive corresponding phone ID */
            if(SubscriptionManager.isValidSubscriptionId(ddsSubId)) {
                phoneId = SubscriptionManager.getPhoneId(ddsSubId);
            }
        }

        Log.i(this, "switchImsPhoneBySubId: new DDS = " + ddsSubId);
        switchImsPhoneByPhoneId(phoneId);
    }

    /**
     * Method to identify the RAT mask for Multimode support.
     * Checks for
     *          - LTE to support IMS
     *          - UMTS to support CSVT.
     */
    private boolean isMultiModeSupported(int nRatMask) {
        final int nMmMask = (1 << ServiceState.RIL_RADIO_TECHNOLOGY_LTE);
        return (nRatMask & nMmMask) != 0;
    }

    /* Method to handle Radio Acess Family change event */
    private void handleRafInfoChange() {
        /* If registered for DDS change event, ignore checking for RAF change event */
        if (mNumMultiModeStacks > 0) {
            Log.i(this, "handleRafInfoChange: " + mNumMultiModeStacks +
                    "Multimode stacks. Handled by DDS switch. EXIT!!!");
            return;
        }

        /* Check for the number of multi-mode stacks */
        int mNumPhones = TelephonyManager.getDefault().getPhoneCount();
        int nTempPhoneId = 0; /* Always assume default phone ID as 0 */
        for (int i = 0; i < mNumPhones; i++) {
            Phone phone = PhoneFactory.getPhone(i);
            if (phone != null && phone.getRadioCapability() != null) {
                int nRafInfo = phone.getRadioCapability().getRadioAccessFamily();
                Log.i(this, "handleRafInfoChange: Phone:" + i + " Info:" + nRafInfo);
                if (isMultiModeSupported(phone.getRadioCapability().getRadioAccessFamily())) {
                    mNumMultiModeStacks++;
                    nTempPhoneId = i;
                }
            }
        }
        Log.i(this, "handleRafInfoChange: NumPhones: " + mNumPhones +
                "Multiple Multimode stacks: " + mNumMultiModeStacks);

        /*
         * If more than 1 Multimode stack is present, then listen to DDS change event
         * else, continue to listen for Radio Access Family change event.
         */
        if (mNumMultiModeStacks > 1) {

            /* Unregister the earlier receiver, if any */
            if (mIsReceiverRegistered) {
                this.unregisterReceiver(mBroadcastReceiver);
            }

            /*
             * To avoid the scenarios where DDS change event is not broadcasted yet,
             * by default check the data subscription information once.
             */
            switchImsPhoneBySubId(SubscriptionManager.INVALID_SUBSCRIPTION_ID);

            /* Start listening to the DDS change event. */
            this.registerReceiver(mBroadcastReceiver,
                    new IntentFilter(TelephonyIntents.ACTION_DEFAULT_DATA_SUBSCRIPTION_CHANGED));
            mIsReceiverRegistered = true;
            Log.i(this, "handleRafInfoChange: registered for DDS switch...");

        } else {
            mNumMultiModeStacks = 0;
            switchImsPhoneByPhoneId(nTempPhoneId);
        }
    }

    /*
     * Implement the methods of the IImsService interface in this stub
     */
    private final IImsService.Stub mBinder = new IImsService.Stub() {

        /**
         * Opens the IMS service for making calls and/or receiving generic IMS calls.
         * The caller may make subsequent calls through {@link #makeCall}.
         * The IMS service will register the device to the operator's network with the credentials
         * (from ISIM) periodically in order to receive calls from the operator's network.
         * When the IMS service receives a new call, it will send out an intent with
         * the provided action string.
         * The intent contains a call ID extra {@link getCallId} and it can be used to take a call.
         *
         * @param serviceClass a service class specified in {@link ImsServiceClass}
         *      For VoLTE service, it MUST be a {@link ImsServiceClass#MMTEL}.
         * @param incomingCallPendingIntent When an incoming call is received,
         *        the IMS service will call {@link PendingIntent#send(Context, int, Intent)} to
         *        send back the intent to the caller with {@link #INCOMING_CALL_RESULT_CODE}
         *        as the result code and the intent to fill in the call ID; It cannot be null
         * @param listener To listen to IMS registration events; It cannot be null
         * @return identifier (greater than 0) for the specified service
         * @see #getCallId
         * @see #getServiceId
         */
        public int open(int phoneId, int serviceClass, PendingIntent incomingCallIntent,
                IImsRegistrationListener listener) {
            //In 1x+VoLTE case, open can be called with a different phoneId after phone switch
            if (phoneId != mImsPhoneId) {
                //Ensure listeners associated with VoLTE Sub are not overridden. So just return.
                return 0;
            }
            enforceCallingOrSelfPermission(ACCESS_IMS_CALL_SERVICE, "open");
            return openForSub(1, serviceClass, incomingCallIntent, listener);
        }

        /**
         * Opens the IMS service for making calls and/or receiving generic IMS calls.
         * The caller may make subsequent calls through {@link #makeCall}.
         * The IMS service will register the device to the operator's network with the credentials
         * (from ISIM) periodically in order to receive calls from the operator's network.
         * When the IMS service receives a new call, it will send out an intent with
         * the provided action string.
         * The intent contains a call ID extra {@link getCallId} and it can be used to take a call.
         *
         * @param subscription The SIM subscription for multi-sim scenarios
         * @param serviceClass a service class specified in {@link ImsServiceClass}
         *      For VoLTE service, it MUST be a {@link ImsServiceClass#MMTEL}.
         * @param incomingCallPendingIntent When an incoming call is received,
         *        the IMS service will call {@link PendingIntent#send(Context, int, Intent)} to
         *        send back the intent to the caller with {@link #INCOMING_CALL_RESULT_CODE}
         *        as the result code and the intent to fill in the call ID; It cannot be null
         * @param listener To listen to IMS registration events; It cannot be null
         * @return identifier (greater than 0) for the specified service
         * @see #getCallId
         * @see #getServiceId
         */
        public int openForSub(int subscription, int serviceClass, PendingIntent incomingCallIntent,
                IImsRegistrationListener listener) {
            //TODO: Check valid subscription using framework hooks for multi sim
            int serviceId = mServiceSub[subscription - 1].getServiceId(serviceClass,
                    incomingCallIntent,
                    listener);
            if (serviceId > 0) {
                mServiceSubMap.put(new Integer(serviceId), mServiceSub[subscription - 1]);
            }
            // TODO: (ims-vt) This needds to run on main thread.
            ImsVideoGlobals.getInstance().setActiveSub(mServiceSub[subscription - 1]);
            mServiceSub[subscription - 1].setPhoneId(mImsPhoneId);
            Log.i (this, "Open returns serviceId " + serviceId);
            return serviceId;
        }

        /**
         * Closes the specified service ({@link ImsServiceClass}) not to make/receive calls.
         * All the resources that were allocated to the service are also released.
         *
         * @param serviceId a service id to be closed which is obtained from {@link ImsManager#open}
         */
        public void close(int serviceId) {
            enforceCallingOrSelfPermission(ACCESS_IMS_CALL_SERVICE, "close");
            //TODO
        }

        /**
         * Checks if the IMS service has successfully registered to the IMS network
         * with the specified service & call type.
         *
         * @param serviceId a service id which is obtained from {@link ImsManager#open}
         * @param serviceType a service type that is specified in {@link ImsCallProfile}
         *        {@link ImsCallProfile#SERVICE_TYPE_NORMAL}
         *        {@link ImsCallProfile#SERVICE_TYPE_EMERGENCY}
         * @param callType a call type that is specified in {@link ImsCallProfile}
         *        {@link ImsCallProfile#CALL_TYPE_VOLTE_N_VIDEO}
         *        {@link ImsCallProfile#CALL_TYPE_VOLTE}
         *        {@link ImsCallProfile#CALL_TYPE_VT}
         *        {@link ImsCallProfile#CALL_TYPE_VS}
         * @return true if the specified service id is connected to the IMS network;
         *        false otherwise
         */
        public boolean isConnected(int serviceId, int serviceType, int callType) {
            enforceReadPhoneState("isConnected");
            return true; //TODO:
        }

        /**
         * Checks if the specified IMS service is opened.
         *
         * @param serviceId a service id which is obtained from {@link ImsManager#open}
         * @return true if the specified service id is opened; false otherwise
         */
        public boolean isOpened(int serviceId) {
            enforceReadPhoneState("isOpened");
            return true; //TODO:
        }

        /**
         * Set the registration listener for the client associated with service id
         * @param serviceId - service ID obtained through open
         * @param listener - registration listener
         */
        public void setRegistrationListener(int serviceId, IImsRegistrationListener listener) {
            enforceReadPhoneState("setRegistrationListener");
            ImsServiceSub service = mServiceSubMap.get(new Integer(serviceId));
            if (service == null) {
                Log.e (this, "Invalid ServiceId ");
                return;
            }
            service.setRegistrationListener(serviceId, listener);
        }

        /**
         * Creates a {@link ImsCallProfile} from the service capabilities & IMS registration state.
         *
         * @param serviceId a service id which is obtained from {@link ImsManager#open}
         * @param serviceType a service type that is specified in {@link ImsCallProfile}
         *        {@link ImsCallProfile#SERVICE_TYPE_NONE}
         *        {@link ImsCallProfile#SERVICE_TYPE_NORMAL}
         *        {@link ImsCallProfile#SERVICE_TYPE_EMERGENCY}
         * @param callType a call type that is specified in {@link ImsCallProfile}
         *        {@link ImsCallProfile#CALL_TYPE_VOLTE}
         *        {@link ImsCallProfile#CALL_TYPE_VT}
         *        {@link ImsCallProfile#CALL_TYPE_VT_TX}
         *        {@link ImsCallProfile#CALL_TYPE_VT_RX}
         *        {@link ImsCallProfile#CALL_TYPE_VT_NODIR}
         *        {@link ImsCallProfile#CALL_TYPE_VS}
         *        {@link ImsCallProfile#CALL_TYPE_VS_TX}
         *        {@link ImsCallProfile#CALL_TYPE_VS_RX}
         * @return a {@link ImsCallProfile} object
         */
        public ImsCallProfile createCallProfile(int serviceId, int serviceType, int callType) {
            enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "createCallProfile");
            ImsServiceSub service = mServiceSubMap.get(new Integer(serviceId));
            if (service == null) {
                Log.e (this, "Invalid ServiceId ");
                return null;
            }
            return service.createCallProfile(serviceId, serviceType, callType);
        }

        /**
         * Creates a {@link ImsCallSession} with the specified call profile.
         * Use other methods, if applicable, instead of interacting with
         * {@link ImsCallSession} directly.
         *
         * @param serviceId a service id which is obtained from {@link ImsManager#open}
         * @param profile a call profile to make the call
         */
        public IImsCallSession createCallSession(int serviceId, ImsCallProfile profile,
                IImsCallSessionListener listener) {
            enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "createCallSession");
            ImsServiceSub service = mServiceSubMap.get(new Integer(serviceId));
            if (service == null) {
                Log.e (this, "Invalid ServiceId ");
                return null;
            }
            return service.createCallSession(serviceId, profile, listener);
        }

        /**
         * Retrieves the call session associated with a pending call.
         *
         * @param serviceId a service id which is obtained from {@link ImsManager#open}
         * @param profile a call profile to make the call
         */
        public IImsCallSession getPendingCallSession(int serviceId, String callId) {
            enforceReadPhoneState("getPendingCallSession");
            ImsServiceSub service = mServiceSubMap.get(new Integer(serviceId));
            if (service == null || callId == null) {
                Log.e (this, "Invalid arguments " + service + " " + callId);
                return null;
            }
            return service.getPendingSession(serviceId, callId);
        }

        /**
         * Ut interface for the supplementary service configuration.
         */
        public IImsUt getUtInterface(int serviceId) {
            enforceReadPhoneState("getUtInterface");
            ImsServiceSub service = mServiceSubMap.get(new Integer(serviceId));
            if (service == null) {
                Log.e (this, "Invalid argument " + service);
                return null;
            }
            return service.getUtInterface();
        }

        /**
        * Config interface for IMS Configuration
        */
        public IImsConfig getConfigInterface(int phoneId) {
            enforceReadPhoneState("getConfigInterface");
            int default_subscription = 1;
            return mServiceSub[default_subscription - 1].getConfigInterface();
        }

        /**
         * Add a new registration listener
         * @param serviceClass - service class
         * @param listener - registration listener
         */
        public void addRegistrationListener(int phoneId, int serviceClass,
                       IImsRegistrationListener listener) {
            enforceReadPhoneState("addRegistrationListener");
            int default_subscription = 1;
            mServiceSub[default_subscription - 1].addRegistrationListener(serviceClass, listener);
        }

        /**
         * Removes an existing registration listener
         * @param serviceClass - service class
         * @param listener - registration listener
         */
        public void removeRegistrationListener(int phoneId, int serviceClass,
                       IImsRegistrationListener listener) {
            enforceReadPhoneState("removeRegistrationListener");
            int default_subscription = 1;
            mServiceSub[default_subscription - 1].removeRegistrationListener(serviceClass,
                listener);
        }

        /**
         * Used for turning on IMS when its in OFF state.
         */
        public void turnOnIms(int phoneId) {
            enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "turnOnIms");
            int default_subscription = 1;
            mServiceSub[default_subscription - 1].turnOnIms();
        }

        /**
         * Used for turning off IMS when its in ON state. When IMS is OFF, device will behave as
         * CSFB'ed.
         */
        public void turnOffIms(int phoneId) {
            enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "turnOffIms");
            int default_subscription = 1;
            mServiceSub[default_subscription - 1].turnOffIms();
        }

        /**
         * ECBM interface for Emergency callback notifications
         */
        public IImsEcbm getEcbmInterface(int serviceId) {
            enforceReadPhoneState("getEcbmInterface");
            ImsServiceSub service = mServiceSubMap.get(new Integer(serviceId));
            if (service == null) {
                Log.e(this, "getEcbmInterface: Invalid argument " + service);
                return null;
            }
            return service.getEcbmInterface();
        }

        public void setUiTTYMode(int serviceId, int uiTtyMode, Message onComplete) {
            enforceCallingOrSelfPermission(MODIFY_PHONE_STATE, "setUiTTYMode");
            ImsServiceSub service = mServiceSubMap.get(new Integer(serviceId));
            if (service == null) {
                Log.e (this, "Invalid arguments " + serviceId);
                return;
            }
            service.setUiTTYMode(serviceId, uiTtyMode, onComplete);
        }

        public IImsMultiEndpoint getMultiEndpointInterface(int serviceId) {
            return null;
        }

    };
}
