#ifndef _HAP_POWER_H
#define _HAP_POWER_H

/*==============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Requests a performance level by percentage for clock speed 
 * and bus speed.  Passing 0 for any parameter results in no 
 * request being issued for that particular attribute. 
 * @param clock - percentage of target's maximum clock speed
 * @param bus - percentage of target's maximum bus speed
 * @param latency - maximum hardware wakeup latency in microseconds.  The
 *                  higher the value the deeper state of sleep
 *                  that can be entered but the longer it may
 *                  take to awaken.
 * @retval 0 on success
 * @par Comments: Performance metrics vary from target to target so the
 *                intent of this API is to allow callers to set a relative
 *                performance level to achive the desired balance between
 *                perforamnce and power saving.
 */
int HAP_power_request(int clock, int bus, int latency);

/**
 * Requests a performance level by absolute values.  Passing 0 
 * for any parameter results in no request being issued for that
 * particular attribute. 
 * @param clock - speed in MHz
 * @param bus - bus speed in MHz
 * @param latency - maximum hardware wakeup latency in microseconds.  The
 *                  higher the value the deeper state of
 *                  sleep that can be entered but the
 *                  longer it may take to awaken.
 * @retval 0 on success
 * @par Comments: This API allows callers who are aware of their target
 *                specific capabilities to set them explicitly.
 */
int HAP_power_request_abs(int clock, int bus, int latency);

/**
 * queries the target for its clock and bus speed capabilities
 * @param clock_max - maximum clock speed supported in MHz
 * @param bus_max - maximum bus speed supported in MHz
 * @retval 0 on success
 */
int HAP_power_get_max_speed(int* clock_max, int* bus_max);

#ifdef __cplusplus
}
#endif
#endif //_HAP_POWER_H

