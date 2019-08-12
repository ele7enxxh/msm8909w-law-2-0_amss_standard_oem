/**
 * @addtogroup MC_SUID mcSuid - SoC unique ID.
 *
 * Copyright (c) 2013 TRUSTONIC LIMITED
 * All rights reserved
 * 
 * The present software is the confidential and proprietary information of
 * TRUSTONIC LIMITED. You shall not disclose the present software and shall
 * use it only in accordance with the terms of the license agreement you
 * entered into with TRUSTONIC LIMITED. This software may be subject to
 * export or import laws in certain countries.
 *
 * @ingroup  MC_DATA_TYPES
 * @{
 */

#ifndef MC_SUID_H_
#define MC_SUID_H_

/** Length of SUID. */
#define MC_SUID_LEN    16

/** Platform specific device identifier (serial number of the chip). */
typedef struct {
    uint8_t data[MC_SUID_LEN - sizeof(uint32_t)];
} suidData_t;

/** Soc unique identifier type. */
typedef struct {
    uint32_t    sipId;  /**< Silicon Provider ID to be set during build. */
    suidData_t  suidData;
} mcSuid_t;

#endif // MC_SUID_H_

/** @} */
