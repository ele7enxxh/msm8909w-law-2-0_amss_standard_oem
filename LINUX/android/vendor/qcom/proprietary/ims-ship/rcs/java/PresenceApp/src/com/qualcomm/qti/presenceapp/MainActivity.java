/*************************************************************************
 Copyright (c)2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
*************************************************************************/

package com.qualcomm.qti.presenceapp;

import android.opengl.Visibility;
import android.os.Bundle;
import android.view.Menu;
import android.telephony.TelephonyManager;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;
import com.qualcomm.qti.presenceapp.R;
import com.qualcomm.qti.rcsimssettings.IQrcsImsSettingsListener;
import com.qualcomm.qti.rcsimssettings.IQrcsImsSettingsService;
import com.qualcomm.qti.rcsimssettings.QrcsImsSettingsError;
import com.qualcomm.qti.rcsimssettings.QrcsImsSettingsIndId;
import com.qualcomm.qti.rcsimssettings.QrcsImsSettingsIntType;
import com.qualcomm.qti.rcsimssettings.QrcsImsSettingsPresenceConfig;
import com.qualcomm.qti.rcsimssettings.QrcsImsSettingsPresenceConfigResp;
import com.qualcomm.qti.rcsimssettings.QrcsImsSettingsQipcallConfigResp;
import com.qualcomm.qti.rcsimssettings.QrcsImsSettingsRespId;
import com.qualcomm.qti.rcsimssettings.QrcsImsSettingsQipcallConfig;
import com.android.ims.internal.uce.uceservice.IUceService;
import com.android.ims.internal.uce.uceservice.ImsUceManager;
import com.android.ims.internal.uce.presence.IPresenceService;
import com.android.ims.internal.uce.options.IOptionsService;
import com.android.ims.internal.uce.common.StatusCode;
import com.qualcomm.qti.rcsimssettings.QrcsImsSettingsConfigInd;
import com.android.ims.internal.uce.common.CapInfo;


import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ListActivity;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.SharedPreferences;
import android.database.Cursor;
import android.net.ConnectivityManager;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.Looper;
import android.os.RemoteException;
import android.provider.ContactsContract;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import android.content.pm.ResolveInfo;
import android.content.pm.ServiceInfo;
import android.content.pm.PackageManager;
import android.content.res.Configuration;

public class MainActivity extends ListActivity {
    private final static String TAG = "MainActivity";

    public Context mContext;
    ArrayList<Contact> mContacts;

    ContactArrayAdapter<Contact> mAdapter;
    ListenerHandler mListenerHandler;

    AsyncTask mPublishTask;
    AsyncTask mSubscribePollingTask;
    AsyncTask mImsEnablerTask;
    AsyncTask mUnPublishTask;

    public static IQrcsImsSettingsService imsSettingService = null;
    public static MainActivity mainActivityObject;
    public static QrcsImsSettingsQipcallConfig qipcall_config = new QrcsImsSettingsQipcallConfig();
    public static QrcsImsSettingsPresenceConfig presence_config = new QrcsImsSettingsPresenceConfig();
    public static int AppStart = 1;
    public static int NewLaunch = 1;

    private static boolean isRCSServBound = false;
    private static boolean isIMSServBound = false;
    public static boolean appAbnormalClose = true;

    private BroadcastReceiver receiver;
    public  DataBroadcastReceiver dataBroadcastReceiver =  new DataBroadcastReceiver();

    public class ContactArrayAdapter<T> extends ArrayAdapter<T> {

        int layoutRes;
        ArrayList<Contact> contacts;

        public ContactArrayAdapter(Context context, int resource,
                int textViewResourceId, List<T> objects) {
            super(context, resource, textViewResourceId, objects);
            layoutRes = resource;
            contacts = (ArrayList<Contact>) objects;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            LinearLayout rowView;

            if (convertView == null) {
                rowView = new LinearLayout(getContext());

                String inflater = Context.LAYOUT_INFLATER_SERVICE;
                LayoutInflater vi = (LayoutInflater) getContext()
                        .getSystemService(inflater);
                vi.inflate(layoutRes, rowView, true);
            } else {
                rowView = (LinearLayout) convertView;
            }

            TextView nameView = (TextView) rowView.findViewById(R.id.name);
            TextView phoneView = (TextView) rowView.findViewById(R.id.phone);
            TextView noteView = (TextView) rowView.findViewById(R.id.Note);
            ImageView bmp = (ImageView) rowView.findViewById(R.id.icon);
            ImageView isMultiSelectedIcon = (ImageView) rowView
                    .findViewById(R.id.is_selected_icon);
            ImageView isExcludedIcon = (ImageView) rowView
                    .findViewById(R.id.is_excluded_icon);

            nameView.setText(contacts.get(position).getName());
            phoneView.setText(contacts.get(position).getPhone());
            noteView.setText(contacts.get(position).getNote());

            updateIcon(bmp, position);
            updateExcludedIcon(isExcludedIcon, false);
            if (contacts.get(position).isContactExcluded()) {
                updateExcludedIcon(isExcludedIcon, true);
                updateMultiSelectedIcon(isMultiSelectedIcon, false);

            } else {
                updateMultiSelectedIcon(isMultiSelectedIcon,
                        contacts.get(position).isMultiSelected());
            }
            return rowView;
        }

        private void updateExcludedIcon(ImageView isExcludedIcon,
                boolean visible) {
            if (visible) {
                isExcludedIcon.setVisibility(View.VISIBLE);
            } else {
                isExcludedIcon.setVisibility(View.INVISIBLE);
            }
        }

        private void updateMultiSelectedIcon(ImageView icon,
                boolean multiSelected) {
            if (multiSelected) {
                icon.setVisibility(View.VISIBLE);
            } else {
                icon.setVisibility(View.INVISIBLE);
            }
        }

        private void updateIcon(ImageView bmp, int position) {
            switch (contacts.get(position).getAvailability()) {
                case 0:
                    bmp.setImageResource(getResources().getIdentifier("icon",
                            "drawable", getPackageName()));
                    break;

                case 1:
                    bmp.setImageResource(getResources().getIdentifier("online",
                            "drawable", getPackageName()));
                    break;

                case 2:
                    bmp.setImageResource(getResources().getIdentifier("busy",
                            "drawable", getPackageName()));
                    break;

                default:
                    bmp.setImageResource(getResources().getIdentifier("icon",
                            "drawable", getPackageName()));
            }
        }
    }

    private SharedPreferences getSharedPrefHandle(String imsPresencePref) {
        SharedPreferences settings = getSharedPreferences(imsPresencePref, 0);

        return settings;
    }

    @Override
    protected void onRestart() {
        Log.d(TAG, "MainActivity onRestart hit");
        super.onRestart();
    }

    @Override
    protected void onResume() {
        Log.d(TAG, "MainActivity onResume hit");
        super.onResume();
    }

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mContext = this;
        mainActivityObject = this;
        appAbnormalClose = true;

        Utility.initLiveLogging(mContext);
        Utility.initNotifyXMLFile(mContext);

        Log.d(TAG, "MainActivity onCreate hit");

        try
        {
            //adb shell setprop persist.OPERATOR_MODE 1
            Process proc = null;
            proc = Runtime.getRuntime().exec(new String[] {
                    "/system/bin/getprop", "persist.OPERATOR_MODE"
            });
            BufferedReader reader = new BufferedReader(new InputStreamReader(proc.getInputStream()));
            Log.d("PRESENCE_UI", "before my prop is:");
            String sMode = reader.readLine();
            Log.d("PRESENCE_UI", "my prop is: " + sMode);

            if(sMode == "")
            {
              sMode = "0";
            }

            if (sMode != null)
            {
                Log.d("PRESENCE_UI", "LauncherrActivity : OnCreate : sMode " + sMode);
                AppGlobalState.setOperatorMode(Integer.parseInt(sMode));
                Log.d("PRESENCE_UI", "LauncherrActivity : OnCreate : getOperatorMode " + AppGlobalState.getOperatorMode());
            }
            //adb shell setprop persist.CD_BY_OPTION 1
            Process procCd = null;
            procCd = Runtime.getRuntime().exec(new String[] {
                    "/system/bin/getprop", "persist.CD_BY_OPTION"
            });
            BufferedReader readerCd = new BufferedReader(new InputStreamReader(procCd.getInputStream()));
            Log.d("PRESENCE_UI", "before my sModeCd is:");

            String sModeCd = readerCd.readLine();
            if(sModeCd == "") {
               sModeCd = "0";
            }
            Log.d("PRESENCE_UI", "After my sModeCd is: " + sModeCd);

            if (sModeCd != null && Integer.parseInt(sModeCd) == 1)
            {
                Log.d("PRESENCE_UI", "LauncherrActivity : OnCreate : sModeCd " + sModeCd);
                AppGlobalState.setCdByOption(true);
                Log.d("PRESENCE_UI", "LauncherrActivity : OnCreate : isCdByOption " + AppGlobalState.isCdByOption());
            }
            else
            {
		AppGlobalState.setCdByOption(false);
		Log.d("PRESENCE_UI", "LauncherrActivity : OnCreate : isCdByOption " + AppGlobalState.isCdByOption());
            }

        } catch (IOException e) {

            e.printStackTrace();
        } catch (Exception e) {

            e.printStackTrace();
        }



        int dummyContactCount = getIntent().getIntExtra("COUNT", 0);
        Log.d(TAG, "dummyContactCount=" + dummyContactCount);
        if (dummyContactCount == 0) {
            mContacts = getContactsFromDB();
        } else {
            mContacts = getDummyContacts(dummyContactCount);
        }

        markExcludedContacts();

        AppGlobalState.setMainActivityContext(mContext);

        if (AppGlobalState.getTimerManager() == null) {
            Timer timerManager = new Timer();
            AppGlobalState.setTimerManager(timerManager);
        }

        AppGlobalState.setContacts(mContacts);

        linkArrayWithDisplayedList(mContacts);

        createListeningThread();

        SharedPreferences setting = getSharedPrefHandle(
                AppGlobalState.IMS_PRESENCE_MY_INFO);
        AppGlobalState.setMyInfoSettingHandle(setting);

        Log.d("PRESENCE_UI", "MainActivity : onCreate : isIMSServBound "+isIMSServBound);
        if(!isIMSServBound)
        {
           isIMSServBound =bindSettingService();
           Log.d("PRESENCE_UI", "After Bind service isIMSServBound "+isIMSServBound);
        }

        IntentFilter filter_available = new IntentFilter();
        filter_available.addAction("RCS_SERVICE_AVAILABLE");

        IntentFilter filter_unavailable = new IntentFilter();
        filter_unavailable.addAction("RCS_SERVICE_UNAVAILABLE");

        //Register for UCE feature
        IntentFilter filter_uceUp = new IntentFilter();
        filter_available.addAction(ImsUceManager.ACTION_UCE_SERVICE_UP);
        //uce DOwn indication
        IntentFilter filter_uceDown = new IntentFilter();
        filter_available.addAction(ImsUceManager.ACTION_UCE_SERVICE_DOWN);

        receiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                // do something based on the intent's action

             Log.d("PRESENCE_UI", "intent.getAction():"+intent.getAction().toString());

                if(intent.getAction() != null && intent.getAction().equals("RCS_SERVICE_AVAILABLE"))
                {
                    Log.d("PRESENCE_UI", "MainActivity : onCreate : onReceive  RCS_SERVICE_AVAILABLE");
                    if(!isIMSServBound)
                    {
                        isIMSServBound =bindSettingService();
                        Log.d("PRESENCE_UI", "After Bind service isIMSServBound "+isIMSServBound);
                    }
                    }
                if(intent.getAction() != null && intent.getAction().equals("RCS_SERVICE_UNAVAILABLE"))
                {
                    Log.d("PRESENCE_UI", "MainActivity : onCreate : onReceive  RCS_SERVICE_UNAVAILABLE");
                    try
                    {
                        if (imsSettingService != null && imsSettingService.getLibLoadStatus()
                                && AppGlobalState.getQrcsImsSettingsclienthandle() != 0) {
                            Log.d("PRESENCE_UI", "MainActivity : before QrcsImsSettings_Deregister");
                            QrcsImsSettingsIntType client_handle = new QrcsImsSettingsIntType();
                            client_handle.setQrcsImsSettingsIntType(AppGlobalState
                                    .getQrcsImsSettingsclienthandle());
                            imsSettingService.QrcsImsSettings_Deregister(client_handle);
                            AppGlobalState.setQrcsImsSettingsclienthandle(0);
                            Log.d("PRESENCE_UI", "MainActivity : after QrcsImsSettings_Deregister");
                        }
                    }catch(Exception e){
                        e.printStackTrace();
                    }
                }

                if(intent.getAction() != null && intent.getAction().equals(ImsUceManager.ACTION_UCE_SERVICE_DOWN))
                {
                    Log.d("PRESENCE_UI", "MainActivity : ACTION_UCE_SERVICE_DOWN");
                    releaseServices();

                    //ToDo: RcsService died retry service connection
                }

                if(intent.getAction() != null && intent.getAction().equals(ImsUceManager.ACTION_UCE_SERVICE_UP))
                {
                    //ToDo: RcsService died retry service connection
                    Log.d("PRESENCE_UI", "MainActivity : ACTION_UCE_SERVICE_UP");
                    bindRcsService();

                }
            }
        };
        registerReceiver(receiver, filter_available);
        registerReceiver(receiver, filter_unavailable);
        registerReceiver(receiver, filter_uceUp);
        registerReceiver(receiver, filter_uceDown);


        if (NewLaunch == 1)
        {

            Log.d("PRESENCE_UI", "LAUNCHING NOW... NewLaunch = " + NewLaunch);
            TelephonyManager teleMan = (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);
            int telephonyManagerNetworkType = teleMan.getNetworkType();
            Log.d("PRESENCE_UI", "MainActivity : onCreate : telephonyManagerNetworkType " + telephonyManagerNetworkType);

            switch (telephonyManagerNetworkType)
            {
                case TelephonyManager.NETWORK_TYPE_LTE:
                    Log.d("PRESENCE_UI", "MainActivity : onCreate : NETWORK_TYPE_LTE ");
                    ContactInfo.networkTypeLTE = true;
                    ContactInfo.vopsEnabled = true;
                    break;

                case TelephonyManager.NETWORK_TYPE_EHRPD:
                    Log.d("PRESENCE_UI", "MainActivity : onCreate : NETWORK_TYPE_EHRPD ");
                    ContactInfo.networkTypeLTE = false;
                    ContactInfo.vopsEnabled = false;
                    break;

                default:
                    ContactInfo.networkTypeLTE = false;
                    ContactInfo.vopsEnabled = false;
                    break;
            }

            NewLaunch = 0;
        }

        Log.d("PRESENCE_UI", "NewLaunch = " + NewLaunch);


    }


    ServiceConnection mQrcsServConn = new ServiceConnection() {

        public void onServiceConnected(ComponentName name, IBinder service) {

            Log.d("PRESENCE_UI", "MainActivity : onServiceConnected : rcs");
            isRCSServBound = true;
            AppGlobalState.setIqRcsService(IUceService.Stub.asInterface(service));
            initAllServices(AppGlobalState.getIqRcsService());
        }

        public void onServiceDisconnected(ComponentName name) {
            Log.d("PRESENCE_UI", "MainActivity : onServiceDisconnected : Before");
            //AppGlobalState.setIqRcsService(null);
            //isRCSServBound = false;
            new Handler().postDelayed(new Runnable() {

                public void run() {
                    /*Intent rcsIntent = new Intent();
                    rcsIntent.setComponent(new ComponentName("com.qualcomm.qti.rcsservice", "com.qualcomm.qti.rcsservice.QRCSService"));
                    rcsIntent.setPackage("com.qualcomm.qti.rcsservice");
                    isRCSServBound = bindService(rcsIntent, mQrcsServConn, Context.BIND_AUTO_CREATE);*/
                    isRCSServBound = bindRcsService();
                    Log.d("PRESENCE_UI", "MainActivity : After rcs service bind isRCSServBound "+isRCSServBound);
                }
            }, 2000);
            Log.d("PRESENCE_UI", "MainActivity : onServiceDisconnected : After");
        }

    };

    void SetCustomFeatureTags() {

        try
        {
            Process procCd = null;
            String sModeCd = "0";
            procCd = Runtime.getRuntime().exec(new String[] {
                    "/system/bin/getprop", "persist.rcs.CustomFT"
            });
            BufferedReader readerCd = new BufferedReader(new InputStreamReader(procCd.getInputStream()));

            sModeCd = readerCd.readLine();

            if(sModeCd != null && sModeCd.equals("")) {
               sModeCd = "0";
            }
            Log.d("PRESENCE_UI", "SetCustomFeatureTags :After my sModeCd is: " + sModeCd);

            if (sModeCd != null && Integer.parseInt(sModeCd) == 1)
            {
                AppGlobalState.setCustomFTEnabled(true);
            }

        } catch (Exception e) {
            e.printStackTrace();
            Log.d("PRESENCE_UI", "RemoteException, Unable to set New Feature Tag");
        }
    }

    void startPresenceService(IUceService rcsService) {
        try {

            int presenceSerrviceHandle = AppGlobalState.getPresenceSerrviceHandle();

            AppGlobalState.setPresenceService(rcsService.getPresenceService());
            if ( presenceSerrviceHandle == 0) {
                Log.d("PRESENCE_UI", "MainActivity : Creating New Presence Service");
                presenceSerrviceHandle = rcsService.createPresenceService(
                                            AppGlobalState.getListenerHandler().presListener,
                                            AppGlobalState.getPresenceListenerHandle());

            } else {
                Log.d("PRESENCE_UI", "MainActivity : PresenceServiceHandle already exists, destroy current handle");

                rcsService.destroyPresenceService(AppGlobalState.getPresenceSerrviceHandle());

                if (imsSettingService != null && imsSettingService.getLibLoadStatus()
                    && AppGlobalState.getQrcsImsSettingsclienthandle() == 0) {

                    m_client_handle = new QrcsImsSettingsIntType();
                    Log.d("PRESENCE_UI", " QrcsImsSettings_Register start ");
                    imsSettingService.QrcsImsSettings_Register(iqimsSettingListener,
                                                               m_client_handle);
                    AppGlobalState.setQrcsImsSettingsclienthandle(m_client_handle
                                                                  .getQrcsImsSettingsIntType());
                }

                presenceSerrviceHandle = rcsService.createPresenceService(
                                            AppGlobalState.getListenerHandler().presListener,
                                            AppGlobalState.getPresenceListenerHandle());
            }
            Log.d("PRESENCE_UI", "MainActivity : presenceSerrviceHandle[" + presenceSerrviceHandle +"]");
            AppGlobalState.setPresenceSerrviceHandle(presenceSerrviceHandle);
            SetCustomFeatureTags();
        } catch (RemoteException e) {
            Log.d("PRESENCE_UI", " MainActivity : startOptionsService :  DeadObjectException dialog  ");
            e.printStackTrace();
        }
        catch (Exception e) {
            e.printStackTrace();
        }

    }

    void startOptionsService(IUceService rcsService){
        try {

            int optionsSrvHandle = AppGlobalState.getCdServiceHandle();
            if(optionsSrvHandle == 0) {
                Log.d("PRESENCE_UI", "MainActivity : Creating New Options Service");
                optionsSrvHandle = rcsService.createOptionsService(
                    AppGlobalState.getListenerHandler().cdServiceListener,
                    AppGlobalState.getOptionsListenerHandle());

            } else {
                Log.d("PRESENCE_UI", "MainActivity : OptionsServiceHandle already exists, destroy current handle");
                rcsService.destroyOptionsService(AppGlobalState.getCdServiceHandle());
                Log.d(TAG,"after QRCSDestroyCDService");
                optionsSrvHandle = rcsService.createOptionsService(
                    AppGlobalState.getListenerHandler().cdServiceListener,
                    AppGlobalState.getOptionsListenerHandle());
            }
            Log.d("PRESENCE_UI", "MainActivity : OptionsServiceHandle[" + optionsSrvHandle +"]");
            AppGlobalState.setCdServiceHandle(optionsSrvHandle);
            IOptionsService optionsService = rcsService.getOptionsService();
            AppGlobalState.setCdService(optionsService);
            optionsService.getMyInfo(optionsSrvHandle, AppGlobalState.getpUserData());

        } catch (RemoteException e) {
            Log.d("PRESENCE_UI", " MainActivity : startOptionsService :  DeadObjectException dialog  ");
            e.printStackTrace();
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }

    void initAllServices(IUceService rcsService) {

        Log.d("PRESENCE_UI", "MainActivity initAllServices  : start");

        //isRCSServBound = true;
        try {
            if (rcsService != null) {
                //isRCSServBound = true;
                boolean serviceStatus = false;
                serviceStatus = rcsService.getServiceStatus();
                if (true == serviceStatus) {
                    Log.d("PRESENCE_UI", "MainActivity : Rcs Service Status is TRUE");
                    startPresenceService(rcsService);
                    startOptionsService(rcsService);
                }
             Log.d("PRESENCE_UI", "MainActivity : setMobileDataContentObserver");
             dataBroadcastReceiver.setMobileDataContentObserver(mContext);
            } else {
                Log.d("PRESENCE_UI",
                        " MainActivity : initAllServices : QRCSStartRCSService  rcsListener.iQRCSListener is NULL  ");
            }
        } catch (RemoteException e) {
            Log.d("PRESENCE_UI", " MainActivity : initAllServices :  DeadObjectException dialog  ");
            e.printStackTrace();
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }

    public QrcsImsSettingsIntType m_client_handle = new QrcsImsSettingsIntType();

    ServiceConnection mQImsServConn = new ServiceConnection() {

        public void onServiceConnected(ComponentName name, IBinder service) {
            Log.d("PRESENCE_UI", " onServiceConnected start ");

            imsSettingService = IQrcsImsSettingsService.Stub.asInterface(service);
            if (imsSettingService != null) {
                try
                {
                    Log.d("PRESENCE_UI", " LIBS STATUS" + imsSettingService.getLibLoadStatus());
                    if (imsSettingService.getLibLoadStatus())
                    {
                        m_client_handle = new QrcsImsSettingsIntType();
                        Log.d("PRESENCE_UI", " QrcsImsSettings_Register start ");
                        imsSettingService.QrcsImsSettings_Register(iqimsSettingListener, m_client_handle);
                        AppGlobalState.setQrcsImsSettingsclienthandle(m_client_handle.getQrcsImsSettingsIntType());
                        if(AppGlobalState.getQrcsImsSettingsclienthandle() != 0)
                        {
                            QrcsImsSettingsIntType client_handle = new QrcsImsSettingsIntType();
                            client_handle.setQrcsImsSettingsIntType(AppGlobalState.getQrcsImsSettingsclienthandle());
                            Log.d("PRESENCE_UI", " QrcsImsSettings_GetQipcallConfig Start ");
                            imsSettingService.QrcsImsSettings_GetQipcallConfig(client_handle);
                            Log.d("PRESENCE_UI", " QrcsImsSettings_GetQipcallConfig End ");
                            Log.d("PRESENCE_UI", " QrcsImsSettings_GetPresenceConfig start ");
                            imsSettingService.QrcsImsSettings_GetPresenceConfig(client_handle);
                            Log.d("PRESENCE_UI", " QrcsImsSettings_GetPresenceConfig End ");
                        }
                    } else
                    {
                        Log.d("PRESENCE_UI", " Libraries are missing. So Exit app");
                        Toast.makeText(mContext, "Libraries are missing. So Exit app",
                                Toast.LENGTH_SHORT).show();
                        finish();
                    }
                }
                catch (Exception e)
                {
                    Log.d("PRESENCE_UI", " Exception is " + e);
                }
            }
            else
            {
                isIMSServBound = false;
            }
        }

        public void onServiceDisconnected(ComponentName name) {
            Log.d("PRESENCE_UI", " rcsimssettings onServiceDisconnected start ");
            imsSettingService = null;
            new Handler().postDelayed(new Runnable() {
                public void run() {
                    /*Intent imsSetting = new Intent();
                    imsSetting.setComponent(new ComponentName("com.qualcomm.qti.rcsimssettings", "com.qualcomm.qti.rcsimssettings.QrcsImsSettingsService"));
                    imsSetting.setPackage("com.qualcomm.qti.rcsimssettings");
                    isIMSServBound = bindService(imsSetting, mQImsServConn, Context.BIND_AUTO_CREATE);*/
                    isIMSServBound =bindSettingService();
                }
            }, 1000);
            Log.d("PRESENCE_UI", "rcsimssettings onServiceDisconnected : After");
        }
    };

    IQrcsImsSettingsListener iqimsSettingListener = new IQrcsImsSettingsListener.Stub() {

        public void QrcsImsSettings_IndicationCb(
                QrcsImsSettingsIntType client_handle,
                QrcsImsSettingsIndId ind_id,QrcsImsSettingsConfigInd ind_msg) throws RemoteException {
                Log.d("PRESENCE_UI", " QrcsImsSettings_IndicationCb start snr ");

            Log.d("PRESENCE_UI", " QrcsImsSettings_IndicationCb ind_id snr " + ind_id.getQrcsImsSettingsIndId());


                if(ind_id.getQrcsImsSettingsIndIdObj().ordinal() == QrcsImsSettingsIndId.qrcs_ims_settings_ind_id.QRCS_IMS_SETTINGS_QIPCALL_CONFIG_IND.ordinal()
				&& AppGlobalState.getIqRcsService() != null)
                {
                 QrcsImsSettingsQipcallConfig mQrcsImsSettingsQipcallConfig = ind_msg.getQrcsImsSettingsQipcallConfig();

                 Log.d("PRESENCE_UI", " QrcsImsSettings_IndicationCb isVt_calling_enabled_valid " + mQrcsImsSettingsQipcallConfig.isVt_calling_enabled());
                 Log.d("PRESENCE_UI", " QrcsImsSettings_IndicationCb isMobile_data_enabled " + mQrcsImsSettingsQipcallConfig.isMobile_data_enabled());
                 Log.d("PRESENCE_UI", " QrcsImsSettings_IndicationCb isVolte_enabled " + mQrcsImsSettingsQipcallConfig.isVolte_enabled());

                 if( mQrcsImsSettingsQipcallConfig.isVt_calling_enabled_valid() &&
                    (Settings.isVt_calling_enabled != mQrcsImsSettingsQipcallConfig.isVt_calling_enabled()) )
                 {
                     Settings.isVt_calling_enabled = mQrcsImsSettingsQipcallConfig.isVt_calling_enabled();

                     SharedPreferences preferences = mContext.getSharedPreferences("presencedata", Context.MODE_PRIVATE);
                     SharedPreferences.Editor editor = preferences.edit();
                     editor.putBoolean("VT_KEY", Settings.isVt_calling_enabled);
                     editor.commit();
                     if(AppGlobalState.getOperatorMode() != AppGlobalState.TMO_MODE)
                     {
                       invokePublish(false);
                     }
                 }

        }
                 else if(ind_id.getQrcsImsSettingsIndIdObj().ordinal() == QrcsImsSettingsIndId.qrcs_ims_settings_ind_id.QRCS_IMS_SETTINGS_PRESENCE_CONFIG_IND.ordinal()
				 && AppGlobalState.getIqRcsService() != null)
                {
                 QrcsImsSettingsPresenceConfig mQrcsImsSettingsPresenceConfig = ind_msg.getQrcsImsSettingsPresenceConfig();
                 Log.d("PRESENCE_UI", " QrcsImsSettings_IndicationCb getPublish_expiry_timer " + mQrcsImsSettingsPresenceConfig.getPublish_expiry_timer());
                 Log.d("PRESENCE_UI", " QrcsImsSettings_IndicationCb getPublish_extended_expiry_timer " + mQrcsImsSettingsPresenceConfig.getPublish_extended_expiry_timer());
                 Log.d("PRESENCE_UI", " QrcsImsSettings_IndicationCb getMinimum_publish_interval " + mQrcsImsSettingsPresenceConfig.getMinimum_publish_interval());
                 Log.d("PRESENCE_UI", " QrcsImsSettings_IndicationCb getCapability_poll_list_subscription_expiry_timer " + mQrcsImsSettingsPresenceConfig.getCapability_poll_list_subscription_expiry_timer());
                 Log.d("PRESENCE_UI", " QrcsImsSettings_IndicationCb isCapability_discovery_enable " + mQrcsImsSettingsPresenceConfig.isCapability_discovery_enable());
                 Log.d("PRESENCE_UI", " QrcsImsSettings_IndicationCb getCapabilites_cache_expiration " + mQrcsImsSettingsPresenceConfig.getCapabilites_cache_expiration());
                 Log.d("PRESENCE_UI", " QrcsImsSettings_IndicationCb getAvailability_cache_expiration " + mQrcsImsSettingsPresenceConfig.getAvailability_cache_expiration());
                 Log.d("PRESENCE_UI", " QrcsImsSettings_IndicationCb getCapability_poll_interval " + mQrcsImsSettingsPresenceConfig.getCapability_poll_interval());
                 Log.d("PRESENCE_UI", " QrcsImsSettings_IndicationCb getMax_subcription_list_entries " + mQrcsImsSettingsPresenceConfig.getMax_subcription_list_entries());
                 Log.d("PRESENCE_UI", " QrcsImsSettings_IndicationCb getPublish_error_recovery_timer " + mQrcsImsSettingsPresenceConfig.getPublish_error_recovery_timer());
                 Log.d("PRESENCE_UI", " QrcsImsSettings_IndicationCb isVolte_user_opted_in_status " + mQrcsImsSettingsPresenceConfig.isVolte_user_opted_in_status());
                    Log.d("PRESENCE_UI", " QrcsImsSettings_IndicationCb getPublish_expiry_timer " + mQrcsImsSettingsPresenceConfig.getPublish_expiry_timer());

                }

                Log.d("PRESENCE_UI", " QrcsImsSettings_IndicationCb end ");
        }

        public void QrcsImsSettings_ErrorCb(
                QrcsImsSettingsIntType client_handle, QrcsImsSettingsError error)
                throws RemoteException {

        }

        public void QrcsImsSettings_QipcallConfigResponseCb(
                QrcsImsSettingsIntType client_handle,
                QrcsImsSettingsRespId resp_id,
                QrcsImsSettingsQipcallConfigResp resp_msg)
                throws RemoteException {

            if (resp_msg != null) {

                Log.d("PRESENCE_UI", " getImsSettingsResp "
                        + resp_msg.mQrcsImsSettingsResp.getImsSettingsResp());
                Log.d("PRESENCE_UI",
                        " getQmi_response_error "
                                + resp_msg.mQrcsImsSettingsResp.getQmi_response_error());
                Log.d("PRESENCE_UI",
                        " getSettings_resp_valid "
                                + resp_msg.mQrcsImsSettingsResp.getSettings_resp_valid());
                Log.d("PRESENCE_UI",
                        " isQmi_response_result "
                                + resp_msg.mQrcsImsSettingsResp.isQmi_response_result());

                if (resp_id.getQrcsImsSettingsRespId() == QrcsImsSettingsRespId.qrcs_ims_settings_resp_id.QRCS_IMS_SETTINGS_GET_QIPCALL_CONFIG_RESP)
                {

                    Settings.isMobile_data_enabled_valid = resp_msg.mQrcsImsSettingsQipcallConfig
                            .isMobile_data_enabled_valid();

                    Log.d("PRESENCE_UI", "isMobile_data_enabled_valid "
                            + Settings.isMobile_data_enabled_valid);

                    Settings.isMobile_data_enabled = resp_msg.mQrcsImsSettingsQipcallConfig
                            .isMobile_data_enabled();

                    Log.d("PRESENCE_UI", "isMobile_data_enabled "
                            + Settings.isMobile_data_enabled);

                    Log.d("PRESENCE_UI", "Settings.inSettingScreen "
                            + Settings.inSettingScreen);
                    Runnable updateDataSetting = new Runnable() {
                        public void run() {
                            if (Settings.inSettingScreen) {
                                try {
                                    Log.d("PRESENCE_UI",
                                            " Before updateDataSetting");
                                    Settings.settingsObject.updateDataSetting();
                                    Log.d("PRESENCE_UI",
                                            " After updateDataSetting");

                                } catch (Exception e) {
                                    Log.d("PRESENCE_UI",
                                            " In Setting screen Exception");
                                    e.printStackTrace();
                                }
                            }

                            if (ContactInfo.inContactInfoScreen) {
                                try {
                                    Log.d("PRESENCE_UI",
                                            " Before updateVTCallButton");
                                    ContactInfo contactInfo = AppGlobalState.getContactInfo();
                                    if (contactInfo != null) {
                                        contactInfo.populateForm(contactInfo
                                                .getIndexOfDisplayedContact());
                                    }
                                    Log.d("PRESENCE_UI",
                                            " After updateVTCallButton");

                                } catch (Exception e) {
                                    Log.d("PRESENCE_UI",
                                            " In updateVTCallButton Exception");
                                    e.printStackTrace();
                                }
                            }
                        }
                    };
                    runOnUiThread(updateDataSetting);
                    Log.d("PRESENCE_UI", " runOnUiThread(updateDataSetting)");

                    Settings.isVt_calling_enabled_valid = resp_msg.mQrcsImsSettingsQipcallConfig
                            .isVt_calling_enabled_valid();
                    Log.d("PRESENCE_UI", "isVt_calling_enabled_valid "+ Settings.isVt_calling_enabled_valid);


                       Settings.isVt_calling_enabled = resp_msg.mQrcsImsSettingsQipcallConfig
                            .isVt_calling_enabled();

                         Log.d("PRESENCE_UI","Settings.isVt_calling_enabled update prefs value:"+ Settings.isVt_calling_enabled);

                        SharedPreferences preferences = mContext.getSharedPreferences("presencedata", Context.MODE_PRIVATE);
                        SharedPreferences.Editor editor = preferences.edit();
                        editor.putBoolean("VT_KEY", Settings.isVt_calling_enabled);

                        editor.commit();

                        Settings.isVt_calling_enabled = preferences.getBoolean("VT_KEY", false);

                    Log.d("PRESENCE_UI","Settings.isVt_calling_enabled "+ Settings.isVt_calling_enabled);
                    Log.d("PRESENCE_UI", "For Video Settings.inSettingScreen "+ Settings.inSettingScreen);

                    //SharedPreferences preferences = mContext.getSharedPreferences(
                            //"ImsPresencePrefMyInfo", mContext.MODE_PRIVATE);
                    //SharedPreferences.Editor editor = preferences.edit();

                    /*if (Settings.isVt_calling_enabled) {
                        editor.putString("Description", "VoLTE Voice and Video Service");
                    } else {
                        editor.putString("Description", "VoLTE Service");
                    }
                    editor.commit();*/

                    if (Settings.inSettingScreen) {
                        Runnable updateVideoSetting = new Runnable() {
                            public void run() {
                                try {
                                    Log.d("PRESENCE_UI", " Before updateVideoSetting");
                                    Settings.settingsObject.updateVideoSetting();
                                    Log.d("PRESENCE_UI", " After updateVideoSetting");
                                } catch (Exception e) {
                                    Log.d("PRESENCE_UI", " In Setting screen Exception");
                                    e.printStackTrace();
                                }
                            }
                        };
                        runOnUiThread(updateVideoSetting);
                        Log.d("PRESENCE_UI", " runOnUiThread(updateVideoSetting)");
                    }

                    qipcall_config.setMobile_data_enabled(resp_msg
                            .mQrcsImsSettingsQipcallConfig.isMobile_data_enabled());
                    qipcall_config.setMobile_data_enabled_valid(resp_msg
                            .mQrcsImsSettingsQipcallConfig.isMobile_data_enabled_valid());
                    qipcall_config.setVt_calling_enabled(resp_msg
                            .mQrcsImsSettingsQipcallConfig.isVt_calling_enabled());
                    qipcall_config.setVt_calling_enabled_valid(resp_msg
                            .mQrcsImsSettingsQipcallConfig.isVt_calling_enabled_valid());

                    Log.d("PRESENCE_UI", " resp_id.getQrcsImsSettingsRespId() "
                                    + resp_id.getQrcsImsSettingsRespId());

                    Log.d("PRESENCE_UI", "AppStart = " + AppStart);
                    AppGlobalState.isDataSettingNvSame = true;


                    if (AppStart == 1 && (AppGlobalState.getOperatorMode() == AppGlobalState.VZW_MODE)
                                      || (AppGlobalState.getOperatorMode() == AppGlobalState.TMO_MODE)) {
                        AppStart = 0;

                        boolean data = false;
                        ConnectivityManager cm = (ConnectivityManager) mContext
                                .getSystemService(Context.CONNECTIVITY_SERVICE);
                        try {
                            Class cmClass = Class.forName(cm.getClass().getName());
                            Method method = cmClass.getDeclaredMethod("getMobileDataEnabled");
                            method.setAccessible(true); // Make the method
                                                        // callback
                            // get the setting for "mobile data"
                            data = (Boolean) method.invoke(cm);
                            Log.d("PRESENCE_UI", "data from Settings  UI" + data);
                        } catch (Exception e) {
                            // Some problem accessible private API
                        }

                        Log.d("PRESENCE_UI", "Data value from setting UI = " + data);

                        if (resp_msg.mQrcsImsSettingsQipcallConfig.isMobile_data_enabled() != data) {
                            Log.d("PRESENCE_UI", "Data value from setting UI does not match with the resp_msg.isMobile_data_enabled()");
                            if (data) {
                                Log.d("PRESENCE_UI", "Setting Data Enabled value from Preference ");
                                qipcall_config.setMobile_data_enabled(true);
                                qipcall_config.setMobile_data_enabled_valid(true);
                            } else {
                                Log.d("PRESENCE_UI", "Setting Data Disabled value from Preference ");
                                qipcall_config.setMobile_data_enabled(false);
                                qipcall_config.setMobile_data_enabled_valid(true);
                            }
                            Log.d("PRESENCE_UI", "Before QrcsImsSettings_SetQipcallConfig");
                            if(imsSettingService != null)
                            {
                                imsSettingService.QrcsImsSettings_SetQipcallConfig(client_handle, qipcall_config);
                            }
                            Log.d("PRESENCE_UI", "After QrcsImsSettings_SetQipcallConfig");
                            AppGlobalState.isDataSettingNvSame = false;
                            ContactInfo.firstPublish = false;
                        } else {
                            AppGlobalState.isDataSettingNvSame = true;
                            Log.d("PRESENCE_UI","Data value from setting UI match with the resp_msg.isMobile_data_enabled()");
                        }
                    }
                    else
                    {
                        AppStart = 0;
                    }
                    Log.d(TAG, "QrcsImsSettings_QipcallConfigResponseCb : isDataSettingNvSame "+AppGlobalState.isDataSettingNvSame);
                }

                if (resp_id.getQrcsImsSettingsRespId() == QrcsImsSettingsRespId.qrcs_ims_settings_resp_id.QRCS_IMS_SETTINGS_SET_QIPCALL_CONFIG_RESP
				&& AppGlobalState.getIqRcsService() != null)
                {
                    Log.d(TAG, "QrcsImsSettings_QipcallConfigResponseCb : before publish");
                    if(ContactInfo.firstPublish)
                    {
                        invokePublish(false);
                    }
                    else
                    {
                        ContactInfo.firstPublish = true;
                    }
                    Log.d(TAG, "QrcsImsSettings_QipcallConfigResponseCb : after publish");
                }
            }
        }

        public void QrcsImsSettings_PresenceConfigResponseCb(
                QrcsImsSettingsIntType client_handle,
                QrcsImsSettingsRespId resp_id,
                QrcsImsSettingsPresenceConfigResp resp_msg)
                throws RemoteException {
            Log.d("PRESENCE_UI", " resp_msg is  " + resp_msg.toString());
            Log.d("PRESENCE_UI", " getImsSettingsResp "
                    + resp_msg.mQrcsImsSettingsResp.getImsSettingsResp());
            Log.d("PRESENCE_UI",
                    " getQmi_response_error "
                            + resp_msg.mQrcsImsSettingsResp.getQmi_response_error());
            Log.d("PRESENCE_UI",
                    " getSettings_resp_valid "
                            + resp_msg.mQrcsImsSettingsResp.getSettings_resp_valid());
            Log.d("PRESENCE_UI",
                    " isQmi_response_result "
                            + resp_msg.mQrcsImsSettingsResp.isQmi_response_result());

            Log.d("PRESENCE_UI",
                    " resp_id.getQrcsImsSettingsRespId() " + resp_id.getQrcsImsSettingsRespId());

            if (resp_id.getQrcsImsSettingsRespId() == QrcsImsSettingsRespId.qrcs_ims_settings_resp_id.QRCS_IMS_SETTINGS_GET_PRESENCE_CONFIG_RESP) {

                Settings.isAvailability_cache_expiration_valid = resp_msg.mQrcsImsSettingsPresenceConfig
                        .isAvailability_cache_expiration_valid();
                if (resp_msg.mQrcsImsSettingsPresenceConfig
                        .isAvailability_cache_expiration_valid()) {
                    Log.d("PRESENCE_UI",
                            " isAvailability_cache_expiration_valid is TRUE");
                    Settings.availabilityCacheExpiration = resp_msg.mQrcsImsSettingsPresenceConfig
                            .getAvailability_cache_expiration();
                    Log.d("PRESENCE_UI", " getAvailability_cache_expiration = "
                            + Settings.availabilityCacheExpiration);

                }
                Settings.isCapabilites_cache_expiration_valid = resp_msg.mQrcsImsSettingsPresenceConfig
                        .isCapabilites_cache_expiration_valid();
                if (resp_msg.mQrcsImsSettingsPresenceConfig
                        .isCapabilites_cache_expiration_valid()) {
                    Log.d("PRESENCE_UI",
                            " isCapabilites_cache_expiration_valid is TRUE");
                    Settings.capabilityCacheExpiration = resp_msg.mQrcsImsSettingsPresenceConfig
                            .getCapabilites_cache_expiration();
                    Log.d("PRESENCE_UI", " getCapabilites_cache_expiration = "
                            + Settings.capabilityCacheExpiration);

                }

                Settings.isCapability_discovery_enable_valid = resp_msg.mQrcsImsSettingsPresenceConfig
                        .isCapability_discovery_enable_valid();
                if (resp_msg.mQrcsImsSettingsPresenceConfig
                        .isCapability_discovery_enable_valid()) {
                    Log.d("PRESENCE_UI",
                            " isCapability_discovery_enable_valid is TRUE");
                    Settings.capabilityDiscoveryEnable = resp_msg.mQrcsImsSettingsPresenceConfig
                            .isCapability_discovery_enable();
                    Log.d("PRESENCE_UI", " isCapability_discovery_enable = "
                            + Settings.capabilityDiscoveryEnable);
                }

                Settings.isCapability_poll_interval_valid = resp_msg.mQrcsImsSettingsPresenceConfig
                        .isCapability_poll_interval_valid();
                if (resp_msg.mQrcsImsSettingsPresenceConfig
                        .isCapability_poll_interval_valid()) {
                    Log.d("PRESENCE_UI",
                            " isCapability_poll_interval_valid is TRUE");
                    Settings.capabilityPollInterval = resp_msg.mQrcsImsSettingsPresenceConfig
                            .getCapability_poll_interval();
                    Log.d("PRESENCE_UI", " getCapability_poll_interval = "
                            + Settings.capabilityPollInterval);
                }

                Settings.isCapability_poll_list_subscription_expiry_timer_valid = resp_msg.mQrcsImsSettingsPresenceConfig
                        .isCapability_poll_list_subscription_expiry_timer_valid();
                if (resp_msg.mQrcsImsSettingsPresenceConfig
                        .isCapability_poll_list_subscription_expiry_timer_valid()) {
                    Log.d("PRESENCE_UI",
                            " isCapability_poll_list_subscription_expiry_timer_valid is TRUE");
                    Settings.listSubscriptionExpiryTimer = resp_msg.mQrcsImsSettingsPresenceConfig
                            .getCapability_poll_list_subscription_expiry_timer();
                    Log.d("PRESENCE_UI", " getCapability_poll_list_subscription_expiry_timer = "
                            + Settings.listSubscriptionExpiryTimer);

                }
                Settings.isMax_subcription_list_entries_valid = resp_msg.mQrcsImsSettingsPresenceConfig
                        .isMax_subcription_list_entries_valid();
                if (resp_msg.mQrcsImsSettingsPresenceConfig
                        .isMax_subcription_list_entries_valid()) {
                    Log.d("PRESENCE_UI",
                            " isMax_subcription_list_entries_valid is TRUE");
                    Settings.maxSubscriptionListEntries = resp_msg.mQrcsImsSettingsPresenceConfig
                            .getMax_subcription_list_entries();
                    Log.d("PRESENCE_UI", " getMax_subcription_list_entries = "
                            + Settings.maxSubscriptionListEntries);
                }

                Settings.isMinimum_publish_interval_valid = resp_msg.mQrcsImsSettingsPresenceConfig
                        .isMinimum_publish_interval_valid();
                if (resp_msg.mQrcsImsSettingsPresenceConfig
                        .isMinimum_publish_interval_valid()) {
                    Log.d("PRESENCE_UI",
                            " isMinimum_publish_interval_valid is TRUE");
                    Settings.minimumPublishInterval = resp_msg.mQrcsImsSettingsPresenceConfig
                            .getMinimum_publish_interval();
                    Log.d("PRESENCE_UI", " getMinimum_publish_interval = "
                            + Settings.minimumPublishInterval);

                }
                Settings.isPublish_expiry_timer_valid = resp_msg.mQrcsImsSettingsPresenceConfig
                        .isPublish_expiry_timer_valid();
                if (resp_msg.mQrcsImsSettingsPresenceConfig
                        .isPublish_expiry_timer_valid()) {
                    Log.d("PRESENCE_UI",
                            " isPublish_expiry_timer_valid is TRUE");
                    Settings.publishExpiryTimer = resp_msg.mQrcsImsSettingsPresenceConfig
                            .getPublish_expiry_timer();
                    Log.d("PRESENCE_UI", " getPublish_expiry_timer = "
                            + Settings.publishExpiryTimer);

                }
                Settings.isPublish_extended_expiry_timer_valid = resp_msg.mQrcsImsSettingsPresenceConfig
                        .isPublish_extended_expiry_timer_valid();
                if (resp_msg.mQrcsImsSettingsPresenceConfig
                        .isPublish_extended_expiry_timer_valid()) {
                    Log.d("PRESENCE_UI",
                            " isPublish_extended_expiry_timer_valid is TRUE");
                    Settings.publishExtendedExpiryTimer = resp_msg.mQrcsImsSettingsPresenceConfig
                            .getPublish_extended_expiry_timer();
                    Log.d("PRESENCE_UI", " getPublish_expiry_timer = "
                            + Settings.publishExtendedExpiryTimer);

                }

                Settings.isVolte_user_opted_in_status_valid = resp_msg.mQrcsImsSettingsPresenceConfig
                        .isVolte_user_opted_in_status_valid();
                if (resp_msg.mQrcsImsSettingsPresenceConfig
                        .isVolte_user_opted_in_status_valid()) {
                    Log.d("PRESENCE_UI",
                            " isVolte_user_opted_in_status_valid is TRUE");
                    Settings.volteUserOptInStatus = resp_msg.mQrcsImsSettingsPresenceConfig
                            .isVolte_user_opted_in_status();
                    Log.d("PRESENCE_UI", " isVolte_user_opted_in_status = "
                            + Settings.volteUserOptInStatus);
                    if(AppGlobalState.getOperatorMode() == AppGlobalState.ATT_MODE)
                    {
                    Log.d("PRESENCE_UI", "For ATT , make Presence check  = "
                            + Settings.volteUserOptInStatus);
                      SharedPreferences preferences = mContext.getSharedPreferences("presencedata", Context.MODE_PRIVATE);
                      SharedPreferences.Editor editor = preferences.edit();
                      editor.putBoolean("PRESENCE_KEY", Settings.volteUserOptInStatus);
                      editor.commit();
                    }

                }
            }
            Log.d("PRESENCE_UI", " getImsSettingsResp   2  ");

            Runnable updateSetting = new Runnable() {
                public void run() {
                    try {
                        Log.d("PRESENCE_UI", " In Setting screen"
                                + Settings.inSettingScreen);
                        if (Settings.inSettingScreen) {
                            Log.d("PRESENCE_UI", " Before Populating values");
                            Settings.settingsObject.updateSettingValues();
                            Log.d("PRESENCE_UI", "After Populating values ");
                        }

                    } catch (Exception e) {
                        Log.d("PRESENCE_UI", " In Setting screen Exception");
                        e.printStackTrace();
                    }
                }
            };
            runOnUiThread(updateSetting);

            Log.d("PRESENCE_UI", " runOnUiThread(updateSetting)");

        }
    };

    private void markExcludedContacts() {
        Utility.prepareExcludedContactList();
        for (Contact c : mContacts) {
            if (Utility.isContactExcluded(c)) {
                c.setContactExcluded();
            }
        }

    }

    @Override
    protected void onPause() {
        Log.d(TAG, "MainActivity onPause()");

        super.onPause();
    }

    @Override
    protected void onStop() {
        Log.d(TAG, "MainActivity onStop()");
        /*if (false) {
            // UT Only.
            for (Contact c : AppGlobalState.getContacts()) {
                TimerTask t = c.getSubscribeTimerTask();

                if (t != null) {
                    t.cancel();
                }
            }
        }*/
        super.onStop();
    }

    private ArrayList<Contact> getDummyContacts(int dummyContactCount) {
        ArrayList<Contact> contacts = new ArrayList<Contact>();
        for (int i = 0; i < dummyContactCount; i++) {
            if ((AppGlobalState.getOperatorMode() == AppGlobalState.VZW_MODE)
             || (AppGlobalState.getOperatorMode() == AppGlobalState.TMO_MODE))
            {
                Contact c = new Contact("Test." + i, "555" + i, 0, "","");
                contacts.add(c);
            }
            else
            {
                Contact c = new Contact("Test." + i, "555" + i, 0, "","");
                contacts.add(c);
            }
        }
        return contacts;
    }

    @Override
    protected Dialog onCreateDialog(int id) {
        AlertDialog.Builder closeApp = new AlertDialog.Builder(this);
        switch (id) {
            case 0:
                closeApp.setTitle(R.string.close_dialog_title)
                        .setMessage(R.string.close_dialog_message)
                        .setCancelable(false)
                        .setPositiveButton(R.string.close_dialog_positive_button,
                                new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int id) {
                                close();
                            }
                        })
                        .setNegativeButton(R.string.close_dialog_negative_button,
                                new DialogInterface.OnClickListener() {

                            public void onClick(DialogInterface dialog, int which) {
                                dialog.dismiss();
                            }
                        });
                break;
            default:
                break;
        }

        AlertDialog versionDialog = closeApp.create();
        return versionDialog;
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {

        if (keyCode == KeyEvent.KEYCODE_BACK)
        {
            showDialog(0);
            return false;
        }
        return super.onKeyDown(keyCode, event);
    }

    private void close()
    {
        Log.d(TAG, "close hit");
        appAbnormalClose = false;
        ContactInfo.firstPublish = true;

        try
        {
            Log.d(TAG, "close before unregisterReceiver");
            unregisterReceiver(receiver);
            Log.d(TAG, "close After unregisterReceiver");
        } catch (Exception e)
        {
            e.printStackTrace();
        }

        try
        {
            if (AppGlobalState.getIqRcsService() != null)
            {
                isRCSServBound = false;
                if(AppGlobalState.isCdByOption() && AppGlobalState.getCdServiceHandle() != 0)
                {
                  AppGlobalState.getIqRcsService().destroyOptionsService(AppGlobalState.getCdServiceHandle());
                  Log.d(TAG,"after QRCSDestroyCDService");
                }

                Log.d(TAG, "close PresServiceHandle:" + AppGlobalState.getPresenceSerrviceHandle());
                if (AppGlobalState.getPresenceSerrviceHandle() != 0)
                {
                    Log.d(TAG,
                            "close PresListenerHandle:"
                                    + AppGlobalState.getPresenceListenerHandle() + "Val:"
                                    + AppGlobalState.getPresenceListenerHandle().getUceLong());
                    if (AppGlobalState.getPresenceListenerHandle() != null
                            && AppGlobalState.getPresenceListenerHandle().getUceLong() != 0)
                    {
                        Log.d(TAG, "close Calling QPresService_RemoveListener");
                        AppGlobalState.getPresenceService().removeListener(
                                AppGlobalState.getPresenceSerrviceHandle(),
                                AppGlobalState.getPresenceListenerHandle());
                        Log.d(TAG, "close Calling QPresService_RemoveListener End");
                    }
                    AppGlobalState.getIqRcsService().destroyPresenceService(
                            AppGlobalState.getPresenceSerrviceHandle());
                    AppGlobalState.setPresenceSerrviceHandle(0);
                }
                else
                {
                    Log.d(TAG, "getPresenceSerrviceHandle null");
                }
                AppGlobalState.setPresenceService(null);
                Log.d(TAG, "Presence Service release");
            }
            unbindService(mQrcsServConn);
            Log.d(TAG, "close unbindService mQrcsServConn");
        } catch (RemoteException e1) {
            e1.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }

        stopTimers();

        Utility.closeLiveLoggingFile();
        Utility.closeNotifyXMLFile();

        try
        {
            isIMSServBound = false;
            if (imsSettingService != null && imsSettingService.getLibLoadStatus()
                    && AppGlobalState.getQrcsImsSettingsclienthandle() != 0) {
                QrcsImsSettingsIntType client_handle = new QrcsImsSettingsIntType();
                client_handle.setQrcsImsSettingsIntType(AppGlobalState
                        .getQrcsImsSettingsclienthandle());
                imsSettingService.QrcsImsSettings_Deregister(client_handle);
            }
            AppStart = 1;
            NewLaunch = 1;
            AppGlobalState.isDataSettingNvSame = false;
            imsSettingService = null;
            unbindService(mQImsServConn);
            Log.d(TAG, "close unbindService mQImsServConn");
        } catch (RemoteException e) {
            e.printStackTrace();
            Log.d("PRESENCE_UI", "RemoteException " + e);
        } catch (Exception e) {
            Log.d("PRESENCE_UI", "Exception " + e);
        }

        new Handler().postDelayed(new Runnable() {

            public void run() {

                finish();
            }
        }, 1000);
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
    super.onConfigurationChanged(newConfig);
    Log.d(TAG, "onConfigurationChanged:newConfig:"+newConfig);
    }

    @Override
    public void onDestroy() {

        Log.d(TAG, "onDestroy hit appAbnormalClose " + appAbnormalClose);
        if (appAbnormalClose)
        {
            ContactInfo.firstPublish = true;
            try
            {
                Log.d(TAG, "onDestroy before unregisterReceiver");
                unregisterReceiver(receiver);
                Log.d(TAG, "onDestroy After unregisterReceiver");
            } catch (Exception e)
            {
                e.printStackTrace();
            }

            try
            {
                if (AppGlobalState.getIqRcsService() != null)
                {
                  if(AppGlobalState.isCdByOption() && AppGlobalState.getCdServiceHandle() != 0)
                  {
                    AppGlobalState.getIqRcsService().destroyOptionsService(AppGlobalState.getCdServiceHandle());
                    Log.d(TAG,"after QRCSDestroyCDService");
                  }
                    isRCSServBound = false;
                    Log.d(TAG,
                            "onDestroy PresServiceHandle:"
                                    + AppGlobalState.getPresenceSerrviceHandle());
                    if (AppGlobalState.getPresenceSerrviceHandle() != 0)
                    {
                        Log.d(TAG,
                                "onDestroy PresListenerHandle:"
                                        + AppGlobalState.getPresenceListenerHandle() + "Val:"
                                        + AppGlobalState.getPresenceListenerHandle().getUceLong());
                        if (AppGlobalState.getPresenceListenerHandle() != null
                                && AppGlobalState.getPresenceListenerHandle().getUceLong() != 0)
                        {
                            Log.d(TAG, "onDestroy Calling QPresService_RemoveListener");
                            AppGlobalState.getPresenceService().removeListener(
                                    AppGlobalState.getPresenceSerrviceHandle(),
                                    AppGlobalState.getPresenceListenerHandle());
                            Log.d(TAG, "onDestroy Calling QPresService_RemoveListener End");
                        }
                        AppGlobalState.getIqRcsService().destroyPresenceService(
                                AppGlobalState.getPresenceSerrviceHandle());
                        AppGlobalState.setPresenceSerrviceHandle(0);
                    }
                    else
                    {
                        Log.d(TAG, "getPresenceSerrviceHandle null");
                    }
                    AppGlobalState.setPresenceService(null);
                    Log.d(TAG, "onDestroy after destroy presence service");
                }
                unbindService(mQrcsServConn);
                Log.d(TAG, "onDestroy after unbind rcs service");
            } catch (RemoteException e1) {
                e1.printStackTrace();
            } catch (Exception e) {
                e.printStackTrace();
            }

            stopTimers();

            Utility.closeLiveLoggingFile();
            Utility.closeNotifyXMLFile();

            try
            {
                isIMSServBound = false;
                if (imsSettingService != null && imsSettingService.getLibLoadStatus()
                        && AppGlobalState.getQrcsImsSettingsclienthandle() != 0) {
                    QrcsImsSettingsIntType client_handle = new QrcsImsSettingsIntType();
                    client_handle.setQrcsImsSettingsIntType(AppGlobalState
                            .getQrcsImsSettingsclienthandle());
                    imsSettingService.QrcsImsSettings_Deregister(client_handle);
                }
                AppStart = 1;
                NewLaunch = 1;
                AppGlobalState.isDataSettingNvSame = false;
                imsSettingService = null;
                unbindService(mQImsServConn);
            } catch (RemoteException e) {
                e.printStackTrace();
                Log.d("PRESENCE_UI", "RemoteException " + e);
            } catch (Exception e) {
                Log.d("PRESENCE_UI", "Exception " + e);
            }
        }
        super.onDestroy();
    }

    private void stopTimers() {
        Log.d(TAG, "Stop timers");
        for (Contact c : AppGlobalState.getContacts()) {
            TimerTask t = c.getSubscribeTimerTask();

            if (t != null) {
                t.cancel();
            }
        }

    }

    private void createListeningThread() {

        if (AppGlobalState.getListenerLooper() == null) {
            HandlerThread listenerThread = new HandlerThread("Listener",
                    android.os.Process.THREAD_PRIORITY_BACKGROUND);

            listenerThread.start();
            Looper listenerLooper = listenerThread.getLooper();
            mListenerHandler = new ListenerHandler(mContext, listenerLooper);

            AppGlobalState.setListenerHandler(mListenerHandler);
            AppGlobalState.setListenerLooper(listenerLooper);

        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {

        super.onCreateOptionsMenu(menu);
        Log.d(TAG, "AppGlobalState.getIqRcsService() status : "+AppGlobalState.getIqRcsService());
        if (AppGlobalState.getIqRcsService() != null) {
        getMenuInflater().inflate(R.menu.menu, menu);
        }
        return true;

    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {

            case R.id.ims_enabler:
                sendLogMesg(mContext, "Menu option IMS Enabler selected.");
                mImsEnablerTask = new ImsEnablerTask(mContext).execute();
                return true;

            case R.id.publish_rich:
                sendLogMesg(mContext, "Menu option Publish Rich selected.");

                Log.d("PRESENCE_UI", "isCdByOption " + AppGlobalState.isCdByOption());

                if(AppGlobalState.getOperatorMode() == AppGlobalState.ATT_MODE)
                {
                    try {
                        if(AppGlobalState.getPresenceService() != null && AppGlobalState.getPresenceSerrviceHandle() != 0)
                        {
                            Log.d(TAG, "before calling QPresService_ReenableService");
                            AppGlobalState.getPresenceService().reenableService(
                                    AppGlobalState.getPresenceSerrviceHandle(), AppGlobalState.getpUserData());
                            Log.d(TAG, "after calling QPresService_ReenableService");
                        }
                        else
                        {
                            Log.d(TAG, "QPresService_ReenableService : service is null");
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }

                if(AppGlobalState.isCdByOption()) {
                  invokePublish(true);
                } else {
                  invokePublish(false);
                }

                return true;

            case R.id.listsubscribepolling:
                invokeListSubscribePolling();
                return true;

            case R.id.listsubscribesimple:
                invokeListSubscribeSimple();
                return true;

            case R.id.myinfo:
                sendLogMesg(mContext, "Menu option MyInfo selected.");
                startMyInfoActivity();
                return true;

            case R.id.settings:
                sendLogMesg(mContext, "Menu option Settings selected.");
                startSettingsActivity();
                return true;

            case R.id.select_all:
                sendLogMesg(mContext, "Menu option SelectAll selected.");
                selectAllContacts(true);
                return true;

            case R.id.select_none:
                sendLogMesg(mContext, "Menu option SelectNone selected.");
                selectAllContacts(false);
                return true;

            case R.id.live_logging:
                startLiveLoggingActivity();
                return true;

            case R.id.refresh_contacts:

                int dummyContactCount = getIntent().getIntExtra("COUNT", 0);
                Log.d(TAG, "dummyContactCount=" + dummyContactCount);
                if (dummyContactCount == 0) {
                    mContacts = getContactsFromDB();
                } else {
                    mContacts = getDummyContacts(dummyContactCount);
                }

                markExcludedContacts();

                AppGlobalState.setMainActivityContext(mContext);

                AppGlobalState.setContacts(mContacts);

                linkArrayWithDisplayedList(mContacts);
                return true;
        }
        return false; // should never happen
    }

    private void invokePublish(Boolean manualPublish) {

        if (Utility.getNotifyFmt(mContext) == AppGlobalState.NOTIFY_FMT_STRUCT && AppGlobalState.getIqRcsService() != null) {
            sendLogMesg(mContext, "Publish fmt=STRUCT based.");
            mPublishTask = new PublishTask().execute(manualPublish);
        }
    }

    private void invokeListSubscribePolling() {
        if (Utility.getNotifyFmt(mContext) == AppGlobalState.NOTIFY_FMT_STRUCT) {
            sendLogMesg(mContext, "List subscribe polling fmt=STRUCT based.");
            mSubscribePollingTask = new ListAvailabilityFetchTask(mContext)
                    .execute();
        }
    }

    private void invokeListSubscribeSimple() {

        if((AppGlobalState.getOperatorMode() == AppGlobalState.VZW_MODE)
        || (AppGlobalState.getOperatorMode() == AppGlobalState.TMO_MODE))
        {
            if (Utility.getNotifyFmt(mContext) == AppGlobalState.NOTIFY_FMT_STRUCT) {
                sendLogMesg(mContext, "List subscribe simple fmt=STRUCT based.");
                mSubscribePollingTask = new ListCapabilityPollingTask(mContext).execute();
            }
        }
        else
        {
            Log.d(TAG, "AppGlobalState.ATT_MODE");
            ArrayList<Contact> contacts = AppGlobalState.getContacts();
            int listCount = 0;

            for (int i = 0; i < contacts.size(); i++) {
                if (contacts.get(i).isMultiSelected()) {
                    listCount++;
                }
            }

            Log.d(TAG, "invokeListSubscribeSimple listCount " + listCount);

            if (listCount > 0)
            {
                if (listCount == 1)
                {
                    if (Utility.getNotifyFmt(mContext) == AppGlobalState.NOTIFY_FMT_STRUCT) {
                        sendLogMesg(mContext, "List subscribe polling fmt=STRUCT based.");
                        mSubscribePollingTask = new ListAvailabilityFetchTask(mContext).execute();
                    }
                }
                else
                {
                    if (Utility.getNotifyFmt(mContext) == AppGlobalState.NOTIFY_FMT_STRUCT) {
                        sendLogMesg(mContext, "List subscribe simple fmt=STRUCT based.");
                        mSubscribePollingTask = new ListCapabilityPollingTask(mContext).execute();
                    }
                }
            }
            else
            {
                Toast.makeText(mContext, "None of the contact selected for list subscription.",
                        Toast.LENGTH_SHORT).show();
            }
        }
    }

    private void selectAllContacts(boolean flag) {
        for (Contact c : AppGlobalState.getContacts()) {
            if (Utility.isContactExcluded(c)) {
                c.setContactExcluded();
                c.setMultiSelected(false);
            } else {
                c.setMultiSelected(flag);
            }
        }
        AppGlobalState.getMainListAdapter().notifyDataSetChanged();
    }

    private void sendLogMesg(Context c, String string) {

        Utility.sendLogMesg(c, string);

    }

    private void startPublishActivity() {
        Intent i = new Intent();
        i.setClassName("com.qualcomm.qti.presenceapp",
                "com.qualcomm.qti.presenceapp.Publish");
        startActivity(i);
    }

    private void startMyInfoActivity() {
        Intent i = new Intent();
        i.setClassName("com.qualcomm.qti.presenceapp",
                "com.qualcomm.qti.presenceapp.MyInfo");
        startActivity(i);
    }

    private void startParActivity() {
        Intent i = new Intent();
        i.setClassName("com.qualcomm.qti.presenceapp",
                "com.qualcomm.qti.presenceapp.Par");
        startActivity(i);
    }

    private void startSettingsActivity() {
        Intent i = new Intent();
        i.setClassName("com.qualcomm.qti.presenceapp",
                "com.qualcomm.qti.presenceapp.Settings");
        startActivity(i);

    }

    private void dumpContacts(ArrayList<Contact> contacts) {
        for (Contact c : contacts) {
            Log.d(TAG, "Contact=" + c);
        }
    }

    private void startLiveLoggingActivity() {
        Intent i = new Intent();
        i.setClassName("com.qualcomm.qti.presenceapp",
                "com.qualcomm.qti.presenceapp.LiveLoggingActivity");
        startActivity(i);

    }

    private void linkArrayWithDisplayedList(ArrayList<Contact> contacts) {
        mAdapter = new ContactArrayAdapter<Contact>(this,
                R.layout.custom, R.id.name, contacts);

        setListAdapter(mAdapter);

        AppGlobalState.setMainListAdapter(mAdapter);
    }

    @Override
    protected void onListItemClick(ListView l, View v,
            int position, long id) {
        super.onListItemClick(l, v, position, id);
        Log.d(TAG, "selected=" + position);
        Log.d(TAG, "Contact=" + mContacts.get(position));
        startContactInfoActivity(position);
    }

    private void startContactInfoActivity(int index) {
        Intent i = new Intent();
        i.setClassName("com.qualcomm.qti.presenceapp",
                "com.qualcomm.qti.presenceapp.ContactInfo");
        i.putExtra("ContactIndex", index);

        sendLogMesg(mContext, "Starting ContactInfo");

        startActivity(i);
    }

    private ArrayList<Contact> getContactsFromDB() {
        ArrayList<Contact> contacts = new ArrayList<Contact>();

        /*
         * 1. Use the contacts.content_uri to get the id of the contacts, 2.
         * then query data.content_uri to get name and phone number. 3. Update
         * the arrayList contacts.
         */
        /*
         * Get a better way to get name and phone number from phoneDB.
         */

        Cursor contactsCursor = getContactsContentCursor();

        for (int i = 0; contactsCursor.moveToNext(); i++) {
            String id = getContactIdFromCursor(contactsCursor);
            Log.d(TAG, "getContactsFromDB() - "+id);
            Cursor dataCursor = getDataCursorRelatedToId(id);
            populateContactDataFromCursor(contacts, dataCursor);

            dataCursor.close();
        }
        Log.d("presence_ui", "data Before Cursor.close");
        contactsCursor.close();
        Log.d("presence_ui", "data After Cursor.close");
        return contacts;
    }

    private String getContactIdFromCursor(Cursor contactsCursor) {
        String id = contactsCursor.getString(contactsCursor
                .getColumnIndex(ContactsContract.Contacts._ID));
        return id;
    }

    private void populateContactDataFromCursor(ArrayList<Contact> contacts,
            Cursor dataCursor) {
        int nameIdx = dataCursor
                .getColumnIndex(ContactsContract.Data.DISPLAY_NAME);
        int phoneIdx = dataCursor
                .getColumnIndex(ContactsContract.CommonDataKinds.Phone.NUMBER);
        SharedPreferences preferences = this.getSharedPreferences(
                "presencedata", Context.MODE_PRIVATE);

        if (dataCursor.moveToFirst()) {
            // Extract the name.
            String name = dataCursor.getString(nameIdx);
            Log.d(TAG, "name from phonebook= " + name);
            // Extract the phone number.
            String rawNumber = dataCursor.getString(phoneIdx);
            Log.d(TAG, "rawNumber= " + rawNumber);
            if(rawNumber != null && rawNumber.length() > 0)
            {
                String number = getNormalizedNumber(rawNumber);
                Log.d(TAG, "number= " + number);
                if (number.length() > 0)
                {
                    Log.d(TAG, "number is not empty:Length:"+number.length());
                    if ((AppGlobalState.getOperatorMode() == AppGlobalState.VZW_MODE)
                     || (AppGlobalState.getOperatorMode() == AppGlobalState.TMO_MODE))
                    {
                        contacts.add(new Contact(name, number, 0, "<Not Subscribed>",""));
                    }
                    else
                    {
                        Log.d(TAG, "setting " + preferences.getString("INDIVIDUAL_PRIFIX", ""));
                        Log.d(TAG, "setting " + preferences.getString("INDIVIDUAL_SUFFIX", ""));
                        if (!(preferences.getString("INDIVIDUAL_PRIFIX", "").equals(""))
                                || !(preferences.getString("INDIVIDUAL_SUFFIX", "").equals("")))
                        {
                            contacts.add(new Contact(name, number, 0, "<Not Subscribed>",preferences
                                    .getString("INDIVIDUAL_PRIFIX", "")
                                    + number
                                    + preferences.getString("INDIVIDUAL_SUFFIX", "")));
                        }
                        else
                        {
                            contacts.add(new Contact(name, number, 0, "<Not Subscribed>",""));
                        }
                    }
                }
            }
        }
    }

    private String getNormalizedNumber(String rawNumber) {
        Log.d(TAG, "getNormalizedNumber:RawNumber from phonebook= " + rawNumber);
        /*
         * remove "(", ")", "-" and "." from the number. Only integers or + is
         * allowed.
         */
        int len = rawNumber.length();
        String out = new String();
        Log.d(TAG, "getNormalizedNumber() : len= " + len);

        if(len > 0) {
        if (rawNumber.charAt(0) == '+') {
            out = "+";
        }

        for (int i = 0; i < len; i++) {
            char c = rawNumber.charAt(i);
            if (Character.isDigit(c)) {
                out += c;
            }
        }
        Log.d(TAG, "Normalized number= " + out);
        }
        return out;
    }

    private Cursor getDataCursorRelatedToId(String id) {
        String where = ContactsContract.Data.CONTACT_ID + " = " + id;

        Cursor dataCursor = getContentResolver().query(
                ContactsContract.Data.CONTENT_URI, null, where, null, null);
        return dataCursor;
    }

    private Cursor getContactsContentCursor() {
        Uri phoneBookContentUri = ContactsContract.Contacts.CONTENT_URI;
        String recordsWithPhoneNumberOnly = ContactsContract.Contacts.HAS_PHONE_NUMBER
                + "='1'";

        Cursor contactsCursor = getContentResolver().query(phoneBookContentUri, null,
                recordsWithPhoneNumberOnly, null, null);
        return contactsCursor;
    }

    static public class PresenceSolResponse {
        public int result;
        public Object data;
    }

    boolean bindSettingService()
    {
        PackageManager pm = this.getPackageManager();
        Intent i = new Intent();
        i.setAction("com.qualcomm.qti.rcsimssettings.QrcsImsSettingsService");
        List<ResolveInfo> l = pm.queryIntentServices(i, 0);
        Log.d("PRESENCE_UI", "After Bind service isIMSServBound l.size =   "+l.size());
        if(l.size() > 0)
        {
            ServiceInfo servInfo = l.get(0).serviceInfo;
            ComponentName name = new ComponentName(servInfo.applicationInfo.packageName, servInfo.name);
            Intent settingServiceIntent = new Intent();
            settingServiceIntent.setComponent(name);
            return bindService(settingServiceIntent, mQImsServConn, Context.BIND_AUTO_CREATE);
        }
        else
        {
            Log.d("PRESENCE_UI", " bindSettingService    no active setting service ");
            return false;
        }

    }

    boolean bindRcsService()
    {
        boolean bServStatus = false;
        Log.d("PRESENCE_UI", " bindRcsService Start");
        ImsUceManager uceMgr = AppGlobalState.GetUceManager();
        IUceService rcsService = null;
        if(uceMgr != null){
            Log.d("PRESENCE_UI", " bindRcsService found ImsUceManager");
            uceMgr.createUceService(false);
            rcsService = uceMgr.getUceServiceInstance();
            if(rcsService != null) {
                Log.d("PRESENCE_UI", " bindRcsService found IRCSService");
                AppGlobalState.setIqRcsService(rcsService);
                initAllServices(rcsService);
                bServStatus = true;
            }
        }
        return bServStatus;

        /** Uncomment if using Proprietary RcsService

        PackageManager pm = this.getPackageManager();
        Intent i = new Intent();
        i.setAction("com.qualcomm.qti.rcsservice.QRCSService");
        List<ResolveInfo> l = pm.queryIntentServices(i, 0);
        Log.d("PRESENCE_UI", " bindRcsService l.size =   "+l.size());
        if(l.size() > 0)
        {
            ServiceInfo servInfo = l.get(0).serviceInfo;
            ComponentName name = new ComponentName(servInfo.applicationInfo.packageName, servInfo.name);
            Intent rcsServiceIntent = new Intent();
            rcsServiceIntent.setComponent(name);
            return bindService(rcsServiceIntent, mQrcsServConn, Context.BIND_AUTO_CREATE);
        }
        else
        {
            Log.d("PRESENCE_UI", " bindRcsService    no active setting service ");
            return false;
        } **/

    }


	void releaseServices() {

		if (AppGlobalState.getIqRcsService() != null) {

			// CDService
			if (AppGlobalState.isCdByOption()
					&& AppGlobalState.getCdServiceHandle() != 0) {

				try {

					AppGlobalState.getIqRcsService().destroyOptionsService(
							AppGlobalState.getCdServiceHandle());
				} catch (Exception e) {
					e.printStackTrace();
				}

				AppGlobalState.setCdServiceHandle(0);

				AppGlobalState.setCdService(null);

			}

			// PresenceService
			if (AppGlobalState.getPresenceSerrviceHandle() != 0) {

				try {

					AppGlobalState.getPresenceService().removeListener(
							AppGlobalState.getPresenceSerrviceHandle(),
							AppGlobalState.getPresenceListenerHandle());
				} catch (Exception e) {
					e.printStackTrace();
				}

				try {
					AppGlobalState.getIqRcsService().destroyPresenceService(
							AppGlobalState.getPresenceSerrviceHandle());
				} catch (Exception e) {
					e.printStackTrace();
				}

				AppGlobalState.setPresenceSerrviceHandle(0);

				AppGlobalState.setPresenceService(null);

			}

			// RcsService
				AppGlobalState.setIqRcsService(null);
		}

	}

}
