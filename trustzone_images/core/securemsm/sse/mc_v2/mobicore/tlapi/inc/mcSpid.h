/**
 * @addtogroup MC_SPID mcSpid - service provider ID.
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

#ifndef MC_SPID_H_
#define MC_SPID_H_

/** Service provider Identifier type. */
typedef uint32_t mcSpid_t;

/** SPID value used as free marker in root containers. */
static const mcSpid_t MC_SPID_FREE = 0xFFFFFFFF;

/** Reserved SPID value. */
static const mcSpid_t MC_SPID_RESERVED = 0;

/** SPID for system applications. */
static const mcSpid_t MC_SPID_SYSTEM = 0xFFFFFFFE;

/** SPID reserved for tests only */
static const mcSpid_t MC_SPID_RESERVED_TEST = 0xFFFFFFFD;

#endif // MC_SPID_H_

/** @} */



