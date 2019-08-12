/*
* Copyright (c) 2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*
*/

#include <inttypes.h>

#define MAC_ADDR_LEN 6
#define MAC_OUI_LEN 3
#define ARGV_IFACE_INDEX 2
#define ARGV_CMDID_INDEX 3
#define UNUSED(x)      (void)(x)
#define MAX_WLAN_MAC_PKT_LENGTH 2304
#define MAX_SLEEP_DURATION 1200

int mac_addr_aton(u8 *mac_addr, const char *txt, size_t length);
int mac_addr_aton(u8 *mac_addr, const char *txt);
s8 char_to_hex(char c);

wifi_interface_handle wifi_get_iface_handle(wifi_handle handle, char *name);
extern wifi_hal_fn fn;
