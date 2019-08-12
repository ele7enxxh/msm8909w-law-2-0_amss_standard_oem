/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

package com.qualcomm.location.izat.flp;

import android.app.Service;
import android.content.Intent;
import android.location.Location;
import android.os.IBinder;
import android.os.RemoteCallbackList;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.util.Log;
import android.util.Pair;
import java.util.LinkedList;
import java.util.Queue;

public class FlpServiceProvider {
    private static final String TAG = "FlpServiceProvider";
    private static final boolean VERBOSE_DBG = Log.isLoggable(TAG, Log.VERBOSE);
    private static final int FLP_RESULT_SUCCESS = 0;
    private static final int FLP_RESULT_ERROR = -1;
    private static final int LOCATION_REPORT_ON_FULL_INDICATION = 1;
    private static final int LOCATION_REPORT_ON_FIX_INDICATION = 2;
    private static final int LOCATION_REPORT_ON_QUERY = 4;
    private static final int LOCATION_REPORT_ON_INDICATIONS = 8; // for legacy
    private static final int FLP_PASSIVE_LISTENER_HW_ID = -1;
    private static final int FEATURE_BIT_DISTANCE_BASED_BATCHING_IS_SUPPORTED = 8;
    private static final Object sCallBacksLock = new Object();

    private static final int FLP_SEESION_BACKGROUND = 1;
    private static final int FLP_SEESION_FOREROUND = 2;
    private static final int FLP_SEESION_PASSIVE = 4;

    private Queue<Pair<ILocationCallback,Long>> mQueryCbQueue
        = new LinkedList<Pair<ILocationCallback,Long>>();
    private RemoteCallbackList<ILocationCallback> mCallbacksForBg
        = new RemoteCallbackList<ILocationCallback>();
    private RemoteCallbackList<ILocationCallback> mCallbacksForFg
        = new RemoteCallbackList<ILocationCallback>();
    private RemoteCallbackList<ILocationCallback> mCallbacksForPassive
        = new RemoteCallbackList<ILocationCallback>();
    private RemoteCallbackList<IMaxPowerAllocatedCallback> mMaxPowerCallbacks
        = new RemoteCallbackList<IMaxPowerAllocatedCallback>();

    public static FlpServiceProvider sInstance = null;
    public static FlpServiceProvider getInstance() {
        if (sInstance == null) {
            sInstance = new FlpServiceProvider();
        }
        return sInstance;
    }
    private int mFlpFeaturMasks = -1;

    public FlpServiceProvider() {
        if (VERBOSE_DBG)
            Log.d(TAG, "FlpServiceProvider construction");
        if (native_flp_init() != FLP_RESULT_SUCCESS) {
            Log.e(TAG, "native flp init failed");
        }
    }

    /* Remote binder */
    private final IFlpService.Stub mBinder = new IFlpService.Stub() {
        public void registerCallback(final int sessionType,
                                     final int id,
                                     final ILocationCallback cb,
                                     final long sessionStartTime) {
            if (VERBOSE_DBG)
                Log.d(TAG, "in IFlpService.Stub(): registerCallback()," +
                           " sessionType is " + sessionType + "; id is " + id
                           +"; sessionStartTime is " + sessionStartTime);
            if (cb != null) {
                if (mCallbacksForBg != null &&
                    mCallbacksForFg != null &&
                    mCallbacksForPassive != null) {
                    switch (sessionType) {
                        case FLP_SEESION_BACKGROUND:
                            synchronized (sCallBacksLock) {
                                mCallbacksForBg.register(cb, sessionStartTime);
                            }
                            try {
                                cb.asBinder().linkToDeath(new IBinder.DeathRecipient() {
                                    @Override
                                    public void binderDied() {
                                        mCallbacksForBg.unregister(cb);
                                        stopFlpSession(id);
                                    }
                                }, 0);
                            } catch (RemoteException e) {
                                throw new RuntimeException("Failed clean up flp sessions", e);
                            }
                            break;
                        case FLP_SEESION_FOREROUND:
                            synchronized (sCallBacksLock) {
                                mCallbacksForFg.register(cb, sessionStartTime);
                            }
                            try {
                                cb.asBinder().linkToDeath(new IBinder.DeathRecipient() {
                                    @Override
                                    public void binderDied() {
                                        mCallbacksForFg.unregister(cb);
                                        stopFlpSession(id);
                                    }
                                }, 0);
                            } catch (RemoteException e) {
                                throw new RuntimeException("Failed clean up flp sessions", e);
                            }
                            break;
                        case FLP_SEESION_PASSIVE:
                            synchronized (sCallBacksLock) {
                                mCallbacksForPassive.register(cb, sessionStartTime);
                            }
                            try {
                                cb.asBinder().linkToDeath(new IBinder.DeathRecipient() {
                                    @Override
                                    public void binderDied() {
                                        mCallbacksForPassive.unregister(cb);
                                        if (id != FLP_PASSIVE_LISTENER_HW_ID) {
                                            stopFlpSession(id);
                                        }
                                    }
                                }, 0);
                            } catch (RemoteException e) {
                                throw new RuntimeException("Failed unregister passive cb", e);
                            }
                            break;
                        default:
                            Log.e(TAG, "unknown sessionType");
                            break;
                    }
                } else {
                    Log.e(TAG, "one of the callback list is not created");
                }
            } else {
                Log.e(TAG, "cb is null");
            }
        }

        public void unregisterCallback(final int sessionType,
                                       final ILocationCallback cb) {
            if (VERBOSE_DBG)
                Log.d(TAG, "in IFlpService.Stub(): unregisterCallback()");
            if (cb != null) {
                if (mCallbacksForBg != null &&
                    mCallbacksForFg != null &&
                    mCallbacksForPassive != null) {
                    synchronized (sCallBacksLock) {
                        switch (sessionType) {
                            case FLP_SEESION_BACKGROUND:
                                mCallbacksForBg.unregister(cb);
                                break;
                            case FLP_SEESION_FOREROUND:
                                mCallbacksForFg.unregister(cb);
                                break;
                            case FLP_SEESION_PASSIVE:
                                mCallbacksForPassive.unregister(cb);
                                break;
                            default:
                                Log.e(TAG, "unknown sessionType");
                                break;
                        }
                    }
                } else {
                    Log.e(TAG, "one of the callback list is not created");
                }
            } else {
                Log.e(TAG, "cb is null");
            }
        }

        public int getAllSupportedFeatures() {
            if (VERBOSE_DBG)
                Log.d(TAG, "in IFlpService.Stub(): getAllSupportedFeatures()");
            if (mFlpFeaturMasks == -1) {
                mFlpFeaturMasks = native_flp_get_all_supported_features();
            }
            return mFlpFeaturMasks;
        }

        public int startFlpSession(int id,
                                   int flags,
                                   long period_ms,
                                   int distance_ms) {
            if (VERBOSE_DBG) {
                Log.d(TAG, "in IFlpService.Stub(): startFlpSession()" +
                           "; id is " + id +
                           "; period_ms is " + period_ms +
                           "; flags is " + flags +
                           "; distance_ms is " + distance_ms);
            }
            long period_ns =
                (period_ms > (Long.MAX_VALUE/1000000)) ? Long.MAX_VALUE : period_ms*1000000;
            return native_flp_start_session(id,
                                            flags,
                                            period_ns,
                                            distance_ms);
        }

        public int updateFlpSession(int id,
                                    int flags,
                                    long period_ms,
                                    int distance_ms) {
            if (VERBOSE_DBG) {
                Log.d(TAG, "in IFlpService.Stub(): updateFlpSession()" +
                           "; id is " + id +
                           "; period_ms is " + period_ms +
                           "; flags is " + flags +
                           "; distance_ms is " + distance_ms);
            }
            long period_ns =
                (period_ms > (Long.MAX_VALUE/1000000)) ? Long.MAX_VALUE : period_ms*1000000;
            return native_flp_update_session(id,
                                             flags,
                                             period_ns,
                                             distance_ms);
        }

        public int stopFlpSession(int id) {
            if (VERBOSE_DBG)
                Log.d(TAG, "in IFlpService.Stub(): stopFlpSession(); id is " + id);
            return native_flp_stop_session(id);
        }

        public int pullLocations(final ILocationCallback cb,
                                 final long sessionStartTime) {
            if (VERBOSE_DBG)
                Log.d(TAG, "in IFlpService.Stub(): pullLocations(), sessionStartTime is "
                           + sessionStartTime);
            if (cb == null) {
                Log.e(TAG, "in IFlpService.Stub(): cb is null.");
                return FLP_RESULT_ERROR;
            }
            synchronized (sCallBacksLock) {
                // save the cb
                mQueryCbQueue.add(new Pair<ILocationCallback,Long>(cb, sessionStartTime));
            }
            return native_flp_get_all_locations();
        }
    };

    /* Remote binder */
    private final ITestService.Stub mTestingBinder = new ITestService.Stub() {
        public void deleteAidingData(long flags) {
            if (VERBOSE_DBG)
                Log.d(TAG, "in IFlpService.Stub(): deleteAidingData(). Flags: " + flags);
            native_flp_delete_aiding_data(flags);
        }

        public void registerMaxPowerChangeCallback(final IMaxPowerAllocatedCallback cb) {
            if (VERBOSE_DBG)
                Log.d(TAG, "in IFlpService.Stub(): registerMaxPowerChangeCallback()");
            if (cb != null) {
                if (mMaxPowerCallbacks != null) {
                    mMaxPowerCallbacks.register(cb);
                    try {
                        cb.asBinder().linkToDeath(new IBinder.DeathRecipient() {
                            @Override
                            public void binderDied() {
                                mMaxPowerCallbacks.unregister(cb);
                            }
                        }, 0);
                        cb.onMaxPowerAllocatedChanged(native_flp_get_max_power_allocated_in_mw());
                    } catch (RemoteException e) {
                        throw new RuntimeException("Failed clean up", e);
                    }
                } else {
                    Log.e(TAG, "mMaxPowerCallbacks is null");
                }
            } else {
                Log.e(TAG, "cb is null");
            }
        }

        public void unregisterMaxPowerChangeCallback(IMaxPowerAllocatedCallback cb) {
            if (VERBOSE_DBG)
                Log.d(TAG, "in IFlpService.Stub(): unregisterMaxPowerChangeCallback()");
            if (cb != null) {
                if (mMaxPowerCallbacks != null) {
                    mMaxPowerCallbacks.unregister(cb);
                } else {
                    Log.e(TAG, "mMaxPowerCallbacks is null");
                }
            } else {
                Log.e(TAG, "cb is null");
            }
        }
    };

    private void onLocationReport(Location[] locations, int reportTrigger) {
        if (VERBOSE_DBG)
            Log.d(TAG, "entering onLocationReport() reportTrigger is " + reportTrigger +
                       "; and the first timestamp is " + locations[0].getTime());

        if (reportTrigger == LOCATION_REPORT_ON_FULL_INDICATION) {
            // Broadcast to all batching callbacks the new value.
            synchronized (sCallBacksLock) {
                int index = mCallbacksForBg.beginBroadcast();
                for (int i = 0; i < index; i++) {
                    try {
                        long sessionStartTime = (long) mCallbacksForBg.getBroadcastCookie(i);
                        if (VERBOSE_DBG)
                            Log.d(TAG, "in the mCallbacksForBg loop : " + i
                                       + "; cd timestamp is" + sessionStartTime);
                        if (sessionStartTime<=locations[0].getTime()) {
                            // return the whole batch
                            if (VERBOSE_DBG)
                                Log.d(TAG, "return whole batch");
                            mCallbacksForBg.getBroadcastItem(i).onLocationAvailable(locations);
                        } else if (sessionStartTime>locations[locations.length-1].getTime()) {
                            if (VERBOSE_DBG)
                                Log.d(TAG, "no need to return");
                        } else {
                            // find the offset
                            int offset = getOffset(sessionStartTime, locations);
                            Location[] newResult = new Location[locations.length - offset];
                            System.arraycopy(locations,
                                             offset,
                                             newResult,
                                             0,
                                             locations.length - offset);
                            mCallbacksForBg.getBroadcastItem(i).onLocationAvailable(newResult);
                        }
                    } catch (RemoteException e) {
                        // The RemoteCallbackList will take care of removing
                        // the dead object.
                    }
                }
                mCallbacksForBg.finishBroadcast();
            }
        } else if (reportTrigger == LOCATION_REPORT_ON_FIX_INDICATION) {
            // Broadcast to all tracking callbacks the new value.
            synchronized (sCallBacksLock) {
                int index = mCallbacksForFg.beginBroadcast();
                for (int i = 0; i < index; i++) {
                    try {
                        long sessionStartTime = (long) mCallbacksForFg.getBroadcastCookie(i);
                        if (VERBOSE_DBG)
                            Log.d(TAG, "in the mCallbacksForFg loop : " + i
                                       + "; cd timestamp is" + sessionStartTime);
                        if (sessionStartTime<=locations[0].getTime()) {
                            // return the whole batch
                            if (VERBOSE_DBG)
                                Log.d(TAG, "return whole batch");
                            mCallbacksForFg.getBroadcastItem(i).onLocationAvailable(locations);
                        } else if (sessionStartTime>locations[locations.length-1].getTime()) {
                            if (VERBOSE_DBG)
                                Log.d(TAG, "no need to return");
                        } else {
                            // find the offset
                            int offset = getOffset(sessionStartTime, locations);
                            Location[] newResult = new Location[locations.length - offset];
                            System.arraycopy(locations,
                                             offset,
                                             newResult,
                                             0,
                                             locations.length - offset);
                            mCallbacksForFg.getBroadcastItem(i).onLocationAvailable(newResult);
                        }
                    } catch (RemoteException e) {
                        // The RemoteCallbackList will take care of removing
                        // the dead object.
                    }
                }
                mCallbacksForFg.finishBroadcast();
            }
        } else if (reportTrigger == LOCATION_REPORT_ON_QUERY) {
            synchronized (sCallBacksLock) {
                if (!mQueryCbQueue.isEmpty()) {
                    Pair<ILocationCallback,Long> cbPair = mQueryCbQueue.remove();
                    if (cbPair != null) {
                        try {
                            // check the timestamp, find the offset
                            ILocationCallback callback = cbPair.first;
                            long sessionStartTime = cbPair.second;
                            if (VERBOSE_DBG)
                                Log.d(TAG, "calling callback for" +
                                           " pulling, sessionStartTime is " +
                                            sessionStartTime);
                            if (sessionStartTime<=locations[0].getTime()) {
                                // return the whole batch
                                if (VERBOSE_DBG)
                                    Log.d(TAG, "return whole batch");
                                callback.onLocationAvailable(locations);
                            } else if (sessionStartTime>locations[locations.length-1].getTime()) {
                                if (VERBOSE_DBG)
                                    Log.d(TAG, "no need to return");
                            } else {
                                int offset = getOffset(sessionStartTime, locations);
                                Location[] newResult = new Location[locations.length - offset];
                                System.arraycopy(locations,
                                                 offset,
                                                 newResult,
                                                 0,
                                                 locations.length - offset);
                                callback.onLocationAvailable(newResult);
                            }
                            // update the timestamp of the callback
                            if (mCallbacksForBg.unregister(callback)) {
                                mCallbacksForBg.register(callback, System.currentTimeMillis());
                            }
                            if (mCallbacksForFg.unregister(callback)) {
                                mCallbacksForFg.register(callback, System.currentTimeMillis());
                            }
                            if (mCallbacksForPassive.unregister(callback)) {
                                mCallbacksForPassive.register(callback, System.currentTimeMillis());
                            }
                        } catch (RemoteException e) {
                            // The RemoteCallbackList will take care of removing
                            // the dead object.
                        }
                    }
                } else {
                    Log.e(TAG, "no available callback on query");
                }
            }
        } else if (reportTrigger == LOCATION_REPORT_ON_INDICATIONS) {
            /*
               For legacy behaviour, the callback are in the passive
               listeners already, so do nothing here.
            */
        } else {
            Log.e(TAG, "unknown reportTrigger");
        }

        // passive listeners
        if (mCallbacksForPassive.getRegisteredCallbackCount() > 0) {
            // Broadcast to all passive listeners
            synchronized (sCallBacksLock) {
                int index = mCallbacksForPassive.beginBroadcast();
                for (int i = 0; i < index; i++) {
                    try {
                        long sessionStartTime = (long) mCallbacksForPassive.getBroadcastCookie(i);
                        if (VERBOSE_DBG)
                            Log.d(TAG, "in the mCallbacksForPassive loop : " + i
                                       + "; cd timestamp is" + sessionStartTime);
                        if (sessionStartTime<=locations[0].getTime()) {
                            // return the whole batch
                            if (VERBOSE_DBG)
                                Log.d(TAG, "return whole batch");
                            mCallbacksForPassive.getBroadcastItem(i).onLocationAvailable(locations);
                        } else if (sessionStartTime>locations[locations.length-1].getTime()) {
                            if (VERBOSE_DBG)
                                Log.d(TAG, "no need to return");
                        } else {
                            // find the offset
                            int offset = getOffset(sessionStartTime, locations);
                            Location[] newResult = new Location[locations.length - offset];
                            System.arraycopy(locations,
                                             offset,
                                             newResult,
                                             0,
                                             locations.length - offset);
                            mCallbacksForPassive.getBroadcastItem(i).onLocationAvailable(newResult);
                        }
                    } catch (RemoteException e) {
                        // The RemoteCallbackList will take care of removing
                        // the dead object.
                    }
                }
                mCallbacksForPassive.finishBroadcast();
            }
        }
    }

    private void onMaxPowerAllocatedChanged(double power_mW) {
        if (VERBOSE_DBG)
            Log.d(TAG, "entering onMaxPowerAllocatedChanged() power: " + power_mW);
        // Broadcast to all clients the new value.
        int index = mMaxPowerCallbacks.beginBroadcast();
        for (int i = 0; i < index; i++) {
            if (VERBOSE_DBG)
                Log.d(TAG, "in the mMaxPowerCallbacks loop : " + i);
            try {
                IMaxPowerAllocatedCallback cb =
                    mMaxPowerCallbacks.getBroadcastItem(i);
                cb.onMaxPowerAllocatedChanged(power_mW);
            } catch (RemoteException e) {
                // The RemoteCallbackList will take care of removing
                // the dead object.
            }
        }
        mMaxPowerCallbacks.finishBroadcast();
    }

    private int getOffset (long sessionStartTime, Location[] locations) {
        int offset = -1, left = 0, right = locations.length-1;
        while(left <= right) {
            int mid = (left+right)/2;
            // found the exactly match
            if (sessionStartTime ==
                locations[mid].getTime()) {
                offset = mid;
                break;
            }
            if (sessionStartTime >
                locations[mid].getTime()) {
                left = mid + 1;
            }
            if (sessionStartTime <
                locations[mid].getTime()) {
                right = mid - 1;
            }
        }
        if (offset == -1) {
            offset = left;
        }

        if (VERBOSE_DBG)
           Log.d(TAG, "offset : " + offset);
        return offset;
    }

    public IFlpService getFlpBinder() {
        return mBinder;
    }

    public ITestService getFlpTestingBinder() {
        return mTestingBinder;
    }

    static {
        native_flp_class_init();
    }

    private static native void native_flp_class_init();
    private native int native_flp_init();
    private native int native_flp_get_all_supported_features();
    private native int native_flp_start_session(int id,
                                                int flags,
                                                long period_ns,
                                                int distance_ms);
    private native int native_flp_update_session(int id,
                                                 int flags,
                                                 long period_ns,
                                                 int distance_ms);
    private native int native_flp_stop_session(int id);
    private native int native_flp_get_all_locations();
    private native void native_flp_delete_aiding_data(long flags);
    private native double native_flp_get_max_power_allocated_in_mw();
}
