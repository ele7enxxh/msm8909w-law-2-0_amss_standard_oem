/* ======================================================================
*  Copyright (c) 2015 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.location.sdk;

import android.content.Context;
import android.location.Location;

/**
 * <p>Copyright (c) 2015 Qualcomm Technologies, Inc.</p>
 * <p>All Rights Reserved.</p>
 * <p>Confidential and Proprietary - Qualcomm Technologies, Inc</p>
 * <br/>
 * <p><b>IZatGeofenceService</b> interface - the api for interacting with the IZat Geofence service. </p>
 * <pre>
 * <code>
 *    // Sample Code
 *
 *    import android.app.Activity;
 *    import android.os.Bundle;
 *    import android.util.Log;
 *    import android.view.View;
 *    import android.widget.Button;
 *    import android.widget.TextView;
 *    import com.qti.location.sdk.IZatManager;
 *    import com.qti.location.sdk.IZatGeofenceService;
 *
 *    public class FullscreenActivity extends Activity {
 *        ......
 *        private IZatGeofenceService.IZatGeofenceHandle mHandle = null;
 *        final GeofenceCallback geofenceCb = new GeofenceCallback();
 *        final Button geofenceBtn = (Button)findViewById(R.id.geofenceBtn);
 *
 *        @Override
 *        protected void onCreate(Bundle savedInstanceState) {
 *            ......
 *            geofenceBtn.setOnClickListener(new View.OnClickListener() {
 *                @Override
 *                public void onClick(View v) {
 *                        // register callback
 *                        gfSevice.registerForGeofenceCallbacks(geofenceCb);
 *
 *                        // set geofence circle
 *                        IZatGeofenceService.IzatGeofence geofence =
 *                            new IZatGeofenceService.IzatGeofence(37.375507,
 *                                                                 -121.983643,
 *                                                                 5000);
 *
 *                        // set transition type
 *                        geofence.setTransitionTypes(
 *                            IZatGeofenceService.IzatGeofenceTransitionTypes.ENTERED_AND_EXITED);
 *
 *                        // set responsiveness
 *                        geofence.setNotifyResponsiveness(10000);
 *
 *                        // set confidence
 *                        geofence.setConfidence(IZatGeofenceService.IzatGeofenceConfidence.MEDIUM);
 *
 *                        // create dwell time object
 *                        IZatGeofenceService.IzatDwellNotify dwellNotify =
 *                            new IZatGeofenceService.IzatDwellNotify (3,
 *                                IZatGeofenceService.IzatDwellNotify.DWELL_TYPE_INSIDE_MASK |
 *                                IZatGeofenceService.IzatDwellNotify.DWELL_TYPE_OUTSIDE_MASK);
 *
 *                        // set dwell time
 *                        geofence.setDwellNotify(dwellNotify);
 *
 *                        // create a geofence name
 *                        String geofenceName = "geofence sample";
 *
 *                        // add a geofence
 *                        mHandle = gfSevice.addGeofence(geofenceName, geofence);
 *                }
 *           });
 *        }
 *    }
 *
 *    public class GeofenceCallback implements IZatGeofenceService.IZatGeofenceCallback {
 *        private static String TAG = "Breach Callback Sample";
 *
 *        public void onTransitionEvent(IZatGeofenceService.IZatGeofenceHandle handle,
 *                                      int event,
 *                                      Location location) {
 *
 *            Log.v(TAG, "got breach event in app, handle.obj is "+ handle.getContext());
 *            if (location != null) {
 *                Log.v(TAG, "The breach location is " +location.getLatitude()+
 *                           "/"+location.getLongitude());
 *            }
 *        }
 *
 *        public void onRequestFailed(IZatGeofenceService.IZatGeofenceHandle handle,
 *                                    int requestType,
 *                                    int errorCode) {
 *
 *            Log.v(TAG, "got onRequestFailed in app, handle.obj is "+ handle.getContext() +
 *                       "; requestType is " + requestType + "; errorCode is " +errorCode);
 *        }
 *
 *        public void onEngineReportStatus(int status,
 *                                         Location location) {
 *
 *            Log.v(TAG, "got geofence status in app, status is "+status);
 *            if (location != null) {
 *               Log.v(TAG, "The last location is " +location.getLatitude()+
 *                          "/"+location.getLongitude());
 *            }
 *        }
 *    }
 * </code>
 * </pre>
 * @version 1.0.0
 */

public interface IZatGeofenceService {

    public static final int GEOFENCE_RESULT_SUCCESS = 0;
    public static final int GEOFENCE_RESULT_ERROR_TOO_MANY_GEOFENCES = -100;
    public static final int GEOFENCE_RESULT_ERROR_ID_UNKNOWN = -102;
    public static final int GEOFENCE_RESULT_ERROR_INVALID_TRANSITION = -103;
    public static final int GEOFENCE_RESULT_ERROR_GENERIC = -149;

    public static final int GEOFENCE_REQUEST_TYPE_ADD = 1;
    public static final int GEOFENCE_REQUEST_TYPE_REMOVE = 2;
    public static final int GEOFENCE_REQUEST_TYPE_PAUSE = 3;
    public static final int GEOFENCE_REQUEST_TYPE_RESUME = 4;
    public static final int GEOFENCE_REQUEST_TYPE_UPDATE = 5;

    public static final int GEOFENCE_EVENT_ENTERED = 1;
    public static final int GEOFENCE_EVENT_EXITED = 2;
    public static final int GEOFENCE_EVENT_UNCERTAIN = 4;
    public static final int GEOFENCE_EVENT_DWELL_INSIDE = 8;
    public static final int GEOFENCE_EVENT_DWELL_OUTSIDE = 16;

    //GNSS is unavailable and GNSS position fixes cannot be used to monitor Geofence
    public static final int GEOFENCE_GEN_ALERT_GNSS_UNAVAILABLE = 1;
    //GNSS is now available and GNSS position fixes can be used to monitor Geofence
    public static final int GEOFENCE_GEN_ALERT_GNSS_AVAILABLE = 2;
    //The engine is out of service and no cell ID coverage information is available
    public static final int GEOFENCE_GEN_ALERT_OOS = 3;
    //The engine has an invalid time
    public static final int GEOFENCE_GEN_ALERT_TIME_INVALID = 4;

    /**
     * Register geofence callback.
     * <p>
     * This allows applications to get geofence callbacks. If there
     * is already a callback registered, then the new callback will
     * override the existing one.</p>
     *
     * @param  callback the callback to receive geofence callbacks.
     *         This parameter can not be null, otherwise a
     *         {@link IZatIllegalArgumentException} will be thrown.
     * @throws IZatIllegalArgumentException
     */
    void registerForGeofenceCallbacks(IZatGeofenceCallback statusCb)
                                      throws IZatIllegalArgumentException;

    /**
     * De-register geofence callback.
     * <p>
     * This allows applications to stop getting geofence callbacks.</p>
     *
     * @param  callback the callback to receive geofence callbacks.
     *         This parameter can not be null, otherwise a
     *         {@link IZatIllegalArgumentException} will be thrown.
     * @throws IZatIllegalArgumentException
     */
    void deregisterForGeofenceCallbacks(IZatGeofenceCallback statusCb)
                                        throws IZatIllegalArgumentException;

    /**
     * Add a geofence.
     * <p>
     * This allows applications to add a geofence and start monitor
     * the transition. When the device enters or exits the geofence
     * circle, the application will get alerts through the callback.
     * When the geofence is failed to be added, the application will
     * also get the failed error through the callback registered. </p>
     *
     * @param  context the object used to identify the geofence, which could
     *         be an string as the name of the geofence, an integer as the id
     *         of the geofence and so on. This parameter can be null.
     * @param  geofence the geofence request.
     *         This parameter can not be null, otherwise a
     *         {@link IZatIllegalArgumentException} will be thrown.
     * @throws IZatIllegalArgumentException
     * @return the handle binding with the geofence.
     */
    IZatGeofenceHandle addGeofence(Object context,
                                   IzatGeofence geofence)
                                   throws IZatIllegalArgumentException;

    /**
     * Remove a geofence.
     * <p>
     * This allows applications to remove a geofence. When the geofence
     * is failed to be removed, the application will also get the failed
     * error through the callback registered. </p>
     *
     * @param  handler the geofence handler.
     *         This parameter can not be null, otherwise a
     *         {@link IZatIllegalArgumentException} will be thrown.
     * @throws IZatIllegalArgumentException
     */
    void removeGeofence(IZatGeofenceHandle handler)
                        throws IZatIllegalArgumentException;

    /**
     * Interface class IZatGeofenceHandle.
     * <p>Copyright (c) 2015 Qualcomm Technologies, Inc.</p>
     * <p>All Rights Reserved.</p>
     * <p>Confidential and Proprietary - Qualcomm Technologies, Inc</p>
     * <br/>
     * <p><b>IZatGeofenceHandle</b> is the interface used to manipulate the geofence added.</p>
     * <pre>
     * <code>
     *    // Sample Code
     *
     *    import android.app.Activity;
     *    import android.os.Bundle;
     *    import android.util.Log;
     *    import android.view.View;
     *    import android.widget.Button;
     *    import android.widget.TextView;
     *    import com.qti.location.sdk.IZatManager;
     *    import com.qti.location.sdk.IZatGeofenceService;
     *
     *    public class FullscreenActivity extends Activity {
     *        ......
     *        private IZatGeofenceService.IZatGeofenceHandle mHandle = null;
     *        final GeofenceCallback geofenceCb = new GeofenceCallback();
     *        final Button geofenceBtn = (Button)findViewById(R.id.geofenceBtn);
     *
     *        @Override
     *        protected void onCreate(Bundle savedInstanceState) {
     *            ......
     *            geofenceBtn.setOnClickListener(new View.OnClickListener() {
     *                @Override
     *                public void onClick(View v) {
     *                        // register callback
     *                        gfSevice.registerForGeofenceCallbacks(geofenceCb);
     *
     *                        // set geofence circle
     *                        IZatGeofenceService.IzatGeofence geofence =
     *                            new IZatGeofenceService.IzatGeofence(37.375507,
     *                                                                 -121.983643,
     *                                                                 5000);
     *                        // add a geofence
     *                        mHandle = gfSevice.addGeofence(geofenceName, geofence);
     *
     *                        // update the geofence through handle
     *                        mHandle.update(
     *                            IZatGeofenceService.IzatGeofenceTransitionTypes.EXITED_ONLY,
     *                            897600);
     *
     *                        // pause the geofence through handle
     *                        mHandle.pause();
     *                }
     *           });
     *        }
     *    }
     *
     *    public class GeofenceCallback implements IZatGeofenceService.IZatGeofenceCallback {
     *
     *        public void onTransitionEvent(IZatGeofenceService.IZatGeofenceHandle handle,
     *                                      int event,
     *                                      Location location) {
     *            ......
     *        }
     *
     *        public void onRequestFailed(IZatGeofenceService.IZatGeofenceHandle handle,
     *                                    int requestType,
     *                                    int errorCode) {
     *            ......
     *        }
     *
     *        public void onEngineReportStatus(int status,
     *                                         Location location) {
     *            ......
     *        }
     *    }
     * </code>
     * </pre>
     */
    interface IZatGeofenceHandle {
        /**
         * Retrieve the context associated with the geofence.
         * <p>
         * This allows applications to get the context object
         * associated with the geofence. </p>
         *
         * @return context associated the geofence.
         */
        Object getContext();

        /**
         * Update the geofence.
         * <p>
         * This allows applications to update the transition types and
         * responsiveness of an existing geofence.</p>
         * <p> If the geofence associated with this handle has been removed,
         * the handle will no longer be valid, so there will be no geofence
         * been added or updated. When the geofence is failed to be updated,
         * the application will get the failed error through the callback
         * registered.</p>
         *
         * @param  transitionTypes the new geofence transition types.
         *         This parameter can not be null, otherwise a
         *         {@link IZatIllegalArgumentException} will be thrown.
         * @throws IZatIllegalArgumentException
         *
         * @param  notifyResponsiveness the new geofence responsiveness in seconds.
         *         This parameter can not be less or equal to zero, otherwise a
         *         {@link IZatIllegalArgumentException} will be thrown.
         * @throws IZatIllegalArgumentException
         */
        void update(IzatGeofenceTransitionTypes transitionTypes,
                    int notifyResponsiveness)
                    throws IZatIllegalArgumentException;

        /**
         * Pause the geofence.
         * <p>
         * This allows applications to pause an existing geofence.</p>
         * <p> If the geofence associated with this handle has been removed,
         * the handle will no longer be valid, so there will be no geofence
         * been added or paused. Once a geofence is paused successfully, when
         * the device enters or exits the geofence circle, the application will
         * not get alert. When the geofence is failed to be paused, the application
         * will get the failed error through the callback registered.</p>
         */
        void pause();

        /**
         * Resume the paused geofence.
         * <p>
         * This allows applications to resume a paused geofence with new
         * transition type.</p>
         * <p> If the geofence associated with this handle has been removed,
         * the handle will no longer be valid, so there will be no geofence
         * been added or resumed. Once a geofence is resumed successfully,
         * the application will continue to get alert when the device enters
         * or exits the geofence circle. When the geofence is failed to be
         * resumed, the application will get the failed error through the
         * callback registered.</p>
         *
         * @param  transitionTypes the new geofence transition types.
         *         This parameter can not be null, otherwise a
         *         {@link IZatIllegalArgumentException} will be thrown.
         * @throws IZatIllegalArgumentException
         */
        void resume(IzatGeofenceTransitionTypes transitionTypes)
                    throws IZatIllegalArgumentException;
    }

    /**
     * Interface class IZatGeofenceCallback.
     * <p>Copyright (c) 2015 Qualcomm Technologies, Inc.</p>
     * <p>All Rights Reserved.</p>
     * <p>Confidential and Proprietary - Qualcomm Technologies, Inc</p>
     * <br/>
     * <p><b>IZatGeofenceCallback</b> is the geofence callback interface.</p>
     * <pre>
     * <code>
     *    // Sample Code
     *
     *    public class GeofenceCallback implements IZatGeofenceService.IZatGeofenceCallback {
     *
     *        private static String TAG = "geofence callback sample";
     *        public void onTransitionEvent(IZatGeofenceService.IZatGeofenceHandle handle,
     *                                      int event,
     *                                      Location location) {
     *
     *            Log.v(TAG, "got breach event in app, handle.obj is "+ handle.getContext());
     *
     *            if (location != null) {
     *               Log.v(TAG, "The breach location is " +location.getLatitude()+
     *                          "/"+location.getLongitude());
     *            } else {
     *               Log.v(TAG, "The breach location is Null");
     *            }
     *        }
     *
     *        public void onRequestFailed(IZatGeofenceService.IZatGeofenceHandle handle,
     *                                    int requestType,
     *                                    int errorCode) {
     *
     *            Log.v(TAG, "got onRequestFailed in app, handle.obj is "+
     *                       handle.getContext() + "; requestType is " +
     *                       requestType + "; errorCode is " +errorCode);
     *        }
     *
     *        public void onEngineReportStatus(int status,
     *                                         Location location) {
     *
     *            Log.v(TAG, "got geofence status in app, status is "+status);
     *
     *            if (location != null) {
     *               Log.v(TAG, "The last location is " +location.getLatitude()+
     *                          "/"+location.getLongitude());
     *            } else {
     *               Log.v(TAG, "The last location is Null");
     *            }
     *        }
     *    }
     * </code>
     * </pre>
     */
    interface IZatGeofenceCallback {
        /**
         * Transition event callback.
         * <p>
         * This API will be called by the underlying service back
         * to applications when the device enters or exits the geofence
         * circle, or the dwell time is up. Applications should implement
         * this interface.</p>
         *
         * @param  handle the handle of the geofence.
         * @param  event indicating the device is enter or exits.
         *         It might be :
         *         {@link #GEOFENCE_EVENT_ENTERED},
         *         {@link #GEOFENCE_EVENT_EXITED},
         *         {@link #GEOFENCE_EVENT_UNCERTAIN},
         *         {@link #GEOFENCE_EVENT_DWELL_INSIDE},
         *         {@link #GEOFENCE_EVENT_DWELL_OUTSIDE}
         * @param  location the location where the device enters or
         *         exits the geofence circle, or the location where
         *         dwell time is up.
         */
        void onTransitionEvent(IZatGeofenceHandle handle,
                               int event,
                               Location location);
        /**
         * Request failure callback.
         * <p>
         * This API will be called by the underlying service back
         * to applications when add/remove/update/pause/resume geofence
         * request is failed. Applications should implement this interface.</p>
         *
         * @param  handle the handle of the geofence.
         * @param  requestType indicating which request is failed.
         *         It might be :
         *         {@link #GEOFENCE_REQUEST_TYPE_ADD},
         *         {@link #GEOFENCE_REQUEST_TYPE_REMOVE},
         *         {@link #GEOFENCE_REQUEST_TYPE_UPDATE},
         *         {@link #GEOFENCE_REQUEST_TYPE_RESUME},
         *         {@link #GEOFENCE_REQUEST_TYPE_PAUSE}
         * @param  errorCode the failure reason.
         *         It might be :
         *         {@link #GEOFENCE_RESULT_ERROR_TOO_MANY_GEOFENCES},
         *         {@link #GEOFENCE_RESULT_ERROR_ID_UNKNOWN},
         *         {@link #GEOFENCE_RESULT_ERROR_INVALID_TRANSITION},
         *         {@link #GEOFENCE_RESULT_ERROR_GENERIC},
         */
        void onRequestFailed(IZatGeofenceHandle handle,
                             int requestType,
                             int errorCode);
        /**
         * Engine status callback.
         * <p>
         * This API will be called by the underlying service back
         * to applications when the geofence engine reports its status.</p>
         *
         * @param  status indicating the geofence engine status.
         *         It might be :
         *         {@link #GEOFENCE_GEN_ALERT_GNSS_UNAVAILABLE},
         *         {@link #GEOFENCE_GEN_ALERT_GNSS_AVAILABLE},
         *         {@link #GEOFENCE_GEN_ALERT_OOS},
         *         {@link #GEOFENCE_GEN_ALERT_TIME_INVALID}
         * @param  location the last location engine generated.
         *         It could be null.
         */
        void onEngineReportStatus(int status,
                                  Location location);
    }

    /**
     * class IzatGeofence.
     * <p>Copyright (c) 2015 Qualcomm Technologies, Inc.</p>
     * <p>All Rights Reserved.</p>
     * <p>Confidential and Proprietary - Qualcomm Technologies, Inc</p>
     */
    public class IzatGeofence {
        private int mNotifyResponsiveness;
        private double mLatitude;
        private double mLongitude;
        private double mRadius;
        private IzatGeofenceTransitionTypes mTransitionTypes;
        private IzatGeofenceConfidence mConfidence;
        private IzatDwellNotify mDwellNotify;

        /**
         * Create the geofence area.
         * <p>
         * This creates a geofence of circular region on horizontal plane. </p>
         * @param  latitude indicating the latitude in degrees, between -90 and +90 inclusive.
         * @param  longitude indicating the latitude in degrees, between -90 and +90 inclusive.
         * @param  radius indicating the radius in meters.
         */
        public IzatGeofence(double latitude,
                            double longitude,
                            double radius) {
            mLatitude = latitude;
            mLongitude = longitude;
            mRadius = radius;
            mNotifyResponsiveness = 0;
            mTransitionTypes = IzatGeofenceTransitionTypes.UNKNOWN;
            mConfidence = IzatGeofenceConfidence.LOW;
        }
        double getLatitude() {
            return mLatitude;
        }
        double getLongitude() {
            return mLongitude;
        }
        double getRadius() {
            return mRadius;
        }
        /**
         * Set the transition type.
         * <p>
         * This set the geofence transition type monitored, based on which
         * the alert is generated.</p>
         *
         * @param  transitionTypes indicating which transition will generate
         *         the alert.
         */
        public void setTransitionTypes(IzatGeofenceTransitionTypes transitionTypes) {
            mTransitionTypes = transitionTypes;
        }
        /**
         * Get the transition type setting.
         *
         * @return the current transition type of the geofence.
         */
        public IzatGeofenceTransitionTypes getTransitionTypes() {
            return mTransitionTypes;
        }
        /**
         * Set the responsiveness.
         * <p>
         * This set the geofence responsiveness. Defaults to 0.
         * Smaller repressiveness means device could get the alert
         * more quicker after the user enters or exits the geofence
         * area. Larger repressiveness means there could be a delay
         * for the alert, but larger repressiveness can help to save power.
         *
         * @param  notifyResponsiveness the responsiveness in milliseconds.
         */
        public void setNotifyResponsiveness(int notifyResponsiveness) {
            mNotifyResponsiveness = notifyResponsiveness;
        }
        /**
         * Get the responsiveness setting.
         *
         * @return the current responsiveness of the geofence.
         */
        public int getNotifyResponsiveness() {
            return mNotifyResponsiveness;
        }
        /**
         * Set the confidence.
         * <p>
         * This set the geofence confidence, which determines the probability
         * that the alert happened at the Geofence boundary. Higher confidence
         * could cost more power consumption.
         *
         * @param  confidence the geofence confidence.
         */
        public void setConfidence(IzatGeofenceConfidence confidence) {
            mConfidence = confidence;
        }
        /**
         * Get the confidence setting.
         *
         * @return the current confidence of the geofence.
         */
        public IzatGeofenceConfidence getConfidence() {
            return mConfidence;
        }
        /**
         * Set the dwell notify.
         * <p>
         * This set the geofence dwell notify.
         *
         * @param  dwellNotify the geofence dwell notify.
         */
        public void setDwellNotify(IzatDwellNotify dwellNotify) {
            mDwellNotify = dwellNotify;
        }
        /**
         * Get the dwell notify setting.
         *
         * @return the current dwell notify of the geofence.
         */
        public IzatDwellNotify getDwellNotify() {
            return mDwellNotify;
        }
    }

    public enum IzatGeofenceTransitionTypes {
        UNKNOWN(0), ENTERED_ONLY(1), EXITED_ONLY(2), ENTERED_AND_EXITED(3);

        private final int value;
        private IzatGeofenceTransitionTypes(int value) {
            this.value = value;
        }
        public int getValue() {
            return value;
        }
    }

    public enum IzatGeofenceConfidence {
        LOW(1), MEDIUM(2), HIGH(3);

        private final int value;
        private IzatGeofenceConfidence(int value) {
            this.value = value;
        }
        public int getValue() {
            return value;
        }
    }

    /**
     * class IzatDwellNotify.
     * <p>Copyright (c) 2015 Qualcomm Technologies, Inc.</p>
     * <p>All Rights Reserved.</p>
     * <p>Confidential and Proprietary - Qualcomm Technologies, Inc</p>
     */
    public class IzatDwellNotify {
        public static final int DWELL_TYPE_INSIDE_MASK = 1;
        public static final int DWELL_TYPE_OUTSIDE_MASK = 2;
        int mDwellTime;
        int mDwellType;
        /**
         * Create the dwell notify.
         * <p>
         * This creates a geofence of dwell notify. </p>
         * @param  dwellTime indicating the time in seconds a
         *         user spends in the geofence area before a dwell
         *         event is sent.
         * @param  dwellType indicating which area the application is
         *         interested in.
         *         It might be (one or two):
         *         {@link #DWELL_TYPE_INSIDE_MASK},
         *         {@link #DWELL_TYPE_OUTSIDE_MASK}
         */
        public IzatDwellNotify(int dwellTime, int dwellType) {
            mDwellTime = dwellTime;
            mDwellType = dwellType;
        }
        /**
         * Get the dwell time.
         *
         * @return the current dwell time of the dwell notify.
         */
        public int getDwellTime() {
            return mDwellTime;
        }
        /**
         * Get the dwell type.
         *
         * @return the current dwell type of the dwell notify.
         */
        public int getDwellType() {
            return mDwellType;
        }
    }
}
