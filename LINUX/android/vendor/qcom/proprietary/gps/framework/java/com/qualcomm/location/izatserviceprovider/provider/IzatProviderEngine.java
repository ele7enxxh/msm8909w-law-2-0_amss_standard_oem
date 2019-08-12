/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION

  Izat Provider Engine

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.

  Not a Contribution, Apache license notifications and
  license are retained for attribution purposes only.
=============================================================================*/

package com.qualcomm.location.izatserviceprovider.provider;

import java.lang.reflect.Field;

import java.util.ArrayList;
import java.util.Properties;
import java.util.List;
import java.util.HashMap;
import java.util.Properties;

import java.io.IOException;


import android.util.Log;
import android.util.Slog;

import android.content.Context;
import android.content.res.Resources;

import android.location.Location;
import android.location.LocationProvider;
import android.location.LocationListener;
import android.location.LocationRequest;
import android.location.LocationManager;

import android.text.TextUtils;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.WorkSource;
import android.os.IBinder;
import android.os.RemoteException;
import android.os.WorkSource;
import android.os.SystemProperties;
import android.os.UserHandle;
import android.os.ServiceManager;

import com.android.location.provider.LocationProviderBase;
import com.android.location.provider.ProviderRequestUnbundled;
import com.android.location.provider.LocationRequestUnbundled;

import android.content.Intent;

import com.android.internal.app.IAppOpsService;
import android.app.AppOpsManager;

public class IzatProviderEngine {

    static {
        nativeOnClassLoad ();
    }

    public static IzatProviderEngine getInstance
    (
        Context context,
        Looper looper
    )
    {
        if (VERBOSE_DBG) Log.v (TAG, "getInstance");
        if (mInstance == null) {
            mInstance = new IzatProviderEngine (context, looper);
        }
        return mInstance;
    }

    // Instance will only be destroyed if there are no active clients
    public static void destroyInstance () {

        if (VERBOSE_DBG) Log.v (TAG, "destroyInstance");

        if (mInstance.mProvidersByName.isEmpty ()) {
            if (VERBOSE_DBG)
                Log.v
                (
                    TAG,
                    "No Active Clients for IzatProviderEngine. Destroying..."
                );
            Object lock = new Object ();
            try {

                synchronized (lock) {
                    mInstance.mHandler.obtainMessage
                    (
                        MSG_DESTROY_ENGINE,
                        lock
                    ).sendToTarget ();

                    if (VERBOSE_DBG)
                        Log.v (TAG, "Waiting for residual handling of msgs...");

                    while (!mInstance.mDoneFlag) lock.wait ();

                    if (VERBOSE_DBG)
                        Log.v
                        (
                            TAG,
                            "Residual handling of msgs finished. Exiting.."
                        );
                    mInstance.mRequestGenerator = null;
                    mInstance.mHandler = null;
                    mInstance = null;
                }
            } catch (InterruptedException e) {
                Log.v (TAG, e.getMessage ());
            }
        }
    }

    public IzatProviderEngine
    (
        Context context,
        Looper looper
    )
    {
        if (VERBOSE_DBG) Log.v (TAG, "Constructor");
        mContext = context;
        mDoneFlag = false;
        mHandler = new IzatProviderEngineMessageHandler (looper);
        mRequestGenerator = new LocationRequestGenerator ();
        Resources resources = mContext.getResources ();
        mAppOpsService = IAppOpsService.Stub.asInterface (ServiceManager.getService (Context.APP_OPS_SERVICE));
        mComboPackageName =
            resources.getString
            (
                com.android.internal.R.string.config_comboNetworkLocationProvider
            );
        nativeOnInstanceInit ();
    }

    public void subscribe
    (
        String providerName,
        LocationProviderBase provider
    )
    {
        mProvidersByName.put (providerName, provider);
        if (VERBOSE_DBG)
            Log.v
            (
                TAG,
                "subscribe - " + providerName
            );
    }

    public void unsubscribe (String providerName) {
        mProvidersByName.remove (providerName);
        if (VERBOSE_DBG)
            Log.v
            (
                TAG,
                "unsubscribe - " + providerName
            );
    }

    public void enable (String providerName) {
        if (VERBOSE_DBG) Log.v (TAG, "enable:" + providerName);
        mHandler.obtainMessage (MSG_ENABLE_PROVIDER, providerName)
                               .sendToTarget ();
    }

    public void disable (String providerName) {
        if (VERBOSE_DBG) Log.v (TAG, "disable:" + providerName);
        mHandler.obtainMessage (MSG_DISABLE_PROVIDER, providerName)
                               .sendToTarget ();
    }

    public void setRequest
    (
        ProviderRequestUnbundled request,
        WorkSource source,
        String providerName
    )
    {
        if (VERBOSE_DBG)
             Log.v
             (
                 TAG,
                 providerName + " called setRequest"
             );

        // Store WorkSource as current WorkSource
        mNewWorkSource = source;

        mHandler.obtainMessage
        (
            MSG_SET_REQUEST,
            new RequestWrapper (request, source, providerName)
        )
        .sendToTarget ();

    }

    public int getStatus (Bundle extras) {
        if (VERBOSE_DBG) Log.v (TAG, "getStatus");
        return LocationProvider.AVAILABLE;
    }

    public long getStatusUpdateTime () {
        if (VERBOSE_DBG) Log.v (TAG, "getStatusUpdateTime");
        return 0;
    }

    // Methods called from Native code
    private void onLocationChanged
    (
        String providerName,
        long utcTime,
        long elapsedRealTimeNanos,
        double latitude,
        double longitude,
        boolean hasAltitude,
        double altitude,
        boolean hasSpeed,
        float speed,
        boolean hasBearing,
        float bearing,
        boolean hasAccuracy,
        float accuracy
    )
    {
        if (VERBOSE_DBG)
             Log.v
             (
                 TAG,
                 "onLocationChanged:"+ providerName
             );
        Location location = new Location (providerName);
        location.setTime (utcTime);
        location.setElapsedRealtimeNanos (elapsedRealTimeNanos);
        location.setLatitude (latitude);
        location.setLongitude (longitude);
        location.setAltitude (altitude);
        location.setSpeed (speed);
        location.setBearing (bearing);
        location.setAccuracy (accuracy);
        location.makeComplete();

        if (mTestAutomationLoggingEnabled) {
            if (VERBOSE_DBG) {
                Log.v (TAG, "LocTech-Label :: OSDC :: Position Report In");
                Log.v (TAG, "LocTech-Value :: UTC Time: " + location.getTime () +
                       ", Elapsed Real Time Nanos: " + location.getElapsedRealtimeNanos () +
                       ", Latitude: " + location.getLatitude () +
                       ", Longitude: " + location.getLongitude () +
                       ", Altitude: " + location.getAltitude () +
                       ", Speed: " + location.getSpeed () +
                       ", Bearing: " + location.getBearing () +
                       ", Accuracy: " + location.getAccuracy () );
            }
        }

        String readyMarker;

        if (providerName.equals (LocationManager.NETWORK_PROVIDER)) {
            readyMarker = mComboPackageName + ".nlp:ready";
        }
        else {
            readyMarker = mComboPackageName + ".nlp:ready"; // TODO ?
        }

        location = markLocationReady (location, readyMarker);

        mHandler.obtainMessage (MSG_LOCATION_CHANGED, location).sendToTarget ();

    }

    private void onStatusChanged (String providerName, int status) {
        if (VERBOSE_DBG) {
            Log.v (TAG, "LocTech-Label :: OSDC :: Status Report In");
            Log.v (TAG, "LocTech-Value :: " + "Provider Name: " + providerName + " Status: " + status);
        }
        mHandler.obtainMessage (MSG_STATUS_CHANGED, status).sendToTarget ();
    }

    // Helpers
    private Location markLocationReady (Location location, String readyMarker) {
        if (VERBOSE_DBG) Log.v (TAG, "markLocationReady");
        Bundle extras = location.getExtras ();
        if (extras == null) {
            extras = new Bundle ();
        }
        extras.putBoolean (readyMarker, true);
        location.setExtras (extras);
        return location;
    }

    private void printList (List <LocationRequestWrapper> listIn, String tag, String listName) {
        if (VERBOSE_DBG) Log.v (tag, listName);
        for (LocationRequestWrapper e : listIn) {
            String prefix = new String ();
            if (listName.contains ("LocTech-Label")) {
                prefix = "LocTech-Value :: ";
            }
            if (VERBOSE_DBG) {
                Log.v (tag, prefix + "Quality:" + e.mLocReq.getQuality () +
                       ", Interval: " + e.mLocReq.getInterval () +
                       ", Displacement: " + e.mLocReq.getSmallestDisplacement () +
                       ", Num Updated: " + e.mLocReq.getNumUpdates () +
                       ", Provider: " + e.mLocReq.getProvider () );
            }
        }
    }

    private void startHighPowerLocationMonitoringOnClientUids (WorkSource newWork) {
        if (newWork != null) {
            for (int i=0; i<newWork.size (); i++) {
                try {
                    int uid = newWork.get (i);
                    String packageName = newWork.getName (i);
                    mAppOpsService.startOperation (AppOpsManager.getToken (mAppOpsService),
                            AppOpsManager.OP_MONITOR_HIGH_POWER_LOCATION, uid, packageName);
                    if (VERBOSE_DBG) {
                        Log.v (TAG, "startOp - uid:" + uid + " packageName:" + packageName);
                    }
                } catch (RemoteException e) {
                    Log.w(TAG, "RemoteException", e);
                }
            }
        }
    }

    private void finishHighPowerLocationMonitoringOnClientUids (WorkSource goneWork) {
        if (goneWork != null) {
            for (int i=0; i<goneWork.size (); i++) {
                try {
                    int uid = goneWork.get (i);
                    String packageName = goneWork.getName (i);
                    mAppOpsService.finishOperation (AppOpsManager.getToken (mAppOpsService),
                            AppOpsManager.OP_MONITOR_HIGH_POWER_LOCATION, uid, packageName);
                    if (VERBOSE_DBG) {
                        Log.v (TAG, "finishOp - uid:" + uid + " packageName:" + packageName);
                    }
                } catch (RemoteException e) {
                    Log.w(TAG, "RemoteException", e);
                }
            }
        }
    }

    private void handleSetRequest (RequestWrapper wrapper) {
        if (VERBOSE_DBG) Log.v (TAG, "handleSetRequest");

        try {

            boolean reportLocation = wrapper.request.getReportLocation ();
            if (VERBOSE_DBG) Log.v (TAG, "Report Location : " + reportLocation);

            List <LocationRequestUnbundled> temp =
                wrapper.request.getLocationRequests ();

            if (wrapper.pn.equals ("fused")) {
                if (mNavigating) {
                    WorkSource localWorkSource = null;
                    if (!reportLocation) {
                        localWorkSource = new WorkSource ();
                    } else {
                        localWorkSource = mNewWorkSource;
                    }
                    WorkSource [] changes = mActiveWorkSource.setReturningDiffs (localWorkSource);
                    if (changes != null) {
                        // start for new work
                        startHighPowerLocationMonitoringOnClientUids (changes [0]);
                        // finish for gone work
                        finishHighPowerLocationMonitoringOnClientUids (changes [1]);
                    }
                } else {
                    mActiveWorkSource = mNewWorkSource;
                }
            }

            List <LocationRequestWrapper> newList =
                new ArrayList <LocationRequestWrapper> ();

            for (LocationRequestUnbundled e : temp) {

                LocationRequest req = new LocationRequest ();

                req.setQuality (e.getQuality ());

                req.setInterval (e.getInterval ());

                req.setSmallestDisplacement
                (
                    e.getSmallestDisplacement ()
                );

                // use reflection to get num updates
                Field locationRequestField =
                    LocationRequestUnbundled.class.getDeclaredField
                    (
                        "delegate"
                    );

                locationRequestField.setAccessible (true);

                req.setNumUpdates
                (
                    ((LocationRequest) locationRequestField.get (e))
                    .getNumUpdates ()
                );

                req.setProvider (wrapper.pn);

                newList.add (new LocationRequestWrapper (req));

            }

           mRequestGenerator.generateAddRemoveRequests
           (
               newList,
               reportLocation,
               wrapper.pn
           );
        } catch (Exception e) {
            Log.w(TAG, "Exception ", e);
        }
    }

    private final class LocationRequestWrapper {
        public LocationRequestWrapper (LocationRequest locationRequest) {
            mLocReq = new LocationRequest ();
            mLocReq.setQuality (locationRequest.getQuality ());
            mLocReq.setInterval (locationRequest.getInterval ());
            mLocReq.setFastestInterval (locationRequest.getFastestInterval ());
            mLocReq.setExpireAt (locationRequest.getExpireAt ());
            mLocReq.setNumUpdates (locationRequest.getNumUpdates ());
            mLocReq.setSmallestDisplacement
            (
                locationRequest.getSmallestDisplacement ()
            );
            mLocReq.setWorkSource (locationRequest.getWorkSource ());
            mLocReq.setHideFromAppOps (locationRequest.getHideFromAppOps ());
            mLocReq.setProvider (locationRequest.getProvider ());
        }

        public boolean equals (Object rhs) {
            if (rhs == this) {
                return true;
            }
            if (rhs == null || rhs.getClass () != this.getClass ()) {
                return false;
            }

            LocationRequestWrapper lrw = (LocationRequestWrapper) rhs;
            return
            (
                (
                    mLocReq.getFastestInterval () ==
                    lrw.mLocReq.getFastestInterval ()
                )
                &&
                (
                    mLocReq.getInterval () == lrw.mLocReq.getInterval ()
                )
                &&
                (
                    mLocReq.getQuality () == lrw.mLocReq.getQuality ()
                )
                &&
                (
                    mLocReq.getNumUpdates () == lrw.mLocReq.getNumUpdates ()
                )
                &&
                (
                    mLocReq.getSmallestDisplacement () ==
                    lrw.mLocReq.getSmallestDisplacement ()
                )
            );
        }

        public int hashCode () {
            final int prime = 31;
            int result = 1;
            result = prime * result + mLocReq.getQuality ();
            result = prime * result + mLocReq.getNumUpdates ();
            result = prime * result + (int) (mLocReq.getFastestInterval () ^ (mLocReq.getFastestInterval () >>> 32));
            result = prime * result + (int) (mLocReq.getInterval () ^ (mLocReq.getFastestInterval () >>> 32));
            result = prime * result + Float.floatToIntBits (mLocReq.getSmallestDisplacement ());
            return result;
        }
        public LocationRequest mLocReq;
    }

    // Nested classes
    private static class RequestWrapper {
        public ProviderRequestUnbundled request;
        public WorkSource source;
        public String pn; // where this request is originating from
        public RequestWrapper
        (
            ProviderRequestUnbundled request,
            WorkSource source,
            String providerName
        )
        {
            this.request = request;
            this.source = source;
            this.pn = providerName;
        }
    }

    private class LocationRequestGenerator {

        public LocationRequestGenerator () {}

        public void generateAddRemoveRequests
        (
            List <LocationRequestWrapper> newListIn,
            boolean reportLocationIn,
            String providerName
        )
        {
            if (VERBOSE_DBG) Log.v ("LocationRequestGenerator", "generateAddRemoveRequests");
            if (newListIn == null)
                return;

            List <LocationRequestWrapper> currentlyActiveList =
                mCurrentRequestsByProvider.get(providerName);
            if (null == currentlyActiveList)
            {
                currentlyActiveList = new ArrayList<LocationRequestWrapper>();
                mCurrentRequestsByProvider.put(providerName, currentlyActiveList);
            }

            List <LocationRequestWrapper> addListOut = new ArrayList <LocationRequestWrapper> ();
            List <LocationRequestWrapper> removeListOut = new ArrayList <LocationRequestWrapper> ();

            if (mTestAutomationLoggingEnabled) {
                printList (newListIn, "LocationRequestGenerator", "LocTech-Label :: OSDC :: Location Request In");
            }

            if (VERBOSE_DBG) {
                printList (newListIn, "LocationRequestGenerator", "newListIn");
                printList
                (
                    currentlyActiveList,
                    "LocationRequestGenerator",
                    "currentlyActiveList Before"
                );
            }

            // remove all or cancel only
            if (!reportLocationIn) {

                if (!currentlyActiveList.isEmpty ()) {
                    removeListOut.addAll(currentlyActiveList);
                    currentlyActiveList.clear ();
                }
                if (VERBOSE_DBG) {
                    printList
                    (
                        currentlyActiveList,
                        "LocationRequestGenerator",
                        "currentlyActiveList After"
                    );
                }

                if (VERBOSE_DBG) {
                    printList
                    (
                        removeListOut,
                        "LocationRequestGenerator",
                        "removeListout"
                    );
                }


                sendRequests (addListOut, removeListOut);
                return;
            }

            // newListIn will become currentActiveList, but before that we
            // need to find out the list elements to add and remove
            // removeListOut is filled with the previous list of requests.
            // For every request coming in newListIn, if it is present in
            // removeListOut, that request is removed (meaning that the
            // request remains), otherwise that request is added to the
            // list of new request (addListOut)
            // At the end, addListOut contains all the new requests and
            // removeListOut contains only the requests that have not been
            // removed because they are not present in newListIn and,
            // therefore, must be removed
            removeListOut.addAll(currentlyActiveList);
            for (LocationRequestWrapper e : newListIn) {
                if (!removeListOut.remove(e)) {
                    addListOut.add(e);
                }
            }

            // newListIn becomes currentActiveList
            currentlyActiveList.clear();
            currentlyActiveList.addAll(newListIn);

            if (VERBOSE_DBG) printList (addListOut, "LocationRequestGenerator", "addListOut");
            if (VERBOSE_DBG) printList (removeListOut, "LocationRequestGenerator", "removeListOut");
            if (VERBOSE_DBG) {
                printList
                (
                    currentlyActiveList,
                    "LocationRequestGenerator",
                    "currentlyActiveList After"
                );
            }

            sendRequests (addListOut, removeListOut);
        }

        private void sendRequests
        (
            List <LocationRequestWrapper> addListOut,
            List <LocationRequestWrapper> removeListOut
        )
        {
            for (LocationRequestWrapper e : addListOut) {

                nativeOnAddRequest
                (
                    e.mLocReq.getProvider (),
                    e.mLocReq.getNumUpdates (),
                    e.mLocReq.getInterval (),
                    e.mLocReq.getSmallestDisplacement (),
                    e.mLocReq.getQuality ()
                );
            }

            for (LocationRequestWrapper e : removeListOut) {

                nativeOnRemoveRequest
                (
                    e.mLocReq.getProvider (),
                    e.mLocReq.getNumUpdates (),
                    e.mLocReq.getInterval (),
                    e.mLocReq.getSmallestDisplacement (),
                    e.mLocReq.getQuality ()
                );
            }
        }

        private HashMap<String, List<LocationRequestWrapper>> mCurrentRequestsByProvider =
            new HashMap<String, List<LocationRequestWrapper>>(2); // 2 is the expected number of providers
    }


    private final class IzatProviderEngineMessageHandler extends Handler {

        public IzatProviderEngineMessageHandler (Looper looper) {
            super (looper);
            if (VERBOSE_DBG)
                Log.v ("IzatProviderEngineMessageHandler", "Constructor");
        }

        // Message handler
        public void handleMessage (Message msg) {

            int msgID = msg.what;

            if (VERBOSE_DBG)
                Log.v
                (
                    "IzatProviderEngineMessageHandler",
                    "handleMessage what - " + msgID
                );

            switch (msgID) {

                case MSG_ENABLE_PROVIDER:
                {
                    String providerName = (String) msg.obj;
                    if (VERBOSE_DBG)
                        Log.v (TAG, "MSG_ENABLE_PROVIDER : " + providerName);
                    nativeOnEnableProvider (providerName);
                    break;
                }

                case MSG_DISABLE_PROVIDER:
                {
                    String providerName = (String) msg.obj;
                    if (VERBOSE_DBG)
                        Log.v (TAG, "MSG_DISABLE_PROVIDER : " + providerName);
                    nativeOnDisableProvider (providerName);
                    break;
                }

                case MSG_SET_REQUEST:
                {
                    RequestWrapper wrapper = (RequestWrapper) msg.obj;
                    if (VERBOSE_DBG)
                        Log.v (TAG, "MSG_SET_REQUEST : " + wrapper.pn);

                    handleSetRequest (wrapper);

                    break;

                }
                case MSG_LOCATION_CHANGED:
                {
                    Location location = (Location) msg.obj;
                    if (VERBOSE_DBG)
                        Log.v
                        (
                            TAG,
                            "MSG_LOCATION_CHANGED : " + location.getProvider ()
                        );
                    LocationProviderBase locationProviderBase =
                        mProvidersByName.get
                        (
                            location.getProvider ()
                        );
                    if (locationProviderBase != null) {
                        locationProviderBase.reportLocation (location);
                    }
                    break;
                }
                case MSG_STATUS_CHANGED:
                {
                    int status = (int) msg.obj;
                    if (VERBOSE_DBG) {
                        Log.v (TAG, "MSG_STATUS_CHANGED: Status: " + status);
                    }
                    boolean wasNavigating = mNavigating;
                    switch (status) {
                        case GNSS_SESSION_BEGIN:
                        {
                            mNavigating = true;
                            mEngineOn = true;
                            break;
                        }
                        case GNSS_ENGINE_ON:
                        {
                            mEngineOn = true;
                            break;
                        }
                        case GNSS_SESSION_END:
                        {
                            mNavigating = false;
                            break;
                        }
                        case GNSS_ENGINE_OFF:
                        {
                            mEngineOn = false;
                            mNavigating = false;
                            break;
                        }
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                        default:
                        {
                            break;
                        }
                    }
                    if (wasNavigating != mNavigating) {
                        // We got session Begin event
                        if (mNavigating) {
                            // This means mActiveWorkSource has clients who are interested in high accuracy/high power request.
                            // so start monitoring for it.
                            startHighPowerLocationMonitoringOnClientUids (mActiveWorkSource);
                        } else {
                            // We got Session End or Engine off event for one of the following reasons:
                            //  1.  GPS setting was turned OFF in the UI.
                            //  2.  GPS Location request for uids in mActiveWorkSource finished.
                            // So finish monitoring
                            finishHighPowerLocationMonitoringOnClientUids (mActiveWorkSource);
                        }
                        // Send an intent to toggle the GPS icon
                        Intent intent = new Intent (LocationManager.HIGH_POWER_REQUEST_CHANGE_ACTION);
                        mContext.sendBroadcastAsUser(intent, UserHandle.ALL);
                    }
                    break;
                }
                case MSG_DESTROY_ENGINE:
                {
                    Object lock = msg.obj;
                    synchronized (lock) {
                        if (VERBOSE_DBG)
                            Log.v
                            (
                                TAG,
                                "MSG_DESTROY_ENGINE"
                            );
                        nativeOnInstanceDeinit ();
                        mDoneFlag = true;
                        lock.notify ();
                    }
                    break;
                }
            }
        }
    }

    // Native method declarations
    private native static void nativeOnClassLoad ();
    private native void nativeOnInstanceInit ();
    private native void nativeOnInstanceDeinit ();
    private native void nativeOnEnableProvider (String providerName);
    private native void nativeOnDisableProvider (String providerName);
    private native void nativeOnAddRequest
    (
        String providerName,
        int numUpdates,
        long interval,
        float smallestDisplacement,
        int accuracy
    );
    private native void nativeOnRemoveRequest
    (
        String providerName,
        int numUpdates,
        long interval,
        float smallestDisplacement,
        int accuracy
    );

    // member constants
    private static final int MSG_ENABLE_PROVIDER        = 111;
    private static final int MSG_DISABLE_PROVIDER       = 222;
    private static final int MSG_SET_REQUEST            = 333;
    private static final int MSG_LOCATION_CHANGED       = 444;
    private static final int MSG_STATUS_CHANGED         = 555;
    private static final int MSG_DESTROY_ENGINE         = 666;

    private static final int GNSS_SESSION_BEGIN         = 4;
    private static final int GNSS_SESSION_END           = 5;
    private static final int GNSS_ENGINE_ON             = 6;
    private static final int GNSS_ENGINE_OFF            = 7;

    private static final String TAG                     = "IzatProviderEngine";
    private static final boolean VERBOSE_DBG            = Log.isLoggable(TAG, Log.VERBOSE);
    private static final String PROPERTIES_FILE_PREFIX  = "/etc/gps";
    private static final String PROPERTIES_FILE_SUFFIX  = ".conf";
    private static final String DEFAULT_PROPERTIES_FILE = PROPERTIES_FILE_PREFIX + PROPERTIES_FILE_SUFFIX;


    private static IzatProviderEngine mInstance = null;
    private LocationRequestGenerator      mRequestGenerator;
    private final HashMap <String, LocationProviderBase> mProvidersByName =
        new HashMap <String, LocationProviderBase> ();

    private Context                              mContext;
    private IzatProviderEngineMessageHandler     mHandler;
    private String                               mComboPackageName;
    private boolean                              mDoneFlag;
    private boolean                              mTestAutomationLoggingEnabled = true;
    private boolean                              mNavigating;
    private boolean                              mEngineOn;

    private final IAppOpsService                 mAppOpsService;
    private WorkSource                           mNewWorkSource = null;
    private WorkSource                           mActiveWorkSource = new WorkSource (); // Initialize using default ctor
}
