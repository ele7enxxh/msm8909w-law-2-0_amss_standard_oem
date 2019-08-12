/*==============================================================================

FILE:        rpm_hash.h

DESCRIPTION: Hashing functions for RPM resource state

      Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
         QUALCOMM Proprietary and Confidential

$Header: //components/rel/rpm.bf/2.1.1/core/power/rpm/server/rpm_hash.h#1 $
$Author: pwbldsvc $
$DateTime: 2015/02/25 02:53:43 $

==============================================================================*/

#ifndef RPM_HASH_H
#define RPM_HASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "rpm.h"
#include "rpm_config.h"

extern uint32_t rpm_system_hash;

uint32_t rpm_hash(uint32_t length, const void *data);

//
// Unrolled variants
//
#define rpm_hash64(x,y) rpm_hash64s(0,(x),(y))
uint32_t rpm_hash64s(uint32_t seed, uint32_t val1, uint32_t val2) __pure;

uint32_t rpm_resource_hash(uint32_t hash, ResourceData *r, uint32_t length, const void *oldValue, const void *newValue);

#ifdef __cplusplus
};
#endif // __cplusplus

#endif // RPM_HASH_H

