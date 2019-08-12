/*  Copyright (c) 2015 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

package com.qualcomm.location.izat.flp;

import java.com.qualcomm.location.izat.flp.ILocationCallback;

interface IFlpService {

    void registerCallback(in int sessionType,
                          in int id,
                          in ILocationCallback cb,
                          in long sessionStartTime);

    void unregisterCallback(in int sessionType,
                            in ILocationCallback cb);

    int getAllSupportedFeatures();

    int startFlpSession(in int id,
                        in int flags,
                        in long period_ms,
                        in int distance_ms);

    int updateFlpSession(in int id,
                         in int flags,
                         in long period_ms,
                         in int distance_ms);

    int stopFlpSession(in int id);

    int pullLocations(in ILocationCallback cb,
                      in long sessionStartTime);
}
