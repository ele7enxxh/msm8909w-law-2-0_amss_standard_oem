/**
 * @addtogroup MC_ROOTID mcRootid - Root container id.
 *
 * Global definition of root ID.
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

#ifndef MC_ROOTID_H_
#define MC_ROOTID_H_

/** Root Identifier type. */
typedef uint32_t mcRootid_t;

/** Reserved root id value 1. */
static const mcRootid_t MC_ROOTID_RESERVED1 = 0;

/** Reserved root id value 2. */
static const mcRootid_t MC_ROOTID_RESERVED2 = 0xFFFFFFFF;

/** Root id for system applications. */
static const mcRootid_t MC_ROOTID_SYSTEM = 0xFFFFFFFE;

#endif // MC_ROOTID_H_

/** @} */

