#ifndef NMRR_PRRR_H
#define NMRR_PRRR_H

/*===========================================================================

                    T r u s t z o n e   T a r g e t
                          H e a d e r  F i l e

DESCRIPTION
  Describe MMU-specific register setting that outlines memory types

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2011-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/cfg/mmu/nmrr_prrr.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/01/13   pre     Initial Revision
============================================================================*/

#include "tt_sd.h"

/*****************************************************************************
 *                             !!! CAUTION !!!                               *
 * Indices must match tt_tz.h or else memory type abstraction will not work. *
 *****************************************************************************/

#define PRRR_INDEX_0  SET_PRRR_MEM(0,     0,   PRR_TR_STRONGLY_ORDERED)
#define NMRR_INDEX_0 (SET_NMRR_OR (0,     NMRR_NON_CACHEABLE)      |  \
                      SET_NMRR_IR (0,     NMRR_NON_CACHEABLE))

#define PRRR_INDEX_1  SET_PRRR_MEM(1,     1,   PRR_TR_DEVICE)
#define NMRR_INDEX_1 (SET_NMRR_OR (1,     NMRR_NON_CACHEABLE)      |  \
                      SET_NMRR_IR (1,     NMRR_NON_CACHEABLE))

#define PRRR_INDEX_2  SET_PRRR_MEM(2,     1,   PRR_TR_NORMAL)
#define NMRR_INDEX_2 (SET_NMRR_OR (2,     NMRR_NON_CACHEABLE)      |  \
                      SET_NMRR_IR (2,     NMRR_NON_CACHEABLE))

#define PRRR_INDEX_3  SET_PRRR_MEM(3,     1,   PRR_TR_NORMAL)
#define NMRR_INDEX_3 (SET_NMRR_OR (3,     NMRR_WRITE_THROUGH_NWA)  |  \
                      SET_NMRR_IR (3,     NMRR_WRITE_THROUGH_NWA))

#define PRRR_INDEX_4  SET_PRRR_MEM(4,     1,   PRR_TR_NORMAL)
#define NMRR_INDEX_4 (SET_NMRR_OR (4,     NMRR_WRITE_THROUGH_NWA)  |  \
                      SET_NMRR_IR (4,     NMRR_WRITE_THROUGH_NWA))

#define PRRR_INDEX_5  SET_PRRR_MEM(5,     1,   PRR_TR_NORMAL)
#define NMRR_INDEX_5 (SET_NMRR_OR (5,     NMRR_WRITE_BACK_WA)      |  \
                      SET_NMRR_IR (5,     NMRR_WRITE_BACK_WA))

#define PRRR_INDEX_6  SET_PRRR_MEM(6,     1,   PRR_TR_NORMAL)
#define NMRR_INDEX_6 (SET_NMRR_OR (6,     NMRR_WRITE_BACK_NWA)     |  \
                      SET_NMRR_IR (6,     NMRR_WRITE_THROUGH_NWA))

#ifdef TZBSP_WITH_TBASE
#define PRRR_INDEX_7  SET_PRRR_MEM(7,     1,   PRR_TR_NORMAL)
#define NMRR_INDEX_7 (SET_NMRR_OR (7,     NMRR_WRITE_BACK_WA)      |  \
                      SET_NMRR_IR (7,     NMRR_WRITE_BACK_WA))
#else
#define PRRR_INDEX_7  SET_PRRR_MEM(7,     0,   PRR_TR_STRONGLY_ORDERED)
#define NMRR_INDEX_7 (SET_NMRR_OR (7,     NMRR_NON_CACHEABLE)      |  \
                      SET_NMRR_IR (7,     NMRR_NON_CACHEABLE))
#endif

#define PRRR (PRRR_INDEX_0 | PRRR_INDEX_1 | PRRR_INDEX_2 | PRRR_INDEX_3 | \
              PRRR_INDEX_4 | PRRR_INDEX_5 | PRRR_INDEX_6 | PRRR_INDEX_7 | \
              /*          Index  Shareable */                             \
              SET_PRRR_NS(0,     0)  |                                    \
              SET_PRRR_NS(1,     1)  |                                    \
              SET_PRRR_DS(0,     1)  |                                    \
              SET_PRRR_DS(1,     1))

#define NMRR (NMRR_INDEX_0 | NMRR_INDEX_1 | NMRR_INDEX_2 | NMRR_INDEX_3 |  \
              NMRR_INDEX_4 | NMRR_INDEX_5 | NMRR_INDEX_6 | NMRR_INDEX_7)


#endif
