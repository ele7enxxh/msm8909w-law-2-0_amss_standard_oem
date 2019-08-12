/* sensor_serviceutil.h
 *
 * Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#ifndef __SENSOR_SERVICEUTIL_H__
#define __SENSOR_SERVICEUTIL_H__
#ifdef _ANDROID_
#include <android/sensor.h>
#ifdef __cplusplus
extern "C" {
#endif

ASensorManager *sensor_service_get_object(void);
void sensor_service_destroy(ASensorManager *p);

#ifdef __cplusplus
}
#endif
#endif /* _ANDROID_ */
#endif /* __SENSOR_SERVICEUTIL_H__ */
