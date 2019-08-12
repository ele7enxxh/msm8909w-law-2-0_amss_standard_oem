/* ======================================================================
*  Copyright (c) 2015 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*  ====================================================================*/

package com.qti.location.sdk;

import android.content.Context;
import android.location.Location;

/**
 * Class IZatTestService. (FOR INTERNAL USE ONLY)
 *
 * @hide
 */
public interface IZatTestService {
    /* Aiding data flags */
    static final long GNSS_AIDING_DATA_POSITION             = (1L << 1);
    static final long GNSS_AIDING_DATA_TIME                 = (1L << 2);
    static final long GNSS_AIDING_DATA_IONO                 = (1L << 3);
    static final long GNSS_AIDING_DATA_UTC                  = (1L << 4);
    static final long GNSS_AIDING_DATA_HEALTH               = (1L << 5);
    static final long GNSS_AIDING_DATA_SADATA               = (1L << 6);
    static final long GNSS_AIDING_DATA_RTI                  = (1L << 7);
    static final long GNSS_AIDING_DATA_FREQ_BIAS_EST        = (1L << 8);
    static final long GNSS_AIDING_DATA_CELLDB_INFO          = (1L << 9);
    // GPS
    static final long GNSS_AIDING_DATA_GPS_EPHEMERIS        = (1L << 10);
    static final long GNSS_AIDING_DATA_GPS_ALMANAC          = (1L << 11);
    static final long GNSS_AIDING_DATA_GPS_SVDIR            = (1L << 12);
    static final long GNSS_AIDING_DATA_GPS_SVSTEER          = (1L << 13);
    static final long GNSS_AIDING_DATA_GPS_TIME             = (1L << 14);
    static final long GNSS_AIDING_DATA_GPS_ALMANAC_CORR     = (1L << 15);
    // GLONASS
    static final long GNSS_AIDING_DATA_GLO_EPHEMERIS        = (1L << 16);
    static final long GNSS_AIDING_DATA_GLO_ALMANAC          = (1L << 17);
    static final long GNSS_AIDING_DATA_GLO_SVDIR            = (1L << 18);
    static final long GNSS_AIDING_DATA_GLO_SVSTEER          = (1L << 19);
    static final long GNSS_AIDING_DATA_GLO_TIME             = (1L << 20);
    static final long GNSS_AIDING_DATA_GLO_ALMANAC_CORR     = (1L << 21);
    // BeiDou
    static final long GNSS_AIDING_DATA_BDS_EPHEMERIS        = (1L << 22);
    static final long GNSS_AIDING_DATA_BDS_ALMANAC          = (1L << 23);
    static final long GNSS_AIDING_DATA_BDS_SVDIR            = (1L << 24);
    static final long GNSS_AIDING_DATA_BDS_SVSTEER          = (1L << 25);
    static final long GNSS_AIDING_DATA_BDS_TIME             = (1L << 26);
    static final long GNSS_AIDING_DATA_BDS_ALMANAC_CORR     = (1L << 27);

    static final long GNSS_AIDING_DATA_ALL                  = ~0L;

    /**
     * Delete GNSS aiding data.
     * <p>
     * This allows to delete any aiding data obtained by GNSS providers.
     * </p>
     *
     * @param  Mask of aiding data elements to delete. This parameter
     *         can not be 0, otherwise a
     *         {@link IZatIllegalArgumentException} will be thrown.
     *         It might be (one or more):
     *         {@link #GNSS_AIDING_DATA_POSITION},
     *         {@link #GNSS_AIDING_DATA_TIME},
     *         {@link #GNSS_AIDING_DATA_IONO},
     *         {@link #GNSS_AIDING_DATA_UTC},
     *         {@link #GNSS_AIDING_DATA_HEALTH},
     *         {@link #GNSS_AIDING_DATA_SADATA},
     *         {@link #GNSS_AIDING_DATA_RTI},
     *         {@link #GNSS_AIDING_DATA_FREQ_BIAS_EST},
     *         {@link #GNSS_AIDING_DATA_CELLDB_INFO},
     *         {@link #GNSS_AIDING_DATA_GPS_EPHEMERIS},
     *         {@link #GNSS_AIDING_DATA_GPS_ALMANAC},
     *         {@link #GNSS_AIDING_DATA_GPS_SVDIR},
     *         {@link #GNSS_AIDING_DATA_GPS_SVSTEER},
     *         {@link #GNSS_AIDING_DATA_GPS_TIME},
     *         {@link #GNSS_AIDING_DATA_GPS_ALMANAC_CORR},
     *         {@link #GNSS_AIDING_DATA_GLO_EPHEMERIS},
     *         {@link #GNSS_AIDING_DATA_GLO_ALMANAC},
     *         {@link #GNSS_AIDING_DATA_GLO_SVDIR},
     *         {@link #GNSS_AIDING_DATA_GLO_SVSTEER},
     *         {@link #GNSS_AIDING_DATA_GLO_TIME},
     *         {@link #GNSS_AIDING_DATA_GLO_ALMANAC_CORR},
     *         {@link #GNSS_AIDING_DATA_BDS_EPHEMERIS},
     *         {@link #GNSS_AIDING_DATA_BDS_ALMANAC},
     *         {@link #GNSS_AIDING_DATA_BDS_SVDIR},
     *         {@link #GNSS_AIDING_DATA_BDS_SVSTEER},
     *         {@link #GNSS_AIDING_DATA_BDS_TIME},
     *         {@link #GNSS_AIDING_DATA_BDS_ALMANAC_CORR},
     *         {@link #GNSS_AIDING_DATA_ALL}
     *
     * @throws IZatIllegalArgumentException
     */
    void deleteAidingData(long flags)
                          throws IZatIllegalArgumentException;

    /**
     * Start to listen for changes on the maximum power allocated by FLP
     * <p>
     * This allows applications to stay up to date with maximum power
     * allocated by FLP.
     * Applications will be notified about the power change via callback
     * for a foreground or background session.
     * If the registration success, callback will be called with current
     * maximum power allocated by FLP</p>
     *
     * @param  callback the callback to receive power changes.
     *         This parameter can not be null, otherwise a
     *         {@link IZatIllegalArgumentException} will be thrown.
     * @throws IZatIllegalArgumentException
     */
    void registerForMaxPowerAllocatedChange(IFlpMaxPowerAllocatedCallback callback)
                                            throws IZatIllegalArgumentException;

    /**
     * Stop listening for changes of the maximum power allocated by FLP
     * <p>
     * This allows applications to stop listening for changes on the max
     * power allocated by FLP
     * If the callback is not registered, then this function
     * will just return. </p>
     *
     * @param  callback the callback used to register for receiving
     *         changes on the maximum power allocated by FLP.
     *         This parameter can not be null, otherwise a
     *         {@link IZatIllegalArgumentException} will be thrown.
     * @throws IZatIllegalArgumentException
     */
    void deregisterForMaxPowerAllocatedChange(IFlpMaxPowerAllocatedCallback callback)
                                              throws IZatIllegalArgumentException;

    /**
     * Interface class IFlpMaxPowerAllocatedCallback.
     * <p>Copyright (c) 2015 Qualcomm Technologies, Inc.</p>
     * <p>All Rights Reserved.</p>
     * <p>Confidential and Proprietary - Qualcomm Technologies, Inc</p>
     */
    interface IFlpMaxPowerAllocatedCallback {
        /**
         * Max power allocated callback.
         * <p>
         * This API will be called by the underlying service back
         * to applications when maximum allocated power changes.
         * Applications should implement this interface.</p>
         *
         * @param  power_mW Maximum power allocated in mW
         */
        void onMaxPowerAllocatedChanged(double power_mW);
    }
}
