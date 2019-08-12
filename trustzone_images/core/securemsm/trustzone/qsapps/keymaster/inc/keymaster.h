/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_HARDWARE_KEYMASTER_H
#define ANDROID_HARDWARE_KEYMASTER_H

#include <stdint.h>

__BEGIN_DECLS

/**
 * The id of this module
 */
#define KEYSTORE_HARDWARE_MODULE_ID "keystore"

#define KEYSTORE_KEYMASTER "keymaster"

/**
 * Settings for "module_api_version" and "hal_api_version"
 * fields in the keymaster_module initialization.
 */
#define KEYMASTER_HEADER_VERSION 4

#define KEYMASTER_MODULE_API_VERSION_0_2 HARDWARE_MODULE_API_VERSION(0, 2)
#define KEYMASTER_DEVICE_API_VERSION_0_2 \
    HARDWARE_DEVICE_API_VERSION_2(0, 2, KEYMASTER_HEADER_VERSION)

#define KEYMASTER_MODULE_API_VERSION_0_3 HARDWARE_MODULE_API_VERSION(0, 3)
#define KEYMASTER_DEVICE_API_VERSION_0_3 \
    HARDWARE_DEVICE_API_VERSION_2(0, 3, KEYMASTER_HEADER_VERSION)

#define KEYMASTER_MODULE_API_VERSION_0_4 HARDWARE_MODULE_API_VERSION(0, 4)
#define KEYMASTER_DEVICE_API_VERSION_0_4 \
    HARDWARE_DEVICE_API_VERSION_2(0, 4, KEYMASTER_HEADER_VERSION)

__END_DECLS

#endif  // ANDROID_HARDWARE_KEYMASTER_H
