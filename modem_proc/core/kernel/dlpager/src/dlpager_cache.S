/*===========================================================================
 * FILE:         dlpager_cache.S
 *
 * SERVICES:     DL PAGER
 *
 * DESCRIPTION:  Cache routines
 *
 * Copyright (c) 2014-2015 Qualcomm Technologies Incorporated.
 * All Rights Reserved. QUALCOMM Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/kernel/dlpager/src/dlpager_cache.S#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/01/15   ao      Initial revision
===========================================================================*/

#include "dlpager_constants.h"

  .text
  .p2align CACHE_LINE_SHIFT // align to cache line
  .globl dlpager_icache_clean_128
  .type dlpager_icache_clean_128, @function
dlpager_icache_clean_128:
    r0 = and(r0, #~(CACHE_LINE_SZ-1))
    r6 = r1
    r4 = #(CACHE_LINE_SZ*4)
    r5 = #(CACHE_LINE_SZ*4)
    r1 = add(r0, #CACHE_LINE_SZ)
    r2 = add(r0, #(CACHE_LINE_SZ * 2))
    r3 = add(r0, #(CACHE_LINE_SZ * 3))
    loop0(1f, r6)
    .falign
1:
    icinva(r0)
    icinva(r1)
    icinva(r2)
    icinva(r3)
    {
      r1:0 = vaddw(r1:0,R5:4)
      r3:2 = vaddw(r3:2,R5:4)
    }:endloop0
    jumpr r31
  .size dlpager_icache_clean_128, .-dlpager_icache_clean_128


  .p2align CACHE_LINE_SHIFT // align to cache line
  .globl dlpager_dczero
  .type dlpager_dczero, @function
dlpager_dczero:
    r3 = lsr(r1, #CACHE_LINE_SHIFT)
    r0 = and(r0, #~(CACHE_LINE_SZ-1))
    loop0(1f, r3)
    .falign
1:
    {
      dczeroa(r0)
      r0 = add(r0, #CACHE_LINE_SZ)
    }:endloop0
    jumpr r31
  .size dlpager_dczero, .-dlpager_dczero

