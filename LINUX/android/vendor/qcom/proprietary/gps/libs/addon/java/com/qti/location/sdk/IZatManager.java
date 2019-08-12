/* ======================================================================
*  Copyright (c) 2015 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.location.sdk;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.util.Log;
import android.os.ServiceManager;
import android.os.RemoteException;
import android.content.pm.ResolveInfo;
import android.location.Location;

import java.lang.IllegalArgumentException;
import java.util.HashMap;
import java.util.Map;
import java.util.Collections;

import com.qualcomm.location.izat.IIzatService;
import com.qualcomm.location.izat.flp.IFlpService;
import com.qualcomm.location.izat.flp.ILocationCallback;
import com.qualcomm.location.izat.flp.ITestService;
import com.qualcomm.location.izat.flp.IMaxPowerAllocatedCallback;
import com.qualcomm.location.izat.geofence.IGeofenceService;
import com.qualcomm.location.izat.geofence.IGeofenceCallback;

/**
 * <p>Copyright (c) 2015 Qualcomm Technologies, Inc.</p>
 * <p>All Rights Reserved.</p>
 * <p>Confidential and Proprietary - Qualcomm Technologies, Inc</p>
 * <br/>
 * <p><b>IZatManager</b> class is the entry point to IZat location services.
 * It requires the permission of android.permission.ACCESS_FINE_LOCATION.</p>
 * <pre>
 * <code>
 *    // Sample Code
 *
 *    import android.app.Activity;
 *    import android.os.Bundle;
 *    import android.view.View;
 *    import android.widget.Button;
 *    import com.qti.location.sdk.IZatManager;
 *    import com.qti.location.sdk.IZatFlpService;
 *
 *    public class FullscreenActivity extends Activity {
 *
 *       private IZatManager mIzatMgr = null;
 *       private IZatFlpService mFlpSevice = null;
 *
 *       &#64Override
 *       protected void onCreate(Bundle savedInstanceState) {
 *
 *            ...
 *            // get the instance of IZatManager
 *            <b>mIzatMgr = IZatManager.getInstance(getApplicationContext());</b>
 *
 *            ...
 *            final Button connectButton = (Button)findViewById(R.id.connectButton);
 *            connectButton.setOnClickListener(new View.OnClickListener() {
 *                &#64Override
 *                public void onClick(View v) {
 *
 *                    // connect to IZatFlpService through IZatManager
 *                    if (mIzatMgr != null) {
 *                        <b>mFlpSevice = mIzatMgr.connectFlpService();</b>
 *                    }
 *                }
 *            });
 *            ...
 *        }
 *    }
 * </code>
 * </pre>
 */
public class IZatManager {
    private static String TAG = "IZatManager";
    private static final boolean VERBOSE = Log.isLoggable(TAG, Log.VERBOSE);
    private static final String SDK_Version = "1.0.0";

    private int mFlpFeaturMasks = -1;
    private final int FEATURE_BIT_TIME_BASED_BATCHING_IS_SUPPORTED = 1;
    private final int FEATURE_BIT_DISTANCE_BASED_TRACKING_IS_SUPPORTED = 2;
    private final int FEATURE_BIT_DISTANCE_BASED_BATCHING_IS_SUPPORTED = 8;

    private Map<IZatFlpServiceImpl.IZatSessionHandlerImpl, FlpRequestMapItem> mFlpRequestsMap
        = createIdMap();
    private Map<IZatFlpServiceImpl.IZatSessionHandlerImpl, FlpRequestMapItem> createIdMap() {
        Map<IZatFlpServiceImpl.IZatSessionHandlerImpl, FlpRequestMapItem> result =
            new HashMap<IZatFlpServiceImpl.IZatSessionHandlerImpl, FlpRequestMapItem>();
        return Collections.synchronizedMap(result);
    }

    private Map<IZatFlpService.IFlpLocationCallback,
                LocationCallbackWrapper> mFlpPassiveCbMap = createPassiveCbMap();
    private Map<IZatFlpService.IFlpLocationCallback,
                LocationCallbackWrapper> createPassiveCbMap() {
        Map<IZatFlpService.IFlpLocationCallback, LocationCallbackWrapper> result =
            new HashMap<IZatFlpService.IFlpLocationCallback, LocationCallbackWrapper>();
        return Collections.synchronizedMap(result);
    }

    private Map<IZatTestService.IFlpMaxPowerAllocatedCallback,
                MaxPowerAllocatedCallbackWrapper> mFlpMaxPowerCbMap = createMaxPowerCbMap();
    private Map<IZatTestService.IFlpMaxPowerAllocatedCallback,
                MaxPowerAllocatedCallbackWrapper> createMaxPowerCbMap() {
        Map<IZatTestService.IFlpMaxPowerAllocatedCallback,
            MaxPowerAllocatedCallbackWrapper> result =
            new HashMap<IZatTestService.IFlpMaxPowerAllocatedCallback,
                        MaxPowerAllocatedCallbackWrapper>();
        return Collections.synchronizedMap(result);
    }

    private Map<IZatGeofenceServiceImpl,
                IZatGeofenceService.IZatGeofenceCallback> mGeofenceClientCallbackMap
        = createGeofenceClientCallbackMap();
    private Map<IZatGeofenceServiceImpl,
                IZatGeofenceService.IZatGeofenceCallback> createGeofenceClientCallbackMap() {
        Map<IZatGeofenceServiceImpl, IZatGeofenceService.IZatGeofenceCallback> result =
            new HashMap<IZatGeofenceServiceImpl, IZatGeofenceService.IZatGeofenceCallback>();
        return Collections.synchronizedMap(result);
    }

    private Map<IZatGeofenceServiceImpl.IZatGeofenceHandleImpl,
                GeofenceMapItem> mGeofencesMap = createGeofencesMap();
    private Map<IZatGeofenceServiceImpl.IZatGeofenceHandleImpl,
                GeofenceMapItem> createGeofencesMap() {
        Map<IZatGeofenceServiceImpl.IZatGeofenceHandleImpl,
            GeofenceMapItem> result =
            new HashMap<IZatGeofenceServiceImpl.IZatGeofenceHandleImpl,
                        GeofenceMapItem>();
        return Collections.synchronizedMap(result);
    }

    private static final int FLP_PASSIVE_LISTENER_HW_ID = -1;
    private static final int FLP_RESULT_SUCCESS = 0;
    private static final int FLP_RESULT_FAILURE = -1;

    private static final int FLP_SEESION_BACKGROUND = 1;
    private static final int FLP_SEESION_FOREROUND = 2;
    private static final int FLP_SEESION_PASSIVE = 4;

    private static final int GEOFENCE_DWELL_TYPE_INSIDE = 1;
    private static final int GEOFENCE_DWELL_TYPE_OUTSIDE = 2;

    private static volatile int sFlpRequestsCnt = 0;
    private static final Object sFlpServiceLock = new Object();
    private static final Object sTestServiceLock = new Object();
    private static final Object sGeofenceServiceLock = new Object();

    private static final String REMOTE_IZAT_SERVICE_NAME = "com.qualcomm.location.izat.IzatService";
    private Context mContext;
    private static IZatManager sInstance = null;
    private static IIzatService sIzatService = null;
    private GeofenceStatusCallbackWrapper mGeofenceCbWrapper = new GeofenceStatusCallbackWrapper();

    /**
     * Return the instance of IZatManager.
     *
     * @param context a context object.
     * @throws IZatIllegalArgumentException if the context is null.
     * @return the instance of IZatManager
     */
    public static synchronized IZatManager getInstance(Context context)
        throws IZatIllegalArgumentException {
        if (context == null) {
            throw new IZatIllegalArgumentException("null argument");
        }

        if (sInstance == null) {
            sInstance = new IZatManager(context);
        }

        return sInstance;
    }

    private IZatManager(Context context) {
        mContext = context;
    }

    /**
     * Get the IZat SDK and IZat Service version
     * <p>
     * This api requires IZatService to be present, otherwise
     * a {@link IZatServiceUnavailableException}<br/>
     * will be thrown.<br/>
     * This returns version in format X.Y.Z:A.B.C where
     * XYZ are major, minor and revision ids for the SDK and
     * ABC are major, minor and revision ids for the service.
     * This api may take as long as several seconds.
     * It is recommended not to call this api in main thread.
     * </p>
     * @throws IZatServiceUnavailableException if the .IzatService is not present
     * @return the version of SDK and Service
     */
    public String getVersion()
        throws IZatServiceUnavailableException {
        String service_version;

        if (sIzatService == null) {
            connectIzatService();
        }

        try {
            service_version = sIzatService.getVersion();
        } catch (RemoteException e) {
            throw new RuntimeException("Failed to get IzatService version", e);
        }

        if (service_version == null) {
            service_version = "1.0.0";
        }

        String version = SDK_Version + ":" + service_version;

        return version;
    }

    private synchronized void connectIzatService() {
        if (sIzatService == null) {
            if (VERBOSE)
                Log.d(TAG, "Connecting to Izat service by name [" +
                      REMOTE_IZAT_SERVICE_NAME + "]");

            // check if Izat service installed
            PackageManager pm = mContext.getPackageManager();
            ResolveInfo ri = pm.resolveService(new Intent(REMOTE_IZAT_SERVICE_NAME), 0);
            if (ri == null) {
                Log.e(TAG, "Izat service (" + REMOTE_IZAT_SERVICE_NAME + ") not installed");
                throw new IZatServiceUnavailableException("Izat service unavailable.");
            }

            // check if Izat service started
            if (ServiceManager.getService(REMOTE_IZAT_SERVICE_NAME) == null) {
                Log.e(TAG, "Izat service (" + REMOTE_IZAT_SERVICE_NAME + ") is not started");
                throw new IZatServiceUnavailableException("Izat service not started.");
            }

            sIzatService =
                IIzatService.Stub.asInterface(ServiceManager.getService(REMOTE_IZAT_SERVICE_NAME));
            if(sIzatService == null) {
                Log.e(TAG, "Izat service (" + REMOTE_IZAT_SERVICE_NAME + ") not started");
                throw new IZatServiceUnavailableException("Izat service unavailable.");
            }
        }
    }

    /**
     * Get a {@link IZatFlpService} interface of FLP service.
     * <p>
     * This api requires IZat FLP service be present, otherwise
     * a {@link IZatServiceUnavailableException}<br/>
     * will be thrown.<br/>
     * This api may take as long as several seconds.
     * It is recommended not to call this api in main thread.
     * </p>
     *
     * @throws IZatServiceUnavailableException if the Flp Service does not present.
     * @return an {@link IZatFlpService} interface.
     */
    public IZatFlpService connectFlpService()
        throws IZatServiceUnavailableException {

        if (sIzatService == null) {
            connectIzatService();
        }

        try {
            IFlpService flpService = (IFlpService)(sIzatService.getFlpService());
            synchronized (sFlpServiceLock) {
                if (mFlpFeaturMasks == -1) {
                    mFlpFeaturMasks = flpService.getAllSupportedFeatures();
                }
            }
            if ((mFlpFeaturMasks & FEATURE_BIT_DISTANCE_BASED_TRACKING_IS_SUPPORTED)>0) {
                return new IZatFlpServiceImpl(flpService);
            } else {
                Log.e(TAG, "Izat FLP positioning is not supported on this device.");
                return null;
            }
        } catch (RemoteException e) {
            throw new RuntimeException("Failed to get IFlpService", e);
        }
    }

    /**
     * Disconnect form the specified {@link IZatFlpService} interface.
     *
     * @throws IZatIllegalArgumentException if the input service is null.
     * @param service handle object that is returned from
     * {@link IZatManager#connectFlpService()}.
     */
    public void disconnectFlpService(IZatFlpService service)
        throws IZatIllegalArgumentException {
        if (service == null || !(service instanceof IZatFlpServiceImpl))
            throw new IZatIllegalArgumentException();

        // stop all the undergoing sessions before disconnecting
        try {
            synchronized (sFlpServiceLock) {
                IFlpService flpService =
                   (IFlpService)(sIzatService.getFlpService());
                if (!mFlpRequestsMap.isEmpty()) {
                    FlpRequestMapItem mapItem = null;
                    for (IZatFlpServiceImpl.IZatSessionHandlerImpl key : mFlpRequestsMap.keySet()) {
                        mapItem = mFlpRequestsMap.get(key);
                        // stop session
                        if (flpService.stopFlpSession(mapItem.getHwId()) !=
                            FLP_RESULT_SUCCESS) {
                            Log.e(TAG, "stopFlpSession failed in disconnecting");
                            return;
                        }
                        if ((mFlpFeaturMasks &
                             FEATURE_BIT_DISTANCE_BASED_BATCHING_IS_SUPPORTED)>0) {
                            // un-register cb
                            if (mapItem.getNotifyType() ==
                                NotificationType.NOTIFICATION_WHEN_BUFFER_IS_FULL) {
                                flpService.unregisterCallback(FLP_SEESION_BACKGROUND,
                                                              mapItem.getCbWrapper());
                            }
                            if (mapItem.getNotifyType() ==
                                NotificationType.NOTIFICATION_ON_EVERY_LOCATION_FIX) {
                                flpService.unregisterCallback(FLP_SEESION_FOREROUND,
                                                              mapItem.getCbWrapper());
                            }
                        } else {
                            flpService.unregisterCallback(FLP_SEESION_PASSIVE,
                                                          mapItem.getCbWrapper());
                        }
                        // remove the item
                        mFlpRequestsMap.remove(key);
                    }
                }
                if (!mFlpPassiveCbMap.isEmpty()) {
                    // remove all passive listeners.
                    LocationCallbackWrapper cbWrapper = null;
                    for (IZatFlpService.IFlpLocationCallback key : mFlpPassiveCbMap.keySet()) {
                        cbWrapper = mFlpPassiveCbMap.get(key);
                        flpService.unregisterCallback(FLP_SEESION_PASSIVE,
                                                      cbWrapper);
                        mFlpPassiveCbMap.remove(key);
                    }
                }
            }
        } catch (RemoteException e) {
            throw new RuntimeException("Failed stop all flp sessions", e);
        }
    }

    /**
     * Connect to test service. (FOR INTERNAL USE ONLY)
     *
     * @hide
     */
    public IZatTestService connectTestService()
        throws IZatServiceUnavailableException {

        if (sIzatService == null) {
            connectIzatService();
        }

        try {
            ITestService testService = (ITestService)(sIzatService.getTestService());
            return new IZatTestServiceImpl(testService);
        } catch (RemoteException e) {
            throw new RuntimeException("Failed to get ITestService", e);
        }
    }

    /**
     * Disconnect from test service. (FOR INTERNAL USE ONLY)
     *
     * @hide
     */
    public void disconnectTestService(IZatTestService service)
        throws IZatIllegalArgumentException {
        if (service == null || !(service instanceof IZatTestServiceImpl))
            throw new IZatIllegalArgumentException();

        if (!mFlpMaxPowerCbMap.isEmpty()) {
            synchronized (sTestServiceLock) {
                for (IZatTestService.IFlpMaxPowerAllocatedCallback key :
                     mFlpMaxPowerCbMap.keySet()) {
                    service.deregisterForMaxPowerAllocatedChange(key);
                    mFlpMaxPowerCbMap.remove(key);
                }
            }
        }
    }

    /**
     * Get a {@link IZatGeofenceService} interface of Geofence service.
     * <p>
     * This api requires IZat Geofence service be present, otherwise
     * a {@link IZatServiceUnavailableException}<br/>
     * will be thrown.<br/>
     * This api may take as long as several seconds.
     * It is recommended not to call this api in main thread.
     * </p>
     *
     * @throws IZatServiceUnavailableException if the Geofence Service does not present.
     * @return an {@link IZatGeofenceService} interface.
     */
    public IZatGeofenceService connectGeofenceService()
        throws IZatServiceUnavailableException {
        if (sIzatService == null) {
            connectIzatService();
        }

        try {
            IGeofenceService geofenceService =
                (IGeofenceService)(sIzatService.getGeofenceService());
            // register the geofence callback
            synchronized (sGeofenceServiceLock) {
                geofenceService.registerCallback(mGeofenceCbWrapper);
            }
            return new IZatGeofenceServiceImpl(geofenceService);
        } catch (RemoteException e) {
            throw new RuntimeException("Failed to get IGeofenceService", e);
        }
    }

   /**
     * Disconnect form the specified {@link IZatGeofenceService} interface.
     *
     * @throws IZatIllegalArgumentException if the input service is null.
     * @param service handle object that is returned from
     * {@link IZatManager#connectGeofenceService()}.
     */
    public void disconnectGeofenceService(IZatGeofenceService service)
        throws IZatIllegalArgumentException {
        if (service == null || !(service instanceof IZatGeofenceServiceImpl))
            throw new IZatIllegalArgumentException();

        try {
            synchronized (sGeofenceServiceLock) {
                IGeofenceService geofenceService =
                   (IGeofenceService)(sIzatService.getGeofenceService());
                // remove all geofence added.
                if (!mGeofencesMap.isEmpty()) {
                    GeofenceMapItem mapItem = null;
                    for (IZatGeofenceServiceImpl.IZatGeofenceHandleImpl key :
                         mGeofencesMap.keySet()) {
                        mapItem = mGeofencesMap.get(key);
                        geofenceService.removeGeofence(mapItem.getHWGeofenceId());
                        mGeofencesMap.remove(key); // do not wait for the callback
                    }
                }
                // un-register the geofence callback
                geofenceService.unregisterCallback(mGeofenceCbWrapper);
                // delete the client callback
                if (!mGeofenceClientCallbackMap.isEmpty()) {
                    for (IZatGeofenceServiceImpl key : mGeofenceClientCallbackMap.keySet()) {
                        if (key == service) {
                            mGeofenceClientCallbackMap.remove(key);
                        }
                    }
                }
            }
        } catch (RemoteException e) {
            throw new RuntimeException("Failed to remove all geofence added", e);
        }
    }

    /**
     * Implementation of interface IZatFlpService. (FOR INTERNAL USE ONLY)
     *
     * @hide
     */
    private class IZatFlpServiceImpl implements IZatFlpService {

        IFlpService mService;
        public IZatFlpServiceImpl(IFlpService service) {
            mService = service;
        }

        @Override
        public IZatFlpSessionHandle startFlpSession(IFlpLocationCallback callback,
                                                    IzatFlpRequest flpRequest)
                                                    throws IZatIllegalArgumentException {
            if (callback == null || flpRequest == null) {
                throw new IZatIllegalArgumentException("invalid input parameter");
            }
            if ((flpRequest.getTimeInterval() <= 0) && (flpRequest.getDistanceInterval() <= 0)) {
                throw new IZatIllegalArgumentException("both time and distance are invalid");
            }
            try {
                synchronized (sFlpServiceLock) {
                    // first to check if this request is already started.
                    NotificationType notifType =
                        NotificationType.NOTIFICATION_ON_EVERY_LOCATION_FIX;
                    if (flpRequest.mIsRunningInBackground) {
                        notifType = NotificationType.NOTIFICATION_WHEN_BUFFER_IS_FULL;
                    }
                    if (!mFlpRequestsMap.isEmpty()) {
                        FlpRequestMapItem mapItem = null;
                        for (IZatSessionHandlerImpl key : mFlpRequestsMap.keySet()) {
                            mapItem = mFlpRequestsMap.get(key);
                            if ((mapItem.getCallback() == callback) &&
                                (mapItem.getNotifyType() == notifType) &&
                                (mapItem.getTimeInterval() == flpRequest.getTimeInterval()) &&
                                (mapItem.getDistanceInterval() ==
                                 flpRequest.getDistanceInterval())){
                                throw new IZatIllegalArgumentException("this session" +
                                                                       " started already.");
                            }
                        }
                    }
                    int hwId = sFlpRequestsCnt++;
                    long sessionStartTime = System.currentTimeMillis();
                    // register the cb
                    LocationCallbackWrapper cbWrapper = new LocationCallbackWrapper(callback);
                    if ((mFlpFeaturMasks & FEATURE_BIT_DISTANCE_BASED_BATCHING_IS_SUPPORTED)>0) {
                        /* If the modem support LB2.0, then the batched fixes and live fix will be
                          returned independently to clients based on the request issued by
                          each client. */
                        if (flpRequest.mIsRunningInBackground) {
                            mService.registerCallback(FLP_SEESION_BACKGROUND,
                                                      hwId,
                                                      cbWrapper,
                                                      sessionStartTime);
                        } else {
                            mService.registerCallback(FLP_SEESION_FOREROUND,
                                                      hwId,
                                                      cbWrapper,
                                                      sessionStartTime);
                        }
                    } else {
                            /* If the modem does not support LB2.0, then the batching client
                               will also receive the live fix, which is the legacy behaviour.*/
                            mService.registerCallback(FLP_SEESION_PASSIVE,
                                                      hwId,
                                                      cbWrapper,
                                                      sessionStartTime);
                    }
                    // start flp session
                    int result = mService.startFlpSession(hwId,
                                    notifType.getCode(),
                                    flpRequest.getTimeInterval(),
                                    flpRequest.getDistanceInterval());
                    if (VERBOSE)
                        Log.v(TAG, "startFlpSession() returning : " + result);
                    if (result == FLP_RESULT_SUCCESS) {
                        IZatSessionHandlerImpl handle =
                            new IZatSessionHandlerImpl();
                        mFlpRequestsMap.put(handle,
                            new FlpRequestMapItem(callback,
                                                  notifType,
                                                  flpRequest.getTimeInterval(),
                                                  flpRequest.getDistanceInterval(),
                                                  cbWrapper,
                                                  hwId,
                                                  sessionStartTime));
                        return handle;
                    } else {
                        if ((mFlpFeaturMasks &
                            FEATURE_BIT_DISTANCE_BASED_BATCHING_IS_SUPPORTED)>0) {
                            if (flpRequest.mIsRunningInBackground) {
                                mService.unregisterCallback(FLP_SEESION_BACKGROUND,
                                                            cbWrapper);
                            } else {
                                mService.unregisterCallback(FLP_SEESION_FOREROUND,
                                                            cbWrapper);
                            }
                        } else {
                                mService.unregisterCallback(FLP_SEESION_PASSIVE,
                                                            cbWrapper);
                        }
                        sFlpRequestsCnt--;
                        return null;
                    }
                }
            } catch (RemoteException e) {
                throw new RuntimeException("Failed startFlpSession", e);
            }
        }

        @Override
        public void stopFlpSession(IZatFlpSessionHandle handler)
            throws IZatIllegalArgumentException {

            if (handler == null || !(handler instanceof IZatFlpServiceImpl.IZatSessionHandlerImpl)) {
                throw new IZatIllegalArgumentException("invalid input parameter");
            }
            try {
                synchronized (sFlpServiceLock) {
                    // first to check if this request is already started.
                    FlpRequestMapItem mapItem = mFlpRequestsMap.get(handler);
                    if (mapItem == null) {
                        Log.e(TAG, "this request ID is unknown.");
                        return;
                    }
                    if (mService.stopFlpSession(mapItem.getHwId()) != FLP_RESULT_SUCCESS){
                        Log.e(TAG, "stopFlpSession() failed. ");
                        return;
                    }
                    if ((mFlpFeaturMasks & FEATURE_BIT_DISTANCE_BASED_BATCHING_IS_SUPPORTED)>0) {
                        if (mapItem.getNotifyType() ==
                            NotificationType.NOTIFICATION_WHEN_BUFFER_IS_FULL) {
                            mService.unregisterCallback(FLP_SEESION_BACKGROUND,
                                                        mapItem.getCbWrapper());
                        } else if (mapItem.getNotifyType() ==
                            NotificationType.NOTIFICATION_ON_EVERY_LOCATION_FIX) {
                            mService.unregisterCallback(FLP_SEESION_FOREROUND,
                                                        mapItem.getCbWrapper());
                        }
                    } else {
                        mService.unregisterCallback(FLP_SEESION_PASSIVE,
                                                    mapItem.getCbWrapper());
                    }
                    mFlpRequestsMap.remove(handler);
                }
            } catch (RemoteException e) {
                throw new RuntimeException("Failed stopFlpSession", e);
            }
        }

        @Override
        public void registerForPassiveLocations(IZatFlpService.IFlpLocationCallback callback)
            throws IZatIllegalArgumentException {
            if (callback == null) {
                throw new IZatIllegalArgumentException("invalid input parameter");
            }
            try {
                synchronized (sFlpServiceLock) {
                    if (mFlpPassiveCbMap.get(callback) == null) {
                        LocationCallbackWrapper cbWrapper = new LocationCallbackWrapper(callback);
                        mFlpPassiveCbMap.put(callback, cbWrapper);
                        mService.registerCallback(FLP_SEESION_PASSIVE,
                                                  FLP_PASSIVE_LISTENER_HW_ID,
                                                  cbWrapper,
                                                  System.currentTimeMillis());
                    } else {
                        Log.w(TAG, "this passive callback is already registered.");
                    }
                }
            } catch (RemoteException e) {
                throw new RuntimeException("Failed registerForPassiveLocations", e);
            }
        }

        @Override
        public void deregisterForPassiveLocations(IZatFlpService.IFlpLocationCallback callback)
            throws IZatIllegalArgumentException {
            if (callback == null) {
                throw new IZatIllegalArgumentException("invalid input parameter");
            }
            try {
                synchronized (sFlpServiceLock) {
                    LocationCallbackWrapper cbWrapper = mFlpPassiveCbMap.get(callback);
                    if (cbWrapper == null) {
                        Log.w(TAG, "this passive callback is not registered.");
                    } else {
                        mService.unregisterCallback(FLP_SEESION_PASSIVE,
                                                    cbWrapper);
                        mFlpPassiveCbMap.remove(callback);
                    }
                }
            } catch (RemoteException e) {
                throw new RuntimeException("Failed deregisterForPassiveLocations", e);
            }
        }

        private class IZatSessionHandlerImpl implements IZatFlpService.IZatFlpSessionHandle {
            @Override
            public void pullLocations() {

                try {
                    synchronized (sFlpServiceLock) {
                        FlpRequestMapItem mapItem = mFlpRequestsMap.get(this);
                        if (mapItem == null) {
                            Log.w(TAG, "no flp session undergoing");
                            return;
                        }
                        if (mapItem.getCbWrapper() == null) {
                            Log.w(TAG, "no available callback");
                            return;
                        }
                        int result = mService.pullLocations(mapItem.getCbWrapper(),
                                                            mapItem.getSessionStartTime());
                        if (result == FLP_RESULT_SUCCESS) {
                            mapItem.setSessionStartTime(System.currentTimeMillis());
                        }
                        if (VERBOSE)
                            Log.v(TAG, "pullLocations() returning : " + result);
                    }
                } catch (RemoteException e) {
                    throw new RuntimeException("Failed pullLocations", e);
                }
            }

            @Override
            public void setToForeground() {
                try {
                    synchronized (sFlpServiceLock) {
                        FlpRequestMapItem mapItem = mFlpRequestsMap.get(this);
                        if (mapItem == null) {
                            Log.w(TAG, "no flp session undergoing");
                            return;
                        }
                        if (mapItem.getCbWrapper() == null) {
                            Log.w(TAG, "no available callback");
                            return;
                        }
                        int result = FLP_RESULT_SUCCESS;
                        if (mapItem.getNotifyType() !=
                            NotificationType.NOTIFICATION_ON_EVERY_LOCATION_FIX) {
                            result = mService.updateFlpSession(mapItem.getHwId(),
                                NotificationType.NOTIFICATION_ON_EVERY_LOCATION_FIX.getCode(),
                                mapItem.getTimeInterval(),
                                mapItem.getDistanceInterval());
                            if (result == FLP_RESULT_SUCCESS) {
                                // update the item flag in map
                                mapItem.updateNotifyType(
                                    NotificationType.NOTIFICATION_ON_EVERY_LOCATION_FIX);
                                mFlpRequestsMap.put(this, mapItem);
                                // move the callback form bg to fg
                                if ((mFlpFeaturMasks &
                                    FEATURE_BIT_DISTANCE_BASED_BATCHING_IS_SUPPORTED)>0) {
                                    mService.unregisterCallback(FLP_SEESION_BACKGROUND,
                                                                mapItem.getCbWrapper());
                                    mService.registerCallback(FLP_SEESION_FOREROUND,
                                                              mapItem.getHwId(),
                                                              mapItem.getCbWrapper(),
                                                              mapItem.getSessionStartTime());
                                }
                            } else {
                                Log.v(TAG, "mService.updateFlpSession failed.");
                            }
                        }
                        if (VERBOSE)
                            Log.v(TAG, "setToForeground() returning : " + result);
                    }
                } catch (RemoteException e) {
                    throw new RuntimeException("Failed setToForeground", e);
                }
            }

            @Override
            public void setToBackground() {
                try {
                    synchronized (sFlpServiceLock) {
                        FlpRequestMapItem mapItem = mFlpRequestsMap.get(this);
                        if (mapItem == null) {
                            Log.w(TAG, "no flp session undergoing");
                            return;
                        }
                        if (mapItem.getCbWrapper() == null) {
                            Log.w(TAG, "no available callback");
                            return;
                        }
                        int result = FLP_RESULT_SUCCESS;
                        if (mapItem.getNotifyType() !=
                            NotificationType.NOTIFICATION_WHEN_BUFFER_IS_FULL) {
                            result = mService.updateFlpSession(mapItem.getHwId(),
                                NotificationType.NOTIFICATION_WHEN_BUFFER_IS_FULL.getCode(),
                                mapItem.getTimeInterval(),
                                mapItem.getDistanceInterval());
                            if (result == FLP_RESULT_SUCCESS) {
                                // update the item flag in map
                                mapItem.updateNotifyType(
                                    NotificationType.NOTIFICATION_WHEN_BUFFER_IS_FULL);
                                mFlpRequestsMap.put(this, mapItem);
                                if ((mFlpFeaturMasks &
                                    FEATURE_BIT_DISTANCE_BASED_BATCHING_IS_SUPPORTED)>0) {
                                    // move the callback form fg to bg
                                    mService.unregisterCallback(FLP_SEESION_FOREROUND,
                                                                mapItem.getCbWrapper());
                                    mService.registerCallback(FLP_SEESION_BACKGROUND,
                                                              mapItem.getHwId(),
                                                              mapItem.getCbWrapper(),
                                                              mapItem.getSessionStartTime());
                                }
                            } else {
                                Log.v(TAG, "mService.updateFlpSession failed.");
                            }
                        }
                        if (VERBOSE)
                            Log.v(TAG, "setToBackground() returning : " + result);
                    }
                } catch (RemoteException e) {
                    throw new RuntimeException("Failed setToBackground", e);
                }
            }
        }
    }

    /**
     * Implementation of interface IZatTestService. (FOR INTERNAL USE ONLY)
     *
     * @hide
     */
    private class IZatTestServiceImpl implements IZatTestService {

        ITestService mService;
        public IZatTestServiceImpl(ITestService service) {
            mService = service;
        }

        @Override
        public void deleteAidingData(long flags)
            throws IZatIllegalArgumentException {
            if (flags == 0) {
                throw new IZatIllegalArgumentException("invalid input parameter." +
                                                       " flags must be filled");
            }
            try {
                mService.deleteAidingData(flags);
            } catch (RemoteException e) {
                throw new RuntimeException("Failed deregisterForPassiveLocations", e);
            }
        }

        @Override
        public void registerForMaxPowerAllocatedChange(
            IZatTestService.IFlpMaxPowerAllocatedCallback callback)
                throws IZatIllegalArgumentException {

            if (callback == null) {
                throw new IZatIllegalArgumentException("invalid input parameter");
            }
            try {
                synchronized (sTestServiceLock) {
                    if (mFlpMaxPowerCbMap.get(callback) == null) {
                        MaxPowerAllocatedCallbackWrapper cbWrapper =
                                    new MaxPowerAllocatedCallbackWrapper(callback);
                        mFlpMaxPowerCbMap.put(callback, cbWrapper);
                        mService.registerMaxPowerChangeCallback(cbWrapper);
                    } else {
                        Log.w(TAG, "this max power callback is already registered.");
                    }
                }
            } catch (RemoteException e) {
                throw new RuntimeException("Failed registerForMaxPowerAllocatedChange", e);
            }
        }

        @Override
        public void deregisterForMaxPowerAllocatedChange(
            IZatTestService.IFlpMaxPowerAllocatedCallback callback)
                throws IZatIllegalArgumentException {

            if (callback == null) {
                throw new IZatIllegalArgumentException("invalid input parameter");
            }
            try {
                synchronized (sTestServiceLock) {
                    MaxPowerAllocatedCallbackWrapper cbWrapper = mFlpMaxPowerCbMap.get(callback);
                    if (cbWrapper == null) {
                        Log.w(TAG, "this passive callback is not registered.");
                    } else {
                        mService.unregisterMaxPowerChangeCallback(cbWrapper);
                        mFlpMaxPowerCbMap.remove(callback);
                    }
                }
            } catch (RemoteException e) {
                throw new RuntimeException("Failed deregisterForMaxPowerAllocatedChange", e);
            }
        }
    }

    private class IZatGeofenceServiceImpl implements IZatGeofenceService {

        IGeofenceService mService;
        public IZatGeofenceServiceImpl(IGeofenceService service) {
            mService = service;
        }

        @Override
        public void registerForGeofenceCallbacks(IZatGeofenceCallback statusCb)
                                                 throws IZatIllegalArgumentException {

            if (statusCb == null) {
                throw new IZatIllegalArgumentException("invalid input parameter");
            }
            /* save the callbacks bonding with the caller. If there is already a
               callback associated with the caller, then the new cb will override
               the existing cb. */
            mGeofenceClientCallbackMap.put(this, statusCb);
        }

        @Override
        public void deregisterForGeofenceCallbacks(IZatGeofenceCallback statusCb)
                                                   throws IZatIllegalArgumentException {
            if (statusCb == null) {
                throw new IZatIllegalArgumentException("invalid input parameter");
            }
            IZatGeofenceCallback cb = mGeofenceClientCallbackMap.get(this);
            if (cb != null) {
                mGeofenceClientCallbackMap.remove(this);
            }
        }

        @Override
        public IZatGeofenceHandle addGeofence(Object context, IzatGeofence geofence)
                                              throws IZatIllegalArgumentException {
            if (geofence == null) {
                throw new IZatIllegalArgumentException("invalid input parameter");
            }
            IZatGeofenceCallback cb = mGeofenceClientCallbackMap.get(this);
            if (cb == null) {
                Log.e(TAG, "callback is not registered.");
                return null;
            }
            try {
                synchronized (sGeofenceServiceLock) {

                    int dwellTime = 0;
                    int dwellType = 0;
                    if (geofence.getDwellNotify() != null) {
                        dwellTime = geofence.getDwellNotify().getDwellTime();
                        dwellType = geofence.getDwellNotify().getDwellType();
                    }

                    int geofenceId = mService.addGeofence(geofence.getLatitude(),
                                                          geofence.getLongitude(),
                                                          geofence.getRadius(),
                                                          geofence.getTransitionTypes().getValue(),
                                                          geofence.getNotifyResponsiveness(),
                                                          geofence.getConfidence().getValue(),
                                                          dwellTime,
                                                          dwellType);

                    IZatGeofenceHandleImpl handle = new IZatGeofenceHandleImpl();
                    mGeofencesMap.put(handle, new GeofenceMapItem(context, geofenceId, cb));
                    return handle;
                }
            } catch (RemoteException e) {
                throw new RuntimeException("Failed addGeofence", e);
            }
        }

        @Override
        public void removeGeofence(IZatGeofenceHandle handler)
                                   throws IZatIllegalArgumentException {
            if (handler == null ||
                !(handler instanceof IZatGeofenceServiceImpl.IZatGeofenceHandleImpl)) {
                throw new IZatIllegalArgumentException("invalid input parameter");
            }
            try {
                synchronized (sGeofenceServiceLock) {
                    GeofenceMapItem mapItem = mGeofencesMap.get(handler);
                    if (mapItem == null) {
                        Log.e(TAG, "this request ID is unknown.");
                        mapItem.getCallback().onRequestFailed(handler,
                                                              GEOFENCE_REQUEST_TYPE_REMOVE,
                                                              GEOFENCE_RESULT_ERROR_ID_UNKNOWN);
                        return;
                    }
                    mService.removeGeofence(mapItem.getHWGeofenceId());
                }
            } catch (RemoteException e) {
                throw new RuntimeException("Failed removeGeofence", e);
            }
        }

        private class IZatGeofenceHandleImpl implements IZatGeofenceService.IZatGeofenceHandle {

            @Override
            public Object getContext() {
                GeofenceMapItem mapItem = mGeofencesMap.get(this);
                if (mapItem != null) {
                    return mapItem.getContext();
                }
                return null;
            }

            @Override
            public void update(IzatGeofenceTransitionTypes transitionTypes,
                               int notifyResponsiveness)
                               throws IZatIllegalArgumentException {
                if (transitionTypes == null || notifyResponsiveness <= 0) {
                    throw new IZatIllegalArgumentException("invalid input parameter");
                }
                try {
                    synchronized (sGeofenceServiceLock) {
                        GeofenceMapItem mapItem = mGeofencesMap.get(this);
                        if (mapItem == null) {
                            Log.e(TAG, "this request ID is unknown.");
                            mapItem.getCallback().onRequestFailed(this,
                                                                  GEOFENCE_REQUEST_TYPE_UPDATE,
                                                                  GEOFENCE_RESULT_ERROR_ID_UNKNOWN);
                            return;
                        }
                        mService.updateGeofence(mapItem.getHWGeofenceId(),
                                                transitionTypes.getValue(),
                                                notifyResponsiveness);
                    }
                } catch (RemoteException e) {
                    throw new RuntimeException("Failed removeGeofence", e);
                }
            }

            @Override
            public void pause() {
                try {
                    synchronized (sGeofenceServiceLock) {
                        GeofenceMapItem mapItem = mGeofencesMap.get(this);
                        if (mapItem == null) {
                            Log.e(TAG, "this request ID is unknown.");
                            mapItem.getCallback().onRequestFailed(this,
                                                                  GEOFENCE_REQUEST_TYPE_REMOVE,
                                                                  GEOFENCE_RESULT_ERROR_ID_UNKNOWN);
                            return;
                        }
                        mService.pauseGeofence(mapItem.getHWGeofenceId());
                    }
                } catch (RemoteException e) {
                    throw new RuntimeException("Failed removeGeofence", e);
                }
            }

            @Override
            public void resume(IzatGeofenceTransitionTypes transitionTypes)
                               throws IZatIllegalArgumentException {
                if (transitionTypes == null) {
                    throw new IZatIllegalArgumentException("invalid input parameter");
                }
                try {
                    synchronized (sGeofenceServiceLock) {
                        GeofenceMapItem mapItem = mGeofencesMap.get(this);
                        if (mapItem == null) {
                            Log.e(TAG, "this request ID is unknown.");
                            mapItem.getCallback().onRequestFailed(this,
                                                                  GEOFENCE_REQUEST_TYPE_RESUME,
                                                                  GEOFENCE_RESULT_ERROR_ID_UNKNOWN);
                            return;
                        }
                        mService.resumeGeofence(mapItem.getHWGeofenceId(),
                                                transitionTypes.getValue());
                    }
                } catch (RemoteException e) {
                    throw new RuntimeException("Failed removeGeofence", e);
                }
            }
        }
    }

    /**
     * Implementation of callback object. (FOR INTERNAL USE ONLY)
     *
     * @hide
     */
    private class LocationCallbackWrapper extends ILocationCallback.Stub {
        IZatFlpService.IFlpLocationCallback mCallback;
        public LocationCallbackWrapper(IZatFlpService.IFlpLocationCallback callback) {
            mCallback = callback;
        }
        public void onLocationAvailable(Location[] locations) {
            if (mCallback == null) {
                Log.w(TAG, "mCallback is NULL in LocationCallbackWrapper");
                return;
            }
            mCallback.onLocationAvailable(locations);
        }
    }

    private enum NotificationType {
        NOTIFICATION_WHEN_BUFFER_IS_FULL(1),
        NOTIFICATION_ON_EVERY_LOCATION_FIX(2);
        private final int mCode;
        private NotificationType(int c) {
            mCode = c;
        }

        public int getCode() {
            return mCode;
        }
    }

    private class FlpRequestMapItem {

        public IZatFlpService.IFlpLocationCallback mCallback = null;
        public NotificationType mNotiType = null;
        public LocationCallbackWrapper mCbWrapper = null;
        public long mMaxTime = -1;
        public int mMaxDistance = -1;
        public int mHwId = -1;
        private long mSessionStartTime = -1;
        public FlpRequestMapItem(IZatFlpService.IFlpLocationCallback callback,
                                 NotificationType notiType,
                                 long maxTime,
                                 int maxDistance,
                                 LocationCallbackWrapper cbWrapper,
                                 int hwId,
                                 long sessionStartTime) {
            mCallback = callback;
            mNotiType = notiType;
            mMaxTime = maxTime;
            mMaxDistance = maxDistance;
            mCbWrapper = cbWrapper;
            mHwId = hwId;
            mSessionStartTime = sessionStartTime;
        }
        public IZatFlpService.IFlpLocationCallback getCallback() {
            return mCallback;
        }
        public void updateNotifyType(NotificationType type) {
            mNotiType = type;
        }
        public NotificationType getNotifyType() {
            return mNotiType;
        }
        public long getTimeInterval() {
            return mMaxTime;
        }
        public int getDistanceInterval() {
            return mMaxDistance;
        }
        public LocationCallbackWrapper getCbWrapper() {
            return mCbWrapper;
        }
        public int getHwId() {
            return mHwId;
        }
        public long getSessionStartTime() {
            return mSessionStartTime;
        }
        public void setSessionStartTime(long sessionStartTime) {
            mSessionStartTime = sessionStartTime;
        }
    }

    private class GeofenceMapItem {
        Object mContext;
        int mHWGeofenceId;
        IZatGeofenceService.IZatGeofenceCallback mCallback;
        public GeofenceMapItem(Object context,
                               int geofenceId,
                               IZatGeofenceService.IZatGeofenceCallback callback) {
            mContext = context;
            mHWGeofenceId = geofenceId;
            mCallback = callback;
        }
        public Object getContext() {
            return mContext;
        }
        public int getHWGeofenceId() {
            return mHWGeofenceId;
        }
        public IZatGeofenceService.IZatGeofenceCallback getCallback() {
            return mCallback;
        }
    }

    private class MaxPowerAllocatedCallbackWrapper
        extends IMaxPowerAllocatedCallback.Stub {

        IZatTestService.IFlpMaxPowerAllocatedCallback mCallback;
        public MaxPowerAllocatedCallbackWrapper(
            IZatTestService.IFlpMaxPowerAllocatedCallback callback) {
            mCallback = callback;
        }
        public void onMaxPowerAllocatedChanged(double power_mW) {
            if (mCallback == null) {
                Log.w(TAG, "mCallback is NULL in MaxPowerAllocatedCallbackWrapper");
                return;
            }
            mCallback.onMaxPowerAllocatedChanged(power_mW);
        }
    }

    private class GeofenceStatusCallbackWrapper
        extends IGeofenceCallback.Stub {

        public void onTransitionEvent(int geofenceHwId,
                                      int event,
                                      Location location) {
            if (VERBOSE)
                Log.d(TAG, "onTransitionEvent - geofenceHwId is " +
                      geofenceHwId + "; event is " + event);
            // find the callback through geofence hw id
            if (!mGeofencesMap.isEmpty()) {
                GeofenceMapItem mapItem = null;
                for (IZatGeofenceServiceImpl.IZatGeofenceHandleImpl key : mGeofencesMap.keySet()) {
                    mapItem = mGeofencesMap.get(key);
                    if (mapItem.getHWGeofenceId() == geofenceHwId) {
                        mapItem.getCallback().onTransitionEvent(key, event, location);
                        return;
                    }
                }
            }
        }

        public void onRequestResultReturned(int geofenceHwId,
                                            int requestType,
                                            int result) {
            if (VERBOSE)
                Log.d(TAG, "onRequestResultReturned - geofenceHwId is " +
                      geofenceHwId + "; requestType is " + requestType +
                      "; result is " + result);

            if ((result == IZatGeofenceService.GEOFENCE_RESULT_SUCCESS) &&
                (requestType != IZatGeofenceService.GEOFENCE_REQUEST_TYPE_REMOVE)) {
                return;
            }
            // find the callback through geofence hw id
            if (!mGeofencesMap.isEmpty()) {
                GeofenceMapItem mapItem = null;
                for (IZatGeofenceServiceImpl.IZatGeofenceHandleImpl key : mGeofencesMap.keySet()) {
                    mapItem = mGeofencesMap.get(key);
                    if (mapItem.getHWGeofenceId() == geofenceHwId) {
                        if (result == IZatGeofenceService.GEOFENCE_RESULT_SUCCESS) {
                            if (requestType == IZatGeofenceService.GEOFENCE_REQUEST_TYPE_REMOVE) {
                                mGeofencesMap.remove(key);
                            }
                        } else {
                            if (requestType == IZatGeofenceService.GEOFENCE_REQUEST_TYPE_ADD) {
                                mGeofencesMap.remove(key);
                            }
                            mapItem.getCallback().onRequestFailed(key, requestType, result);
                        }
                        return;
                    }
                }
            }
        }

        public void onEngineReportStatus(int status,
                                         Location location) {
            if (VERBOSE)
                Log.d(TAG, "onEngineReportStatus - status is " + status);
            // broadcast to all clients
            if (!mGeofenceClientCallbackMap.isEmpty()) {
                for (IZatGeofenceServiceImpl key : mGeofenceClientCallbackMap.keySet()) {
                    mGeofenceClientCallbackMap.get(key).onEngineReportStatus(status,
                                                                             location);
                }
            }
        }
    }
}
