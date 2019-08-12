/**
 * @file
 * Driver ID definition.
 *
 * Copyright (c) 2013 TRUSTONIC LIMITED
 * All rights reserved
 * 
 * The present software is the confidential and proprietary information of
 * TRUSTONIC LIMITED. You shall not disclose the present software and shall
 * use it only in accordance with the terms of the license agreement you
 * entered into with TRUSTONIC LIMITED. This software may be subject to
 * export or import laws in certain countries.
 */

#ifndef RTMDRVID_H_
#define RTMDRVID_H_

#define MC_DRV_VENDOR_ID_SHIFT     (16)
#define MC_DRV_VENDOR_ID_MASK      (0xFFFF << MC_DRV_VENDOR_ID_SHIFT)
#define MC_DRV_NUMBER_MASK         (0x0000FFFF)

/** MobiCore vendor IDs. */
typedef enum {
    MC_DRV_VENDOR_ID_GD   = 0 << MC_DRV_VENDOR_ID_SHIFT,
} mcDrvVendorId_t;

/** MobiCore GD driver numbers. */
typedef enum {
	MC_DRV_NUMBER_INVALID = 0,
	MC_DRV_NUMBER_CRYPTO  = 1,
    MC_DRV_NUMBER_KEYPAD  = 2,
    /** Last GD driver number reserved for pre-installed drivers. 
     * GD driver numbers up to this constant may not be used for loadable drivers. */
    MC_DRV_NUMBER_LAST_PRE_INSTALLED = 100,
} mcDrvNumber_t;

/** MobiCore driver IDs for Trustlets. */
typedef enum {
	MC_DRV_ID_INVALID = MC_DRV_VENDOR_ID_GD | MC_DRV_NUMBER_INVALID,
	MC_DRV_ID_CRYPTO  = MC_DRV_VENDOR_ID_GD | MC_DRV_NUMBER_CRYPTO,
    MC_DRV_ID_KEYPAD  = MC_DRV_VENDOR_ID_GD | MC_DRV_NUMBER_KEYPAD,
    /** Last GD driver ID reserved for pre-installed drivers. 
     * GD driver IDs up to this constant may not be used for loadable drivers. */
    MC_DRV_ID_LAST_PRE_INSTALLED = MC_DRV_VENDOR_ID_GD | MC_DRV_NUMBER_LAST_PRE_INSTALLED,
} mcDriverId_t;

#endif /* RTMDRVID_H_ */
